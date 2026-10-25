# ESTree Modernization — Master Plan

> **For agentic workers:** REQUIRED SUB-SKILL: use `superpowers:subagent-driven-development`
> (recommended) or `superpowers:executing-plans`. Each task has its own directory under
> `docs/tasks/`; take **one task**, read its `BRIEF.md`, write its `PLAN.md` first, finish with
> `REPORT.md`. Steps inside task plans use checkbox (`- [ ]`) syntax.

**Goal:** Modernize the ESTree engine in place (build, language level, dependencies, ownership,
tests) and isolate the Lindenmayer tree-generation logic into a standalone, deterministic,
engine-independent module (`treegen`) so it can later be split out and reused.

**Architecture:** Strangler migration. A new pure-C++17 static library `treegen`
(`ESEngine/TreeGen/`) is built beside the legacy `Lindenmayer*` classes; the legacy classes are
turned into thin adapters, then deleted in T07. The engine (`Engine/`, `GraphicEngine/`) is kept
and only cleaned up, not redesigned. Pipeline inside `treegen`:
`rule file → TreeParams → rewrite → ModuleString → interpret → Skeleton → geometry → TreeModel`.

**Tech Stack:** C++17, CMake ≥ 3.20 (presets), MSVC v142+ (primary), GLFW/glad/glm/Bullet/FreeType/
rapidjson/stb (existing submodules), doctest + CTest (new), GitHub Actions (T11).

**Spec:** the user's request of 2026-10-25 (keep the current engine; prepare the generation logic so
the modules can be separated later; plan decomposed into tasks that subagents can take with
context) plus the analysis in `docs/architecture/overview.md`. Target contract:
`docs/architecture/treegen-design.md`.

## Global Constraints

Every task's requirements implicitly include these.

- Language level C++17 for all new code; the engine must keep building with MSVC v142 (VS 2019 Build Tools) on x64 Debug and Release.
- `treegen` may depend only on the C++ standard library, `glm`, and `rapidjson`. No `Engine/`, `GraphicEngine/`, OpenGL, Boost, Bullet, FreeType, stb includes. Enforced by a CTest (T03).
- No new global mutable state and no `time(0)` seeding in `treegen`; randomness comes from an explicit `uint32_t seed` and `std::mt19937`.
- Behavior-preserving: for the rule files in `Resources/LindenmayerRules/*.json` the generated production strings of deterministic rules must stay identical to the pre-refactor output (golden tests from T02), unless a brief explicitly lists an intentional change.
- Rule-file formats stay backward compatible: all existing `*.json` and `legacy/*.l` files keep loading.
- Tests are headless: no OpenGL context, no window, no network, no files outside the repo (resources are read relative to the repo root via a CMake-provided define).
- No new compiler warnings at `/W4` relative to the T00 baseline count.
- Windows is the primary platform; do not add Windows-only code to `treegen`.
- Do not edit `Vendor/` contents; only T12 moves submodule pins.
- Never commit to `master`; one branch per task: `task/Txx-slug`.

## Review Focus

Input classes/failure modes the tasks' main tests would not hit by accident. Each line has a
test in the task that owns the code (named in brackets).

1. **Unbalanced `[` / `]` in a production** (`]` first, or more `]` than `[`): today `std::stack::top()` on an empty stack is UB/crash. Expected: typed `treegen::InterpretError`, no crash. [T05]
2. **Missing file / malformed JSON / missing `axiom` or `rules`**: today `document["parameters"]` on a failed parse crashes. Expected: `treegen::LoadError` with file path and reason. [T03]
3. **Exponential growth** (e.g. depth 14 on a 3-way branching rule): today memory just explodes. Expected: `TreeParams`/`RewriteOptions` limit (`maxModules`, default 5 000 000) → `treegen::RewriteError`, not OOM. [T04]
4. **Parameter arity mismatch** (`A(1)` matched by a rule declaring `A(l,w)`, or the other way round): today indexes past `values`. Expected: `RewriteError` naming rule and symbol. [T04]
5. **Degenerate geometry** (radius 0, length 0, NaN from `0/0` or `x/0` in an expression, parent/child same position): today produces NaN normals. Expected: no NaN/Inf in `MeshData`; zero-radius segments are skipped or clamped to a documented epsilon. [T04 for NaN in expressions, T06 for geometry]
6. A literal `-1` parameter (`+(−1)` style or `F(-1)`) must be honored, not treated as "absent". [T05]

