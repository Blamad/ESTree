# T02 — Characterization tests of the legacy solver and params loader — Implementation Plan

**Status:** done · **Branch:** `task/T02-characterization-tests` · **Base:** `modernization @ fc539f2`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** Freeze the current observable behavior of `LindenmayerTreeParams` and `LindenmayerTreeSolver::generateTreeProduction` as golden tests, and document confirmed defects as `KNOWN BUG:` tests.

**Approach:** One doctest executable `legacy_characterization_tests` compiled from the unchanged legacy `.cpp` files (they need only Boost headers, glm, rapidjson; no shim expected). A dump helper turns a loaded `LindenmayerTreeParams` into `key=value` text; goldens live in `tests/golden/` and are compared byte-for-byte (`--regenerate` rewrites them). Deterministic rule files get solver goldens; stochastic ones get property tests.

## Files

- Create: `tests/legacy_characterization/legacy_characterization_tests.cpp` — tests, dump helper, `--regenerate` main
- Modify: `tests/CMakeLists.txt` — one `estree_add_test` block
- Create: `tests/golden/{README.md,KNOWN_BUGS.md,.gitattributes}`, `tests/golden/params/*.txt`, `tests/golden/solver/{README.md,*.dN.txt}`
- Not touched: anything under `ESEngine/`, `Vendor/`

## Interfaces

- Consumes: `estree_add_test`, `ESTREE_REPO_ROOT`, legacy `LindenmayerTreeParams(std::string path)`, `getAvailableRules()`, `getRules()`, `LindenmayerTreeSolver::generateTreeProduction(params)`.
- Produces: golden files (read by T03/T04 as the behavior reference) and `KNOWN BUG:` test names that T03/T04/T05 flip.

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | Golden file name = rule path relative to `LindenmayerRules`, `/` replaced by `__`, plus `.txt` / `.dN.txt` | `advancedTree.json` and `legacy/advancedTree.l` would collide otherwise |
| 2 | Files that never set `depth` (the `.l` files using bare-number headers) record `depth=UNSET` instead of the uninitialized `int` | Reading it is indeterminate (varies by build config); also KNOWN_BUGS |
| 3 | Floats are dumped with `%.9g`; productions verbatim | Round-trip exact, no locale dependence |
| 4 | Goldens compared in binary mode, `\n` only, `tests/golden/.gitattributes` has `* -text` | No CRLF drift |

## Steps

- [x] 1 Plan + commit
- [x] 2 CMake block + skeleton, build, dump helper, generate params goldens, review by eye
- [x] 3 Solver goldens (deterministic files), property tests (stochastic), completeness test
- [x] 4 Probe suspected defects with minimal inputs; add `KNOWN BUG:` tests + `KNOWN_BUGS.md`
- [x] 5 READMEs, 20x repeat run, `git diff modernization --stat -- ESEngine` empty, commit, status `review`

## Review focus covered

Review Focus items 2, 4, 6 of `docs/PLAN.md` describe today's crash/UB behavior; they are not pinned here (crashes cannot be asserted in-process). Item 6 (literal `-1`) belongs to the interpreter (needs GL types), out of scope; noted in the report.

## Deviations from the brief

- Deterministic solver goldens skip the own-depth file for 4 .l files whose depth is never set (indeterminate).
- Total test run takes ~19 s in Debug (std::regex on 55 KB products); ctest runs each TEST_CASE as its own process, so 26 cases cost ~8 min for 20 repeats.

## Blockers

(none)
