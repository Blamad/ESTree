# T03 — `treegen` scaffold, parameters and loaders — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: use `superpowers:subagent-driven-development`
> or `superpowers:executing-plans`. Steps use checkbox (`- [ ]`) syntax.

**Status:** in-progress · **Branch:** `task/T03-treegen-scaffold-params` · **Base:** `modernization @ e2a1d9b`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** Create the engine-independent `treegen` static library (`Error`, `Rng`, `Module`, `TreeParams`, `.json`/`.l` loaders)
with the boundary enforced by the build and by a CTest, and turn the legacy `LindenmayerTreeParams` into a thin adapter over it.

**Approach:** `treegen` is defined in the root `CMakeLists.txt` *before* the global `include_directories`/`add_definitions`
calls and additionally has its `INCLUDE_DIRECTORIES` property reset, so its include path is exactly its own `include/`, glm
and rapidjson; a planted `#include "Engine/..."` fails to compile. All loaders are hand-written (no `std::regex`, no Boost).
`LindenmayerTreeParams` publicly inherits `treegen::TreeParams` (so every field engine callers read keeps its name and
type) and keeps `Rule`, `addRule`, `getRules`, `getAvailableRules`, `radians` as a facade.

## Files

- Create: `ESEngine/TreeGen/CMakeLists.txt` — STATIC `treegen` + alias, `GLOB_RECURSE ... CONFIGURE_DEPENDS`, C++17, minimal includes
- Create: `ESEngine/TreeGen/include/treegen/{Error,Rng,Module,TreeParams}.h`, `src/{Rng,Module,Loader}.cpp`
- Create: `tests/treegen/{CMakeLists.txt,main.cpp,rng_tests.cpp,module_tests.cpp,loader_tests.cpp,boundary_check.cmake,boundary_selftest.cmake}`
- Modify: `CMakeLists.txt` — exclude `ESEngine/TreeGen/` from the engine glob, `add_subdirectory(ESEngine/TreeGen)` outside the engine gate and before the global includes, `treegen` into the engine's `target_link_libraries`
- Modify: `tests/CMakeLists.txt` — `add_subdirectory(treegen)`, `LIBS treegen` for `legacy_characterization_tests`
- Modify: `ESEngine/ESTree/Models/Tree/LindenmayerTreeParams.{h,cpp}` — adapter
- Modify: `tests/legacy_characterization/legacy_characterization_tests.cpp` — flip KNOWN BUG #1, #3, #10, #11, #12; `definesDepth` knows the header form
- Modify: `tests/golden/params/*` (only affected files), `tests/golden/solver/*` (only files whose axiom/depth changed, see Decisions), `tests/golden/KNOWN_BUGS.md`, `tests/golden/solver/README.md`
- Modify: `docs/DECISIONS.md` (append), `docs/architecture/treegen-design.md` (additive helpers), `tests/README.md` (one line about `treegen_tests`)

## Interfaces

