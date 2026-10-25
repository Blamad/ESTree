# Known bugs of the legacy loader and solver (confirmed by test)

Each open entry has a `KNOWN BUG: ...` test in `tests/legacy_characterization/legacy_characterization_tests.cpp`
that asserts the **current, wrong** result. The task named as owner flips the expectation to the correct value
when it fixes the defect (and logs it in `docs/DECISIONS.md`).

## Open

None. The solver defects (#2, #4-#9) were fixed in T04, see below.

## Fixed in T03 (loader, now `treegen::loadTreeParams`)

The `KNOWN BUG:` tests were flipped to the correct expectation and renamed (the prefix is dropped); the params goldens
below were regenerated. See `docs/DECISIONS.md` D-009.

| # | Defect | Reproducing input | Old result | Result now | Test |
|---|---|---|---|---|---|
| 1 | Loader substitution consumed the delimiter after a match, so adjacent identical parameters were skipped | `fillRuleWithVariables("F(a1,a1)", {a1=35})` | `F(35.000000,a1)` | `F(35.000000,35.000000)` | `params: substitution replaces the second of two adjacent identical parameters` |
| 3 | Loader substitution needed a delimiter before the name, so a parameter at the start of a production was not replaced | `fillRuleWithVariables("a1+F", {a1=35})` (T02 wrote `a1F`, which is a different identifier and stays unreplaced) | `a1+F` | `35.000000+F` | `params: substitution replaces a parameter at the start of a production` |
| 10 | Custom parameters were not substituted into the axiom | `legacy/parametricTree2.l` (`define startLength 1.2`) | axiom `B(1)F(startLength)+(45)A` | `B(1)F(1.200000)+(45)A` | `params: custom parameters are substituted into the axiom` |
| 11 | Old `.l` files with bare-number header lines (depth, length, radius, angle): each one-token line was taken as the axiom, the values were ignored and `depth` stayed **uninitialized** | `legacy/advancedTree.l`, `testX/Y/Z.l` | axiom `F`, `initialLength=1`, `initialRadius=1`, `angle=15`, `depth` indeterminate | header values honored (`depth=4`, `0.5`, `0.25`, 7 degrees) | `params: bare-number header lines of old .l files are depth, length, radius and angle` |
| 12 | Default `angle` (15) was stored as 15 radians for loaded files; `define tropism` in `.l` was ignored | `legacy/testX.l` (header angle 15) | `angle=15` | `radians(15)` = 0.261799; `define tropism x y z` is parsed and normalized | `params: the default angle is radians(15) for every constructor` |
| 13 | (found in T03) `legacy/test.l` starts with a UTF-8 BOM, so its first comment line became a junk rule whose "symbol" was the BOM | `legacy/test.l` | `symbols=A ï»¿#depth,` | `symbols=A` | `legacy l: CRLF line ends and a UTF-8 BOM are ignored` (treegen) |

Regenerated params goldens: `legacy__advancedTree.l`, `legacy__testX.l`, `legacy__testY.l`, `legacy__testZ.l` (#11, #12),
`legacy__parametricTree2.l` (#10), `legacy__test.l` (#13). Regenerated solver goldens (they are a function of the loaded axiom
and depth): `legacy__parametricTree2.l.d0..d3` (#10), new `legacy__advancedTree.l.d4` (own depth, #11).

## Fixed in T04 (solver, now `treegen::rewrite`)

`LindenmayerTreeSolver::generateTreeProduction` is an adapter over `treegen::rewrite` (no regex). The `KNOWN BUG:` tests of
`legacy_characterization_tests.cpp` were flipped to the correct expectation and renamed; `tests/treegen/rewriter_tests.cpp` pins the same
behavior on every platform. See `docs/DECISIONS.md` D-014.

| # | Defect | Reproducing input | Old result | Result now | Test |
|---|---|---|---|---|---|
| 2 | `fillRuleWithParameters` consumed the delimiter after a match | rule `A(x) -> F(x,x)` applied to `A(2)` | `F(2,x)` | `F(2.000000,2.000000)` | `solver: substitution replaces the second of two adjacent identical parameters` |
| 4 | Leading minus: expression split at the first operator char | rule `A -> F(-5*2)` | `F(-5.000000)` | `F(-10.000000)` | `solver: an expression with a leading minus is evaluated` |
| 5 | Negative parameter value hits #4 | rule `A(l) -> F(l*3)` applied to `A(-2)` | `F(-2.000000)` | `F(-6.000000)` | `solver: a negative parameter value in an expression is evaluated` |
| 6 | Chained operators and precedence: only the first operator was used | `F(2*3*4)`, `F(1+2*3)` | `F(6.000000)`, `F(3.000000)` | `F(24.000000)`, `F(7.000000)` | `solver: chained operators are evaluated with precedence` |
| 7 | `if (newProduct.length() > 0)` kept the old string when everything rewrote to `""` | rule `A -> ""` | `A` | empty string | `solver: a rule that rewrites everything to the empty string gives the empty string` |
| 8 | Symbols were put unescaped into a regex | rules for `+` and `A` | `std::regex_error` | `+` is rewritten | `solver: rule symbols that are regex metacharacters can be rewritten` |
| 9 | Arity mismatch was silently accepted (and, with too few values, read past the end of `values`) | rule `A(l) -> F(l*2)` applied to bare `A` | `F(l*2)` | `treegen::RewriteError` naming symbol and rule | `solver: a rule with parameters applied to a symbol without arguments is a RewriteError`, `Review Focus 4: ...` |
| 14 | (found in T04) A malformed production was copied into the output: `legacy/parametricTree2.l` has `[^+a)` (a `)` without `(`) | `legacy/parametricTree2.l` at depth >= 1 | `...[^+18.950001)F(1.200000)A]...` | `treegen::RewriteError` (`rule A -> "...": unexpected ')' at offset ...`) | `rewrite goldens: legacy/parametricTree2.l has a malformed production and is a RewriteError` |

Goldens: only `solver/legacy__parametricTree2.l.d1..d3` were deleted (#14: the output was the frozen nonsense). The remaining 160 goldens (34 rule files)
are byte-identical; `treegen::rewrite` reproduces them digit for digit (compared in the canonical `std::to_string` form, because the legacy
string kept literal numbers as written: `+(45)` instead of `+(45.000000)`; see `tests/treegen/rewriter_golden_tests.cpp`).

## Suspected, not confirmed or not testable here

- (Resolved in T04: the solver no longer uses `std::regex`.) `res.suffix()` read after the loop's failed `regex_search` (`generateTreeProduction`, `fillRuleWithParameters`): with the MSVC
  standard library the unmatched tail **is** preserved (tests "solver: ... goldens" pass), so there is no observable bug on this
  toolchain. The standard leaves `match_results` unspecified after a failed search, and on libstdc++ the tail **is lost** (confirmed
  by T11 in Docker/GCC 13, 8 of 26 cases failed). Neither the loader (T03) nor the solver (T04) uses it any more.
- Arity mismatch with too few values (Review Focus 4, `values[i]` past the end): crash or UB, not assertable in-process. Fixed in T04: `RewriteError`.
- Interpreter items (`returnNewIndexAfterParameter` float index, O(n^2) parameter scan, literal `-1` parameter):
  need GL types, out of scope for T02. Owner T05.
- Fixed by T03, covered by the `treegen_tests`: missing file / malformed JSON / missing `axiom` (Review Focus 2), which used to crash.
