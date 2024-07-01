/**
 * \file      AppAndroid.cpp
 * \brief     App::run implementation from App.h for the Android platform
 * \details   The functions implement mostly the callbacks for the platform
 *            Android over the JNI interface into SLInterface.h.
 *            For more info on how to create a new app with SLProject see:
 *            https://github.com/cpvrlab/SLProject4/wiki/Creating-a-New-App
 *            For more info about App framework see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      June 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <App.h>
#include <SLGLState.h>
#include <SLEnums.h>
#include <SLInterface.h>
#include <AppCommon.h>
#include <SLAssetManager.h>
#include <SLScene.h>
#include <SLSceneView.h>
#include <CVCapture.h>
#include <Profiler.h>
#include <SLAssetLoader.h>
#include "mediapipe.h"

#include <jni.h>

//-----------------------------------------------------------------------------
// Global variables
App::Config    App::config; //!< The configuration set in App::run
static JNIEnv* environment; //! Pointer to JAVA environment used in ray tracing callback
static int     svIndex = 0; //!< SceneView index

//-----------------------------------------------------------------------------
int slAndroidMain(int argc, char* argv[]);

//-----------------------------------------------------------------------------
/*! Java Native Interface (JNI) function declarations. These functions are
called by the Java interface class AppAndroidJNI. The function name follows the pattern
Java_{package name}_{JNI class name}_{function name}(JNIEnv* env,jclass obj,*);
The functions mostly forward to the C-Interface functions of SLProject declared
in SLInterface.h.
*/
extern "C" {
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onInit(JNIEnv* env, jclass obj, jint width, jint height, jint dpi, jstring filePath);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onTerminate(JNIEnv* env, jclass obj);
JNIEXPORT bool JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onPaintAllViews(JNIEnv* env, jclass obj);
JNIEXPORT bool JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onUpdate(JNIEnv* env, jclass obj);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onResize(JNIEnv* env, jclass obj, jint width, jint height);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onMouseDown(JNIEnv* env, jclass obj, jint button, jint x, jint y);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onMouseUp(JNIEnv* env, jclass obj, jint button, jint x, jint y);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onMouseMove(JNIEnv* env, jclass obj, jint x, jint y);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onTouch2Down(JNIEnv* env, jclass obj, jint x1, jint y1, jint x2, jint y2);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onTouch2Move(JNIEnv* env, jclass obj, jint x1, jint y1, jint x2, jint y2);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onTouch2Up(JNIEnv* env, jclass obj, jint x1, jint y1, jint x2, jint y2);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onDoubleClick(JNIEnv* env, jclass obj, jint button, jint x, jint y);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onClose(JNIEnv* env, jclass obj);
JNIEXPORT bool JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_usesRotation(JNIEnv* env, jclass obj);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onRotationQUAT(JNIEnv* env, jclass obj, jfloat quatX, jfloat quatY, jfloat quatZ, jfloat quatW);
JNIEXPORT bool JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_usesLocation(JNIEnv* env, jclass obj);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onLocationLatLonAlt(JNIEnv* env, jclass obj, jdouble latitudeDEG, jdouble longitudeDEG, jdouble altitudeM, jfloat accuracyM);
JNIEXPORT jint JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_getVideoType(JNIEnv* env, jclass obj);
JNIEXPORT jint JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_getVideoSizeIndex(JNIEnv* env, jclass obj);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_grabVideoFileFrame(JNIEnv* env, jclass obj);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_copyVideoImage(JNIEnv* env, jclass obj, jint imgWidth, jint imgHeight, jbyteArray srcBuffer);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onSetupExternalDir(JNIEnv* env, jclass obj, jstring externalDirPath);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_copyVideoYUVPlanes(JNIEnv* env, jclass obj, jint srcW, jint srcH, jbyteArray yBuf, jint ySize, jint yPixStride, jint yLineStride, jbyteArray uBuf, jint uSize, jint uPixStride, jint uLineStride, jbyteArray vBuf, jint vSize, jint vPixStride, jint vLineStride);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_setCameraSize(JNIEnv* env, jclass obj, jint sizeIndex, jint sizeIndexMax, jint width, jint height);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_setDeviceParameter(JNIEnv* env, jclass obj, jstring parameter, jstring value);
JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_initMediaPipeAssetManager(JNIEnv* env, jclass obj, jobject assetManager, jstring cacheDirPath);
};

