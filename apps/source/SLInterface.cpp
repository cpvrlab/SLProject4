/**
 * \file      SLInterface.cpp
 * \brief     Implementation of the main Scene Library C-Interface.
 * \details   The SLInterface.cpp has all implementations of the SLProject
 *            C-Interface. Only these functions should called by the
 *            OS-dependent GUI applications. These functions can be called from
 *            any C, C++ or ObjectiveC GUI framework or by a native API such
 *            as Java Native Interface (JNI).
 *            For more info on how to create a new app with SLProject see:
 *            https://github.com/cpvrlab/SLProject4/wiki/Creating-a-New-App
 *            For more info about App framework see:
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include <SLInterface.h>
#include <AppCommon.h>
#include <SLAssimpImporter.h>
#include <SLAssetManager.h>
#include <SLAssetLoader.h>
#include <SLInputManager.h>
#include <SLScene.h>
#include <SLSceneView.h>
#include <SLImGui.h>
#include <Profiler.h>
#include <ZipUtils.h>

//-----------------------------------------------------------------------------
//! global flag that determines if the application should be closed
bool gShouldClose = false;

//-----------------------------------------------------------------------------
/*! Global creation function for a app instance. This function should be
called only once per application. The SLScene constructor call is delayed until
the first SLSceneView is created to guarantee, that the OpenGL context is
present.<br>
/param cmdLineArgs Command line arguments (not used yet)
/param shaderPath Path to the shader files (readonly)
/param modelPath Path to the 3D model files (readonly)
/param texturePath Path to the texture image files (readonly)
/param fontPath Path to the font image files (readonly)
/param calibrationPath Path to the calibration ini files (readonly)
/param configPath Path where the config files are stored (read-write)
/param applicationName The apps name
/param onSceneLoadCallback C Callback function as void* pointer for the scene creation.
<br>
See examples usages in:
  - apps/source/platforms/android   /AppAndroid.cpp > Java_ch_bfh_cpvrlab_AppAndroidJNI_onInit
  - apps/source/platforms/emscripten/AppEmscripten.cpp > App::run
  - apps/source/platforms/glfw      /AppGLFW > App::run
  - apps/source/platforms/ios       /ViewController.mm > viewDidLoad
*/
void slCreateApp(SLVstring&      cmdLineArgs,
                 const SLstring& dataPath,
                 const SLstring& shaderPath,
                 const SLstring& modelPath,
                 const SLstring& texturePath,
                 const SLstring& fontPath,
                 const SLstring& videoPath,
                 const SLstring& configPath,
                 const SLstring& applicationName)
{
    assert(AppCommon::scene == nullptr && "SLScene is already created!");

    // For more info on PROFILING read Utils/lib-utils/source/Profiler.h
#if PROFILING
    if (Utils::dirExists(AppCommon::externalPath))
    {
        SLstring computerInfo = Utils::ComputerInfos::get();
        SLstring profileFile  = AppCommon::externalPath + "Profile_" + computerInfo + ".slt";
        BEGIN_PROFILING_SESSION(profileFile);
        PROFILE_THREAD("Main Thread");
    }
#endif

    // Default paths for all loaded resources
    SLstring exe           = Utils::unifySlashes(cmdLineArgs.size() ? cmdLineArgs[0] : "", false);
    AppCommon::exePath     = Utils::getDirName(exe);
    AppCommon::dataPath    = Utils::unifySlashes(dataPath);
    AppCommon::shaderPath  = shaderPath;
    AppCommon::modelPath   = modelPath;
    AppCommon::texturePath = texturePath;
    AppCommon::fontPath    = fontPath;
    AppCommon::videoPath   = videoPath;
    AppCommon::configPath  = configPath;

    SLGLState* stateGL = SLGLState::instance();
    SL_LOG("Path to exe      : %s", AppCommon::exePath.c_str());
    SL_LOG("Path to Models   : %s", modelPath.c_str());
    SL_LOG("Path to Shaders  : %s", shaderPath.c_str());
    SL_LOG("Path to Textures : %s", texturePath.c_str());
    SL_LOG("Path to Fonts    : %s", fontPath.c_str());
    SL_LOG("Path to Config.  : %s", configPath.c_str());
    SL_LOG("Path to Documents: %s", AppCommon::externalPath.c_str());
    SL_LOG("OpenCV Version   : %d.%d.%d", CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_VERSION_REVISION);
    SL_LOG("OpenCV has OpenCL: %s", cv::ocl::haveOpenCL() ? "yes" : "no");
    SL_LOG("OpenGL Ver. Str. : %s", stateGL->glVersion().c_str());
    SL_LOG("OpenGL Ver. No.  : %s", stateGL->glVersionNO().c_str());
    SL_LOG("OpenGL Vendor    : %s", stateGL->glVendor().c_str());
    SL_LOG("OpenGL Renderer  : %s", stateGL->glRenderer().c_str());
    SL_LOG("OpenGL GLSL Ver. : %s (%s) ", stateGL->glSLVersion().c_str(), stateGL->getSLVersionNO().c_str());
    SL_LOG("------------------------------------------------------------------");

    AppCommon::createApp(applicationName);
}
//-----------------------------------------------------------------------------
/*! Global creation function for a SLSceneview instance returning the index of
the sceneview. It creates the new SLSceneView instance by calling the callback
function slNewSceneView. If you have a custom SLSceneView inherited class you
have to provide a similar function and pass it function pointer to
slCreateSceneView. You can create multiple sceneview per application.<br>
<br>
See examples usages in:
  - app_demo_slproject/glfw:    AppDemoMainGLFW.cpp   in function main()
  - app-demo/android: AppDemoAndroidJNI.cpp in Java_ch_fhnw_comgr_GLES3Lib_onInit()
  - app_demo_slproject/ios:     ViewController.m      in viewDidLoad()
*/
SLint slCreateSceneView(SLAssetManager* am,
                        SLScene*        scene,
                        int             screenWidth,
                        int             screenHeight,
                        int             dotsPerInch,
                        SLSceneID       initScene,
                        void*           onWndUpdateCallback,
                        void*           onSelectNodeMeshCallback,
                        void*           onNewSceneViewCallback,
                        void*           onImGuiBuild,
                        void*           onImGuiLoadConfig,
                        void*           onImGuiSaveConfig)
{
    // assert(scene && "No valid scene!");

    // Use our own sceneview creator callback or the passed one.
    cbOnNewSceneView newSVCallback;
    if (onNewSceneViewCallback == nullptr)
        newSVCallback = &slNewSceneView;
    else
        newSVCallback = (cbOnNewSceneView)onNewSceneViewCallback;

    // Create the sceneview & get the pointer with the sceneview index
    SLSceneView* sv = newSVCallback(scene, dotsPerInch, AppCommon::inputManager);

    // maintain multiple scene views in AppCommon
    AppCommon::sceneViews.push_back(sv);

    // Scale for proportional and fixed size fonts
    SLfloat dpiScaleProp  = (float)dotsPerInch / 120.0f;
    SLfloat dpiScaleFixed = (float)dotsPerInch / 142.0f;

    // Default settings for the first time
    SLImGui::fontPropDots  = std::max(16.0f * dpiScaleProp, 16.0f);
    SLImGui::fontFixedDots = std::max(13.0f * dpiScaleFixed, 13.0f);

    // Create gui renderer
    AppCommon::gui = new SLImGui((cbOnImGuiBuild)onImGuiBuild,
                                 (cbOnImGuiLoadConfig)onImGuiLoadConfig,
                                 (cbOnImGuiSaveConfig)onImGuiSaveConfig,
                                 dotsPerInch,
                                 AppCommon::fontDataProp,
                                 AppCommon::fontDataFixed);

    sv->init("SceneView",
             screenWidth,
             screenHeight,
             onWndUpdateCallback,
             onSelectNodeMeshCallback,
             AppCommon::gui,
             AppCommon::configPath);

    // Set active sceneview and load scene. This is done for the first sceneview
    if (!scene)
    {
        if (AppCommon::sceneID == SL_EMPTY_SCENE_ID)
            AppCommon::sceneToLoad = initScene;
        else
            AppCommon::sceneToLoad = AppCommon::sceneID;
    }
    else
        sv->onInitialize();

    // return the identifier index
    return (SLint)AppCommon::sceneViews.size() - 1;
}
//-----------------------------------------------------------------------------
/*! Global C function for synchronous asset loading used by all platforms
except on emscripten where slLoadCoreAssetsAsync is used.
*/
void slLoadCoreAssetsSync()
{
    AppCommon::registerCoreAssetsLoad();
    AppCommon::assetLoader->loadAssetsSync();
}
//-----------------------------------------------------------------------------
/*! Global C function for asynchronous asset loading used on emscripten
platform.
*/
void slLoadCoreAssetsAsync()
{
    AppCommon::registerCoreAssetsLoad();
    AppCommon::assetLoader->loadAssetsAsync([] {});
}
//-----------------------------------------------------------------------------
/*! Global C function for scene switching with a sceneID
 */
