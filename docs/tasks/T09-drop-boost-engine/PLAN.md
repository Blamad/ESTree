# T09 — Remove Boost from the engine (outside `Models/Tree`) — Implementation Plan

**Status:** in-progress · **Branch:** `task/T09-drop-boost-engine` · **Base:** `modernization @ 3e1cbf1`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** Replace every Boost use outside `ESEngine/ESTree/Models/Tree/**` with C++17 std facilities, behavior unchanged.

**Approach:** Mechanical replacement in small groups, building Debug after each group. The CLI parser is extracted into
header-only `ESEngine/ESTree/CommandLine.h` (no engine dependency) so it can be unit-tested headlessly if the `tests/`
harness (T01, parallel) exists when finished. CMake is untouched (Boost stays linked until T13).

## Files

- Create: `ESEngine/ESTree/CommandLine.h` — hand-written parser + help text.
- Create: `ESEngine/Engine/GameObject/Uuid.h` — tiny 128-bit random id (replaces `boost::uuids`, which the brief did not list but the grep finds).
- Modify: every non-`Models/Tree` file found by `grep -rli boost ESEngine` (Logger, Context, ConsoleInterpreter/Utils/Component, Mesh/InstancedMesh/Skybox/Renderable, GameObject, Light/Mouse/PhysicsManager, Grass, ModelExporter, Shader.h, InputState.h, ESTree.cpp).
- Cross-ownership (T10): `GameObject.{h,cpp}`, `PhysicsManager.{h,cpp}`, `*Manager` — only Boost lines/includes change. `Scene.*` is untouched: `Uuid` keeps its name and gains `operator<`.

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | `Uuid` becomes a struct {hi, lo} with `operator<`/`==` and `uuidToString`, random from `std::mt19937_64` seeded by `std::random_device` | Same type name keeps `Scene` (T10's file) unchanged; only used as map key and physics-body fallback name |
| 2 | Context start time = `std::chrono::system_clock` time_point; fps uses `duration_cast<milliseconds>` | Same wall-clock semantics as `local_time`; fps result identical |
| 3 | Logger time: `system_clock` + `localtime_s`, ms from epoch remainder; fixed `HH:MM:SS.mmm` via existing `formatValue` | Keep log format unchanged |
| 4 | Grass RNG: `std::mt19937` seeded `time(0)` + `uniform_real_distribution<float>(-1,1)` behind a static function-object member with same `randomGenerator()` call syntax | Same call sites in `Grass.cpp` |
| 5 | `ConsoleUtils::split` keeps `boost::split` semantics (split on any char of `by`, keep empty tokens) | Behavior preserved |

## Steps

- [ ] 1 BOOST_FOREACH -> range-for (all files) + std includes; build Debug
- [ ] 2 uuid, time (Logger/Context/fps), filesystem (ModelExporter), random (Grass), split (ConsoleUtils); build Debug
- [ ] 3 `CommandLine.h` + ESTree.cpp, help text scenes 0-4; build Debug; record `--help`, `--scene 9`, `--width abc` outputs
- [ ] 4 Optional headless test in `tests/app/` only if T01 harness exists
- [ ] 5 Build Debug+Release, count /W4 warnings, smoke test scenes 0-4, grep acceptance
- [ ] 6 REPORT.md, status `review`

Each group committed as `T09: ...`.

## Review focus covered

Behavior-preserving log format; `create_directory` warning only when created; no exceptions escaping `main` on bad CLI.

## Deviations from the brief

- `boost::uuids` (GameObject/PhysicsManager) is not in the brief's mapping; replaced as per decision 1.

## Blockers

(none)
