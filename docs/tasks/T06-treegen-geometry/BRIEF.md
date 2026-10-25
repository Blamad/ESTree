# T06 — `treegen` geometry and facade

**Depends on:** T05 · **Unblocks:** T07, T08 · **Size:** L · **Branch:** `task/T06-treegen-geometry`

## Goal

Turn a `Skeleton` into render-ready CPU data without any engine type:
`buildGeometry(skeleton, params, opts) -> TreeGeometry { MeshData trunk; leafTransforms }`, plus
the facade `generateTree(params, opts) -> TreeModel` (rewrite → interpret → geometry) — with
dynamic sizing (no fixed buffers), deterministic leaves, and no NaNs.

## Context to read

- `docs/architecture/treegen-design.md` (MeshData, Geometry, TreeGenerator), `docs/architecture/overview.md` → *Geometry (as implemented)*
- `ESEngine/ESTree/Models/Tree/LindenmayerTreeMeshGenerator.{h,cpp}` (ring math, indices, UVs, normals, thread split)
- `ESEngine/ESTree/Models/Tree/LindenmayerTree.cpp` (`generateInstancedLeaves`, buffer sizes)
- `ESEngine/Engine/GameObject/Mesh.{h,cpp}`, `Engine/Core/Vertex.h` (what the adapter will feed — **do not include them in treegen**)

## Scope

In:
1. `MeshData.h`, `Geometry.h/.cpp`, `TreeGenerator.h/.cpp` per the design doc.
2. Reproduce legacy trunk geometry exactly (parity first): per segment a bottom ring (parent frame rotated by `segment.roll - parent.roll` about Y, v=0) and a top ring (segment frame, v=1), `ringDensity` vertices per ring (default 9, last duplicates first for the UV seam, u = i/(ringDensity-1)), ring point `(radius·cos θ /2, 0, radius·sin θ /2, 1)` transformed by the frame, radial normal from the frame's 3×3, 6 indices per quad, order as in `createIndiciesForSegment`. Root segment emits nothing by itself.
3. Sizing: compute exact vertex/index counts from the skeleton up front (`reserve`), no hard-coded 100 000 / 4 000 000 limits (Review Focus: growth). Index type `uint32_t`.
4. Parallelism: ring-point computation may use `std::thread`/`std::async` chunks above a threshold (e.g. > 50 000 vertices), single-threaded below; result must be bit-identical regardless of thread count (test).
5. Leaves: `leafTransforms` for every tip (`isTip`) kept with probability `params.leafGrowthProb` using `Rng(seed).derive(3)`; if `leavesAngleDiversity > 0`, multiply by a rotation about Y of a uniform angle in `[-d, d]` (legacy order: `leafModel = segmentModel * leafRotation`; the engine premultiplies the tree's world transform — keep that out of treegen). `leafGrowthProb <= 0` ⇒ empty.
6. Robustness (Review Focus #5, geometry half): segments with `radius <= 0` or non-finite frames are skipped (no triangles) and counted in a `GeometryStats` (returned via optional out-param or field on `TreeGeometry`); output never contains NaN/Inf — test by scanning all vertices.
7. Facade `generateTree` + `GenerateOptions{seed, ringDensity}`: single seed → `Rng` streams 1/2/3 so each stage is independent of the others' draw counts.
8. Tests `tests/treegen/geometry_tests.cpp`: single `F(1)` ⇒ 18 vertices / 24 indices … (derive exact counts from ring density; assert), chain of N segments counts, UV range, normals unit-length (±1e-4), indices < vertex count, seam duplicates, thread-count independence, determinism (same seed ⇒ identical bytes; different seed ⇒ different leaf set for `leafGrowthProb=0.5`), leaf count vs tips, zero-radius skip, NaN scan over every shipped rule file at depth min(file depth, 5), facade end-to-end on `parametricTernaryTreeE.json`.
9. Parity test vs. the legacy math: freeze the legacy ring computation in `tests/treegen/legacy_reference/` (same approach as T05) and compare vertex positions/normals/uvs and indices for 3 shipped productions at small depth.

Out of scope: smooth shared-joint meshes, LOD, new UV scheme, engine integration (T07).

## Owned files

`ESEngine/TreeGen/include/treegen/{MeshData,Geometry,TreeGenerator}.h`, `src/{Geometry,TreeGenerator}.cpp`, `tests/treegen/geometry*`, `tests/treegen/legacy_reference/**` (add), `ESEngine/ESTree/Models/Tree/LindenmayerTreeMeshGenerator.*` — **do not touch yet** (T07 deletes it).

## Acceptance criteria

1. Parity test passes (vertices within 1e-4, indices identical) for ≥ 3 productions.
2. NaN scan passes for every shipped rule file; thread-count independence and determinism tests pass.
3. `ctest` green, boundary green, `/W4` clean. Report vertex/triangle counts and `generateTree` time (Release) for depth 8 and 10 of `parametricTernaryTreeE.json` and for `parametricSympodialTreeA.json`.
4. `TreeGenerator.h` is the only header a consumer needs (`#include <treegen/TreeGenerator.h>` compiles standalone — add a header self-containment test).

## Verification

Automated. **Needs human verification** only after T07.

## Notes

- Keep `radius` semantics: the stored value is a diameter (see overview). Document it in `Geometry.h`.
- The legacy generator reserves memory by guess and `Mesh` is created with fixed GL buffer capacity — T07 must size the GL buffers from `MeshData` instead; mention required capacities in `REPORT.md`.
