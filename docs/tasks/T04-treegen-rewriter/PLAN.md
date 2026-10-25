# T04 — `treegen` rewriter — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: use `superpowers:subagent-driven-development`
> or `superpowers:executing-plans`. Steps use checkbox (`- [ ]`) syntax.

**Status:** in-progress · **Branch:** `task/T04-treegen-rewriter` · **Base:** `modernization @ 5853916`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** Replace the regex/string rewriting with a typed, seeded rewriting engine
(`treegen::rewrite`) with a real expression evaluator, bounded growth and precise errors, and hook the
legacy `LindenmayerTreeSolver` to it.

**Approach:** `Expression` compiles an arithmetic expression over a rule's formal parameters into a small
postfix program (parsed once). `Rewriter` compiles each rule production once into a template (literal
modules whose parameters are `Expression`s), then iterates `depth` times over a `ModuleString`, choosing rules
with `Rng(seed).derive(1)`. Output is a `ModuleString`, never an intermediate string. No `std::regex`, no
Boost, no statics.

## Files

- Create: `ESEngine/TreeGen/include/treegen/Expression.h`, `src/Expression.cpp` — parse/evaluate (`+ - * /`, unary `-`/`+`, parentheses, literals, identifiers)
- Create: `ESEngine/TreeGen/include/treegen/Rewriter.h`, `src/Rewriter.cpp` — `RewriteOptions`, `rewrite()`
- Create: `tests/treegen/expression_tests.cpp`, `tests/treegen/rewriter_tests.cpp`, `tests/treegen/rewriter_golden_tests.cpp`
- Modify: `ESEngine/ESTree/Models/Tree/LindenmayerTreeSolver.{h,cpp}` — delegate to `treegen::rewrite`, drop Boost + static RNG
- Modify: `tests/legacy_characterization/legacy_characterization_tests.cpp` — flip KNOWN BUGS #2, #4–#9
- Modify: `tests/golden/KNOWN_BUGS.md`, `tests/golden/solver/**` (only goldens that hit a bug), `docs/DECISIONS.md` (append D-0xx), `docs/architecture/treegen-design.md` only if an interface changes
- `tests/treegen/CMakeLists.txt` needs no change (`*_tests.cpp` is globbed)

## Interfaces

- Consumes (T03): `Module/ModuleString/parseModules/toString`, `Rule`, `TreeParams`, `Rng(seed).derive(id).uniform01()`, `RewriteError`
- Produces (design doc, unchanged): `struct RewriteOptions { uint32_t seed = 0; }; ModuleString rewrite(const TreeParams&, const RewriteOptions& = {});`
- Additive public helper (internal use, tested directly):
  `class Expression { static Expression parse(std::string_view text, const std::vector<std::string>& names); double evaluate(const float* args, size_t count) const; }`

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | Expression values are `double` internally, parameters/literals enter as `float`, the result is narrowed to `float` | Legacy folded in double (`atof`) and stored a float; keeps golden digits |
| 2 | Productions are compiled lazily when a rule first becomes a candidate (`initialDepth <= iteration`), not for unreachable rules | Depth 0 never fails; an unused broken rule does not break a file |
| 3 | Selection: one `uniform01()` per module whose symbol has at least one applicable rule; rule i is chosen when `cumulative(i-1) <= u < cumulative(i)`; past the sum the module is kept | Legacy semantics, half-open interval (legacy `<= 0` could pick a zero-probability rule on an exact 0 draw) |
| 4 | Arity: rule parameter count must equal module parameter count, otherwise `RewriteError` naming symbol and rule | Review Focus #4 (also replaces legacy "params dropped" for `A(1)` vs rule `A`) |
| 5 | NaN/Inf (also float overflow) in any evaluated parameter → `RewriteError` naming symbol, rule and expression | Review Focus #5 |
| 6 | Growth cap checked whenever a module is appended; `RewriteError` names iteration, size and the limit | Review Focus #3 |
| 7 | Golden comparison is numeric: `toString(parseModules(golden))` vs `toString(rewrite())` | Legacy copies literal numbers verbatim (`+(45)`) but folds results as `std::to_string` (`+(45.000000)`); a `ModuleString` cannot represent the difference. Fixed-point check below |
| 8 | If goldens differ in the 6th decimal because legacy re-reads 6-decimal text each iteration, bind parameters through the same 6-decimal quantization | Behavior preserving (to be decided by measurement, recorded in DECISIONS) |

