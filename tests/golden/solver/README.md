# Solver goldens

`<id>.dN.txt` holds the production string of the (deleted) legacy solver `LindenmayerTreeSolver::generateTreeProduction` for the rule file `<id>` with
`params.depth` forced to `N`. `<id>` is the path under `Resources/LindenmayerRules/` with `/` written as `__`
(`legacy__test.l`). `N` is 0, 1, 2, 3 and, when the file defines it, the file's own depth (so `d8` for a
depth-8 file). The file content is the product string verbatim (no trailing newline). If the product is longer
than 200 000 characters the file instead holds `length=<n>` and `fnv1a64=<hex>` (none of the shipped files
reach that; the largest own-depth product is about 55 KB).

## Which files are deterministic (34 of 43)

A file is deterministic when **every symbol has exactly one production and its probability is 1**. The solver
still draws a number from its `time(0)`-seeded static RNG for every rewritten symbol, but `value - 1 <= 0`
always holds, so the draw cannot change the result (verified by the "does not depend on the time-seeded static
RNG" test and by the 20x repeat run in the T02 report).

JSON: `advancedTree`, `parametricMonopodialTreeA..D`, `parametricSympodialTreeA..D`,
`parametricTernaryTreeA`, `B`, `C`, `E`, `symetricTree`, `test`, `test2`.

Legacy `.l`: `advancedTree`, `normalTest`, `parametricMonopodialTreeA..D`, `parametricSympodialTreeA..D`,
`parametricTernaryTreeA`, `parametricTree`, `parametricTree2`, `symetricTree`, `test`, `testX`, `testY`, `testZ`.

## Excluded (9 files, property tests only)

| File | Reason |
|---|---|
| `advancedTree2.json` | 5 productions for `A`, probabilities 0.275/0.275/0.15/0.15/0.15 (+ `initialDepth`) |
| `fibbonacciTree.json` | 4 productions for `a`, probabilities 0.125/0.225/0.35/0.3 |
| `randomTree.json` | 4 productions for `a`, probability 0.25 each |
| `randomTree2.json` | 6 productions for `b`, 4 for `a`, probabilities < 1 |
| `parametricTernaryTreeD.json` | `A` has probability 0.8: 20 % of the time the symbol is left unchanged |
| `legacy/advancedTree2.l` | 5 productions for `A`, probabilities < 1 |
| `legacy/fibbonacciTree.l` | 4 productions for `a`, probabilities < 1 |
| `legacy/randomTree.l` | 4 productions for `a`, probability 0.25 each |
| `legacy/randomTree2.l` | 6 productions for `b`, 4 for `a`, probabilities < 1 |

## Special cases inside the deterministic set

- `test2.json`: its only rule is for `a`, the axiom `F(70)X` contains none, so every depth is `F(70)X`
  (the golden shows that the loader/solver round-trip is the identity there).
- `legacy/advancedTree.l`, `testX.l`, `testY.l`, `testZ.l` set their depth with a bare-number header line (depth 4 for
  `advancedTree.l`, depth 1 for the others; T03 fixed the loader, `KNOWN_BUGS.md` #11). The own depth is `d4` for
  `advancedTree.l` (new golden) and already covered by `d1` for the others.
- `legacy/parametricTree2.l` had a rule-file mistake (`[^+a)`, a lost `(`; a rule with parameters applied to a bare `F` is fine). The legacy golden
  froze the resulting nonsense. Since T03 its axiom is substituted (`F(1.200000)` instead of `F(startLength)`, `KNOWN_BUGS.md` #10). Since T04 the stray
  `)` was a `treegen::RewriteError` (#14) and `d1..d3` were deleted. T07 restored the `(` (`[^+(a)`, #15, D-017) and wrote `d1..d3` again **with
  `treegen`** (canonical form, seed 7), so these three are not legacy output; `d0` is the original golden.

## Who checks them

- `tests/treegen/rewriter_golden_tests.cpp` (`treegen_tests`, every platform): `treegen::rewrite` against every golden.
- `tests/treegen/golden_data_tests.cpp`: the directory holds exactly `d0..d3` and the own depth of every deterministic file, no stale files.

The golden test compares in the canonical number form, `toString(parseModules(golden))`: the legacy solver copied the literal numbers of a
production as written (`+(45)`) and printed computed ones with `std::to_string` (`+(45.000000)`), while `ModuleString` prints
every number the second way. Every digit is still compared, and the goldens were not rewritten. The
files of the legacy solver are in the legacy form, the three regenerated ones in the canonical form.

## Platform note

Goldens (except `legacy__parametricTree2.l.d1..d3`) were produced with MSVC (Debug) and the legacy `std::regex` solver. `treegen::rewrite` has no regex and matches
them on MSVC and on Linux/GCC (T04). A golden mismatch is a finding, not a reason to regenerate.
