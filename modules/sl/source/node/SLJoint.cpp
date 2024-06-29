/**
 * \file      SLJoint.cpp
 * \date      Autumn 2014
 * \authors   Marc Wacker, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLJoint.h>
#include <SLAnimSkeleton.h>

//-----------------------------------------------------------------------------
/*! Constructor
 */
SLJoint::SLJoint(SLuint id, SLAnimSkeleton* creator)
  : SLNode("Unnamed Joint"),
    _id(id),
    _skeleton(creator),
    _radius(0)
{
}
//-----------------------------------------------------------------------------
/*! Constructor
 */
SLJoint::SLJoint(const SLstring& name, SLuint id, SLAnimSkeleton* creator)
  : SLNode(name), _id(id), _skeleton(creator), _radius(0)
{
}
//-----------------------------------------------------------------------------
/*! Creation function to create a new child joint for this joint.
 */
SLJoint* SLJoint::createChild(SLuint id)
{
    SLJoint* joint = _skeleton->createJoint(id);
    addChild(joint);
    return joint;
}
//-----------------------------------------------------------------------------
/*! Creation function to create a new child joint for this joint.
 */
SLJoint* SLJoint::createChild(const SLstring& name, SLuint id)
{
    SLJoint* joint = _skeleton->createJoint(name, id);
    addChild(joint);
    return joint;
}
//-----------------------------------------------------------------------------
/*! Updates the current max radius with the input vertex position in joint space.
 */
void SLJoint::calcMaxRadius(const SLVec3f& vec)
{
    SLVec3f boneSpaceVec = _offsetMat * vec;
    _radius              = std::max(_radius, boneSpaceVec.length());
}
//-----------------------------------------------------------------------------
/*! Getter that calculates the final joint transform matrix.
 */
SLMat4f SLJoint::calcFinalMat()
{
    return updateAndGetWM() * _offsetMat;
}
//-----------------------------------------------------------------------------
/*! Getter that calculates the final joint transform matrix.
 */
void SLJoint::needUpdate()
{
    SLNode::needUpdate();

    // a joint must always know it's creator
    assert(_skeleton && "Joint didn't have a valid creator");
    _skeleton->changed(true);
}
//-----------------------------------------------------------------------------
