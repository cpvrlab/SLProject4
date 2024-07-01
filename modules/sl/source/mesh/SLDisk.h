/**
 * \file      SLDisk.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLDISK_h
#define SLDISK_h

#include <SLRevolver.h>

//-----------------------------------------------------------------------------
/**
 * @brief SLDisk creates a disk mesh based on SLRevolver
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * For this mesh it means that the objects for OpenGL drawing (_vao, _vaoP,
 * _vaoN, _vaoT and _vaoS) remain unused until the first frame is rendered.
 */
class SLDisk : public SLRevolver
{
public:
    SLDisk(SLAssetManager* assetMgr,
           SLfloat         radius      = 1.0f,
           const SLVec3f&  revolveAxis = SLVec3f::AXISY,
           SLuint          slices      = 36,
           SLbool          doubleSided = false,
           SLstring        name        = "disk mesh",
           SLMaterial*     mat         = nullptr);

    // Getters
    SLfloat radius() { return _radius; }

protected:
    SLfloat _radius;      //!< radius of cone
    SLbool  _doubleSided; //!< flag if disk has two sides
};
//-----------------------------------------------------------------------------
#endif // SLDISK_h
