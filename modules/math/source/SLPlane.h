/**
 * \file      math/SLPlane.h
 * \brief     Declaration of a 3D plane class
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPLANE_H
#define SLPLANE_H

#include <SLMath.h>
#include <SLVec3.h>

//-----------------------------------------------------------------------------
//! Defines a plane in 3D space with the equation ax + by + cy + d = 0
/*!
SLPlane defines a plane in 3D space with the equation ax + by + cy + d = 0
where [a,b,c] is the plane normal and d is the distance from [0,0,0]. The class
is used to define the 6 planes of the view frustum.
*/
class SLPlane
{
public:
    SLPlane(const SLVec3f& v1,
            const SLVec3f& v2,
            const SLVec3f& v3);
    SLPlane()
    {
        N.set(0, 0, 1);
        d = 0.0f;
    }
    ~SLPlane() { ; }

    SLVec3f N; //!< plane normal
    SLfloat d; //!< d = -(ax+by+cy) = -normal.dot(point)

    void setPoints(const SLVec3f& v1,
                   const SLVec3f& v2,
                   const SLVec3f& v3);
    void setNormalAndPoint(const SLVec3f& N,
                           const SLVec3f& P);
    void setCoefficients(const SLfloat A,
                         const SLfloat B,
                         const SLfloat C,
                         const SLfloat D);

    //! Returns distance between a point P and the plane
    inline SLfloat distToPoint(const SLVec3f& p) { return (d + N.dot(p)); }

    void print(const char* name);
};
//-----------------------------------------------------------------------------
#endif
