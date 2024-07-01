/**
 * \file      SLSphere.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLSPHERE_H
#define SLSPHERE_H

#include <SLSpheric.h>

#include <utility>

class SLRay;
class SLMaterial;
class SLAssetManager;

//-----------------------------------------------------------------------------
/**
 * @brief SLSphere creates a sphere mesh based on SLSpheric w. 180 deg polar angle.
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * For this mesh it means that the objects for OpenGL drawing (_vao, _vaoP,
 * _vaoN, _vaoT and _vaoS) remain unused until the first frame is rendered.
 */
class SLSphere : public SLSpheric
{
public:
    explicit SLSphere(SLAssetManager* assetMgr,
                      SLfloat         radius,
                      SLuint          stacks = 32,
                      SLuint          slices = 32,
                      SLstring        name   = "sphere mesh",
                      SLMaterial*     mat    = nullptr) : SLSpheric(assetMgr,
                                                             radius,
                                                             0.0f,
                                                             180.0f,
                                                             stacks,
                                                             slices,
                                                             std::move(name),
                                                             mat)
    {
    }
};
//-----------------------------------------------------------------------------
#endif // SLSPHERE_H
