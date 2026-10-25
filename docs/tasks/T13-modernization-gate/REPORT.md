# T13 — Modernization gate — Report

**Result:** done · **Branch:** `task/T13-modernization-gate` · **Final commit:** `91c776c` (merged into `modernization` as `d4dde3c`)

The implementing subagent could not write this file or `SUMMARY.md`; the orchestrator wrote both from the agent's hand-back and
**re-verified the merged tree** on a clean build dir with `BOOST_LIB` unset (marked *verified*). Executive summary and the owner checklist: [SUMMARY.md](SUMMARY.md).

## What changed

- **Boost removed from the build:** root `CMakeLists.txt` lost every Boost line and `bcrypt` (`Uuid.h` is `std::mt19937_64`); `find_package(Threads)` stays. `tests/engine/CMakeLists.txt` and `setup.bat` lost their Boost parts. `CMakePresets.json` never mentioned Boost.
- **CI (`ci.yml`):** no Boost download/cache/`libboost-*` packages; the Windows job now builds the engine (Debug + Release); Linux keeps engine ON and OFF. `actionlint` (Docker) exit 0; the workflow has never run on GitHub.
- **Docs:** README (new build instructions, `treegen_cli`, project description), `CLAUDE.md`, `tests/README.md`, `overview.md` (final state), `docs/PLAN.md` ("Program status" section), `BACKLOG.md`, D-020. Comment-only edits in `Geometry.h/.cpp`.
- **Three fixes found by the fresh-clone/Linux runs (none caused by the Boost removal):** GLFW 3.4 fails to configure on Linux without `wayland-scanner` → `option(GLFW_BUILD_WAYLAND OFF)` before `add_subdirectory(Vendor/glfw)` (T12 had tested Linux only with the engine off); `LindenmayerTreeFactory.h` included `Rigidbody.h` for the file `RigidBody.h` (case-insensitive file systems only); `setup.bat` used `mklink /d` (needs privilege) → `mklink /j`.

## Evidence

| Check | Result |
|---|---|
| Fresh `git clone` at `C:\temp\estree-gate` (agent), 7 submodules borrowed with `--reference`, all pins resolved, `BOOST_LIB` unset | README commands on `vs2019`: Debug 2m28s, Release done at 5m34s, `ctest` Debug and Release `100% tests passed out of 232`; `ninja-msvc` clean Debug 1m23s, 232/232 |
| *verified* on `modernization` tip (clean `build/final`, `BOOST_LIB` unset) | clean Debug build exit 0 in 1m34s; `100% tests passed out of 232`; scenes 0, 2, 4 alive after 14 s, closed cleanly (`Quitting..`), trees generated (1 / 4 / 16), no crash events; `treegen_cli parametricMonopodialTreeA.json --seed 1` → `segments=256 vertices=4608 triangles=4096 leaves=128` |
| Linux Docker (Ubuntu 24.04, GCC 13.3, CMake 3.28) | *verified* engine OFF `100% tests passed, 0 tests failed out of 219`; agent: engine ON with no `libboost` package: build exit 0, `ESTree` linked, 232/232 |
| `/W4` unique project warnings (Vendor excluded) | agent: 109 on a clean `ninja-msvc` Debug build, identical set to `modernization` before T13 (method of `T00/notes/baseline.md`); orchestrator's script: 105 on the same state — compare each method only with itself |
| Boost grep (cpp/h/CMakeLists/json/md/yml/bat/cmake, excluding Vendor/build/.claude) | no source, CMake, preset, script or workflow hit remains; the rest are the boundary check (it forbids `boost/`), "no Boost" statements and history in `DECISIONS.md`/`docs/tasks` |

## Constraint audit (`docs/PLAN.md`)

| Constraint | Proof |
|---|---|
| C++17; MSVC v142 x64 Debug and Release | `CMAKE_CXX_STANDARD 17`; vs2019 (14.29) Debug + Release built |
| `treegen` depends only on std, glm, rapidjson | `treegen_boundary` + `treegen_boundary_selftest`; its only link is `Threads::Threads` |
| No global mutable state, no `time(0)` | grep of `ESEngine/TreeGen` for `time(`, `rand`, `random_device`, `thread_local`, `extern`: none; `Rng` tests; `generateTree` takes a seed |
| Goldens unchanged except logged flips | `rewriter_golden_tests`, `loader_golden_tests`, `golden_data_tests` |
| Rule formats keep loading | `loader_golden_tests` loads all 43 shipped files |
| Tests headless | no GLFW/glad in tests. **Partly unmet:** `engine_physics` and `engine_gameobject` link Bullet (documented T10 exception) and `obj_tests.cpp` writes to the OS temp dir (BACKLOG) |
| No new `/W4` warnings | identical warning set before/after T13 |
| No Windows-only code in `treegen` | grep for `windows.h`, `_WIN32`: none |
| `Vendor/` untouched except T12 | `git diff master modernization -- Vendor .gitmodules`: six submodule pins only |
| Never commit to `master` | `master` is still `c30a23c` |
| Review Focus 1 / 2 / 3 / 4 / 5 / 6 | `Turtle: Review Focus 1`; 14 `Review Focus 2:` cases in `loader_tests.cpp`; 2 `Review Focus 3`, 5 `Review Focus 4`, `Review Focus 5` (0/0, x/0, overflow; geometry finite + NaN scan over every shipped file) in rewriter/geometry tests; `Turtle: Review Focus 6` (literal `-1`) |

## Follow-ups

All in `docs/BACKLOG.md`: CI never run on GitHub (and the Windows job builds all of Bullet — restrict it to the three libraries), warning cleanup, `CommandLine.h` unit test, GLFW 3.5.1, OBJ vertex welding, `obj_tests` temp dir, old `build/` housekeeping.
