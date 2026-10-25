# T06 — `treegen` geometry and facade — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: use `superpowers:subagent-driven-development`
> or `superpowers:executing-plans`. Steps use checkbox (`- [ ]`) syntax.

**Status:** done · **Branch:** `task/T06-treegen-geometry` · **Base:** `modernization @ 9c59d75`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** Turn a `Skeleton` into render-ready CPU data (`MeshData` trunk + leaf transforms) without any engine type, and
add the facade `generateTree` (rewrite → interpret → geometry) with exact sizing, deterministic leaves and no NaN/Inf.

**Approach:** One sequential pre-pass over the skeleton classifies every segment (usable / skipped) and lists the emitted
segments, which gives the exact vertex and index counts (`2·rd` vertices and `6·(rd-1)` indices per emitted segment).
The output arrays are then sized once and filled per emitted segment by independent workers (a segment owns a fixed
slice of both arrays), so the result cannot depend on the thread count. The ring math is copied from
`LindenmayerTreeMeshGenerator::computeRingPoint` operation by operation (same float order) so parity is bit-exact on one platform,
and proven against a frozen header-only copy of the legacy math.

## Files

- Create: `ESEngine/TreeGen/include/treegen/MeshData.h` — `MeshVertex`, `MeshData`.
- Create: `ESEngine/TreeGen/include/treegen/Geometry.h`, `src/Geometry.cpp` — `GeometryOptions`, `GeometryStats`, `TreeGeometry`, `buildGeometry`.
- Create: `ESEngine/TreeGen/include/treegen/TreeGenerator.h`, `src/TreeGenerator.cpp` — `GenerateOptions`, `TreeModel`, `generateTree` (includes every other public header, so it is the only one a consumer needs).
- Modify: `ESEngine/TreeGen/CMakeLists.txt` — link `Threads::Threads` (only that).
- Create: `tests/treegen/legacy_reference/LegacyGeometryReference.h` — frozen legacy ring/index/UV/normal math (glm only), deleted in T07.
- Create: `tests/treegen/geometry_tests.cpp` (counts, UV, normals, seam, indices, determinism, threads, leaves, robustness, NaN sweep), `geometry_parity_tests.cpp`, `geometry_facade_tests.cpp`, `geometry_header_tests.cpp` (only `#include <treegen/TreeGenerator.h>`).
- Modify: `docs/architecture/treegen-design.md` (additive: `GeometryStats`, `GeometryOptions::threads`, `GenerateOptions::threads`, throw behaviour), `docs/DECISIONS.md` (append D-015).

## Interfaces

- Consumes: `Skeleton`/`SkeletonSegment`/`isTip` (T05), `Rng` (`derive(3)` for leaves), `TreeParams` (`leafGrowthProb`, `leavesAngleDiversity`, `seed`), `rewrite`, `interpret`.
- Produces (design doc + additive fields marked +):

