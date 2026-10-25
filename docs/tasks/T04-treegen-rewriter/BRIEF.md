# T04 — `treegen` rewriter

**Depends on:** T03 · **Unblocks:** T07, T08 · **Size:** L · **Branch:** `task/T04-treegen-rewriter`
**Parallel with:** T05 (different files)

## Goal

Replace the regex/string rewriting with a typed, seeded rewriting engine:
`ModuleString rewrite(const TreeParams&, const RewriteOptions&)`, with a real expression
evaluator, bounded growth and precise errors — and hook the legacy `LindenmayerTreeSolver` to it.

## Context to read

- `docs/architecture/treegen-design.md` (Rewriter, Module), `docs/architecture/overview.md` → *Rule syntax*, *Suspected defects*
- `ESEngine/ESTree/Models/Tree/LindenmayerTreeSolver.{h,cpp}` (behavior to preserve)
- `tests/golden/solver/**`, `tests/golden/KNOWN_BUGS.md`, `tests/legacy_characterization/**`
- `ESEngine/TreeGen/**` as left by T03 (`Module`, `Rng`, `TreeParams`)

## Scope

In:
1. `Expression.h/.cpp` (internal or public): parse + evaluate numeric expressions over `+ - * /`, unary minus, parentheses, numeric literals, and identifiers bound to rule parameters. Standard precedence/left associativity. Parse once per rule (compile production text into a template: literal modules + parameter expressions), not per application.
2. `Rewriter.h/.cpp`: `rewrite()` iterates `params.depth` times over a `ModuleString`:
   - match by `symbol`; arity must equal `rule.paramNames.size()` (modules without params match rules with none);
   - candidates = rules for the symbol with `initialDepth <= iteration`; pick by cumulative `probability` using `Rng(seed).derive(1)`; if the draw is past the cumulative sum, the module is kept unchanged (legacy behavior);
   - substitute parameters, evaluate expressions, emit `Module`s;
   - `maxModules` exceeded → `RewriteError` (Review Focus #3); arity mismatch → `RewriteError` naming symbol/rule (Review Focus #4);
   - expression evaluation producing NaN/Inf (e.g. division by zero) → `RewriteError` naming the expression (Review Focus #5, first half).
3. Legacy compatibility: `ModuleString` → `toString()` must equal the legacy production for every deterministic golden (`tests/golden/solver/**`). Where legacy is wrong (leading-minus folding — see `KNOWN_BUGS.md`), implement the **correct** math, flip the `KNOWN BUG` tests, and add a `DECISIONS.md` entry; goldens for rules that hit the bug get regenerated in the same commit with the reason in the message.
4. Wire-up: `LindenmayerTreeSolver::generateTreeProduction(params)` becomes `toString(treegen::rewrite(params, {seed}))` where `seed` comes from `params.seed` or, if 0, a fresh `std::random_device` value (so the engine still gives varied trees; T07 exposes the seed). Remove the static Boost RNG from the solver.
5. Tests in `tests/treegen/rewriter_tests.cpp`: expression precedence table (`2+3*4`, `(2+3)*4`, `-5*2`, `10/4`, `1-2-3`), parameter binding, arity errors, depth/`initialDepth` gating, probability selection frequencies over 10 000 draws within ±3σ and **seed determinism** (same seed ⇒ identical string; different seeds differ for a stochastic rule), growth cap, NaN/Inf, empty production, symbols with no rule pass through unchanged, and the golden comparison for all deterministic shipped rules.

Out of scope: turtle/geometry, context-sensitive rules or conditions, performance micro-tuning beyond "no O(n²) string concatenation".

## Owned files

`ESEngine/TreeGen/include/treegen/{Rewriter,Expression}.h`, `src/{Rewriter,Expression}.cpp`, `tests/treegen/rewriter*`, `ESEngine/ESTree/Models/Tree/LindenmayerTreeSolver.*`, `tests/golden/solver/**` (only per item 3).

## Acceptance criteria

1. All T02 solver goldens pass through `treegen::rewrite` (except documented, logged fixes).
2. `ctest` green, boundary test green, `/W4` clean for new files.
3. Review Focus #3, #4, #5(NaN part) each pinned by a named test.
4. Benchmark note in `REPORT.md`: wall time of `rewrite` at depth 10 for `parametricTernaryTreeE.json` (or the largest feasible depth ≤ 30 s) legacy vs new, same machine, Release. Expect a large speed-up; report the numbers, don't assert a threshold.
5. `ESTree` still builds; `LindenmayerTreeSolver` has no Boost includes left.

## Verification

Automated. **Human check (optional):** scene 0 and scene 2 still generate plausible trees (stochastic rules now come from `std::mt19937`).

## Notes

- Float formatting for `toString` must remain `std::to_string(float)` to keep goldens identical.
- Legacy parameter substitution happens at **load time** for custom parameters (T03) and at **rewrite time** for rule-local parameters; keep that split.
- Choose a compact `Module` storage if profiling shows allocation dominating (e.g. small inline array) but keep the public `Module` shape from the design doc.
