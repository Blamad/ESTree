# T08 — Headless `treegen_cli` and OBJ writer

**Depends on:** T04, T06 · **Unblocks:** T13 (and any future split-out) · **Size:** S · **Branch:** `task/T08-treegen-cli-obj-export`
**Parallel with:** T07 (read-only use of `treegen`)

## Goal

A command-line tool that proves the module is genuinely engine-free and gives agents a
verifiable end-to-end artifact:
`treegen_cli <rules.json> --seed N [--depth D] [--ring R] --out tree.obj`.

## Context to read

- `docs/architecture/treegen-design.md` (ObjWriter, TreeGenerator)
- `ESEngine/ESTree/Models/ModelExporter.cpp` (existing OBJ conventions: `v/vn/vt`, `usemtl`, face format — match where sensible)

## Scope

In:
1. `ObjWriter.h/.cpp` in `treegen` (`writeObj(file, trunk, leaves)`): trunk as object `trunk`; each leaf transform emits a 2×2 double-sided quad (same corner layout as the engine's leaf: `(±1, ±1, 0)`, front/back normals, uv per corner) as object `leaves`; 1-based indices; `\n` newlines; `.`-decimal regardless of locale; creates parent dirs; throws `treegen::Error` on I/O failure. No MTL (materials are an engine concern).
2. `tools/treegen_cli/main.cpp` + CMake (`ESTREE_BUILD_TOOLS` option, default ON): argument parsing by hand (no Boost, no new deps), `--help`, exit codes (0 ok, 2 usage, 3 load error, 4 generation error), prints a one-line summary (`segments`, `vertices`, `triangles`, `leaves`, `seed`, elapsed ms). Links **only** `treegen`.
3. Tests `tests/treegen/obj_tests.cpp`: writer on a hand-built `MeshData` (exact expected file text), index base, locale independence (call `std::setlocale(LC_ALL, "de_DE")` if available; skip otherwise), I/O failure on an invalid path; CLI smoke test via CTest `add_test` running the exe on `Resources/LindenmayerRules/parametricMonopodialTreeA.json --seed 1` into the build dir and validating the file (first line, counts of `v`/`f` lines > 0, all face indices ≤ vertex count) with a small CMake/PowerShell-free script (use a second tiny C++ test exe, not a shell script).
4. Determinism check in CTest: two runs with the same seed produce byte-identical files; different seeds differ for a stochastic file.
5. `tools/treegen_cli/README.md` (usage, exit codes, example).

Out of scope: glTF, MTL, textures, batch mode.

## Owned files

`tools/treegen_cli/**`, `ESEngine/TreeGen/include/treegen/ObjWriter.h`, `src/ObjWriter.cpp`, `tests/treegen/obj*`, tool/option block in root `CMakeLists.txt`.

## Acceptance criteria

1. `treegen_cli --help` exits 0; bad args exit 2; missing file exits 3 with a message naming the file.
2. Same-seed determinism and different-seed variance tests pass in `ctest`.
3. `treegen_cli` links nothing but `treegen` (show `dumpbin /dependents` or the CMake link line in `REPORT.md`).
4. Report the OBJ stats for 3 shipped rule files (counts + file size) in `REPORT.md`.

## Verification

Automated. Optional human check: open a generated `.obj` in Blender/MeshLab and eyeball the shape.
