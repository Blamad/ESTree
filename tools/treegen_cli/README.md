# treegen_cli

Headless tree generator: reads a Lindenmayer rule file, grows a tree with the engine-independent `treegen`
library and writes it as a Wavefront OBJ file. It links only `treegen` (no engine, OpenGL or Boost).

Built by default (`-DESTREE_BUILD_TOOLS=OFF` skips it), also when `-DESTREE_BUILD_ENGINE=OFF`.

## Usage

```
treegen_cli <rules.json|rules.l> --out <tree.obj> [--seed N] [--depth D] [--ring R] [--threads T]
```

| Option | Meaning |
|---|---|
| `<rules>` | rule file, `.json` or legacy `.l` (e.g. `Resources/LindenmayerRules/parametricMonopodialTreeA.json`) |
| `--out FILE` | OBJ file to write; missing parent directories are created |
| `--seed N` | seed, `0..4294967295`; default `0` = the `seed` key of the rule file if it has one. Same seed gives a byte-identical file |
| `--depth D` | overrides the iteration count of the rule file |
| `--ring R` | vertices per trunk ring (seam vertex included), `R >= 3`, default 9 |
| `--threads T` | geometry threads, `0` automatic (default), `1` none; the output does not depend on it |
| `--help` | usage, exit code 0 |

`--opt=value` is accepted as well as `--opt value`.

### Exit codes

| Code | Meaning |
|---|---|
| 0 | ok |
| 2 | usage error (unknown option, missing or invalid value, missing rule file or `--out`) |
| 3 | the rule file cannot be loaded; the message on stderr names the file |
| 4 | generation failed (rewrite/interpret error, too many modules) or the OBJ file could not be written |

### Output

One line on stdout:

```
segments=256 vertices=4608 triangles=4096 leaves=128 seed=1 elapsed_ms=43 out=tree.obj
```

`segments` counts the skeleton segments without the root, `vertices`/`triangles` are the trunk mesh, `leaves` the leaf quads,
`elapsed_ms` covers load, generation and writing. Errors go to stderr.

The OBJ file has object `trunk` (`v`, `vn`, `vt`, then `f v/vt/vn` triangles) and object `leaves` (one double-sided 2x2 quad
per leaf, 8 vertices each). Indices are 1-based, numbers use `.` and are written with their shortest exact representation.
There is no MTL file; open it in Blender or MeshLab.

## Example

```
treegen_cli Resources/LindenmayerRules/randomTree.json --seed 7 --out out/random7.obj
```

## Tests

CTest entries `treegen_cli_*` (see `CMakeLists.txt`) run the executable through the helper `tests/obj_check.cpp`
(exit codes, OBJ validation, byte comparison); no shell scripts are involved, so they run the same on Windows and Linux.
