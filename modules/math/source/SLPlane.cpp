/**
 * \file      math/SLPlane.cpp
 * \brief     Implementation of the the SLPlane class
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLPlane.h>
//-----------------------------------------------------------------------------
/*!
SLPlane::SLPlane ctor with 3 points
*/
SLPlane::SLPlane(const SLVec3f& v0,
                 const SLVec3f& v1,
                 const SLVec3f& v2)
{
    setPoints(v0, v1, v2);
}
//-----------------------------------------------------------------------------
/*!
SLPlane::setFromPoints set the plane from 3 points
*/
void SLPlane::setPoints(const SLVec3f& v0,
                        const SLVec3f& v1,
                        const SLVec3f& v2)
{
    SLVec3f edge1(v1 - v0);
    SLVec3f edge2(v2 - v0);
    N.cross(edge1, edge2);
    N.normalize();
    d = -N.dot(v0);
}
//-----------------------------------------------------------------------------
/*!
SLPlane::setByNormalAndPoint defines the plane by a normal N and a point P
*/
void SLPlane::setNormalAndPoint(const SLVec3f& normal, const SLVec3f& P)
{
    N.set(normal);
    N.normalize();
    d = -N.dot(P);
}
//-----------------------------------------------------------------------------
/*!
SLPlane::setByCoefficients defines the plane by the coefficient A,B,C & D
*/
void SLPlane::setCoefficients(const SLfloat A,
                              const SLfloat B,
                              const SLfloat C,
                              SLfloat       D)
{
    // set the normal vector
    N.set(A, B, C);

    //compute the lenght of the vector
    SLfloat len = N.length();

    // normalize the vector
    N.set(N.x / len, N.y / len, N.z / len);

    // and divide d by th length as well
    d = D / len;
}
//-----------------------------------------------------------------------------
/*!
SLPlane::print prints the normal and the coefficent d
*/
void SLPlane::print(const char* name)
{
    SLMATH_LOG("Plane(%s: a=%4.3f, b=%4.3f, c=%4.3f)",
               name,
               N.x,
               N.y,
               N.z);
}
//-----------------------------------------------------------------------------
