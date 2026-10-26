# T03 — `treegen` scaffold, parameters and loaders — Report

**Result:** done · **Branch:** `task/T03-treegen-scaffold-params` · **Final commit:** `b41386c` (merged into `modernization` as `7724a01`)

The implementing subagent could not write this file; the orchestrator wrote it from the agent's hand-back and
**re-verified the headline claims on the merged tree** (marked *verified*).

## What changed

- **New module `ESEngine/TreeGen/`**: static lib `treegen` (+ alias), headers `include/treegen/{Error,Rng,Module,TreeParams}.h`, sources `src/{Rng,Module,Loader}.cpp`,
  signatures as in `docs/architecture/treegen-design.md`.
  - `Rng`: `derive` via `seed_seq`, hand-rolled `uniform01` → identical streams on every platform.
  - `parseModules`/`toString` round-trip; malformed text → `RewriteError` with offset.
  - Loaders (JSON + `.l`): hand-written, no `std::regex`, no Boost; `LoadError` messages `path[:line[:col]]: reason`; optional `seed`/`maxModules` (top level or in `parameters`; `define seed N` in `.l`).
- **Root `CMakeLists.txt`**: engine glob excludes `ESEngine/TreeGen`; `add_subdirectory` before the global `include_directories` and outside the engine gate; `ESTree` links `treegen`.
- **Legacy adapter**: `LindenmayerTreeParams` now inherits `treegen::TreeParams` (only `LindenmayerTreeParams.*` changed outside TreeGen); solver untouched.
- **Tests** (`tests/treegen/`, one `treegen_tests` executable globbing `*_tests.cpp` plus `treegen_boundary` and `treegen_boundary_selftest` with 18 planted cases).

## Acceptance criteria

| Criterion | Evidence |
|---|---|
| Build + ctest green, legacy characterization satisfied by the new loader | *verified* (ninja-msvc Debug, merged): build exit 0, **`100% tests passed out of 101`** (41 before + 60 new) |
| Every shipped rule file loads through `treegen::loadTreeParams` | test iterates all 43 files (agent) ; all 21 JSON files reproduce the T02 goldens bit for bit, also on Linux |
| Boundary enforced by compile and by test | *verified*: `treegen_boundary` and `treegen_boundary_selftest` pass. Agent: planting `#include "Engine/Core/Logger.h"` → `fatal error C1083` and `Module.cpp:1: forbidden include (engine layer)`; compile line has one `-I` (own include dir) and two `-external:I` (glm, rapidjson) |
| `ESTree` still builds and links | *verified*: merged build exit 0; agent: `treegen.lib` on the link line, no TreeGen source compiled inside `ESTree.dir`; smoke launch `--scene 0` alive after 20 s |
| Malformed inputs → `LoadError` (Review Focus #2) | tests prefixed `Review Focus 2:` (missing file, `{}`, truncated JSON, `rules` not an array, wrong types…) |
| `/W4` clean for treegen | agent: 0 warnings in `ESEngine/TreeGen`, `tests/treegen`, `LindenmayerTreeParams.*`; GCC `-Wall -Wextra -Wpedantic`: none |
| **Linux** (added requirement) | *verified*: Docker ubuntu:24.04 / GCC 13, engine OFF: `100% tests passed, 0 tests failed out of 62` |

## Intentional behavior changes (D-009 in `docs/DECISIONS.md`)

KNOWN BUGS #1, #3, #10, #11, #12 fixed in the loader (tests renamed, `KNOWN BUG:` prefix dropped, `tests/golden/KNOWN_BUGS.md` updated); new fix **#13**:
UTF-8 BOM in `legacy/test.l` no longer creates a junk rule. Regenerated goldens (reviewed): params for `legacy__{advancedTree,testX,testY,testZ,parametricTree2,test}.l`,
solver `legacy__parametricTree2.l.d0..d3` (+ new `legacy__advancedTree.l.d4`); all other goldens byte-identical.

## Interface changes

`docs/architecture/treegen-design.md`: additive only — `parseTreeParamsLegacy(text, name)`, `substituteParameters(text, values)`, a "Loader behaviour" section (D-009).

## Deviations

- `target_link_libraries(ESTree treegen)` without `PRIVATE` (CMake forbids mixing signatures on one target).
- The T02 example for bug #3 (`a1F`) is a different identifier under whole-word substitution; the flipped test uses `a1+F` and checks `a1F` stays unreplaced.
- Boundary script additionally forbids `ESTree/`, `GL/`, `<iostream>`. Extra files: `tests/treegen/main.cpp`, `tests/treegen/data/*.json`, a paragraph in `tests/README.md`.
- The Linux run caught that `.cmake` check scripts need `cmake_minimum_required` (on CMake 3.28 `while(TRUE)` otherwise never loops) — fixed in `d8d98b7`.

## Needs human verification

The four header-form `.l` files (`advancedTree.l`, `testX.l`, `testY.l`, `testZ.l`) now use their header values (depth, length, radius, angle in degrees) instead of defaults / 15 radians,
so trees generated from them look different. Check with `tree file <name>` in the console if those files matter; all 21 JSON files and the 18 `.l` files that set `angle` are unchanged.

## Follow-ups

- `ConsoleTreeCommand` does not catch `LoadError` (a malformed file now throws instead of hitting UB) — T07.
- CTest discovery breaks on `[`, `]`, `;` in doctest case names (documented in `tests/README.md`).
- T04 must provide a portable rewriter; flips KNOWN BUGS #2, #4–#9.
