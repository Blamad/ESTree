# T11 — CI (Windows + Linux) and headless Linux build — Implementation Plan

**Status:** review · **Branch:** `task/T11-ci-and-linux-build` · **Base:** `modernization @ fc539f2`
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
- [x] CMake option + gating; Windows engine ON (unchanged) and OFF (without BOOST_LIB) verified
- [x] Workflow + actionlint (exit 0)
- [x] Docker Linux: engine OFF and engine ON (ESTree links, 15/15 tests) with the workflow's commands
- [x] Docs (README badge, tests/README, D-008)

## Linux portability edits under `ESEngine/` (all behavior-neutral)

- `Engine/GameObject/{GameObject.h,Component.h}`, `GraphicEngine/Core/TextureBuffer.h`, `Engine/Core/Texture.h`: opaque enum declarations need an underlying type (`: int`, MSVC default).
- `GraphicEngine/Input/InputState.h`: removed extra `InputState::` qualification.
- `Engine/UI/ConsoleInterpreter.h`: `#include <memory>`; `Engine/UI/ConsoleComponent.cpp`: `windows.h` under `_WIN32`; `ESTree/Console/ConsoleTreeCommand.cpp`: `<sys/stat.h>`.
- Rvalue bound to non-const reference (MSVC extension) changed to `const&`: `InstancedTransform::translateModel`, `Shader::update{Projection,View}Matrix`, `PhysicsManager::raycast`, `SegmentTransform` ctor, `LindenmayerTreeParams::{addRule,getRules}`, `Material` parameters of `LindenmayerTree`, `LindenmayerTreeFactory::generateTree`, `{Forest,SingleTree,Tree}Scene::createLindenmayerTree`.
- `tests/engine/CMakeLists.txt`: skipped when the engine is OFF; `engine_gameobject_tests` also links Bullet/Boost libs (GNU ld needs them).

## Deviations from the brief

- Windows CI runs with the engine OFF (cannot build it on a runner). Linux builds the engine ON (compile+link only), which exceeded the brief's best-effort expectation.
- `tests/engine/CMakeLists.txt` edited (not in the owned list) to honour the option.

## Blockers

(none)
