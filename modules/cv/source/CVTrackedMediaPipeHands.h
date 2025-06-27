/**
 * \file      CVTrackedMediaPipeHands.h
 * \date      December 2022
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SL_OS_MACIOS
#   ifdef SL_BUILD_WITH_MEDIAPIPE
#       ifndef CVTRACKEDMEDIAPIPEHANDS_H
#           define CVTRACKEDMEDIAPIPEHANDS_H
#           include <CVTracked.h>
#           include <mediapipe.h>

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
    CVTrackedMediaPipeHands(SLstring dataPath);
    ~CVTrackedMediaPipeHands();

    bool track(CVMat          imageGray,
               CVMat          imageBgr,
               CVCalibration* calib) final;

private:
    void processImageInMediaPipe(CVMat imageBgr);
    void drawResults(mp_multi_face_landmark_list* landmarks,
                     CVMat                        imageBgr);

    mp_instance* _instance;
    mp_poller*   _landmarksPoller;
    CVMat        _imageRgb;
};
//-----------------------------------------------------------------------------

#       endif // CVTRACKEDMEDIAPIPEHANDS_H
#   endif     // SL_BUILD_WITH_MEDIAPIPE
#endif     // SL_PLATFROM_IOS
