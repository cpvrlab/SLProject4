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
//! SLDisk creates a disk mesh based on SLRevolver
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
