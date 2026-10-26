# T07 — Engine adapter switch-over; delete the legacy pipeline — Report

**Result:** done (owner checklist pending) · **Branch:** `task/T07-engine-adapter-switchover` · **Final commit:** `b0ad0f2` (merged into `modernization` as `6915bd2`)

The implementing subagent could not write this file; the orchestrator wrote it from the agent's hand-back and
**re-verified on the merged tree** (marked *verified*). Merge note: `DECISIONS.md` conflict — both T07 and T08 had chosen D-017; T07 keeps D-017, T08 is now **D-018**.

## What changed

- **Adapter:** `LindenmayerTree` keeps its API; it takes `treegen::TreeParams` plus an optional console seed, `generate()` calls `treegen::generateTree` and uploads into `Mesh`/`InstancedMesh`. GL buffers are sized from the output (the 100 000 / 4 000 000 caps are gone; `Mesh`/`InstancedMesh` needed no edit). No leaf mesh for an empty leaf set; no hull `RigidBody` for a tree without geometry. New pure header `TreeMeshConversion.h`: `MeshVertex`→`Vertex` (color zeroed), `uint32_t`→`int` indices (refused above `INT_MAX`), leaf matrices as `world * leaf`, `checkedBufferSize`, seed helpers.
- **Seed:** non-zero `TreeParams::seed`, else console `seed <n>`, else a fresh random non-zero value. Log: `Generation finished. N tris. Time: Xms. Seed: S`; console prints the seed after `tree`/`rtree`. `rtree` drops a rule-file seed and draws a new one; `rtree seed <n>` reuses n; `tree file X seed n` with a seeded file X ignores n and says so.
- **Errors:** `generate()` and the factory propagate `treegen::Error`; the console prints one `Tree error: ...` line (on `rtree` the old tree stays); scenes use `LindenmayerTreeFactory::generateTreeFromFile`, which logs and skips a broken tree. Also fixed in `ConsoleTreeCommand`: an infinite loop on an unknown parameter and a C-cast of the selected object (now `dynamic_cast`, with "Selected object is not a tree!").
- **Factory/scenes:** factory gained `Scene*` and `seed` parameters; the three duplicated `createLindenmayerTree` helpers are gone. `LindenmayerTreeParams`/`Rule` deleted (no alias).
- **`legacy/parametricTree2.l`:** `[^+a)` → `[^+(a)` (the only edit that keeps every typed character; `[^(a)` and `[+(a)` cannot be ruled out — D-017, owner decision below). Params golden edited; `solver/legacy__parametricTree2.l.d1..d3` rewritten with `treegen`.
- **Deleted:** `Lindenmayer{Solver,Interpreter,MeshGenerator}.*`, `Segment.h`, `tests/legacy_characterization/`, `tests/treegen/legacy_reference/`, the MSVC-only guard, and `turtle_equivalence_tests.cpp` / `geometry_parity_tests.cpp` (they only compared against the frozen references). `tests/golden/**` data stays (used by `treegen` golden tests).
- **Tests:** new `tree_adapter_tests` (`tests/adapter/`, headless, links only `treegen`); three checks ported to `golden_data_tests.cpp` (golden-directory completeness, deterministic files give the same production for every seed, stochastic files keep alphabet and bracket balance). All flipped KNOWN BUG cases #1–#14 map to existing `loader_tests`/`rewriter_tests`/`rewriter_golden_tests`.
- **Docs:** `overview.md` has a *Tree pipeline* section; `CLAUDE.md` known defects and golden READMEs updated; D-017.

## Acceptance criteria

| Criterion | Evidence |
|---|---|
| Build Debug/Release, ctest green | *verified* (merged tree, ninja-msvc): Debug build exit 0 and `100% tests passed out of 232`; Release build exit 0 and `... out of 232` |
| No Boost in `ESEngine` sources | agent + merged tree: `grep -rli boost ESEngine --include=*.h --include=*.cpp` → no matches |
| `Lindenmayer*` only in adapter/factory/console/scenes | agent: confirmed |
| Linux | *verified*: Docker Ubuntu 24.04/GCC 13, engine OFF: `100% tests passed, 0 tests failed out of 219`. Agent: engine ON (CI package list) links and 201/201; engine ON without any Boost package compiles everything and fails only at link on `-lboost_*` (until T13) |
| Smoke | *verified*: scenes 0–4 alive after 14 s and closed cleanly (`Quitting..`), `Generation finished ... Seed:` lines (scene 0: `23328 tris. Time: 113ms. Seed: 94942092`; scene 2: 4 trees; scene 4: 16 trees); no crash events. Agent also: invalid rule file → `Tree ... skipped: ...` and the scene keeps running; `depth` 9 (629 856 tris, ≈ 7× the old cap) alive 30 s, clean close |

## Deviations

- Changed T04/T06 test files (`geometry_tests`, `geometry_facade_tests` — a synthetic broken input replaces `parametricTree2.l` — and `rewriter_golden_tests`); factory signature changed; `ConsoleTreeCommand.h` gained a private method. `tree_adapter_tests` was written header-first, not strict TDD.

## Human verification result (owner, 2026-10-25)

Items 1-4 confirmed: branches droop as intended and the model is coherent (1), trees render and exits are clean (2), the seed is present (3), seeded regeneration works (4).
Items 5-8 were not confirmed one by one ("the rest probably works too"). New observation (not a T07 regression, see BACKLOG): leaf sway appears to pause while the camera moves.

## Needs human verification (owner checklist — blocking for sign-off)

Run `build\ninja-msvc\Debug\ESTree.exe --scene N` from `build\ninja-msvc` (Resources linked).
1. `--scene 0`: compare the tree with `images/ternaryTreeE.PNG` (shape, bark seams, leaves). Trees using `T` droop on purpose (D-016).
2. `--scene 2` (forest) and `--scene 4`: trees render, no crash on exit.
3. `~` console, `help`: `tree` and `rtree` list `seed <int>`.
4. `tree file 4 pos 0,0,0 seed 42` twice → the same tree; the log shows the same tris count and `Seed: 42`.
5. `tree file 4` twice → different trees, different seeds in the log (not file 15: `parametricTernaryTreeB` is deterministic, every seed gives the same tree).
6. With a tree selected: `rtree` → new tree and new seed; `rtree seed 42` reproduces the tree from step 4.
7. `tree file legacy/parametricTree2.l` now generates a tree: look at its first branch and decide between the readings `[^+(a)` (chosen), `[^(a)`, `[+(a)` (D-017). A nonexistent file or `tree foo 1` prints one console line and does not crash.
8. Large trees: `tree file 18` (ternary E); with a `Cube` selected `rtree` says "Selected object is not a tree!".

## Follow-ups

- T13: remove the Boost link lines / `BOOST_LIB` requirement in the root `CMakeLists.txt` (the engine no longer includes Boost); then Windows CI can build the engine too (T11 follow-up).
- Stale comments in `treegen` (`Geometry.h`, `Geometry.cpp`) still mention the legacy classes.
- The console input path is untested headless; the checklist above is the owner's.