## Task table

Status is maintained by the orchestrating session only. Legend: `todo` · `in-progress` · `review` · `done`.

| ID | Task | Depends on | Size | Status |
|---|---|---|---|---|
| [T00](tasks/T00-build-baseline/BRIEF.md) | Reproducible build baseline (CMake presets, VS2019 toolset, C++17) | — | M | done |
| [T01](tasks/T01-test-harness/BRIEF.md) | Test harness (doctest + CTest) | T00 | S | done |
| [T02](tasks/T02-characterization-tests/BRIEF.md) | Characterization (golden) tests of the legacy solver and params loader | T01 | M | done |
| [T03](tasks/T03-treegen-scaffold-params/BRIEF.md) | `treegen` scaffold: CMake target, boundary check, `Module`, `TreeParams`, loaders | T02 | M | done |
| [T04](tasks/T04-treegen-rewriter/BRIEF.md) | `treegen` rewriter: expression evaluator, seeded stochastic rewriting | T03 | L | done |
| [T05](tasks/T05-treegen-turtle-skeleton/BRIEF.md) | `treegen` turtle interpreter → `Skeleton` | T03 | L | done |
| [T06](tasks/T06-treegen-geometry/BRIEF.md) | `treegen` geometry: `Skeleton` → `MeshData` + leaf transforms | T05 | L | done |
| [T07](tasks/T07-engine-adapter-switchover/BRIEF.md) | Engine adapter switch-over; delete legacy `Lindenmayer*` pipeline | T04, T06 | M | done (owner checks 5-8 unconfirmed, see SUMMARY.md) |
| [T08](tasks/T08-treegen-cli-obj-export/BRIEF.md) | Headless `treegen_cli` + OBJ writer | T04, T06 | S | done |
| [T09](tasks/T09-drop-boost-engine/BRIEF.md) | Remove Boost from engine code outside `Models/Tree` | T00 | M | done |
| [T10](tasks/T10-ownership-and-sanitizers/BRIEF.md) | Ownership fixes, casts, ASan/UBSan pass over the engine | T00 | M | done |
| [T11](tasks/T11-ci-and-linux-build/BRIEF.md) | GitHub Actions CI (Windows + Linux), headless tests | T01 | M | done (CI not yet run on GitHub) |
| [T12](tasks/T12-dependency-refresh/BRIEF.md) | Bump submodule pins (GLFW, glm, Bullet, FreeType, stb, rapidjson) | T07, T09, T10 | M | done |
| [T13](tasks/T13-modernization-gate/BRIEF.md) | Final gate: Boost removed from CMake, docs, README, acceptance run | all | S | done |

Size: S ≈ half a day, M ≈ 1–2 days, L ≈ 2–4 days of focused agent work.

## Program status (T13, 2026-10-25)

All tasks T00-T12 are merged into `modernization`; T13 is in review. The build is Boost-free, `treegen` is a standalone library with a
headless CLI, 232 headless CTest entries pass on Windows (MSVC) and Linux (GCC 13), and the legacy `Lindenmayer*` pipeline is gone. Open items:
the CI workflow has never run on GitHub, and the owner's visual/behavioural checklist (consolidated, with exact steps, in
[`tasks/T13-modernization-gate/SUMMARY.md`](tasks/T13-modernization-gate/SUMMARY.md)) is only partly confirmed. `modernization` is not merged into `master`;
that is the owner's decision.

## Dependency graph

```
T00 ──┬─ T01 ──┬─ T02 ── T03 ──┬─ T04 ────────────┬─ T07 ──┐
      │        │               └─ T05 ── T06 ──┬──┤        │
      │        └─ T11                          │  └─ T08   ├─ T12 ── T13
      ├─ T09 ──────────────────────────────────┼───────────┤
      └─ T10 ──────────────────────────────────┴───────────┘
```

## Parallelization waves

Run tasks of one wave in parallel (separate git worktrees); start the next wave when the listed
predecessors are merged to the integration branch.

