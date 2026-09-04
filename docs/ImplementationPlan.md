# **SLProject4 Implementation Plan**

This plan tracks the work agreed per release. Each version section lists its
points with the current state, so an open item can be picked up without
re-deriving the context behind it.

## Versioning

The version string lives in `apps/source/AppCommon.cpp` (`AppCommon::version`)
and is the single source of truth — it is what the **About** dialog displays.

**It is kept in accordance with this plan**: the major and minor components name
the release section below, and the patch component is the number of points that
section contains. Version 4.3.008 therefore means the eight points of the
*Version 4.3* section. Adding a ninth point means bumping the version to
4.3.009 and syncing every site in the table below at the same time.

| Site | Purpose |
|---|---|
| `apps/source/AppCommon.cpp` | **source of truth**, shown in the About dialog |
| `docs/Doxyfile` (`PROJECT_NUMBER`) | version on the generated documentation site |
| `apps/CMakeLists.txt` (`MACOSX_BUNDLE_*_VERSION*`) | iOS/macOS bundle `Info.plist` |
| `apps/.../android/example_project/app/build.gradle` | Android `versionName` |
| `apps/.../android/example_project/.../AndroidManifest.xml` | Android `versionName` |

The ASCII banner in `AppCommon.cpp` carries the major and minor version too and
has to be regenerated on a minor bump; the generator URL is in the comment
above it.

---

## **Version 4.3.008**

Documentation, packaging and CI clean-up following a full review of the
repository. Points 1–7 come from that review; point 8 was found while
investigating the two red build badges.

### 1. Add the missing LICENSE file — **done**
Every source-file header and the Doxygen mainpage assert GPL-3.0, but the
licence text itself was never in the repository, so a clone carried no
enforceable licence. Added the canonical GPL-3.0 text at the repository root,
copied from the vendored `externals/eigen/COPYING.GPL` (identical MD5 to
`externals/libigl/LICENSE.GPL`, so it is the unmodified FSF text).

### 2. Fix the Doxyfile input paths — **done**
`INPUT` listed `../apps/app_demo_webgpu`, which does not exist — the folder is
`apps/app_webgpu` — and `modules/sens/source` was missing entirely, so the
sensor module never appeared in the generated HTML. Both corrected; all 30
`INPUT` entries now resolve.

### 3. Correct the documentation URL — **done**
`docs/README.md` still linked to the retired `pallas.ti.bfh.ch` site while the
top-level `README.md` pointed at `cpvrlab.github.io/SLProject4`. Repointed, and
the steps for generating the documentation locally were written down: a bare
`doxygen` run is not enough, because `HTML_EXTRA_STYLESHEET` needs the
`doxygen-awesome-css` clone and the images have to be copied into `html/`
afterwards. That clone is now git-ignored.

### 4. Bring build instructions into the repository — **done**
Build instructions existed only in the GitHub wiki, so a downloaded ZIP or a
shallow clone contained no path from source to binary. Added `BUILD.md` at the
repository root and linked it from `README.md`.

It is written from the repository rather than copied from the wiki, because
duplicating eleven pages guarantees the two drift apart. Facts were verified
against the source, which caught several stale wiki claims that were therefore
not carried over:
- The macOS page states CMake ≥ 3.3; the top-level `CMakeLists.txt` requires
  **3.15**.
- Many wiki pages link to `cpvrlab/SLProject` (the pre-4 repository name).
- The Emscripten page links to the retired `pallas.ti.bfh.ch` docs host, which
  point 3 replaced with `cpvrlab.github.io/SLProject4`.
- The Emscripten page says to install the *latest* emsdk; CI pins **3.1.60**,
  which is what `BUILD.md` documents.

`BUILD.md` adds material the wiki does not have: the full `SL_*` option table
with defaults and the per-platform overrides applied in `CMakeLists.txt`, and a
troubleshooting section covering the confusing missing-header failure that
`-DSL_DOWNLOAD_PREBUILTS=OFF` produces on a clean tree.

Note for future reference: `pallas.ti.bfh.ch` is **not** retired as a host. It
still serves `data.zip` (594 MB) and the prebuilt libraries over an HTTP → HTTPS
redirect, and the default build depends on it. Only the documentation that used
to live there has moved to GitHub Pages.

### 5. Correct the text errors in `Introduction.md` — **done**
The colour legend was checked against the actual diagram rather than corrected
by eye: each group's fill was read out of `docs/images/SLProject_UML_min.svg`
by locating the class name and taking the enclosing box fill. The legend at the
top of the page proved correct in every case, so the prose was aligned to it.

| Group | Fill in the SVG | Legend term |
|---|---|---|
| App code (`AppCommon`) | `#f6d678` | brown |
| Central (`SLAssetManager`) | `#d8f2ff` | light blue |
| Renderers (`SLRaytracer`) | `#82abff` | dark blue |
| Materials (`SLMaterial`) | `#ffff91` | yellow |
| Scene graph (`SLNode`) | `#d1ffa3` | green |
| Mesh (`SLMesh`) | `#ffd8ff` | pink |
| Vertex arrays (`SLGLVertexArray`) | `#d6adff` | violet |
| Animation (`SLAnimation`) | `#ffd8d8` | red |
| CV (`CVCapture`) | `#ffd6ad` | peach |

Fixed:
- Removed the duplicated "The **red classes** build the animation framework"
  bullet from the legend.
