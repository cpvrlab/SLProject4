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
section contains. Version 4.3.017 therefore means the seventeen points of the
*Version 4.3* section. Adding an eighteenth point means bumping the version to
4.3.018 and syncing every site in the table below at the same time.

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

## **Version 4.3.020**

Documentation, packaging and CI clean-up following a full review of the
repository, plus a path tracer correctness pass. Points 1–7 come from that
review; point 8 was found while investigating the two red build badges; points
9–17 are unrelated to it and come from investigating the fireflies that the
path tracer leaves in the Muttenzer Box. Point 16 was found while
reading the Timing panel during point 13, and point 17 follows from it: having
fixed what the panel says about speed, it should say something about noise too.
Point 18 belongs to none of these: it is the OptiX build, which point 6 deferred
for want of a Windows machine with an NVidia card. Point 19 closes what point 14
left open — the demo had no material with a soft specular lobe anywhere, so the
code that point fixed was never executed by the application — and adding one immediately found a
defect that the quadrature there could not. Point 20 is CI upkeep: GitHub
retired the artifact action the workflows uploaded with. All twenty points are
closed.

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
and the sampled point. `lightEmisRGB` in `AppDemoScenePTMuttenzerBox.cpp` was
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

### ✅ 14. Fix the glossy material path
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

**Implemented.** All four, plus the two places where the same defect appears
symmetrically and the list above had only named one half of it.

**The weight.** `SLPathtracer::phongLobeWeight` now returns
`(n+2)/(n+1) * |cos(theta)|`, and the caller multiplies it by the material
colour. The absolute value is needed because `SLMesh::preShade` does not flip
the hit normal towards the ray, so a back face hit — every ray leaving the
inside of the glass — has a negative cosine for the same geometry. Verified
against a quadrature of the same integral, uniform incident radiance and unit
albedo, `phi` being the angle between the surface normal and the lobe axis:

| n | phi | quadrature | old weight | new weight |
|---|---|---|---|---|
| 5 | 0° | 1.000000 | 1.17x | 1.00007 |
| 5 | 45° | 0.710393 | 1.64x | 0.99988 |
| 5 | 80° | 0.269291 | 4.33x | 1.00001 |
| 5 | 89° | 0.178621 | **6.53x** | 0.99999 |
| 20 | 45° | 0.707110 | 1.48x | 0.99995 |
| 20 | 89° | 0.097093 | **10.79x** | 0.99940 |
| 100 | 45° | 0.707106 | 1.43x | 1.00005 |
| 100 | 80° | 0.175233 | 5.76x | 1.00026 |
| 100 | 89° | 0.049125 | **20.56x** | 0.99978 |

The old weight is the same constant everywhere, because it had no geometry in
it at all: it is `(n+2)/(n+1)` whatever the surface is doing. The error is
therefore not a subtle bias but a shape error — a glossy surface got no
falloff towards grazing, which is precisely where a highlight is supposed to
die out, and the sharper the lobe the worse it was.

**The horizon.** `reflectMC`'s return value is now honoured, and `refractMC`
returns the same thing rather than `void`. The test is a comparison of *signs*
against the perfect direction, not the old "points along the normal": that is
what makes it work for a back face hit and for total internal reflection, where
`SLRay::refract` hands back a direction on the incident side. How much was
being traced and fully weighted despite carrying no energy:

| n | phi | lobe samples below the horizon |
|---|---|---|
| 5 | 45° | 2.5% |
| 5 | 80° | 34.1% |
| 5 | 89° | 48.3% |
| 20 | 80° | 21.4% |
| 20 | 89° | 46.8% |
| 100 | 89° | 43.1% |

Near half the rays at a grazing angle. They cost a full trace each and their
radiance was added at full weight, so this was both the waste and part of the
overbrightness above.

**The rotation matrix.** Replaced everywhere by `SLRay::lobeToWorld`, which
returns an orthonormal basis with the lobe axis as its third column, built with
the branchless construction of Duff et al., *Building an Orthonormal Basis,
Revisited* (JCGT 6(1), 2017). No cross product, no `acos`, no tolerance to tune.
`diffuseMC` had the identical construction and gets it too.

