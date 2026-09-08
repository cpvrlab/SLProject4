/**
 * \file      SLPathtracer.cpp
 * \date      July 2014
 * \authors   Thomas Schneiter, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include <algorithm>
#include <cmath>
#include <typeinfo>

#include <SLCamera.h>
#include <SLLightRect.h>
#include <SLPathtracer.h>
#include <SLSceneView.h>
#include <GlobalTimer.h>
#include <Profiler.h>

extern SLfloat rnd01();

//-----------------------------------------------------------------------------
//! Sentinel pdf for SLPathtracer::trace
/*! Handed down for rays that next event estimation cannot generate: the
primary ray, and every specular or transmissive bounce. A light source reached
by such a ray contributes its full emission, because shade() never produced a
competing estimate of the same path. */
static const SLfloat PDF_NO_MIS = -1.0f;
//-----------------------------------------------------------------------------
//! Path depth up to which Russian roulette always lets the path continue
/*! Purely a variance and cost knob: every value here is unbiased, so this
trades render time against noise and nothing else.

Roulette is what makes the estimator unbiased, but it is not free. Because the
survival probability is the albedo, the 1/survival of a survivor cancels that
bounce's attenuation exactly, so a path that survives fifteen bounces arrives
carrying full weight instead of 0.75^15. Roulette therefore turns "many paths,
each tiny" into "few paths, each full size": the same mean with a much heavier
tail. Taking the first bounces deterministically keeps that tail out of the
part of the sum that carries most of the energy.

A model of the interreflection series alone (albedo 0.75, exact answer
1/(1-0.75) = 4) says the roulette tail shrinks quickly with this value:

    start depth   mean     sd     mean path length
              3   3.998   1.46     7.00
              5   4.000   0.82     9.02
              8   4.000   0.34    12.00

Measured on the real scene, it does not. Raising it from 3 to 8 made the
ceiling around the light *worse* (median absolute residual 3.77 -> 4.05 at
100 spp) for 1.7 times the path length. The model is wrong about what dominates
there: the noise in that region is not the roulette tail but the near field of
the area light, where a surface centimetres from a 1.0 x 0.65 emitter receives
an enormous radiance that neighbouring surfaces then see through a rare bounce.
Tracing deeper only finds more of those. So the value is kept low: roulette
removes the depth bias, and the near field needs a different fix (solid angle
sampling of the rectangle, or the sample clamp). */
static const SLint RR_START_DEPTH = 3;
//-----------------------------------------------------------------------------
//! Russian roulette survival test for the continuation of a path
/*! Returns 0 if the path is absorbed and must not be continued, otherwise the
survival probability that the caller has to divide its contribution by.

Russian roulette is not the same thing as Monte Carlo, although both are
random. Monte Carlo is the estimator itself: sample from a density and average
f(x)/p(x). Russian roulette is a technique used inside it, and it answers a
different question — how to terminate an unbounded recursion without making the
answer wrong. Light bounces between the walls forever, so the recursion has to
be stopped somewhere. Stopping it at a fixed depth discards the light that the
longer paths would have carried, which is a bias that no number of samples can
remove. Stopping it at random and dividing the survivors by their survival
probability leaves the mean untouched:

    E = q * 0 + (1 - q) * L / (1 - q) = L

The survival probability is the albedo of the surface, so that the 1/survival
of a survivor cancels the albedo of that bounce exactly. That is the classic
formulation: the colour of the surface decides how likely the path is to
continue, rather than how much it is dimmed. A surface with an albedo of 1
absorbs nothing and therefore always survives, which is why the hard depth cap
in trace() is still needed as a safety net. */
static SLfloat russianRoulette(SLint depth, SLCol4f albedo)
{
    if (depth <= RR_START_DEPTH) return 1.0f;

    SLfloat survival = std::min(albedo.maxXYZ(), 1.0f);

    if (survival <= 0.0f) return 0.0f;  // black surface, nothing to continue
    if (survival >= 1.0f) return 1.0f;  // absorbs nothing, always continues

    return (rnd01() < survival) ? survival : 0.0f;
}
//-----------------------------------------------------------------------------
//! Weight of one sample drawn from the Phong lobe of the given exponent
/*! SLRay::reflectMC and SLRay::refractMC draw a direction from the Phong lobe

    p(w) = (n + 1) / (2 * PI) * cos^n(alpha)

where alpha is the angle to the perfect specular or transmissive direction and
n is the shininess or the translucency. The matching normalised Phong BSDF is

    f(w) = rho * (n + 2) / (2 * PI) * cos^n(alpha)

and the Monte Carlo estimator of the rendering equation weights the incoming
radiance by f * cos(theta) / p, with theta the angle to the SURFACE NORMAL and
not to the lobe axis. The cos^n(alpha) and the 2*PI cancel and what is left is

    rho * (n + 2) / (n + 1) * cos(theta)

The colour rho is applied by the caller as the material's specular or
transmissive colour, so this returns the scalar part.

The cos(theta) is what was missing: the code applied only (n+2)/(n+1), so every
glossy sample was too bright by 1/cos(theta). That is a factor of 1 straight
along the normal and unbounded at the horizon, which is the direction a wide
lobe on a grazing surface samples most often, so the error showed up as a
bright rim exactly where a glossy highlight is supposed to fall off.

For a perfect mirror the caller applies no weight at all rather than calling
this: (n+2)/(n+1) is the normalisation of the lobe estimator and there is no
lobe to normalise when the direction was not drawn from one. At the PERFECT
limit of 1000 that factor is 1.001, so it was silently adding a tenth of a
percent of energy per specular bounce.

\param exponent  shininess for reflection, translucency for transmission
\param sampleDir sampled direction, already known to be on the right side
\param normal    the surface normal at the hit point */
static SLfloat phongLobeWeight(SLfloat        exponent,
                               const SLVec3f& sampleDir,
                               const SLVec3f& normal)
{
    // The absolute value, because SLMesh::preShade does not flip the hit
    // normal towards the ray: on a back face hit both the normal and the
    // sampled direction sit on the other side and the cosine comes out
    // negative although the geometry is the same.
    SLfloat cosTheta = std::abs(sampleDir.dot(normal));

    return (exponent + 2.0f) / (exponent + 1.0f) * cosTheta;
}
//-----------------------------------------------------------------------------
//! Power heuristic (beta = 2) weight of the strategy whose density is pdfThis
/*! Two strategies that can both generate the same path each return an unbiased
estimate of it, so simply adding them would count the path twice. Weighting
them with w(pdfThis) + w(pdfOther) = 1 counts it exactly once, and the power
heuristic puts nearly all of that weight on whichever strategy had the higher
density for this particular path, which is the one with the lower variance
here. Written as 1/(1+r^2) rather than a^2/(a^2+b^2) so that a very large
density cannot overflow. */
static SLfloat misWeight(SLfloat pdfThis, SLfloat pdfOther)
{
    if (pdfThis <= 0.0f) return 0.0f;

    SLfloat ratio = pdfOther / pdfThis;
    return 1.0f / (1.0f + ratio * ratio);
}
//-----------------------------------------------------------------------------
//! Solid angle density with which shade() would have sampled this light hit
/*! Returns 0 if the hit surface is not a light that shade() samples over its
area. shade() draws a point uniformly on the rectangle, so the density over the
area is 1/area; the conversion to a density over solid angle, as seen from the
shaded point, is the usual dist^2 / (cosLight * area). */
static SLfloat lightPdfMC(SLRay* ray)
{
    if (!ray->hitNode || typeid(*ray->hitNode) != typeid(SLLightRect))
        return 0.0f;

    auto* rect = (SLLightRect*)ray->hitNode;
    if (!rect->isOn()) return 0.0f;

    // SLRay::setDir does not normalise, so do not assume a unit direction
    SLVec3f dir(ray->dir);
    dir.normalize();

    // spotDirWS is the normal of the rectangle, as in shade()
    SLfloat cosLight = -dir.dot(rect->spotDirWS());
    if (cosLight <= 0.0f) return 0.0f;

    return (ray->length * ray->length) / (cosLight * rect->area());
}

