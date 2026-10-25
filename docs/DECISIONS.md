# Decision log

Append-only. Newest at the bottom. Format:

```
## D-NNN — Title (YYYY-MM-DD, task Txx)
Decision: ...
Why: ...
Consequences / alternatives rejected: ...
```

## D-001 — Keep the engine, isolate tree generation (2026-10-25, planning)
Decision: The engine stays as is (cleanup only). Tree generation moves into a pure `treegen` static library with no dependency on `Engine/`/`GraphicEngine/`; legacy `Lindenmayer*` classes become adapters, then are deleted (T07).
Why: Owner wants to keep the current engine now but be able to split the generator out later (e.g. as a Blender/Godot/Unity plugin core, CLI or WASM tool).
Consequences: Boundary enforced by a CTest include-grep (T03) and by building `treegen` as its own CMake target with a minimal include path.

## D-002 — Test framework: doctest via FetchContent (2026-10-25, planning)
Decision: Use doctest (single header, pinned tag `v2.4.11`) fetched by CMake `FetchContent`, behind option `ESTREE_BUILD_TESTS` (default ON). T01 may deviate if offline builds prove problematic; record it here.
Why: Smallest dependency, fast compile, works with CTest.
Consequences: Network needed on first configure; T11 caches it in CI.

## D-003 — Library layout and naming (2026-10-25, planning)
Decision: Directory `ESEngine/TreeGen/` (headers in `include/treegen/`, sources in `src/`), CMake target `treegen`, namespace `treegen`, include as `#include <treegen/Xxx.h>`. Tests in `tests/`, tools in `tools/`.
Why: Self-contained layout that can be lifted out of the repo unchanged.
Consequences: The engine's include root stays `ESEngine/`; adapters add `ESEngine/TreeGen/include` via `target_link_libraries(ESTree PRIVATE treegen)`.

## D-004 — Rule-file format v1 is frozen; extensions are additive (2026-10-25, planning)
Decision: Existing JSON (`parameters`, `axiom`, `rules[{symbol,production,probability,initialDepth}]`) and `.l` formats keep working unchanged. New optional keys (`seed`, `maxModules`, later `condition`) are additive.
Why: 20+ shipped rule files and the thesis examples must keep working.
Consequences: Loader tests iterate over every file in `Resources/LindenmayerRules`.

## D-005 — Build: CMake ≥ 3.21, presets, explicit vc140 Boost libs (2026-10-25, T00)
Decision: Root `cmake_minimum_required(3.21)` (presets schema v3). Presets pass `CMAKE_POLICY_VERSION_MINIMUM=3.5` so CMake 4.x accepts vendored projects (Bullet 2.4.3, FreeType 2.8) without editing `Vendor/`. Boost is configured through `BOOST_LIB` (env or cache) and linked explicitly (`date_time thread chrono system atomic filesystem program_options`, `vc140` static libs) with `BOOST_ALL_NO_LIB`, because auto-link would request non-existent `vc142/vc145` names. `bcrypt` is linked on Windows for `boost::uuids`. C++17 is the project standard.
Why: Owner installed CMake 4.4.4; the bundled VS2019 CMake 3.20 cannot read schema v3 and cannot generate for VS 2026.
Consequences: Revisit when Boost is removed (T13). `Vendor/` untouched.

## D-006 — Typed component access keeps the `ComponentType` enum (2026-10-25, T10)
Decision: `GameObject::getComponentAs<T>(ComponentType)` / `getComponentsAs<T>(ComponentType)` (and the forwarding `Component::getComponentAs<T>`) are `static_cast` accessors; `Component` has a virtual destructor so Debug builds `assert(dynamic_cast<T*>(...))`. `getComponent(TYPE)` is unchanged. Casts in `ESEngine/Engine` were replaced; the ones in `ESEngine/ESTree` (scenes, console commands, `Models/`) still use C-casts.
Why: Smallest change that fits the existing enum scheme; no ECS redesign (BACKLOG).
Consequences: A wrong `T` for a type (e.g. `InstancedMesh` read as `Mesh`) now asserts in Debug instead of being silent UB. Call sites in `ESTree/` can migrate at will.

## D-007 — ASan build needs the VS 2019 Build Tools toolset on this machine (2026-10-25, T10)
Decision: `ESTREE_SANITIZE=ON` (presets `ninja-msvc-asan`, `vs2019-asan`) adds `/fsanitize=address` to ESTree, the tests and the vendored libraries. The x64 ASan runtime exists only in the VS 2019 Build Tools here (VS "18" ships the i386 runtime only), so `ninja-msvc-asan` is configured with `VCVARS64` pointing at the 2019 `vcvars64.bat`. The 14.29 ASan runtime only starts together with the 14.29 CRT DLLs (a newer `vcruntime140.dll` in System32 gives 0xC0000142), so the CRT DLLs are copied next to each executable after linking. GCC/Clang get `-fsanitize=address,undefined`.
Why: Without the copy no ASan executable runs on this machine.
Consequences: ASan executables are self-contained next to their DLLs; on a machine whose System32 CRT matches the toolset the copy is redundant but harmless.

