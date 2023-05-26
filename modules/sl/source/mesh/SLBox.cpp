//#############################################################################
//  File:      SLBox.cpp
//  Date:      July 2014
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <SLBox.h>
#include <SLMaterial.h>

//-----------------------------------------------------------------------------
//! SLBox::SLBox ctor with min. & max. coords. of the axis aligned box
SLBox::SLBox(SLAssetManager* assetMgr,
             SLfloat         minx,
             SLfloat         miny,
             SLfloat         minz,
             SLfloat         maxx,
             SLfloat         maxy,
             SLfloat         maxz,
             const SLstring& name,
             SLMaterial*     mat) : SLMesh(assetMgr, name)
{
    _min.set(minx, miny, minz);
    _max.set(maxx, maxy, maxz);
    SLVec3f diag(_max - _min);
    assert(diag.length() > FLT_EPSILON && "Box has no size!");
    buildMesh(mat);
}
//-----------------------------------------------------------------------------
//! SLBox::SLBox ctor with min. & max. vectors of the axis aligned box
SLBox::SLBox(SLAssetManager* assetMgr,
             const SLVec3f&  min,
             const SLVec3f&  max,
             const SLstring& name,
             SLMaterial*     mat) : SLMesh(assetMgr, name)
{
    _min.set(min);
    _max.set(max);
    SLVec3f diag(_max - _min);
    assert(diag.length() > FLT_EPSILON && "Box has no size!");
    buildMesh(mat);
}
//-----------------------------------------------------------------------------
//! SLBox::buildMesh fills in the underlying arrays from the SLMesh object
void SLBox::buildMesh(SLMaterial* material)
{
    deleteData();

    // allocate new vectors of SLMesh
    P.resize(24);       // 6 sides with 4 vertices
    I16.resize(12 * 3); // 6 sides with 2 triangles * 3 indices
    N.resize(P.size());

    // Set one default material index
    mat(material);

    SLuint p = 0, i = 0;

    // predefined normals
    SLVec3f NX = SLVec3f(1, 0, 0);
    SLVec3f NY = SLVec3f(0, 1, 0);
    SLVec3f NZ = SLVec3f(0, 0, 1);

    // clang-format off
    // towards +x
    P[p].x=_max.x; P[p].y=_max.y; P[p].z=_max.z; N[p]= NX; p++;  //  0
    P[p].x=_max.x; P[p].y=_min.y; P[p].z=_max.z; N[p]= NX; p++;  //  1
    P[p].x=_max.x; P[p].y=_min.y; P[p].z=_min.z; N[p]= NX; p++;  //  2
    P[p].x=_max.x; P[p].y=_max.y; P[p].z=_min.z; N[p]= NX; p++;  //  3
    I16[i++] = 0; I16[i++] = 1; I16[i++] = 2;
    I16[i++] = 0; I16[i++] = 2; I16[i++] = 3;
   
    // towards -z
    P[p].x=_max.x; P[p].y=_max.y; P[p].z=_min.z; N[p]=-NZ; p++;  //  4
    P[p].x=_max.x; P[p].y=_min.y; P[p].z=_min.z; N[p]=-NZ; p++;  //  5
    P[p].x=_min.x; P[p].y=_min.y; P[p].z=_min.z; N[p]=-NZ; p++;  //  6
    P[p].x=_min.x; P[p].y=_max.y; P[p].z=_min.z; N[p]=-NZ; p++;  //  7
    I16[i++] = 4; I16[i++] = 5; I16[i++] = 6;
    I16[i++] = 4; I16[i++] = 6; I16[i++] = 7;
   
    // towards -x
    P[p].x=_min.x; P[p].y=_max.y; P[p].z=_max.z; N[p]=-NX; p++;  //  8
    P[p].x=_min.x; P[p].y=_max.y; P[p].z=_min.z; N[p]=-NX; p++;  //  9
    P[p].x=_min.x; P[p].y=_min.y; P[p].z=_min.z; N[p]=-NX; p++;  // 10
    P[p].x=_min.x; P[p].y=_min.y; P[p].z=_max.z; N[p]=-NX; p++;  // 11
    I16[i++] = 8; I16[i++] = 9;  I16[i++] = 10;
    I16[i++] = 8; I16[i++] = 10; I16[i++] = 11;
   
    // towards +z
    P[p].x=_max.x; P[p].y=_max.y; P[p].z=_max.z; N[p]= NZ; p++;  // 12
    P[p].x=_min.x; P[p].y=_max.y; P[p].z=_max.z; N[p]= NZ; p++;  // 13
    P[p].x=_min.x; P[p].y=_min.y; P[p].z=_max.z; N[p]= NZ; p++;  // 14
    P[p].x=_max.x; P[p].y=_min.y; P[p].z=_max.z; N[p]= NZ; p++;  // 15
    I16[i++] = 12; I16[i++] = 13; I16[i++] = 14;
    I16[i++] = 12; I16[i++] = 14; I16[i++] = 15;
   
    // towards +y
    P[p].x=_max.x; P[p].y=_max.y; P[p].z=_max.z; N[p]= NY; p++;  // 16
    P[p].x=_max.x; P[p].y=_max.y; P[p].z=_min.z; N[p]= NY; p++;  // 17
    P[p].x=_min.x; P[p].y=_max.y; P[p].z=_min.z; N[p]= NY; p++;  // 18
    P[p].x=_min.x; P[p].y=_max.y; P[p].z=_max.z; N[p]= NY; p++;  // 19
    I16[i++] = 16; I16[i++] = 17; I16[i++] = 18;
    I16[i++] = 16; I16[i++] = 18; I16[i++] = 19;
   
    // towards -y
    P[p].x=_min.x; P[p].y=_min.y; P[p].z=_max.z; N[p]=-NY; p++;  // 20
    P[p].x=_min.x; P[p].y=_min.y; P[p].z=_min.z; N[p]=-NY; p++;  // 21
    P[p].x=_max.x; P[p].y=_min.y; P[p].z=_min.z; N[p]=-NY; p++;  // 22
    P[p].x=_max.x; P[p].y=_min.y; P[p].z=_max.z; N[p]=-NY;       // 23
    I16[i++] = 20; I16[i++] = 21; I16[i++] = 22;
    I16[i++] = 20; I16[i++] = 22; I16[i] = 23;
}
//-----------------------------------------------------------------------------

