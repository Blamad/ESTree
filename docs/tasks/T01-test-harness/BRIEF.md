# T01 — Test harness (doctest + CTest)

**Depends on:** T00 · **Unblocks:** T02, T11 · **Size:** S · **Branch:** `task/T01-test-harness`

## Goal

A `tests/` tree with a doctest-based runner wired into CTest, one smoke test passing, and a
documented way to add test executables — so that every later task can write tests first.

## Context to read

- `docs/DECISIONS.md` D-002 (doctest via FetchContent, tag `v2.4.11`, option `ESTREE_BUILD_TESTS`)
- Root `CMakeLists.txt` (after T00), `docs/PLAN.md` → Global Constraints (headless tests, repo-relative resources)

## Scope

In:
1. `option(ESTREE_BUILD_TESTS "Build unit tests" ON)`; when ON: `include(CTest)`/`enable_testing()`, `add_subdirectory(tests)`.
2. `tests/CMakeLists.txt` with a helper `estree_add_test(<name> SOURCES ... LIBS ...)` that builds an executable linking `doctest::doctest`, registers it with `add_test`, and sets `ESTREE_REPO_ROOT` as a compile definition (absolute repo root, used by tests to find `Resources/`).
3. doctest fetched via `FetchContent` (pinned tag, `FETCHCONTENT_QUIET OFF`); if the fetch is impossible offline, vendor the single header into `Vendor/doctest/` *only if* that is the sole way — record in `DECISIONS.md`.
4. `tests/smoke/smoke_tests.cpp`: one `TEST_CASE("harness runs")` and one that reads `Resources/LindenmayerRules/test.json` via `ESTREE_REPO_ROOT` and asserts the file exists and is non-empty.
5. A `tests/README.md` (10–20 lines): how to run all tests, one test, add a new executable, naming rules (`<area>_tests.cpp`, `TEST_CASE("<unit>: <behavior>")`).

Out of scope: any test of engine/legacy code (T02), CI (T11), GL-dependent tests (never).

## Owned files

`tests/CMakeLists.txt`, `tests/README.md`, `tests/smoke/**`, the test option block in root `CMakeLists.txt`.

## Acceptance criteria

1. `cmake --preset vs2019 && cmake --build --preset debug && ctest --test-dir build/vs2019 -C Debug --output-on-failure` → 100% passed, ≥ 2 tests.
2. Breaking an assertion makes `ctest` fail (show it once in `REPORT.md`, then revert).
3. `-DESTREE_BUILD_TESTS=OFF` configures and builds without fetching doctest.
4. The test executable does not link GLFW/glad/Bullet/FreeType.

## Verification

Fully automated; no human verification needed.

## Notes

Use `doctest_discover_tests` if the doctest CMake module is available, otherwise one `add_test` per executable is fine.
