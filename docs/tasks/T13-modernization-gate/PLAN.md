# T13 — Modernization gate — Implementation Plan

**Status:** review · **Branch:** `task/T13-modernization-gate` · **Base:** `modernization @ a6b77ac`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** Remove Boost from the build, verify the whole program on a fresh clone, audit the global constraints and leave the docs describing the repository as it is.

**Approach:** Build-file edits first (small, mechanical: nothing in the sources includes Boost since T07/T09), then verify incrementally in the worktree, then once more on a real `git clone` in a short path with `BOOST_LIB` unset, then docs. No functional code changes.

## Files

- Modify: `CMakeLists.txt` — drop `BOOST_LIB`/`BOOST_INCLUDEDIR`/`ESTREE_BOOST_INCLUDE_DIR`, `link_directories`, `BOOST_LIBRARIES`, `BOOST_ALL_NO_LIB`, `bcrypt`
- Modify: `tests/engine/CMakeLists.txt` — it consumed `${BOOST_LIBRARIES}` and `bcrypt` (minimal, forced by the removal; not listed in the brief's owned files)
- Modify: `setup.bat`, `.github/workflows/ci.yml`, `README.md`, `CLAUDE.md`, `tests/README.md`
- Modify: `docs/architecture/overview.md`, `docs/PLAN.md` (statuses + program status), `docs/BACKLOG.md`
- Modify (comments only): `ESEngine/TreeGen/**/Geometry.{h,cpp}` if they mention deleted legacy classes
- Create: `docs/tasks/T13-modernization-gate/SUMMARY.md` (REPORT.md is refused for subagents; the full report is in the hand-off message)

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | `Threads::Threads` comes from `treegen` (PUBLIC), not re-found in the root file | the engine inherits it; nothing else needs it |
| 2 | `bcrypt` removed | `Uuid.h` is `std::mt19937_64` based; nothing else uses CNG |
| 3 | Windows CI job also builds the engine (VS 2022 generator, Debug+Release) if it fits the runner | Boost was the only reason it was off |

## Steps

- [x] 1 CMake/setup.bat/CI/docs Boost removal; reconfigure + build Debug/Release + ctest in the worktree (no `BOOST_LIB`)
- [x] 2 `grep -rni boost` audit, `actionlint` via Docker
- [x] 3 Fresh clone in `C:\temp\estree-gate`, README commands verbatim, treegen_cli, `--help`, scene smoke; Linux Docker engine OFF and ON
- [x] 4 Constraint audit + `/W4` warning count
- [x] 5 Docs refresh, SUMMARY.md, PLAN statuses, BACKLOG

## Review focus covered

All six lines are audited in SUMMARY.md / the hand-off report with the owning test.

## Deviations from the brief

- `tests/engine/CMakeLists.txt` edited (it referenced `BOOST_LIBRARIES`).
- Two Linux build breaks found by the fresh-clone check and fixed minimally: `LindenmayerTreeFactory.h` included `Rigidbody.h` (file is `RigidBody.h`), and GLFW 3.4 needs `wayland-scanner` unless `GLFW_BUILD_WAYLAND` is OFF (D-020). `setup.bat` uses `mklink /j` (`/d` needs elevation).
- `SUMMARY.md` could not be written either (the harness refused it); its full text is in the hand-off message and must be created by the orchestrator (README, PLAN.md and CLAUDE.md link to it).
- `REPORT.md` is not written (harness refuses it for subagents); `SUMMARY.md` plus the hand-off message replace it.

## Blockers

(none)
