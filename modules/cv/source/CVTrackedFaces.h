/**
 * \file      CVTrackedFaces.h
 * \date      Spring 2018
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Marcus Hudritsch, Michael Goettlicher
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#ifndef CVTrackedFaces_H
#define CVTrackedFaces_H

/*
The OpenCV library version 3.4 with extra module must be present.
If the application captures the live video stream with OpenCV you have
to define in addition the constant APP_USES_CVCAPTURE.
All classes that use OpenCV begin with CV.
See also the class docs for CVCapture, CVCalibration and CVTracked
for a good top down information.
*/

#include <CVTypedefs.h>
#include <CVTracked.h>

typedef Utils::Averaged<CVVec2f> AvgCVVec2f;

//-----------------------------------------------------------------------------
//! OpenCV face & facial landmark tracker class derived from CVTracked
/*! Tracking class for face and facial landmark tracking. The class uses the
OpenCV face detection algorithm from Viola-Jones to find all faces in the image
and the facial landmark detector provided in cv::facemark. For more details
see the comments in CVTrackedFaces::track method.
*/
class CVTrackedFaces : public CVTracked
{
public:
    explicit CVTrackedFaces(string faceClassifierFilename, // haarcascade_frontalface_alt2.xml
                            string faceMarkModelFilename,  // lbfmodel.yaml
                            int    smoothLength = 5);
    ~CVTrackedFaces();

    bool track(CVMat          imageGray,
               CVMat          imageBgr,
               CVCalibration* calib) final;

    static void delaunayTriangulate(CVMat             imageBgr,
                                    const CVVPoint2f& points,
                                    bool              drawDetection);

private:
    CVCascadeClassifier* _faceDetector;    //!< Viola-Jones face detector
    cv::Ptr<CVFacemark>  _facemark;        //!< Facial landmarks detector smart pointer
    vector<AvgCVVec2f>   _avgPosePoints2D; //!< vector of averaged facial landmark 2D points
    CVRect               _boundingRect;    //!< Bounding rectangle around landmarks
    CVVPoint2f           _cvPosePoints2D;  //!< vector of OpenCV point2D
    CVVPoint3f           _cvPosePoints3D;  //!< vector of OpenCV point2D
    int                  _smoothLength;    //!< Smoothing filter lenght
};
//-----------------------------------------------------------------------------
#endif // CVTrackedFaces_H