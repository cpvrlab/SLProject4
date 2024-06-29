/**
 * \file      SLNodeLOD.h
 * \date      July 2021
 * \authors   Jan Dellsperger, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLNODELOD_H
#define SLNODELOD_H

#include <SLNode.h>

//-----------------------------------------------------------------------------
//! Level of detail (LOD) group node based on screen space coverage
/*! An LOD group node can be used to improve the rendering performance for a
 mesh is very detailed and has thousands for vertices and triangles. Such a
 details mesh doesn't need to be detailed in full resolution if the mesh is
 displayed far away from the camera because you can see all triangles anyway.
 We therefore need to create multiple levels of details with lower no. of
 triangles and vertices. You have to create these lower resolution version of
 an original mesh in an external program such as Blender that has multiple
 decimation algorithms for this purpose.\n
 See the method addChildLOD for more information how to add the levels.
 */
class SLNodeLOD : public SLNode
{
public:
    explicit SLNodeLOD(const SLstring& name = "NodeLOD") : SLNode(name) { ; }

    void         addChildLOD(SLNode* child,
                             SLfloat minLodLimit,
                             SLubyte levelForSM = 0);
    virtual void cullChildren3D(SLSceneView* sv);
};
//-----------------------------------------------------------------------------
#endif