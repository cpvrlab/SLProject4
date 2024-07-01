/**
 * \file      SLCone.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLCONE_H
#define SLCONE_H

#include <SLRevolver.h>

//-----------------------------------------------------------------------------
/**
 * @brief SLCone creates a cone mesh based on SLRevolver
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * For this mesh it means that the objects for OpenGL drawing (_vao, _vaoP,
 * _vaoN, _vaoT and _vaoS) remain unused until the first frame is rendered.
 */
class SLCone : public SLRevolver
{
public:
    SLCone(SLAssetManager* assetMgr,
           SLfloat         coneRadius,
           SLfloat         coneHeight,
           SLuint          stacks    = 36,
           SLuint          slices    = 36,
           SLbool          hasBottom = true,
           SLstring        name      = "cone mesh",
           SLMaterial*     mat       = nullptr);

    // Getters
    SLfloat radius() { return _radius; }
    SLfloat height() { return _height; }
    SLbool  hasBottom() { return _hasBottom; }

protected:
    SLfloat _radius;    //!< radius of cone
    SLfloat _height;    //!< height of cone
    SLbool  _hasBottom; //!< Flag if cone has a bottom
};
//-----------------------------------------------------------------------------
#endif // SLCONE_H
