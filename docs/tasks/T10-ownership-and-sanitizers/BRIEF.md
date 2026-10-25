# T10 — Ownership fixes and sanitizer pass

**Depends on:** T00 · **Unblocks:** T12 · **Size:** M · **Branch:** `task/T10-ownership-and-sanitizers`
**Parallel with:** T01, T09 (T09 edits Boost lines in some of the same files — keep your edits to the lines named below and rebase often)

## Goal

Fix the confirmed ownership/lifetime defects in `Scene`/`GameObject`, remove the worst unsafe
casts via typed accessors, and get a clean run of the (headless-testable) code under
AddressSanitizer/UBSan.

## Context to read

- `ESEngine/Engine/Scene/Scene.{h,cpp}` (`~Scene` calls `release()` on each `unique_ptr`)
- `ESEngine/Engine/GameObject/{GameObject,Component,Transform,Camera,RigidBody,Mesh,InstancedMesh}.*`
- `ESEngine/Engine/Manager/{SceneManager,PhysicsManager,LightsManager,MouseManager}.*`
- grep: `\(Camera\*\)`, `\(Transform\*\)`, `\(RigidBody\*\)`, `\(Light\*\)`, `\(Behaviour\*\)`, `\(Renderable\*\)`

## Scope

In:
1. `Scene::~Scene`: stop `release()`-ing the `unique_ptr`s (leak and skipped destructors); verify destruction order (physics bodies removed from the world before objects die, lights unregistered, GL objects destroyed while the context is alive — `Engine::~Engine` order). Add a comment only if the order is non-obvious.
2. Add typed accessors `template<class T> T* GameObject::getComponentAs(ComponentType)`-style (pick a minimal API that fits the existing `getComponent(TYPE)` enum scheme; keep `getComponent` working) and replace C-style casts at call sites **in `Engine/Scene`, `Engine/GameObject`, `Engine/Manager`** with `static_cast` through the accessor. Don't redesign the component system (BACKLOG).
3. `Scene::addGameObject`/`createGameObject`/`removeGameObject`: remove the repeated `gameObjects[uuid]` lookups (use the iterator/returned pointer); guard `removeGameObject` against unknown ids.
4. Make any other pointer-ownership defect you can *prove* (ASan report, or a clear double-free/leak by reading) — list each with evidence in `REPORT.md`; skip speculative ones.
5. Sanitizer build: add a CMake option `ESTREE_SANITIZE` (MSVC `/fsanitize=address`; Clang/GCC `-fsanitize=address,undefined`) as a preset `vs2019-asan`; run the headless tests under it. If the engine itself can't start headless, say so and run what can run (e.g. a small test that constructs `Scene` pieces that don't need GL — only if such a test is feasible without GL; otherwise document).
6. Tests: headless unit tests in `tests/engine/` for `GameObject` component add/get/remove and `Scene` add/remove ownership **only for classes that do not require a GL context** (if `Scene` constructs managers needing GL, test `GameObject`/`Uuid` only and explain).

Out of scope: redesigning ECS, smart-pointer migration of the whole code base, render changes, Boost removal.

## Owned files

`ESEngine/Engine/Scene/**`, `ESEngine/Engine/GameObject/{GameObject,Component,Transform}.*`, ownership/cast lines in `Engine/Manager/*`, `tests/engine/**`, ASan block in `CMakeLists.txt`/presets.

## Acceptance criteria

1. No `release()` on owning pointers in `Scene`; `grep -n "release()" ESEngine/Engine/Scene` is empty.
2. Cast count: `grep -rnE "\((Camera|Transform|RigidBody|Light|Behaviour|Renderable)\*\)" ESEngine/Engine` reduced to 0 (or each remaining one justified in the report).
3. Build Debug/Release OK; `/W4` not above baseline; `ctest` green; ASan preset builds and the headless tests pass under it.
4. `REPORT.md` lists proven defects fixed (with evidence) and suspected-but-unproven ones left alone.

## Verification

Automated. **Needs human verification:** run scenes 0–4 and quit normally — no crash on exit (the old `release()` path hid destructor bugs; the fix may surface order problems — report them, don't paper over).
