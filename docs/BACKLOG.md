# Backlog (out of scope for the current plan)

Ideas that came up during analysis. Not scheduled; promote to a task only on the owner's decision.

## Generator (`treegen`)
- Conditions and context-sensitive rules (`A(x) : x > 3 -> ...`, `B < A > C`), `#define`-style named constants in rule files.
- Shared-joint smooth geometry (one ring per node, per-vertex smoothed normals, branch-collar blending), bark UV tiling by length.
- LOD chain + billboard impostors; leaf-card atlas with alpha-to-coverage.
- Wind data per vertex (branch depth, sway weight) baked into vertex color / extra attribute.
- glTF 2.0 export (binary), vertex colors; USD maybe.
- Rule-file v2 (versioned JSON schema, validation messages with line/column).
- Growth animation (re-run the turtle with a growth parameter instead of full regeneration).

## Productization paths (see the analysis in the 2026-10-25 conversation)
- Blender add-on / Godot GDExtension / Unity native plugin using `treegen` as the core.
- WASM build + web rule editor with live preview and glTF export.
- Python bindings (pybind11) for procedural-content pipelines.

## Engine
- Intel iGPU compatibility: `ESTree.exe` crashes inside `ig9icd64.dll` (Intel HD 530, driver 31.0.101.2111) at a deterministic call; works on NVIDIA. Find the offending GL call (KHR_debug, bisect scene setup) — see T00 REPORT.
- PBR materials + IBL, cascaded shadow maps, frustum culling, render queue sorting.
- Replace subroutines/`std140 binding=1` with a GL 3.3/4.1-compatible path (macOS), or a Vulkan/WebGPU backend behind `Renderer`.
- Dear ImGui instead of the hand-written console UI for parameter editing.
- Typed component access (`getComponent<T>()`) replacing C-casts everywhere (T10 only fixes the worst cases).

## From T10 (ownership pass)
- 26 remaining C-style casts in `ESEngine/ESTree`; typed `getComponent<T>()` migration there.
- Removal of the active camera is unguarded; static `Context` pointers outlive their objects.
- GL objects are never deleted (`glDelete*`), fine at exit, wrong if scenes are ever reloaded.

## Observations from owner testing
- Leaf sway seems to stop while the camera moves (owner, 2026-10-25, after T07). Not reproduced by code reading: sway is a pure function of the `time` uniform (`Context::setTime(glfwGetTime())` once per frame in `Engine::renderingLoop`, `GenericShader.vert` `calcMove`), none of which changed since the original commit (`git diff c30a23c HEAD` shows only destructor order in `Engine.cpp`), and the camera code only moves the camera. Probably pre-existing or perceptual (slow, low-frequency sway); to check: hold RMB without moving, move with keys only, and compare with the pre-modernization build `build/Debug/ESTree.exe` (needs the NVIDIA GPU set in Windows graphics settings).

## Follow-ups consolidated by T13 (from the T00-T12 reports, not done)
- CI: the workflow has never run on GitHub. After the first run check the Windows engine job (newest Visual Studio generator on `windows-latest`, runner time with all of Bullet) and the Linux engine-ON job; paste failing logs into a task. Cut CI time by building only `BulletDynamics BulletCollision LinearMath` (351 ninja steps build every Bullet library) (T00, T11).
- Warnings: ~100 unique `/W4` warnings remain (C4244/C4267 conversions, C4239, C4458 shadowing, C4100); a cleanup task could reach zero and add `/WX` for project code (T00).
- Parser unit test for `ESEngine/ESTree/CommandLine.h` in `tests/app/` (T09). `Scene` teardown order is covered only by exe runs; a headless test would need the GL-free part of `Scene` split out (T10).
- `treegen`: `toString` is the slow stage of `generateTree` (`std::to_string` per parameter); drop the six-decimal numeric legacy model (D-014) once digit-identical production strings are no longer needed, regenerating the goldens with a DECISIONS entry (T04).
- OBJ export: weld vertices or add a binary/glTF format (352 MB for ternary E depth 10) (T08).
- Dependencies: GLFW 3.5.1 exists (pinned 3.4); `Vendor/stb` and `Vendor/rapidjson` follow master (no release tags); glad (branch `c`, 2020) was not regenerated (T12).
- Build/tooling: `vs2026` preset does not configure when only the VS 2026 Build Tools are installed (CMake cannot see the instance); `ninja-msvc-asan` needs `VCVARS64` of the VS 2019 Build Tools (D-007); `scripts\msvc-env.cmd` prints a cosmetic `vswhere.exe is not recognized`; worktrees under deep paths hit "Filename too long" for Bullet/doctest, use a short build path (T00, T01, T10, T12).
- Housekeeping: the 2017-2020 `build/` directory (VS14 cache, `build/Debug/ESTree.exe` reference binary) may be deleted once nobody needs the reference exe (T00).
- `ModelExporter` output (.obj/.mtl) was never compared byte for byte with a pre-T09 build (T09, owner checklist).
