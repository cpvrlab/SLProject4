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
    void    computeNoise();

    // Setters
    void calcDirect(SLbool di) { _calcDirect = di; }
    void calcIndirect(SLbool ii) { _calcIndirect = ii; }
    void sampleClamp(SLfloat max) { _sampleClamp = max; }

    // Getters
    SLbool  calcDirect() const { return _calcDirect; }
    SLbool  calcIndirect() const { return _calcIndirect; }
    SLfloat sampleClamp() const { return _sampleClamp; }

    //! Mean relative standard error of the pixels of the last render
    /*! How far the average pixel is expected to sit from the value this
    estimator converges to, as a fraction of the pixel itself. It is a measure
    of the noise only: it says nothing about the bias that sampleClamp adds,
    and it falls monotonically as the clamp tightens. See computeNoise. */
    SLfloat noiseRSE() const { return _noiseRSE; }

    //! Relative standard error of the noisiest 0.1% of the pixels
    /*! The figure to watch for fireflies. A firefly covers a handful of pixels
    out of hundreds of thousands, so it barely moves noiseRSE, but it dominates
    this one.

    It has a floor that comes straight from its definition: it reports the pixel
    at rank 99.9%, so it only moves once the fireflies reach more than 0.1% of
    the image. Measured on synthetic fireflies at 640x360 and 1000 spp, with the
    outliers 3000 times an ordinary sample:

      firefly pixels   noiseRSE   noiseRSE999
              0.010%    0.00320       0.00338
              0.100%    0.00370       0.00342     still an ordinary pixel
              0.200%    0.00421       0.60547
              5.000%    0.02910       0.75061

    Below that threshold this figure reads as if the image were clean and only
    noiseRSE moves, by very little. It is the right figure for the Muttenzer
    Box, whose fireflies cover about 4% of the far wall (see _sampleClamp), but
    a scene with rarer ones needs a higher percentile or the maximum. */
    SLfloat noiseRSE999() const { return _noiseRSE999; }

    //! Monte Carlo efficiency, the inverse of variance times time
    /*! The figure to compare two renderers or two settings with, because it is
    the only one that stays fair when a change alters what a sample costs, as
    Russian roulette does. It does not depend on the sample count: the variance
    of a pixel falls as 1/N while the render time grows as N, so their product
    is constant in N and two renders at different samples per pixel can be
    compared directly. Higher is better. */
    SLfloat efficiency() const
    {
        return _noiseRSE > 0.0f && _renderSec > 0.0f
                 ? 1.0f / (_noiseRSE * _noiseRSE * _renderSec)
                 : 0.0f;
    }

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

    //! Sum and sum of squares of the luminance of every sample, per pixel
    /*! The two moments that the variance of a pixel is computed from, in
    computeNoise. Luminance rather than colour, because the noise of a pixel is
    one number and not three, and it is linear, so summing the luminance of the
    samples and taking the luminance of _radianceSum come to the same thing.

    They are double and not float, although _radianceSum is float, because the
    variance is the small difference of two large sums, S2 - S1*S1/N. In a quiet
    region those two agree to several digits and a float would leave almost none
    of the result. A firefly makes it worse from the other side: 1e4 squared is
    1e8, and adding an ordinary 1e-4 to that is lost entirely in float. Double
    carries both ends. The cost is 16 bytes per pixel, 3.7 MB at 640x480.

    Indexed like _radianceSum, with y * width + x. */
    vector<SLdouble> _lumSum;
    vector<SLdouble> _lumSumSq;

    SLfloat _noiseRSE;    //!< mean relative standard error, see noiseRSE()
    SLfloat _noiseRSE999; //!< the same for the worst 0.1%, see noiseRSE999()
};
//-----------------------------------------------------------------------------
#endif
