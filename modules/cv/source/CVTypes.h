/**
 * \file      CVTypes.h
 * \date      Winter 2019
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Marcus Hudritsch, Michael Goettlicher
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#ifndef CVTYPES_H
#define CVTYPES_H

#include <opencv2/calib3d.hpp>

//-----------------------------------------------------------------------------
class CVCalibrationEstimatorParams
{
public:
    enum class EstimatorMode
    {
        ExtractAndCalculate,
        OnlyCaptureAndSave
    };

    int calibrationFlags()
    {
        int flags = 0;
        if (fixPrincipalPoint)
            flags |= cv::CALIB_FIX_PRINCIPAL_POINT;
        if (fixAspectRatio)
            flags |= cv::CALIB_FIX_ASPECT_RATIO;
        if (zeroTangentDistortion)
            flags |= cv::CALIB_ZERO_TANGENT_DIST;
        if (calibRationalModel)
            flags |= cv::CALIB_RATIONAL_MODEL;
        if (calibTiltedModel)
            flags |= cv::CALIB_TILTED_MODEL;
        if (calibThinPrismModel)
            flags |= cv::CALIB_THIN_PRISM_MODEL;

        return flags;
    }
    void toggleFixPrincipalPoint() { fixPrincipalPoint = !fixPrincipalPoint; }
    void toggleFixAspectRatio() { fixAspectRatio = !fixAspectRatio; }
    void toggleZeroTangentDist() { zeroTangentDistortion = !zeroTangentDistortion; }
    void toggleRationalModel() { calibRationalModel = !calibRationalModel; }
    void toggleTiltedModel() { calibTiltedModel = !calibTiltedModel; }
    void toggleThinPrismModel() { calibThinPrismModel = !calibThinPrismModel; }

    bool fixPrincipalPoint     = false;
    bool fixAspectRatio        = false;
    bool zeroTangentDistortion = false;
    bool calibRationalModel    = false;
    bool calibTiltedModel      = false;
    bool calibThinPrismModel   = false;

    EstimatorMode mode                   = EstimatorMode::ExtractAndCalculate;
    bool          useReleaseObjectMethod = false;
};
//-----------------------------------------------------------------------------
enum class CVCameraType
{
    FRONTFACING,
    BACKFACING,
    VIDEOFILE
};
//-----------------------------------------------------------------------------
#endif // CVCALIBRATIONESTIMATORPARAMS_H
