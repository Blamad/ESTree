# T09 — Remove Boost from the engine (outside `Models/Tree`)

**Depends on:** T00 · **Unblocks:** T12, T13 · **Size:** M · **Branch:** `task/T09-drop-boost-engine`
**Parallel with:** T01, T10 (disjoint files) — do **not** touch `ESEngine/ESTree/Models/Tree/**` (T03–T07 own it) or `CMakeLists.txt` (T13 removes the Boost link).

## Goal

Replace every Boost use in the engine and app code with C++17 standard facilities, behavior
unchanged, so that after T07 the whole project is Boost-free.

## Context to read

- `docs/architecture/overview.md` → *Build* (Boost usage list)
- `grep -rn "boost" ESEngine --include=*.cpp --include=*.h` (files outside `Models/Tree`): `ESTree.cpp`, `Models/{Grass,ModelExporter}.*`, `Engine/Core/Logger.*`, `Engine/GameObject/{GameObject,InstancedMesh,Mesh,Renderable}.*`, `Engine/Manager/{Context,LightsManager,MouseManager,PhysicsManager}.*`, `Engine/UI/{ConsoleComponent,ConsoleInterpreter,ConsoleUtils}.*`, `GraphicEngine/Core/Shader.h`, `GraphicEngine/Input/InputState.h`

## Scope

In:
1. Mapping: `BOOST_FOREACH` → range-for; `boost::shared_ptr`/`make_shared` → `std::`; `boost::filesystem` → `std::filesystem`; `boost::posix_time` / `microsec_clock` → `std::chrono`; `boost::thread`/mutex → `std::thread`/`std::mutex`; `boost::random` → `<random>`; `boost::algorithm::split/trim/to_lower` → small helpers in `Engine/UI/ConsoleUtils.h` or `Engine/Core/StringUtils.h`; `boost::lexical_cast` → `std::stoi/stof` with error handling at call sites; `boost::program_options` in `ESTree.cpp` → a small hand-written parser (`--help/-h`, `--width/-W N`, `--height/-H N`, `--scene/-s N`, `--fullscreen/-f`) that prints the same help text; **fix the help text** to say scenes `0-4` and list what each is.
2. Invalid CLI input prints a one-line error + usage and returns 1 (as today), no exceptions escaping.
3. Where an include only existed to pull in transitive std headers, add the correct `<...>` include.
4. A unit test for the argument parser if you extract it into a header-only `ESEngine/ESTree/CommandLine.h` (headless, in `tests/app/`); the engine target must not be needed to compile it.

Out of scope: the files in `Models/Tree/`, CMake link lines, any behavior/UX change besides the help text, dependency upgrades.

## Owned files

Everything listed under *Context to read* except `Models/Tree/**`; `tests/app/**`.

## Acceptance criteria

1. `grep -rln "boost" ESEngine` lists **only** files under `ESEngine/ESTree/Models/Tree/` (and nothing else).
2. Build Debug/Release OK; `/W4` warning count not above T00 baseline.
3. `ESTree.exe --help` prints all options; `--scene 9`/`--width abc` print an error + usage and exit 1 (record the actual outputs).
4. `ctest` green (parser tests if extracted).
5. Model export (`ModelExporter`) produces the same files byte-for-byte as before for a given input — verify with a headless test of the export's pure formatting part if feasible, otherwise list as human verification.

## Verification

Automated + **human:** console commands that use the touched code (`export`, light creation, cube commands) still work — give the command list in the report.

## Notes

- `std::filesystem::create_directory` returns `false` when it exists; the legacy code logs a warning only when it *creates* — keep that.
- `Context`/`Logger` timing: legacy used millisecond `local_time`; keep log formatting stable (don't reformat existing log lines).
