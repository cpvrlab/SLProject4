/**
 * \file      AppDemoVideo.cpp
 * \brief     All video capturing and video tracking functions are in here.
 * \date      August 2019
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include <AppDemoSceneView.h>
#include <AppCommon.h>
#include <AppDemoSceneID.h>
#include <SLScene.h>
#include <SLSceneView.h>
#include <CVImage.h>
#include <CVCapture.h>
#include <CVTracked.h>
#include <CVTrackedAruco.h>
#include <SLGLTexture.h>
#include <CVCalibrationEstimator.h>
#include <GlobalTimer.h>
#include <Profiler.h>

#ifndef SL_EMSCRIPTEN
#    include <FtpUtils.h>
#endif

//-----------------------------------------------------------------------------
/*! Global pointer for the video texture defined in AppDemoLoad for video scenes
 It gets updated in the following onUpdateTracking routine
 */
SLGLTexture* gVideoTexture = nullptr;

/*! Global pointer for a gVideoTracker that is set in AppDemoLoad for video scenes
 It gets updated in the following onUpdateTracking routine
 */
CVTracked* gVideoTracker = nullptr;

/*! Global pointer to a node that from witch the gVideoTracker changes the pose.
 it gets updated in the following onUpdateTracking routine
 */
SLNode* gVideoTrackedNode = nullptr;

//-----------------------------------------------------------------------------
/*! always update scene camera fovV from calibration because the calibration
 may have been adapted in adjustForSL after a change of aspect ratio!
 Attention: The active scene view camera may be a different one that the
 tracking camera but we have to update the tracking camera only!
*/
void updateTrackingSceneCamera(CVCamera* ac)
{
    PROFILE_FUNCTION();

    if (gVideoTrackedNode && typeid(*gVideoTrackedNode) == typeid(SLCamera))
    {
        SLCamera* trackingCam = dynamic_cast<SLCamera*>(gVideoTrackedNode);
        trackingCam->fov(ac->calibration.cameraFovVDeg());
    }
}
//-----------------------------------------------------------------------------
// CVCalibrationEstimator* calibrationEstimator = nullptr;
void runCalibrationEstimator(CVCamera* ac, SLScene* s, SLSceneView* sv)
{
    PROFILE_FUNCTION();

    AppDemoSceneView* adSv                 = dynamic_cast<AppDemoSceneView*>(sv);
    static bool       processedCalibResult = false;
    try
    {
        if (!AppCommon::calibrationEstimator)
        {
            AppCommon::calibrationEstimator = new CVCalibrationEstimator(AppCommon::calibrationEstimatorParams,
                                                                         CVCapture::instance()->activeCamSizeIndex,
                                                                         ac->mirrorH(),
                                                                         ac->mirrorV(),
                                                                         ac->type(),
                                                                         Utils::ComputerInfos::get(),
                                                                         AppCommon::calibIniPath,
                                                                         AppCommon::externalPath,
                                                                         AppCommon::exePath);

            // clear grab request from sceneview
            adSv->grab           = false;
            processedCalibResult = false;
        }

        if (AppCommon::calibrationEstimator->isStreaming())
        {
            AppCommon::calibrationEstimator->updateAndDecorate(CVCapture::instance()->lastFrame,
                                                               CVCapture::instance()->lastFrameGray,
                                                               adSv->grab);
            // reset grabbing switch
            adSv->grab = false;

            stringstream ss;
            ss << "Click on the screen to create a calibration photo. Created "
               << AppCommon::calibrationEstimator->numCapturedImgs()
               << " of " << AppCommon::calibrationEstimator->numImgsToCapture();
            s->info(ss.str());
        }
        else if (AppCommon::calibrationEstimator->isBusyExtracting())
        {
            // also reset grabbing, user has to click again
            adSv->grab = false;
            AppCommon::calibrationEstimator->updateAndDecorate(CVCapture::instance()->lastFrame,
                                                               CVCapture::instance()->lastFrameGray,
                                                               false);
            s->info("Busy extracting corners, please wait with grabbing ...");
        }
        else if (AppCommon::calibrationEstimator->isCalculating())
        {
            AppCommon::calibrationEstimator->updateAndDecorate(CVCapture::instance()->lastFrame,
                                                               CVCapture::instance()->lastFrameGray,
                                                               false);
            s->info("Calculating calibration, please wait ...");
        }
        else if (AppCommon::calibrationEstimator->isDone())
        {
            if (!processedCalibResult)
            {
                if (AppCommon::calibrationEstimator->calibrationSuccessful())
                {
                    processedCalibResult = true;
                    ac->calibration      = AppCommon::calibrationEstimator->getCalibration();

                    std::string computerInfo      = Utils::ComputerInfos::get();
                    string      mainCalibFilename = "camCalib_" + computerInfo + "_main.xml";
                    string      scndCalibFilename = "camCalib_" + computerInfo + "_scnd.xml";
                    std::string errorMsg;
                    if (ac->calibration.save(AppCommon::calibFilePath, mainCalibFilename))
                    {

#ifndef SL_EMSCRIPTEN
                        if (!FtpUtils::uploadFile(AppCommon::calibFilePath,
                                                  mainCalibFilename,
                                                  AppCommon::CALIB_FTP_HOST,
                                                  AppCommon::CALIB_FTP_USER,
                                                  AppCommon::CALIB_FTP_PWD,
                                                  AppCommon::CALIB_FTP_DIR,
                                                  errorMsg))
                        {
                            Utils::log("WAIApp", errorMsg.c_str());
                        }
#endif
                    }
                    else
                    {
                        errorMsg += " Saving calibration failed!";
                    }

                    s->info("Calibration successful." + errorMsg);
                }
                else
                {
                    s->info(("Calibration failed!"));
                }
            }
        }
        else if (AppCommon::calibrationEstimator->isDoneCaptureAndSave())
        {
            s->info(("Capturing done!"));
        }
    }
    catch (CVCalibrationEstimatorException& e)
    {
        log("SLProject", e.what());
        s->info("Exception during calibration! Please restart!");
    }
}

