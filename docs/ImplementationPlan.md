# **SLProject4 Implementation Plan**

This plan tracks the work agreed per release. Each version section lists its
points with the current state, so an open item can be picked up without
re-deriving the context behind it.

A point marked ✅ is finished. Work that is still open is either a point without
that marker or a follow-up listed under the point that uncovered it.

## Versioning

The version string lives in `apps/source/AppCommon.cpp` (`AppCommon::version`)
and is the single source of truth — it is what the **About** dialog displays.

**It is kept in accordance with this plan**: the major and minor components name
the release section below, and the patch component is the number of points that
section contains. Version 4.3.016 therefore means the sixteen points of the
*Version 4.3* section. Adding a seventeenth point means bumping the version to
4.3.017 and syncing every site in the table below at the same time.

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

## **Version 4.3.016**

Documentation, packaging and CI clean-up following a full review of the
repository, plus a path tracer correctness pass. Points 1–7 come from that
review; point 8 was found while investigating the two red build badges; points
9–16 are unrelated to it and come from investigating the fireflies that the
path tracer leaves in the Muttenzer Box. Point 16 was found while
reading the Timing panel during point 13. Point 14 is still open.

### ✅ 1. Add the missing LICENSE file
Every source-file header and the Doxygen mainpage assert GPL-3.0, but the
licence text itself was never in the repository, so a clone carried no
enforceable licence. Added the canonical GPL-3.0 text at the repository root,
copied from the vendored `externals/eigen/COPYING.GPL` (identical MD5 to
`externals/libigl/LICENSE.GPL`, so it is the unmodified FSF text).

### ✅ 2. Fix the Doxyfile input paths
`INPUT` listed `../apps/app_demo_webgpu`, which does not exist — the folder is
`apps/app_webgpu` — and `modules/sens/source` was missing entirely, so the
sensor module never appeared in the generated HTML. Both corrected; all 30
`INPUT` entries now resolve.

### ✅ 3. Correct the documentation URL
`docs/README.md` still linked to the retired `pallas.ti.bfh.ch` site while the
top-level `README.md` pointed at `cpvrlab.github.io/SLProject4`. Repointed, and
the steps for generating the documentation locally were written down: a bare
`doxygen` run is not enough, because `HTML_EXTRA_STYLESHEET` needs the
`doxygen-awesome-css` clone and the images have to be copied into `html/`
afterwards. That clone is now git-ignored.

### ✅ 4. Bring build instructions into the repository
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

### ✅ 5. Correct the text errors in `Introduction.md`
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

### ✅ 6. Document the SL headers that have no Doxygen at all
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

### ✅ 7. Finish the ClarendonFilter rename
An earlier commit renamed the exercise source and its `CMakeLists.txt` from the
misspelled *Calderon* to the correct *Clarendon*, but left the enclosing folder,
the `add_subdirectory()` call and the second `imshow` window title behind.
All three corrected; `grep -r Calderon` over the repository now matches nothing
outside this plan, and the `cv02_ClarendonFilter` target configures and builds.

### ✅ 8. Repair the GitHub Actions macOS builds
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

### ✅ 9. Fix the path tracer's 8 bit accumulation buffer
`SLPathtracer` kept its progressive mean in `_images[1]`, a `CVImage` of format
`PF_rgb`, i.e. **8 bit per channel**. Every sample read that image back,
blended, clamped and wrote it again, so the running mean was rounded to 1/255
in linear space once per sample.

The update is `a_k = a_(k-1) + (x_k - a_(k-1)) / k`. As soon as `|x - a| / k`
falls below half a quantisation step — that is, as soon as `k > 510 * |x - a|`
— the write rounds back to the value already stored and the pixel **freezes**.
Simulating one bright sample (8.0) on a pixel whose true value is 0.235 shows
it exactly, in display levels:

| accumulator | k=10 | k=25 | k=50 | k=100 | k=200 | k=400 |
|---|---|---|---|---|---|---|
| float | 180 | 153 | 143 | 138 | 135 | **133** |
| 8 bit | 180 | 154 | 147 | 147 | 147 | **147** |

with 132 as the converged target. That is why the fireflies averaged out but
not enough: the renderer stopped converging at roughly sample 30 to 50 no
matter what *Samples/pix* was set to. Because the ratchet is one sided —
outliers freeze high and nothing freezes low — the image also grew brighter the
longer it rendered.

