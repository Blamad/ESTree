# T07 — Engine adapter switch-over; delete the legacy pipeline — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: use `superpowers:subagent-driven-development`
> or `superpowers:executing-plans`. Steps use checkbox (`- [ ]`) syntax.

**Status:** review · **Branch:** `task/T07-engine-adapter-switchover` · **Base:** `modernization @ 1bb4e50`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** `LindenmayerTree` becomes a thin adapter over `treegen::generateTree` that uploads into the existing
`Mesh`/`InstancedMesh`; the legacy `Lindenmayer*` generation classes, their Boost usage and the frozen test references are
deleted; the tree seed is controllable from the console.

**Approach:** Pure conversion helpers (no GL) live in one header and are unit-tested first. `LindenmayerTree::generate()` runs
`generateTree`, then fills a `Mesh` whose GL buffers are created with exactly the generated sizes (no 100 000 / 4 000 000
magic numbers; `Mesh`/`GLVertexArray` need no edit because the buffer capacity is a constructor argument and the vertex
vectors can be filled before the first `updateMesh()`). The facade `LindenmayerTreeParams` is dropped in favour of
`treegen::TreeParams`; the three scene helpers collapse into `LindenmayerTreeFactory`.

## Findings that shape the plan

- `Mesh(vertices, indices, shader, vBufferSize, iBufferSize, usage)` allocates `GLVertexArray(iBufferSize, vBufferSize)`;
  `updateMesh()` is a `glBufferSubData` of the whole CPU vectors, so a too small capacity silently overflows (the old
  100 000 cap). `InstancedMesh` sizes from its constructor vectors (`&v[0]` on an empty instance vector would be UB, so an
  empty leaf set creates no leaf mesh, as the old `leafGrowthProb <= 0` early return did). No engine edit needed.
- The scene helpers add the tree with `Scene::addGameObject` (the scene is still being constructed, `SceneManager::currentScene`
  is not set yet), the factory uses `Context::getSceneManager()`. The factory therefore gets an optional `Scene*`.
