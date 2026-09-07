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
    void sampleClamp(SLfloat max) { _sampleClamp = max; }

    // Getters
    SLbool  calcDirect() const { return _calcDirect; }
    SLbool  calcIndirect() const { return _calcIndirect; }
    SLfloat sampleClamp() const { return _sampleClamp; }

private:
    function<void(bool, int, SLuint)> renderSlicesPTAsync;

    SLbool _calcDirect;   //!< flag to calculate direct illumination
    SLbool _calcIndirect; //!< flag to calculate indirect illumination

    //! Upper limit on the radiance of a single sample, 0 to switch it off
    /*! A firefly is a path that carries far more energy than the pixel it lands
    in, most often a caustic: light that reaches a diffuse surface through the
    mirror or the glass sphere. The estimator is right about it, but such a path
    is found so seldom that the average is still visibly lumpy after a thousand
    samples. Capping what one sample may contribute removes it at once.

    This is a deliberate bias, and the only one left in the renderer: it
    discards the part of a caustic above the limit and makes it darker than it
    is. Measured on the Muttenzer Box at 100 spp, against no clamp:

      limit   fireflies, far wall      mean radiance of the caustic
        off                 4.35%                             1.000
         30                 4.75%                             0.985
         10                 4.08%                             0.938
          3                 0.02%                             0.695

    The fireflies of this scene sit between 3 and 10, so 30 does nothing and 10
    barely helps. 3 removes them almost entirely and costs 2 to 5% on ordinary
    surfaces, 15% on the ceiling below the light and 30% on the caustic under
    the glass sphere. That is the trade, and it is why this is a menu item that
    can be switched off rather than a constant.

    Note that it caps the SAMPLE and never the running mean. Clamping the mean
    is the defect that the float accumulation buffer removed: it froze the
    fireflies at a wrong value instead of averaging them away. */
    SLfloat _sampleClamp;

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