## D-008 — CI: engine behind `ESTREE_BUILD_ENGINE`; Linux builds the engine against system Boost (2026-10-25, T11)
Decision: `ESTREE_BUILD_ENGINE` (default ON on Windows, OFF elsewhere) gates GLFW, Bullet, FreeType, the `ESTree` executable and `tests/engine`. `BOOST_LIB` is required only for an MSVC engine build; `BOOST_INCLUDEDIR` overrides the Boost header directory. Non-MSVC engine builds link system Boost (`boost_*` libs) instead of the vc140 names. Windows CI runs with the engine OFF (runners have no vc140 libs); Linux CI builds the engine ON (compile+link only) and OFF. A handful of MSVC-only constructs were made standard C++ (opaque enums with `: int`, `const T&` for temporaries, no extra qualification, `<sys/stat.h>`, `windows.h` guarded).
Why: The full engine cannot be built on a stock Windows runner until Boost is removed (T07/T13); Linux compile coverage catches non-portable code early.
Consequences: Windows CI does not compile the engine; the owner's local `ninja-msvc`/`vs2019` build remains the Windows engine check until T13.

## D-009 — treegen loaders replace the legacy params loader; loader defects #1, #3, #10-#13 fixed (2026-10-25, T03)
Decision: `treegen::loadTreeParams` (hand-written JSON/`.l` loading, no `std::regex`, no Boost) is the only loader; `LindenmayerTreeParams` is an adapter that inherits `treegen::TreeParams` and keeps `Rule`/`addRule`/`getRules`/`getAvailableRules`/`radians`. Additive API beyond the design doc: `parseTreeParamsLegacy`, `substituteParameters`. Optional keys `seed`/`maxModules` live at the JSON top level or in `parameters` (`define seed N` / `define maxModules N` in `.l`). A missing file, invalid JSON, a missing/mistyped `axiom` or `rules`, and any wrongly typed value is a `treegen::LoadError` (`<path>[:<line>]: <reason>`), which replaces the crash/UB of the old loader.
Flipped KNOWN BUG expectations (tests renamed, `KNOWN BUG:` prefix dropped; list in `tests/golden/KNOWN_BUGS.md`):
- #1 adjacent identical parameters (`F(a1,a1)` is now `F(35.000000,35.000000)`) and #3 parameter at the start of a production: custom parameters are replaced as whole words (boundary `[A-Za-z0-9]`, same class as before; also at the end of the text). The T02 example `a1F` is a different identifier and stays unreplaced; the tested start-of-text case is `a1+F`.
- #10 custom parameters are substituted into the axiom (`legacy/parametricTree2.l`); substitution now runs after the whole file is read, for the axiom and all productions.
- #11 bare-number header lines of old `.l` files (`advancedTree.l`, `testX/Y/Z.l`) are depth, initialLength, initialRadius, angle (degrees) while no axiom has been seen; `depth` defaults to 0 instead of being uninitialized.
- #12 the default angle is `radians(15)` for loaded files too (it was 15 radians), `define tropism x y z` is parsed and normalized. Visible effect: none for the 39 files that set `angle` (all 21 JSON files and 18 `.l` files); the four header-form `.l` files now use their header angle (7 or 15 degrees) instead of 15 radians.
- #13 (new) a UTF-8 BOM at the start of a `.l` file is skipped; `legacy/test.l` used to get a junk rule whose symbol was the BOM. Also: `\r` is stripped, `.l` tokens are separated by runs of spaces and tabs, values and probabilities are parsed strictly (`LoadError` instead of `atof` returning 0).
Regenerated goldens (reviewed by eye, nothing else changed; the other 37 params goldens and all other solver goldens are byte-identical): `params/legacy__advancedTree.l`, `legacy__testX.l`, `legacy__testY.l`, `legacy__testZ.l`, `legacy__parametricTree2.l`, `legacy__test.l`; solver goldens that are a function of the loaded axiom/depth: `solver/legacy__parametricTree2.l.d0..d3`, new `solver/legacy__advancedTree.l.d4`.
Why: Review Focus 2 and the "behavior-preserving except listed bug fixes" rule; the loader had to be portable (the regex-based loader loses text on libstdc++).
Consequences: Callers of `LindenmayerTreeParams(path)` can now see a `LoadError` for a malformed file (ConsoleTreeCommand does not catch it yet: T07). Trees built from the four header-form `.l` files change (they used 15 radians and defaults); needs a human look if those files matter. The solver is untouched; bugs #2, #4-#9 stay with T04.
