# T10 — Ownership fixes and sanitizer pass — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: use `superpowers:subagent-driven-development`
> or `superpowers:executing-plans`. Steps use checkbox (`- [ ]`) syntax.

**Status:** in-progress · **Branch:** `task/T10-ownership-and-sanitizers` · **Base:** `modernization @ 3e1cbf1`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** Fix the confirmed ownership/lifetime defects in `Scene`/`GameObject`, replace the unsafe
component casts with a typed accessor, and get a sanitizer (ASan) build of the engine and of whatever
is headless-testable.

**Approach:** First map who owns what and in which order things die (below), because the
`release()` calls in `Scene::~Scene` and `UIManager::~UIManager`, a non-virtual `~Window`, and
`release()` in `~GLWindow` currently mean *nothing is ever destroyed on exit*. Fixing one layer
exposes the next, so the fixes go in together and in dependency order. Every extra defect fixed
beyond `Scene` is justified by a reading proof (quoted in REPORT.md) and kept to a few lines.

## Ownership and destruction-order analysis (read from the code, before any change)

`main` → `delete engine` → `Engine::~Engine`:

| Step today | What really happens |
|---|---|
| `window.reset()` first | `Window` has **no virtual destructor** and is deleted through `unique_ptr<Window>` that holds a `GLWindow` → UB; on MSVC only the base dtor runs, so `~GLWindow` (instance=nullptr, `glfwTerminate`) **never runs**. If it did run, the GL context would be gone before the next two lines |
| `renderer.reset()` | `Renderer` trivial |
| `sceneManager.reset()` | `shared_ptr<Scene>` → `~Scene` (dynamic type is e.g. `PhysicsScene`, deleter is correct) |

`Scene::~Scene` today: resets skybox, mouse/lights/physics/ui managers, then `release()`s every
`unique_ptr<GameObject>` → **all GameObjects (meshes, bodies, trees) leak and their destructors never
run**. With the `release()` loop gone the objects would die *after* `physicsManager` (declaration order
puts `gameObjects` after it, but the explicit `reset()`s run first in the dtor body), and
`GameObject` itself has **no virtual destructor** while `Scene` stores `Cube`, `LindenmayerTree`, … as
`unique_ptr<GameObject>` → UB, derived members never destroyed.

Raw-pointer web that has to be torn down in order:
- `PhysicsManager` ← `RigidBody*` registered in the Bullet world. `~btCollisionWorld` dereferences every
  body still registered (`Vendor/bullet3/.../btCollisionWorld.cpp:75-93`), so bodies must leave the
  world *before* they are deleted, and the world must be deleted *before* nothing else.
- `LightsManager` ← `Light*` sets; `MouseManager` ← `GameObject*` (picked, selected), `Camera*`,
  `CameraBehaviour*`, `PhysicsManager*`; `Scene::activeCamera` ← `Camera*`.
- Bullet objects (`btRigidBody`, motion state, shape; `btDiscreteDynamicsWorld`, solver, dispatcher,
  config, broadphase) are all `new`ed and never deleted.
- **GL objects:** no class in `Engine/` or `GraphicEngine/` has a destructor that calls `glDelete*`
  (only `glDeleteShader` right after linking). So today nothing touches GL during teardown and the
  context lifetime is not a live problem; I will *not* add GL deletes (out of scope, would be a new
  feature). The order is still fixed so that when they are added the context outlives them:
  `sceneManager` → `renderer` → `window` (reverse of construction). `ShaderManager` / `TextureManager`
  are function-local statics that die after `main` — they hold `shared_ptr<Shader>`/`Texture` only, no
  GL calls in their dtors, so safe today; noted as a follow-up (explicit `clear()` before the window).

Target teardown (Scene): unregister every object's rigid body from the world and every light from the
lights manager, `gameObjects.clear()` (objects die while managers are alive), then members die in
reverse declaration order (frame buffer, ui, physics, lights, mouse, skybox) — no explicit `reset()`
needed. A comment documents why the unregister loop must precede the clear.

## Files

