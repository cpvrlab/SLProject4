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
    SLCol4f trace(SLRay* ray, SLbool em);
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
};
//-----------------------------------------------------------------------------
#endif