```cpp
struct MeshVertex { glm::vec3 position, normal; glm::vec2 uv; };            // 32 bytes, no padding (static_assert)
struct MeshData   { std::vector<MeshVertex> vertices; std::vector<uint32_t> indices; };
struct GeometryOptions { int ringDensity = 9; uint32_t seed = 0; unsigned threads = 0; /* + 0 auto, 1 never, n exactly n */ };
struct GeometryStats {                                                       // +
	std::size_t segments = 0, emittedSegments = 0, skippedZeroRadius = 0, skippedInvalidFrame = 0, skippedLeaves = 0;
};
struct TreeGeometry { MeshData trunk; std::vector<glm::mat4> leafTransforms; GeometryStats stats; };   // + stats
TreeGeometry buildGeometry(const Skeleton&, const TreeParams&, const GeometryOptions& = {});          // throws Error on ringDensity < 3, a bad parent index, > 2^32-1 vertices
struct GenerateOptions { uint32_t seed = 0; int ringDensity = 9; unsigned threads = 0; };             // + threads
struct TreeModel { Skeleton skeleton; TreeGeometry geometry; ModuleString modules; };
TreeModel generateTree(const TreeParams&, const GenerateOptions& = {});
```

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | `F(1)` gives 18 vertices and **48** indices (8 quads x 6), 16 triangles. The brief's "24 indices" is a typo (it is the triangle count x 1.5). Tests assert 18/48 and "16 triangles". | Legacy `createIndiciesForSegment` emits 6 indices per quad, `ringDensity-1` quads. |
| 2 | A segment is emitted iff its own radius, its parent's radius (bottom ring) are finite and `> 0` and its own and its parent's frames are usable (all 16 entries finite and `|x| <= 1e8`, and the x/z columns of the 3x3 not (nearly) parallel). Reason counters: `skippedZeroRadius` (radius `<= 0`) vs `skippedInvalidFrame` (non-finite/huge radius or matrix, singular frame, non-finite roll). | A ring with radius 0 or a singular frame yields a zero normal -> `normalize` = NaN. The `1e8` bound keeps `dot(n,n)` of the unnormalised normal below `FLT_MAX`. |
| 3 | The radial normal is `normalize(R*(r cos, 0, r sin)*2)` as in the legacy code; if the result is not finite (underflow for tiny radius), the local direction `(cos, 0, sin)` is used. | Bit parity for normal input, hard no-NaN guarantee otherwise. |
| 4 | Work unit = one emitted segment (both rings + its 16 triangles). `threads == 0`: `hardware_concurrency()` (min 1) threads iff vertices `> 50000`, else 1. `threads == n > 1`: exactly `min(n, segments)` threads even below the threshold (so tests can force it). Thread creation failure falls back to the remaining chunks on the caller. | Bit-identical by construction; testable. |
| 5 | Leaves: iterate **all** segments (including the root, as the legacy loop does) in index order; for each tip draw `u = uniform01()` from `Rng(seed).derive(3)`; keep iff `u <= leafGrowthProb`; when `leavesAngleDiversity > 0` and kept, draw `a = uniform01()*2d - d`; `leaf = model * rotate(I, a, Y)`. A kept leaf with a non-finite/huge model is dropped and counted in `skippedLeaves` (draws are consumed first so the stream stays aligned). `leafGrowthProb <= 0` or NaN -> empty without drawing. | Legacy order; draws independent of robustness decisions. |
| 6 | `generateTree`: `seed = options.seed ? options.seed : params.seed`; stages get `RewriteOptions{seed}`, `InterpretOptions{seed}`, `GeometryOptions{ringDensity, seed, threads}` and each derives its own stream (1/2/3) inside. | Stages independent of each other's draw counts (already true by `Rng::derive`). |
| 7 | `buildGeometry` of an empty skeleton returns empty data; `ringDensity < 3` throws `treegen::Error`. | Fail loudly on caller misuse; ring density 2 collapses to a flat strip and 1 divides by zero. |

## Steps

### Step group 1: types and the failing tests

- [x] **1.1** Create `MeshData.h`, `Geometry.h` (declarations), `TreeGenerator.h`; stub `Geometry.cpp`/`TreeGenerator.cpp` that compile (bodies throw).
- [x] **1.2** Write `geometry_tests.cpp`: `buildGeometry: single F(1) gives 18 vertices and 48 indices`, chain of N=5 (`2*rd*N` / `6*(rd-1)*N`), other ring densities (3, 5, 9, 16), root alone emits nothing, UV range and `v` 0/1 per ring, seam duplicates, unit normals, indices `<` vertex count and in the legacy order. Run -> FAIL.
- [x] **1.3** Commit: `T06: add geometry interfaces and failing tests`.

### Step group 2: trunk geometry

