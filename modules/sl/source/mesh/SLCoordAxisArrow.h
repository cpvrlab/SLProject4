/**
 * \file      SLCoordAxisArrow.h
 * \date      April 2020
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Jan Dellsperger, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#ifndef SLCOORDAXISARROW_H
#define SLCOORDAXISARROW_H

#include <SLMesh.h>

//-----------------------------------------------------------------------------
/**
 * @brief  Single arrow for coordinate axis
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * For this mesh it means that the objects for OpenGL drawing (_vao, _vaoP,
 * _vaoN, _vaoT and _vaoS) remain unused until the first frame is rendered.
 */
class SLCoordAxisArrow : public SLMesh
{
public:
    explicit SLCoordAxisArrow(SLAssetManager* assetMgr,
                              SLMaterial*     material        = nullptr,
                              SLfloat         arrowThickness  = 0.05f,
                              SLfloat         arrowHeadLenght = 0.2f,
                              SLfloat         arrowHeadWidth  = 0.1f);

    void buildMesh(SLMaterial* material = nullptr);

private:
    SLfloat _arrowThickness;  //!< Thickness of the arrow
    SLfloat _arrowHeadLength; //!< Lenght of the arrow head
    SLfloat _arrowHeadWidth;  //!< Width of the arrow head
};
//-----------------------------------------------------------------------------

#endif