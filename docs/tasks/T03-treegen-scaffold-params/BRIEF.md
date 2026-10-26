# T03 — `treegen` scaffold, parameters and loaders

**Depends on:** T02 · **Unblocks:** T04, T05 · **Size:** M · **Branch:** `task/T03-treegen-scaffold-params`

## Goal

Create the engine-independent module skeleton (`ESEngine/TreeGen/`, target `treegen`) with the
shared vocabulary types (`Error`, `Rng`, `Module`, `TreeParams`, `Rule`) and the rule-file
loaders (`.json`, `.l`), and turn the legacy `LindenmayerTreeParams` into a thin adapter over it
— with the boundary enforced by the build and a test.

## Context to read

- `docs/architecture/treegen-design.md` (**the contract — implement exactly these names**), `docs/DECISIONS.md` D-001, D-003, D-004
- `ESEngine/ESTree/Models/Tree/LindenmayerTreeParams.{h,cpp}` (the code you are porting)
- `tests/golden/params/**`, `tests/golden/KNOWN_BUGS.md`, `tests/legacy_characterization/**` (from T02)
- `Resources/LindenmayerRules/` (all files must keep loading)

## Scope

In:
1. `ESEngine/TreeGen/CMakeLists.txt`: STATIC library `treegen` (+ alias `treegen::treegen`), `file(GLOB_RECURSE ... CONFIGURE_DEPENDS src/*.cpp include/*.h)`, `target_compile_features(treegen PUBLIC cxx_std_17)`, include dirs limited to `include/`, `Vendor/glm`, `Vendor/rapidjson/include` (the root `include_directories` must **not** leak engine paths into this target — add `treegen` before the global `include_directories` call or use target-scoped includes; verify with a deliberate bad include).
2. Headers/sources per the design doc: `Error.h`, `Rng.h/.cpp`, `Module.h/.cpp` (`parseModules`, `toString`), `TreeParams.h`, `Loader.cpp` (`loadTreeParams`, `parseTreeParamsJson`).
3. Loader behavior = legacy behavior for all shipped files (T02 golden params must match), **plus**: missing file / invalid JSON / missing `axiom` or `rules` / wrong types → `LoadError` with path and reason (no crash). Optional JSON keys `seed`, `maxModules`. `.l` format `tropism` (currently `//TODO parse vector`) may stay unsupported but must not crash.
4. Replace Boost split/foreach/regex-heavy substitution with std code. The custom-parameter substitution must produce the same text as legacy for shipped files; fix the adjacent-occurrence case if T02 `KNOWN_BUGS.md` confirms it (flip that test, log in `DECISIONS.md`).
5. `LindenmayerTreeParams` (legacy header) keeps its public API but stores/loads through `treegen::TreeParams` (composition or inheritance); `Rule` legacy type may remain as a facade. Engine callers (`ConsoleTreeCommand`, scenes) must compile unchanged.
6. Link the engine: `target_link_libraries(ESTree PRIVATE treegen)`; make sure the engine's global include dirs still resolve.
7. Tests under `tests/treegen/`: one executable `treegen_tests` built from `file(GLOB ... CONFIGURE_DEPENDS *_tests.cpp)` in `tests/treegen/CMakeLists.txt` (so T04–T08 add `xxx_tests.cpp` files without touching CMake; name yours `module_tests.cpp`, `rng_tests.cpp`, `loader_tests.cpp`). `Module` round trip (`parseModules("F(2)T+(45)A")` → toString), `Rng` determinism (same seed ⇒ same sequence; `derive` streams differ), loader tests for every shipped file (compared to T02 goldens), error cases, and the **boundary tests**: (a) `treegen_boundary` CTest — a CMake script that greps `ESEngine/TreeGen/**` for forbidden includes (`Engine/`, `GraphicEngine/`, `glad`, `GLFW`, `btBulletDynamicsCommon`, `ft2build`, `stb_`, `boost/`, `windows.h`) and fails with the offending file:line; (b) a negative self-test proving the script catches a planted violation.

Out of scope: rewriting (T04), interpreter (T05), removing Boost from legacy solver/tree classes.

## Owned files

`ESEngine/TreeGen/{CMakeLists.txt,include/treegen/{Error,Rng,Module,TreeParams}.h,src/{Rng,Module,Loader}.cpp}`, `tests/treegen/{module,rng,loader,boundary}*`, `ESEngine/ESTree/Models/Tree/LindenmayerTreeParams.*`, root CMake lines adding the subdirectory/link.

## Acceptance criteria

1. Build + `ctest` green; `legacy_characterization_tests` still green, with the params goldens satisfied by the new loader.
2. Every file in `Resources/LindenmayerRules/` loads through `treegen::loadTreeParams` (test iterates the directory).
3. `treegen_boundary` passes; adding `#include "Engine/Core/Logger.h"` to a treegen source makes **both** the compile and the boundary test fail (show in `REPORT.md`).
4. `ESTree` target builds and links; no engine source outside `LindenmayerTreeParams.*` changed.
5. Malformed inputs (missing file, `{}`, truncated JSON, `rules` not an array) throw `treegen::LoadError` (Review Focus #2).
6. `treegen` compiles at `/W4` without warnings.

## Verification

Automated. **Human check (optional):** `ESTree.exe --scene 0` still shows the same tree (loader semantic parity).

## Notes

- `TreeParams::angle` is stored in radians; JSON gives degrees (legacy converts with `radians()`).
- `tropism` from JSON is normalized; default `(0,-1,0)`.
- Keep rule declaration order per symbol (stochastic selection depends on it).
- Don't introduce `std::regex` in the new loader; hand-written scanning is faster and testable.
