# T08 — Headless `treegen_cli` and OBJ writer — Report

**Result:** done · **Branch:** `task/T08-treegen-cli-obj-export` · **Final commit:** `4ef73f2` (merged into `modernization` as `fcec01b`)

The implementing subagent could not write this file; the orchestrator wrote it from the agent's hand-back and
re-verified the merged tree (results appended at the end).

## What changed

- `treegen/ObjWriter.h` + `src/ObjWriter.cpp`: `writeObj(file, trunk, leaves)`. Header `# treegen OBJ export`, object `trunk` (all `v`, then `vn`, then `vt`, then `f v/vt/vn` triangles, 1-based) and object `leaves` (each leaf is the engine's 8-vertex double-sided panel, positions transformed by the leaf matrix, normals by its cofactor matrix). Numbers via `std::to_chars` in fixed notation through a 1 MiB buffer; `-0` and NaN/Inf written as `0`. Creates parent directories; throws `treegen::Error` on open/write failure, on an out-of-range trunk index (detected before the file is opened) and on an index count that is not a multiple of 3. Empty objects omitted.
- `tools/treegen_cli/` (`main.cpp`, `CMakeLists.txt`, `README.md`, `tests/obj_check.cpp`); links only `treegen`. Root `CMakeLists.txt`: `ESTREE_BUILD_TOOLS` block (default ON) at the end, outside the engine gate.
- Tests: `tests/treegen/obj_tests.cpp` (10 cases: exact text, index base, normals under scale/mirror/singular matrices, C++ and C locale, `-0`/NaN, parent dirs, determinism, I/O failure, bad index) and 15 CTest entries `treegen_cli_*` driven by `obj_check` (no shell or PowerShell): help, bad args, missing file, bad JSON, unwritable output, generation, validation, same seed, thread-count independence, different seed.
- D-017 in `docs/DECISIONS.md`; `treegen-design.md` unchanged.

## Acceptance criteria

| Criterion | Evidence |
|---|---|
| `--help` exit 0, bad args exit 2, missing file exit 3 naming the file | agent: CTest entries `help`, `no_args`, `missing_out`, `unknown_option`, `bad_seed`, `bad_ring`, `missing_file`, `bad_json` pass; message `treegen_cli: cannot load rule file 'nofile.json': ...`; unwritable output exits 4 |
| Same seed → byte-identical; different seeds differ | agent: monopodialA and randomTree identical; `--threads 3` equals default; seed 1 vs 2 on `randomTree.json` differ |
| CLI links nothing but `treegen` | agent: link line `treegen.lib` + system libs only; `dumpbin /dependents`: CRT, MSVCP140, VCRUNTIME, KERNEL32 |
| Build, tests, Linux | agent: engine OFF Debug `244/244`; full config `257/257`; Linux Docker (Ubuntu 24.04/GCC 13) `218/218`; no warnings at `/W4` or `-Wall -Wextra -Wpedantic` |

## OBJ stats (seed 1, Release, agent)

| Rule file | segments | vertices | triangles | leaves | size |
|---|---|---|---|---|---|
| parametricMonopodialTreeA | 256 | 4 608 | 4 096 | 128 | 650 388 B |
| parametricTernaryTreeB | 1 458 | 26 244 | 23 328 | 729 | 3 870 695 B |
| randomTree | 123 | 2 214 | 1 968 | 0 | 256 126 B |

`parametricTernaryTreeE.json --depth 10`: 118 097 segments, 2 125 746 vertices, 1 889 552 triangles, 59 049 leaves, 351 858 568 B; 2251 ms in Release (load + generate + write), 18 663 ms in Debug.

## Deviations and notes

- `--seed` optional (0 = the rule file's `seed` key); `--opt=value` accepted; exit code 4 also covers a failed OBJ write.
- `obj_check run` uses `std::system` with per-platform quoting (CTest cannot assert a specific exit code); passed on both platforms, also with spaces in the Windows path.
- UVs are written as stored for both trunk and leaves (the engine's `ModelExporter` flips `v` for leaves only). Faces are triangles, not quads.
- Numbers are shortest exact fixed notation (tiny values print like `-0.000000017484556`), valid but verbose.
- The C-locale half of the locale test runs only if a German locale is installed — probably skipped on the Docker image; the C++ global-locale half always runs.

## Follow-ups

- OBJ files are huge for deep trees (352 MB for ternaryE depth 10); a later task could weld vertices or add a binary format (glTF is in `docs/BACKLOG.md`).
- Optional human check: open a generated `.obj` in Blender or MeshLab (the shape was not judged).