Two corrections to what this point claimed before it was implemented. **The ray
does not leave in an arbitrary direction.** At exactly ±z the degenerate matrix
is `diag(cos a, cos a, cos a)`, i.e. the identity at `+z` and `-I` at `-z`, and
`-I` maps the `+z` lobe correctly onto `-z` for a distribution that is
azimuthally symmetric — so both poles happen to work. What actually fails is
the approach to them, and in `float`: for an axis 1e-4 off `+z`, `dir.z` rounds
to exactly 1, `acos` returns 0 and the lobe is snapped back onto the pole, an
axis error equal to the full 1e-4 offset. The new construction is exact there.
Worst error over 200'000 uniform random axes, old vs new: axis 6.2e-7 vs
1.5e-7, orthonormality 8.3e-7 vs 2.4e-7.

**The NaN, on the other hand, is real and worse than described.** `reflect` and
`refract` do not renormalise their result, so `dir.z` can round above 1;
`acos(1.0000001f)` is a NaN, the whole rotation matrix becomes NaN and the
scattered direction comes out `(nan, 0.2, nan)`. That poisons the pixel, and no
number of samples averages a NaN away. Confirmed by direct construction.

**The perfect mirror.** The `(shininess + 2) / (shininess + 1)` on the
transparent branch's Fresnel reflection is gone, and so is the one on the
reflective branch, which had the same defect for the same reason: the factor
normalises a lobe estimator and there is no lobe when the direction was not
drawn from one. This is the one part of the point that is *not* dormant in the
Muttenzer Box. Exactly, per interaction with a sphere:

| path | factor removed |
|---|---|
| glass sphere, Fresnel reflection (shininess 100) | 102/101 = **1.00990** |
| glass sphere, transmission (translucency 1000) | 1002/1001 = 1.00100 |
| mirror sphere, reflection (shininess 1000) | 1002/1001 = 1.00100 |

so the two spheres were gaining a few tenths of a percent to about a percent of
energy per bounce off them. Sub-percent on the image and well under the noise
at any sample count this is run at, but it was energy created from nothing and
it compounded along a specular chain.

`modules/sl/source/ray/SLRayMC.{h,cpp}` are deleted. Nothing in the repository
referenced them and no `CMakeLists.txt` built them, so nothing can regress; the
`diffuseMC` comment that point 13 corrected in `SLRay.cpp` survived in this
copy only because the copy was invisible to every build. The comment in
`SLRay.cpp` is now rewritten to say what cosine distributed scattering actually
is and to name Russian roulette as the *unrelated* technique it was confused
with.

**Left open.** The lobe sampling itself is still only verified against
quadrature and not in an image: no scene in the demo has a material with a
shininess or translucency below `PERFECT`, so nothing in the application calls
`reflectMC` or `refractMC` at all, and there is no headless render harness to
measure one with. A glossy scene would be worth adding, and is the natural
place to check that the falloff now looks right rather than only integrating
right. *Added in point 19, which found a defect the quadrature could not see.* Also, the Phong lobe is still sampled around the *mirror* direction and
clipped at the horizon rather than being a proper microfacet model, so up to
half the samples at a grazing angle are still discarded — correct, but wasteful.
And as always, none of this touches `modules/sl/source/optix/`; see point 18.

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

### ✅ 17. Report a noise figure in the Timing panel
The panel measures only speed. Every variance reduction point above — the area
light estimator of 11, the MIS of 12, the Russian roulette of 13, the clamp of
15 — was judged by looking at the image, which is why point 13 could appear to
be a 30% slowdown for most of its development. A number for the noise makes
those judgements repeatable, and it can be had without a reference image and
without tracing a single extra ray.

**The estimator.** `_radianceSum` already holds, per pixel, the sum of the
linear radiance of every sample taken. Adding a second buffer with the sum of
the *squares* gives the variance for one multiply-add per sample. With `n` the
sample count (`_aaSamples`, the same for every pixel, as the path tracer does
no adaptive sampling) and `x_i` the luminance of sample `i`:

    S1    = sum(x_i)                    <- _radianceSum, already there
    S2    = sum(x_i^2)                  <- the new buffer

    mu    = S1 / n                      the pixel value, as displayed
    s^2   = (S2 - S1^2 / n) / (n - 1)   variance of a single sample
    var   = s^2 / n                     variance of the pixel estimate mu
    SE    = sqrt(s^2 / n)               standard error of the pixel

