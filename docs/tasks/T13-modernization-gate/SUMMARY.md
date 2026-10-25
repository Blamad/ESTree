# ESTree modernization — summary (2026-10-25)

Written by the orchestrator from the T13 hand-back and the verification runs (the harness does not let subagents write this file).
Branch: `modernization` (integration); `master` is still `c30a23c`.

## What was achieved

- **Build:** Boost is gone from sources, CMake, scripts and CI; no environment variable is needed any more. CMake ≥ 3.21 with presets (`vs2019`, `ninja-msvc`, `vs2026`), C++17, `scripts/msvc-env.cmd`, preset-based `setup.bat`. Dependency pins refreshed (T12: GLFW 3.4, glm 1.0.3, Bullet 3.25, FreeType 2.15.0, current stb and rapidjson).
- **`treegen`:** a pure C++17 library (std + glm + rapidjson only, enforced by a CTest and by the compile line) with a seeded, deterministic pipeline: loaders (JSON, legacy `.l`) → expression/rewriter → turtle → skeleton → geometry → `generateTree`. A headless `treegen_cli` and an OBJ writer prove it is engine-free. The legacy `Lindenmayer*` classes are deleted; the engine's `LindenmayerTree` is a thin adapter.
- **Fixed along the way:** regex loader/solver defects (14 documented), the unbalanced `]` crash, the malformed-file crash, unbounded growth, NaN geometry, fixed 100 000-vertex GL buffer caps, the tropism no-op (D-010, D-016), the Intel-iGPU-by-default start on the Optimus laptop (discrete-GPU hint), and several ownership/lifetime bugs in the engine (T10: nothing was ever destroyed, non-virtual destructors, dangling selection pointers, never-deleted Bullet objects).
- **Engineering infrastructure:** doctest + CTest harness (232 tests), golden data for every shipped rule file, ASan presets, CI workflow (Windows + Linux), Docker-verified Linux builds, `CLAUDE.md` + `docs/` with plan, decisions (D-001…D-020) and per-task reports.

## Numbers

- 232 CTest entries green on Windows (MSVC 14.29 and 19.51) and Linux (GCC 13); 219 with the engine off.
- `/W4` unique project warnings: 109 on a clean `ninja-msvc` Debug build (T13 agent's method); earlier numbers 143 (vs2019 after T00), 191 (ninja after T09, older code), 105 (orchestrator's script) and 121 (T12 agent) were each taken by a different method — compare only like with like.
- Ternary E depth 10 (2.1 M vertices, 1.9 M triangles): `generateTree` ≈ 190 ms in Release; the old rewriter alone took 2.2–2.5 s for the same depth.

## Remaining debt

The Windows CI job builds all of Bullet (slow; restrict to the three libraries in use); the engine runs only on the NVIDIA GPU (Intel HD 530 driver crash, BACKLOG); 26 C-style casts remain in `ESEngine/ESTree`; GL objects are never deleted; project warnings remain; `obj_tests` write to the OS temp dir; the rewriter keeps a six-decimal numeric model for legacy digit-compatibility (D-014); owner verification is partial (below).

## Recommended next steps

1. (done) `modernization` is pushed and CI is green on GitHub.
2. Finish the checklist below and decide on merging into `master`.
3. Cheap wins: restrict Bullet to the three libraries in CI, warning cleanup with `/WX`, GLFW 3.5.1.
4. Product work (see `docs/BACKLOG.md`): rule conditions, smooth shared-joint geometry, glTF export, then a Blender/Godot plugin or a WASM rule editor on top of `treegen`.

## Owner checklist (still open; items the owner already confirmed are omitted)

Already confirmed by the owner: window closes cleanly; moving a crate; `rm` twice then `normals on`; `ternaryB_seed42.obj` looks good; T07 items 1–4 (branches droop as intended and the model is coherent, trees render and exits are clean, the seed is present, seeded regeneration works).

Setup: `cd build\ninja-msvc` (or `build\vs2019` after `setup.bat`), run `Debug\ESTree.exe --scene N` on the NVIDIA GPU; `Resources` must be linked next to the binaries.

1. ~~CI~~ — done: pushed, fixed two workflow bugs (PowerShell `-D...=3.5` splitting, mixed generators in a shared FetchContent cache) and confirmed green by the owner on 2026-10-25 (Windows Debug/Release with the engine, Linux ON/OFF). Still worth a glance: the README badge URL (`Blamad/ESTree`, `ci.yml`).
2. **Scenes 1, 2, 4:** scene 1 physics behaves sensibly and LMB picks a crate (Bullet 3.25); scenes 2 and 4 trees render; grass looks as before.
3. **Console text:** `~`, `help` — text renders (FreeType 2.15), `tree`/`rtree` list `seed <int>`; window, input and resize work (GLFW 3.4).
4. **Seeds (T07 items 5–6):** `tree file 4` twice gives different trees and seeds (not file 15, it is deterministic); with a tree selected `rtree` gives a new tree and seed, `rtree seed 42` reproduces the tree from `tree file 4 ... seed 42`.
5. **Errors and large trees (T07 item 8):** `tree foo 1` or a missing file prints one console line, no crash; `tree file 18` works; with a Cube selected `rtree` prints "Selected object is not a tree!".
6. **`legacy/parametricTree2.l`:** look at the first branch and choose between `[^+(a)` (applied), `[^(a)` and `[+(a)` (D-017).
7. **Trees without `T`:** `tree file parametricMonopodialTreeA.json` and `tree file symetricTree.json` look as before.
8. **Header-form `.l` files (optional):** `legacy/advancedTree.l`, `testX.l`, `testY.l`, `testZ.l` now use their header values (D-009); confirm they look acceptable.
9. **Export and commands:** `export x` then compare `models/x.obj`/`.mtl` with output of the old `build\Debug\ESTree.exe`; `fps` and `fps clr`; lights and cubes via the console; click an unnamed object; `ESTree.exe -W 800 -H 600 -s 3 -f` starts fullscreen.
10. **Leaf sway:** does it pause while the camera moves? Hold RMB without moving, then move with keys only, and compare with the old exe (code review found no link to camera movement — see BACKLOG).
11. **Decisions:** merge `modernization` into `master` (still `c30a23c`); keep GLFW 3.4 or bump; delete the old `build\` content when the reference exe is no longer needed.
