# SLProject4

A C++17 3D computer-graphics teaching framework: an OpenGL renderer, a CPU ray
tracer and path tracer, optional OptiX GPU renderers, computer vision and SLAM,
building for Windows, macOS, Linux, iOS, Android and Emscripten.

## Read this first

**`docs/ImplementationPlan.md` is where the work lives.** It tracks the agreed
work per release as numbered *points*, each recording what was wrong, what was
done, how it was verified, and what it left open. A point marked ✅ is finished.
Before starting anything, read the section for the current version — an open
point usually carries the context behind it, so it can be picked up without
re-deriving the reasoning.

Do not treat the plan as a queue to work through unprompted. Points are picked
deliberately.

## Versioning — easy to get wrong

The version string is `AppCommon::version` in `apps/source/AppCommon.cpp`, and
it is what the About dialog shows. It is **not** free-form: the major and minor
components name the section in the implementation plan, and **the patch
component is the number of points that section contains**. `4.3.018` means the
eighteen points of the *Version 4.3* section.

So adding a point to the plan means bumping the version, and the version lives
in five places that must move together:

| Site | Purpose |
|---|---|
| `apps/source/AppCommon.cpp` | **source of truth**, shown in the About dialog |
| `docs/Doxyfile` (`PROJECT_NUMBER`) | the generated documentation site |
| `apps/CMakeLists.txt` (`MACOSX_BUNDLE_*_VERSION*`) | iOS/macOS `Info.plist` |
| `apps/source/platforms/android/example_project/app/build.gradle` | Android |
| `.../app/src/main/AndroidManifest.xml` | Android |

The ASCII banner in `AppCommon.cpp` carries the major and minor version too and
must be regenerated on a minor bump; the generator URL is in the comment above
it.

## Building

`BUILD.md` at the repository root is the full account, including the per-platform
prerequisites and a troubleshooting section. In short:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --target app-demo
```

CMake 3.15 or newer. The first configure downloads prebuilt libraries and a
594 MB `data.zip` from `pallas.ti.bfh.ch`; `SL_DOWNLOAD_PREBUILTS=OFF` on a clean
tree produces a confusing missing-header failure, which `BUILD.md` explains.

Build directories are git-ignored by the `build*/` pattern.

The `SL_*` options and their defaults are listed and echoed at configure time in
the top-level `CMakeLists.txt`. Most are `ON`; `SL_BUILD_VULKAN_APPS` and
`SL_BUILD_WITH_OPTIX` are `OFF`.

## OptiX

`SL_BUILD_WITH_OPTIX` is `OFF` by default and most contributors never touch it.
When enabled it needs, **to build only**, the NVidia CUDA Toolkit (for `nvcc`,
`cuda.lib`, and the `CUDA_PATH` the CMake checks) and MSVC's `cl.exe`, which is
the only host compiler nvcc accepts on Windows. It does **not** need the OptiX
SDK — those headers are vendored in `externals/lib-optix`. A built binary needs
only the NVidia display driver.

`SL_OPTIX_CUDA_ARCH` defaults to `sm_75`, deliberately the *lowest* architecture
the toolkit supports rather than the newest: the kernels are compiled to PTX and
the driver compiles that PTX for the actual card at load time, and PTX is forward
compatible.

**The OptiX renderers are a separate implementation from the CPU ones** and have
not received the correctness work of plan points 9–15. Do not assume a fix in
`modules/sl/source/ray/` exists in `modules/sl/source/optix/` or the CUDA kernels
in `modules/sl/cuda/`. Point 18 lists what is knowingly missing.

## Layout

| Path | |
|---|---|
| `modules/sl` | the renderer: scene graph, GL, ray/path tracers, OptiX |
| `modules/math`, `modules/utils` | `SLMat*`/`SLVec*`, logging, IO |
| `modules/cv`, `modules/sens`, `modules/wai` | computer vision, sensors, SLAM |
| `apps/app_demo` | the main demo application and its scenes |
| `apps/exercises` | teaching exercises |
| `externals/`, `modules/*/externals` | third-party code — do not reformat |

Renderers: `modules/sl/source/ray/` holds `SLRaytracer` and `SLPathtracer` (CPU);
`modules/sl/source/optix/` plus the `.cu` kernels in `modules/sl/cuda/` hold the
OptiX ones.

## Code style

- `.clang-format` at the root is authoritative. `scripts/clang_format_all.sh`
  runs it over `modules/*/source` and `apps`, skipping externals. Format only
  what you touched — a blanket run over an unrelated file buries the change.
- `.clang-tidy` enables `bugprone-*` only (minus `easily-swappable-parameters`).
- Types are prefixed `SL` (`SLMat4f`, `SLVec3f`, `SLNode`), CV types `CV`.
- Every file opens with a Doxygen header giving `\file`, `\date`, `\authors`,
  `\copyright` and the `\remarks` pointing at the coding-style wiki page. Keep
  the form when adding files.
- GPL-3.0. `LICENSE` is at the root.

## Commits

Messages here carry a short imperative summary and then a body that explains
**why**, what was measured or verified, and what was deliberately left undone —
not a restatement of the diff. Where the change belongs to a plan point, the
summary says so: `... (plan point 17)`.

A change that is not a plan point should say so explicitly in the body, e.g.
*"No new point in the implementation plan and no version bump: this is a
presentation fix to the panel of point 16, not a point of its own."* That
sentence is what stops the version rule above from silently drifting.
