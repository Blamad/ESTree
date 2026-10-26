# T05 — `treegen` turtle interpreter → `Skeleton` — Report

**Result:** done (one visual decision pending, see below) · **Branch:** `task/T05-treegen-turtle-skeleton` · **Final commit:** `6c75edf` (merged into `modernization` as `bc7583c`)

The implementing subagent could not write this file; the orchestrator wrote it from the agent's hand-back and
**re-verified on the merged tree** (marked *verified*). The merge needed one conflict resolution (`DECISIONS.md`: D-010 and D-014 kept) and one test fix (below).

## What changed

- New `Skeleton.h`, `Turtle.h`, `src/Turtle.cpp` — `treegen::interpret(params, modules, opts)` → `Skeleton` (index-based segments, root = 0, `parent < index`, `childCount`, `model`, `radius` = legacy *diameter*, `roll`). Interface as in `treegen-design.md` (`InterpretOptions` lives in `Turtle.h`).
- Alphabet and defaults identical to the legacy table; `Module::params[0]` is the argument; `X~(v)` draws `uniform01()*v` from `Rng(seed).derive(2)`. `applyTropism`/`restoreHorizontalOrientation` ported to plain glm (no gtx); dead `restoreHorizontalOrientationAlfa` dropped.
- `LindenmayerTreeInterpreter` is an adapter: `parseModules` + `interpret`, then replays the skeleton into the **unchanged** `LindenmayerTreeMeshGenerator` (parent before child, same ring order, same `roll - parent.roll` alignment) and returns the same `vector<shared_ptr<Segment>>`. Boost RNG gone; `Segment::roll` defaults to 0 (was uninitialised). On a `treegen::Error` the adapter logs and draws the root only.
- Tests: `turtle_tests.cpp` (30 analytic cases incl. `Review Focus 1`, `Review Focus 6`), `turtle_equivalence_tests.cpp` (12 cases, sweep of 137 goldens up to depth 4) and the frozen reference `tests/treegen/legacy_reference/LegacyTurtleReference.h` (deleted in T07).

## Acceptance criteria

| Criterion | Evidence |
|---|---|
| Equivalence harness + analytic tests pass; ctest, boundary green | *verified* on merged tree: Windows `100% tests passed out of 189`; Docker Ubuntu 24.04/GCC 13 `100% tests passed, 0 tests failed out of 150`. Agent: sweep without `T` = 0 deviation vs verbatim legacy; with `T` = 0 vs legacy+normalisation |
| Review Focus #1 and #6 pinned | tests `Turtle: Review Focus 1: ...` (message contains "module N") and `Review Focus 6: ...` |
| `LindenmayerTreeMeshGenerator.*` unchanged | *verified*: `git diff` vs HEAD empty |
| `ESTree` builds and runs | *verified*: build exit 0; scenes 0–4 alive after 12 s and closed cleanly (`Quitting..`), no crash events |
| No `shared_ptr<Segment>` in treegen | agent: none; no boost / regex / iostream in new files |
| `/W4` and GCC `-Wall -Wextra -Wpedantic` clean | agent: no warnings in treegen / tests / adapter |

## Post-merge fix (orchestrator)

T04 deleted the goldens `solver/legacy__parametricTree2.l.d1..d3`, which the T05 sweep expected to *skip*. The test
`Turtle equivalence: every solver golden up to depth 4` therefore failed after the merge (`0 == 3`). Changed the last check to `skipped.size() <= notModuleStrings.size()`; all 189 tests pass.

## DECISION for the owner: tropism (D-010) — ACCEPTED by the owner on 2026-10-25 (D-016)

In the shipped binary the legacy interpreter starts from `glm::quat()`, which with the pinned glm (0.9.9.8, no `GLM_FORCE_CTOR_INIT`) is the **zero** quaternion. Rotations before the first `F` are lost and the
accumulated rotation stays zero, so **`T` (tropism) bends nothing** in the current binary (older glm initialised `quat()` to identity; the glm update was the last change of 2020-04). `treegen` starts from the identity quaternion,
as the overview, the thesis and the brief describe, so trees with `T` now **droop towards the tropism**.

- Affected: rule files with `T`, notably the ternary trees A–E, incl. scene 0's default `parametricTernaryTreeB.json` (branch tips move up to 6.3 units on a 13.6 high tree at depth 6). Trees without `T` and without rotations before the first `F`: bit-identical.
- Check: `ESTree.exe --scene 0` and compare with `images/ternaryTreeE.PNG` / the YouTube clips from the README (these show the intended, working tropism).
- If rejected: drop the `T` modules in the adapter before `interpret` (restores the old look exactly).

## Other intentional differences (D-010)

Literal `-1` parameter honored; parameters of unknown symbols ignored as a whole; unbalanced `]` → `InterpretError` naming the module; `$` on a heading parallel to the world axis leaves the rotation unchanged (legacy: NaN);
only the accumulated rotation is renormalised after each `F` (the frame keeps the raw local rotation because `$` builds quaternions of norm < 1 that shipped trees depend on); `~` randomness from `Rng(seed).derive(2)`.

## Needs human verification

1. Scene 0: accept/reject the tropism change above.
2. Trees without `T` (e.g. `tree file parametricMonopodialTreeA.json`, `symetricTree.json`) should look identical to before (shape and seam alignment).
3. Optional: `tree file legacy/parametricTree2.l` logs an error and shows only the root (typo in that rule file; fix in T07).

## Follow-ups

- T07: delete `tests/treegen/legacy_reference/` and the adapter; fix `legacy/parametricTree2.l`; `ConsoleTreeCommand` catches `LoadError`/`RewriteError`/`InterpretError`.
- T06 must treat `radius` as a legacy diameter and may rely on `parent < index` and per-segment `roll`.
