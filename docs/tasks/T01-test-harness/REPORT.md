# T01 — Test harness (doctest + CTest) — Report

**Result:** done · **Branch:** `task/T01-test-harness` · **Final commit:** `686c461` (merged into `modernization` as `6f19592`)

The implementing subagent could not write this file (subagents may not write reports); the
orchestrator wrote it from the agent's hand-back and **re-verified the result in the main checkout**.

## What changed

- Root `CMakeLists.txt`: 6 lines appended — `option(ESTREE_BUILD_TESTS ... ON)`, `include(CTest)`, `add_subdirectory(tests)`.
- `tests/CMakeLists.txt`: doctest v2.4.11 via `FetchContent` (`SYSTEM`, to keep its warnings out of `/W4`), helper
  `estree_add_test(<name> SOURCES ... LIBS ...)` that defines `ESTREE_REPO_ROOT`, links `doctest::doctest`, and registers
  tests with `doctest_discover_tests` (working directory = repo root).
- `tests/smoke/smoke_tests.cpp` (2 cases: harness runs; finds `Resources/LindenmayerRules/test.json` via `ESTREE_REPO_ROOT`), `tests/README.md`.

## Acceptance criteria

| Criterion | Evidence |
|---|---|
| 1. Configure, build, ctest green, ≥ 2 tests | Orchestrator re-run on merged `modernization` (ninja-msvc, Debug): `cmake --preset ninja-msvc` OK; `[2/2] Linking CXX executable tests\Debug\smoke_tests.exe`; `100% tests passed out of 2` |
| 2. Broken assertion fails ctest | Agent: `1 + 1 == 3` → `[doctest] Status: FAILURE!`, `50% tests passed, 1 tests failed out of 2`, ctest exit 8; reverted (agent-reported, not re-run) |
| 3. `-DESTREE_BUILD_TESTS=OFF` works without fetching doctest | Agent: configure succeeded, no `_deps`/`tests` dir (configure only; agent-reported) |
| 4. No GLFW/glad/Bullet/FreeType linked into tests | Agent: `smoke_tests.exe` link line has only system libs (agent-reported) |

## Deviations

- `doctest_discover_tests` (allowed by the brief): ctest lists the two TEST_CASEs individually.
- `SYSTEM` on `FetchContent_Declare` (silences doctest's C5285 at `/W4`).
- Not run: `vs2019` preset and Release (untouched by this change).

## Interface for later tasks

`estree_add_test(<name> SOURCES <files...> [LIBS <targets...>])` in `tests/CMakeLists.txt`; add one executable per area
(`DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` in exactly one source). `ESTREE_REPO_ROOT` is an absolute path without trailing slash.

## Follow-ups

- First configure needs network (doctest fetch); T11 should cache `_deps`.
- Worktrees: long paths under `.claude/worktrees/...` trigger a harmless CMake object-path warning for Bullet.
- `scripts\msvc-env.cmd` prints "vswhere.exe is not recognized" but works (cosmetic; look at it in a later cleanup).
- In worktrees whose `Vendor/*` are junctions, plain `git status` fails; use `git status --ignore-submodules=all`.

## Needs human verification

Nothing.
