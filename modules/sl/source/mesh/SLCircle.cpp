/**
 * \file      SLCircle.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLCircle.h>

//-----------------------------------------------------------------------------
SLCircle::SLCircle(SLAssetManager* assetMgr, SLstring name, SLMaterial* material)
  : SLPolyline(assetMgr, name)
{
    SLint   circlePoints = 60;
    SLfloat deltaPhi     = Utils::TWOPI / (SLfloat)circlePoints;

    SLVVec3f points;
    for (SLint i = 0; i < circlePoints; ++i)
    {
        SLVec2f c;
        c.fromPolar(1.0f, i * deltaPhi);
        points.push_back(SLVec3f(c.x, c.y, 0));
    }

    buildMesh(points, true, material);
}
//-----------------------------------------------------------------------------