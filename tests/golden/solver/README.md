# Solver goldens

`<id>.dN.txt` holds `LindenmayerTreeSolver::generateTreeProduction` output for the rule file `<id>` with
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
- `legacy/advancedTree.l`, `testX.l`, `testY.l`, `testZ.l` never set a depth (their bare-number header lines are
  read as axioms, see `KNOWN_BUGS.md`), so `params.depth` is uninitialized: only `d0..d3` are stored; there is no
  own-depth golden.
- `legacy/parametricTree2.l` contains rule-file mistakes (`[^+a)`, axiom with an unsubstituted `startLength`, a
  rule with parameters applied to a bare `F`). The golden freezes the resulting nonsense on purpose.

## Platform note

Goldens were produced with MSVC (Debug) and its `std::regex`. A different standard library may behave
differently where the legacy code relies on unspecified behavior (`res.suffix()` after a failed `regex_search`,
see `KNOWN_BUGS.md`); a golden mismatch on another toolchain is then a finding, not a reason to regenerate.
