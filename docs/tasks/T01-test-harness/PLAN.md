# T01 — Test harness (doctest + CTest) — Implementation Plan

**Status:** done · **Branch:** `task/T01-test-harness` · **Base:** `modernization @ 3e1cbf1`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** A `tests/` tree with a doctest runner wired into CTest, two passing smoke tests, and a documented way to add test executables.

**Approach:** Fetch doctest v2.4.11 with `FetchContent` inside `tests/CMakeLists.txt` (so `ESTREE_BUILD_TESTS=OFF` never fetches). `estree_add_test` wraps `add_executable` + `add_test` and defines `ESTREE_REPO_ROOT`. The root CMakeLists gets one small block appended at the end.

## Files

- Create: `tests/CMakeLists.txt` — FetchContent, `estree_add_test`
- Create: `tests/smoke/smoke_tests.cpp` — two TEST_CASEs
- Create: `tests/README.md`
- Modify: root `CMakeLists.txt` — append option + `include(CTest)` + `add_subdirectory(tests)`

## Interfaces

- Produces: `estree_add_test(<name> SOURCES <files...> [LIBS <targets...>])`; compile define `ESTREE_REPO_ROOT` (string literal, forward slashes).

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | Block appended at end of root CMakeLists | Minimizes merge conflicts with T09/T10 |
| 2 | One `add_test` per executable, working dir = repo root | Brief allows it; simple |
| 3 | `include(CTest)` (provides `BUILD_TESTING` + enable_testing) | As the brief says |

## Steps

- [x] 1 Write smoke test + CMake; configure, build, run `ctest` -> expect 2 passed
- [x] 2 Break an assertion -> ctest fails; revert
- [x] 3 Configure with `-DESTREE_BUILD_TESTS=OFF` in a separate build dir; confirm no doctest fetch
- [x] 4 Check link line of smoke test has no glfw/glad/Bullet/FreeType
- [x] 5 README, REPORT, status `review`

## Review focus covered

Headless tests, repo-relative resources (via `ESTREE_REPO_ROOT`).

## Deviations from the brief

- `doctest_discover_tests` instead of plain `add_test`; `SYSTEM` on the FetchContent declare (silences doctest C5285 under /W4).

## Blockers

(none)