`SE` is how far the pixel is expected to sit from its converged value, in the
same units as the pixel. Dividing by the pixel itself makes it dimensionless
and therefore comparable between scenes, exposures and resolutions:

    RSE_p    = sqrt(s_p^2 / n) / (mu_p + eps)      eps = 1e-3
    noiseRSE = (1/N) * sum over all N pixels of RSE_p

The `eps` keeps black pixels, where `mu` is 0 and no amount of sampling will
change it, from dividing by zero.

**The self check.** A Monte Carlo estimator converges with `1/sqrt(n)`, so
`noiseRSE` must fall by half when the samples per pixel are quadrupled. If it
does not, the estimator is wrong, not the scene — which makes this figure a
regression test on points 11 to 13 as much as a readout.

**The figure to actually compare, which is not the noise.** Noise alone ranks
techniques wrongly as soon as they change what a sample costs, and Russian
roulette does exactly that: point 13 traces 17% more rays per sample on
purpose. The measure that accounts for it is the Monte Carlo efficiency, the
inverse of variance times time:

    efficiency = 1 / (noiseRSE^2 * renderSec)

This is the right figure because it does not depend on the sample count. The
variance of the mean falls as `1/n` and the render time grows as `n`, so their
product is constant in `n` and two runs at different samples per pixel can be
compared directly. A change is worth keeping if it raises this number, whatever
it did to the noise or to the speed taken on their own.

**Fireflies need a percentile, not a mean.** A firefly is a few pixels out of
230400, so it barely moves `noiseRSE`. The figure that moves is the 99.9th
percentile of `RSE_p`, which at 640x360 is the worst 230 pixels or so. That is
the number that should have been watched while point 15 was tuned.

The percentile has a floor that follows from its own definition, and it was
measured on synthetic fireflies at 640x360 and 1000 spp, the outliers 3000
times an ordinary sample:

    firefly pixels   noiseRSE   noiseRSE999
            0.010%    0.00320       0.00338
            0.100%    0.00370       0.00342   <- still an ordinary pixel
            0.200%    0.00421       0.60547
            1.000%    0.00848       0.75006
            5.000%    0.02910       0.75061

Reporting the pixel at rank 99.9% means it cannot see a firefly population
smaller than 0.1% of the image: below that the rank lands on an ordinary pixel
and the figure reads clean. That is the right threshold here, since the
fireflies of the Muttenzer Box cover about 4% of the far wall by the table in
point 15, but a scene with rarer ones would need a higher percentile.

**The limit of all of the above: variance is not error.** `noiseRSE` measures
how far the image is from where *this* estimator converges, not how far it is
from the truth, and the clamp of point 15 is deliberately biased. Variance
falls monotonically as the clamp tightens, all the way to a perfectly
noise-free black image at a clamp of 0, so `noiseRSE` will always claim that
more clamping is better and will never once report the caustic energy that the
clamp destroys. Ranking clamp settings needs an error against a reference
instead — the Muttenzer Box at clamp `Off` at a very high sample count, stored
once:

    relMSE = (1/N) * sum over p of (I_p - ref_p)^2 / (ref_p^2 + eps)   eps = 0.01

The `ref^2` in the denominator is what stops the bright ceiling light from
dominating the sum, which is the reason plain MSE and PSNR are poor measures
for a high dynamic range render. `relMSE` counts the noise the clamp removes
and the dimming it causes in one number, so it is the only one of these that
can say the clamp went too far.

**Notes for the implementation.**
- The samples must be taken from the raw linear radiance, next to the existing
  `radianceSum += color` in `SLPathtracer::renderSlices`. Measuring `_images[0]`
  instead would be meaningless: that buffer is clamped to [0,1] and gamma
  corrected for the display, so every firefly in it is flattened to white and
  the signal being measured is gone.
- When the clamp of point 15 is on, the samples reaching the sum are already
  clamped, so the figure describes the noise of the clamped estimator. That is
  the correct behaviour: it is the image the renderer actually produces.