//-----------------------------------------------------------------------------
SLPathtracer::SLPathtracer()
{
    name("PathTracer");
    _calcDirect   = true;
    _calcIndirect = true;
    _sampleClamp  = 3.0f; // see SLPathtracer.h; 0 switches it off
    _noiseRSE     = 0.0f;
    _noiseRSE999  = 0.0f;
    gamma(2.2f);
}
//-----------------------------------------------------------------------------
/*!
Main render function. The Path Tracing algorithm starts from here
*/
SLbool SLPathtracer::render(SLSceneView* sv)
{
    _sv         = sv;
    _state      = rtBusy; // From here we state the PT as busy
    _renderSec  = 0.0f;   // reset time
    _progressPC = 0;      // % rendered

    initStats(0); // init statistics
    prepareImage();

    // Drop the 8 bit accumulation image that older versions kept here.
    // The path tracer needs only _images[0], which holds the clamped and
    // gamma corrected image for the display. The progressive mean itself is
    // accumulated in _radianceSum (see SLPathtracer.h).
    while (_images.size() > 1)
    {
        delete _images[_images.size() - 1];
        _images.pop_back();
    }

    // Allocate and clear the high precision accumulation buffer. Its size is
    // taken from _images[0] and not from the viewport, because prepareImage
    // scales the image with _resolutionFactor.
    const size_t numPixels = (size_t)_images[0]->width() *
                             (size_t)_images[0]->height();
    _radianceSum.assign(numPixels, SLCol4f::BLACK);

    // The two moments the noise figure is computed from. See computeNoise.
    _lumSum.assign(numPixels, 0.0);
    _lumSumSq.assign(numPixels, 0.0);
    _noiseRSE    = 0.0f;
    _noiseRSE999 = 0.0f;

    // Measure time
    double t1 = GlobalTimer::timeS();

    // Lambda function for async slice rendering
    renderSlicesPTAsync = [this](bool isMainThread, SLint curSample, SLuint threadNum)
    {
        SLPathtracer::renderSlices(isMainThread, curSample, threadNum);
    };

    // Do multi-threading only in release config
    SL_LOG("\n\nRendering with %d samples", _aaSamples);
    SL_LOG("\nCurrent Sample:       ");
    for (int currentSample = 1; currentSample <= _aaSamples; currentSample++)
    {
        vector<thread> threads; // vector for additional threads
        _nextLine = 0;

        // Start additional threads on the renderSlices function
        for (SLuint t = 0; t < Utils::maxThreads() - 1; t++)
            threads.emplace_back(renderSlicesPTAsync, false, currentSample, t);

        // Do the same work in the main thread
        renderSlicesPTAsync(true, currentSample, 0);

        for (auto& thread : threads)
            thread.join();

        _progressPC = (SLint)((SLfloat)currentSample / (SLfloat)_aaSamples * 100.0f);
    }

    _renderSec = GlobalTimer::timeS() - (SLfloat)t1;
    _raysPerMS.set((float)SLRay::totalNumRays() / _renderSec / 1000.0f);
    _progressPC = 100;
    computeNoise();

    SL_LOG("\nTime to render image: %6.3fsec", _renderSec);

    _state = rtFinished;
    return true;
}
//-----------------------------------------------------------------------------
/*!
Renders a slice of 4px width.
*/
void SLPathtracer::renderSlices(const bool isMainThread,
                                SLint      currentSample,
                                SLuint     threadNum)
{
    if (!isMainThread)
    {
        PROFILE_THREAD(string("PT-Worker-") + std::to_string(threadNum));
    }

    PROFILE_FUNCTION();

    // Time points
    double t1 = 0;

    const SLint imgW = (SLint)_images[0]->width();
    const SLint imgH = (SLint)_images[0]->height();

    while (_nextLine < imgW)
    {
        // The next section must be protected
        // Making _nextLine an atomic was not sufficient.
        _mutex.lock();
        SLint minX = _nextLine;
        _nextLine += 4;
        _mutex.unlock();

        // The image width is not necessarily a multiple of the slice width of
        // 4px, so the last slice has to be cut off. Without this the loop below
        // would index past the end of _radianceSum.
        SLint maxX = std::min(minX + 4, imgW);

        for (SLint x = minX; x < maxX; ++x)
        {
            for (SLint y = 0; y < imgH; ++y)
            {
                SLCol4f color(SLCol4f::BLACK);

                // calculate direction for primary ray - scatter with random variables for anti aliasing
                SLRay primaryRay;
                setPrimaryRay((SLfloat)((SLfloat)x - rnd01() + 0.5f),
                              (SLfloat)((SLfloat)y - rnd01() + 0.5f),
                              &primaryRay);

                ///////////////////////////////////
                color += trace(&primaryRay, PDF_NO_MIS);
                ///////////////////////////////////

                // Optional firefly clamp. The brightest channel is brought
                // down to the limit and the other two are scaled with it, so
                // that the sample loses energy but keeps its colour. Clamping
                // each channel on its own would shift the hue of everything it
                // touches. See _sampleClamp for why this is off by default.
                if (_sampleClamp > 0.0f)
                {
                    SLfloat maxChannel = color.maxXYZ();
                    if (maxChannel > _sampleClamp)
                        color *= _sampleClamp / maxChannel;
                }

                // Add the raw linear radiance of this sample to the running sum.
                // Nothing is clamped or quantised here on purpose: the estimator
                // only converges with 1/sqrt(N) if every sample keeps its full
                // value and its full precision. The correction that one sample
                // applies to the mean shrinks with 1/currentSample, so rounding
                // the mean to 8 bit (as the old _images[1] did) would freeze
                // bright outliers (fireflies) at a wrong value forever.
                const size_t pixel = (size_t)y * (size_t)imgW + (size_t)x;

                SLCol4f& radianceSum = _radianceSum[pixel];
                radianceSum += color;

                // The same sample as a single number, for the noise figure of
                // computeNoise. It is taken here and not from _images[0],
                // because that image is clamped to [0,1] and gamma corrected
                // for the display, which flattens every firefly to white and
                // destroys exactly the signal being measured. Rec. 709
                // luminance, matching the linear primaries the renderer works
                // in. When _sampleClamp is on the sample arriving here is
                // already clamped, so the figure describes the noise of the
                // clamped estimator, which is the image actually produced.
                const SLdouble lum = 0.2126 * (SLdouble)color.r +
                                     0.7152 * (SLdouble)color.g +
                                     0.0722 * (SLdouble)color.b;
                _lumSum[pixel] += lum;
                _lumSumSq[pixel] += lum * lum;

                // The mean of all samples taken so far for this pixel
                color = radianceSum / (SLfloat)currentSample;

                // From here on the color is for the display only: clamp it into
                // the displayable range and apply the gamma correction.
                color.clampMinMax(0.0f, 1.0f);
                color.gammaCorrect(_oneOverGamma);

                // image to render
                _images[0]->setPixeliRGB(x,
                                         y,
                                         CVVec4f(color.r,
                                                 color.g,
                                                 color.b,
                                                 color.a));
            }

            // update image after 500 ms
            if (_sv->onWndUpdate && isMainThread)
            {
                if (GlobalTimer::timeS() - t1 > 0.5f)
                {
                    renderUIBeforeUpdate();
                    _sv->onWndUpdate(); // update window
                    t1 = GlobalTimer::timeS();
                }
            }
        }
    }
}
//-----------------------------------------------------------------------------
/*!
Recursively traces ray in scene.
*/
SLCol4f SLPathtracer::trace(SLRay* ray, SLfloat bsdfPdf)
{
    // The radiance gathered at this hit point. It must start at BLACK. It used
    // to start at ray->backgroundColor, which added the background to EVERY
    // surface at EVERY bounce and therefore compounded down the path. That
    // stayed invisible in scenes with a black background and no skybox only.
    SLCol4f finalColor(SLCol4f::BLACK);

    // Participating Media init
    SLfloat absorption = 1.0f; // used to calculate absorption along the ray
    SLfloat scaleBy    = 1.0f; // used to scale surface reflectance at the end of random walk

    // Intersect scene
    SLNode* root = _sv->s()->root3D();
    if (root) root->hitRec(ray);

    // End of recursion: the ray escaped the scene, so it gathers the radiance
    // of the environment (skybox or camera background). This is the only place
    // where the background may contribute.
    if (ray->length >= FLT_MAX)
        return ray->backgroundColor;

    // End of recursion: the safety net. Russian roulette below decides how long
    // a path lives, but it cannot terminate a surface that absorbs nothing,
    // such as the perfect mirror and the glass of the Muttenzer Box, so a hard
    // cap is still needed against an endless specular chain. The cap is set
    // high enough (see the startPathtracing call sites) that roulette and not
    // this test ends virtually every path, so the bias it used to cause is now
    // negligible instead of dominant.
    if (ray->depth > maxDepth())
        return SLCol4f::BLACK;

    // hit material
    SLMaterial* mat = ray->hitMesh->mat();
    ray->hitMesh->preShade(ray);

    // set object color
    SLCol4f objectColor = SLCol4f::BLACK;
    if (ray->hitMatIsDiffuse())
        objectColor = mat->diffuse();
    else if (ray->hitMatIsReflective())
        objectColor = mat->specular();
    else if (ray->hitMatIsTransparent())
        objectColor = mat->transmissive();

    // set object emission
    SLCol4f objectEmission = mat->emissive();
    SLfloat maxEmission    = objectEmission.maxXYZ();

    // End of recursion: a light source is hit
    if (maxEmission > 0)
    {
        // The primary ray, or a specular or transmissive bounce. shade() only
        // ever samples from a diffuse surface, so it cannot have produced a
        // competing estimate of this path and the emission counts in full.
        // This also covers the camera looking straight at the light.
        if (bsdfPdf < 0.0f)
            return mat->emissive() * absorption;

        SLfloat lightPdf = lightPdfMC(ray);

        if (lightPdf <= 0.0f)
        {
            // Emissive, but not a light that shade() samples over an area. If
            // it is a light node at all, shade() samples it as a point or
            // directional delta light, whose direction the scattering can
            // never reproduce, so counting it here as well would count it
            // twice. SLLightSpot::hitRec and SLLightDirect::hitRec currently
            // reject every ray that is not primary, so this cannot be reached;
            // the test keeps the invariant here rather than resting on those
            // two overrides. Anything else is an ordinary emissive mesh that
            // shade() ignores, so it counts in full.
            if (dynamic_cast<SLLight*>(ray->hitNode) != nullptr)
                return SLCol4f::BLACK;

            return mat->emissive() * absorption;
        }

        // Both shade() and the scattering can generate this path. Weight the
        // two estimates with the power heuristic; shade() applies the
        // complementary weight, so together they count the path exactly once.
        return mat->emissive() * absorption * misWeight(bsdfPdf, lightPdf);
    }

    // add absorption to base color from Participating Media
    objectColor = objectColor * absorption;

    // diffuse reflection
    if (ray->hitMatIsDiffuse())
    {
        // Add component wise the texture color
        if (mat->numTextures() > 0)
        {
            objectColor &= ray->hitTexColor;
        }

        if (_calcDirect)
            finalColor += shade(ray, &objectColor) * scaleBy;

        // Russian roulette decides whether the path continues. The direct
        // illumination above is a terminal estimate at this vertex and is
        // always taken; only the continuation is gambled on. The roll is made
        // inside the test so that switching the indirect illumination off does
        // not consume random numbers here.
        SLfloat survival = _calcIndirect ? russianRoulette(ray->depth, objectColor)
                                         : 0.0f;

        if (survival > 0.0f)
        {
            SLRay scatter;
            ray->diffuseMC(&scatter);

            // diffuseMC draws the direction from the cosine distribution, so
            // its solid angle density is cos(theta)/PI. It is handed down so
            // that a light hit further along the path can be weighted against
            // the light sampling that shade() just did at this same vertex.
            // With the direct illumination switched off shade() never runs,
            // so there is nothing to weight against and the hit counts fully.
            SLfloat scatterPdf = PDF_NO_MIS;
            if (_calcDirect)
                scatterPdf = std::max(scatter.dir.dot(ray->hitNormal), 0.0f) *
                             Utils::ONEOVERPI;

            // material emission, material diffuse and recursive indirect
            // illumination, scaled up by 1/survival for the paths that lived
            finalColor += (trace(&scatter, scatterPdf) & objectColor) *
                          (scaleBy / survival);
        }
    }
    else if (ray->hitMatIsReflective())
    {
        // Russian roulette, as in the diffuse branch. A perfect mirror has an
        // albedo of 1, always survives, and is bounded only by the depth cap.
        SLfloat survival = russianRoulette(ray->depth, objectColor);
        if (survival <= 0.0f) return finalColor;

        // scatter toward perfect specular direction
        SLRay reflected;
        ray->reflect(&reflected);

        // scatter around perfect reflected direction only if material not perfect
        SLfloat lobeWeight = 1.0f;
        if (mat->shininess() < SLMaterial::PERFECT)
        {
            if (!ray->reflectMC(&reflected, SLRay::lobeToWorld(reflected.dir)))
                return finalColor; // sample below the horizon, see reflectMC

            lobeWeight = phongLobeWeight(mat->shininess(),
                                         reflected.dir,
                                         ray->hitNormal);
        }

        // lobe weight * recursive indirect illumination and material base color
        finalColor += (lobeWeight * (trace(&reflected, PDF_NO_MIS) & objectColor)) *
                      (scaleBy / survival);
    }
    else if (ray->hitMatIsTransparent())
    {
        // Russian roulette, as in the diffuse branch. This is independent of
        // the Fresnel choice further down, which picks reflection or
        // transmission but never ends the path.
        SLfloat survival = russianRoulette(ray->depth, objectColor);
        if (survival <= 0.0f) return finalColor;

        // scatter toward perfect transmissive direction
        SLRay refracted;
        ray->refract(&refracted);

        // init Schlick's approximation
        SLVec3f rayDir = ray->dir;
        rayDir.normalize();
        SLVec3f refrDir = refracted.dir;
        refrDir.normalize();
        SLfloat n, nt;
        SLVec3f hitNormal = ray->hitNormal;
        hitNormal.normalize();

        // ray from outside in
        if (ray->isOutside)
        {
            n  = 1.0f;
            nt = mat->kn();
        }
        else // ray from inside out
        {
            n  = mat->kn();
            nt = 1.0f;
        }

        // calculate Schlick's approx.
        SLfloat nbig, nsmall;
        nbig            = n > nt ? n : nt;
        nsmall          = n < nt ? n : nt;
        SLfloat R0      = ((nbig - nsmall) / (nbig + nsmall));
        R0              = R0 * R0;
        SLbool  into    = (rayDir * hitNormal) < 0;
        SLfloat c       = 1.0f - (into ? (-rayDir * hitNormal) : (refrDir * hitNormal));
        SLfloat schlick = R0 + (1 - R0) * c * c * c * c * c;

        SLfloat P                     = 0.25f + 0.5f * schlick; // probability of reflectance
        SLfloat reflectionProbability = schlick / P;
        SLfloat refractionProbability = (1.0f - schlick) / (1.0f - P);

        // scatter around perfect transmissive direction only if material not perfect
        SLfloat lobeWeight     = 1.0f;
        SLbool  refractIsValid = true;
        if (mat->translucency() < SLMaterial::PERFECT)
        {
            refractIsValid = ray->refractMC(&refracted,
                                            SLRay::lobeToWorld(refracted.dir));

            if (refractIsValid)
                lobeWeight = phongLobeWeight(mat->translucency(),
                                             refracted.dir,
                                             ray->hitNormal);
        }

        // probability of reflection
        if (rnd01() > (0.25f + 0.5f * schlick))
        {
            // scatter toward transmissive direction. A sample that came back
            // out on the incident side carries no energy, see refractMC.
            if (refractIsValid)
                finalColor += (lobeWeight *
                               (trace(&refracted, PDF_NO_MIS) & objectColor) *
                               refractionProbability) *
                              (scaleBy / survival);
        }
        else
        {
            // scatter toward perfect specular direction. This is a mirror ray
            // and not a lobe sample, so it carries no lobe weight: it used to
            // be multiplied by (shininess + 2) / (shininess + 1), which is the
            // normalisation of the Phong lobe estimator and belongs only where
            // a direction was drawn from that lobe. For the shininess of 100
            // of the Muttenzer Box glass that was a 1% energy gain per bounce.
            SLRay scattered;
            ray->reflect(&scattered);

            // recursive indirect illumination and material base color
            finalColor += ((trace(&scattered, PDF_NO_MIS) & objectColor) *
                           reflectionProbability) *
                          (scaleBy / survival);
        }
    }

    return finalColor;
}
//-----------------------------------------------------------------------------
/*!
Calculates the direct illumination at the hit point of the ray by sampling the
light sources explicitly (next event estimation).

A rectangular light is an AREA light and is estimated with a Monte Carlo area
estimator: one point is sampled uniformly on the rectangle (pdf = 1/area) and
the radiance reflected towards the ray is

    Lo = albedo/PI * Le * cosSurface * cosLight / dist^2 * area

with the direction, the distance and both cosines taken at the SAMPLED point.

The previous version took all of them at the light CENTRE while it tested the
visibility at a random point, and it replaced the geometric term by the OpenGL
attenuation and the spot cone exponent. That is the Blinn-Phong rasteriser
model, not an estimator of the area light integral. It missed the area factor
completely, which made the direct light 1/area too bright (1.54x for the
1.0 x 0.65 light of the Muttenzer Box), and it broke down for surfaces close to
a large light. Above all it put the direct illumination on a different scale
than the emissive material of the light mesh that the paths see when they hit
the light through the mirror or the glass sphere, so the two estimates of the
same illumination did not agree.

All other light types are point or directional (delta) lights without any area.
For those the classic attenuation and spot cone model is kept.
*/
SLCol4f SLPathtracer::shade(SLRay* ray, SLCol4f* mat)
{
    SLCol4f color  = SLCol4f::BLACK;
    SLNode* root3D = _sv->s()->root3D();
    SLVec3f N(ray->hitNormal);

    // loop over light sources in scene
    for (auto* light : _sv->s()->lights())
    {
        if (!light || !light->isOn()) continue;

        if (typeid(*light) == typeid(SLLightRect))
        {
            ///////////////////////////////////////////////////////
            // Area light: Monte Carlo estimate over its surface  //
            ///////////////////////////////////////////////////////

            auto* rect = (SLLightRect*)light;

            // One uniformly distributed sample point on the light (pdf=1/area)
            SLVec3f toLight(rect->samplePointMC() - ray->hitPoint);
            SLfloat distSqr = toLight.lengthSqr();
            if (distSqr < FLT_EPSILON) continue;
            SLfloat dist = sqrt(distSqr);
            toLight /= dist;

            // Cosine at the shaded surface and at the sampled light point.
            // spotDirWS is the normal of the light rectangle.
            SLfloat cosSurface = toLight.dot(N);
            SLfloat cosLight   = -toLight.dot(rect->spotDirWS());

            // Sample is below the surface or behind the (one sided) light
            if (cosSurface <= 0.0f || cosLight <= 0.0f) continue;

            // Visibility of the sampled point. The light mesh itself does not
            // block, see SLLightRect::hitRec which ignores shadow rays.
            SLRay shadowRay(dist, toLight, ray);
            root3D->hitRec(&shadowRay);
            if (shadowRay.length < dist) continue;

            // Geometric term of the area formulation
            SLfloat geometry = cosSurface * cosLight / distSqr;

            // The cosine weighted scattering in trace() can generate this same
            // path, so weight the two estimates against each other; trace()
            // applies the complementary weight. With the indirect
            // illumination switched off the scattering never runs and this
            // estimate carries the path alone.
            SLfloat weight = 1.0f;
            if (_calcIndirect)
            {
                // The same conversion of 1/area into a solid angle density
                // that lightPdfMC does for the hit coming the other way
                SLfloat lightPdf = distSqr / (cosLight * rect->area());
                SLfloat bsdfPdf  = cosSurface * Utils::ONEOVERPI;
                weight           = misWeight(lightPdf, bsdfPdf);
            }

            // albedo * brdf(1/PI) * Le * G / pdf, with pdf = 1/area
            color += (*mat & light->diffuse()) *
                     (Utils::ONEOVERPI * geometry * rect->area() * weight);
        }
        else
        {
            ///////////////////////////////////////////////////////
            // Point or directional light: no area, classic model //
            ///////////////////////////////////////////////////////

            SLVec3f L(light->positionWS().vec3() - ray->hitPoint);
            SLfloat lightDist = L.length();
            L /= lightDist;
            SLfloat LdN = L.dot(N);

            // hit point faces away from the light
            if (LdN <= 0.0f) continue;

            SLfloat lighted = light->shadowTestMC(ray, L, lightDist, root3D);
            if (lighted <= 0.0f) continue;

            // calculate spot effect if light is a spotlight
            SLfloat spotEffect = 1.0f;
            if (light->spotCutOffDEG() < 180.0f)
            {
                SLfloat LdS = std::max(-L.dot(light->spotDirWS()), 0.0f);

                // check if point is in spot cone
                if (LdS <= light->spotCosCut()) continue;

                spotEffect = pow(LdS, (SLfloat)light->spotExponent());
            }

            // material color * light emission * LdN * brdf(1/pi) * lighted
            SLCol4f diffuseColor = (*mat & (light->diffuse() * LdN)) *
                                   (Utils::ONEOVERPI * lighted);

            color += light->attenuation(lightDist) * spotEffect * diffuseColor;
        }
    }

    return color;
}
//-----------------------------------------------------------------------------
/*!
Turns the two luminance moments that renderSlices accumulated into the noise
figures of the Timing panel. Called once, after the last sample pass.

For a pixel that received N samples with luminances x_i, from the sums

    S1 = sum(x_i)  and  S2 = sum(x_i^2)

the variance of one sample and the standard error of the pixel, which is the
mean of those samples, are

    s^2 = (S2 - S1^2 / N) / (N - 1)
    SE  = sqrt(s^2 / N)

SE is in the units of the pixel, so it is divided by the pixel to give a
relative standard error that can be compared between scenes, exposures and
resolutions. noiseRSE is the mean of that over the image, noiseRSE999 the
99.9th percentile, which is the figure fireflies move.

A Monte Carlo estimator converges with 1/sqrt(N), so noiseRSE has to fall by
half when the samples per pixel are quadrupled. That makes it a check on the
estimators in trace() and shade() as much as a readout: if it does not halve,
something upstream is wrong.
*/
void SLPathtracer::computeNoise()
{
    const size_t numPixels = _lumSum.size();

    // The variance of a sample needs at least two samples to exist.
    if (numPixels == 0 || _aaSamples < 2)
    {
        _noiseRSE    = 0.0f;
        _noiseRSE999 = 0.0f;
        return;
    }

    const SLdouble n = (SLdouble)_aaSamples;

    // Keeps a black pixel, where mu is 0 and no number of samples is going to
    // change that, from dividing by zero.
    const SLdouble eps = 1e-3;

    vector<SLfloat> rse(numPixels);
    SLdouble        sum = 0.0;

    for (size_t i = 0; i < numPixels; ++i)
    {
        const SLdouble s1 = _lumSum[i];
        const SLdouble s2 = _lumSumSq[i];
        const SLdouble mu = s1 / n;

        SLdouble variance = (s2 - s1 * s1 / n) / (n - 1.0);

        // A pixel that got the same value every time, a background pixel for
        // instance, has a true variance of 0 and the subtraction above can land
        // just below it. Without this the sqrt would return a NaN.
        if (variance < 0.0)
            variance = 0.0;

        rse[i] = (SLfloat)(sqrt(variance / n) / (mu + eps));
        sum += (SLdouble)rse[i];
    }

    _noiseRSE = (SLfloat)(sum / (SLdouble)numPixels);

    // nth_element partitions around the wanted rank instead of sorting the
    // whole image, which is O(numPixels) rather than O(numPixels log numPixels)
    // and is all a percentile needs.
    size_t k = (size_t)(0.999 * (SLdouble)numPixels);
    if (k >= numPixels)
        k = numPixels - 1;
    std::nth_element(rse.begin(),
                     rse.begin() + (std::ptrdiff_t)k,
                     rse.end());
    _noiseRSE999 = rse[k];
}
//-----------------------------------------------------------------------------
//! Saves the current PT image as PNG image
void SLPathtracer::saveImage()
{
    static SLint no = 0;
    SLchar       filename[255];
    snprintf(filename,
             sizeof(filename),
             "Pathtraced_%d_%d.png",
             _aaSamples,
             no++);
    _images[0]->savePNG(filename);
}
//-----------------------------------------------------------------------------