The mean is now accumulated unclamped in `vector<SLCol4f> _radianceSum` and
divided by the sample number for the display only. The clamp to [0,1] moved
from the running mean to the display copy; clamping the mean itself discarded
energy permanently and was the second source of the brightening.

Measured on the Muttenzer Box, noise taken as the median absolute residual
against a local 7x7 median so that the walls' brightness gradient does not
count as noise:

| region | 10 → 100 spp before | after | ideal |
|---|---|---|---|
| far wall | 1.92x | 2.33x | 3.16x |
| red wall | 1.66x | 2.01x | 3.16x |
| blue wall | 1.66x | 2.80x | 3.16x |
| floor | 2.10x | 2.51x | 3.16x |

and the sample-count brightening is gone: the far wall median went 129 → 135
from 10 to 100 spp before and 129 → 130 after; the blue wall 86 → 91 before and
86 → 87 after.

Two latent defects were found while replacing the buffer and are fixed with it:
- `_images[1]` was allocated at `_sv->viewportW/H()` while `_images[0]` is
  scaled by `_resolutionFactor`, so the accumulator had the wrong size whenever
  that factor was not 1. It went unnoticed because `CVImage::getPixeli` wraps
  the coordinates with a modulo.
- The 4px slice loop always ran `minX + 4` columns and relied on
  `setPixeliRGB` clamping `x` internally. That merely double-wrote the last
  column into an image, but indexes past the end of a raw buffer on any width
  that is not a multiple of 4. The loop is now cut off at the image width.

### ✅ 10. Make the shared random number generator thread safe
`rnd01()` in `modules/sl/source/ray/SLRay.cpp` returned values from a single
global `std::mt19937` bound with `std::bind` at namespace scope, under a
comment reading "So far they work perfectly with CPP11 multithreading". They do
not: `SLPathtracer::render` calls it concurrently from every worker thread, so
the engine's 624 word state and its position index are read and written without
synchronisation. That is a data race and therefore undefined behaviour, and in
practice the racing threads hand each other torn and repeated values, which
correlates samples the estimator assumes to be independent.

Each thread now has its own `thread_local` engine, seeded from the current time
mixed with a shared `std::atomic` counter so that threads created within the
same second still get different sequences.

This one cannot be isolated in the measurements of point 9 — per pixel variance
was never dominated by it — so it is fixed on its own terms rather than for a
measured gain. It affects `SLRaytracer` and `SLLightRect` sampling as well as
the path tracer.

### ✅ 11. Give the path tracer a real area light estimator
`SLPathtracer::shade` estimated the direct light of a rectangular light with
the Blinn-Phong rasteriser model rather than with an estimator of the area
light integral. It took the direction, the distance and both cosines at the
light **centre** while `SLLightRect::shadowTestMC` tested the visibility at a
**random point**, and it replaced the geometric term by the OpenGL attenuation
and the spot cone exponent. The light area was missing altogether.

An earlier reading of this code claimed the light cosine was missing too. It is
not. With `spotCutOffDEG(90)` and `spotExponent(1.0)` the spot term
`pow(max(-L · spotDirWS, 0), 1)` happens to equal cos(theta_light), because
`SLLightRect::spotDirWS` is the rectangle's normal. That is a coincidence of
this scene's settings and not a property of the estimator: `spotExponent(2)`
would silently turn it into a different emitter.

The consequence was not only a wrong absolute brightness. It put the direct
illumination on a different scale than the emissive material of the light mesh,
which paths see when they reach the light through the mirror or the glass
sphere, so the two estimates of the same illumination disagreed.

A rect light is now sampled uniformly over its surface (pdf = 1/area) and
estimated with

    Lo = albedo/PI * Le * cosSurface * cosLight / dist^2 * area

with every quantity taken at the sampled point. `SLLightRect::samplePointMC()`
and `area()` are new, and `shadowTestMC` uses the sampler instead of
duplicating it. Point and directional lights have no area and keep the classic
attenuation and spot cone model.

The formula was verified by deterministic quadrature against the two limits it
has to satisfy — a large overhead emitter must give `albedo * Le`, a small one
must give the point light result:

| case | quadrature | expected |
|---|---|---|
| 20 x 20 light at height 1 | 0.99189 | 0.99010 |
| 200 x 200 light at height 1 | 0.99992 | 0.99990 |
| 0.01 x 0.01 light at height 1 | 3.183e-05 | 3.183e-05 |

Measured on the Muttenzer Box at 100 spp, median luminance:

| region | before | after | ratio |
|---|---|---|---|
| far wall | 131 | 107 | 0.817 |
| red wall | 102 | 84 | 0.824 |
| blue wall | 87 | 72 | 0.828 |
| floor | 136 | 111 | 0.816 |

That is 0.639 in linear against 0.625 predicted from the missing area factor
(1/0.65 = 1.54); the small remainder is the difference between the light centre
and the sampled point. `lightEmisRGB` in `AppDemoSceneRTMuttenzerBox.cpp` was
raised from 7 to 10, which brings the apparent brightness back to within 3 to 4
percent of the old look. 11.0 would match it exactly, but the scene is not
calibrated against anything physical, so the value is a matter of taste; what
matters is that the emission now scales the light mesh and the direct
illumination together. Note that it also scales `SLLight::globalAmbient`, which
only the GL rasteriser uses — the path tracer ignores it.

Two further defects fixed in the same pass:
- `trace()` seeded `finalColor` with `ray->backgroundColor`, which added the
  background to every surface at every bounce and compounded down the path. It
  stayed invisible only because this scene has a black background and no
  skybox. The escape and the max depth cases are now separate, and an escaping
  ray returns the environment radiance instead of black.
- `shade()` declared `diffuseColor` outside the light loop and assigned it only
  when the light was visible, while the accumulation ran unconditionally. With
  two or more lights, a shadowed light re-added the previous light's
  contribution.

With points 9 to 11 in place, fireflies at 1000 spp sit at 0.00% of pixels in
all five measured regions, at both light settings.

The work this point leaves open is carried as points 12 to 15.

### ✅ 12. Multiple Importance Sampling (MIS) between light and BSDF sampling
`shade()` samples the light directly, and the cosine weighted scattering in
`trace()` reaches it by chance. Both are unbiased estimates of the same paths,
so the renderer has to do something about the overlap. It used to do it with
the boolean `em` argument of `trace()`: after a diffuse bounce a light hit
counted zero, after a specular bounce it counted in full. Correct, but crude —
it discards every light hit the scattering finds, however good a sample it was.

`em` is replaced by the solid angle density with which the previous vertex
generated the ray, or by the sentinel `PDF_NO_MIS` for the primary ray and for
specular and transmissive bounces, which next event estimation cannot generate
at all. Where both strategies can produce a path, each estimate is weighted
with the power heuristic (beta = 2) so that the path is counted exactly once.

Verified against a quadrature of the same integral, at the Muttenzer floor
centre:

| estimator | result |
|---|---|
| ground truth (quadrature) | 0.252686 |
| light sampling only | 0.252686 |
| BSDF sampling only | 0.254314 |
| both, unweighted | 0.505435 |
| both, MIS weighted | **0.252688** |

**What it does not do — contrary to what this point claimed before it was
implemented — is remove the fireflies.** Those come from the path
wall → diffuse scatter → mirror or glass sphere → light. The middle vertex is
specular, so there is no light sampling strategy there to weight against and
the MIS weight is necessarily 1: that path is untouched. Written from the light
side it is L-S-D-E, an ordinary caustic, and no amount of importance sampling
from the eye lets a unidirectional path tracer sample it well. The practical
lever against it is point 15; the principled one is a caustic capable method
such as photon mapping.

What MIS does buy is robustness where a surface is close to a large light, the
regime in which uniform area sampling has high variance and the scattering is
the better strategy. Standard deviation of the direct light estimate at a point
below the 1.0 x 0.65 light:

| distance to the light | light sampling only | MIS |
|---|---|---|
| 2.43 (the floor centre) | 0.007 | 0.007 |
| 1.00 | 0.174 | 0.181 |
| 0.40 | 2.242 | 1.772 |
| 0.15 | 11.280 | 3.062 |

Everything in the Muttenzer Box is a unit or more from the light, so it changes
nothing there, and two renders confirm that: at 100 spp the mean linear
radiance of the five measured regions moves by at most 0.16% with no consistent
sign, and the noise is unchanged (far wall MAD 1.70 either way). That the mean
does not move is the result that matters, since MIS is only correct if it is
unbiased.

