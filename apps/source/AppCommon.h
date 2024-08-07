/**
 * \file      AppCommon.h
 * \brief     The AppCommon class holds the top-level instances of the app-demo
 * \details   For more info on how to create a new app with SLProject see:
 *            https://github.com/cpvrlab/SLProject4/wiki/Creating-a-New-App
 *            For more info about App framework see:
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      February 2018
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#ifndef SLAPPCOMMON_H
#define SLAPPCOMMON_H

#include <atomic>
#include <mutex>
#include <map>
#include <optional>

#include <CVTypes.h>
#include <SLDeviceLocation.h>
#include <SLDeviceRotation.h>
#include <SLInputManager.h>
#include <SLSceneView.h>
#include <SLEnums.h>
#include <SLFileStorage.h>

class SLScene;
class SLAssetLoader;
class SLImGui;
class CVCalibrationEstimator;
class SLUiInterface;

using std::optional;

//-----------------------------------------------------------------------------
//! Top level class for the major global instances off an SLProject app.
/*!
 The AppCommon holds static instances of top-level items such as the asset
 manager, the scene pointer, the vector of all sceneviews, the gui pointer,
 the camera calibration objects and the device rotation and location
 information.<br>
 The static function createApp is called by the C-interface functions
 slCreateApp and the function deleteApp by slTerminate. At the moment only
 one scene can be open at the time.
 <br>
 AppCommon holds two static video camera calibrations, one for a main camera
 (mainCam) and one for the selfie camera on mobile devices (scndCam).
 The pointer activeCamera points to the active one.
*/
class AppCommon
{
public:
    // Major owned instances of the app
    static SLInputManager   inputManager; //!< Input events manager
    static SLAssetManager*  assetManager; //!< asset manager is the owner of all assets
    static SLAssetLoader*   assetLoader;  //!< Asset-loader for async asset loading
    static SLScene*         scene;        //!< Pointer to the one and only SLScene instance
    static SLVSceneView     sceneViews;   //!< Vector of sceneview pointers
    static SLUiInterface*   gui;          //!< Pointer to the GUI
    static SLDeviceRotation devRot;       //!< Mobile device rotation from IMU
    static SLDeviceLocation devLoc;       //!< Mobile device location from GPS

    static void createApp(SLstring appName);
    static void registerCoreAssetsLoad();
    static void switchScene(SLSceneView* sv, SLSceneID sceneID);
    static void deleteApp();

    static SLstring            name;          //!< Application name
    static SLstring            appTag;        //!< Tag string used in logging
    static SLstring            version;       //!< SLProject version string
    static SLstring            asciiLabel;    //!< SLProject ascii label string
    static SLstring            configuration; //!< Debug or Release configuration
    static SLstring            gitBranch;     //!< Current GIT branch
    static SLstring            gitCommit;     //!< Current GIT commit short hash id
    static SLstring            gitDate;       //!< Current GIT commit date
    static SLstring            exePath;       //!< executable root path
    static SLstring            configPath;    //!< Default path for calibration files
    static SLstring            externalPath;  //!< Default path for external file storage
    static SLstring            dataPath;      //!< Path to data directory (it is set platform dependent)
    static SLstring            shaderPath;    //!< Path to GLSL shader programs
    static SLstring            modelPath;     //!< Path to 3D models
    static SLstring            texturePath;   //!< Path to texture images
    static SLstring            fontPath;      //!< Path to font images
    static SLstring            videoPath;     //!< Path to video files
    static SLSceneID           sceneID;       //!< ID of currently loaded scene
    static optional<SLSceneID> sceneToLoad;   //!< Scene id to load at start up

    // static methods for parallel job processing
    static void   handleParallelJob();
    static void   jobProgressMsg(string msg);
    static void   jobProgressNum(int num) { _jobProgressNum = num; }
    static void   jobProgressMax(int max) { _jobProgressMax = max; }
    static string jobProgressMsg();
    static int    jobProgressNum() { return _jobProgressNum; }
    static int    jobProgressMax() { return _jobProgressMax; }

    static map<string, string>         deviceParameter;    //!< Generic device parameter
    static deque<function<void(void)>> jobsToBeThreaded;   //!< Queue of functions to be executed in a thread
    static deque<function<void(void)>> jobsToFollowInMain; //!< Queue of function to follow in the main thread
    static atomic<bool>                jobIsRunning;       //!< True if a parallel job is running

    static CVCalibrationEstimatorParams calibrationEstimatorParams;
    static CVCalibrationEstimator*      calibrationEstimator;
    static SLstring                     calibIniPath;  //!< That's where data/calibrations folder is located
    static SLstring                     calibFilePath; //!< That's where calibrations are stored and loaded from

    static SLIOBuffer fontDataProp;
    static SLIOBuffer fontDataFixed;

    static const string CALIB_FTP_HOST;  //!< ftp host for calibration up and download
    static const string CALIB_FTP_USER;  //!< ftp login user for calibration up and download
    static const string CALIB_FTP_PWD;   //!< ftp login pwd for calibration up and download
    static const string CALIB_FTP_DIR;   //!< ftp directory for calibration up and download
    static const string PROFILE_FTP_DIR; //!< ftp directory for profiles upload

private:
    static void onDoneLoading(SLSceneView* sv, SLScene* s, SLfloat startLoadMS);
    static void onDoneAssembling(SLSceneView* sv, SLScene* s, SLfloat startLoadMS);

    static string      _jobProgressMsg; //!< Text message to show during progress
    static atomic<int> _jobProgressNum; //!< Integer value to show progress
    static atomic<int> _jobProgressMax; //!< Max. integer progress value
    static mutex       _jobMutex;       //!< Mutex to protect parallel access
};
//-----------------------------------------------------------------------------
#endif
