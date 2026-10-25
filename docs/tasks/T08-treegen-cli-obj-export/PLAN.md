# T08 — Headless `treegen_cli` and OBJ writer — Implementation Plan

**Status:** in-progress · **Branch:** `task/T08-treegen-cli-obj-export` · **Base:** `modernization @ 1bb4e50`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** A command-line tool linking only `treegen` that turns a rule file plus a seed into an OBJ file, with the OBJ writer living in `treegen`.

**Approach:** `writeObj` formats numbers with `std::to_chars` (locale independent, shortest round-trip, fixed notation) into a 1 MiB
`std::string` buffer flushed to a binary `std::ofstream`. The CLI is a thin hand-written parser over `loadTreeParams` + `generateTree` + `writeObj`.
CLI tests are CTest entries that run the exe and a second tiny C++ checker (`obj_check`, which also compares files), so nothing depends on a shell.

## Files

- Create: `ESEngine/TreeGen/include/treegen/ObjWriter.h`, `ESEngine/TreeGen/src/ObjWriter.cpp` — the writer (picked up by the globs)
- Create: `tests/treegen/obj_tests.cpp` — writer unit tests (picked up by the glob)
- Create: `tools/treegen_cli/{main.cpp,CMakeLists.txt,README.md}` — CLI target `treegen_cli`, links only `treegen`
- Create: `tools/treegen_cli/tests/obj_check.cpp` — validator/comparator used by CTest (no doctest, no treegen)
- Modify: root `CMakeLists.txt` (end): `option(ESTREE_BUILD_TOOLS ON)` + `add_subdirectory(tools/treegen_cli)`
- Modify: `docs/DECISIONS.md` (append D-017)

## Interfaces

- Consumes: `loadTreeParams`, `generateTree(GenerateOptions{seed, ringDensity, threads})`, `MeshData`, `treegen::Error`/`LoadError`.
- Produces: `void treegen::writeObj(const std::filesystem::path&, const MeshData& trunk, const std::vector<glm::mat4>& leaves)` (as in `treegen-design.md`, unchanged).

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | Leaf quad vertices are transformed by the leaf matrix; normals by the cofactor matrix (inverse-transpose up to scale, sign-corrected, normalised) | exact for non-uniform scale and finite for singular matrices |
| 2 | Numbers: `to_chars(..., chars_format::fixed)`, `-0` printed as `0`, non-finite printed as `0` | portable, deterministic, no exponent for OBJ readers |
| 3 | UVs are written as stored (the engine exporter flips `v` for leaves only; inconsistent, not copied) | one convention for trunk and leaves |
| 4 | An object (`o trunk`, `o leaves`) is emitted only when it has vertices | no empty groups |
| 5 | `--seed` optional (default 0 = `params.seed`); exit 4 also covers a failed write | brief lists no extra codes |

## Steps

### Step group 1: writer
- [ ] 1.1 Failing `obj_tests.cpp` (exact text, index base, locale, I/O failure, parent dirs, empty)
- [ ] 1.2 Implement `ObjWriter`; commit `T08: add ObjWriter`

### Step group 2: CLI
- [ ] 2.1 `obj_check` + CTest registrations (help 0, bad args 2, missing file 3, generate+validate, determinism, variance)
- [ ] 2.2 `main.cpp`, CMake, README, root option; commit

### Step group 3: verification
- [ ] 3.1 Windows full + engine OFF + ctest, `/W4` clean; Linux Docker; link line; stats for 3 rule files; ternaryE depth 10 time
- [ ] 3.2 D-017, PLAN status `review`

## Review focus covered

Determinism (same seed identical bytes), locale independence, boundary (`treegen_boundary` stays green), no Boost in the CLI.

## Deviations from the brief

(none so far)

## Blockers

(none)
