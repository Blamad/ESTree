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

## Needs human verification

1. Start the engine: from `build\vs2019` (after `setup.bat vs2019` or the junction step in `CLAUDE.local.md`) run `Debug\ESTree.exe --scene 0`; expect the single-tree scene to render (needs OpenGL 4.3 on the GTX 960M / Intel HD 530). Check the console toggle (`~`) opens.
2. Optionally run `Release\ESTree.exe --scene 2` (forest) and quit normally.

## Follow-ups

- T09 should fix the `--help` text (`scenes 0-3` → `0-4`).
- 143 unique `/W4` warnings remain (C4244 ×40, C4239 ×29, C4267 ×19, C4458 ×19, …); candidates for a cleanup task later.
- The first `build/` directory content from 2017–2020 (VS14 cache, `build/Debug/ESTree.exe`) is still on disk and git-ignored; safe to delete once nobody needs the reference exe.
- Bullet builds all of its libraries (351 ninja steps); restricting to the three libs in use would speed CI (T11).
