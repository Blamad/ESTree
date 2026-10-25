# Tests

Unit tests use [doctest](https://github.com/doctest/doctest) (v2.4.11, fetched by CMake on first
configure) and run through CTest. Tests are headless: never open a window or touch OpenGL.

- Disable: `-DESTREE_BUILD_TESTS=OFF` (doctest is then not fetched).
- Run all: `ctest --test-dir build/vs2019 -C Debug --output-on-failure`
- Run one: `ctest --test-dir build/vs2019 -C Debug -R smoke_tests`, or run the exe
  (`build/vs2019/tests/Debug/smoke_tests.exe -tc="harness*"`).
- Add an executable: put sources in `tests/<area>/<area>_tests.cpp` and in `tests/CMakeLists.txt` add
  `estree_add_test(<area>_tests SOURCES <area>/<area>_tests.cpp LIBS <targets>)`.
  Exactly one source per executable defines `DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` before including
  `<doctest/doctest.h>`.
- Repo files: use the `ESTREE_REPO_ROOT` define (absolute path, no trailing slash), e.g.
  `std::string(ESTREE_REPO_ROOT) + "/Resources/..."`. The working directory is the repo root too.
- Naming: files `<area>_tests.cpp`; cases `TEST_CASE("<unit>: <behavior>")`.
- Do not link GLFW/glad/Bullet/FreeType into test executables.

## Reproducing CI locally

CI is `.github/workflows/ci.yml`. The root option `ESTREE_BUILD_ENGINE` (default ON on Windows, OFF
elsewhere) decides whether the engine executable and its dependencies (GLFW, Bullet, FreeType, Boost libs)
are configured. With OFF only the standalone targets and tests are built; `BOOST_LIB` is not needed.

- **Windows jobs** (MSVC, Debug and Release) run with the engine OFF, because GitHub runners lack the
  prebuilt vc140 Boost libs. Boost *headers* come from `-DBOOST_INCLUDEDIR=<dir containing boost/>`
  (CI downloads the Boost 1.72.0 headers once and caches them). Locally:
  `cmake -S . -B build/ci -DESTREE_BUILD_ENGINE=OFF`, `cmake --build build/ci --config Debug`,
  `ctest --test-dir build/ci -C Debug --output-on-failure`. The full engine build stays local:
  set `BOOST_LIB` and use the presets (see `CLAUDE.md`).
- **Linux jobs** (GCC, Release): engine ON needs `ninja-build libgl1-mesa-dev libx11-dev libxrandr-dev
  libxinerama-dev libxcursor-dev libxi-dev libxext-dev` plus the `libboost-*-dev` packages listed in the
  workflow (system Boost instead of `BOOST_LIB`); it only compiles and links, nothing is run with a display.
  `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DESTREE_BUILD_ENGINE=ON`, then
  `cmake --build build --parallel`, `ctest --test-dir build --output-on-failure`.
  The engine-OFF Linux entry is the same without the packages, using `-DESTREE_BUILD_ENGINE=OFF`.
- Without a Linux machine, run the same commands in `ubuntu:24.04` (install `cmake g++ git`, mount the repo
  with its submodules at `/src`, build in a directory outside the mount).
- doctest is fetched by CMake; CI caches it through `-DFETCHCONTENT_BASE_DIR`.
- Lint the workflow: `docker run --rm -v <repo>/.github:/repo/.github -w /repo rhysd/actionlint:latest`
  (actionlint needs a `.git` entry in the working directory to find the project).
