/**
 * \file      SLPathtracer.cpp
 * \authors   Thomas Schneiter
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include <algorithm>

#include <SLCamera.h>
#include <SLLightRect.h>
#include <SLPathtracer.h>
#include <SLSceneView.h>
#include <GlobalTimer.h>
#include <Profiler.h>

extern SLfloat rnd01();

//-----------------------------------------------------------------------------
SLPathtracer::SLPathtracer()
{
    name("PathTracer");
    _calcDirect   = true;
    _calcIndirect = true;
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
    _radianceSum.assign((size_t)_images[0]->width() *
                          (size_t)_images[0]->height(),
                        SLCol4f::BLACK);
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
                color += trace(&primaryRay, false);
                ///////////////////////////////////

                // Add the raw linear radiance of this sample to the running sum.
                // Nothing is clamped or quantised here on purpose: the estimator
                // only converges with 1/sqrt(N) if every sample keeps its full
                // value and its full precision. The correction that one sample
                // applies to the mean shrinks with 1/currentSample, so rounding
                // the mean to 8 bit (as the old _images[1] did) would freeze
                // bright outliers (fireflies) at a wrong value forever.
                SLCol4f& radianceSum = _radianceSum[(size_t)y * (size_t)imgW +
                                                    (size_t)x];
                radianceSum += color;

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
SLCol4f SLPathtracer::trace(SLRay* ray, SLbool em)
{
    SLCol4f finalColor(ray->backgroundColor);

    // Participating Media init
    SLfloat absorption = 1.0f; // used to calculate absorption along the ray
    SLfloat scaleBy    = 1.0f; // used to scale surface reflectance at the end of random walk

    // Intersect scene
    SLNode* root = _sv->s()->root3D();
    if (root) root->hitRec(ray);

    // end of recursion - no object hit OR max depth reached
    if (ray->length >= FLT_MAX || ray->depth > maxDepth())
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

    // stop recursion if light source is hit
    if (maxEmission > 0)
    {
        if (ray->depth == 1)
            return mat->emissive() * absorption;
        else
            return mat->emissive() * absorption * em;
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

        if (_calcIndirect)
        {
            SLRay scatter;
            ray->diffuseMC(&scatter);

            // material emission, material diffuse and recursive indirect illumination
            finalColor += (trace(&scatter, false) & objectColor) * scaleBy;
        }
    }
    else if (ray->hitMatIsReflective())
    {
        // scatter toward perfect specular direction
        SLRay reflected;
        ray->reflect(&reflected);

        // scatter around perfect reflected direction only if material not perfect
        if (mat->shininess() < SLMaterial::PERFECT)
        {
            // rotation matrix for glossy
            SLMat3f rotMat;
            SLVec3f rotAxis((SLVec3f(0.0f, 0.0f, 1.0f) ^ reflected.dir).normalize());
            SLfloat rotAngle = acos(reflected.dir.z);
            rotMat.rotation(rotAngle * 180.0f * Utils::ONEOVERPI, rotAxis);
            ray->reflectMC(&reflected, rotMat);
        }

        // shininess contribution * recursive indirect illumination and matrial base color
        finalColor += ((mat->shininess() + 2.0f) / (mat->shininess() + 1.0f) *
                       (trace(&reflected, true) & objectColor)) *
                      scaleBy;
    }
    else if (ray->hitMatIsTransparent())
    {
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
        if (mat->translucency() < SLMaterial::PERFECT)
        {
            // rotation matrix for translucency
            SLMat3f rotMat;
            SLVec3f rotAxis((SLVec3f(0.0f, 0.0f, 1.0f) ^ refracted.dir).normalize());
            SLfloat rotAngle = acos(refracted.dir.z);
            rotMat.rotation((SLfloat)(rotAngle * 180.0f * Utils::ONEOVERPI), rotAxis);
            ray->refractMC(&refracted, rotMat);
        }

        // probability of reflection
        if (rnd01() > (0.25f + 0.5f * schlick))
            // scatter toward transmissive direction
            finalColor += ((mat->translucency() + 2.0f) /
                           (mat->translucency() + 1.0f) *
                           (trace(&refracted, true) & objectColor) *
                           refractionProbability) *
                          scaleBy;
        else
        {
            // scatter toward perfect specular direction
            SLRay scattered;
            ray->reflect(&scattered);

            // shininess contribution * recursive indirect illumination and material basecolor
            finalColor += ((mat->shininess() + 2.0f) /
                           (mat->shininess() + 1.0f) *
                           (trace(&scattered, true) & objectColor) *
                           reflectionProbability) *
                          scaleBy;
        }
    }

    return finalColor;
}
//-----------------------------------------------------------------------------
/*!
Calculates direct illumination for intersection point of ray
*/
SLCol4f SLPathtracer::shade(SLRay* ray, SLCol4f* mat)
{
    SLCol4f color        = SLCol4f::BLACK;
    SLCol4f diffuseColor = SLCol4f::BLACK;
    SLVec3f L, N;
    SLfloat lightDist, LdN, df, spotEffect, lighted;

    // loop over light sources in scene
    for (auto* light : _sv->s()->lights())
    {
        if (light && light->isOn())
        {
            N.set(ray->hitNormal);
            L.sub(light->positionWS().vec3(), ray->hitPoint);
            lightDist = L.length();
            L /= lightDist;
            LdN = L.dot(N);

            // check shadow ray if hit point is towards the light
            lighted = (SLfloat)((LdN > 0) ? light->shadowTestMC(ray,
                                                                L,
                                                                lightDist,
                                                                _sv->s()->root3D())
                                          : 0);

            // calculate spot effect if light is a spotlight
            if (lighted > 0.0f && light->spotCutOffDEG() < 180.0f)
            {
                SLfloat LdS = std::max(-L.dot(light->spotDirWS()), 0.0f);

                // check if point is in spot cone
                if (LdS > light->spotCosCut())
                {
                    spotEffect = pow(LdS, (SLfloat)light->spotExponent());
                }
                else
                {
                    lighted    = 0.0f;
                    spotEffect = 0.0f;
                }
            }
            else
                spotEffect = 1.0f;

            if (lighted > 0.0f)
            {
                df = std::max(LdN, 0.0f); // diffuse factor

                // material color * light emission * LdN * brdf(1/pi) * lighted(for soft shadows)
                diffuseColor = (*mat & (light->diffuse() * df) * Utils::ONEOVERPI * lighted);
            }

            color += light->attenuation(lightDist) * spotEffect * diffuseColor;
        }
    }

    return color;
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
