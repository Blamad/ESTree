# ESTree
[![CI](https://github.com/Blamad/ESTree/actions/workflows/ci.yml/badge.svg)](https://github.com/Blamad/ESTree/actions/workflows/ci.yml)

Simple game engine capable of rendering scene, performing physics simulations and generating tree models using Lindenmayer sytem formal grammar.
Created as a part of masters dissertation.

YouTube clip demonstrating project capabilities:<br/>
[![main_scene_clip](https://img.youtube.com/vi/CheCvFOeFZI/0.jpg)](https://youtu.be/CheCvFOeFZI)

ESTree is using few libraries:
 * __BOOST__: general perfomance purposes, multithreading
 * __GLFW, GLM, GLAD, OpenGL__: scene rendering
 * __BulletPhysics__: physics simulation (collisions, mouse picking, collapsing towers made of blocks) 
 * __RapidJSON__: parsing configuration files
 * __FreeType__: text rendering
 * __STB__: texture loading
 * __Freetype__: fonts

Single tree scene clip:<br/>
[![single_tree_scene_clip](https://img.youtube.com/vi/QXZW_28LS78/0.jpg)](https://youtu.be/QXZW_28LS78)

Sample screenshots presenting current engine abilities regarding tree generation:

 * Simple bush model: <br/>
  ![Sample screenshot](/images/withHdr.PNG) 
 * Ternary tree models generated from rules described in "The Algorithmic Beauty of Plants":<br/>
  ![Sample screenshot](/images/ternaryTreeA.PNG)
  ![Sample screenshot](/images/ternaryTreeE.PNG)
 
 
 # Project setup
 Requirements: Windows x64, Visual Studio 2019 (or newer) C++ toolset, __CMake 3.21+__ and prebuilt x64 Boost 1.72 (static `vc140` libs; the engine still uses it, it is being phased out). Set the env variable __BOOST_LIB__ to a directory containing `Includes/` and `Libs/`. </br>
 Clone recursively (`git clone --recursive`), then configure with a preset from __CMakePresets.json__: </br>
 `cmake --preset vs2019`, then `cmake --build --preset vs2019-debug` (or `ninja-msvc`: `scripts\msvc-env.cmd cmake --preset ninja-msvc`, which sets up the MSVC environment; `vs2026` needs a registered VS 2026 instance). </br>
 __setup.bat [preset] [--clean]__ does the configure step and links `Resources` next to the binaries (`build/<preset>/Resources`). Run `build/<preset>/Debug/ESTree.exe` from `build/<preset>` (the working directory must contain `Resources`). </br>
 FreeType is linked statically, so no extra `freetype.dll` is needed.
 
 # Usage
 Scene navigation: RMB + WSAD (Space and left alt to move vertically)<br/>
 Object selection/pick up: LMB<br/>
 Console toggle: ~<br/>
 
 Application has number of commands available in built-in user console such as:
 * adding and removing objects from scene,
 * creating movable light sources,
 * generating Lindenmayer trees from configuration files,
 * exporting models to .obj files,
 * help listing all actual commands.
 
 For further initialisation launch options you can execute app with --help.