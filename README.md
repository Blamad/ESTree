# ESTree
[![CI](https://github.com/Blamad/ESTree/actions/workflows/ci.yml/badge.svg)](https://github.com/Blamad/ESTree/actions/workflows/ci.yml)

A hand-written OpenGL game engine (C++17) that renders scenes, runs physics simulations and generates tree models with
Lindenmayer systems (parametric, stochastic, with tropism). Created as part of a master's dissertation (2017-2020) and
modernized in 2026 without replacing the engine.
An executable of the original version (release `v1.0`, 2020, before the modernization) is available in the __Releases__ tab of the repository.

The tree generator is also available on its own as **`treegen`**, an engine-independent static library
(`ESEngine/TreeGen/`, depends only on the C++ standard library, glm and rapidjson) with a headless command-line tool,
`treegen_cli`, that writes a tree as a Wavefront OBJ file. The engine uses `treegen` through a thin adapter.

YouTube clip demonstrating project capabilities:<br/>
[![main_scene_clip](https://img.youtube.com/vi/CheCvFOeFZI/0.jpg)](https://youtu.be/CheCvFOeFZI)

Libraries (all vendored as git submodules in `Vendor/`, no other dependency):
 * __GLFW, GLM, GLAD, OpenGL__: window, input, math, scene rendering
 * __Bullet Physics__: physics simulation (collisions, mouse picking, collapsing towers made of blocks)
 * __RapidJSON__: parsing rule files
 * __FreeType__: text rendering
 * __stb__: texture loading
 * __doctest__ (fetched by CMake when tests are enabled): unit tests

Single tree scene clip:<br/>
[![single_tree_scene_clip](https://img.youtube.com/vi/QXZW_28LS78/0.jpg)](https://youtu.be/QXZW_28LS78)

Sample screenshots presenting current engine abilities regarding tree generation:

 * Simple bush model: <br/>
  ![Sample screenshot](/images/withHdr.PNG)
 * Ternary tree models generated from rules described in "The Algorithmic Beauty of Plants":<br/>
  ![Sample screenshot](/images/ternaryTreeA.PNG)
  ![Sample screenshot](/images/ternaryTreeE.PNG)

# Project setup

Requirements: __CMake 3.21+__ and a C++17 compiler. Windows x64 with the Visual Studio 2019 (or newer) C++ toolset is the primary
platform and the only one that can run the engine; Linux (GCC 13) is built and tested in CI (the engine only compiles and links
there, it needs a display and OpenGL 4.3 to run). Nothing else has to be installed: there is no Boost and no other system library
on Windows (on Linux the engine needs the X11/GL development packages, see `.github/workflows/ci.yml`).

```
git clone --recursive https://github.com/Blamad/ESTree.git
cd ESTree
cmake --preset vs2019
cmake --build --preset vs2019-debug --parallel        # or vs2019-release
ctest --test-dir build/vs2019 -C Debug --output-on-failure
```

 * Presets are in `CMakePresets.json`: `vs2019` (Visual Studio 2019 generator), `ninja-msvc` (Ninja with the MSVC from the current
   developer environment; run its commands through `scripts\msvc-env.cmd`, e.g. `scripts\msvc-env.cmd cmake --preset ninja-msvc`),
   `vs2026`, and the AddressSanitizer variants `vs2019-asan` / `ninja-msvc-asan`. Without presets,
   `cmake -S . -B build` and `cmake --build build --config Debug` work as well.
 * __setup.bat [preset] [--clean]__ does the configure step and links `Resources` next to the binaries (`build/<preset>/Resources`).
 * The first configure downloads doctest (network needed once). `-DESTREE_BUILD_TESTS=OFF` skips tests and the download,
   `-DESTREE_BUILD_TOOLS=OFF` skips `treegen_cli`, `-DESTREE_BUILD_ENGINE=OFF` builds only `treegen`, `treegen_cli` and the
   headless tests (default on Linux).
 * A clean build takes a few minutes (Bullet is the long pole).
 * Run the engine from `build/<preset>` (the working directory must contain `Resources`):
   `cd build/vs2019 && Debug\ESTree.exe --help`, then e.g. `Debug\ESTree.exe --scene 0`.

# Usage

Scene navigation: RMB + WSAD (Space and left alt to move vertically)<br/>
Object selection/pick up: LMB<br/>
Console toggle: ~<br/>

Application has number of commands available in built-in user console such as:
 * adding and removing objects from scene,
 * creating movable light sources,
 * generating Lindenmayer trees from configuration files (`tree`, `rtree`; `seed <n>` makes a tree reproducible, the seed of every
   generated tree is logged),
 * exporting models to .obj files,
 * help listing all actual commands.

For further initialisation launch options you can execute app with --help (`--scene 0..4`, window size, fullscreen).

## Generating trees without the engine

```
treegen_cli Resources/LindenmayerRules/parametricMonopodialTreeA.json --seed 1 --out out/tree.obj
```

The tool is built to `build/<preset>/tools/treegen_cli/<Config>/treegen_cli(.exe)`; options and exit codes are in
[`tools/treegen_cli/README.md`](tools/treegen_cli/README.md). The same seed gives a byte-identical OBJ file on every platform and
thread count. Rule-file formats (`.json`, legacy `.l`) are described in [`docs/architecture/overview.md`](docs/architecture/overview.md),
the library contract in [`docs/architecture/treegen-design.md`](docs/architecture/treegen-design.md).

# Tests

`ctest --test-dir build/<preset> -C Debug --output-on-failure` runs the headless unit tests (doctest): the whole `treegen`
library (loaders, rewriter, turtle, geometry, golden files of the original implementation, OBJ export), the engine adapter helpers, the
`treegen_cli` end-to-end checks, the `treegen` dependency-boundary check and, with the engine enabled, ownership tests of the engine
classes. No test opens a window or needs OpenGL. Details: [`tests/README.md`](tests/README.md).

# Documentation

Everything about how the modernization was planned, decided and verified is in [`docs/`](docs/): the master plan and task
table ([`docs/PLAN.md`](docs/PLAN.md)), the final summary with the open owner checklist
([`docs/tasks/T13-modernization-gate/SUMMARY.md`](docs/tasks/T13-modernization-gate/SUMMARY.md)), design decisions
([`docs/DECISIONS.md`](docs/DECISIONS.md)), the backlog ([`docs/BACKLOG.md`](docs/BACKLOG.md)) and the architecture notes
([`docs/architecture/`](docs/architecture/)). Contributor / agent instructions: [`CLAUDE.md`](CLAUDE.md).
