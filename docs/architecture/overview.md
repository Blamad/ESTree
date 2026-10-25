# Current architecture (as of 2026-10-25, before modernization)

Facts below were read from the code at commit `c30a23c`. Anything marked *(verify)* was inferred
from reading, not from running. ~8.5k lines of C++ in `ESEngine/`, no tests, no CI, 153 commits (2017-04 → 2020-04).

## Engine

```
ESTree app (ESEngine/ESTree)        main(), Scenes/, Console/ commands, Models/
   │
Engine (ESEngine/Engine)            Core/ (Engine loop, Logger, Screen, Texture2D, Vertex)
   │                                GameObject/ (GameObject + Component: Transform, Mesh, InstancedMesh,
   │                                  Camera, Light(s), RigidBody, Skybox, CubeMap, Material, Behaviour)
   │                                Manager/ (Scene/Shader/Texture/Lights/Physics/Mouse/UI managers, Context)
   │                                Scene/ (Scene: owns GameObjects, update + render frame), UI/ (console)
GraphicEngine (ESEngine/GraphicEngine)   GLWindow (GLFW), GLRenderer, Shader, GLVertexArray,
                                         TextureBuffer, Framebuffer/{Depth,HDR,PostEffect}, Input/
```

- OpenGL **4.3 core** via glad; GLSL uses `std140, binding=1` UBOs and `subroutine`s. macOS (GL ≤ 4.1) cannot run it.
- `Engine::renderingLoop`: poll input → `Scene::update` (physics, UI, mouse picking, behaviours) → `Scene::renderFrame` (shadow-map pass in `LightsManager::prepareShadowBuffer`, forward Blinn-Phong pass with PCF shadows, skybox, optional HDR/post-effect framebuffer, UI).
- A `GameObject` holds typed components (`getComponent(TRANSFORM|RENDERABLE|RIGIDBODY|LIGHT|BEHAVIOUR|CAMERA)` returns `Component*`; callers C-cast to the concrete type).
- `Mesh` keeps CPU `std::vector<Vertex> vertices` and `std::vector<int> indices`, uploads with `updateMesh()`; buffers are allocated with fixed capacities (`vBufferSize`, `iBufferSize`) and `GL_STREAM_DRAW` for generated trees.
- `Scene` stores `std::unordered_map<Uuid, std::unique_ptr<GameObject>>`; no culling, no render sorting. Up to 10 point + 10 directional lights.
- Physics: Bullet (`RigidBody`, `PhysicsManager`), used for mouse picking and the block-tower scene. Trees get a hull `RigidBody`.
- Text/UI console: FreeType; commands are registered in `ESTree.cpp` (`tree`, `rtree`, cube commands, model export).
- Scenes (`--scene N`): 0 `SingleTreeScene`, 1 `PhysicsScene`, 2 `ForestScene`, 3 `TestScene`, 4 `TreeScene`. The tree scenes create their trees with `LindenmayerTreeFactory` (T07).

## Build

- `CMakeLists.txt` (cmake_minimum_required 3.0) globs `ESEngine/*`, links `glfw`, `BulletDynamics/BulletCollision/LinearMath`, `freetype`, and **Boost** `date_time` + `thread` (hard-coded `vc140-x64-1_72` names), plus Boost headers from `$ENV{BOOST_LIB}`.
- `setup.bat` generates a "Visual Studio 14 Win64" solution into `build/` and symlinks `Resources`.
- Boost was used for: `random`, `thread`, `date_time` (timing), `foreach`, `filesystem` (`ModelExporter`), `program_options` (`ESTree.cpp`), `algorithm/string` (split). No `#include <boost/...>` is left in `ESEngine/` (T07, T09); only the build files still link the Boost libraries (T13 removes that).
- Non-portable bits: `#include <GLM\common.hpp>` (backslash, 15 occurrences of `GLM/` in upper case), Windows-only build script.

## Tree pipeline (`ESEngine/ESTree/Models/Tree/` over `ESEngine/TreeGen/`)

Since T07 the engine holds no generation logic; `treegen` (pure C++17, contract: `treegen-design.md`) does all of it.

```
Resources/LindenmayerRules/*.json | legacy/*.l
        │  treegen::loadTreeParams(path)                 → TreeParams (+ optional "seed")
        ▼
        │  treegen::generateTree(params, {seed, ringDensity})
        ▼    rewrite (seeded, std::mt19937) → interpret (turtle → Skeleton) → buildGeometry (threads above 50 000 vertices)
TreeModel { geometry.trunk: MeshData (MeshVertex + uint32 indices), geometry.leafTransforms: vector<mat4> }
        │  LindenmayerTree::generate()   (the adapter, Models/Tree/TreeMeshConversion.h holds the pure conversions)
        ▼
Mesh (trunk, GL buffers sized from the output)  +  InstancedMesh (leaf cross-quad, one instance per leaf, in a child GameObject)
```