- [x] **2.1** Implement the classification pre-pass, exact sizing, the per-segment fill (ring table of `sin/2`, `cos/2` precomputed per ring density with the same `theta*j` argument as the legacy code), the thread split and `GeometryStats`.
- [x] **2.2** Run `treegen_tests -tc="buildGeometry*"` -> PASS. Commit: `T06: implement trunk geometry`.

### Step group 3: parity against the frozen legacy math

- [x] **3.1** Write `legacy_reference/LegacyGeometryReference.h` (banner "frozen reference, deleted in T07"; plain `RefSegment{parent, model, radius, roll}`; verbatim `enqueueGenerationData`, `createIndiciesForSegment`, `computeRingPoint`, adapter replay loop).
- [x] **3.2** `geometry_parity_tests.cpp`: for `parametricTernaryTreeE.json`, `parametricSympodialTreeA.json`, `parametricMonopodialTreeA.json`, `symetricTree.json`, `legacy/advancedTree.l` at depth <= 3..4: compare positions, normals, UVs (1e-4) and indices (identical). Also a hand-made skeleton with non-zero roll differences. Run -> PASS. Commit: `T06: add legacy geometry reference and parity tests`.

### Step group 4: determinism, threads, leaves, robustness

- [x] **4.1** Tests: same seed -> identical bytes (`memcmp` of vertices, indices, leaves), threads 1 vs 2 vs 4 vs 7 vs 0 byte-identical on a >50000-vertex tree (forced single vs forced multi, plus the automatic path), `leafGrowthProb=0.5` different seeds -> different leaf set, leaf count == tips at prob 1, 0 -> empty, diversity rotation is a Y rotation within `[-d, d]`, leaf model with diversity 0 equals the tip model.
- [x] **4.2** Robustness tests: zero radius, negative radius, NaN radius, NaN/Inf model, huge model, singular frame, zero parent radius, bad parent index throws, ringDensity < 3 throws; every vertex scanned for NaN/Inf; stats counters asserted.
- [x] **4.3** NaN sweep over every `*.json`/`*.l` in `Resources/LindenmayerRules` at `min(file depth, 5)` (count and require expected `RewriteError`/`LoadError`/`InterpretError` files, never a NaN). Run -> PASS. Commit: `T06: leaves, threads and robustness tests`.

### Step group 5: facade

- [x] **5.1** `geometry_facade_tests.cpp`: end-to-end on `parametricTernaryTreeE.json` (counts consistent with the skeleton, modules non-empty, no NaN, deterministic), seed 0 uses `params.seed`, explicit seed beats it, stages independent (changing `leafGrowthProb` changes leaves only, trunk bytes and skeleton unchanged), and `geometry_header_tests.cpp` self-containment.
- [x] **5.2** Implement `generateTree`. Commit: `T06: add generateTree facade`.

### Step group 6: platform checks and docs

- [x] **6.1** Link `Threads::Threads`; Windows Debug full build + `ctest`; Release `treegen_tests` + a timing test program (a throwaway scratch exe outside the repo, not committed).
- [x] **6.2** Linux check in Docker (engine OFF); `/W4` and `-Wall -Wextra -Wpedantic` clean.
- [x] **6.3** Build the full `ESTree` target and smoke launch `ESTree.exe --scene 0`.
- [x] **6.4** Update `treegen-design.md`, append D-015 to `DECISIONS.md`, set this plan's status to `review`. Commit: `T06: docs and decision log`.

## Review focus covered

- **#5 geometry half** (radius 0, NaN frames/radius, parent == child position -> zero-length is fine because the frame stays regular): tests `buildGeometry: zero radius ...`, `... non-finite frame ...`, `NaN scan over all shipped rule files`, every vertex checked with `std::isfinite`.

## Deviations from the brief

- `F(1)` index count is 48, not 24 (decision 1).
- Additive fields not in the design doc: `GeometryOptions::threads`, `GenerateOptions::threads`, `TreeGeometry::stats` (decision 4/2); logged in D-015.

## Blockers

(none)
