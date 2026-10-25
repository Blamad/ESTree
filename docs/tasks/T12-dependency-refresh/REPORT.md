# T12 — Dependency refresh — Report

**Result:** done · **Branch:** `task/T12-dependency-refresh` · **Final commit:** `c3a9f12` (merged into `modernization` as `d4927bb`)

The implementing subagent could not write this file; the orchestrator wrote it from the agent's hand-back. The main checkout's submodules were then synced to the new pins
(`git submodule update --init`, which fetched the new commits from GitHub) and the merged tree re-verified (results at the end).

## Bumps (every bump kept, none reverted; no source, CMake or test edit was needed)

| Library | Old pin | New pin | Commit |
|---|---|---|---|
| stb | `f54acd4` | master `003501f` | `T12: bump stb to master 003501f` |
| rapidjson | `v1.1.0-542` | master `24b5e7a` (`v1.1.0-763`) | `T12: bump rapidjson to master 24b5e7a` |
| FreeType | `VER-2-10-1-111` | `VER-2-15-0` (`37515cf`) | `T12: bump freetype2 to VER-2-15-0` |
| GLFW | `3.3-213` | `3.4` (`7b6aead`) | `T12: bump glfw to 3.4` |
| Bullet | `2.89-196` | `3.25` (`2c204c4`) | `T12: bump bullet3 to 3.25` |
| glm | `0.9.5.3-2451` | `1.0.3` (`8d1fd52`) | `T12: bump glm to 1.0.3` |
| glad | branch `c` `5bf3eda` | unchanged (regenerating is not a one-liner) | — |

GLFW 3.5.1 also exists; 3.4 was used as the brief says. stb and rapidjson follow master (no release tags).

## glm audit

No `glm/gtx` header is used; `glm/gtc/quaternion.hpp` only in `Turtle.cpp`, `Geometry.cpp`, `Transform.h`; no default-constructed `glm::quat()`/`glm::mat4()` in `ESEngine/`
(every quaternion is explicit; `treegen` uses an identity constant, D-010). No `GLM_*` define needed; no golden regenerated. D-019 records this.

## Agent's verification

- Baseline on the unmodified pins: Debug build OK, 232/232 ctest, scene-0 smoke OK.
- After each bump: configure OK, Debug build OK, 232/232 ctest; after the glm bump also Release build + 232/232.
- Smoke launches (14 s, `CloseMainWindow()`): scene 0 after FreeType and glm, scene 2 after GLFW, scene 1 after Bullet — alive, expected log lines, no `Application Error`. Not smoke-launched after stb/rapidjson.
- Linux Docker (Ubuntu 24.04, GCC 13, engine OFF) run once after the final bump: 219/219 (`treegen` goldens and parity green with glm 1.0.3 and the new rapidjson).
- `/W4` unique project warnings (full clean Debug, `Vendor/` excluded): 121 before and 121 after (the older 191 is for an earlier code state; 121 is the like-for-like number at that tip).

## Needs human verification

1. Scenes 0, 1, 2: rendering looks as before.
2. Scene 1 (Bullet 3.25): physics behaves sensibly and mouse picking selects objects.
3. Console text renders correctly (FreeType 2.15); window, input and resize work with GLFW 3.4.
4. `tree`/`rtree`: trees look as before (the geometry is bit-identical in the goldens).

## Follow-ups

- Consider GLFW 3.5.1; `Vendor/stb` and `Vendor/rapidjson` follow master.
- Bullet object paths exceed the Windows path limit in deep worktree paths (harmless; a shallower build path would be safer).
