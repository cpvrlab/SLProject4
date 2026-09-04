# **SLProject4 Implementation Plan**

This plan tracks the work agreed per release. Each version section lists its
points with the current state, so an open item can be picked up without
re-deriving the context behind it.

The version string lives in `apps/source/AppCommon.cpp` (`AppCommon::version`)
and is the single source of truth — it is what the **About** dialog displays.
Every other version site is synced to it by hand:

| Site | Purpose |
|---|---|
| `apps/source/AppCommon.cpp` | **source of truth**, shown in the About dialog |
| `docs/Doxyfile` (`PROJECT_NUMBER`) | version on the generated documentation site |
| `apps/CMakeLists.txt` (`MACOSX_BUNDLE_*_VERSION*`) | iOS/macOS bundle `Info.plist` |
| `apps/.../android/example_project/app/build.gradle` | Android `versionName` |
| `apps/.../android/example_project/.../AndroidManifest.xml` | Android `versionName` |

---

## **Version 4.3.000**

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

### 4. Bring build instructions into the repository — **open**
Build instructions exist only in the GitHub wiki, so a downloaded ZIP or a
shallow clone contains no path from source to binary.
- Add a `BUILD.md` at the repository root covering the prerequisites and the
  CMake invocation for each platform.
- Keep the wiki as the long-form reference and link to it, rather than
  duplicating it wholesale.
- Mention the `SL_DOWNLOAD_DATA` / `SL_DOWNLOAD_PREBUILTS` options explicitly:
  configuring with them `OFF` on a clean tree produces confusing
  missing-header errors rather than a clear message.

### 5. Correct the text errors in `Introduction.md` — **open**
- The bullet "The **red classes** build the animation framework" appears twice
  in the class-diagram legend.
- The CV classes are called *peach* in the legend but *orange* in their own
  section heading; pick one.
- The page footers still read "Juli 2024" / "June 2024"; refresh them, or drop
  the dates so they cannot go stale again.

### 6. Document the SL headers that have no Doxygen at all — **open**
Nine headers in `modules/sl/source` carry neither a `//!` brief nor a `/*! */`
block, so they appear in the generated docs as bare, unexplained symbols.
Coverage elsewhere is good — 1770 `//!` briefs across 189 files, and 73 of 104
headers carry a file or class brief — which makes these stand out:

- `SL.h`
- `mesh/SLSphere.h`
- `mesh/SLCircle.h`
- `input/SLAssimpIOSystem.h`
- `input/SLInputEventInterface.h`
- `optix/SLOptixHelper.h`
- `gl/SLGLFbo.h`
- `gl/SLGLOVRWorkaround.h`
- `node/SLHorizonNode.h`

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

### Version bookkeeping
The ASCII banner in `AppCommon.cpp` still reads *SLProject 4.2* and has to be
regenerated for 4.3 — it is figlet "Small Slant" art, reproducible at the URL in
the comment above it.
