/**
 * \file      SLCone.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLCONE_H
#define SLCONE_H

#include <SLRevolver.h>

//-----------------------------------------------------------------------------
//! SLCone creates a cone mesh based on SLRevolver
class SLCone : public SLRevolver
{
public:
    SLCone(SLAssetManager* assetMgr,
           SLfloat         coneRadius,
           SLfloat         coneHeight,
           SLuint          stacks    = 36,
           SLuint          slices    = 36,
           SLbool          hasBottom = true,
           SLstring        name      = "cone mesh",
           SLMaterial*     mat       = nullptr);

    // Getters
    SLfloat radius() { return _radius; }
    SLfloat height() { return _height; }
    SLbool  hasBottom() { return _hasBottom; }

protected:
    SLfloat _radius;    //!< radius of cone
    SLfloat _height;    //!< height of cone
    SLbool  _hasBottom; //!< Flag if cone has a bottom
};
//-----------------------------------------------------------------------------
#endif // SLCONE_H
