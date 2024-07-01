/**
 * \file      SLBox.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLBOX_H
#define SLBOX_H

#include <SLEnums.h>
#include <SLMesh.h>

//-----------------------------------------------------------------------------
/**
 * @brief Axis aligned box mesh
 * @details The SLBox node draws an axis aligned box from a minimal corner to 
 * a maximal corner. If the minimal and maximal corner are swapped the normals 
 * will point inside.
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * For this SLMesh it means that the objects for OpenGL drawing (_vao, _vaoP,
 * _vaoN, _vaoT and _vaoS) remain unused until the first frame is rendered.
 */
class SLBox : public SLMesh
{
public:
    SLBox(SLAssetManager* assetMgr,
          SLfloat         minx = 0,
          SLfloat         miny = 0,
          SLfloat         minz = 0,
          SLfloat         maxx = 1,
          SLfloat         maxy = 1,
          SLfloat         maxz = 1,
          const SLstring& name = "box mesh",
          SLMaterial*     mat  = nullptr);
    SLBox(SLAssetManager* assetMgr,
          const SLVec3f&  min,
          const SLVec3f&  max,
          const SLstring& name = "box mesh",
          SLMaterial*     mat  = nullptr);

    void buildMesh(SLMaterial* mat);

private:
    SLVec3f _min; //!< minimal corner
    SLVec3f _max; //!< maximum corner
};
//-----------------------------------------------------------------------------
#endif