- The two sums are `double` even though `_radianceSum` is `float`, because the
  variance is the small difference of two large sums, `S2 - S1*S1/N`. In a quiet
  region those agree to several digits and a float would leave almost none of
  the result; a firefly attacks the same sum from the other end, since 1e4
  squared is 1e8 and an ordinary 1e-4 added to that is lost. Double carries both
  ends and costs 16 bytes per pixel, 3.7 MB at 640x480. Welford's online update
  was the alternative and would also have worked.
- Cost is one buffer the size of the image and one multiply-add per sample. No
  extra rays, and nothing in the estimator changes, so the rendered image is
  bit-identical with the figure switched on.

### ✅ 18. Fixes for the Windows/OptiX build
The OptiX renderers had not built for years. `SL_BUILD_WITH_OPTIX` is `OFF` by
default, so nobody met the breakage; turning it on died on the first `.cu` file.
Point 6 deferred the OptiX question for want of a Windows machine with an NVidia
card, and this point is what that machine answered.

**Nothing was wrong with the OptiX code.** Every blocker was CUDA toolkit drift
or a line somebody had commented out. The 2019 vintage OptiX 7.0 host code
compiled against the current headers untouched, and the one risk that could not
be settled by reading — whether a 2025 driver still serves an OptiX 7.0 ABI —
turned out fine.

Verified on an RTX 4090 (compute capability 8.9), driver 591.86, CUDA 13.0.88,
CMake 4.0.3, clang-cl 20.1.2 and MSVC 14.44.35207. The vendored headers in
`externals/lib-optix/include` are OptiX 7.0.0, ABI 22. A standalone probe
confirmed the driver still serves it:

    OPTIX_VERSION = 70000 (ABI 22)     cuInit              = 0
    CUDA_VERSION  = 13000              cuCtxCreate         = 0
    devices       = 1 (RTX 4090)       optixInit           = 0  (Success)
                                       deviceContextCreate = 0  (Success)

That test is worth keeping in mind rather than assuming: OptiX ships no import
library at all. The implementation lives in the display driver and
`optix_stubs.h` pulls the entry points out of it with `GetProcAddress`, so the
ABI is a promise NVidia keeps, not something this repository controls. If a
future driver drops ABI 22, the remedy is newer vendored headers.

#### The four build blockers

**`-arch sm_60` was hard coded.** CUDA 13 answers `nvcc fatal : Unsupported gpu
architecture 'sm_60'` and accepts `compute_75` and up only, so this alone
stopped every build on an up to date toolkit. It is now the cache variable
`SL_OPTIX_CUDA_ARCH`, editable from the CMake GUI.

Its default is `sm_75`, the *lowest* architecture the toolkit still supports and
not the newest, which is the opposite of the obvious choice and the reason to
record it here. These kernels are compiled to PTX, and the display driver
compiles that PTX for the card it finds when the program runs. PTX is forward
compatible, so `sm_75` runs on Turing, Ampere, Ada, Blackwell and whatever
follows, while an `sm_89` default would fail on any card older than an RTX 40.
One default therefore serves every student machine, and there is no speed in it
either way, since the driver optimises for the real card regardless.

**`cuCtxCreate` grew a fourth argument.** CUDA 13 points the macro at
`cuCtxCreate_v4`, which takes a `CUctxCreateParams*` this code does not use.
`SLOptix.cpp` passes `nullptr` for it under a `CUDA_VERSION >= 13000` guard, so
the declared minimum of CUDA 10 still compiles.

**The CUDA driver API was not linked.** `modules/sl/CMakeLists.txt` had
`#${CUDA_CUDA_LIBRARY}` commented out, which is `cuda.lib`. `CUDA_LIBRARIES`
next to it is the *runtime*, `cudart`; the `cu*` functions that `SLOptix.cpp`
and `SLOptixRaytracer.cpp` are written against live in the driver API, and
`optixDeviceContextCreate` takes a `CUcontext` that only the driver API can
produce. The result was eighteen undefined symbols at link time.

It is restored and moved inside a guard repeating the condition of the
`find_package` block at the top of the file, so that no CUDA is linked unless
OptiX was asked for. It sat outside any guard before, and worked only because
the variables are undefined when OptiX is off and CMake expands an undefined
variable to nothing. `${optix_LIBRARY}` was dropped from the same list: nothing
in the project has ever set it, and OptiX has no library to link in any case.

