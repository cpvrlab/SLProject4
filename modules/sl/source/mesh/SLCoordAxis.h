/**
 * \file      SLCoordAxis.h
 * \date      July 2016
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLCOORDAXIS_H
#define SLCOORDAXIS_H

#include <SLEnums.h>
#include <SLMesh.h>

//-----------------------------------------------------------------------------
//! Axis aligned coordinate axis mesh
/*!
The SLAxis mesh draws axis aligned arrows to indicate the coordinate system.
All arrows have unit lenght. The arrow along x,y & z axis have the colors
red, green & blue.
*/
class SLCoordAxis : public SLMesh
{
public:
    explicit SLCoordAxis(SLAssetManager* assetMgr,
                         SLfloat         arrowThickness  = 0.05f,
                         SLfloat         arrowHeadLenght = 0.2f,
                         SLfloat         arrowHeadWidth  = 0.1f);

    void buildMesh();

private:
    SLfloat _arrowThickness;  //!< Thickness of the arrow
    SLfloat _arrowHeadLength; //!< Lenght of the arrow head
    SLfloat _arrowHeadWidth;  //!< Width of the arrow head
};
//-----------------------------------------------------------------------------
#endif