It is kept because it costs nothing — the scatter ray is traced either way, and
samples that used to be discarded now contribute their weighted share — and
because any scene with a large or nearby area light needs it.

### ✅ 13. Replace the fixed maximum depth with Russian roulette
**Russian roulette is not the same thing as Monte Carlo**, and the two are easy
to conflate because both are random. Monte Carlo is the estimator itself: to
evaluate an integral that has no closed form, draw samples `x` from a density
`p` and average `f(x)/p(x)`. That is what the entire path tracer is. Russian
roulette is a technique used *inside* such an estimator, and it answers a
different question — how do you terminate an unbounded recursion without making
the answer wrong?

The rendering equation recurses without end: light bounces between the walls of
the box forever, each bounce carrying less energy than the last. No renderer
can trace an infinite path, so it has to stop somewhere. There are two ways.

**A hard cut-off**, which is what `SLPathtracer::trace` does today:

    if (ray->depth > maxDepth())
        return SLCol4f::BLACK;

Every path is killed after a fixed number of bounces and the light that would
have arrived along longer paths is discarded. This is **biased**: no matter how
many samples are averaged, the estimator converges to something darker than the
true solution, because every sample is missing the same energy. More samples
cannot fix it — this is the one kind of error that a longer render does not
reduce.

**Russian roulette**, which removes that bias. At each bounce, kill the path
with probability `q` and return zero; otherwise continue and divide the result
by `1 - q`. The expectation is unchanged:

    E = q * 0 + (1 - q) * L / (1 - q) = L

Paths still terminate in finite time, since the chance of surviving `n` bounces
falls off geometrically. But the estimator's *mean* is still the full infinite
sum: nothing is thrown away, and instead the rare surviving long path is scaled
up to stand in for all the long paths that were killed. The price is variance;
the gain is that no work is spent on paths that can no longer deliver energy.

The survival probability is normally taken from the path throughput, the
running product of the albedos along the path. Setting `1 - q` to roughly the
largest component of that throughput lets dark paths die quickly while bright
ones live on, so samples are spent in proportion to the energy still available.
Roulette is usually only switched on after the first few bounces, which are
cheap and always worth taking.

This matters more here than it might seem. `maxDepth` is fixed at **5** and is
not reachable from the UI at all: all thirteen `startPathtracing` call sites in
`SLSceneView.cpp` and `AppDemoGui.cpp` pass the literal 5. For a closed box the
interreflected energy is the geometric series `1 + rho + rho^2 + ...`, so
cutting it after `N` terms discards a fraction `rho^N`. With the cream walls at
albedo 0.75 that is `0.75^5`, roughly a quarter of the interreflection missing;
the coloured walls lose less. That is a closed-box estimate rather than a
measurement, but it is the right order of magnitude, and it is part of why the
box reads flatter than the same scene in Blender even now that the direct light
is correct.

Documentation defect to fix with this point: the comment above
`SLRay::diffuseMC` in `modules/sl/source/ray/SLRay.cpp` reads "This is only
used for photonmapping(russian roulette)". Both halves are wrong.
`SLPathtracer::trace` is its only caller in the repository, and cosine
distributed scattering is importance sampling, not Russian roulette — precisely
the conflation this point is about.

**Implemented.** The survival probability is the albedo, so the 1/survival of a
survivor cancels that bounce's attenuation exactly and the colour of a surface
decides how likely a path is to continue rather than how much it is dimmed.
Roulette applies to the continuation only; the next event estimate at each
vertex is terminal and always taken. `maxDepth` went from 5 to 32 at all
thirteen `startPathtracing` call sites and is now only a safety net, because a
surface with an albedo of 1 — the mirror's specular and the glass's
transmissive are both white — can never be terminated by roulette.

Verified on a model of the interreflection series (albedo 0.75, exact 4.0): the
old hard cut at 5 returned 3.0508, i.e. **76.3% of the answer**; roulette
returns 4.0007. Measured on the scene at 100 spp, the recovered light appears
exactly where the theory says it should, in proportion to how much of a
region's light is indirect:

| region | brightening | implied indirect fraction |
|---|---|---|
| ceiling (receives no direct light at all) | x1.324 | 100% |
| blue wall | x1.080 | 26% |
| far wall | x1.075 | 24% |
| red wall | x1.063 | 20% |
| floor | x1.050 | 16% |

Cost: 410M rays in 130.8s before, 479M in 151.9s after, i.e. **+17% rays and
+16% time**, matching the modelled path length of 6 -> 7. (The *Rays per ms*
figure in the Timing panel appeared to fall from 154 to 106, which is wrong:
the actual throughput is 3136 vs 3156 rays/ms. `_raysPerMS` is an averaging
accumulator that the path tracer writes once per render, so it mixes in the
interactive GL frames that trace no rays. Worth fixing separately.)

Two things were tried while tuning and reverted, recorded so they are not tried
again:

- **`RR_START_DEPTH` 3 -> 8.** A model of the roulette tail alone predicted a
  fourfold drop in standard deviation. On the real scene it made the ceiling
  *worse* (median absolute residual 3.77 -> 4.05 at 100 spp) for 1.7x the path
  length. The model measured the wrong thing: the noise there was never the
  roulette tail. Kept at 3.
- **Moving the light down from y = 1.18 to 1.10.** This did find the real cause
  of the ceiling noise. The light rectangle hangs 0.01 below the ceiling at
  `pT = 1.19`, so a ceiling point directly above it is sealed into a slot: only
  **0.07%** of its cosine weighted hemisphere escapes, and all of its indirect
  light arrives through that one rare direction carrying the full radiance of
  the room. At y = 1.10 the gap is 0.09 and 5.15% escapes, 77 times more often,
  and that patch went from the worst converging region in the image (ratio
  1.76x from 10 to 100 spp) to an ordinary one (2.17x, the same as the floor).
  But it also exposes the black back face of the one sided light and the 9cm of
  ceiling it shadows, which reads as a hole above the lamp. A 13% noise gain in
  one patch was not worth that, so the light stays at 1.18 and point 15 deals
  with the region instead.

### 14. Fix the glossy material path
Dormant in the Muttenzer Box, because `SLMaterial::PERFECT` is 1000 and both
spheres sit exactly at 1000 (`refl` shininess 1000, `refr` translucency 1000),
so `reflectMC` and `refractMC` are never called there. It affects any material
below that limit:
- The Phong estimator weight `(shininess + 2) / (shininess + 1)` is missing its
  `cos(theta)` factor. For lobe sampling with `pdf = (n+1)/(2*PI) * cos^n(alpha)`
  the correct weight is `rho * (n+2)/(n+1) * cos(theta)`, so glossy reflection
  is currently too bright by `1/cos(theta)` and blows up at grazing angles.
- `reflectMC` returns a bool reporting that the sampled direction ended up below
  the surface horizon. `SLPathtracer::trace` ignores it and traces the ray
  anyway.
- The rotation axis `(0,0,1) x dir` degenerates to a zero vector when the lobe
  axis is near ±z. `SLVec3::normalize` guards with `if (L > 0)`, so the result
  is an unnormalised zero vector rather than a NaN, but the rotation matrix
  built from it is meaningless and the ray leaves in an arbitrary direction.
  `acos(dir.z)` can also produce a NaN for `|dir.z| > 1` after rounding.
- The transparent branch multiplies a *perfect* mirror ray by
  `(shininess + 2) / (shininess + 1)`, a normalisation that belongs only with
  lobe sampling. For the glass sphere's shininess of 100 that is a 1% energy
  gain per bounce.

Noted while reading, unrelated to the above but in the same files:
`modules/sl/source/ray/SLRayMC.{h,cpp}` are a duplicate of `SLRay` that no
`CMakeLists.txt` builds and nothing references. They carry their own stale copy
of the `diffuseMC` comment from point 13. They should be deleted or explained.

### ✅ 15. Offer a per sample radiance clamp
A clamp on the radiance of a single sample, e.g. `color.clampMinMax(0, 10)`
directly after `trace()` in `renderSlices`, removes the residual fireflies at
once. It is what most production renderers ship, and it is biased by
construction: it darkens exactly the bright caustic paths it is aimed at.

