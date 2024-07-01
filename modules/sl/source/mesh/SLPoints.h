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
/**
 * @brief The SLPoints mesh object of witch the vertices are drawn as points.
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * For this mesh it means that the objects for OpenGL drawing (_vao, _vaoP,
 * _vaoN, _vaoT and _vaoS) remain unused until the first frame is rendered.
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