void slSwitchScene(SLSceneView* sv, SLSceneID sceneID)
{
    AppCommon::switchScene(sv, sceneID);
}
//-----------------------------------------------------------------------------
/*! Global sceneview construction function returning the index of the created
sceneview instance. If you have a custom SLSceneView inherited class you
have to provide a similar function and pass it function pointer to
slCreateSceneView.
*/
SLSceneView* slNewSceneView(SLScene*        s,
                            int             dotsPerInch,
                            SLInputManager& inputManager)
{
    return new SLSceneView(s, dotsPerInch, inputManager);
}
//-----------------------------------------------------------------------------
/*! Global closing function that deallocates the sceneview and scene instances.
All the scenegraph deallocation is started from here and has to be done before
the GUI app terminates.
*/
bool slShouldClose()
{
    return gShouldClose;
}
//-----------------------------------------------------------------------------
/*! Global closing function that sets our global running flag. This lets
the windowing system know that we want to terminate.
*/
void slShouldClose(bool val)
{
    gShouldClose = val;
}
//-----------------------------------------------------------------------------
/*! Global closing function that deallocates the sceneview and scene instances.
All the scenegraph deallocation is started from here and has to be done before
the GUI app terminates.
*/
void slTerminate()
{
    SL_LOG("Begin of Terminate");

    // Deletes all remaining sceneviews and the current scene instance
    AppCommon::deleteApp();

    // For more info on PROFILING read Utils/lib-utils/source/Profiler.h
#if PROFILING
    SLstring filePathName = PROFILER_TRACE_FILE_PATH;

    SL_LOG("Before END_PROFILING_SESSION");
    END_PROFILING_SESSION();
    SL_LOG("After END_PROFILING_SESSION");

    Profiler::instance().endSession();

    if (Utils::fileExists(filePathName))
        SL_LOG("Profile written : %s", filePathName.c_str());
    else
        SL_LOG("No Profile written: %s", filePathName.c_str());
#else
    SL_LOG("No Profiling");
#endif

    SL_LOG("End of Terminate");
    SL_LOG("------------------------------------------------------------------");
}
//-----------------------------------------------------------------------------
/*!
 * Updates the parallel running job an allowes the update of a progress bar.
 * \return Returns true if parallel jobs are still running.
 */