| Wave | Tasks | Notes |
|---|---|---|
| 0 | T00 | Everything else needs a working build |
| 1 | T01 ∥ T09 ∥ T10 | Disjoint files (see ownership matrix) |
| 2 | T02 ∥ T11 | T11 only needs the harness |
| 3 | T03 | Creates the `treegen` module all later tasks extend |
| 4 | T04 ∥ T05 | Both only need `Module`/`TreeParams` from T03 |
| 5 | T06 | Needs `Skeleton` from T05 |
| 6 | T07 ∥ T08 | T07 needs T04+T06; T08 needs T04+T06 (read-only on treegen) |
| 7 | T12 | Touches CMake + `Vendor/` pins → run alone, after code tasks |
| 8 | T13 | Gate |

## File ownership matrix (conflict avoidance)

A task edits only the paths it owns. Anything else → write it into your `PLAN.md` first and keep the change minimal.

| Task | Owns |
|---|---|
| T00 | `CMakeLists.txt`, `CMakePresets.json`, `setup.bat`, `README.md` (build section), `CLAUDE.md` (build section), `CLAUDE.local.md` |
| T01 | `tests/CMakeLists.txt`, `tests/smoke/**`, test option in root `CMakeLists.txt` |
| T02 | `tests/legacy_characterization/**`, `tests/golden/**` |
| T03 | `ESEngine/TreeGen/{CMakeLists.txt,include/**,src/**}` (scaffold, `Module*`, `TreeParams*`, `Loader*`, `Rng*`), `tests/treegen/{boundary,params}*` |
| T04 | `ESEngine/TreeGen/.../Rewriter*`, `Expression*`, `tests/treegen/rewriter*` |
| T05 | `ESEngine/TreeGen/.../Turtle*`, `Skeleton*`, `tests/treegen/turtle*` |
| T06 | `ESEngine/TreeGen/.../Geometry*`, `MeshData*`, `TreeGenerator*`, `tests/treegen/geometry*` |
| T07 | `ESEngine/ESTree/Models/Tree/**`, `ESEngine/ESTree/Console/ConsoleTreeCommand.*`, tree scenes |
| T08 | `tools/treegen_cli/**`, `ESEngine/TreeGen/.../ObjWriter*`, `tests/treegen/obj*` |
| T09 | `ESEngine/Engine/**` and `ESEngine/GraphicEngine/**` lines using Boost, `ESEngine/ESTree/ESTree.cpp`, `Models/{Grass,ModelExporter}.*`. **Not** `Models/Tree/**`, **not** `CMakeLists.txt` |
| T10 | `ESEngine/Engine/{Scene,GameObject}/**`, `Manager/*Manager.*` (ownership/casts only) |
| T10 (cont.) | `ESTREE_SANITIZE` option + `vs2019-asan` preset, `tests/engine/**` |
| T11 | `.github/workflows/**`, `ESTREE_BUILD_ENGINE` option block in `CMakeLists.txt`, CI notes/badge in `README.md` |
| T12 | `Vendor/**` pins, `.gitmodules`, CMake changes forced by API breaks |
| T13 | `CMakeLists.txt` (Boost removal), `README.md`, `docs/**` status sections |

## Definition of done (every task)

1. Acceptance criteria in the `BRIEF.md` are met and each is backed by a command + output in `REPORT.md`.
2. `cmake --build` Debug and Release succeed; `ctest` passes (once T01 exists).
3. The `treegen` boundary test passes (once T03 exists).
4. `REPORT.md` lists deviations, follow-ups, and anything that needs human verification.
5. Branch is rebased on the integration branch, commits are task-prefixed, no stray files.

## Risks

- **No GL in agent sessions** → rendering regressions are invisible to agents. Mitigation: pure
  unit tests for everything in `treegen`; T07 ends with an explicit human-verification checklist.
- **Characterization gaps**: stochastic rules can't be golden-tested before T04 introduces seeds.
  Mitigation: T02 locks deterministic rules + statistical properties; T04 re-bases stochastic
  expectations on seeded output and records the change in `DECISIONS.md`.
- **Merge conflicts in CMake**: T03 uses `GLOB_RECURSE ... CONFIGURE_DEPENDS` inside `treegen` so
  later tasks only add files.
- **Toolchain drift**: this machine has only VS Build Tools 2019 and "18"; T00 pins what works.
