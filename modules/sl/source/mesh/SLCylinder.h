/**
 * \file      SLCylinder.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLCYLINDER_H
#define SLCYLINDER_H

#include <SLRevolver.h>

//-----------------------------------------------------------------------------
/**
 * @brief SLCylinder is creates sphere mesh based on its SLRevolver methods
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * For this mesh it means that the objects for OpenGL drawing (_vao, _vaoP,
 * _vaoN, _vaoT and _vaoS) remain unused until the first frame is rendered.
 */
class SLCylinder : public SLRevolver
{
public:
    SLCylinder(SLAssetManager* assetMgr,
               SLfloat         cylinderRadius,
               SLfloat         cylinderHeight,
               SLuint          stacks    = 1,
               SLuint          slices    = 16,
               SLbool          hasTop    = true,
               SLbool          hasBottom = true,
               SLstring        name      = "cylinder mesh",
               SLMaterial*     mat       = nullptr);

    // Getters
    SLfloat radius() { return _radius; }
    SLfloat height() { return _height; }
    SLbool  hasTop() { return _hasTop; }
    SLbool  hasBottom() { return _hasBottom; }

private:
    SLfloat _radius;    //!< radius of cylinder
    SLfloat _height;    //!< height of cylinder
    SLbool  _hasTop;    //!< Flag if cylinder has a top
    SLbool  _hasBottom; //!< Flag if cylinder has a bottom
};
//-----------------------------------------------------------------------------
#endif // SLCYLINDER_H
