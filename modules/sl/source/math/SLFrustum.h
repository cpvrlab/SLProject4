/**
 * \file      SLFrustum.h
 * \authors   Luc Girod
 * \date      Summer 2021
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLFRUSTUM
#define SLFRUSTUM

//-----------------------------------------------------------------------------
//! Matrix to 6 frustum plane conversion functions
class SLFrustum
{
public:
    static void viewToFrustumPlanes(SLPlane*       planes,
                                    const SLMat4f& projectionMat,
                                    const SLMat4f& viewMat);
    static void viewToFrustumPlanes(SLPlane*       planes,
                                    const SLMat4f& A);
    static void getPointsInViewSpace(SLVec3f* points,
                                     float    fovV,
                                     float    ratio,
                                     float    clipNear,
                                     float    clipFar);
};
//-----------------------------------------------------------------------------
#endif
