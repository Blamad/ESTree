# T00 — Reproducible build baseline

**Depends on:** — · **Unblocks:** everything · **Size:** M · **Branch:** `task/T00-build-baseline`

## Goal

Make the *unchanged* engine configure, build (Debug + Release) and start (`ESTree.exe --help`)
on this machine with a single documented command sequence, using modern CMake and the installed
MSVC toolset, and record a baseline (warning count, build time) that later tasks are measured against.

## Why

Today the build is hard-wired to "Visual Studio 14 2015 Win64", CMake 3.0, Boost 1.72 `vc140`
library names, and a `setup.bat` that deletes `build/`. The machine only has VS 2019 and VS "18"
Build Tools, and the old `build/` cache points to a CMake that no longer exists. Nothing else can
be verified until this works.

## Context to read

- `CMakeLists.txt`, `setup.bat`, `README.md` (Project setup section), `.gitmodules`
- `CLAUDE.local.md` (tool locations on this machine — verify, correct)
- `docs/architecture/overview.md` → *Build*
- `ESEngine/ESTree/ESTree.cpp` (uses `boost::program_options`), grep for `#include <GLM\` / `<GLM/`

## Scope

In:
1. Raise `cmake_minimum_required` to 3.20; set `CMAKE_CXX_STANDARD 17`, `CMAKE_CXX_STANDARD_REQUIRED ON`, `CMAKE_CXX_EXTENSIONS OFF`; remove the `-std=c++11` flag line.
2. Add `CMakePresets.json` with configure presets `vs2019` (generator "Visual Studio 16 2019", `-A x64`) and, if a VS "18"/2022 generator is installed and works, `vs-latest`; build presets `debug`/`release`; binary dir `build/<preset>`. (Keep `build/` git-ignored.)
2b. Toolchain: see the *Tool audit* in `CLAUDE.local.md`. CMake 4.4.4 and Ninja 1.13.2 are installed under `C:\Users\Blamad\tools` (not on PATH; see `CLAUDE.local.md` for the PATH snippet). Provide presets `vs2019` (generator "Visual Studio 16 2019") and `ninja-msvc` (Ninja Multi-Config, MSVC 14.51 from VS "18" via `vcvars64.bat`; also try the `Visual Studio 18 2026` generator as `vs2026`), verify each that builds, and record which preset is the recommended default. Keep `CMakePresets.json` `version` compatible with CMake 3.20 only if `vs2019` is meant to work with the bundled one; otherwise document that the repo requires CMake ≥ 3.25 and say so in `README.md`.
3. Boost handling: keep Boost for now (removed by T09/T13). Replace the hard-coded `libboost_*-vc140-*-1_72` names with a `find_package(Boost ...)`-free but *configurable* variable (`BOOST_ROOT`/`%BOOST_LIB%`), verified to link against `D:\Dev\Boost` with the v142 toolset. If prebuilt `vc140` libs are ABI-incompatible with v142, document the fix that works (e.g. use `D:\Dev\boost_1_72_0` headers + build the two needed libs, or auto-link header-only parts) — pick the least invasive that builds.
4. Fix portability nits that block the build: `#include <GLM\...>` backslash/uppercase includes → `<glm/...>`.
5. Make `setup.bat` use the presets and stop deleting arbitrary dirs (`del /s /q build` → only if the user passes `--clean`), and create the `Resources` link for the chosen binary dir (`build/<preset>/Resources`).
6. Capture baseline numbers in `docs/tasks/T00-build-baseline/notes/baseline.md`: cmake/compiler versions, `/W4` warning count for the `ESTree` target (Debug), build wall time, `ESTree.exe --help` output.
7. Update `CLAUDE.md` (Build and test section, remove "provisional"), `CLAUDE.local.md` (verified paths), and the `README.md` *Project setup* section.

Out of scope: removing Boost from source, dependency bumps, tests, CI, behavior changes, renaming files.

## Owned files

`CMakeLists.txt`, `CMakePresets.json`, `setup.bat`, build sections of `README.md` / `CLAUDE.md`, `CLAUDE.local.md`, `ESEngine/**` only for the include-case fix.

## Acceptance criteria

1. From a clean checkout with submodules: `cmake --preset vs2019` then `cmake --build --preset debug` and `--preset release` succeed (exit code 0).
2. `build/<preset>/Debug/ESTree.exe --help` prints the options and exits 0 (run from the build dir with `Resources` linked).
3. `grep -rn "GLM\\\\" ESEngine` finds no backslash includes; `grep -rn "<GLM/" ESEngine` finds none.
4. `notes/baseline.md` exists with: toolchain versions, warning count, build time, `--help` output.
5. `CLAUDE.md` no longer says "Provisional"; the commands in it are the ones you ran.
6. Engine behavior untouched: `git diff --stat` shows only build files, docs, and include-case edits.

## Verification

Automated: the commands above. **Needs human verification:** launching the engine window
(`ESTree.exe --scene 0`) on a machine with a GPU — list the steps in `REPORT.md`; do not claim it works if you could not open a GL context.

## Notes / risks

- Bullet is large; first full build may take many minutes — run builds with `run_in_background`/timeouts accordingly (`cmake --build ... --parallel`).
- `glad` is compiled from `Vendor/glad/src/glad.c`; make sure submodules are initialised (`git submodule update --init --recursive`).
- freetype may require `-DFT_DISABLE_*` options on newer toolsets; only touch if it fails.
- If a vendored library fails with the new toolset and the fix needs editing `Vendor/`, **stop** and report (T12 owns that), unless a CMake option solves it.
