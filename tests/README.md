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
