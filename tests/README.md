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
- Naming: files `<area>_tests.cpp`; cases `TEST_CASE("<unit>: <behavior>")`. Do not put `[`, `]` or `;` in a case name:
  CTest discovery (`doctest_discover_tests`) writes names into a CMake list and breaks on them.
- `treegen` has one executable, `treegen_tests`, built from every `tests/treegen/*_tests.cpp` (CMake globs them, so a new
  area is just a new file; `tests/treegen/main.cpp` holds the doctest main). Its fixtures live in `tests/treegen/data/`.
  `treegen_boundary` (`boundary_check.cmake`) fails when a file under `ESEngine/TreeGen/` includes something outside the
  standard library, glm and rapidjson; `treegen_boundary_selftest` proves the check catches planted violations. Both run
  without the engine, so they and `treegen_tests` also run on the Linux and Windows CI jobs.
- `tree_adapter_tests` (`tests/adapter/`, T07) tests the engine's tree adapter without GL: the pure helpers in
  `ESEngine/ESTree/Models/Tree/TreeMeshConversion.h` (vertex/index/leaf conversion, seed rules) and the adapter path of every shipped rule file
  through `treegen::generateTree`. It needs only `treegen` and glm, so it also runs with the engine OFF. The console command and the GL upload
  are human-verified (see the T07 report).
- `tests/golden/` holds the frozen reference data of the pre-treegen implementation (loader and solver output, `KNOWN_BUGS.md`); the
  treegen tests compare against it (`loader_golden_tests`, `rewriter_golden_tests`, `golden_data_tests`). The legacy pipeline and its
  characterization test are gone (T07).
- Do not link GLFW/glad/Bullet/FreeType into test executables.

## Reproducing CI locally

CI is `.github/workflows/ci.yml`. The root option `ESTREE_BUILD_ENGINE` (default ON on Windows, OFF
elsewhere) decides whether the engine executable and its dependencies (GLFW, Bullet, FreeType)
are configured. With OFF only the standalone targets and tests are built. There is no Boost anywhere.

- **Windows jobs** (MSVC, Debug and Release) build the full engine (`-DESTREE_BUILD_ENGINE=ON`; the exe is compiled and linked,
  never started on the runner). No generator is named, so CMake takes the newest Visual Studio of the runner image. Locally:
  `cmake -S . -B build/ci -A x64 -DESTREE_BUILD_ENGINE=ON`, `cmake --build build/ci --config Debug --parallel`,
  `ctest --test-dir build/ci -C Debug --output-on-failure` (or use the presets, see `CLAUDE.md`).
- **Linux jobs** (GCC, Release): engine ON needs `ninja-build libgl1-mesa-dev libx11-dev libxrandr-dev
  libxinerama-dev libxcursor-dev libxi-dev libxext-dev`; it only compiles and links, nothing is run with a display.
  `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DESTREE_BUILD_ENGINE=ON`, then
  `cmake --build build --parallel`, `ctest --test-dir build --output-on-failure`.
  The engine-OFF Linux entry is the same without the packages, using `-DESTREE_BUILD_ENGINE=OFF`.
- Without a Linux machine, run the same commands in `ubuntu:24.04` (install `cmake g++ git`, mount the repo
  with its submodules at `/src`, build in a directory outside the mount).
- doctest is fetched by CMake; CI caches it through `-DFETCHCONTENT_BASE_DIR`.
- Lint the workflow: `docker run --rm -v <repo>/.github:/repo/.github -w /repo rhysd/actionlint:latest`
  (actionlint needs a `.git` entry in the working directory to find the project).
