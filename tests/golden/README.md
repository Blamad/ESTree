# Golden data for the legacy characterization tests

Produced by `legacy_characterization_tests` (`tests/legacy_characterization/`), which compiles the unchanged
`LindenmayerTreeParams.cpp` and `LindenmayerTreeSolver.cpp`. They record what the legacy code does today so
that T03/T04 can prove a rewrite preserved it.

| Path | Content |
|---|---|
| `params/<id>.txt` | `key=value` dump of the loaded params of every shipped rule file (axiom, depth, lengths, angle in radians, tropism, custom parameters, every rule with its substituted production, probability, initialDepth, parameter names). Floats are `%.9g`. `depth=UNSET` = the file never sets it |
| `solver/<id>.dN.txt` | production string at depth N for the deterministic rule files, see `solver/README.md` |
| `KNOWN_BUGS.md` | confirmed defects; the `KNOWN BUG:` tests assert the current wrong behavior |

`<id>` = path below `Resources/LindenmayerRules/` with `/` replaced by `__` (`legacy__test.l`).
Files are compared byte for byte, never line-ending converted (`.gitattributes`: `-text`).

## Regenerating

```bash
legacy_characterization_tests --regenerate      # rewrites params/ and solver/; off by default
git diff tests/golden                           # review every change by eye
```

Run it from any directory (paths come from the `ESTREE_REPO_ROOT` define). A normal run never writes.

## Rule: goldens change only with a `DECISIONS.md` entry

A golden diff means observable behavior changed. Either it is a regression (fix the code) or an intentional
change (bug fix, new default): then add an entry to `docs/DECISIONS.md` naming the golden files and the
reason, and regenerate in the same commit. Never regenerate just to make a test pass. Adding a new shipped
rule file needs new goldens (the completeness test fails until they exist). When a task fixes a `KNOWN BUG:`
test, it flips that test's expectation, updates `KNOWN_BUGS.md` and logs it in `DECISIONS.md`.
