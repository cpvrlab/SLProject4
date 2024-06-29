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
//! Single arrow for coordinate axis
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