//-----------------------------------------------------------------------------
int App::run(Config configuration)
{
    App::config = configuration;
    return 0;
}
//-----------------------------------------------------------------------------
//! Native ray tracing callback function that calls the Java class method AppAndroidJNI.RaytracingCallback
bool Java_renderRaytracingCallback()
{
    jclass    klass  = environment->FindClass("ch/bfh/cpvrlab/AppAndroidJNI");
    jmethodID method = environment->GetStaticMethodID(klass, "RaytracingCallback", "()Z");
    return environment->CallStaticBooleanMethod(klass, method);
}
//-----------------------------------------------------------------------------
//! Native OpenGL info string print functions used in onInit
static void printGLString(const char* name, GLenum s)
{
    const char* v = (const char*)glGetString(s);
    SL_LOG("GL %s = %s\n", name, v);
}
//-----------------------------------------------------------------------------
std::string jstring2stdstring(JNIEnv* env, jstring jStr)
{
    if (!jStr) return "";
    jboolean    isCopy;
    const char* chars = env->GetStringUTFChars(jStr, &isCopy);
    std::string stdString(chars);
    env->ReleaseStringUTFChars(jStr, chars);
    return stdString;
}
//-----------------------------------------------------------------------------
//! Creates the scene and sceneview instance
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onInit(JNIEnv* env, jclass obj, jint width, jint height, jint dpi, jstring filePath)
{
    environment              = env;
    const char* nativeString = env->GetStringUTFChars(filePath, 0);
    string      devicePath(nativeString);
    env->ReleaseStringUTFChars(filePath, nativeString);

    slAndroidMain(0, nullptr);
    const App::Config& config = App::config;

    SLVstring* cmdLineArgs = new SLVstring();

    SL_LOG("GUI            : Android");

    string device_path_msg = "Device path:" + devicePath;
    SL_LOG(device_path_msg.c_str(), 0);

    AppCommon::calibFilePath = devicePath + "/data/config/"; //that's where calibrations are stored an loaded from
    AppCommon::calibIniPath  = devicePath + "/data/calibrations/";
    CVCapture::instance()->loadCalibrations(Utils::ComputerInfos::get(), // deviceInfo string
                                            AppCommon::calibFilePath);     // for calibrations made

    ////////////////////////////////////////////////////
    slCreateApp(*cmdLineArgs,
                devicePath + "/data/",
                devicePath + "/data/shaders/",
                devicePath + "/data/models/",
                devicePath + "/data/images/textures/",
                devicePath + "/data/images/fonts/",
                devicePath + "/data/videos/",
                devicePath + "/",
                "AppDemoAndroid");

    slLoadCoreAssetsSync();

    ////////////////////////////////////////////////////////////////////
    svIndex = slCreateSceneView(AppCommon::assetManager,
                                AppCommon::scene,
                                static_cast<int>(width),
                                static_cast<int>(height),
                                static_cast<int>(dpi),
                                config.startSceneID,
                                nullptr,
                                nullptr,
                                reinterpret_cast<void*>(config.onNewSceneView),
                                reinterpret_cast<void*>(config.onGuiBuild),
                                reinterpret_cast<void*>(config.onGuiLoadConfig),
                                reinterpret_cast<void*>(config.onGuiSaveConfig));
    ////////////////////////////////////////////////////////////////////

    delete cmdLineArgs;
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onTerminate(JNIEnv* env, jclass obj)
{
    slTerminate();
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT bool JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onPaintAllViews(JNIEnv* env, jclass obj)
{
    return slPaintAllViews();
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT bool JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onUpdate(JNIEnv* env, jclass obj)
{
    if (AppCommon::sceneViews.empty())
        return false;

    SLSceneView* sv = AppCommon::sceneViews[svIndex];

    if (AppCommon::sceneToLoad)
    {
        slSwitchScene(sv, *AppCommon::sceneToLoad);
        AppCommon::sceneToLoad = {}; // sets optional to empty
    }

    if (AppCommon::assetLoader->isLoading())
        AppCommon::assetLoader->checkIfAsyncLoadingIsDone();

    ////////////////////////////////////////////////////////////////
    SLbool appNeedsUpdate  = App::config.onUpdate && App::config.onUpdate(sv);
    SLbool jobIsRunning    = slUpdateParallelJob();
    SLbool isLoading       = AppCommon::assetLoader->isLoading();
    SLbool viewNeedsUpdate = slPaintAllViews();
    ////////////////////////////////////////////////////////////////

    return appNeedsUpdate || viewNeedsUpdate || jobIsRunning || isLoading;
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onResize(JNIEnv* env, jclass obj, jint width, jint height)
{
    slResize(svIndex, width, height);
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onMouseDown(JNIEnv* env, jclass obj, jint button, jint x, jint y)
{
    slMouseDown(svIndex, (SLMouseButton)button, x, y, K_none);
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onMouseUp(JNIEnv* env, jclass obj, jint button, jint x, jint y)
{
    slMouseUp(svIndex, (SLMouseButton)button, x, y, K_none);
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onMouseMove(JNIEnv* env, jclass obj, jint x, jint y)
{
    slMouseMove(svIndex, x, y);
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onTouch2Down(JNIEnv* env, jclass obj, jint x1, jint y1, jint x2, jint y2)
{
    slTouch2Down(svIndex, x1, y1, x2, y2);
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onTouch2Move(JNIEnv* env, jclass obj, jint x1, jint y1, jint x2, jint y2)
{
    slTouch2Move(svIndex, x1, y1, x2, y2);
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onTouch2Up(JNIEnv* env, jclass obj, jint x1, jint y1, jint x2, jint y2)
{
    slTouch2Up(svIndex, x1, y1, x2, y2);
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onDoubleClick(JNIEnv* env, jclass obj, jint button, jint x, jint y)
{
    slDoubleClick(svIndex, MB_left, x, y, K_none);
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onRotationQUAT(JNIEnv* env, jclass obj, jfloat quatX, jfloat quatY, jfloat quatZ, jfloat quatW)
{
    slRotationQUAT(quatX, quatY, quatZ, quatW);
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onClose(JNIEnv* env, jclass obj)
{
    SL_LOG("onClose");
    slTerminate();
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT bool JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_usesRotation(JNIEnv* env, jclass obj)
{
    return slUsesRotation();
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT
  jint JNICALL
  Java_ch_bfh_cpvrlab_AppAndroidJNI_getVideoType(JNIEnv* env, jclass obj)
{
    return (int)CVCapture::instance()->videoType();
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT
  jint JNICALL
  Java_ch_bfh_cpvrlab_AppAndroidJNI_getVideoSizeIndex(JNIEnv* env, jclass obj)
{
    return CVCapture::instance()->activeCamera->camSizeIndex();
}
//-----------------------------------------------------------------------------
//! Grabs a frame from a video file using OpenCV
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_grabVideoFileFrame(JNIEnv* env, jclass obj)
{
    SLSceneView* sv      = AppCommon::sceneViews[0];
    CVCapture*   capture = CVCapture::instance();

    // Get the current capture size of the videofile
    CVSize videoSizeBefore = capture->captureSize;

    // If viewportWdivH is negative the viewport aspect will be adapted to the video
    // aspect ratio. No cropping will be applied.
    // Android doesn't know the video file frame size before grab
    float viewportWdivH = sv->viewportWdivH();
    if (sv->viewportSameAsVideo())
        viewportWdivH = -1;

    capture->grabAndAdjustForSL(viewportWdivH);

    // If video aspect has changed we need to tell the new viewport to the sceneview
    CVSize videoSizeAfter = capture->captureSize;
    if (sv->viewportSameAsVideo() && videoSizeBefore != videoSizeAfter)
        sv->setViewportFromRatio(SLVec2i(videoSizeAfter.width, videoSizeAfter.height),
                                 sv->viewportAlign(),
                                 sv->viewportSameAsVideo());
}
//-----------------------------------------------------------------------------
//! Copies the video image data to the CVCapture class
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_copyVideoImage(JNIEnv* env, jclass obj, jint imgWidth, jint imgHeight, jbyteArray imgBuffer)
{
    SLuchar* srcLumaPtr = reinterpret_cast<SLuchar*>(env->GetByteArrayElements(imgBuffer, 0));

    if (srcLumaPtr == nullptr)
        SL_EXIT_MSG("copyVideoImage: No image data pointer passed!");

    SLSceneView* sv            = AppCommon::sceneViews[0];
    CVCapture*   capture       = CVCapture::instance();
    float        videoImgWdivH = (float)imgWidth / (float)imgHeight;

    if (sv->viewportSameAsVideo())
    {
        // If video aspect has changed we need to tell the new viewport to the sceneview
        if (Utils::abs(videoImgWdivH - sv->viewportWdivH()) > 0.01f)
            sv->setViewportFromRatio(SLVec2i(imgWidth, imgHeight), sv->viewportAlign(), true);
    }

    capture->loadIntoLastFrame(sv->viewportWdivH(), imgWidth, imgHeight, PF_yuv_420_888, srcLumaPtr, true);
}
//-----------------------------------------------------------------------------
//! This function is not in use and was an attempt to copy the data faster.
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_copyVideoYUVPlanes(JNIEnv* env, jclass obj, jint srcW, jint srcH, jbyteArray yBuf, jint ySize, jint yPixStride, jint yLineStride, jbyteArray uBuf, jint uSize, jint uPixStride, jint uLineStride, jbyteArray vBuf, jint vSize, jint vPixStride, jint vLineStride)
{
    // Cast jbyteArray to unsigned char pointer
    SLuchar* y = reinterpret_cast<SLuchar*>(env->GetByteArrayElements(yBuf, 0));
    SLuchar* u = reinterpret_cast<SLuchar*>(env->GetByteArrayElements(uBuf, 0));
    SLuchar* v = reinterpret_cast<SLuchar*>(env->GetByteArrayElements(vBuf, 0));

    if (y == nullptr) SL_EXIT_MSG("copyVideoYUVPlanes: No pointer for y-buffer passed!");
    if (u == nullptr) SL_EXIT_MSG("copyVideoYUVPlanes: No pointer for u-buffer passed!");
    if (v == nullptr) SL_EXIT_MSG("copyVideoYUVPlanes: No pointer for v-buffer passed!");

    // If viewportWdivH is negative the viewport aspect will be adapted to the video
    // aspect ratio. No cropping will be applied.
    float viewportWdivH = AppCommon::sceneViews[0]->viewportWdivH();
    if (AppCommon::sceneViews[0]->viewportSameAsVideo())
        viewportWdivH = -1;

    CVCapture::instance()->copyYUVPlanes(viewportWdivH, srcW, srcH, y, ySize, yPixStride, yLineStride, u, uSize, uPixStride, uLineStride, v, vSize, vPixStride, vLineStride);
}
//-----------------------------------------------------------------------------
//! Copies the GPS information to the SLApplicaiton class
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onLocationLatLonAlt(JNIEnv* env,
                                                                                        jclass  obj,
                                                                                        jdouble latitudeDEG,
                                                                                        jdouble longitudeDEG,
                                                                                        jdouble altitudeM,
                                                                                        jfloat  accuracyM)
{
    slLocationLatLonAlt(latitudeDEG, longitudeDEG, altitudeM, accuracyM);
}
//-----------------------------------------------------------------------------
//! Asks the SLApplicaiton class if the GPS sensor data is requested
extern "C" JNIEXPORT bool JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_usesLocation(JNIEnv* env, jclass obj)
{
    return slUsesLocation();
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_onSetupExternalDir(JNIEnv* env,
                                                                                       jclass  obj,
                                                                                       jstring externalDirPath)
{
    std::string externalDirPathNative = jstring2stdstring(env, externalDirPath);
    slSetupExternalDir(externalDirPathNative);
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_setCameraSize(JNIEnv* env,
                                                                                  jclass  obj,
                                                                                  jint    sizeIndex,
                                                                                  jint    sizeIndexMax,
                                                                                  jint    width,
                                                                                  jint    height)
{
    CVCapture::instance()->setCameraSize(sizeIndex, sizeIndexMax, width, height);
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_setDeviceParameter(JNIEnv* env,
                                                                                       jclass  obj,
                                                                                       jstring parameter,
                                                                                       jstring value)
{
    std::string par = jstring2stdstring(env, parameter);
    std::string val = jstring2stdstring(env, value);
    slSetDeviceParameter(par.c_str(), val.c_str());
}
//-----------------------------------------------------------------------------
extern "C" JNIEXPORT void JNICALL Java_ch_bfh_cpvrlab_AppAndroidJNI_initMediaPipeAssetManager(JNIEnv* env,
                                                                                              jclass  obj,
                                                                                              jobject assetManager,
                                                                                              jstring cacheDirPath)
{
    mp_init_asset_manager(env, assetManager, cacheDirPath);
}
//-----------------------------------------------------------------------------