- The CV section called them *orange* while the legend says *peach*; both
  describe `#ffd6ad`, so the section now follows the legend.
- The Application Code section called the app boxes *grey*, which collides with
  the grey used for the external libraries at the bottom. `#f6d678` is the
  brown of the legend, so that is what it now says.
- Refreshed the footer to September 2026 and the copyright to 2002-2026. The
  same stale footer on the mainpage `SLProject.md` (June 2024) was refreshed
  too, since it is the identical defect on the site's front page.
- Two typos in passing: "redered" → "rendered", "for the the scenegraph" → "for
  the scenegraph".

### 6. Document the SL headers that have no Doxygen at all — **done**
Coverage in `modules/sl/source` is now 104 of 104 headers.

The original list of nine was wrong. It came from a grep for `//!` and `/*!`
only, which missed the `/** @brief */` style also used in this codebase, and a
second bug — a `///[^/]` pattern that matched separator rules like `//////`
because `[^/]` also matches the newline. Three of the nine were already
documented and needed nothing: `mesh/SLSphere.h`, `mesh/SLCircle.h` and
`gl/SLGLOVRWorkaround.h`. The six that genuinely lacked any symbol
documentation were `SL.h`, `gl/SLGLFbo.h`, `input/SLAssimpIOSystem.h`,
`input/SLInputEventInterface.h`, `node/SLHorizonNode.h` and
`optix/SLOptixHelper.h`.

Each change was verified to be comment-only by stripping comments from the old
and new versions and diffing the result, and the `sl` library was rebuilt.

Doxygen configuration shaped what was worth writing: `SKIP_FUNCTION_MACROS` is
on, so function-like macros such as `SL_LOG` and `SL_GETBIT` never render, and
the `SL_OS_*` defines sit in conditional branches the documentation build does
not take. For `SL.h` the effort therefore went into the file-level block and
the typedefs, which do render.

Documentation defects fixed in passing:
- `input/SLAssimpIOSystem.h` declared `\file SLAssimpIOStream.h`, the wrong
  filename, which attaches its documentation to a file that does not exist.
- `node/SLHorizonNode.h` and `optix/SLOptixHelper.h` each carried `\authors`
  twice.

Code defects found while reading, documented in place but deliberately not
changed, since they are code rather than documentation:
- `SLGLFbo::attachment` is declared but never assigned or read anywhere, so it
  is dead and uninitialised. Marked `\deprecated`.
- `gl/SLGLFbo.h` is not self-contained: it uses `GL_NEAREST`, `GL_RGB16F` and
  friends while including only `<SL.h>`. It compiles only because
  `SLGLFbo.cpp` includes `SLGLState.h` first.
- `UNUSED_PARAMETER` in `SL.h` names its parameter `r` but casts `x`, so any
  use fails to compile. It has no callers.
- `SL_TOGBIT` expands to a bare `if`/`else` and mis-binds inside an unbraced
  `if`.

Open sub-item: `optix/SLOptixHelper.h` lives inside `#ifdef SL_HAS_OPTIX`,
which is not in the Doxyfile `PREDEFINED` list, so none of it reaches the
documentation site. Adding it there would publish the OptiX classes. To be
decided together with the OptiX build itself, which needs a Windows/NVIDIA
machine to verify.

### 7. Finish the ClarendonFilter rename — **done**
An earlier commit renamed the exercise source and its `CMakeLists.txt` from the
misspelled *Calderon* to the correct *Clarendon*, but left the enclosing folder,
the `add_subdirectory()` call and the second `imshow` window title behind.
All three corrected; `grep -r Calderon` over the repository now matches nothing
outside this plan, and the `cv02_ClarendonFilter` target configures and builds.

### 8. Repair the GitHub Actions macOS builds — **done**
Both macOS workflows targeted `runs-on: macos-13`, a runner image GitHub has
retired. The jobs never started: they queued for exactly 24 hours and were
auto-cancelled with zero steps executed, which GitHub renders as a red
*failing* badge. The same commits built fine on `ubuntu-22.04`, so no code was
ever at fault.
- Deleted `build-x86_64-macos.yml`. The free Intel image is gone, and
  cross-compiling x86-64 on an arm64 runner would mis-select prebuilts, because
  `cmake/DownloadPrebuilts.cmake` branches on `CMAKE_SYSTEM_PROCESSOR` to choose
  between the `mac64_*` and `macArm64_*` bundles. Intel macOS remains supported
  for local builds; only the CI job is dropped.
- `build-arm64-macos.yml` moved to `macos-15`, which also makes it a native
  arm64 build rather than a cross-compile from an Intel host.
- Removed the dangling macOS-x86 badge from `README.md`.

Follow-ups still open from this point:
- Three `workflow_dispatch`-only workflows still target the retired `macos-13`
  and will hang for 24 hours the next time they are run by hand:
  `build-dep-opencv.yml`, `build-dep-g2o.yml`, `build-dep-openssl.yml`. These
  build the Intel `mac64_*` prebuilts, so the choice is to repoint them at
  `macos-15-intel` or to stop shipping Intel prebuilts.
- Eleven of the twelve remaining workflows pin `actions/checkout@v3`, which runs
  on the deprecated Node 16; only `deploy-pages.yml` is on v4.
- Nothing has compiled on macOS since October 2025, so genuine breakage may be
  hiding behind the queue timeout. The first green `macos-15` run is the proof.
