# T12 — Dependency refresh

**Depends on:** T07, T09, T10 (run alone — touches `Vendor/` pins and CMake) · **Unblocks:** T13 · **Size:** M · **Branch:** `task/T12-dependency-refresh`

## Goal

Move the submodule pins to current, stable releases where that is low-risk, fixing the resulting
API breaks, and document the resulting versions.

## Current pins (2026-10-25, `git submodule status`)

| Submodule | Pinned | Notes |
|---|---|---|
| `Vendor/glfw` | `3.3-213-g44b5d065` (Mar 2020) | → latest 3.4 stable tag |
| `Vendor/glm` | `0.9.5.3-2451-gefbfecab` (master, ~2020) | → `1.0.x` tag; check `GLM_ENABLE_EXPERIMENTAL` (gtx headers are used: `matrix_decompose`, `vector_angle`), `glm::quat` default-ctor change in 1.0 (identity vs uninitialized — the legacy code relies on `glm::quat()`!) |
| `Vendor/bullet3` | `2.89-196-g9c8b87dd` | → latest stable tag (3.2x); only `BulletDynamics BulletCollision LinearMath` are used |
| `Vendor/freetype2` | `VER-2-10-1-111` | → latest stable tag |
| `Vendor/rapidjson` | `v1.1.0-542` | master is fine; no tag since 2016 |
| `Vendor/stb` | master 2020 | → current master |
| `Vendor/glad` | branch `c` | generated loader; regenerate for GL 4.3 core only if it is a one-liner, otherwise leave |

## Context to read

- `.gitmodules`, `CMakeLists.txt`, `docs/DECISIONS.md`
- Reports of T00 (toolchain), T07 (what still uses glm quats), T10

## Scope

In:
1. One submodule at a time (commit per bump): update pin to a **tag**, build Debug+Release, run `ctest`, fix breaks, commit `T12: bump <lib> to <tag>`.
2. For glm 1.0: audit every `glm::quat()` / `glm::mat4()` default construction and every `glm/gtx` include; define `GLM_FORCE_QUAT_DATA_WXYZ`/`GLM_ENABLE_EXPERIMENTAL`/`GLM_FORCE_CTOR_INIT` as required; the T05/T06 parity tests are the safety net — they must stay green.
3. Record the final versions in `docs/architecture/overview.md` → *Build* and a `DECISIONS.md` entry per bump that needed code changes.
4. If a bump needs more than ~half a day of fixes, **revert that bump**, note it in `REPORT.md` under *Follow-ups*, and continue with the next.

Out of scope: adding new dependencies, build-system rewrite, switching to vcpkg/Conan.

## Owned files

`Vendor/**` pins, `.gitmodules`, minimal engine edits forced by API changes (list them).

## Acceptance criteria

1. Per bumped library: tag name + commit in `REPORT.md`; Debug+Release build and `ctest` green after each bump (show the final run).
2. `/W4` count not above baseline from vendor-induced warnings in project code.
3. `git submodule status` output pasted in the report.

## Verification

Automated. **Needs human verification:** run scenes 0, 1 (Bullet), 2; mouse picking and console text (FreeType) still work.
