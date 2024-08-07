/**
 * \file      SLSpheric.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLSPHERIC_H
#define SLSPHERIC_H

#include <SLRevolver.h>

class SLRay;
class SLMaterial;

//-----------------------------------------------------------------------------
//! SLSphere creates a sphere mesh based on SLRevolver
class SLSpheric : public SLRevolver
{
public:
    SLSpheric(SLAssetManager* assetMgr,
              SLfloat         radius,
              SLfloat         thetaStartDEG,
              SLfloat         thetaEndDEG,
              SLuint          stacks = 32,
              SLuint          slices = 32,
              SLstring        name   = "spheric mesh",
              SLMaterial*     mat    = nullptr);

    // Getters
    SLfloat radius() { return _radius; }

protected:
    SLfloat _radius;        //!< radius of the sphere
    SLfloat _thetaStartDEG; //!< Polar start angle 0-180deg
    SLfloat _thetaEndDEG;   //!< Polar end angle 1-180deg
};
//-----------------------------------------------------------------------------
#endif // SLSPHERIC_H
