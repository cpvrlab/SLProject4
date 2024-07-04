/**
 * \file      SLPolyline.h
 * \date      July 2016
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPOLYLINE_H
#define SLPOLYLINE_H

#include <SLMesh.h>

#include <utility>

//-----------------------------------------------------------------------------
//! SLPolyline creates a polyline object
/*!
The SLPolyline node draws a polyline object
*/
class SLPolyline : public SLMesh
{
public:
    explicit SLPolyline(SLAssetManager* assetMgr,
                        const SLstring& name = "Polyline") : SLMesh(assetMgr, name){};

    //! ctor for polyline with a vector of points
    SLPolyline(SLAssetManager* assetMgr,
               const SLVVec3f& points,
               SLbool          closed   = false,
               const SLstring& name     = "Polyline",
               SLMaterial*     material = nullptr) : SLMesh(assetMgr, name)
    {
        buildMesh(points, closed, material);
    }

    void buildMesh(const SLVVec3f& points,
                   SLbool          closed   = false,
                   SLMaterial*     material = nullptr)
    {
        assert(points.size() > 1);
        P          = points;
        _primitive = closed ? PT_lineLoop : PT_lines;
        mat(material);
        if (P.size() < 65535)
            for (SLuint i = 0; i < P.size(); ++i)
                I16.push_back((SLushort)i);
        else
            for (SLuint i = 0; i < P.size(); ++i)
                I32.push_back(i);
    }
};
//-----------------------------------------------------------------------------
#endif