bool slUpdateParallelJob()
{
    AppCommon::handleParallelJob();
    return AppCommon::jobIsRunning;
}
//-----------------------------------------------------------------------------
/*!
 * Draws all scene views
 * \return return true if another repaint is needed.
 */
bool slPaintAllViews()
{
    bool needUpdate = false;

    for (auto sv : AppCommon::sceneViews)
    {
        // Save previous frame as image
        if (sv->screenCaptureIsRequested())
        {
            SLstring path = AppCommon::externalPath + "screenshots/";
            Utils::makeDirRecurse(path);
            SLstring filename = "Screenshot_" +
                                Utils::getDateTime2String() + ".png";
            SLstring pathFilename = path + filename;
            sv->saveFrameBufferAsImage(pathFilename);
        }

        if (sv->onPaint() && !needUpdate)
            needUpdate = true;
    }

    return needUpdate;
}
//-----------------------------------------------------------------------------
/*! Global resize function that must be called whenever the OpenGL frame
changes it's size.
*/
void slResize(int sceneViewIndex, int width, int height)
{
    SLResizeEvent* e = new SLResizeEvent;
    e->svIndex       = sceneViewIndex;
    e->width         = width;
    e->height        = height;
    AppCommon::inputManager.queueEvent(e);
}
//-----------------------------------------------------------------------------
/*! Global event handler for mouse button down events.
 */
