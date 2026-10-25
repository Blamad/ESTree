# T11 — CI (Windows + Linux) and headless Linux build — Report

**Result:** done locally; **never run on GitHub** · **Branch:** `task/T11-ci-and-linux-build` · **Final commit:** `3f76c2e` (merged into `modernization` as `c70f4fe`)

The implementing subagent could not write this file; the orchestrator wrote it from the agent's hand-back and re-verified
the Windows side and (after merging T02) the Linux side in Docker.

## What changed

- `.github/workflows/ci.yml`: triggers push + pull_request (docs/md/images ignored).
  - **Windows** (windows-latest, MSVC): Debug + Release with `ESTREE_BUILD_ENGINE=OFF` (runners have no vc140 Boost libs).
  - **Linux** (ubuntu-latest, GCC Release): matrix engine ON and engine OFF (system Boost 1.83 from apt for ON).
  - Submodules recursive; FetchContent cached via `-DFETCHCONTENT_BASE_DIR`; Boost 1.72.0 **headers** downloaded once and cached where tests need them; `ctest --output-on-failure`; logs uploaded on failure.
- `CMakeLists.txt`: `ESTREE_BUILD_ENGINE` (default ON on Windows, OFF elsewhere) gates GLFW/Bullet/FreeType and the `ESTree` target; `BOOST_LIB` is required only for an MSVC engine build; `BOOST_INCLUDEDIR` (cache or env) overrides the Boost header dir (`ESTREE_BOOST_INCLUDE_DIR`); non-MSVC engine builds link system `boost_*`.
- Linux portability edits under `ESEngine/` (behavior-neutral): opaque enum declarations `: int` (`GameObject.h`, `Component.h`, `TextureBuffer.h`, `Texture.h`), removed extra qualification (`InputState.h`), `<memory>`/`<sys/stat.h>` includes, `windows.h` guarded by `_WIN32`, rvalue-to-non-const-ref MSVC extension → `const&` (`InstancedTransform`, `Shader::update*Matrix`, `PhysicsManager::raycast`, `SegmentTransform`, `LindenmayerTreeParams::{addRule,getRules}`, tree `Material` params, `*Scene::createLindenmayerTree`).
- `README.md` badge, `tests/README.md` ("Reproducing CI locally"), `docs/DECISIONS.md` D-008; `tests/engine/CMakeLists.txt` (skipped when the engine is OFF; links Bullet/Boost for GNU ld).

## Acceptance criteria

| Criterion | Evidence |
|---|---|
| Workflow lints | agent: `docker run rhysd/actionlint:latest` exit 0 |
| Windows, engine OFF, no `BOOST_LIB` | agent: configure + Debug/Release build + ctest 2/2; orchestrator re-ran the configure (`-DESTREE_BUILD_ENGINE=OFF`, no `BOOST_LIB`) OK |
| Windows full config | *verified*: after merge `ninja-msvc` Debug builds `ESTree.exe` and `100% tests passed out of 41` |
| Linux in Docker | agent (ubuntu:24.04, GCC 13, cmake 3.28): engine ON `ESTree` compiles and links, ctest 15/15; engine OFF ctest 2/2. Orchestrator re-ran engine OFF **after the T02 merge**: see *Post-merge finding* |

## Post-merge finding (important)

With T02 merged, the Linux job went red: **8 of 26 legacy golden tests fail on libstdc++** — the legacy solver loses the tail of regex rewrites
(`match_results::suffix()` after a failed `regex_search` is unspecified; MSVC happens to keep it). Details in the T02 report. Resolution:
`legacy_characterization_tests` is now built **only for MSVC** (`tests/CMakeLists.txt`, commit `d436ba8`). The portable `treegen` code (T03+) must be
tested on Linux too, so this CI will catch regressions of that kind.

## Deviations

- Windows CI does not compile the engine (needs vc140 Boost libs); it is only verified locally until Boost is removed (T07/T13).
- Linux builds the full engine (better than the brief's best effort) with system Boost 1.83, not 1.72.
- Linux builds `all`, i.e. all of Bullet; real runner time unknown (limit 40 min on Linux).

## Not verified

- Anything on GitHub (no push was made). The `windows-latest` job (default VS 2022 generator) was only mimicked with Ninja + MSVC 19.51. `archives.boost.io` reachability from runners (it worked from Docker).

## Human action

1. Push `modernization` (or a PR branch) when ready and check the Actions tab — the repo is public, so this uses free minutes.
2. Paste any failing job log back. Likeliest failures: Boost download, runner time limits, VS generator differences on Windows.
3. Confirm the badge URL (`Blamad/ESTree`, `ci.yml`).

## Follow-ups

- Restrict Bullet to the 3 libs actually used to cut CI time.
- Drop the MSVC guard on the legacy tests when the legacy sources are deleted (T07).

## First real GitHub run (owner, 2026-10-25) — three jobs failed, two workflow bugs, fixed

- **Windows (Debug and Release):** `cmake ... -DCMAKE_POLICY_VERSION_MINIMUM=3.5` in a PowerShell step: PowerShell splits an unquoted `-DNAME=3.5` at the dot into `-DNAME=3` and `.5`
  ("Ignoring extra path `.5`", "Invalid CMAKE_POLICY_VERSION_MINIMUM value `3`"). Reproduced locally; every `-D` argument in the Windows step is now quoted.
  (The runner image has VS 18 / MSVC 19.51 and CMake 4.4, so the engine configure otherwise got as far as the vendored `cmake_minimum_required` checks.)
- **Linux (engine OFF):** the OFF job configured without `-G Ninja` (Unix Makefiles) while the ON job used Ninja, both with the same cache key and `FETCHCONTENT_BASE_DIR`; the restored
  `doctest-subbuild` belonged to the other generator ("Does not match the generator used previously"). Both Linux jobs now use Ninja (installed in both) and the cache keys carry the
  generator/config/engine setting (`fetchcontent-<os>-ninja-engine-<ON|OFF>-...`, `fetchcontent-<os>-vs-<config>-...`).
- Lesson: the earlier local/Docker runs used `-G Ninja` everywhere and never shared a cache directory between configurations, so neither bug could show up before a real run.