Owned (per brief):
- Modify: `ESEngine/Engine/Scene/Scene.{h,cpp}` — dtor, `addGameObject`/`createGameObject`/`removeGameObject`, cast replacement, virtual dtor
- Modify: `ESEngine/Engine/GameObject/{GameObject.h,GameObject.cpp,Component.h,Transform.cpp}` — `getComponentAs<T>`/`getComponentsAs<T>`, virtual dtors, cast replacement
- Modify: `ESEngine/Engine/Manager/{PhysicsManager,MouseManager,LightsManager,LightsUtils,UIManager}.*` — Bullet teardown, `forgetGameObject`, `removeLight` uninitialised pointer, `UIManager` `release()`, casts
- Modify: `CMakeLists.txt` (ASan block, after the `/W4` block), `CMakePresets.json` (`ninja-msvc-asan`, `vs2019-asan`)
- Create (blocked on T01): `tests/engine/**`

Cast sites inside `Engine/GameObject` that are not in the owned-file list but are named by brief scope item 2
and counted by acceptance criterion 2 — only the cast lines change: `RigidBody.cpp`, `Mesh.cpp`,
`DirectionalLight.h`. `RigidBody.{h,cpp}` additionally get a destructor and a leak fix (proof below).

**Touched outside the owned list** (each is a proven ownership defect that the `Scene` fix would otherwise
turn from "hidden" into "crash on exit"; the changes are a few lines each — drop them if a reviewer prefers):

| File | Change | Proof (reading) |
|---|---|---|
| `Engine/Core/Engine.cpp` | `~Engine`: destroy `sceneManager`, `renderer`, `window` (reverse of construction) | brief item 1 names `Engine::~Engine` |
| `GraphicEngine/Core/Window.h` | `virtual ~Window()` | `unique_ptr<Window>` deletes a `GLWindow` |
| `GraphicEngine/Core/GLWindow.cpp` | `inputState.release()` removed; C4701 `state` initialised | `release()` leaks `InputState`; brief item "look at C4701" |
| `GraphicEngine/Input/InputState.cpp` | dtor: remove `delete &mousePosition;` … (5 members that are by-value `std::vector`s) | invalid free; hidden only by the `release()` above |
| `GraphicEngine/Core/VertexArray.h` | `virtual ~VertexArray()` | `unique_ptr<VertexArray>` holds `GLVertexArray` |
| `Engine/UI/UIComponent.h` | `virtual ~UIComponent()` | `unique_ptr<UIComponent>` holds `ConsoleComponent` etc. (needed to fix `UIManager::release()`) |
| `Engine/UI/ConsoleInterpreter.cpp` | 3 cast lines only (`(Mesh*)`, `(RigidBody*)`) | acceptance criterion 2 counts `ESEngine/Engine` recursively |

## Interfaces

- Produces (`Component.h` / `GameObject.h`):
  ```cpp
  template<class T> T* GameObject::getComponentAs(ComponentType type);                 // static_cast (+ debug dynamic_cast assert); nullptr if absent
  template<class T> std::vector<T*> GameObject::getComponentsAs(ComponentType type);
  template<class T> T* Component::getComponentAs(const ComponentType &type);           // forwards to parent
  ```
  `getComponent(TYPE)` stays unchanged.
- Produces: `void MouseManager::forgetGameObject(GameObject*)` (clears picked/selected pointers to a dying object).
- Produces: CMake option `ESTREE_SANITIZE` (default OFF); presets `ninja-msvc-asan`, `vs2019-asan` (+ build presets).

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | Accessor takes the existing enum: `getComponentAs<Transform>(TRANSFORM)` | Fits the current `getComponent(TYPE)` scheme; no redesign (BACKLOG) |
| 2 | `Component` gets a virtual destructor, so debug builds can `assert(dynamic_cast<T*>(c))` in the accessor | Turns a silently wrong C-cast (e.g. `InstancedMesh` read as `Mesh`) into a debug assert; release builds pay nothing |
| 3 | `removeGameObject(unknown)` logs a warning and returns | "guard against unknown ids"; no exception, matches the codebase's logging style |
| 4 | Unregister-then-clear in `~Scene` instead of reordering members | Bullet needs bodies out of the world before deletion; a member reorder cannot express that |
| 5 | Bullet objects are owned by `RigidBody` (body, motion state, shape) and `PhysicsManager` (world, solver, …) with plain `delete` in reverse creation order | No smart-pointer migration (out of scope); smallest correct change |
| 6 | Hull-shape builder deletes its three temporaries (`btTriangleMesh`, `btConvexTriangleMeshShape`, `btShapeHull`) | `btConvexHullShape(points, n)` copies the points (`btConvexHullShape.cpp:38`), so the temporaries are unreferenced |
| 7 | ASan on MSVC: `/fsanitize=address`, `/RTC1` removed, `_DISABLE_VECTOR_ANNOTATION`/`_DISABLE_STRING_ANNOTATION` defined, `/INCREMENTAL:NO`; Clang/GCC `-fsanitize=address,undefined` | `/RTC1` is incompatible; annotation macros avoid LNK2038 against the prebuilt vc140 Boost libs |
| 8 | Not fixed (suspected only, listed in REPORT): `Context` static pointers outlive their objects, `DirectionalLight` ctor self-initialises `directory(directory)`, active camera removal | not ownership proofs in scope / would need design decisions |

