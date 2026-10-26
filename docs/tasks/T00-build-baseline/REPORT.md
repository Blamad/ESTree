# T00 — Reproducible build baseline — Report

**Result:** done (one preset, `vs2026`, does not work on this machine — see Deviations) · **Branch:** `task/T00-build-baseline` · **Final commit:** see `git log` (commits prefixed `T00:`)

## What changed

- `CMakeLists.txt`: min 3.21, C++17, `BOOST_LIB` env/cache variable, explicit vc140 Boost libs + `BOOST_ALL_NO_LIB`, `bcrypt` on Windows, `-std=c++11` removed.
- `CMakePresets.json` (new): `vs2019`, `vs2026`, `ninja-msvc` + debug/release build presets; passes `CMAKE_POLICY_VERSION_MINIMUM=3.5` for vendored projects.
- `scripts/msvc-env.cmd` (new): runs a command inside the MSVC x64 environment (for `ninja-msvc`).
- `setup.bat`: preset-based, no unconditional `del /s /q build`; `--clean` only removes `build/<preset>`; links `Resources`.
- 17 source files: include lines normalized (`<GLM\…>` → `<glm/…>`, backslashes → `/`); `Engine/Core/Texture.h`: `#include <string>`.
- Docs: `README.md` setup section, `CLAUDE.md` build section + known-defects line, `docs/DECISIONS.md` D-005, `notes/baseline.md`; `CLAUDE.local.md` (git-ignored) verified recipe.

## Acceptance criteria

| Criterion (from BRIEF) | Evidence |
|---|---|
| 1. Presets configure + build Debug and Release | `cmake --preset vs2019` OK; `cmake --build --preset vs2019-release --parallel` → `exit=0 elapsed=00:04:52`; `vs2019-debug` → `exit=0` (final run), logs in `build/vs2019-*.log` |
| 2. `ESTree.exe --help` exits 0 | Debug and Release print the option list, `help exit=0`; `--width abc` → error text, exit 1 (see `notes/baseline.md`) |
| 3. No backslash / uppercase GLM includes | Grep for `<GLM`, `GLM[/\\]` over `ESEngine/**/*.{h,cpp}` → no matches; script-verified: no `\` left on any `#include` line |
| 4. `notes/baseline.md` | present: toolchain, 143 unique `/W4` project warnings (658 raw log lines), build times, `--help` output |
| 5. `CLAUDE.md` not "Provisional" | grep `rovisional` → no matches; commands in it are the ones run here |
| 6. Behavior untouched | `git diff --stat` for sources: only include lines + one `#include <string>` (`Texture.h`) |

## Verification run

- `vs2019` Debug + Release built and linked; `ninja-msvc` Debug configured and built (351 steps, 1m31s) with `cl` 19.51.36246, then `--help` exit 0 from `build/ninja-msvc`.
- `scripts\msvc-env.cmd` re-verified after a fix (parenthesized `(x86)` path broke an `if (...)` block): configure + build exit 0, and a wrong `VCVARS64` gives a message + exit 1.
- No tests exist yet (T01).

## Interface changes

None (build files only). New repo requirement: **CMake ≥ 3.21** (documented in README, CLAUDE.md, D-005).

## Deviations from the brief

- `vs2026` preset is defined but **does not configure here**: "could not find any instance of Visual Studio" (the VS "18" Build Tools instance isn't registered with CMake's VS locator). `ninja-msvc` covers the newer compiler. `ninja-msvc` Release was not built.
- Extra: besides `GLM`, all backslash includes (`GLFW`, `glad`, `BulletCollision`, `boost`) were normalized (needed for Linux in T11).
- Extra: `#include <string>` in `Texture.h` and `bcrypt` link were required for the unchanged engine to build with the newer toolset (not in the brief's list).

## Human verification — result and a pre-existing driver crash

The owner ran `Debug\ESTree.exe --scene 0` from `build\vs2019`: one frame, then the process
vanished without the final `Quitting..` log line. Investigation (same session, processes started
by the agent on the same desktop):

- Windows Application log: **access violation `0xC0000005` in `ig9icd64.dll`** (Intel HD Graphics 530
  OpenGL driver 31.0.101.2111, fault offset `0x9c490`) — for the new vs2019 Debug, the new Release
  **and the original 2020 VS14 binary** (`build\Debug\ESTree.exe`) alike. So it is **not a regression**
  of T00. On this Optimus laptop Windows starts the exe on the Intel iGPU by default.
- Fix applied (commit after T00): `ESTree.cpp` exports `NvOptimusEnablement` /
  `AmdPowerXpressRequestHighPerformance` so hybrid-graphics drivers pick the discrete GPU.
  With it, `Debug\ESTree.exe --scene 0` stayed alive for the full 30 s test window and the log shows
  mouse picking events (`Object clicked: CrateCube`, `Rigidbody picked/released`), i.e. the
  render/update loop and input work on the GTX 960M.
- Not fixed: the engine still crashes the Intel HD 530 driver at the same call (deterministic).
  Likely an unsupported or strictly validated GL feature (the shaders use GLSL 4.30 subroutines and
  `std140 binding=` UBOs) — see Follow-ups.

Remaining optional checks for the owner: console toggle `~`, `--scene 2` (forest), normal quit
(window close should log `Quitting..`).

## Follow-ups

- Intel HD 530 (and probably other Intel iGPUs) crash in `ig9icd64.dll`; find the offending GL call
  (e.g. with `glDebugMessageCallback`/KHR_debug and bisecting scene setup) if Intel support matters.
  Out of scope for the current plan; add to `docs/BACKLOG.md` if wanted.
- `GLWindow.cpp:127` warning C4701 (`state` possibly uninitialized) — worth a look in T10.
- T09 should fix the `--help` text (`scenes 0-3` → `0-4`).
- 143 unique `/W4` warnings remain (C4244 ×40, C4239 ×29, C4267 ×19, C4458 ×19, …); candidates for a cleanup task later.
- The first `build/` directory content from 2017–2020 (VS14 cache, `build/Debug/ESTree.exe`) is still on disk and git-ignored; safe to delete once nobody needs the reference exe.
- Bullet builds all of its libraries (351 ninja steps); restricting to the three libs in use would speed CI (T11).
