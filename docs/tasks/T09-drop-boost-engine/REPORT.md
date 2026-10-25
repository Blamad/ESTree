# T09 — Remove Boost from the engine (outside `Models/Tree`) — Report

**Result:** done · **Branch:** `task/T09-drop-boost-engine` · **Final commit:** `c6285c6` (merged into `modernization` as `8b2d9fc`)

The implementing subagent could not write this file (subagents may not write reports); the orchestrator wrote it from the
agent's hand-back and **re-verified the key claims on the merged `modernization`** (marked *verified*).

## What changed

- `BOOST_FOREACH` → range-for in 17 files; `ModelExporter` → `std::filesystem` (the "directory created" warning still fires only on creation).
- `Logger`, `Context`, console `fps` command → `std::chrono` (log line format unchanged). `Grass` → `std::mt19937` + `uniform_real_distribution`.
- `ConsoleUtils::split` hand-written with `boost::split` semantics.
- `boost::uuids` (not in the brief, found by grep) → new `Engine/GameObject/Uuid.h` (`Uuid` keeps its name; 128-bit random v4-style id, `operator==`/`<`, `uuidToString`), so `Scene.*` is untouched.
- New header-only `ESEngine/ESTree/CommandLine.h` (`CommandLine::parse`, `helpText`) used by `ESTree.cpp`; help text now names scenes 0–4; `NvOptimusEnablement` export kept.
- `CMakeLists.txt` untouched (Boost is still linked for `Models/Tree`; removed in T13). `bcrypt` link stays until then.

## Acceptance criteria

| Criterion | Evidence |
|---|---|
| 1. Boost only under `Models/Tree` | *verified*: grep `boost` (case-insens.) in `ESEngine/**/*.{h,cpp}` → only the 10 files in `ESEngine/ESTree/Models/Tree/` |
| 2. Build Debug/Release, warnings not above baseline | *verified* Debug (ninja-msvc, cl 19.51): `build exit=0`, **191 unique project warnings** (file+line+code, `Vendor/` excluded). Agent: Debug and Release OK on ninja-msvc, Debug OK on vs2019; agent's own count 207 vs 218 on the base commit (−11), same toolchain, slightly different counting. See *Baseline note* |
| 3. CLI behavior | *verified*: `--scene 9` → `error: the argument ('9') for option '--scene' is out of range (0-4)` + usage, exit 1; agent: `--help` exit 0, `--width abc` → error + usage, exit 1 |
| 4. ctest green | *verified*: `100% tests passed out of 2` (T01 harness present after merge). The parser unit test was not written in the branch (harness did not exist there) — follow-up |
| 5. ModelExporter byte-identical | **not verified** (needs the running engine); human check below |

## Smoke tests (*verified* on merged `modernization`, ninja-msvc Debug, NVIDIA GPU)

`Debug\ESTree.exe --scene 0…4` each: still running after 20 s, log contains `Rendering started`, no `Application Error` event.

## Baseline note (warnings)

Warning counts differ between toolchains and counting scripts: T00 baseline **143** was measured with `vs2019` (MSVC 14.29) on the
pre-T09 code; ninja-msvc (cl 19.51) gives more warnings (agent: 218 on the base commit with its method). Compare like with like:
**reference for `ninja-msvc` Debug is now 191** (method in `notes/baseline.md`); `vs2019` stays 143 pre-T09. Later tasks must not
increase the count on the toolchain they use.

## Deviations

- Worktree started on a stale commit; the agent ran `git reset --hard modernization` (clean tree) before working.
- `--scene` outside 0–4 and non-positive `--width/--height` now give error + usage + exit 1 (before: unknown scene started with no scene).
- `Logger::formatValue` keeps its old quirk of mis-padding some millisecond values (behavior preserved on purpose).

## Needs human verification

*Status 2026-10-25: owner reported "everything works as it should" after the T10 checks, but did not go through this list
item by item — treat as unconfirmed.*

1. Scene 0: console (`~`) `export x`, compare `models/x.obj` and `.mtl` with a pre-T09 build.
2. Console `fps` and `fps clr`.
3. Light and cube commands; click an object that has no name.
4. Grass in scene 1 or 2 looks as before.
5. `ESTree.exe -W 800 -H 600 -s 3 -f` (fullscreen path).

## Follow-ups

- Parser unit test for `CommandLine.h` (`tests/app/`) now that T01's harness exists.
- `Models/Tree/**` still uses Boost (T07); `CMakeLists.txt` Boost removal in T13.