**nvcc rejected the host compiler.** `CUDA_HOST_COMPILER` defaults to
`CMAKE_C_COMPILER`, so a clang-cl configuration hands nvcc clang-cl, and nvcc
stops with `nvcc fatal : Host compiler targets unsupported OS` — a message that
names neither the flag nor the compiler at fault. nvcc wants MSVC's `cl.exe`,
which is not on the `PATH` outside a developer prompt, so `find_program` cannot
see it either. The OptiX block now locates the latest Visual Studio with
`vswhere` and sets `CUDA_HOST_COMPILER` to its `Hostx64/x64/cl.exe`, with
`SL_OPTIX_CUDA_HOST_COMPILER` as the manual override. Configurations that
already use MSVC skip the whole thing, since FindCUDA's default is then correct.

#### What a student needs

The distinction that matters for teaching: **the OptiX SDK is never needed**, on
any machine. The headers are in the repository and the implementation is in the
driver. The old configure message told students to download it, and that advice
is now gone.

| | Needed for | Why |
|---|---|---|
| NVidia driver | running | provides `nvcuda.dll` and OptiX itself |
| CUDA Toolkit | **building only** | `nvcc` for the PTX, `cuda.lib`, and it sets `CUDA_PATH` |
| MSVC C++ tools | **building only** | nvcc's host compiler |
| OptiX SDK | never | vendored in `externals/lib-optix` |

A *built* binary needs only the driver: `CUDA_USE_STATIC_CUDA_RUNTIME` is on, so
`cudart` is linked statically and `nvcuda.dll` is the only CUDA DLL referenced.
It is relocatable as long as the PTX travels with it, since `SLOptixHelper.cpp`
looks for the files at `<exe>/../modules/sl/` — a zip of `Release/` plus
`modules/sl/*.ptx` is self contained.

A student who enables the flag without the toolkit gets a `CMake Warning` naming
`CUDA_PATH` and saying the menu entries will stay disabled, and the
configuration continues so the rest of the project still builds. Without MSVC
the configuration stops with a `CMake Error` naming `SL_OPTIX_CUDA_HOST_COMPILER`
and the alternatives. Both messages were verified by provoking them.

#### Two defects in the renderers themselves

**The OptiX path tracer rendered one frame and froze.** `render()` ended with
`_state = rtFinished` and nothing ever set it back: `SLSceneView` returns a
renderer to `rtReady` on mouse up and mouse wheel, but only the CPU `_raytracer`
and `_pathtracer`, never the OptiX pair. It is `rtReady` now, so the image
follows the camera.

Note what this exposes about its sibling. The OptiX *ray* tracer appears to
update live by design, and does not: `_doDistributed` defaults to true,
`renderDistrib()` never assigns `_state` at all, and so it is left at the
`rtReady` of the constructor and re-renders every frame by accident.
`renderClassic()` next to it *does* set `rtBusy` and never clears it, so
switching distributed off would freeze the ray tracer exactly as the path tracer
was frozen. That is left open below.

**The path tracer's gamma was hard coded and wrong.** The kernel applied
`pow(c, 0.5)`, a gamma of 2.0, while the CPU `SLPathtracer` corrects with its
settable `gamma()` property whose default is 2.2. `ortParams` had no gamma field
at all, so the value could not reach the GPU and the Gamma slider could not
affect the image. The field exists now, `SLOptixPathtracer` sets `gamma(2.2)` in
its constructor as the CPU one does, and `updateScene` uploads it every frame.
The difference it was costing, in display levels:

| linear | `pow(c,0.5)` | `pow(c,1/2.2)` | levels lost |
|---|---|---|---|
| 0.50 | 180.3 | 186.1 | 5.8 |
| 0.25 | 127.5 | 135.8 | 8.3 |
| 0.10 | 80.6 | 89.5 | 8.9 |
| 0.05 | 57.0 | 65.3 | 8.3 |

Real, but small: this is not why the OptiX path tracer looks dark. See the
`max_depth` item below, which is worth roughly a quarter of the light.

#### An OptiX failure no longer takes the application down

