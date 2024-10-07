/**
 * \file      AppCommon.cpp
 * \brief     The AppCommon class holds the top-level instances for SLProject apps
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
 
#include <App.h>
#include <AppCommon.h>
#include <SL.h>
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
SLstring                 AppCommon::version = "4.2.001"; // 7.10.2024

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

SLSceneID                   AppCommon::sceneID = SL_EMPTY_SCENE_ID;
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
//! Application creation function
/*! Writes and inits the static application information and create the single
instances of SLGLState, SLAssetManager and SLAssetLoader. Gets called by the
C-interface function slCreateApp.
<br>
<br>
See examples usages in:
  - platforms/android/AppAndroid.cpp in the App::run function
  - platforms/emscripten/AppEmscripten.cpp in the App::run function
  - platforms/glfw/AppGLFW.cpp in the App::run function
  - platforms/ios/ViewController.mm in the viewDidLoad function
<br>
/param applicationName The apps name
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
/*! Registers core assets to load async by all apps. Scene specific assets have
to be loaded async by overriding SLScene::registerAssetsToLoad and
SLScene::assemble. Async loading and assembling means that it happens in a
parallel thread and that inthere are no OpenGL calls allowed. OpenGL calls are
only allowed in the main thread.
*/
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
/*!
 * Deletes the current scene and creates a new one identified by the sceneID.
 * All assets get registered for async loading while Imgui shows a progress
 * spinner in the UI. After the parallel loading the scene gets assembled back
 * in the main thread.
 * \param sv Pointer to the sceneview
 * \param sceneID Scene identifier defined in SLEnum
 */
void AppCommon::switchScene(SLSceneView* sv, SLSceneID sceneID)
{
    SLAssetManager*& am = AppCommon::assetManager;
    SLAssetLoader*&  al = AppCommon::assetLoader;

    SLfloat startLoadMS = GlobalTimer::timeMS();

    //////////////////////
    // Delete old scene //
    //////////////////////

    if (App::config.onBeforeSceneDelete)
        App::config.onBeforeSceneDelete(sv, AppCommon::scene);

    if (AppCommon::scene)
    {
        delete AppCommon::scene;
        AppCommon::scene = nullptr;
    }

    // Deactivate in general the device sensors
    AppCommon::devRot.init();
    AppCommon::devLoc.init();

    // reset existing sceneviews
    for (auto* sceneView : AppCommon::sceneViews)
        sceneView->unInit();

    // Clear all data in the asset manager
    am->clear();

    ////////////////////
    // Init new scene //
    ////////////////////

    AppCommon::sceneID = sceneID;
    SLScene* s         = App::config.onNewScene(sceneID);
    SL_LOG("Scene name       : %s (SceneID: %d)",
           s->name().c_str(),
           AppCommon::sceneID);

    // Initialize all preloaded stuff from SLScene
    s->init(am);

#ifndef SL_EMSCRIPTEN
    s->initOculus(AppCommon::dataPath + "shaders/");
#endif

    // Reset the global SLGLState state
    SLGLState::instance()->initAll();

    ///////////////////////////////
    // Prepare for async loading //
    ///////////////////////////////

    // Register assets on the loader that have to be loaded before assembly.
    al->scene(s);
    s->registerAssetsToLoad(*al);

    // `onDone` is a wrapper around `onDoneLoading` that will be called
    // in the main thread after loading.
    auto onDone = std::bind(onDoneLoading, sv, s, startLoadMS);

    // Start loading assets asynchronously.
    al->loadAssetsAsync(onDone);

    if (App::config.onBeforeSceneLoad)
        App::config.onBeforeSceneLoad(sv, s);
}
//-----------------------------------------------------------------------------
/*! OnDoneLoading is called when the asyn asset loading is finished and the
async scene assembly can be started.
*/
void AppCommon::onDoneLoading(SLSceneView* sv, SLScene* s, SLfloat startLoadMS)
{
    SLAssetManager* am = AppCommon::assetManager;
    SLAssetLoader*  al = AppCommon::assetLoader;

    // Register a task to assemble the scene.
    al->addLoadTask(std::bind(&SLScene::assemble, s, am, sv));

    // `onDone` is a wrapper around `onDoneAssembling` that will be called
    // in the main thread after loading.
    auto onDone = std::bind(onDoneAssembling, sv, s, startLoadMS);

    // Start assembling the scene asynchronously.
    al->loadAssetsAsync(onDone);

    if (App::config.onBeforeSceneAssembly)
        App::config.onBeforeSceneAssembly(sv, s);
}
//-----------------------------------------------------------------------------
void AppCommon::onDoneAssembling(SLSceneView* sv, SLScene* s, SLfloat startLoadMS)
{
    /* Assign the scene to the sceneview. The sceneview exists and is used
     * before it knows its scene. This is new since we do async loading and
     * show a spinner in the sceneview. */
    sv->scene(s);
    sv->postSceneLoad();

    // Make sure the scene view has a camera
    if (!sv->camera())
        sv->camera(sv->sceneViewCamera());

    // call onInitialize on all scene views to init the scenegraph and stats
    for (auto* sceneView : AppCommon::sceneViews)
        if (sceneView != nullptr)
            sceneView->onInitialize();

    AppCommon::scene = s;

    if (App::config.onAfterSceneAssembly)
        App::config.onAfterSceneAssembly(sv, s);

    s->loadTimeMS(GlobalTimer::timeMS() - startLoadMS);
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