- `LindenmayerTree : GameObject` is the adapter: constructor takes `treegen::TreeParams` (and an optional console seed), `generate()` runs
  the pipeline and uploads. The seed is `TreeParams::seed` (JSON `"seed"`) if non-zero, else the console `seed <n>`, else a fresh random
  value; it is logged with the generation time (`Generation finished. N tris. Time: Xms. Seed: S`) and kept in `getSeed()`.
- Conversions (`treeadapter::` in `TreeMeshConversion.h`, no GL, tested headless): `MeshVertex` → `Vertex` (color zero), `uint32_t` → `int`
  indices (refused above `INT_MAX`), leaf matrices → `InstancedTransform` premultiplied by the tree's world matrix.
- GL buffers are created with exactly the generated sizes (`Mesh(vertices, indices, shader, vBufferSize, iBufferSize, GL_STREAM_DRAW)`); there is no
  vertex/index cap any more (ternary E at depth 10 is 2.1 M vertices).
- `LindenmayerTreeFactory::generateTree` / `generateTreeFromFile` build the tree, give it the hull `RigidBody` and register it with a scene (the
  scenes pass themselves, the console uses the active scene). `ConsoleTreeCommand` (`tree`, `rtree`, parameter `seed <n>`) catches `treegen::Error`
  and prints one line to the in-game console; a scene that cannot load or generate a tree logs it and skips that tree.

### Rule syntax (as implemented)

- Symbols are **single characters**; parameters in parentheses: `F(len)`, `A(l,w)`.
- Production strings reference custom parameters (from `"parameters"`) and rule-local parameters; numeric expressions inside parentheses support `+ - * /` with one binary operator (e.g. `l*lr`).
- Rule fields: `symbol`, `production`, optional `probability` (default 1.0; rules for one symbol are tried in order, `value -= probability`), optional `initialDepth` (rule only applies once iteration index ≥ it).
- Interpreter alphabet (turtle looks along **+Y**):

| Symbol | Meaning | Default parameter |
|---|---|---|
| `F`, `f` | draw a segment of the given length (`length * lengthScale`) | current length |
| `[` `]` | push / pop turtle state | — |
| `^` / `v` | pitch about X, + / − | `angle` |
| `<` / `>` | yaw about Z, + / − | `angle` |
| `+` / `-` | roll about Y, + / − (also accumulates `roll`) | `angle` |
| `;` | scale current angle | 0.625 |
| `` ` `` | scale length and radius | 0.625 |
| `=` | scale radius only | 0.625 |
| `$` | restore horizontal orientation | — |
| `T` | tropism towards `params.tropism` | `tropismBendingFactor` |
| `X~(v)` | random parameter uniformly in `[0, v)` (not used by any shipped rule file) | — |

  After a segment is drawn the turtle's local rotation resets to identity (the cumulative rotation lives in `totalRotation`), length resets to `initialLength`, while `radius`, `angle`, `lengthScale` persist. Unknown characters are ignored.
- Angles in rule files are degrees; parameters in `getNumericParameter` are converted with `toRadians`.

### Geometry (as implemented; now `treegen::buildGeometry`)

- Each `F` creates **two rings** of `ringDensity` (= 9) vertices: a copy of the parent's top ring (rotated by the roll difference, UV v=0) and the new top ring (v=1); 16 triangles per segment. The 9th ring vertex duplicates the 1st for the UV seam.
- Ring point: `(radius·cos(θ)/2, 0, radius·sin(θ)/2)` in the segment's local frame, so the **stored `radius` is a diameter** (the geometric radius is half). Keep this in mind when porting.
- Normals are radial (no smoothing across segments), so joints show seams.
- Leaves: every segment with no children (`isLastStem`) gets a double-sided 2×2 quad, instanced, optional random angle ± `leavesAngleDiversity`, kept with probability `leafGrowthProb`.

### Defects of the former pipeline (all fixed or gone: D-009, D-010, D-014, D-015, T07)

- `fillRuleWithVariables` consumes the delimiter after a match, so two occurrences sharing one delimiter (`(a1,a1)`) may leave the second unreplaced.
- `LindenmayerTreeSolver::applyRule` splits an expression at the *first* operator character, so a leading minus (`(-5*2`) is likely mis-evaluated (left side `""` → 0, giving `0 - 5` instead of `-10`); precedence/associativity of chained operators is not handled.
- `LindenmayerTreeInterpreter::returnNewIndexAfterParameter` returns `float`; indices above 2^24 would lose precision.
- Parameter text is re-scanned from the start for every `F`, with `std::string` copied by value in `getNumericParameter(std::string product, ...)` → O(n²) on large products.
