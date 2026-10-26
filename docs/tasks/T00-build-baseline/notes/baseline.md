# T00 baseline (2026-10-25, branch `task/T00-build-baseline`)

## Toolchain

| Item | Value |
|---|---|
| CMake | 4.4.4 (`C:\Users\Blamad\tools\cmake-4.4.4`) |
| Ninja | 1.13.2 |
| `vs2019` preset | VS 2019 Build Tools, MSVC toolset 14.29.30133 (v142), Windows SDK 10.0.19041.0 |
| `ninja-msvc` preset | VS "18" Build Tools, `cl` 19.51.36246 x64 (via `scripts\msvc-env.cmd`) |
| `vs2026` preset | **does not configure here**: CMake reports "could not find any instance of Visual Studio" (Build Tools instance is not registered). Kept in presets for machines with a registered VS 2026. |
| Boost | 1.72, prebuilt `vc140` static x64 libs, `BOOST_LIB=D:\Dev\Boost`; works with v142 and 19.51 |
| Standard | C++17 (`CMAKE_CXX_STANDARD 17`, extensions off) |

## Build times (this machine, wall clock, `--parallel`)

| Build | Time | Result |
|---|---|---|
| `vs2019` Release, clean (Bullet, FreeType, GLFW, app) | 4m53s | OK |
| `vs2019` Debug, clean | ≈ 3m46s + 2m55s over two runs (first run stopped at the `<string>` include errors, second at the missing `bcrypt` link) + 8s final relink | OK after fixes |
| `ninja-msvc` Debug, clean, configure+build (351 steps) | 1m31s | OK |
| incremental relink of `ESTree` only | ≈ 8 s | OK |

Executable sizes (vs2019): Debug 6685 KB, Release 2191 KB.

## Warnings at `/W4` (`ESTree` target, vs2019 Debug, one full compile)

- Raw warning lines in the log: 658 (513 located under `ESEngine/`, the rest from `Vendor/glm` headers), the same header warning repeats per translation unit.
- **Unique project warnings (file|line|code): 143.** By code: C4244 ×40, C4239 ×29, C4267 ×19, C4458 ×19, C4100 ×14, C4456 ×11, C4305 ×8, C4189 ×2 (rest < 2).
- Unique warnings located in `Vendor/` headers: 8 (glm `C4201`, etc.).
- Baseline number to beat/hold in later tasks: **143 unique project warnings** (method: collect `file(line,col): warning Cxxxx` from the ESTree compile, drop `Vendor/` paths, dedupe on file+line+code).

## `ESTree.exe --help` (Release and Debug, exit 0)

```
Allowed options:
  -h [ --help ]              produce help message
  -W [ --width ] arg (=1366) set window width
  -H [ --height ] arg (=768) set window height
  -s [ --scene ] arg (=0)    load one of four scenes (0-3)
  -f [ --fullscreen ]        launch application in fullscreen mode
```

`ESTree.exe --width abc` → `error: the argument ('abc') for option '--width' is invalid`, exit 1.
(Help text still says scenes `0-3`; scenes `0-4` exist — fixed by T09.)

## Fixes needed to make the unchanged engine build

1. Forward-slash/lowercase includes (`<GLM\...>`, `<GLFW\...>`, `<glad\...>`, `<BulletCollision\...>`, `<boost\...>`): 17 files, include lines only.
2. `Engine/Core/Texture.h` used `std::string` without `#include <string>` (previously arrived transitively); one include added.
3. Link `bcrypt` on Windows (`boost::uuids` in `GameObject`).
4. Boost: explicit vc140 libs for `date_time thread chrono system atomic filesystem program_options` plus `BOOST_ALL_NO_LIB` (auto-link would look for `vc142`/`vc145` names).
5. `CMAKE_POLICY_VERSION_MINIMUM=3.5` in presets (CMake 4 rejects Bullet's `cmake_minimum_required(2.4.3)` otherwise).

## Not verified here

- The engine window/rendering (no GL context in the agent session): see `REPORT.md` → *Needs human verification*.
- `ninja-msvc` Release and `vs2026`.

## Update after T09 (2026-10-25): per-toolchain warning references

Warning counts depend on the compiler. Same counting method (unique `file|line|code`, `Vendor/` excluded):

| Toolchain | Code state | Unique project `/W4` warnings |
|---|---|---|
| `vs2019` (MSVC 14.29) | after T00 | 143 |
| `ninja-msvc` (cl 19.51) | after T01 + T09 (merged `modernization` @ `8b2d9fc`) | 191 |

Later tasks must not increase the number on the toolchain they use.
