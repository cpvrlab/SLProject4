//#############################################################################
//  File:      CVTrackedMediaPipeHands.h
//  Date:      December 2022
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifdef SL_BUILD_WITH_MEDIAPIPE
#    ifndef CVTRACKEDMEDIAPIPEHANDS_H
#        define CVTRACKEDMEDIAPIPEHANDS_H

#        include <CVTracked.h>
#        include <mediapipe.h>

//-----------------------------------------------------------------------------
//! MediaPipe hand tracking class derived from CVTracked
/*! Tracking class for MediaPipe hand tracking. MediaPipe is a machine
learning framework from google with various built-in solutions for face
tracking, hand tracking and much more. We use our custom C wrapper
[libmediapipe](https://github.com/cpvrlab/libmediapipe) to integrate the
framework. See mediapipe.h for more information about the MediaPipe data types
and functions. This tracker tracks two hands and draws the detected landmarks
over the image.

Link to the hand landmarker solution:
https://developers.google.com/mediapipe/solutions/vision/hand_landmarker

Online examples:
https://mediapipe-studio.webapps.google.com/demo/hand_landmarker
*/
class CVTrackedMediaPipeHands : public CVTracked
{
public:
    typedef CVPoint3f Results[2][21];

    CVTrackedMediaPipeHands(SLstring dataPath);
    ~CVTrackedMediaPipeHands();

    bool           track(CVMat          imageGray,
                         CVMat          imageBgr,
                         CVCalibration* calib) final;
    const Results& results() const { return _results; }

private:
    void processImageInMediaPipe(CVMat imageBgr);
    void drawResults(mp_multi_face_landmark_list* landmarks,
                     CVMat                        imageBgr);

    mp_instance* _instance;
    mp_poller*   _landmarksPoller;
    mp_poller*   _worldLandmarksPoller;
    CVMat        _imageRgb;
    Results      _results;
};
//-----------------------------------------------------------------------------

#    endif // CVTRACKEDMEDIAPIPEHANDS_H
#endif     // SL_BUILD_WITH_MEDIAPIPE