**Implemented** as `SLPathtracer::_sampleClamp`, exposed in the PT menu as
*Firefly Clamp* with Off / 10 / 5 / 3. It caps the brightest channel and scales
the other two with it, so a clamped sample loses energy but keeps its colour;
clamping each channel on its own would shift the hue of everything it touches.
It caps the **sample**, never the running mean — clamping the mean is the
defect point 9 removed, and it froze fireflies instead of averaging them.

Measured at 100 spp. Fireflies, as the percentage of pixels more than 20
display levels above their local median:

| region | off | 30 | 10 | 3 |
|---|---|---|---|---|
| far wall | 4.35% | 4.75% | 4.08% | **0.02%** |
| floor | 2.10% | 1.92% | 1.82% | **0.01%** |
| ceiling away from the light | 7.76% | 6.31% | 6.70% | **0.24%** |
| ceiling below the light | 12.47% | 12.70% | 10.97% | **1.46%** |
| caustic under the glass | 9.58% | 9.13% | 8.99% | **4.59%** |
| mirror sphere | 3.78% | 2.70% | 2.60% | **0.06%** |

The fireflies of this scene sit **between 3 and 10**, not above it: moderate
and frequent rather than extreme and rare. 30 does nothing at all and 10 barely
helps, which is why the menu offers 10 / 5 / 3 and not the 30 first guessed.

What a limit of 3 costs, as mean linear radiance against no clamp: floor 0.977,
far wall 0.959, mirror sphere 0.955, ceiling away from the light 0.947, ceiling
below the light 0.849, **caustic under the glass 0.695**. The bias lands where
it should, on the feature that was carrying its energy in rare bright samples.
The caustic being a third too dark is the price of the clean image.

The bulk noise barely moves (far wall median absolute residual 2.23 -> 2.00):
this is purely a tail operation and does not make the estimator converge
faster. It costs no time (15.12s vs 15.65s at 100 spp).

The default is 3, because that is what makes the renderer usable at the sample
counts it is actually run at, and Off is one click away. Nothing else on this
list will make the clamp unnecessary short of a caustic capable method such as
photon mapping: point 12 was expected to reduce these fireflies and measurably
does not, because the middle vertex of the path is specular.

### ✅ 16. Fix the *Rays per ms* figure in the Timing panel
`SLRaytracer` initialised its throughput counter as `_raysPerMS.init(60, 0.0f)`
— a moving average over 60 slots, all starting at zero — but `set()` is called
exactly **once per completed render**, at all three call sites. `average()`
always divides by the full window, so after *k* renders the panel showed *k*/60
of the truth and only became correct after sixty of them:

| render | true rays/ms | displayed | implied k |
|---|---|---|---|
| before point 13, 1000 spp | 3136 | 154 | 2.9 |
| after point 13, 1000 spp | 3156 | 106 | 2.0 |
| point 15 at clamp 3, 100 spp | 3011 | 726 | 14.5 |

The *k* is simply how many times render had been pressed since the app started,
which defeats the figure's purpose entirely: it is meant to compare the multi
core throughput of different machines and architectures, and two people
benchmarking the same hardware would get different answers depending on how
often they had rendered.

It also produced a false conclusion during point 13: the panel appeared to show
throughput falling from 154 to 106 when Russian roulette went in, suggesting a
30% slowdown. The real throughput was 3136 against 3156, i.e. unchanged, and
the extra 16% of wall clock time was entirely the 17% of extra rays that
roulette traces on purpose.

Fixed by initialising the window to 1, which takes the "shortcut for no
averaging" branch of `Utils::Averaged::set` and reports the last completed
render exactly. The other users of `AvgFloat` — the frame, cull, shadow map and
draw timers of `SLSceneView` and `SLScene` — are set every frame and fill their
window within a second, so their averaging is appropriate and untouched.

Two properties of the figure are documented at `raysPerMS()` rather than
changed, because they are inherent to measuring a whole render:
- The window update that the main thread performs every 500ms falls inside the
  timed region. It costs about 0.1%, since the other worker threads keep
  tracing through the redraw.
- `SLPathtracer::render` joins every worker thread after each sample pass, so a
  1000 spp render crosses a thread barrier a thousand times, and the slice queue
  takes a mutex per 4 pixel column. The cost of both grows with core count, so
  the figure understates a many core machine. It describes this renderer on this
  machine rather than raw ray throughput; timing a single sample pass with the
  window update disabled would be the cleaner benchmark.
