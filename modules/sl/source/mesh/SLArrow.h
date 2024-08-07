/**
 * \file      SLArrow.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLARROW_H
#define SLARROW_H

#include <SLRevolver.h>

//-----------------------------------------------------------------------------
/**
 * @brief SLArrow is creates an arrow mesh based on its SLRevolver methods
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * For this SLMesh it means that the objects for OpenGL drawing (_vao, _vaoP,
 * _vaoN, _vaoT and _vaoS) remain unused until the first frame is rendered.
 */
class SLArrow : public SLRevolver
{
public:
    SLArrow(SLAssetManager* assetMgr,
            SLfloat         arrowCylinderRadius,
            SLfloat         length,
            SLfloat         headLength,
            SLfloat         headWidth,
            SLuint          slices,
            const SLstring& name = "arrow mesh",
            SLMaterial*     mat  = nullptr) : SLRevolver(assetMgr, name)
    {
        assert(slices >= 3 && "Error: Not enough slices.");
        assert(headLength < length);
        assert(headWidth > arrowCylinderRadius);

        _radius     = arrowCylinderRadius;
        _length     = length;
        _headLength = headLength;
        _headWidth  = headWidth;
        _slices     = slices;
        _revAxis.set(0, 0, 1);

        // Add revolving polyline points with double points for sharp edges
        _revPoints.reserve(8);
        _revPoints.push_back(SLVec3f(0, 0, 0));
        _revPoints.push_back(SLVec3f(_headWidth, 0, _headLength));
        _revPoints.push_back(SLVec3f(_headWidth, 0, _headLength));
        _revPoints.push_back(SLVec3f(_radius, 0, _headLength));
        _revPoints.push_back(SLVec3f(_radius, 0, _headLength));
        _revPoints.push_back(SLVec3f(_radius, 0, _length));
        _revPoints.push_back(SLVec3f(_radius, 0, _length));
        _revPoints.push_back(SLVec3f(0, 0, _length));

        buildMesh(mat);
    }

private:
    SLfloat _radius;     //!< radius of arrow cylinder
    SLfloat _length;     //!< length of arrow
    SLfloat _headLength; //!< length of arrow head
    SLfloat _headWidth;  //!< width of arrow head
};
//-----------------------------------------------------------------------------
#endif // SLARROW_H
