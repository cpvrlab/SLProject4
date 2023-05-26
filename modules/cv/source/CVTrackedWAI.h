//#############################################################################
//  File:      CVTrackedWAI.h
//  Date:      Spring 2020
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Michael Goettlicher, Jan Dellsperger
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef CVTrackedWAI_H
#define CVTrackedWAI_H

#include <CVTracked.h>
#include <WAIOrbVocabulary.h>
#include <WAISlam.h>

//-----------------------------------------------------------------------------
//! Tracker that uses the ORB-Slam based WAI library (Where Am I)
/*
 This tracker can can track a on-the-fly built 3D point cloud generated by
 the ORB-SLAM2 library that is integrated within the lib-WAI. It only works
 well if the camera is calibrated. SLAM stands for Simultaneous Localisation
 And Mapping. See the app-Demo-SLProject Demo Scene > Video > Track WAI.
 */
class CVTrackedWAI : public CVTracked
{
public:
    explicit CVTrackedWAI(const string& vocabularyFile);
    ~CVTrackedWAI() override;

    bool track(CVMat          imageGray,
               CVMat          imageRgb,
               CVCalibration* calib) final;

private:
    WAISlam*                 _waiSlamer               = nullptr;
    ORB_SLAM2::ORBextractor* _trackingExtractor       = nullptr;
    ORB_SLAM2::ORBextractor* _initializationExtractor = nullptr;
    WAIOrbVocabulary*        _voc;
};
//-----------------------------------------------------------------------------
#endif