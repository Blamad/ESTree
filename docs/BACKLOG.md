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
