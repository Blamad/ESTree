# T05 — `treegen` turtle interpreter → `Skeleton` — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: use `superpowers:subagent-driven-development`
> or `superpowers:executing-plans`. Steps use checkbox (`- [x]`) syntax.

**Status:** review · **Branch:** `task/T05-treegen-turtle-skeleton` · **Base:** `modernization @ 5853916` (T03 done)
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** `treegen::interpret(params, modules, opts) -> Skeleton`: the legacy turtle as pure data (no GL, no
`shared_ptr` tree, seeded randomness), numerically equivalent to the legacy segment frames, and the legacy
`LindenmayerTreeInterpreter` reduced to an adapter that replays the skeleton into the unchanged mesh generator.

**Approach:** Port the state machine of `LindenmayerTreeInterpreter::generateMeshData` one `case` at a time to
operate on `Module`s (parameters = `params[0]`, absent = `params.empty()`), keep the tropism / horizontal-orientation
formulas verbatim in glm, and prove equivalence against a frozen header-only copy of the legacy math
(`tests/treegen/legacy_reference/`) on shipped productions. The legacy class then calls `parseModules` +
`interpret` once and replays `Skeleton::segments` in index order (= creation order = parent before child), which is
exactly the order `createSegment` enqueued rings in, so `LindenmayerTreeMeshGenerator.*` stays byte-identical.

## Findings from reading the legacy code (drive the decisions below)

