# T07 — Engine adapter switch-over; delete the legacy pipeline

**Depends on:** T04, T06 · **Unblocks:** T12 · **Size:** M · **Branch:** `task/T07-engine-adapter-switchover`

## Goal

`LindenmayerTree` (the `GameObject`) becomes a thin adapter that calls `treegen::generateTree`
and uploads the result into the existing `Mesh`/`InstancedMesh`; all remaining legacy
`Lindenmayer*` generation classes, their Boost usage and the frozen test references are deleted;
the tree seed becomes controllable from the in-game console.

## Context to read

- `docs/architecture/treegen-design.md` → *Compatibility with the legacy pipeline*
- `ESEngine/ESTree/Models/Tree/*` (everything), `Console/ConsoleTreeCommand.*`, `Scenes/{SingleTree,Forest,Tree}Scene.*`
- `ESEngine/Engine/GameObject/{Mesh,InstancedMesh,InstancedTransform}.*`, `Engine/Core/Vertex.h`
- Reports of T03–T06 (`docs/tasks/T0[3-6]-*/REPORT.md`) for deviations and required GL buffer sizes

## Scope

In:
1. New adapter code in `ESEngine/ESTree/Models/Tree/`: `LindenmayerTree` keeps its constructor and public API (`generate()`, `getTreeParams()`, `getBarkMaterial()`, `getLeavesMaterial()`), but internally holds `treegen::TreeParams` + `uint32_t seed` and calls `treegen::generateTree`. Conversion helpers (`MeshVertex`→`Vertex`, `uint32_t`→`int`, leaf matrices → `InstancedTransform` premultiplied with the tree's world transform exactly as `generateInstancedLeaves` did).
2. Size the GL buffers from the generated data (`vBufferSize = vertices.size()`, `iBufferSize = indices.size()` plus a small margin if `Mesh` requires it); remove the 100 000 / 4 000 000 magic numbers. Check `Mesh::updateMesh` for capacity assumptions.
3. Seed: `LindenmayerTreeParams`/`TreeParams::seed` (JSON `"seed"`) if non-zero, else the console `seed <n>` argument, else a fresh random value; log the effective seed with the generation time (`std::chrono`, replacing Boost `posix_time`) so a good-looking tree can be reproduced. `ConsoleTreeCommand`: add `seed` parameter to `tree`; `rtree` keeps the selected tree's params and by default draws a **new** seed (document), `rtree seed <n>` reuses one.
4. Delete: `LindenmayerTreeSolver.*`, `LindenmayerTreeInterpreter.*`, `LindenmayerTreeMeshGenerator.*`, `Segment.h`, the legacy `Rule`/`LindenmayerTreeParams` facade if nothing else needs it (callers may use `treegen::TreeParams` directly; keep a one-line alias header if scenes need it), `tests/legacy_characterization/**` and `tests/treegen/legacy_reference/**` **only if** their goldens were migrated to `treegen_tests` (T03–T06) — otherwise leave them and say why.
5. Tree scenes: remove the three duplicated private `createLindenmayerTree` helpers in favor of `LindenmayerTreeFactory` (keep behavior; positions/materials unchanged).
6. No `boost/` include remains in `ESEngine/ESTree/Models/Tree/`.
7. Update `docs/architecture/overview.md` (the *Legacy tree pipeline* section becomes *Tree pipeline* pointing at `treegen`).

Out of scope: visual changes, new features, other Boost users (T09), CMake Boost removal (T13).

## Owned files

`ESEngine/ESTree/Models/Tree/**`, `ESEngine/ESTree/Console/ConsoleTreeCommand.*`, `ESEngine/ESTree/Scenes/{SingleTreeScene,ForestScene,TreeScene}.*`, `docs/architecture/overview.md`.

## Acceptance criteria

1. Build Debug/Release; `ctest` green (all `treegen` tests; legacy tests removed or still passing).
2. `grep -rn "boost" ESEngine/ESTree/Models/Tree` → no matches; `grep -rn "Lindenmayer" ESEngine` shows only the adapter/factory/console/scenes.
3. Console `tree file 15 seed 42` twice ⇒ identical log lines for vertex/index counts (record in report via a headless unit test of the adapter's pure conversion helpers — the console itself is human-verified).
4. A unit test (headless, no GL) of the conversion helpers: vertex fields, index narrowing, leaf matrix premultiplication.

## Verification

Automated (above). **Needs human verification (blocking for sign-off):**
1. `ESTree.exe --scene 0` — compare the tree with `images/ternaryTreeE.PNG` (shape, bark seams, leaves).
2. `--scene 2` forest renders; no crash on exit.
3. Console: `tree file 15 pos 0,0,0 seed 42` twice → same tree; `tree file 15` twice → different trees; `rtree` on a selected tree regenerates.
4. `~` console `help` still lists tree commands. Provide this checklist in `REPORT.md`.

## Notes

- The hull `RigidBody` is built from the mesh in `LindenmayerTreeFactory::generateTree` (`initAsAHullShape(0)`); make sure it still sees the final vertices.
- `GameObject::addGameObject` children (leaves) are owned by the tree object — keep that structure.
