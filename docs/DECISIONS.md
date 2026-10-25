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