1. `SegmentTransform(glm::quat(), glm::quat(), ...)` default-constructs two quaternions. In the pinned glm
   (0.9.9.8, no `GLM_FORCE_CTOR_INIT`) `glm::quat()` is **(0,0,0,0)**, not identity (verified with a 10-line program
   on MSVC Debug and `/O2`). Consequences of the as-compiled legacy: rotations issued before the first `F` multiply
   a zero quaternion and are lost; `totalRotation` is zero for the whole tree, so `T` always sees `front = (0,1,0)` and
   (tropism `(0,-1,0)`) is a **no-op**. The documented table (and the brief's "`T` bends towards the tropism" test)
   describes the identity behaviour of older glm. The new code starts from identity quaternions (D-010).
2. The root `Segment::roll` is uninitialised in the legacy code and is read for the first segment
   (`segTransform.roll - parent->roll`); the new root has `roll = 0`; `Segment.h` gets `float roll = 0`.
3. Legacy `getNumericParameter` treats `-1` as "absent" and, for unknown symbols with parameters (`A(-1,2)`), then
   executes the digits' neighbours as symbols (`-` = roll). Reading a `ModuleString` fixes both.
4. `glm::angleAxis(a, c)` in `applyTropism` uses a non-normalised axis (`c = cross(front, tropism)`),
   so the tropism quaternion is slightly non-unit (norm < 1) and its effective angle is `~a*sin(theta)`. Kept as is (behaviour).
   `$` builds its quaternion from an improper matrix and also yields norm < 1 (e.g. `^(45)$` gives `(0.707,0,0,0)`).
   First attempt (normalise the local rotation on each `F`) changed every `$` tree by up to 1.0 world unit, so only
   the accumulated rotation is renormalised; the frame uses the raw local rotation like the legacy code.
5. `restoreHorizontalOrientation` divides by `|cross(front, down)|`, which is 0 for a freshly reset turtle
   (`front = (0,1,0)`): NaN frames in the legacy. New code: if the cross product is shorter than 1e-6 the rotation is left unchanged.

## Files

- Create: `ESEngine/TreeGen/include/treegen/Skeleton.h` — `SkeletonSegment`, `Skeleton`, `isTip`
- Create: `ESEngine/TreeGen/include/treegen/Turtle.h` — `InterpretOptions`, `interpret`
- Create: `ESEngine/TreeGen/src/Turtle.cpp` — state machine, `applyTropism`, `restoreHorizontalOrientation` (anonymous namespace)
- Create: `tests/treegen/turtle_tests.cpp` — analytic tests
- Create: `tests/treegen/turtle_equivalence_tests.cpp` — old-vs-new harness (reads `tests/golden/solver/*.txt`)
- Create: `tests/treegen/legacy_reference/LegacyTurtleReference.h` — frozen legacy math (glm only), deleted in T07
- Modify: `ESEngine/ESTree/Models/Tree/LindenmayerTreeInterpreter.{h,cpp}` — adapter; Boost RNG, turtle helpers removed
- Modify: `ESEngine/ESTree/Models/Tree/Segment.h` — `float roll = 0`
- Modify: `docs/DECISIONS.md` (append D-010), this PLAN.
- Must stay byte-identical: `ESEngine/ESTree/Models/Tree/LindenmayerTreeMeshGenerator.{h,cpp}`.
- Not modified: CMake files (`treegen` and `treegen_tests` pick new files up by glob).

## Interfaces

- Consumes: `treegen::{Module, ModuleString, parseModules, Rng, TreeParams, InterpretError}` (T03).
- Produces (design doc, unchanged):

```cpp
struct SkeletonSegment { int parent; glm::mat4 model; float radius; float roll; int childCount = 0; };
struct Skeleton { std::vector<SkeletonSegment> segments; };         // [0] = root, identity frame, radius = initialRadius, roll 0
inline bool isTip(const SkeletonSegment& s) { return s.childCount == 0; }
struct InterpretOptions { uint32_t seed = 0; };
Skeleton interpret(const TreeParams&, const ModuleString&, const InterpretOptions& = {});   // throws InterpretError
```

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | Identity initial quaternions (finding 1) | The table and the brief describe working tropism; zero-quaternion start is an accident of the glm upgrade. Logged as D-010 with a human-verification step (ternary trees change). The reference supports both inits and a test pins the as-compiled difference. |
| 2 | `X~` without parameters: no draw, parameter treated as absent | Legacy computed `-random` (garbage). |
| 3 | Parameters on `[`, `]`, `$` are ignored without a draw | Legacy never read them. |
| 4 | Only modules that actually read a parameter draw from the RNG, in module order, from `Rng(seed).derive(2)` | Draw order is part of the determinism contract. |
| 5 | Radians conversion keeps the legacy constant (`a * 3.14159265359f / 180.f`) | Bit-level closeness for the equivalence tests. |
| 6 | `interpret` does not use `glm/gtx` | gtx needs `GLM_ENABLE_EXPERIMENTAL`; `angle(a,b)` = `acos(clamp(dot,-1,1))` is written out. |
| 7 | Adapter seeds `~` with `params.seed`; `treegen::Error` from `parseModules`/`interpret` is caught in the adapter, logged, and the tree degenerates to its root | An uncaught exception in the render thread would abort the app; legacy never threw. No shipped rule file uses `~`. |
| 8 | Tolerance 1e-4 absolute on `model`/`radius`/`roll`. Two references: verbatim legacy (must match to 1e-4 for productions without `T`: it does, bit-identical, 134 goldens) and legacy + `normalizeTotalRotation` (must match for ALL, including `T`: bit-identical). Verbatim vs treegen on `T` productions is only bounded (< 0.25) and reported: 0.077 max at depth 3, 0.21 (ternary B) / 1.7 (C) at depth 6 | The legacy accumulated rotation shrinks with every `T` (finding 4); that is the single, deliberate numerical difference. |

## Steps

### Step group 1: plan
- [x] **1.1** Write this plan; commit `T05: add plan`.

### Step group 2: Skeleton + analytic tests (TDD)
- [x] **2.1** Add `Skeleton.h`, `Turtle.h` (declaration only), `tests/treegen/turtle_tests.cpp` with the cases below; build `treegen_tests`; expected: link error / FAIL (`interpret` undefined).
- [x] **2.2** Implement `Turtle.cpp`; rebuild; run `treegen_tests -tc="Turtle*"`; expected: PASS.
- [x] **2.3** Commit `T05: add Skeleton and turtle interpreter`.

Analytic cases (names have no brackets/semicolons; epsilon 1e-4, positions read from `model[3]`):
`Turtle: F(1) is one segment ending at (0,1,0)`, `... chain F(1)F(1) ends at (0,2,0)`, `... branch structure: F branch parent indices and childCount`
(`F(1)[+(90)F(1)]F(1)`: parents 0,1,1,1... exact numbers asserted), `... pitch ^(90) turns +Y into +Z`, `... yaw <(90) turns +Y into -X`, `... roll + keeps direction
and accumulates roll`, `... v and minus are the negative rotations`, `... semicolon scales the angle for later default rotations`, `... backtick scales length and radius`,
`... equals scales the radius only`, `... radius and lengthScale persist after F while rotation resets`, `... dollar restores horizontal orientation (front stays horizontal)`,
`... T bends towards the tropism (monotone angle decrease)`, `... tilde parameter is deterministic per seed and bounded`, `... defaults come from params`,
`... unknown characters and their parameters are ignored`, `... Review Focus 6: a literal -1 parameter is honored`, `... Review Focus 1: unbalanced closing bracket throws InterpretError naming the module`,
`... unmatched opening bracket at the end is allowed`, `... empty input gives only the root`, `... pre-first-F rotation is kept (identity start)`, `... 100000 segment chain` (prints ms, asserts only in Release).

### Step group 3: equivalence harness
- [x] **3.1** Write `LegacyTurtleReference.h` (verbatim legacy math; `QuatInit::{Identity, AsCompiledLegacy}`; root roll 0; no random).
- [x] **3.2** Write `turtle_equivalence_tests.cpp`: for >= 5 productions of `tests/golden/solver` (monopodial, sympodial, ternary with `T`, symetric `.l`, `$`-using, depth <= 4) compare all segments; plus the as-compiled-legacy difference test.
- [x] **3.3** Run; record the measured max deviations in Decisions 8; commit `T05: add legacy turtle reference and equivalence tests`.

### Step group 4: legacy wiring
- [x] **4.1** Rewrite `LindenmayerTreeInterpreter.{h,cpp}` as the adapter; `Segment.h` `roll = 0`.
- [x] **4.2** `git diff --stat modernization -- ESEngine/ESTree/Models/Tree/LindenmayerTreeMeshGenerator.h ESEngine/ESTree/Models/Tree/LindenmayerTreeMeshGenerator.cpp` is empty.
- [x] **4.3** Full Debug build (`ESTree` + tests) and smoke launch; commit `T05: replay Skeleton in the legacy interpreter`.

### Step group 5: verification and docs
- [x] **5.1** `ctest` on ninja-msvc Debug; `/W4` clean for `ESEngine/TreeGen`, `tests/treegen`, adapter; Release build of `treegen_tests` for the 100k timing.
- [x] **5.2** Linux Docker run (GCC 13, `-Wall -Wextra -Wpedantic`, engine OFF).
- [x] **5.3** Append D-010 to `docs/DECISIONS.md`; set this plan `Status: review`; commit.

## Review focus covered

- #1 unbalanced `[` / `]` → `Turtle: Review Focus 1: ...` (leading `]`, more `]` than `[`, `]` after a balanced pair; message names the module index).
- #6 literal `-1` → `Turtle: Review Focus 6: ...` (`F(-1)`, `+(-1)`, `T(-1)`).

## Deviations from the brief

- Identity quaternion start instead of "equivalent to the as-compiled legacy" (finding 1, Decision 1): trees using `T` (scene 0's default is ternary B) change versus the current binary. Needs the owner's decision, see REPORT text in the hand-back and D-010.
- "Normalize quaternions on each F" is applied to the accumulated rotation only (finding 4), because normalising the local rotation breaks `$`.
- Equivalence for `T` productions is proven against the legacy reference plus that one normalisation, not against the verbatim legacy (Decision 8).
- `$` degenerate guard (finding 5) and root `roll = 0` (finding 2): both turn NaN / uninitialised memory into defined values.
- Adapter catches `treegen::Error` (Decision 7).

## Blockers

(none)
