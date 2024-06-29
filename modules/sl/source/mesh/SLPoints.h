/**
 * \file      SLPoints.h
 * \date      October 2017
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPOINTS_H
#define SLPOINTS_H

#include <SLMesh.h>
#include <SLRnd3f.h>

//-----------------------------------------------------------------------------
//! SLPoints creates
/*! The SLPoints mesh object of witch the vertices are drawn as points.
 */
class SLPoints : public SLMesh
{
public:
    //! Ctor for a given vector of points
    SLPoints(SLAssetManager* assetMgr,
             const SLVVec3f& points,
             const SLstring& name     = "point cloud",
             SLMaterial*     material = nullptr);
    SLPoints(SLAssetManager* assetMgr,
             const SLVVec3f& points,
             const SLVVec3f& normals,
             const SLstring& name     = "point cloud",
             SLMaterial*     material = 0);

    //! Ctor for a random point cloud.
    SLPoints(SLAssetManager* assetMgr,
             SLuint32        nPoints,
             SLRnd3f&        rnd,
             const SLstring& name = "normal point cloud",
             SLMaterial*     mat  = nullptr);
};
//-----------------------------------------------------------------------------
#endif
