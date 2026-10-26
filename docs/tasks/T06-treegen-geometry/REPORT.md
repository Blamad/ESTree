# T06 — `treegen` geometry and facade — Report

**Result:** done · **Branch:** `task/T06-treegen-geometry` · **Final commit:** `143c455` (merged into `modernization` as `0d23587`)

The implementing subagent could not write this file; the orchestrator wrote it from the agent's hand-back and
re-verified the merged tree (marked *verified*; results appended at the end).

## What changed

- New `MeshData.h`, `Geometry.h/.cpp`, `TreeGenerator.h/.cpp` (the latter includes the whole pipeline); `ESEngine/TreeGen/CMakeLists.txt` links `Threads::Threads`.
- Ring math is the legacy math operation for operation (`radius` is a diameter, last ring vertex repeats the first, root emits nothing). Exact counts: `2*rd` vertices and `6*(rd-1)` indices per emitted segment, `uint32_t` indices, no caps. A pre-pass lists emitted segments; each fills its own slice of the output, in chunks over `std::thread` (only above 50 000 vertices by default).
- Leaves from `Rng(seed).derive(3)`; `leafModel = segmentModel * leafRotation`; engine world transform stays out of treegen.
- Tests: `geometry_tests.cpp`, `geometry_parity_tests.cpp`, `geometry_facade_tests.cpp`, `geometry_header_tests.cpp`; frozen reference `tests/treegen/legacy_reference/LegacyGeometryReference.h` (deleted in T07).
- Docs: `treegen-design.md` (additive), D-015 in `DECISIONS.md`. No engine file touched (`LindenmayerTreeMeshGenerator.*`, `LindenmayerTree.*` untouched).

## Acceptance criteria

| Criterion | Evidence |
|---|---|
| Parity with legacy ring/index math (≥ 3 productions) | agent: 5 shipped files + 1 hand-written production, ring density 9 and 5, 1 and 3 threads: max deviation 0 (tolerance 1e-4), indices identical; a deliberately broken roll alignment made all 7 parity cases fail |
| NaN scan, determinism, thread independence | agent: every `.json` and `.l` rule file at `min(depth, 5)`; only `legacy/parametricTree2.l` fails (`RewriteError`, asserted); byte-identical for threads 1, 2, 3, 4, 7, 64 and auto on 72 000 vertices |
| ctest, `/W4`, boundary | agent: Windows `100% tests passed out of 226`; Linux Docker `... out of 187`; no warnings under `/W4` and `-Wall -Wextra -Wpedantic` |
| `TreeGenerator.h` self-contained | `geometry_header_tests.cpp` includes only that header |
| Engine untouched, smoke OK | agent: `ESTree.exe --scene 0` alive after 15 s (still the legacy generator) |

## Timings (Release, `generateTree`, seed 1, best of 3, agent)

| Tree | Segments | Vertices | Triangles | Leaves | auto threads | 1 thread |
|---|---|---|---|---|---|---|
| ternaryE depth 8 | 13 121 | 236 178 | 209 936 | 6 561 | 20.7 ms | 25.1 ms |
| ternaryE depth 10 | 118 097 | 2 125 746 | 1 889 552 | 59 049 | 191 ms | 215 ms |
| sympodialA depth 8 | 256 | 4 608 | 4 096 | 128 | 0.9 ms | 0.8 ms |

ternaryE depth 10 stage split: rewrite 97 ms, interpret 34 ms, geometry 43.5 ms (71 ms single-threaded).

## For T07 (GL buffer capacities)

Size GL buffers per tree from the output: vertices `trunk.vertices.size()`, indices `trunk.indices.size()`, leaves `leafTransforms.size()`. The legacy caps (100 000 vertices / 4 000 000 indices) would overflow ternaryE depth 10 (2.1 M vertices, 5.67 M indices; `MeshVertex` = 32 bytes → ≈ 68 MB of vertex data). Convert `uint32_t` indices to the engine's `int` only below 2^31.

## Interface changes (additive; design doc + D-015)

`GeometryOptions::threads` / `GenerateOptions::threads` (0 auto, 1 never, n = `min(n, segments)`; output bit-identical for any value), `TreeGeometry::stats` (`GeometryStats`), `buildGeometry` throws `treegen::Error` for `ringDensity < 3`, a bad parent link or > 2^32-1 vertices.

## Deviations and behavior worth knowing

- `F(1)` gives 18 vertices and **48** indices (16 triangles); the brief's "24 indices" was a typo — tests assert 48.
- A segment is skipped (no vertices/indices) when its own or its parent's radius is `<= 0`, or on a non-finite / beyond-1e8 radius, roll or frame, or a singular frame; counted in `stats`.
- Leaf iteration includes the root, as the legacy loop did; a skeleton without segments gets one leaf at the root. A kept leaf with a non-finite transform is dropped after its RNG draws and counted in `stats.skippedLeaves`.

## Follow-ups

- T07: delete `tests/treegen/legacy_reference/`, switch the engine to `generateTree`, convert `MeshVertex`/indices, catch `RewriteError` for `legacy/parametricTree2.l`; the "Fixed buffers" known defect closes then.

## Needs human verification

None until T07 (nothing rendered changed).
