/**
 * \file      SLPolygon.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPOLYGON_H
#define SLPOLYGON_H

#include <SLMesh.h>

//-----------------------------------------------------------------------------
//! SLPolygon creates a convex polyon mesh
/*!
The SLPolygon node draws a convex polygon with. The normale vector is
calculated from the first 3 vertices.
*/
class SLPolygon : public SLMesh
{
public:
    //! ctor for generic convex polygon
    SLPolygon(SLAssetManager* assetMgr,
              const SLVVec3f& corner,
              const SLstring& name = "polygon mesh",
              SLMaterial*     mat  = nullptr);

    //! ctor for generic convex polygon with texCoords
    SLPolygon(SLAssetManager* assetMgr,
              const SLVVec3f& corners,
              const SLVVec2f& texcoords,
              const SLstring& name = "polygon mesh",
              SLMaterial*     mat  = nullptr);

    //! ctor for centered rectangle in x-y-plane (N=-z)
    SLPolygon(SLAssetManager* assetMgr,
              SLfloat         width,
              SLfloat         height,
              const SLstring& name,
              SLMaterial*     mat = nullptr);

    void buildMesh(SLMaterial* mat);

protected:
    SLVVec3f _corners; //!< corners in ccw order
    SLVVec2f _uv1;     //!< texture coords for corners
};
//-----------------------------------------------------------------------------
#endif
