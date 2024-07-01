/**
 * \file      SLTriangle.h
 * \date      July 2014
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Philipp Jueni, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#ifndef SLTRIANGLE_H
#define SLTRIANGLE_H

#include <SLMesh.h>

//-----------------------------------------------------------------------------
/**
 * @brief A triangle class as the most simplest mesh
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * For this mesh it means that the objects for OpenGL drawing (_vao, _vaoP,
 * _vaoN, _vaoT and _vaoS) remain unused until the first frame is rendered.
 */
class SLTriangle : public SLMesh
{
public:
    SLTriangle(SLAssetManager* assetMgr,
               SLMaterial*     mat,
               const SLstring& name = "triangle mesh",
               const SLVec3f&  p0   = SLVec3f(0, 0, 0),
               const SLVec3f&  p1   = SLVec3f(1, 0, 0),
               const SLVec3f&  p2   = SLVec3f(0, 1, 0),
               const SLVec2f&  t0   = SLVec2f(0, 0),
               const SLVec2f&  t1   = SLVec2f(1, 0),
               const SLVec2f&  t2   = SLVec2f(0, 1));

    void buildMesh(SLMaterial* mat);

protected:
    SLVec3f p[3]; //!< Array of 3 vertex positions
    SLVec2f t[3]; //!< Array of 3 vertex tex. coords. (opt.)
};
//-----------------------------------------------------------------------------
#endif // SLTRIANGLE_H