## Steps

### Step group 1: setup and baseline
- [x] **1.1** Reset to `modernization`, branch, Vendor junctions, configure `ninja-msvc`
- [ ] **1.2** Commit `T04: add plan`
- [ ] **1.3** Temporary (uncommitted) harness: wall time of the legacy solver, `parametricTernaryTreeE.json` depth 10, Release; measured before wiring

### Step group 2: Expression (TDD)
- [ ] **2.1** `expression_tests.cpp`: precedence table (`2+3*4`=14, `(2+3)*4`=20, `-5*2`=-10, `10/4`=2.5, `1-2-3`=-4, `8/4/2`=1, `2--3`=5, `-(1+2)`=-3), parameter binding, whitespace, unknown identifier / unbalanced `(` / trailing garbage / empty → `RewriteError`, `0/0` and `1/0` evaluate to NaN/Inf (checked by the rewriter, not by `evaluate`)
- [ ] **2.2** Run → FAIL (no header)
- [ ] **2.3** Implement recursive-descent parser emitting postfix ops; evaluate with a fixed-size stack
- [ ] **2.4** Run → PASS; commit `T04: add Expression`

### Step group 3: Rewriter (TDD)
- [ ] **3.1** `rewriter_tests.cpp`: depth 0, symbols without rules pass through, empty production, parameter binding with chained expressions, `initialDepth` gating, arity mismatch both ways (Review Focus #4), NaN/Inf (`0/0`, `x/0`, Review Focus #5), growth cap (Review Focus #3), seed determinism, different seeds differ, probability frequencies within ±3σ over 10 000 draws, a draw past the cumulative sum keeps the module, regex-metacharacter symbols (`+`, `^`, `*`) are rewritable, adjacent identical parameters, leading minus / negative values / chained operators
- [ ] **3.2** Run → FAIL
- [ ] **3.3** Implement `Rewriter.{h,cpp}`
- [ ] **3.4** Run → PASS; Linux build; commit `T04: add rewrite()`

### Step group 4: goldens
- [ ] **4.1** `rewriter_golden_tests.cpp`: iterate `tests/golden/solver/*.txt` (via `ESTREE_REPO_ROOT`), load rule file, force depth, compare numerically
- [ ] **4.2** Investigate every mismatch; classify as KNOWN BUG #2/#4–#9 or as a defect of mine; regenerate only the goldens that hit a bug (list in DECISIONS)
- [ ] **4.3** Commit goldens + test

### Step group 5: wire the legacy class, flip KNOWN BUG tests
- [ ] **5.1** `generateTreeProduction` = `toString(rewrite(params, {seed}))`, `seed` = `params.seed` or `std::random_device`; delete Boost + static RNG; keep the public signature
- [ ] **5.2** Flip the seven `KNOWN BUG:` tests in `legacy_characterization_tests.cpp` (#9 as `treegen::RewriteError`); update `KNOWN_BUGS.md`; append D-0xx
- [ ] **5.3** Full Debug build + ctest; commit

### Step group 6: verification
- [ ] **6.1** Windows full build + ctest, `/W4` warnings in new files, Linux Docker (62 + new tests), `ESTree` build + smoke launch
- [ ] **6.2** Release benchmark (legacy numbers from 1.3 vs new)
- [ ] **6.3** Set `Status: review`, commit

## Review focus covered

| Focus | Test |
|---|---|
| #3 growth cap | `Review Focus 3: ...` in `rewriter_tests.cpp` |
| #4 arity | `Review Focus 4: ...` |
| #5 (NaN/Inf from expressions) | `Review Focus 5: ...` |

## Deviations from the brief

- Golden comparison is numeric, not byte for byte (decision 7): legacy output mixes verbatim literals (`45`) and `std::to_string` results; a `ModuleString` can only print the canonical form. The goldens stay byte-identical unless the file hits a listed bug.

## Blockers

(none)
