# T10 — Ownership fixes and sanitizer pass — Report

**Result:** done · **Branch:** `task/T10-ownership-and-sanitizers` · **Final commit:** `6595274` (merged into `modernization` as `fb8f630`)

The implementing subagent could not write this file (subagents may not write reports); the orchestrator wrote it from the
agent's hand-back and **re-verified the headline claims on the merged `modernization`** (marked *verified*).
Merge note: one conflict (`LightsManager.cpp`, T09 range-for vs T10 `static_cast`) resolved by combining both.

## Proven defects fixed (agent's evidence)

1. `Scene::~Scene` and `UIManager::~UIManager` called `release()` on `unique_ptr`s → nothing was ever destroyed.
2. Objects deleted through bases with non-virtual destructors (MSVC C5205 for `VertexArray` ×28, `FrameBuffer`, `Renderer`, `UIComponent`, `Window`; same for `GameObject`/`Component`). `~GLWindow` (which calls `glfwTerminate`) never ran.
3. `InputState::~InputState` did `delete &member` on five by-value vectors; new test `InputState: can be created, filled and destroyed` crashes the old code; ASan reports `new-delete-type-mismatch` at `InputState.cpp:78`.
4. No Bullet object was ever deleted (`RigidBody` body/motion state/shape, `PhysicsManager` world and helpers, three temporaries per tree in the hull builder). Bodies still in the world when objects die → `heap-use-after-free` in `~btCollisionWorld` (experiment, reverted), so `~Scene` unregisters bodies and lights first.
5. `rm` / `rtree` left `MouseManager`'s selected/picked pointers dangling → `forgetGameObject` + find-based guard in `removeGameObject`.
6. `LightsManager::removeLight` erased an uninitialised pointer; `GLWindow.cpp:127` C4701 fixed.

`Engine::~Engine` now destroys scene → renderer → window. Typed component access keeps the `ComponentType` enum (D-006). Engine cast regex count 31 → 0; `release()` in `ESEngine` → 0 matches.

**Left alone (by design):** 26 C-casts in `ESEngine/ESTree`, static `Context` pointers outliving their objects, removal of the active camera, self-initialised `DirectionalLight::directory`, GL objects never deleted (no `glDelete*` anywhere).

## Acceptance criteria

| Criterion | Evidence |
|---|---|
| 1. No `release()` in Scene | agent: 0 matches in `ESEngine` |
| 2. Cast count 0 in Engine | agent: regex 31 → 0 |
| 3. Build + `/W4` not above baseline + ctest + ASan preset | *verified* on merged tree (ninja-msvc Debug): build exit 0, **191 unique project warnings** (same as before T10), `100% tests passed out of 15`. Agent: Release and `vs2019` Debug also OK; ASan presets build and pass 15/15, no findings on exe or tests |
| 4. Proven defects listed with evidence | above |
| Smoke / clean shutdown | *verified*: scenes 0–4, window alive after 12 s, `CloseMainWindow()` → process exits, log has `Rendering started` and `Quitting..` for every scene, no `Application Error` event |

## Tests added (headless, `tests/engine/`)

`gameobject_tests.cpp`, `inputstate_tests.cpp`, `physics_tests.cpp` (links Bullet on purpose — it tests Bullet teardown order; a documented exception to the T01 README rule). `Scene` itself can't be built without GL, so its teardown order is covered only by the exe runs.

## Interface changes

Typed accessor on `GameObject`/`Component` (D-006), virtual destructors on engine base classes, `MouseManager::forgetGameObject`. New CMake option `ESTREE_SANITIZE` and presets `ninja-msvc-asan`, `vs2019-asan`.

## Deviations

- x64 ASan runtime is not in VS "18" Build Tools (x86 only); `ninja-msvc-asan` needs `VCVARS64` pointing at the VS 2019 Build Tools `vcvars64.bat` (D-007); `vs2019-asan` works as is.
- The ASan runtime dies with `0xC0000142` next to System32's newer `vcruntime140.dll`; the CMake block copies the matching CRT DLLs beside each exe (~59 CMake lines in total, opt-in only).
- `tests/CMakeLists.txt` touched (`add_subdirectory(engine)` + ASan hook) and a handful of files outside the owned list.
- `vs2019-asan` configure hit "Filename too long" in the deep worktree path; needed `-DFETCHCONTENT_SOURCE_DIR_DOCTEST`.
- MSVC ASan has no leak detector → leaks (Bullet etc.) are verified by reading code and use-after-free experiments, not by LSan.

## Human verification (owner, 2026-10-25)

1. Window closes cleanly — **confirmed by the owner**.
2. Scene 1: picking up / moving a crate — **confirmed**.
3. Select an object, `rm` twice, then `normals on`: no crash — **confirmed**.
4. `rtree` regeneration of a selected tree — **not checked** by the owner (optional).

## Follow-ups

- C-casts in `ESEngine/ESTree`, removal of the active camera, static `Context` pointers (BACKLOG candidates).
- GL resources are never deleted (`glDelete*`); harmless at process exit, relevant if scenes are ever reloaded.
- `scripts\msvc-env.cmd` prints a cosmetic `vswhere.exe is not recognized`.