`OPTIX_CHECK` and `CUDA_CHECK` throw `SLOptixException`, and **nothing in the
project caught it**. `SLSceneView::init` called `setupOptix` on both renderers
with no `try`, so any OptiX failure was an unhandled exception that killed the
process before the first window appeared.

This is not hypothetical, and it is the case a configure time check can never
cover: whether OptiX works is decided by the driver and the card, not by the
build. A student with a GTX 10 series card gets a build that cannot run, because
CUDA 13 cannot generate anything below `compute_75` while OptiX itself would
still support the card. A student with an old driver fails at `optixInit`.

There is now `SLOptix::available`, false until a device context exists.
`createStreamAndContext` delegates to `createStreamAndContextOrThrow` and
catches, so every caller is covered rather than the single call site in
`AppCommon.cpp`; `SLSceneView::init` catches around `setupOptix`; and both
`startOptix*` functions check the flag and catch around `setupScene`. The two
menu entries take the flag as ImGui's `enabled` argument and grey out exactly as
they do in a build without OptiX, with the reason in the log. `_renderType` is
now assigned only after `setupScene` succeeds, so a failure leaves the OpenGL
view standing instead of selecting a renderer that cannot draw.

Not verified at runtime: there is no old card or old driver here to provoke it
with. Renaming one of the generated `.ptx` files exercises the identical catch,
since `getPtxStringFromFile` throws on a missing file.

#### Deliberately not fixed

The OptiX renderers are a separate implementation from the CPU ones and have
received none of points 9 to 15. Bringing them up to date is real work and it is
not started here; these are recorded so the next reader does not rediscover
them.

- **The OptiX path tracer still cuts hard at `max_depth`**, and every call site
  passes 5 — the same literal and the same defect that point 13 removed from the
  CPU path tracer, where the hard cut returned 76.3% of the answer and its
  removal brightened the ceiling by 1.32x. Roughly a quarter of the
  interreflected light is missing, and it is bias, so no sample count fixes it.
  This, and not the gamma above, is why the OptiX path tracer looks dark.
- **The OptiX ray tracer has no Fresnel term.** There is no `fresnel` or
  `schlick` anywhere on the OptiX side, while the CPU ray tracer has
  `_doFresnel = true` by default. The reflection ray is weighted by a flat `kr`
  at every angle, and the Muttenzer glass sphere has `kr = 0.05`. For `kn = 1.5`
  Schlick gives R0 = 0.04 at normal incidence, so head on the flat value is very
  nearly right, but reflectance climbs to roughly 40% at 80 degrees and 90% at
  89. The bright rim and the mirrored highlight that make a glass ball read as
  glass are simply absent. About ten lines in `SLOptixRaytracerShading.cu`.
- **`OPTIX_CHECK` is commented out** around the path tracer's `optixLaunch`,
  under a `Todo: Bugfix needed for Optix needs some work for newer shader
  models`. A failing launch there is silent and leaves a stale buffer. The ray
  tracer's check is intact.
- **`SLOptixRaytracer::renderClassic` sets `rtBusy` and never clears it**, so
  the OptiX ray tracer would freeze after one frame if `doDistributed` were ever
  switched off. Dormant only because the default is true.
- **The OptiX path tracer menu has no Gamma slider**, so the 2.2 from the
  constructor is now correct but not adjustable. Three lines of ImGui.
- **`find_package(CUDA)` is removed in CMake 4.** It still works under policy
  CMP0146 with a deprecation warning, but the whole PTX pipeline —
  `cuda_compile_ptx`, `cuda_include_directories` — comes from that module and
  will need rewriting to `enable_language(CUDA)`. `SLOptixHelper.cpp` hard codes
  the string `cuda_compile_ptx_1_generated_` when locating PTX at runtime, which
  is a FindCUDA implementation detail, so the two have to move together.
- **The OptiX build was not compared against the CPU renderers** on any
  measurement of the kind points 9 to 17 use. Only that it builds, runs, and
  renders.

### ✅ 19. A soft lobe scene, and rough dielectrics that are rough on both sides
Point 14 could only verify its lobe sampling against a quadrature of the same
integral, because no scene in the demo had a `shininess` or a `translucency`
below `SLMaterial::PERFECT` and therefore nothing in the application ever called
`reflectMC` or `refractMC`. This point adds the scene that exercises them, and
adding it exposed a defect within minutes that the quadrature could not have
found.

