/**
 * \file      SLPoints.cpp
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLPoints.h>
#include <climits>

//-----------------------------------------------------------------------------
//! SLPoints ctor with a given vector of points
SLPoints::SLPoints(SLAssetManager* assetMgr,
                   const SLVVec3f& points,
                   const SLstring& name,
                   SLMaterial*     material) : SLMesh(assetMgr, name)
{
    assert(!name.empty());

    _primitive = PT_points;

    if (points.size() > UINT_MAX)
        SL_EXIT_MSG("SLPoints supports max. 2^32 vertices.");

    P = points;

    mat(material);
}
//-----------------------------------------------------------------------------
//! SLPoints ctor with a given vector of points
SLPoints::SLPoints(SLAssetManager* assetMgr,
                   const SLVVec3f& points,
                   const SLVVec3f& normals,
                   const SLstring& name,
                   SLMaterial*     material) : SLMesh(assetMgr, name)
{
    assert(!name.empty());

    _primitive = PT_points;

    if ((SLulong)points.size() > (SLulong)UINT_MAX)
        SL_EXIT_MSG("SLPoints supports max. 2^32 vertices.");
    if (points.size() != normals.size())
        SL_EXIT_MSG("SLPoints: different size of points and normals vector.");

    P = points;
    N = normals;

    mat(material);
}
//-----------------------------------------------------------------------------
//! SLPoints ctor for a random point cloud with the rnd generator.
SLPoints::SLPoints(SLAssetManager* assetMgr,
                   SLuint32        nPoints,
                   SLRnd3f&        rnd,
                   const SLstring& name,
                   SLMaterial*     material) : SLMesh(assetMgr, name)
{
    assert(!name.empty() && "No name provided in SLPoints!");

    _primitive = PT_points;

    for (int i = 0; i < (int)nPoints; ++i)
        P.push_back(rnd.generate());

    mat(material);
}
//-----------------------------------------------------------------------------