- Consumes: nothing (first treegen task).
- Produces: exactly the names of `docs/architecture/treegen-design.md` (`Error`/`LoadError`/`RewriteError`/`InterpretError`, `Rng{Rng(seed), derive, uniform01}`,
  `Module`, `ModuleString`, `parseModules`, `toString`, `Rule`, `TreeParams`, `loadTreeParams`, `parseTreeParamsJson`), plus additive helpers (design doc + D-009):
  - `TreeParams parseTreeParamsLegacy(std::string_view text, std::string name = {})` — the `.l` text parser (in-memory tests)
  - `std::string substituteParameters(std::string_view text, const std::map<std::string, float>& values)` — the custom-parameter substitution used by both loaders and by the legacy adapter

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | `target_link_libraries(ESTree ... treegen)` uses the existing plain signature (no `PRIVATE`) | CMake forbids mixing plain and keyword signatures on one target |
| 2 | `LindenmayerTreeParams : public treegen::TreeParams`, legacy header keeps all its includes | Engine callers read the public fields directly; other legacy files rely on the header's transitive `<regex>`, Boost and iostream includes (they may not change in T03) |
| 3 | `seed` / `maxModules` are accepted at the JSON top level and inside `parameters` (top level wins); in `.l` as `define seed N` / `define maxModules N` | `parameters` is where users put numeric knobs; top level is where a config key belongs; no shipped file uses either name (goldens prove it) |
| 4 | Custom-parameter substitution is a whole-word replacement (`[A-Za-z0-9]` boundaries, same class as the legacy regex) applied per parameter in map order, **after** the whole file is read, to the axiom and every production | Fixes #1, #3, #10; identical to legacy for shipped files when defines precede rules; end-of-text occurrences are replaced too |
| 5 | `.l` tokens are split on runs of spaces/tabs; `define` values and probabilities are parsed strictly; `\r` and a UTF-8 BOM are stripped | `legacy/test.l` starts with a BOM that legacy turned into a junk rule with a 4-byte "symbol"; a `char` symbol makes that impossible to keep (new fix #13, logged) |
| 6 | `.l` header: while no axiom has been seen, the first four bare-number lines are depth, initialLength, initialRadius, angle (degrees) | Fixes #11; matches the files' own comment `#depth, initialLength, initialRaidus, angle, axiom, rules` |
| 7 | `Rng::uniform01` uses `(mt() >> 8) * 2^-24` and `derive` seeds through `std::seed_seq{seed, id}` | `std::uniform_real_distribution` differs between libstdc++ and MSVC; `seed_seq` and `mt19937` are fully specified, so streams are bit-identical on every platform |
| 8 | Boundary script also forbids `ESTree/`, `GL/` and `<iostream>` includes | Design doc: no `ESTree/` dependency, no GL, no `std::cout` in the library |
| 9 | Solver goldens of files whose *loader output* changes (axiom of `parametricTree2.l`, depth of the 4 bare-header files) are regenerated too | The solver golden is a function of the loaded params; this is the same flip, not a solver change |

## Steps

### Step group 0: setup and plan

- [x] **0.1** Branch from `modernization`, junction the 7 `Vendor/` dirs, configure `ninja-msvc` once
- [x] **0.2** Write this plan. Commit: `T03: add plan`

### Step group 1: scaffold, boundary, `Rng`, `Module`

- [ ] **1.1** `tests/treegen/CMakeLists.txt`, `main.cpp`, `rng_tests.cpp`, `module_tests.cpp` (failing: no `treegen` target) — cases: same seed same sequence, `derive` streams differ and do not depend on draws taken, `uniform01 in [0,1)`, `parseModules("F(2)T+(45)A")` fields + `toString` == `F(2.000000)T+(45.000000)A`, `X~(5)`, brackets, empty string, malformed text (`F(`, `F(a)`, `)`, `F(1,)`, `~`) → `RewriteError`
- [ ] **1.2** `ESEngine/TreeGen/CMakeLists.txt`, `Error.h`, `Rng.*`, `Module.*`, root CMake wiring; build `treegen_tests`; expected PASS
- [ ] **1.3** `boundary_check.cmake` + `boundary_selftest.cmake` + two CTests; expected PASS; then the deliberate-bad-include experiment (compile + `treegen_boundary` both fail), output kept for the report
- [ ] **1.4** Commit: `T03: add treegen scaffold, Rng, Module and boundary check`

### Step group 2: `TreeParams` and the loaders

- [ ] **2.1** `loader_tests.cpp` (failing): every shipped file loads and its dump equals `tests/golden/params/<id>.txt` (new expectations for the flipped files are produced by step 4); `substituteParameters` cases (#1 adjacent, #3 at start, end of text, no partial words, empty value map); `.l` header (#11), `define tropism`, default angle (#12), axiom substitution (#10), BOM, CRLF; **Review Focus #2** cases: missing file, empty file, truncated JSON, `{}`, missing `axiom`, missing `rules`, `rules` not an array, `axiom` not a string, rule without `production`, wrong parameter type, bad `tropism`, bad `probability`, bad symbol, bad `seed`/`maxModules`; `seed`/`maxModules` parsing; rule declaration order; tropism normalization; JSON degrees to radians
- [ ] **2.2** `TreeParams.h`, `Loader.cpp`; expected PASS (golden comparisons for the files whose golden changes wait for step 4 and are checked there)
- [ ] **2.3** `-Wall -Wextra -Wpedantic` check through Docker (GCC 13), `/W4` zero warnings in the `treegen` target
- [ ] **2.4** Commit: `T03: add TreeParams and json/.l loaders`

### Step group 3: legacy adapter

- [ ] **3.1** Flip the KNOWN BUG #1, #3, #10, #11, #12 expectations in `legacy_characterization_tests.cpp` (rename: drop `KNOWN BUG:`), run: expected FAIL (still old behavior)
- [ ] **3.2** Rewrite `LindenmayerTreeParams.{h,cpp}` as the adapter; `legacy_characterization_tests` gets `LIBS treegen`; expected: flipped tests PASS; golden mismatches only for the intended files
- [ ] **3.3** `legacy_characterization_tests --regenerate`; review `git diff --stat modernization -- tests/golden`; update `KNOWN_BUGS.md`, golden READMEs
- [ ] **3.4** Commit: `T03: turn LindenmayerTreeParams into a treegen adapter, fix loader defects`, then `T03: regenerate affected goldens`

### Step group 4: verification and docs

- [ ] **4.1** Full Windows build (`ninja-msvc` Debug, `BOOST_LIB` set) incl. `ESTree.exe`; `ctest -j4` green
- [ ] **4.2** Linux: `ubuntu:24.04`, GCC 13, engine OFF, `treegen_tests` + `treegen_boundary*` pass
- [ ] **4.3** `DECISIONS.md` (D-009), `treegen-design.md` additive helpers, `tests/README.md`; set status `review`; commit `T03: docs and decisions`

## Review focus covered

- #2 (missing file / malformed JSON / missing `axiom` or `rules` / wrong types → `LoadError` with path and reason): `loader_tests.cpp`, cases prefixed `Review Focus 2:`.

## Deviations from the brief

- `target_link_libraries(ESTree ... treegen)` is not `PRIVATE` (Decision 1).
- Solver goldens of 5 files are regenerated in addition to the params goldens (Decision 9); `legacy/test.l` loses its junk BOM rule (Decision 5). All logged in `DECISIONS.md`.

## Blockers

(none)