- `ConsoleTreeCommand` has an endless loop on an unknown parameter (`Unknown tree parameter!` without `erase`) and casts any
  selected object to `LindenmayerTree*` with a C cast. Both are fixed while the file is open (they would defeat the "print a
  message instead of crashing" requirement).
- File 15 (`parametricTernaryTreeB.json`) is fully deterministic (no probabilities, `leavesGrowthProbability` 1, no leaf angle
  diversity), so `tree file 15` twice gives the same tree whatever the seed. The human checklist uses `tree file 4`
  (`advancedTree2.json`, stochastic) for "different trees".
- `legacy/parametricTree2.l`: `[^+a)` — see Decisions.

## Files

- Create: `ESEngine/ESTree/Models/Tree/TreeMeshConversion.h` — header-only pure helpers (namespace `treeadapter`).
- Create: `tests/adapter/tree_adapter_tests.cpp`, target `tree_adapter_tests` (links `treegen` only; runs with the engine OFF).
- Create: `tests/treegen/golden_data_tests.cpp` — the characterization checks that only existed in the legacy test.
- Rewrite: `LindenmayerTree.{h,cpp}`, `LindenmayerTreeFactory.{h,cpp}`.
- Modify: `ConsoleTreeCommand.{h,cpp}`, `SingleTreeScene.*`, `ForestScene.*`, `TreeScene.*`, `tests/CMakeLists.txt`, `tests/treegen/{geometry,geometry_facade,rewriter_golden,loader_golden}_tests.cpp`.
- Modify: `Resources/LindenmayerRules/legacy/parametricTree2.l`, `tests/golden/params/legacy__parametricTree2.l.txt`, new `tests/golden/solver/legacy__parametricTree2.l.d1..d3.txt`.
- Delete: `LindenmayerTreeSolver.*`, `LindenmayerTreeInterpreter.*`, `LindenmayerTreeMeshGenerator.*`, `Segment.h`,
  `LindenmayerTreeParams.*`, `tests/legacy_characterization/`, `tests/treegen/legacy_reference/`,
  `tests/treegen/{turtle_equivalence,geometry_parity}_tests.cpp` (they exist only to compare against the frozen references).
- Docs: `docs/architecture/overview.md`, `CLAUDE.md` (known defects), `tests/README.md`, `tests/golden/{README,KNOWN_BUGS}.md`,
  `tests/golden/solver/README.md`, `docs/DECISIONS.md` (D-017), `docs/architecture/treegen-design.md` only if an interface changes (none planned).

## Interfaces

Produces:

```cpp
// TreeMeshConversion.h, namespace treeadapter
Vertex                          toEngineVertex(const treegen::MeshVertex&);               // color value-initialised (0,0,0,0)
std::vector<Vertex>             toEngineVertices(const std::vector<treegen::MeshVertex>&);
int                             toEngineIndex(uint32_t);                                   // throws std::length_error if > INT_MAX
std::vector<int>                toEngineIndices(const std::vector<uint32_t>&);
int                             checkedBufferSize(std::size_t count, const char* what);   // for the GL capacities, throws std::length_error
std::vector<InstancedTransform> toInstancedTransforms(const std::vector<glm::mat4>& leaves, const glm::mat4& world);   // world * leaf
uint32_t                        randomSeed();                                              // never 0
uint32_t                        chooseSeed(uint32_t paramsSeed, uint32_t requestedSeed, const std::function<uint32_t()>& fresh = randomSeed);

// LindenmayerTree (GameObject)
LindenmayerTree(const treegen::TreeParams&, const Material& bark = ..., const Material& leaves = ..., bool useMeshWiring = false, bool normalVisualisation = false, uint32_t requestedSeed = 0);
void generate();                       // throws treegen::Error
treegen::TreeParams getTreeParams();   // unchanged name
uint32_t getSeed() const;              // effective seed, valid after generate()

// LindenmayerTreeFactory
LindenmayerTree* generateTree(const treegen::TreeParams&, const std::string& name, const Material& bark, const Material& leaves,
                              const glm::vec3& position, uint32_t seed = 0, Scene* scene = nullptr, bool meshWiring = false, bool normalVisualisation = false);   // throws treegen::Error
LindenmayerTree* generateTreeFromFile(const std::string& path, ... same tail ...);   // logs and returns nullptr on LoadError/RewriteError/InterpretError
```

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | `LindenmayerTreeParams` and `Rule` are deleted, callers use `treegen::TreeParams` / `treegen::loadTreeParams` | the facade only existed for the legacy solver and its characterization test; no alias header needed |
| 2 | Seed precedence: `TreeParams::seed` (JSON `seed`, non-zero) > console `seed n` (non-zero) > fresh `random_device` value (never 0). The effective seed is logged and passed as `GenerateOptions::seed` | brief; 0 means "caller decides" in treegen |
| 3 | `rtree` clears the JSON seed of the copied params (its purpose is to re-roll) and draws a new seed; `rtree seed n` reuses n. `tree file X seed n` with a seeded file X ignores n and says so in the console | keeps the "params win" rule for generation, gives feedback instead of silent surprise |
| 4 | `generate()` propagates `treegen::Error`; the console prints one line (`Tree error: ...`) and keeps the old tree on `rtree`; `generateTreeFromFile` (scenes) logs and skips | the old adapter swallowed errors and drew the root only, which hid bad rule files |
| 5 | Mesh vertices are moved into a `Mesh` created with exact capacities and uploaded once | no second copy of a 100 MB vertex array, no capacity assumptions |
| 6 | `legacy/parametricTree2.l` `[^+a)` becomes `[^+(a)` (insert the lost `(`) | the old binary executed `^` and `+` with their default angle and skipped the stray `a)`; inserting the `(` is the only edit that keeps every character the author typed. The alternatives `[^(a)` / `[+(a)` (the sibling branches use `[+(a)`) cannot be told apart from the file, so this is logged in D-017 and put on the owner's checklist |
| 7 | `turtle_equivalence_tests.cpp` and `geometry_parity_tests.cpp` are deleted with the references; analytic turtle and geometry tests stay | they are only meaningful against the frozen legacy math the brief says to delete |

## Steps

### Step group 1: conversion helpers (tests first)

- [x] **1.1** `tests/adapter/tree_adapter_tests.cpp` + CMake target; cases: vertex fields (position/normal/uv, color zero), index narrowing incl. `INT_MAX` ok and `INT_MAX+1` / `UINT32_MAX` refused, `checkedBufferSize` refusal, leaf matrices premultiplied by a world transform (non-commuting translate/rotate), seed rules (params > console > random, never 0, injected `fresh`), `tree file 15 seed 42` twice gives identical counts and arrays, every shipped rule file at `min(depth, 6)` through generateTree + conversions without exception.
- [x] **1.2** Run: expected FAIL (header missing). Implement `TreeMeshConversion.h`. Run: PASS. Commit `T07: add tree mesh conversion helpers and tests`.

### Step group 2: adapter, factory, console, scenes; delete the legacy pipeline

- [x] **2.1** Rewrite `LindenmayerTree`, factory; delete solver/interpreter/mesh generator/Segment/Params; update console and scenes; build Debug. Commit `T07: switch the engine to treegen::generateTree`.
- [x] **2.2** Fix `parametricTree2.l`, regenerate goldens (params + new solver d1..d3), update tests that expected the `RewriteError`. Commit `T07: fix parametricTree2.l`.

### Step group 3: legacy tests

- [x] **3.1** Map every legacy characterization case to a treegen test (table in the report); port the three missing ones (golden completeness, seed independence of deterministic files, stochastic alphabet/bracket balance) into `golden_data_tests.cpp`.
- [x] **3.2** Delete `tests/legacy_characterization`, `legacy_reference`, the two reference-based test files, the MSVC guard. Commit `T07: delete legacy characterization tests and frozen references`.

### Step group 4: docs and verification

- [x] **4.1** overview.md, CLAUDE.md, tests/README.md, golden READMEs, D-017. Commit.
- [x] **4.2** Verify: Windows Debug+Release build + ctest, Docker Linux engine OFF tests + engine ON compile, greps, smoke launches of scenes 0-4. Set Status `review`.

## Verification evidence (2026-10-25, this machine)

| Check | Result |
|---|---|
| Windows `ninja-msvc` Debug build + `ctest -C Debug` | exit 0; `100% tests passed, 0 tests failed out of 201` |
| Windows Release build + `ctest -C Release` | exit 0; `100% tests passed out of 201` |
| Docker Ubuntu 24.04 / GCC 13, engine OFF | `100% tests passed, 0 tests failed out of 188` (incl. `tree_adapter_tests`) |
| Docker Ubuntu 24.04 / GCC 13, engine ON (CI package list) | `ESTree` linked; `100% tests passed, 0 tests failed out of 201`; no GCC warning in the files of this task |
| Docker, engine ON without any Boost package | all `ESTree` objects compile (no `error:`); only the link fails with `cannot find -lboost_date_time` ... (the Boost libs named in `CMakeLists.txt`, T13) |
| `grep -rni boost ESEngine/ESTree/Models/Tree` / `grep -rli boost ESEngine --include=*.h --include=*.cpp` | no matches / no matches |
| `grep -rl Lindenmayer ESEngine` (outside TreeGen comments) | only `Models/Tree/{LindenmayerTree,LindenmayerTreeFactory,TreeMeshConversion}`, `ConsoleTreeCommand.*`, the three scenes |
| MSVC `/W4` | no warning in the new/changed lines (the remaining ones in the scene files and the factory's local `factory` are in untouched lines) |
| Smoke Debug, scenes 0-4 (`--scene N`, 12-14 s, `CloseMainWindow`) | alive after the wait, exit code 0, `Rendering started` ... `Quitting..`, `Generation finished. N tris. Time: Xms. Seed: S` for every tree, 0 Application Error events |
| Smoke Release, scenes 0 and 2 | same |
| Bad rule file (scene 0 with an invalid `parametricTernaryTreeB.json`, restored afterwards) | `Tree Tree skipped: rule A -> ...: unexpected ')' ...` logged by `LindenmayerTreeFactory`, scene keeps running |
| Buffer sizing (scene 0 with `depth` 9 temporarily in `parametricTernaryTreeB.json`, restored) | `Generation finished. 629856 tris` (708 588 vertices, 7x the old cap), alive 30 s, clean close; ternary E depth 10 (2.1 M vertices) is converted in `tree_adapter_tests` |

Not verifiable by an agent: the console (`tree ... seed`, `rtree`, `help`) and what is drawn; see the human checklist in the report.

## Review focus covered

- 1/2: console catches `InterpretError`/`LoadError`/`RewriteError`; scenes log and skip (human check in the report).
- "growth": GL capacities are exact; depth-10 ternaryE (2.1 M vertices) is generated and converted in `tree_adapter_tests`.

## Deviations from the brief

- `turtle_equivalence_tests.cpp` / `geometry_parity_tests.cpp` removed (not listed in the brief, forced by deleting `legacy_reference/`).
- Console: infinite loop on unknown parameters and the unchecked C-cast of the selected object are fixed (same file).
- `LindenmayerTreeFactory` gains `Scene*`/`seed` parameters and a `generateTreeFromFile` variant (needed so the scenes can use it).

## Blockers

(none)
