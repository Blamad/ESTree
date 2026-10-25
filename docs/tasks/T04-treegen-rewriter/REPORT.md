# T04 — `treegen` rewriter — Report

**Result:** done · **Branch:** `task/T04-treegen-rewriter` · **Final commit:** `387339d` (merged into `modernization` as `e5f0405`)

The implementing subagent could not write this file; the orchestrator wrote it from the agent's hand-back and
**re-verified on the merged tree** (marked *verified*).

## What changed

- New `ESEngine/TreeGen/{include/treegen,src}/{Expression,Rewriter}.{h,cpp}`.
  - **Expression:** recursive-descent parser (`+ - * /`, unary `±`, parentheses, literals, identifiers bound to rule parameters; standard precedence, left associative), compiled to a postfix program, evaluated without allocation; nesting depth capped at 64.
  - **Rewriter:** each production compiled once into literal modules + expressions (lazily, when a rule first becomes a candidate). Selection with `Rng(seed).derive(1)`: one draw per rewritten module, rule k wins when `cum(k-1) <= u < cum(k)`, a draw past the sum keeps the module. `maxModules` checked on every append (and for the axiom). `RewriteError`s name symbol, rule or expression: arity mismatch, malformed production/expression, NaN/Inf/float overflow, growth.
- `LindenmayerTreeSolver` is now `toString(rewrite(params, {seed}))` (seed = `params.seed`, else `random_device`); no Boost, no static RNG, signature unchanged.
- Tests: `tests/treegen/{expression,rewriter,rewriter_golden}_tests.cpp` (45 cases incl. `Review Focus 3`, `4`, `5`, seed determinism for the 9 shipped stochastic files, 10 000-draw frequencies within ±3σ, `initialDepth` gating). KNOWN BUGS #2 and #4–#9 flipped in `legacy_characterization_tests.cpp`.

## Acceptance criteria

| Criterion | Evidence |
|---|---|
| Deterministic goldens reproduced through `rewrite` | agent: test `rewrite goldens: every deterministic rule file reproduces its solver goldens` passes on Windows and Linux (34 files / 160 goldens, digit for digit) |
| ctest + boundary green | *verified*: ninja-msvc Debug `100% tests passed out of 146`; Docker Ubuntu 24.04/GCC 13 engine OFF `100% tests passed, 0 tests failed out of 107`. Agent: Release 146/146 |
| `/W4` and GCC warnings clean for new files | agent: 0 warnings for TreeGen, tests/treegen, solver, legacy test; GCC `-Wall -Wextra -Wpedantic` silent |
| Review Focus #3, #4, #5 (NaN part) | named tests in `rewriter_tests.cpp` |
| `ESTree` builds, runs | *verified*: merged build exit 0; scenes 0 and 2 run, trees generate (`23328 tris`) and close cleanly (`Quitting..`) |
| No Boost in the solver | none left (transitively `LindenmayerTreeParams.h` still includes it; T07) |

## Benchmark (Release /O2, `parametricTernaryTreeE.json`, depth 10, 3 runs, agent)

| | Time |
|---|---|
| Legacy `generateTreeProduction` | 2.16–2.55 s (4.46 MB string) |
| New `rewrite` | 115–190 ms (708 581 modules) |
| New `toString` | 270–410 ms |

`rewrite` alone ≈ 15–20× faster, ≈ 5× including `toString`.

## Decisions / deviations (D-014)

1. **Canonical comparison of goldens**: legacy kept literals verbatim (`+(45)`) and printed computed numbers as `+(45.000000)`; a `ModuleString` can only print the latter. Golden tests compare `toString(parseModules(golden))` with `toString(rewrite(...))` — every digit compared, no golden rewritten.
2. **Six-decimal numeric model** (legacy-compat device, may be dropped later): computed values are rounded to six decimals; arguments that are the float nearest a six-decimal number are evaluated as that decimal in double precision. Without it 21 of 34 files differed in the 6th decimal.
3. **New KNOWN BUG #14**: `legacy/parametricTree2.l` contains a stray `)` (`[^+a)`); legacy copied it into the output, the rewriter throws `RewriteError` naming rule A. Only `solver/legacy__parametricTree2.l.d1..d3` goldens were deleted; `Resources/` untouched.
4. Behavior changes: a parameterless rule applied to `A(1)` is now an arity error (legacy silently dropped `(1)`); one `uniform01()` draw is consumed per module whose symbol has applicable rules.

## Interface changes

Additive: new `Expression.h`; `treegen-design.md` gained a "Rewriter behaviour" section. `RewriteOptions`/`rewrite` exactly as designed.

## Needs human verification

Scenes 0 and 2 should still show plausible trees. Stochastic rules now come from `std::mt19937` via `Rng`, so a given seed yields different trees than the old `time(0)` RNG.

## Follow-ups

- T07: the console `tree file legacy/parametricTree2.l` now throws `RewriteError` (fix the rule file, likely `[^(a)`, and catch `RewriteError`/`LoadError` in `ConsoleTreeCommand`).
- `toString` is the slow part (`std::to_string` per parameter) — optional optimization.
