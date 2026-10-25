# T12 — Dependency refresh — Implementation Plan

**Status:** in-progress  ·  **Branch:** `task/T12-dependency-refresh`  ·  **Base:** `modernization @ 5c90420`
**Brief:** [BRIEF.md](BRIEF.md)

**Goal:** Move the `Vendor/` submodule pins to current stable release tags where that is low-risk, fixing the resulting API breaks.

**Approach:** One submodule per commit (`T12: bump <lib> to <tag>`), in the order stb, rapidjson, FreeType, GLFW, Bullet, glm. After each bump: reconfigure, build `ninja-msvc` Debug (+Release), full `ctest`, scene-0 smoke launch. A bump that cannot be made green with minimal edits (or breaks `treegen` parity) is reverted and documented. Linux (Docker) is run after the glm bump and at the end.

## Files

- Modify: `Vendor/*` gitlinks, `.gitmodules` (only if branch hints change), minimal engine/CMake/test edits forced by API changes (listed in the final report)
- Modify: `docs/architecture/overview.md` (Build: final versions), `docs/DECISIONS.md` (append D-019+ for bumps needing code changes)

## Pins (before)

glfw `3.3-213-g44b5d065`, glm `0.9.5.3-2451-gefbfecab`, bullet3 `2.89-196-g9c8b87dd`, freetype2 `VER-2-10-1-111`, rapidjson `v1.1.0-542`, stb `f54acd4`, glad branch `c` (left as is).

## Targets

stb master, rapidjson master, freetype2 `VER-2-15-0`, glfw `3.4`, bullet3 `3.25`, glm `1.0.3`.

## Decisions

| # | Decision | Why |
|---|---|---|
| 1 | Real submodule checkouts in the worktree, objects borrowed with `--reference` | Pins move; a junction would move the main checkout |
| 2 | Warning baseline: full `ninja-msvc` Debug build (all targets), unique `file|line|code`, `Vendor/` excluded: **121** | Same toolchain, like with like |
| 3 | glad left alone | Regeneration is not a one-liner (generated loader, not a tag) |

## Steps

- [x] Baseline: build Debug, 232 ctest pass, scene-0 smoke OK (121 warnings)
- [ ] stb, rapidjson, FreeType, GLFW, Bullet, glm bumps (one commit each, verify each)
- [ ] Linux Docker run (219 tests), Release build + ctest, final smoke
- [ ] Docs: overview.md Build, DECISIONS.md; status `review`

## Deviations from the brief

(none yet)

## Blockers

(none)
