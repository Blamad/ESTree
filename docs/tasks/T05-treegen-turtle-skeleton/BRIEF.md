# T05 — `treegen` turtle interpreter → `Skeleton`

**Depends on:** T03 · **Unblocks:** T06 · **Size:** L · **Branch:** `task/T05-treegen-turtle-skeleton`
**Parallel with:** T04 (different files; hand-build `ModuleString`s in tests)

## Goal

Extract the turtle interpretation out of `LindenmayerTreeInterpreter` into
`treegen::interpret(params, modules, opts) -> Skeleton`: pure data, no meshes, no GL types, no
shared-pointer tree, seeded randomness — numerically equivalent to the legacy segment frames.

## Context to read

- `docs/architecture/treegen-design.md` (Skeleton, Turtle), `docs/architecture/overview.md` → interpreter alphabet table, *Geometry*
- `ESEngine/ESTree/Models/Tree/LindenmayerTreeInterpreter.{h,cpp}`, `Segment.h`, `LindenmayerTreeMeshGenerator.{h,cpp}` (how `modelMatrix`/`radius`/`roll` are consumed)
- `ESEngine/ESTree/Models/Tree/LindenmayerTree.cpp` (`generateInstancedLeaves` uses `isLastStem` and `modelMatrix`)

## Scope

In:
1. `Skeleton.h` (+ `Turtle.h/.cpp`): data model per design doc: `std::vector<SkeletonSegment>`, `parent` as index, `childCount`, `model` (frame at segment end), `radius`, `roll`. Root = index 0, identity-ish frame as in legacy `createRoot`.
2. Turtle semantics **identical to the legacy table**: `F f [ ] ^ v < > + - ; \` = $ T`, defaults (`angle`, 0.625, `tropismBendingFactor`), post-`F` state reset (rotation→identity, `totalRotation` accumulates, length→`initialLength`, `radius/angle/lengthScale/roll` persist), unknown chars ignored. Parameters now come from `Module::params[0]`; `Module::randomized` (`X~(v)`) draws `uniform01()*v` from `Rng(seed).derive(2)`.
3. Bug fixes (log in `DECISIONS.md`): literal `-1` parameters honored (use `params.empty()` not a sentinel); unbalanced `]` (or `]` with empty stack) → `InterpretError` with the module index (Review Focus #1); unmatched `[` at end is allowed (legacy ignored it) — keep, with a test.
4. Replace the legacy tropism/orientation helpers with equivalent `glm` code in `Turtle.cpp` (`applyTropism`, `restoreHorizontalOrientation`); keep `restoreHorizontalOrientationAlfa` out (dead code).
5. Legacy wiring (strangler): `LindenmayerTreeInterpreter::generateMeshData(...)` keeps its signature but now calls `treegen::interpret` once (parse `product` via `parseModules`) and then replays the skeleton into the old `LindenmayerTreeMeshGenerator` (enqueue rings in parent-before-child order exactly like `createSegment` did). Engine output must stay equivalent. Remove its Boost RNG.
6. Tests `tests/treegen/turtle_tests.cpp` — analytic cases with `EXPECT` on positions/frames (epsilon 1e-4): `F(1)` ⇒ one segment, end at `(0,1,0)`; `F(1)F(1)` chain; `F[+F]F` ⇒ parent indices & `childCount`; `^(90)F(1)` direction; pitch/yaw/roll axes; `;` `` ` `` `=` scaling; `$`; `T` bends towards tropism (monotone angle decrease); `~` determinism; defaults; unknown chars; `-1` param; unbalanced `]`; empty input; 100 000-segment chain runs in < 1 s Release (print time, don't hard-assert in Debug).
7. **Equivalence harness** (test-only, no GL): copy the legacy interpreter math into `tests/treegen/legacy_reference/` as a frozen reference (it can't link the legacy class because it needs `Mesh`) and compare `model` matrices + radius + roll of old vs new for 5 shipped deterministic productions (from T02 goldens at depth ≤ 4), epsilon 1e-4. Delete the reference in T07.

Out of scope: mesh/rings/leaves (T06), rewriting (T04), changing the alphabet.

## Owned files

`ESEngine/TreeGen/include/treegen/{Skeleton,Turtle}.h`, `src/Turtle.cpp`, `tests/treegen/turtle*`, `tests/treegen/legacy_reference/**`, `ESEngine/ESTree/Models/Tree/{LindenmayerTreeInterpreter.*,Segment.h}`.

## Acceptance criteria

1. Equivalence harness passes for ≥ 5 productions; analytic tests pass; `ctest` green; boundary green; `/W4` clean.
2. Review Focus #1 and #6 pinned by named tests.
3. `ESTree` builds; the legacy tree pipeline still compiles and its mesh code path is unchanged (diff of `LindenmayerTreeMeshGenerator.*` is empty).
4. No `shared_ptr<Segment>` in `treegen`.

## Verification

Automated. **Needs human verification:** scene 0 tree looks identical to before (ring/seam alignment depends on `roll` and frames) — give the exact steps in `REPORT.md`.

## Notes

- Legacy `createSegment` builds the bottom ring from the **parent's** `modelMatrix` rotated by `roll - parent->roll` about Y; T06 needs `roll` per segment and the parent link — keep both in `SkeletonSegment`.
- `radius` is a legacy *diameter* in geometry (see overview) — don't "fix" it here.
- Keep an eye on quaternion normalization drift for long chains; normalize on each `F`.
