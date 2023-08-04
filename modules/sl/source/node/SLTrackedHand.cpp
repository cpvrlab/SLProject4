//#############################################################################
//  File:      SLTrackedHand.cpp
//  Date:      July 2023
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <SLTrackedHand.h>

#include <SLAnimSkeleton.h>

//-----------------------------------------------------------------------------
const std::vector<std::tuple<mp_hand_landmark, mp_hand_landmark, std::string>> ELEMENTS =
  {
    {mp_hand_landmark_thumb_cmc, mp_hand_landmark_thumb_mcp, "thumb1"},
    {mp_hand_landmark_thumb_mcp, mp_hand_landmark_thumb_ip, "thumb2"},
    {mp_hand_landmark_thumb_ip, mp_hand_landmark_thumb_tip, "thumb3"},
    {mp_hand_landmark_index_finger_mcp, mp_hand_landmark_index_finger_pip, "index1"},
    {mp_hand_landmark_index_finger_pip, mp_hand_landmark_index_finger_dip, "index2"},
    {mp_hand_landmark_index_finger_dip, mp_hand_landmark_index_finger_tip, "index3"},
    {mp_hand_landmark_middle_finger_mcp, mp_hand_landmark_middle_finger_pip, "middle1"},
    {mp_hand_landmark_middle_finger_pip, mp_hand_landmark_middle_finger_dip, "middle2"},
    {mp_hand_landmark_middle_finger_dip, mp_hand_landmark_middle_finger_tip, "middle3"},
    {mp_hand_landmark_ring_finger_mcp, mp_hand_landmark_ring_finger_pip, "ring1"},
    {mp_hand_landmark_ring_finger_pip, mp_hand_landmark_ring_finger_dip, "ring2"},
    {mp_hand_landmark_ring_finger_dip, mp_hand_landmark_ring_finger_tip, "ring3"},
    {mp_hand_landmark_pinky_mcp, mp_hand_landmark_pinky_pip, "pinky1"},
    {mp_hand_landmark_pinky_pip, mp_hand_landmark_pinky_dip, "pinky2"},
    {mp_hand_landmark_pinky_dip, mp_hand_landmark_pinky_tip, "pinky3"},
};
//-----------------------------------------------------------------------------
/*! Constructor that adds the model node to its children
 @param name Node name
 @param tracker Tracker that generates the 3D positions of the landmarks
 @param model Oculus left hand model that the tracking is applied to
 */
SLTrackedHand::SLTrackedHand(const SLstring&          name,
                             CVTrackedMediaPipeHands* tracker,
                             SLNode*                  model)
  : SLNode(name),
    _tracker(tracker),
    _skeleton(model->findChild<SLNode>("hands:Lhand")->skeleton())
{
    addChild(model);
}
//-----------------------------------------------------------------------------
// Applies the tracked points the 3D hand model
void SLTrackedHand::doUpdate()
{
    // You can find the MediaPipe hand model here:
    // https://developers.google.com/mediapipe/solutions/vision/hand_landmarker#models

    auto& results = _tracker->results();
    auto& points  = results[0];

    /*
     * COMPUTE HAND ORIENTATION
     *
     * The orientation of the hand is computed using the points of the palm.
     * The vector from the wrist to the root of the middle finger is used as the
     * Y-axis as it points almost perfectly upwards in the model.
     * The cross product between this vector and the vector from the wrist to the
     * root of the ring finger is used as the Z-axis.
     * Finally, the cross product between thes Z-axis and Y-axis is taken to
     * compute the X-axis.
     * From these three vectors, a world matrix is constructed and applied to
     * the root joint of the skeleton.
     */

    // Compute positions of the wrist, the middle finger root and the ring finger root.
    SLVec3f wp = convertMP2SL(points[mp_hand_landmark_wrist]);
    SLVec3f mp = convertMP2SL(points[mp_hand_landmark_middle_finger_mcp]);
    SLVec3f rp = convertMP2SL(points[mp_hand_landmark_ring_finger_mcp]);

    // Compute the vectors from the wrist to the fingers.
    SLVec3f w2m = (mp - wp).normalized();
    SLVec3f w2r = (rp - wp).normalized();

    // Compute the Y-axis of the world matrix.
    SLVec3f axisY = w2m;

    // Compute the Z-axis of the world matrix.
    SLVec3f axisZ;
    axisZ.cross(w2m, w2r);
    axisZ.normalize();

    // Compute the X-axis of the world matrix.
    SLVec3f axisX;
    axisX.cross(axisZ, axisY);
    axisX.normalize();

    // Find the root hand joint.
    SLNode* rootNode = _skeleton->getJoint("hands:b_l_hand");

    // Construct the world matrix, convert it to an object matrix and apply it.
    // axisX and axisZ are swapped because the joint in the 3D model have unusual orientations.
    SLMat4f wm = rootNode->updateAndGetWM();
    wm.m(0, axisZ.x);
    wm.m(1, axisZ.y);
    wm.m(2, axisZ.z);
    wm.m(4, axisY.x);
    wm.m(5, axisY.y);
    wm.m(6, axisY.z);
    wm.m(8, axisX.x);
    wm.m(9, axisX.y);
    wm.m(10, axisX.z);
    SLMat4f om = rootNode->parent()->updateAndGetWMI() * wm;
    rootNode->om(om);

    /*
     * COMPUTE FINGER ORIENTATION
     *
     * To compute the finger rotation, we use the vector from each joint to
     * its child. First, the finger is reset to its initial rotation from the
     * 3D model.  Next, the direction of each joint to its child is computed.
     * This direction is used to construct a rotation from the old direction
     * to the new direction. Finally, the parent node is rotated using this
     * rotation.
     *
     * We don't set the rotation directly instead of rotating from the old
     * direction to the new one because this might twist the fingers weirdly.
     * By resetting the fingers to their initial rotation, we make sure that
     * the shortest rotation from the neutral pose is performed, which looks
     * quite natural most of the time.
     */

    // Iterate over the finger joints.
    for (auto& element : ELEMENTS)
    {
        // Find the parent node.
        SLstring name       = "hands:b_l_" + std::get<2>(element);
        SLNode*  parentNode = _skeleton->getJoint(name);

        // Reset the parent node to its initial rotation.
        parentNode->om(parentNode->initialOM());
        SLVec3f oldDir = parentNode->axisYWS();

        // Compute the direction from the parent to its child.
        SLVec3f parentPos = convertMP2SL(points[std::get<0>(element)]);
        SLVec3f childPos  = convertMP2SL(points[std::get<1>(element)]);
        SLVec3f newDir    = (childPos - parentPos).normalized();

        // Rotate the joint from its initial rotation to the new one.
        parentNode->rotate(SLQuat4f(oldDir, newDir), TS_world);
    }
}
//-----------------------------------------------------------------------------
//! Converts a MediaPipe point to an SL point
SLVec3f SLTrackedHand::convertMP2SL(CVPoint3f point)
{
    return {point.x, -point.y, -point.z};
}
//-----------------------------------------------------------------------------