A word on the vocabulary, because the menu and this text look like they
contradict each other. *Gloss* is *Glanz*: a surface with much of it is a sharp
mirror, and the menu accordingly calls the original scene **Muttenzer Box
Glossy** and the new one **Muttenzer Box Soft**. English computer graphics uses
*glossy* for the opposite end, the imperfect lobe between mirror and diffuse,
which is the sense point 14's title carries. This point says **soft** for the
wide lobe throughout, since that word means the same thing in both languages.

**The scene.** `AppDemoScenePTMuttenzerBox2` (`SID_PTMuttenzerBox2`, under
*Renderer > Path Tracing*) is the Muttenzer Box with the same two spheres in the
same places, and exactly one thing changed:

| sphere | material | original | here |
|---|---|---|---|
| mirror | reflective, specular white | `shininess` 1000 (`PERFECT`) | `shininess` 100 |
| glass | refractive, `kt` 0.95, `kn` 1.5 | `translucency` 1000, `shininess` unused | `translucency` and `shininess` 100 |
| glass | transmissive color | white | red, `(1.0, 0.2, 0.2)` |

Keeping everything else — the geometry, the walls, the area light, the camera —
identical to the original is what makes the pair useful: the two scenes can be
rendered side by side and every difference in the image is the lobe. A first
version of the scene had two rows of three spheres sweeping the exponent over
1000/100/20, which is how the defect below was found; it is reduced to this
because the comparison against the original is worth more than the sweep. The
OpenGL preview and the classic ray tracer draw this scene exactly like the
original; only the path tracer separates them, which is the point of it.

The red transmissive color is there for the caustic. It is the factor the path
tracer applies to everything the sphere transmits, so the bright patch the glass
throws onto the floor comes out red, and the caustic is the one feature of this
scene that is neither a lobe nor a shadow — a light path that only exists
because the glass focuses it. Green and blue are 0.2 and not 0: a channel at
zero is opaque to that channel and the caustic would carry no shading in it at
all. Worth knowing when looking for it: the per sample clamp of point 15
defaults to 3.0, and a caustic sample is exactly the kind of rare high energy
sample it clips, so the caustic is dimmer than the estimator says it is until
*Renderer > Path Tracer > Firefly Clamp* is set to *Off (unbiased)*, at the
price of the fireflies coming back.

Two presentation fixes came with it: `SID_RTMuttenzerBox` is renamed
`SID_PTMuttenzerBox`, since the scene it names is a path tracing scene, and it
is dropped from the *Ray Tracing* menu, where it was listed a second time under
the wrong renderer group.

**The defect it exposed.** In the transmissive branch of `SLPathtracer::trace`,
Schlick's term picks either transmission or reflection. The transmission drew
its direction from the Phong lobe of the material's `translucency`; the
reflection called `SLRay::reflect` and traced *that mirror ray*, whatever the
`shininess` was. So on a transparent material the shininess was dead: it
controlled nothing, at the one place where a material's own reflection lobe
should apply. The first render of the scene, in its
three-per-row form, showed it plainly: at 1000 spp the three glass spheres
carried the identical razor sharp image of the light rectangle although their
interiors were frosted visibly apart.

Point 14's quadrature could not see this. It checks the weight of a sample that
was drawn from the lobe; it cannot check whether the branch draws one at all.
That is the general shape of the gap this point closes, and the reason the
scene was worth adding rather than another integral.

**Fixed.** The Fresnel reflection now goes through `reflectMC` with
`SLRay::lobeToWorld` and is weighted by `phongLobeWeight` whenever the shininess
is below `PERFECT`, with the horizon test honoured — exactly what the reflective
branch has done since point 14. A rough dielectric is rough on both sides of the
interface: the same microscopic slopes that spread the transmitted lobe spread
the reflected one. The two widths stay independently controllable, `shininess`
for the reflection and `translucency` for the transmission, so a polished shell
over a diffusing interior is still expressible by leaving the shininess at
`PERFECT`. The perfect case is untouched and still carries no lobe weight, for
the reason point 14 gives.

