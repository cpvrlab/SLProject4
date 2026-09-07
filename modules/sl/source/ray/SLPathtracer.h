/**
 * \file      SLPathtracer.h
 * \date      February 2014
 * \authors   Thomas Schneiter, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPATHTRACER_H
#define SLPATHTRACER_H

#include <SLRaytracer.h>

//-----------------------------------------------------------------------------
//! Classic Monte Carlo Pathtracing algorithm for real global illumination
class SLPathtracer : public SLRaytracer
{
public:
    SLPathtracer();
    ~SLPathtracer() { SL_LOG("Destructor       : ~SLPathtracer"); }

    // classic ray tracer functions
    SLbool  render(SLSceneView* sv);
    void    renderSlices(bool   isMainThread,
                         SLint  currentSample,
                         SLuint threadNum);
    //! Traces one ray. bsdfPdf is the solid angle density with which the
    /*! scattering at the previous vertex produced this ray, or the negative
    sentinel PDF_NO_MIS if next event estimation cannot generate the same
    path (the primary ray, and every specular or transmissive bounce). It is
    what lets a light hit be weighted against the light sampling in shade(). */
    SLCol4f trace(SLRay* ray, SLfloat bsdfPdf);
    SLCol4f shade(SLRay* ray, SLCol4f* mat);
    void    saveImage();

    // Setters
    void calcDirect(SLbool di) { _calcDirect = di; }
    void calcIndirect(SLbool ii) { _calcIndirect = ii; }

    // Getters
    SLbool calcDirect() const { return _calcDirect; }
    SLbool calcIndirect() const { return _calcIndirect; }

private:
    function<void(bool, int, SLuint)> renderSlicesPTAsync;

    SLbool _calcDirect;   //!< flag to calculate direct illumination
    SLbool _calcIndirect; //!< flag to calculate indirect illumination

    //! Linear, unclamped sum of all radiance samples taken so far per pixel
    /*! The progressive mean of a path tracer must never be kept in an 8 bit
    image. Rounding the running mean to 1/255 after every sample stops the
    convergence as soon as the correction of one sample, which is
    |sample - mean| / sampleNo, falls below half a quantisation step, i.e. as
    soon as sampleNo > 510 * |sample - mean|. A bright outlier (a firefly) then
    freezes at a wrong value and never averages out again, no matter how many
    samples are rendered. That is why this buffer holds the raw sum in full
    float precision and is divided by the sample number for the display only.
    Its size is _images[0]->width() * _images[0]->height() and it is indexed
    with y * width + x. */
    vector<SLCol4f> _radianceSum;
};
//-----------------------------------------------------------------------------
#endif
