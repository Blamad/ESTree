# T02 — Characterization tests of the legacy solver and params loader — Report

**Result:** done · **Branch:** `task/T02-characterization-tests` · **Final commit:** `6e3d251` (merged into `modernization` as `9af01f2`)

The implementing subagent could not write this file; the orchestrator wrote it from the agent's hand-back and
**re-verified on the merged tree**: `100% tests passed out of 41` (ninja-msvc Debug, `ctest -j4`, 17 s wall; the 15 earlier tests + 26 new).

## What changed

- `tests/legacy_characterization/legacy_characterization_tests.cpp` — 26 doctest cases, custom `main` with `--regenerate` (off by default).
  Built from the **unchanged** `LindenmayerTreeParams.cpp` + `LindenmayerTreeSolver.cpp` (no shims needed, nothing under `ESEngine/` or `Vendor/` touched).
- `tests/CMakeLists.txt`: +8 lines (an `estree_add_test` block).
- `tests/golden/` (≈ 1 MB): `README.md`, `KNOWN_BUGS.md`, `.gitattributes` (`* -text`, goldens are byte-exact), `params/` (43 files = every shipped rule file: 21 `.json` + 22 `legacy/*.l`, `legacy/x.l` → `legacy__x.l`), `solver/` (README + goldens for the 34 deterministic rule files at depth 0..3 and own depth; length+hash fallback for very large products).

## Acceptance criteria

| Criterion | Evidence |
|---|---|
| ctest green, deterministic | *verified*: 41/41. Agent: `ctest -R "^(params\|solver\|KNOWN)" --repeat until-fail:20` → 26/26; 20 direct exe runs with 1.1 s sleeps (solver RNG seeded by `time(0)`) → 0 failures; an in-process case re-runs each deterministic file 3×|
| Goldens for every shipped rule file | 43 params goldens (a completeness test fails on missing/stale files); 34 solver goldens; the 9 exclusions are justified in `tests/golden/solver/README.md` (stochastic: `advancedTree2`, `fibbonacciTree`, `randomTree`, `randomTree2`, `parametricTernaryTreeD`, plus the same four as `.l`) and get property tests (non-empty, balanced brackets, alphabet) |
| Legacy code untouched | `git diff modernization --stat -- ESEngine Vendor` empty |
| `KNOWN_BUGS.md` with owners | 12 confirmed defects, each with a `KNOWN BUG:` test asserting the current wrong behavior and an owner (T03 loader / T04 solver) — see `tests/golden/KNOWN_BUGS.md` |

## Confirmed defects (summary; details in `KNOWN_BUGS.md`)

Adjacent identical parameters not substituted (loader + solver); parameter at production start not substituted; leading minus
(`F(-5*2)` → `-5`); chained operators ignored (`2*3*4` → 6); rule rewriting to `""` ignored; regex-metacharacter symbols (`+`) throw
`std::regex_error`; arity mismatch silently accepted; custom parameters not substituted into the axiom; old `.l` files with bare-number headers
take them as axiom and leave `depth` uninitialized (`advancedTree.l`, `testX/Y/Z.l`); default `angle` not converted to radians and `define tropism` ignored in `.l`.

## Not confirmed / not testable here (for T03–T05)

- `res.suffix()` after a failed `regex_search`: tail preserved on MSVC, unspecified by the standard — may break on libstdc++/libc++ (**watch in T11 Linux**); T04 must not copy the pattern.
- Missing file / malformed JSON / too-few-arguments arity: crash or UB, not assertable in-process (T03/T04 Review Focus #2, #4).
- Interpreter items (float index, O(n²) scan, literal `-1`): need GL types, T05.

## Deviations and notes

- Goldens come from MSVC's `std::regex`; a mismatch on another toolchain is a finding, not a reason to regenerate.
- Full run ≈ 13–19 s in Debug (`std::regex` on 55 KB products); legacy sources are compiled a second time, so their `/W4` warnings also appear for the test target (the app warning baseline is unaffected).
- No `DECISIONS.md` entry needed; the golden README states that goldens change only with one.

## Follow-ups

- T03 flips KNOWN BUG #1, #3, #10, #11, #12; T04 flips #2, #4–#9. Each flip needs a `DECISIONS.md` entry.
- Consider a slimmer set of own-depth goldens if repository size becomes a concern (≈ 1 MB now).

## Post-merge finding (orchestrator, Linux/Docker, 2026-10-25)

Running the merged tests on Ubuntu 24.04 / GCC 13 (libstdc++, `ESTREE_BUILD_ENGINE=OFF`): **8 of 26 legacy cases fail**
(`params: every shipped rule file loads...`, `solver: deterministic ... goldens`, `solver: stochastic...`, `expression folding`,
`substitution keeps the text after the last match`, and three `KNOWN BUG` cases). Example: `A(-2)` with `F(l*3)` yields `F(-2*`
instead of `F(-2.000000)` — the text after the last regex match is lost. This **confirms** the suspected defect "`res.suffix()` after
a failed `regex_search`" (unspecified by the standard; MSVC happens to preserve the tail) and makes the legacy solver/loader non-portable.
Consequence: `legacy_characterization_tests` is built **only for MSVC** (guard in `tests/CMakeLists.txt`); T03/T04 must not reuse the pattern,
and the new `treegen` tests must be green on Linux too.