//-----------------------------------------------------------------------------
//! logic that ensures that we have a valid calibration state
void ensureValidCalibration(CVCamera* ac, SLSceneView* sv)
{
    PROFILE_FUNCTION();

    // we have to make sure calibration process is stopped if someone stops calibrating
    if (AppCommon::calibrationEstimator)
    {
        delete AppCommon::calibrationEstimator;
        AppCommon::calibrationEstimator = nullptr;
    }

    if (ac->calibration.state() == CS_uncalibrated)
    {
        // Try to read device lens and sensor information
        string strF = AppCommon::deviceParameter["DeviceLensFocalLength"];
        string strW = AppCommon::deviceParameter["DeviceSensorPhysicalSizeW"];
        string strH = AppCommon::deviceParameter["DeviceSensorPhysicalSizeH"];
        if (!strF.empty() && !strW.empty() && !strH.empty())
        {
            float devF = strF.empty() ? 0.0f : stof(strF);
            float devW = strW.empty() ? 0.0f : stof(strW);
            float devH = strH.empty() ? 0.0f : stof(strH);

            // Changes the state to CS_guessed
            ac->calibration = CVCalibration(devW,
                                            devH,
                                            devF,
                                            cv::Size(CVCapture::instance()->lastFrame.cols,
                                                     CVCapture::instance()->lastFrame.rows),
                                            ac->mirrorH(),
                                            ac->mirrorV(),
                                            ac->type(),
                                            Utils::ComputerInfos::get());
        }
        else
        {
            // make a guess using frame size and a guessed field of view
            ac->calibration = CVCalibration(cv::Size(CVCapture::instance()->lastFrame.cols,
                                                     CVCapture::instance()->lastFrame.rows),
                                            60.0,
                                            ac->mirrorH(),
                                            ac->mirrorV(),
                                            ac->type(),
                                            Utils::ComputerInfos::get());
        }
    }
}
//-----------------------------------------------------------------------------
//! Implements the update per frame for video update and feature tracking
/*! This routine is called once per frame before any other update within the
 the main rendering loop (see: AppDemoMainGLFW::onPaint or GLES3View::onDrawFrame).
 See the documentation within SLCVTracked and in all of its inheritants.
*/
bool onUpdateVideo()
{
    PROFILE_FUNCTION();

    if (AppCommon::sceneViews.empty())
        return false;

    SLScene*     s  = AppCommon::scene;
    SLSceneView* sv = AppCommon::sceneViews[0];

    if (CVCapture::instance()->videoType() != VT_NONE &&
        !CVCapture::instance()->lastFrame.empty())
    {
        SLfloat trackingTimeStartMS = GlobalTimer::timeMS();

        CVCamera* ac = CVCapture::instance()->activeCamera;

        if (AppCommon::sceneID == SID_VideoCalibrateMain ||
            AppCommon::sceneID == SID_VideoCalibrateScnd)
        {
            runCalibrationEstimator(ac, s, sv);
        }
        else
        {
            ensureValidCalibration(ac, sv);
            // Attention: Always update scene camera fovV from calibration because the calibration may have
            // been adapted in adjustForSL after a change of aspect ratio!
            // The active scene view camera may be a different one that the tracking camera
            // but we have to update the tracking camera only!
            updateTrackingSceneCamera(ac);

            if (gVideoTracker && gVideoTrackedNode)
            {
                bool foundPose = gVideoTracker->track(CVCapture::instance()->lastFrameGray,
                                                      CVCapture::instance()->lastFrame,
                                                      &ac->calibration);
                if (foundPose)
                {
                    // clang-format off
                    // convert matrix type CVMatx44f to SLMat4f
                    CVMatx44f cvOVM = gVideoTracker->objectViewMat();
                    SLMat4f glOVM(cvOVM.val[0], cvOVM.val[1], cvOVM.val[2], cvOVM.val[3],
                                  cvOVM.val[4], cvOVM.val[5], cvOVM.val[6], cvOVM.val[7],
                                  cvOVM.val[8], cvOVM.val[9], cvOVM.val[10],cvOVM.val[11],
                                  cvOVM.val[12],cvOVM.val[13],cvOVM.val[14],cvOVM.val[15]);
                    // clang-format on

                    // set the object matrix depending if the
                    // tracked node is attached to a camera or not
                    if (typeid(*gVideoTrackedNode) == typeid(SLCamera))
                    {
                        gVideoTrackedNode->om(glOVM.inverted());
                        gVideoTrackedNode->setDrawBitsRec(SL_DB_HIDDEN, true);
                    }
                    else
                    {
                        // see comments in CVTracked::calcObjectMatrix
                        gVideoTrackedNode->om(sv->camera()->om() * glOVM);
                        gVideoTrackedNode->setDrawBitsRec(SL_DB_HIDDEN, false);
                    }
                }
                else
                    gVideoTrackedNode->setDrawBitsRec(SL_DB_HIDDEN, false);
            }

            // Update info text only for chessboard scene
            if (AppCommon::sceneID == SID_VideoCalibrateMain ||
                AppCommon::sceneID == SID_VideoCalibrateScnd ||
                AppCommon::sceneID == SID_VideoTrackChessMain ||
                AppCommon::sceneID == SID_VideoTrackChessScnd)
            {
                SLfloat      fovH = ac->calibration.cameraFovHDeg();
                SLfloat      err  = ac->calibration.reprojectionError();
                stringstream ss; // info line text
                ss << "Tracking Chessboard on " << (CVCapture::instance()->videoType() == VT_MAIN ? "main " : "scnd. ") << "camera. ";
                if (ac->calibration.state() == CS_calibrated)
                    ss << "FOVH: " << fovH << ", error: " << err;
                else
                    ss << "Not calibrated. FOVH guessed: " << fovH << " degrees.";
                s->info(ss.str());
            }
        }

        //...................................................................
        // copy image to video texture
        if (gVideoTexture)
        {
            //SL_LOG("glFormat: %s\n", CVImage::formatString(CVCapture::instance()->format).c_str());
                
            if (ac->calibration.state() == CS_calibrated && ac->showUndistorted())
            {
                CVMat undistorted;
                ac->calibration.remap(CVCapture::instance()->lastFrame, undistorted);
                gVideoTexture->copyVideoImage(undistorted.cols,
                                              undistorted.rows,
                                              CVCapture::instance()->format,
                                              undistorted.data,
                                              undistorted.isContinuous(),
                                              true);
            }
            else
            {
                gVideoTexture->copyVideoImage(CVCapture::instance()->lastFrame.cols,
                                              CVCapture::instance()->lastFrame.rows,
                                              CVCapture::instance()->format,
                                              CVCapture::instance()->lastFrame.data,
                                              CVCapture::instance()->lastFrame.isContinuous(),
                                              true);
            }
        }
        else
            SL_WARN_MSG("No video texture to copy to.");

#ifndef SL_EMSCRIPTEN
        CVTracked::trackingTimesMS.set(GlobalTimer::timeMS() - trackingTimeStartMS);
#endif

        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------