void slMouseDown(int           sceneViewIndex,
                 SLMouseButton button,
                 int           xpos,
                 int           ypos,
                 SLKey         modifier)
{
    SLMouseEvent* e = new SLMouseEvent(SLInputEvent::MouseDown);
    e->svIndex      = sceneViewIndex;
    e->button       = button;
    e->x            = xpos;
    e->y            = ypos;
    e->modifier     = modifier;
    AppCommon::inputManager.queueEvent(e);
}
//-----------------------------------------------------------------------------
/*! Global event handler for mouse move events.
 */
void slMouseMove(int sceneViewIndex,
                 int x,
                 int y)
{
    SLMouseEvent* e = new SLMouseEvent(SLInputEvent::MouseMove);
    e->svIndex      = sceneViewIndex;
    e->x            = x;
    e->y            = y;
    AppCommon::inputManager.queueEvent(e);
}
//-----------------------------------------------------------------------------
/*! Global event handler for mouse button up events.
 */
void slMouseUp(int           sceneViewIndex,
               SLMouseButton button,
               int           xpos,
               int           ypos,
               SLKey         modifier)
{
    SLMouseEvent* e = new SLMouseEvent(SLInputEvent::MouseUp);
    e->svIndex      = sceneViewIndex;
    e->button       = button;
    e->x            = xpos;
    e->y            = ypos;
    e->modifier     = modifier;
    AppCommon::inputManager.queueEvent(e);
}
//-----------------------------------------------------------------------------
/*! Global event handler for double click events.
 */
void slDoubleClick(int           sceneViewIndex,
                   SLMouseButton button,
                   int           xpos,
                   int           ypos,
                   SLKey         modifier)
{
    SLMouseEvent* e = new SLMouseEvent(SLInputEvent::MouseDoubleClick);
    e->svIndex      = sceneViewIndex;
    e->button       = button;
    e->x            = xpos;
    e->y            = ypos;
    e->modifier     = modifier;
    AppCommon::inputManager.queueEvent(e);
}
//-----------------------------------------------------------------------------
/*! Global event handler for the two finger touch down events of touchscreen
devices.
*/
void slTouch2Down(int sceneViewIndex,
                  int xpos1,
                  int ypos1,
                  int xpos2,
                  int ypos2)
{
    SLTouchEvent* e = new SLTouchEvent(SLInputEvent::Touch2Down);
    e->svIndex      = sceneViewIndex;
    e->x1           = xpos1;
    e->y1           = ypos1;
    e->x2           = xpos2;
    e->y2           = ypos2;

    AppCommon::inputManager.queueEvent(e);
}
//-----------------------------------------------------------------------------
/*! Global event handler for the two finger move events of touchscreen devices.
 */
void slTouch2Move(int sceneViewIndex,
                  int xpos1,
                  int ypos1,
                  int xpos2,
                  int ypos2)
{
    SLTouchEvent* e = new SLTouchEvent(SLInputEvent::Touch2Move);
    e->svIndex      = sceneViewIndex;
    e->x1           = xpos1;
    e->y1           = ypos1;
    e->x2           = xpos2;
    e->y2           = ypos2;
    AppCommon::inputManager.queueEvent(e);
}
//-----------------------------------------------------------------------------
/*! Global event handler for the two finger touch up events of touchscreen
devices.
*/
void slTouch2Up(int sceneViewIndex,
                int xpos1,
                int ypos1,
                int xpos2,
                int ypos2)
{
    SLTouchEvent* e = new SLTouchEvent(SLInputEvent::Touch2Up);
    e->svIndex      = sceneViewIndex;
    e->x1           = xpos1;
    e->y1           = ypos1;
    e->x2           = xpos2;
    e->y2           = ypos2;
    AppCommon::inputManager.queueEvent(e);
}
//-----------------------------------------------------------------------------
/*! Global event handler for mouse wheel events.
 */
