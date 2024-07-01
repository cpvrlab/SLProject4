/**
 * \file      SLRevolver.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLREVOLVER_H
#define SLREVOLVER_H

#include <SLMesh.h>

#include <utility>

class SLAssetManager;

//-----------------------------------------------------------------------------
/**
 * @brief SLRevolver is an SLMesh object built out of revolving points.
 * @details SLRevolver is an SLMesh object that is built out of points that 
 * are revolved in slices around and axis. The surface will be outwards if the 
 * points in the array _revPoints increase towards the axis direction. If all 
 * points in the array _revPoints are different the normals will be smoothed. 
 * If two consecutive points are identical the normals will define a hard edge. 
 * Texture coords. are cylindrically mapped. See the online example 
 * \subpage example-revolver with various revolver objects.
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * For this mesh it means that the objects for OpenGL drawing (_vao, _vaoP,
 * _vaoN, _vaoT and _vaoS) remain unused until the first frame is rendered.
 */
class SLRevolver : public SLMesh
{
public:
    //! ctor for generic revolver mesh
    SLRevolver(SLAssetManager* assetMgr,
               SLVVec3f        revolvePoints,
               SLVec3f         revolveAxis,
               SLuint          slices      = 36,
               SLbool          smoothFirst = false,
               SLbool          smoothLast  = false,
               SLstring        name        = "revolver mesh",
               SLMaterial*     mat         = nullptr);

    //! ctor for derived revolver shapes
    SLRevolver(SLAssetManager* assetMgr, SLstring name) : SLMesh(assetMgr, std::move(name)) { ; }

    void   buildMesh(SLMaterial* mat = nullptr);
    SLuint stacks() { return _stacks; }
    SLuint slices() { return _slices; }

protected:
    SLVVec3f _revPoints; //!< Array revolving points
    SLVec3f  _revAxis;   //!< axis of revolution
    SLuint   _stacks;    //!< No. of stacks (mostly used)
    SLuint   _slices;    //!< NO. of slices

    //! flag if the normal of the first point is eqaual to -revAxis
    SLbool _smoothFirst;

    //! flag if the normal of the last point is eqaual to revAxis
    SLbool _smoothLast;
};
//-----------------------------------------------------------------------------
#endif // SLREVOLVER_H
