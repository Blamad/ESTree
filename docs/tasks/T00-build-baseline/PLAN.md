# T00 — Reproducible build baseline — Implementation Plan

**Status:** in-progress · **Branch:** `task/T00-build-baseline` · **Base:** `modernization @ ec45198`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** the unchanged engine configures, builds (Debug+Release) and starts (`--help`) from presets with CMake 4.4.4 on this machine; baseline numbers recorded.

**Approach:** raise CMake min to 3.21 (presets v3), set C++17, add presets (`vs2019`, `vs2026`, `ninja-msvc`), make Boost configurable and explicit (all needed `vc140` static libs + `BOOST_ALL_NO_LIB` so header auto-link doesn't look for non-existent `vc142/vc145` names), pass `CMAKE_POLICY_VERSION_MINIMUM=3.5` for vendored projects with ancient `cmake_minimum_required`, lowercase `GLM` includes, update `setup.bat`/docs.

## Files

- Modify: `CMakeLists.txt`, `setup.bat`, `README.md` (setup section), `CLAUDE.md` (build section), `CLAUDE.local.md`, 15 `#include <GLM...>` lines in `ESEngine/**`
- Create: `CMakePresets.json`, `docs/tasks/T00-build-baseline/notes/baseline.md`, `REPORT.md`

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | Repo requires CMake ≥ 3.21 (presets schema v3); bundled VS2019 CMake 3.20 is unsupported | owner installed CMake 4.4.4; 3.20 only knows presets v2 |
| 2 | `CMAKE_POLICY_VERSION_MINIMUM=3.5` in presets, not edits in `Vendor/` | CMake 4 rejects `cmake_minimum_required(<3.5)` in Bullet/FreeType; Vendor is read-only until T12 |
| 3 | Link all needed Boost libs explicitly, disable auto-link | auto-link derives toolset name from `_MSC_VER` (vc142+) but only `vc140` libs exist |

## Steps

- [ ] 1. Fix `GLM` include case (sed), grep proves none left
- [ ] 2. CMake: min 3.21, C++17, Boost vars, presets file
- [ ] 3. Configure `vs2019`; fix until it succeeds
- [ ] 4. Build Debug, then Release; record warnings/time
- [ ] 5. Run `ESTree.exe --help`
- [ ] 6. Try `ninja-msvc` and `vs2026`; keep what works
- [ ] 7. `setup.bat`, README, CLAUDE.md, CLAUDE.local.md, `notes/baseline.md`, `REPORT.md`
- [ ] 8. Commit per logical group (`T00: ...`)

## Deviations from the brief

(none yet)

## Blockers

(none yet)