## Steps

### Step group 1: plan and typed accessor
- [ ] **1.1** Commit this plan (`T10: add plan`)
- [ ] **1.2** Add `getComponentAs`/`getComponentsAs` to `GameObject`/`Component`; virtual `~Component`, `~GameObject`
- [ ] **1.3** Replace casts in `Engine/Scene`, `Engine/GameObject`, `Engine/Manager`, three lines of `ConsoleInterpreter.cpp`
- [ ] **1.4** Build Debug; `grep -rnE "\((Camera|Transform|RigidBody|Light|Behaviour|Renderable)\*\)" ESEngine/Engine` → empty. Commit

### Step group 2: Scene ownership
- [ ] **2.1** `~Scene` (unregister, clear), `addGameObject`/`createGameObject` via iterator, `removeGameObject` guard + `MouseManager::forgetGameObject`, `LightsManager::removeLight`
- [ ] **2.2** `grep -n "release()" ESEngine/Engine/Scene` → empty. Build. Commit

### Step group 3: the lifetime chain exposed by 2
- [ ] **3.1** `PhysicsManager` dtor, `RigidBody` dtor + hull temporaries, `UIManager` `release()`
- [ ] **3.2** `Window`/`VertexArray`/`UIComponent` virtual dtors, `GLWindow`, `InputState`, `Engine::~Engine`
- [ ] **3.3** Build Debug + Release; warnings vs baseline (143); smoke scenes 0–4; try `CloseMainWindow()` for the quit path. Commit

### Step group 4: sanitizer
- [ ] **4.1** `ESTREE_SANITIZE` block + presets; configure and build `ninja-msvc-asan` Debug
- [ ] **4.2** Run the exe under ASan (scenes 0–4, quit path); record findings. Commit

### Step group 5: tests (blocked on T01)
- [ ] **5.1** If `tests/CMakeLists.txt` exists on `modernization`: merge it into this branch; write `tests/engine/*_tests.cpp` (below); run `ctest` normally and under ASan
- [ ] **5.2** Otherwise: record in REPORT.md that item 6 is pending on T01 and describe the tests

Planned tests (`tests/engine/ownership_tests.cpp`, GL-free; `GameObject`, `Transform`, `Component`, Bullet
`LinearMath`, boost uuid; no `Scene`: its constructor builds `LightsManager`, which compiles shaders → needs GL):
- `GameObject: component add/get/remove` — new object has exactly one TRANSFORM; `getComponentAs<Transform>(TRANSFORM) != nullptr`; absent type → `nullptr`; adding a second component of a type makes `getComponent` return the last (current behaviour, pinned); `getComponents` returns all.
- `GameObject: destruction through base pointer runs derived destructor` — derived object with a counter, deleted through `unique_ptr<GameObject>` (fails without the virtual dtor).
- `GameObject: unique ids` — 1000 objects, all `id`s distinct.
- `PhysicsManager: add/remove/destroy bodies` — needs `Logger` + `Context` (boost date_time) sources: world with N boxes, remove half, destroy objects, destroy manager; green under ASan only if the teardown order is right.
- `InputState: construct and destroy` — fails under ASan before the dtor fix (invalid free).

## Review focus covered

None of the PLAN.md review-focus items belong to T10.

## Deviations from the brief

- Brief item 5 names the preset `vs2019-asan`; the Ninja preset `ninja-msvc-asan` is the one verified end to end
  (MSVC 19.51 with the ASan runtime). `vs2019-asan` is added as well and verified as far as noted in REPORT.md.
- Touches the files listed under "Touched outside the owned list" above.

## Blockers

(none yet)
