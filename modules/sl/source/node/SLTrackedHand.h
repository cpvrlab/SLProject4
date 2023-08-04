//#############################################################################
//  File:      SLTrackedHands.h
//  Date:      July 2023
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef SLTRACKEDHAND_H
#define SLTRACKEDHAND_H

#include <SLNode.h>
#include <CVTrackedMediaPipeHands.h>

//-----------------------------------------------------------------------------
//! A node that applies MediaPipe hand tracking to a 3D hand model
/*! The node only tracks the left hand and requires the Oculus hand model
 * to work. The tracking breaks if there are multiple hands visible.
 */
class SLTrackedHand : public SLNode
{
public:
    explicit SLTrackedHand(const SLstring&          name,
                           CVTrackedMediaPipeHands* tracker,
                           SLNode*                  model);

    void doUpdate() override;

private:
    static SLVec3f convertMP2SL(CVPoint3f point);

    CVTrackedMediaPipeHands* _tracker;
    SLAnimSkeleton*          _skeleton;
};
//-----------------------------------------------------------------------------

#endif