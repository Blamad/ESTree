# Golden data of the shipped rule files

Produced in T02 by the original (regex based) implementation, which was deleted in T07. They record what the legacy code did so that the
`treegen` rewrite could prove it preserved it; since T07 they are the frozen reference data of `treegen` itself. The `params/` goldens are
checked, on every platform, against `treegen::loadTreeParams` by `tests/treegen/loader_golden_tests.cpp`, the `solver/` goldens against
`treegen::rewrite` by `tests/treegen/rewriter_golden_tests.cpp`, and `tests/treegen/golden_data_tests.cpp` checks that the directories are
complete and have no stale files.

| Path | Content |
|---|---|
| `params/<id>.txt` | `key=value` dump of the loaded params of every shipped rule file (axiom, depth, lengths, angle in radians, tropism, custom parameters, every rule with its substituted production, probability, initialDepth, parameter names). Floats are `%.9g`. `depth=UNSET` = the file never sets it |
| `solver/<id>.dN.txt` | production string at depth N for the deterministic rule files, see `solver/README.md` |
| `KNOWN_BUGS.md` | the defects of the legacy loader and solver that were found while characterizing them, each with the `treegen` test that now pins the correct behavior |

`<id>` = path below `Resources/LindenmayerRules/` with `/` replaced by `__` (`legacy__test.l`).
Files are compared byte for byte, never line-ending converted (`.gitattributes`: `-text`).

## Regenerating

There is no regeneration switch any more (it lived in the deleted legacy test). When a change is intentional, write the new file by hand
or with a throwaway program that prints `dumpParams(...)` (the format is in `loader_golden_tests.cpp`) or `treegen::toString(treegen::rewrite(...))`
for the golden's depth and seed 7, then review `git diff tests/golden` by eye. T07 did this for `legacy/parametricTree2.l`
(`params/legacy__parametricTree2.l.txt` edited, `solver/legacy__parametricTree2.l.d1..d3.txt` written with treegen), see D-017.

## Rule: goldens change only with a `DECISIONS.md` entry

A golden diff means observable behavior changed. Either it is a regression (fix the code) or an intentional
change (bug fix, new default): then add an entry to `docs/DECISIONS.md` naming the golden files and the
reason, and regenerate in the same commit. Never regenerate just to make a test pass. Adding a new shipped
rule file needs new goldens (the completeness test fails until they exist). 