**The old scene.** The glass of `AppDemoScenePTMuttenzerBox` had a shininess of
100 — a value that did nothing as long as the branch reflected as a perfect
mirror, and that would now have softened the reflection of the reference
scene's clear glass behind our backs. It is raised to `PERFECT`, which is what that scene has always
depicted. Nothing else there reads it: the material's specular colour is black,
so the classic ray tracer's Blinn-Phong term is zero, and `RefractReflect.frag`
has no specular term at all.

**Verified in the image.** 1000 spp at 640x360, 159 s, firefly clamp 3: the
mirror sphere reflects the box blurred instead of sharply, the glass sphere is
frosted, and it lays a distinctly red caustic on the floor beside itself. The
sharp mirrored quad of the light rectangle that the defect above left on the
glass is gone. Visual only — there is still no headless render harness to put a
number on it, which is point 14's open item and not resolved here.

**Left open.**
- **This is not a microfacet model.** The lobe is still Phong around the mirror
  or refraction direction and clipped at the horizon, with no shadowing-masking
  term, and the reflected and transmitted lobes are drawn independently rather
  than from one shared microfacet normal, so at grazing angles they describe
  surfaces that disagree with each other. Energy is not conserved between the
  two the way a real rough dielectric conserves it.
- **MIS does not cover the soft lobes.** Point 12 weights light sampling
  against BSDF sampling on the diffuse branch only. A wide lobe that finds the
  area light by chance is still pure BSDF sampling, which is why the soft
  spheres are grainier than the perfect ones at equal sample count — clearly so
  at the exponent of 20 in the render above.
- **The Fresnel reflection is tinted by the transmissive color.**
  `SLPathtracer::trace` sets its `objectColor` once per branch, and on the
  transmissive branch that is `mat->transmissive()` — which then multiplies the
  reflected contribution as well as the transmitted one. For a dielectric this
  is wrong: the reflection at a glass surface is uncolored, and the color of
  glass comes from the absorption along the path *through* it. It was invisible
  while every transmissive material in the demo was white, and the red sphere of
  this scene is what makes it visible: in the render above the highlight on the
  red sphere is red, where a glass sphere under a white light should show a
  white one. The fix is to weight the reflected
  contribution with white, or with a separate specular color, rather than with
  the transmissive one — small, but it changes what every transparent material
  reflects, so it is a point of its own and not a footnote to this one.
- As always, none of this touches `modules/sl/source/optix/`; see point 18.

### ✅ 20. Move the CI workflows off the retired artifact action v3
Every run of *Deploy Emscripten* since 30 January 2025 has failed before it
could upload anything, with GitHub's own message rather than a build error:

> This request has been automatically failed because it uses a deprecated
> version of `actions/upload-artifact: v3`.

The artifact backend behind v3 was switched off on that date, so the failure is
not a warning that can be lived with — GitHub fails the run outright. The build
itself was fine; only the last step was.

`actions/upload-artifact` is bumped from `@v3` to `@v4` in all six workflows
that use it, not just the one that was noticed: `deploy-wasm-emscripten.yml`,
and the five `build-dep-*.yml` workflows that publish the prebuilt libraries,
which would have failed the same way the next time a dependency is rebuilt.
`actions/checkout` goes from `@v3` to `@v4` in the same pass — it runs on the
retired Node 16 and today only warns, but it is heading the same way.

Two v4 behaviour changes were checked against this repository: artifact names
must be unique within a run, and all thirteen upload steps have distinct names
(`prebuilt-g2o-windows`, `prebuilt-g2o-linux`, and so on); and v4 artifacts are
immutable, which matters only if a workflow uploaded twice into one name, and
none does. `deploy-pages.yml` already used `actions/upload-pages-artifact@v3`,
a different action that is current at v3, and is untouched.

**Deployment stays manual, deliberately.** The workflow builds the WebAssembly
app and leaves it as a downloadable artifact; it does not copy it to the pallas
server. The data folder with the textures and models is far too large to carry
through GitHub, so the server copy is made by hand — download `emscripten_build`
from the run on the Actions page, unzip it, and upload the files to
`public/www`. The comment at the foot of the workflow says so; this point does
not change that.

No behavioural change to any code in the repository: this is CI configuration
only.
