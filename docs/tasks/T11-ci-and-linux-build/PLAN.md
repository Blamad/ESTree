# T11 — CI (Windows + Linux) and headless Linux build — Implementation Plan

**Status:** in-progress · **Branch:** `task/T11-ci-and-linux-build` · **Base:** `modernization @ fc539f2`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** GitHub Actions that build and test on `windows-latest` (MSVC) and `ubuntu-latest` (GCC) on every push/PR, and prove the non-engine targets build headless on Linux.

**Approach:** Add the CMake option `ESTREE_BUILD_ENGINE` (ON on Windows, OFF elsewhere) that gates the engine executable and its heavy dependencies (GLFW, Bullet, FreeType, prebuilt Boost libs). `BOOST_LIB` is required only when it is ON; a `BOOST_INCLUDEDIR` override supplies Boost headers for header-only users (tests) without the Windows libs. The Windows job cannot build the engine on GitHub's runner (no vc140 Boost libs), so it also runs with the engine OFF; the full engine build stays a local/owner check until T07/T13 drop Boost. The Linux engine build is attempted in Docker, time-boxed.

## Files

- Create: `.github/workflows/ci.yml`
- Modify: `CMakeLists.txt` (option + Boost gating blocks only)
- Modify: `tests/engine/CMakeLists.txt` (early `return()` when the engine is OFF; those tests link Bullet/Boost libs)
- Modify: `README.md` (badge), `tests/README.md` (reproduce CI locally)
- Possibly: minimal Linux portability edits under `ESEngine/` (listed in the final report)

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | `tests/engine` gated inside its own CMakeLists, not in `tests/CMakeLists.txt` | `tests/CMakeLists.txt` is edited concurrently by T02 |
| 2 | `add_definitions(... BOOST_ALL_NO_LIB ...)` and Boost include dir stay global | Tests (T02) inherit them; no behavior change for engine builds |
| 3 | Windows CI uses engine OFF + header-only Boost via `BOOST_INCLUDEDIR` | No vc140 libs on runners |

## Steps

- [x] Setup worktree, junction Vendor
- [ ] CMake option + gating; verify Windows engine ON (unchanged) and OFF (with/without BOOST_LIB)
- [ ] Workflow + actionlint
- [ ] Docker Linux run (engine OFF), then engine ON attempt
- [ ] Docs, report

## Deviations from the brief

(none yet)

## Blockers

(none yet)
