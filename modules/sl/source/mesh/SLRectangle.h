/**
 * \file      SLRectangle.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLRECTANGLE_H
#define SLRECTANGLE_H

#include <SLMesh.h>

//-----------------------------------------------------------------------------
//! SLRectangle creates a rectangular mesh with a certain resolution
/*!
The SLRectangle node draws a rectangle with a minimal and a maximal corner in
the x-y-plane. The normal is [0,0,1].
*/
class SLRectangle : public SLMesh
{
public:
    //! ctor for rectangle w. min & max corner
    SLRectangle(SLAssetManager* assetMgr,
                const SLVec2f&  min,
                const SLVec2f&  max,
                SLuint          resX,
                SLuint          resY,
                const SLstring& name = "rectangle mesh",
                SLMaterial*     mat  = nullptr);

    //! ctor for rectangle w. min & max corner & tex. coord.
    SLRectangle(SLAssetManager* assetMgr,
                const SLVec2f&  min,
                const SLVec2f&  max,
                const SLVec2f&  uv_min,
                const SLVec2f&  uv_max,
                SLuint          resX,
                SLuint          resY,
                const SLstring& name = "rectangle mesh",
                SLMaterial*     mat  = nullptr);

    void buildMesh(SLMaterial* mat);

protected:
    SLVec3f _min;    //!< min corner
    SLVec3f _max;    //!< max corner
    SLVec2f _uv_min; //!< min corner tex.coord.
    SLVec2f _uv_max; //!< max corner tex.coord.
    SLuint  _resX;   //!< resolution in x direction
    SLuint  _resY;   //!< resolution in y direction
};
//-----------------------------------------------------------------------------
#endif
