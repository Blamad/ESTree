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
- Scenes (`--scene N`): 0 `SingleTreeScene`, 1 `PhysicsScene`, 2 `ForestScene`, 3 `TestScene`, 4 `TreeScene`. Each tree scene duplicates a private `createLindenmayerTree(...)` helper.

## Build

- `CMakeLists.txt` (cmake_minimum_required 3.0) globs `ESEngine/*`, links `glfw`, `BulletDynamics/BulletCollision/LinearMath`, `freetype`, and **Boost** `date_time` + `thread` (hard-coded `vc140-x64-1_72` names), plus Boost headers from `$ENV{BOOST_LIB}`.
- `setup.bat` generates a "Visual Studio 14 Win64" solution into `build/` and symlinks `Resources`.
- Boost is used for: `random`, `thread`, `date_time` (timing), `foreach`, `filesystem` (`ModelExporter`), `program_options` (`ESTree.cpp`), `algorithm/string` (split). All replaceable by C++17 std.
- Non-portable bits: `#include <GLM\common.hpp>` (backslash, 15 occurrences of `GLM/` in upper case), Windows-only build script.

## Legacy tree pipeline (`ESEngine/ESTree/Models/Tree/`)

```
Resources/LindenmayerRules/*.json | legacy/*.l
        │  LindenmayerTreeParams(filePath)      JSON via rapidjson, .l via ifstream; custom parameters are
        ▼                                       textually substituted into productions (regex, std::to_string(float))
LindenmayerTreeParams {axiom, depth, initialLength, initialRadius, angle, leafGrowthProb, leavesAngleDiversity,
        │               tropism, tropismBendingFactor, customParameters, rules: map<symbol, vector<Rule>>}
        │  LindenmayerTreeSolver::generateTreeProduction(params)  -> std::string
        ▼   regex rewriting, depth times; static RNG seeded time(0); expression folding via regex (one binary op)
product string, e.g. "F(2)T+(45)A(...)[...]"
        │  LindenmayerTreeInterpreter::generateMeshData(params, product, meshGenerator)
        ▼   turtle; creates Segment tree + enqueues ring vertices in the mesh generator
vector<shared_ptr<Segment>> + LindenmayerTreeMeshGenerator (ring data)
        │  meshGenerator->generateMesh()   (boost::thread split of ring-point computation)
        ▼
Mesh (trunk)  +  generateInstancedLeaves(): one cross-quad InstancedMesh at every terminal segment
```

`LindenmayerTree : GameObject` orchestrates this (`generate()`), `LindenmayerTreeFactory::generateTree` also adds a hull `RigidBody` and registers the object with the scene. `ConsoleTreeCommand` (`tree`, `rtree`) and the tree scenes are the callers.

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

### Geometry (as implemented)

- Each `F` creates **two rings** of `ringDensity` (= 9) vertices: a copy of the parent's top ring (rotated by the roll difference, UV v=0) and the new top ring (v=1); 16 triangles per segment. The 9th ring vertex duplicates the 1st for the UV seam.
- Ring point: `(radius·cos(θ)/2, 0, radius·sin(θ)/2)` in the segment's local frame, so the **stored `radius` is a diameter** (the geometric radius is half). Keep this in mind when porting.
- Normals are radial (no smoothing across segments), so joints show seams.
- Leaves: every segment with no children (`isLastStem`) gets a double-sided 2×2 quad, instanced, optional random angle ± `leavesAngleDiversity`, kept with probability `leafGrowthProb`.

### Suspected defects *(verify in T02/T04/T05)*

- `fillRuleWithVariables` consumes the delimiter after a match, so two occurrences sharing one delimiter (`(a1,a1)`) may leave the second unreplaced.
- `LindenmayerTreeSolver::applyRule` splits an expression at the *first* operator character, so a leading minus (`(-5*2`) is likely mis-evaluated (left side `""` → 0, giving `0 - 5` instead of `-10`); precedence/associativity of chained operators is not handled.
- `LindenmayerTreeInterpreter::returnNewIndexAfterParameter` returns `float`; indices above 2^24 would lose precision.
- Parameter text is re-scanned from the start for every `F`, with `std::string` copied by value in `getNumericParameter(std::string product, ...)` → O(n²) on large products.