void slMouseWheel(int   sceneViewIndex,
                  int   pos,
                  SLKey modifier)
{
    SLMouseEvent* e = new SLMouseEvent(SLInputEvent::MouseWheel);
    e->svIndex      = sceneViewIndex;
    e->y            = pos;
    e->modifier     = modifier;
    AppCommon::inputManager.queueEvent(e);
}
//-----------------------------------------------------------------------------
/*! Global event handler for keyboard key press events.
 */
void slKeyPress(int   sceneViewIndex,
                SLKey key,
                SLKey modifier)
{
    SLKeyEvent* e = new SLKeyEvent(SLInputEvent::KeyDown);
    e->svIndex    = sceneViewIndex;
    e->key        = key;
    e->modifier   = modifier;
    AppCommon::inputManager.queueEvent(e);
}
//-----------------------------------------------------------------------------
/*! Global event handler for keyboard key release events.
 */
void slKeyRelease(int   sceneViewIndex,
                  SLKey key,
                  SLKey modifier)
{
    SLKeyEvent* e = new SLKeyEvent(SLInputEvent::KeyUp);
    e->svIndex    = sceneViewIndex;
    e->key        = key;
    e->modifier   = modifier;
    AppCommon::inputManager.queueEvent(e);
}

//-----------------------------------------------------------------------------
/*! Global event handler for unicode character input.
 */
void slCharInput(int          sceneViewIndex,
                 unsigned int character)
{
    SLCharInputEvent* e = new SLCharInputEvent();
    e->svIndex          = sceneViewIndex;
    e->character        = character;
    AppCommon::inputManager.queueEvent(e);
}
//-----------------------------------------------------------------------------
bool slUsesRotation()
{
    if (AppCommon::scene)
        return AppCommon::devRot.isUsed();
    return false;
}
//-----------------------------------------------------------------------------
/*! Global event handler for device rotation change with angle & and axis.
 */
void slRotationQUAT(float quatX,
                    float quatY,
                    float quatZ,
                    float quatW)
{
    AppCommon::devRot.onRotationQUAT(quatX, quatY, quatZ, quatW);
}
//-----------------------------------------------------------------------------
bool slUsesLocation()
{
    return AppCommon::devLoc.isUsed();
}
//-----------------------------------------------------------------------------
/*! Global event handler for device GPS location with longitude and latitude in
degrees and altitude in meters. This location uses the World Geodetic System
1984 (WGS 84). The accuracy in meters is a radius in which the location is with
a probability of 68% (2 sigma).
*/
void slLocationLatLonAlt(double latitudeDEG,
                         double longitudeDEG,
                         double altitudeM,
                         float  accuracyM)
{
    AppCommon::devLoc.onLocationLatLonAlt(latitudeDEG,
                                          longitudeDEG,
                                          altitudeM,
                                          accuracyM);
}
//-----------------------------------------------------------------------------
//! Global function to retrieve a window title generated by the scene library.
string slGetWindowTitle(int sceneViewIndex)
{
    SLSceneView* sv = AppCommon::sceneViews[(SLuint)sceneViewIndex];
    return sv->windowTitle();
}
//-----------------------------------------------------------------------------
// Get available external directories and inform slproject about them
void slSetupExternalDir(const SLstring& externalPath)
{
    if (Utils::dirExists(externalPath))
    {
        SL_LOG("Ext. directory   : %s", externalPath.c_str());
        AppCommon::externalPath = Utils::trimRightString(externalPath, "/") + "/";
    }
    else
    {
        SL_LOG("ERROR: external directory does not exists: %s", externalPath.c_str());
    }
}
//-----------------------------------------------------------------------------
//! Adds a value to the applications device parameter map
void slSetDeviceParameter(const SLstring& parameter,
                          SLstring        value)
{
    AppCommon::deviceParameter[parameter] = std::move(value);
}
//-----------------------------------------------------------------------------
