/**
 * \file      AppCommon.cpp
 * \brief     The AppCommon class holds the top-level instances of the app-demo
 * \details   For more info on how to create a new app with SLProject see:
 *            https://github.com/cpvrlab/SLProject4/wiki/Creating-a-New-App
 *            For more info about App framework see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      Februar 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SL.h>
#include <AppCommon.h>
#include <SLAssetManager.h>
#include <SLScene.h>
#include <SLSceneView.h>
#include <SLUiInterface.h>
#include <SLImGui.h>
#include <utility>
#include <GlobalTimer.h>
#include <SLGLProgramManager.h>
#include <SLOptix.h>
#include <SLAssetLoader.h>

//-----------------------------------------------------------------------------
//! Global static objects
SLInputManager           AppCommon::inputManager;
SLAssetManager*          AppCommon::assetManager = nullptr;
SLAssetLoader*           AppCommon::assetLoader  = nullptr;
SLScene*                 AppCommon::scene        = nullptr;
vector<SLSceneView*>     AppCommon::sceneViews;
SLUiInterface*           AppCommon::gui = nullptr;
SLDeviceRotation         AppCommon::devRot;
SLDeviceLocation         AppCommon::devLoc;
std::optional<SLSceneID> AppCommon::sceneToLoad;
SLstring                 AppCommon::name    = "SLProjectApp";
SLstring                 AppCommon::appTag  = "SLProject";
SLstring                 AppCommon::version = "4.2.000";

//! ASCII Label: https://patorjk.com/software/taag/#p=display&f=Small%20Slant&t=SLProject%204.2
SLstring AppCommon::asciiLabel = R"(
   ______   ___             _         __    ____   ___ 
  / __/ /  / _ \_______    (_)__ ____/ /_  / / /  |_  |
 _\ \/ /__/ ___/ __/ _ \  / / -_) __/ __/ /_  _/ / __/ 
/___/____/_/  /_/  \___/_/ /\__/\__/\__/   /_/(_)____/ 
                      |___/  
)";

#ifdef _DEBUG
SLstring AppCommon::configuration = "Debug";
#else
SLstring AppCommon::configuration = "Release";
#endif
SLstring            AppCommon::gitBranch = SL_GIT_BRANCH;
SLstring            AppCommon::gitCommit = SL_GIT_COMMIT;
SLstring            AppCommon::gitDate   = SL_GIT_DATE;
map<string, string> AppCommon::deviceParameter;

CVCalibrationEstimatorParams AppCommon::calibrationEstimatorParams;
CVCalibrationEstimator*      AppCommon::calibrationEstimator = nullptr;
SLstring                     AppCommon::calibIniPath;
SLstring                     AppCommon::calibFilePath;

SLIOBuffer AppCommon::fontDataProp;
SLIOBuffer AppCommon::fontDataFixed;

SLstring AppCommon::exePath;
SLstring AppCommon::configPath;
SLstring AppCommon::externalPath;
SLstring AppCommon::dataPath;
SLstring AppCommon::shaderPath;
SLstring AppCommon::modelPath;
SLstring AppCommon::texturePath;
SLstring AppCommon::fontPath;
SLstring AppCommon::videoPath;

SLSceneID                   AppCommon::sceneID = SID_Empty;
deque<function<void(void)>> AppCommon::jobsToBeThreaded;
deque<function<void(void)>> AppCommon::jobsToFollowInMain;
atomic<bool>                AppCommon::jobIsRunning(false);
string                      AppCommon::_jobProgressMsg = "";
atomic<int>                 AppCommon::_jobProgressNum(0);
atomic<int>                 AppCommon::_jobProgressMax(0);
mutex                       AppCommon::_jobMutex;

const string AppCommon::CALIB_FTP_HOST  = "pallas.ti.bfh.ch:21";
const string AppCommon::CALIB_FTP_USER  = "upload";
const string AppCommon::CALIB_FTP_PWD   = "FaAdbD3F2a";
const string AppCommon::CALIB_FTP_DIR   = "calibrations";
const string AppCommon::PROFILE_FTP_DIR = "profiles";

//-----------------------------------------------------------------------------
//! Application and Scene creation function
/*! Writes and inits the static application information and create the single
instance of the scene. Gets called by the C-interface function slCreateApp.
<br>
<br>
See examples usages in:
  - app_demo_slproject/glfw:    AppDemoMainGLFW.cpp   in function main()
  - app_demo_slproject/android: AppDemoAndroidJNI.cpp in Java_ch_fhnw_comgr_GLES3Lib_onInit()
  - app_demo_slproject/ios:     ViewController.m      in viewDidLoad()
<br>
/param applicationName The apps name
/param onSceneLoadCallback C Callback function as void* pointer for the scene creation.
*/
void AppCommon::createApp(SLstring appName)
{
    assert(AppCommon::scene == nullptr &&
           "You can create only one AppCommon");

    // Initialize the global SLGLState instance
    SLGLState::instance()->initAll();

    name = std::move(appName);
    SLGLProgramManager::init(dataPath + "shaders/", configPath);

    assetManager = new SLAssetManager();

    assetLoader = new SLAssetLoader(AppCommon::modelPath,
                                    AppCommon::texturePath,
                                    AppCommon::shaderPath,
                                    AppCommon::fontPath);

    GlobalTimer::timerStart();

#ifdef SL_HAS_OPTIX
    SLOptix::exePath = AppCommon::exePath;
    SLOptix::createStreamAndContext();
#endif
}
//-----------------------------------------------------------------------------
void AppCommon::registerCoreAssetsLoad()
{
    SLAssetLoader&  al = *AppCommon::assetLoader;
    SLAssetManager* am = AppCommon::assetManager;

    // FIXME: There are dependencies between these load tasks, how do we express this?

    // Load all core shader programs.
    al.addLoadTask([am]
                   { SLGLProgramManager::loadPrograms(); });

    // Generate static fonts.
    al.addLoadTask([am, fontPath = AppCommon::fontPath]
                   { am->generateStaticFonts(AppCommon::fontPath); });

    // Load data for ImGUI fonts.
    al.addRawDataToLoad(AppCommon::fontDataProp,
                        AppCommon::fontPath + "DroidSans.ttf",
                        IOK_font);
    al.addRawDataToLoad(AppCommon::fontDataFixed,
                        AppCommon::fontPath + "ProggyClean.ttf",
                        IOK_font);
}
//-----------------------------------------------------------------------------
//! Calls the destructor of the single scene instance.
/*! Destroys all data by calling the destructor of the single scene instance.
All other date gets destroyed from there. This function gets called by the
SLProject C-Interface function slTerminate that should be called at the end of
any SLProject application.
*/
void AppCommon::deleteApp()
{
    // The WebGL context is apparently already destroyed when we call this function
#ifndef SL_EMSCRIPTEN
    assert(AppCommon::scene != nullptr &&
           "You can delete a scene only once");

    for (auto* sv : sceneViews)
        delete sv;
    sceneViews.clear();

    delete scene;
    scene = nullptr;

    delete assetManager;
    assetManager = nullptr;

    delete assetLoader;
    assetLoader = nullptr;
#endif

    if (gui)
    {
        delete gui;
        gui = nullptr;
    }

    // Deallocate global font data.
    AppCommon::fontDataProp.deallocate();
    AppCommon::fontDataFixed.deallocate();

    // The WebGL context is apparently already destroyed when we call this function
#ifndef SL_EMSCRIPTEN
    // delete default stuff:
    SLGLProgramManager::deletePrograms();
    SLMaterialDefaultGray::deleteInstance();
#endif

    // Delete the default materials
    SLMaterialDefaultGray::deleteInstance();
    SLMaterialDefaultColorAttribute::deleteInstance();

    // Delete the global SLGLState instance
    SLGLState::deleteInstance();
}
//-----------------------------------------------------------------------------
//! Starts parallel job if one is queued.
/*!
Parallel executed job can be queued in jobsToBeThreaded. Only functions are
allowed that do not call any OpenGL functions. So no scenegraph changes are
allowed because they involve mostly OpenGL state and context changes.
Only one parallel job is executed at once parallel to the main rendering thread.
The function in jobsToFollowInMain will be executed in the main tread after the
parallel are finished.<br>
The handleParallelJob function gets called in slUpdateAndPaint before a new
frame gets started. See an example parallel job definition in AppDemoGui.
If a parallel job is running the jobProgressMsg can be shown during execution.
If jobProgressMax is 0 the jobProgressNum value can be shown an number.
If jobProgressMax is not 0 the fraction of jobProgressNum/jobProgressMax can
be shown within a progress bar. See the example in AppDemoGui::build.
*/
void AppCommon::handleParallelJob()
{
    // Execute queued jobs in a parallel thread
    if (!AppCommon::jobIsRunning &&
        !AppCommon::jobsToBeThreaded.empty())
    {
        function<void(void)> job = AppCommon::jobsToBeThreaded.front();
        thread               jobThread(job);
        AppCommon::jobIsRunning = true;
        AppCommon::jobsToBeThreaded.pop_front();
        jobThread.detach();
    }

    // Execute the jobs to follow in the this (the main) thread
    if (!AppCommon::jobIsRunning &&
        AppCommon::jobsToBeThreaded.empty() &&
        !AppCommon::jobsToFollowInMain.empty())
    {
        for (const auto& jobToFollow : AppCommon::jobsToFollowInMain)
            jobToFollow();
        AppCommon::jobsToFollowInMain.clear();
    }
}
//-----------------------------------------------------------------------------
//! Thread-safe setter of the progress message
void AppCommon::jobProgressMsg(string msg)
{
    AppCommon::_jobMutex.lock();
    AppCommon::_jobProgressMsg = std::move(msg);
    AppCommon::_jobMutex.unlock();
}
//-----------------------------------------------------------------------------
//! Thread-safe getter of the progress message
string AppCommon::jobProgressMsg()
{
    lock_guard<mutex> guard(_jobMutex);
    return _jobProgressMsg;
}
//-----------------------------------------------------------------------------
