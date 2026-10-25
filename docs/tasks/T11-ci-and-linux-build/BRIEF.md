# T11 — CI (Windows + Linux) and headless Linux build

**Depends on:** T01 (more valuable after T03+) · **Unblocks:** — · **Size:** M · **Branch:** `task/T11-ci-and-linux-build`

## Goal

GitHub Actions that, on every push/PR, build and test the project on `windows-latest` (MSVC) and
`ubuntu-latest` (GCC and/or Clang), so every later task is verified automatically — and prove
that `treegen` + tests + CLI build on Linux without a display.

## Context to read

- `CMakeLists.txt`, `CMakePresets.json` (T00), `tests/README.md` (T01), `.gitmodules`
- `docs/PLAN.md` → Global Constraints (headless tests, no network except dependency fetch)

## Scope

In:
1. `.github/workflows/ci.yml`: triggers `push` (all branches except docs-only paths) and `pull_request`; matrix: Windows MSVC Debug+Release; Ubuntu GCC Release. Checkout with submodules (`submodules: recursive`), cache FetchContent downloads and the build dir where safe, run `ctest --output-on-failure`, upload test logs on failure.
2. Engine target on Linux: make a **best-effort** attempt to build the full `ESTree` executable with `xvfb`-free compile-only (no run). Needs: GL headers/`libx11-dev` etc. via `apt`, case-correct includes, no Windows-only code. Fix portability issues with minimal edits in the files listed below; if the engine cannot be made to compile in ≤ ~2 hours of effort, make the Linux job build only `treegen`, `treegen_tests`, `legacy_characterization_tests` (if still present) and `treegen_cli`, and list the blockers in `REPORT.md`. Gate engine build behind CMake option `ESTREE_BUILD_ENGINE` (default ON on Windows; OFF on Linux until fixed).
3. A status badge line in `README.md`.
4. Document in `tests/README.md` how to reproduce CI locally.

Out of scope: macOS, release packaging, code signing, GPU tests, dependency bumps.

## Owned files

`.github/workflows/**`, `ESTREE_BUILD_ENGINE` option block in `CMakeLists.txt`, portability edits strictly required to compile on Linux (list them), CI badge in `README.md`.

## Acceptance criteria

1. `actionlint` (if available) or a YAML parse of the workflow passes; the workflow runs the same configure/build/test commands you executed locally (show them).
2. Local proof on this Windows machine: the exact `ctest` command used by CI passes. If WSL/Docker is available, also run the Linux commands there and record the output; otherwise state **"Linux job not executed locally"** in the report — do not claim it passes.
3. `ESTREE_BUILD_ENGINE=OFF` configure + build + test works on Windows (this is what the Linux job relies on).

## Verification

First real run happens on GitHub — **needs human action:** push the branch and report the first CI result (or paste the failing log back).

## Notes

- Without network access a fresh configure cannot fetch doctest; CI has network.
- Keep jobs under ~15 min; Bullet is the long pole — build only the Bullet targets the engine needs (`BulletDynamics BulletCollision LinearMath`).
