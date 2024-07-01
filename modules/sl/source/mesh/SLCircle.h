/**
 * \file      SLCircle.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SL_CIRCLE_H
#define SL_CIRCLE_H

#include <SLPolyline.h>

//-----------------------------------------------------------------------------
/**
 * @brief Circle line mesh derived from SLPolyline
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * For this mesh it means that the objects for OpenGL drawing (_vao, _vaoP,
 * _vaoN, _vaoT and _vaoS) remain unused until the first frame is rendered.
 */
class SLCircle : public SLPolyline
{
public:
    SLCircle(SLAssetManager* assetMgr, 
    SLstring name = "Circle", SLMaterial* material = nullptr);
};
//-----------------------------------------------------------------------------
#endif