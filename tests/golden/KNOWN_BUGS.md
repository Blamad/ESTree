# Known bugs of the legacy loader and solver (confirmed by test)

Each entry has a `KNOWN BUG: ...` test in `tests/legacy_characterization/legacy_characterization_tests.cpp`
that asserts the **current, wrong** result. The task named as owner flips the expectation to the correct value
when it fixes the defect (and logs it in `docs/DECISIONS.md`).

| # | Defect | Reproducing input | Current result | Correct result | Owner |
|---|---|---|---|---|---|
| 1 | Loader substitution consumes the delimiter after a match, so adjacent identical parameters are skipped | `fillRuleWithVariables("F(a1,a1)", {a1=35})` | `F(35.000000,a1)` | `F(35.000000,35.000000)` | T03 |
| 2 | Same defect in `LindenmayerTreeSolver::fillRuleWithParameters` | rule `A(x) -> F(x,x)` applied to `A(2)` | `F(2,x)` | `F(2,2)` | T04 |
| 3 | Loader substitution needs a delimiter before the name, so a parameter at the start of a production is not replaced | `fillRuleWithVariables("a1F", {a1=35})` | `a1F` | `35.000000F` | T03 |
| 4 | Leading minus: expression split at the first operator char, left side `""` = 0 | rule `A -> F(-5*2)` | `F(-5.000000)` (0 - 5) | `F(-10.000000)` | T04 |
| 5 | Negative parameter value hits defect 4 | rule `A(l) -> F(l*3)` applied to `A(-2)` | `F(-2.000000)` | `F(-6.000000)` | T04 |
| 6 | Chained operators and precedence: only the first operator is used, the rest is `atof`-truncated | `F(2*3*4)`, `F(1+2*3)` | `F(6.000000)`, `F(3.000000)` | `F(24.000000)`, `F(7.000000)` | T04 |
| 7 | `if (newProduct.length() > 0) product = newProduct;` keeps the old string when everything rewrites to `""` | rule `A -> ""` | `A` | empty string | T04 |
| 8 | `generateSymbolRegex` does not escape symbols; turtle symbols such as `+` make the regex invalid | rules for `+` and `A` | `std::regex_error` thrown | `+` can be rewritten | T04 |
| 9 | Rule with parameters applied to a symbol without an argument list leaves names and expression in place (arity mismatch, no error) | rule `A(l) -> F(l*2)` applied to bare `A` | `F(l*2)` | typed `RewriteError` naming rule and symbol | T04 |
| 10 | Custom parameters are not substituted into the axiom | `legacy/parametricTree2.l` (`define startLength 1.2`) | axiom `B(1)F(startLength)+(45)A` | `B(1)F(1.200000)+(45)A` | T03 |
| 11 | Old `.l` files with bare-number header lines (depth, length, radius, angle): each one-token line is taken as the axiom (last wins), the values are ignored and `depth` stays **uninitialized** | `legacy/advancedTree.l`, `testX/Y/Z.l` | axiom `F`, `initialLength=1`, `initialRadius=1`, `angle=15`, `depth` indeterminate | header values honored, or a loader error | T03 |
| 12 | Default `angle` (15) is stored as 15 radians; only values read from a file or the 2-argument constructor are converted. `define tropism` in `.l` is a TODO and ignored | `legacy/normalTest.l` (tropism), `legacy/testX.l` (angle) | `angle=15`, tropism default | `radians(15)`; tropism parsed | T03 |

## Suspected, not confirmed or not testable here

- `res.suffix()` read after the loop's failed `regex_search` (`generateTreeProduction`, both
  `fillRule*` functions): with the MSVC standard library the unmatched tail **is** preserved (tests
  "substitution keeps the text after the last match" and all solver goldens pass), so there is no observable
  bug on this toolchain. The standard leaves `match_results` unspecified after a failed search, so it may
  break on libstdc++/libc++ (T11 Linux). T04 should not copy the pattern.
- Missing file / malformed JSON / missing `axiom` (Review Focus 2) and arity mismatch with too few values
  (Review Focus 4, `values[i]` past the end): crash or UB, not assertable in-process. Owners T03/T04.
- Interpreter items (`returnNewIndexAfterParameter` float index, O(n^2) parameter scan, literal `-1` parameter):
  need GL types, out of scope for T02. Owner T05.
