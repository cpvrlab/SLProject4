/**
 * \file      SLGrid.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLGRID_H
#define SLGRID_H

#include <SLMesh.h>

//-----------------------------------------------------------------------------
//! SLGrid creates a rectangular grid with lines with a certain resolution
/*! The SLGrid mesh draws a grid between a minimal and a maximal corner in the
XZ-plane.
*/
class SLGrid : public SLMesh
{
public:
    //! ctor for rectangle w. min & max corner
    SLGrid(SLAssetManager* assetMgr,
           SLVec3f         minXZ,
           SLVec3f         maxXZ,
           SLuint          resX,
           SLuint          resZ,
           SLstring        name = "grid mesh",
           SLMaterial*     mat  = nullptr);

    void buildMesh(SLMaterial* mat);

protected:
    SLVec3f _min;  //!< min corner
    SLVec3f _max;  //!< max corner
    SLuint  _resX; //!< resolution in x direction
    SLuint  _resZ; //!< resolution in z direction
};
//-----------------------------------------------------------------------------
#endif
