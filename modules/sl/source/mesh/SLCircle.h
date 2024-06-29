/**
 * \file      SLCircle.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SL_CIRCLE_H
#define SL_CIRCLE_H

#include <SLPolyline.h>

//-----------------------------------------------------------------------------
class SLCircle : public SLPolyline
{
public:
    SLCircle(SLAssetManager* assetMgr, 
    SLstring name = "Circle", SLMaterial* material = nullptr);
};
//-----------------------------------------------------------------------------
#endif