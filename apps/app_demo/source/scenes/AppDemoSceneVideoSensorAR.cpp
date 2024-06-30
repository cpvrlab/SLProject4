/**
 * \file      AppDemoSceneVideoSensorAR.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneVideoSensorAR.h>
#include <AppCommon.h>
#include <CVCapture.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLCoordAxis.h>
#include <SLBox.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;

//-----------------------------------------------------------------------------
AppDemoSceneVideoSensorAR::AppDemoSceneVideoSensorAR()
  : SLScene("Video Sensor AR")
{
    info("Minimal scene to test the devices IMU and GPS Sensors. "
         "See the sensor information. GPS needs a few sec. to improve the accuracy.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneVideoSensorAR::registerAssetsToLoad(SLAssetLoader& al)
{
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppCommon::texturePath +
                        "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneVideoSensorAR::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 60);
    cam1->lookAt(0, 0, 0);
    cam1->fov(CVCapture::instance()->activeCamera->calibration.cameraFovVDeg());
    cam1->clipNear(0.1f);
    cam1->clipFar(10000.0f);
    cam1->setInitialState();
    cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);
    cam1->background().texture(gVideoTexture);

    // Turn on main video
    CVCapture::instance()->videoType(VT_MAIN);

    // Create directional light for the sunlight
    SLLightDirect* light = new SLLightDirect(am, this, 1.0f);
    light->powers(1.0f, 1.0f, 1.0f);
    light->attenuation(1, 0, 0);

    // Let the sun be rotated by time and location
    AppCommon::devLoc.sunLightNode(light);

    SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
    axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
    axis->scale(2);
    axis->rotate(-90, 1, 0, 0);

    // Yellow center box
    SLMaterial* yellow = new SLMaterial(am,
                                        "mY",
                                        SLCol4f(1, 1, 0, 0.5f));
    SLNode*     box    = new SLNode(new SLBox(
                               am,
                               -.5f,
                               -.5f,
                               -.5f,
                               .5f,
                               .5f,
                               .5f,
                               "Box",
                               yellow),
                             "Box Node");

    // Scene structure
    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(light);
    scene->addChild(cam1);
    scene->addChild(box);
    scene->addChild(axis);

    sv->camera(cam1);

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
    // activate rotation and gps sensor
    AppCommon::devRot.isUsed(true);
    AppCommon::devRot.zeroYawAtStart(false);
    AppCommon::devLoc.isUsed(true);
    AppCommon::devLoc.useOriginAltitude(true);
    AppCommon::devLoc.hasOrigin(false);
    cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
    cam1->camAnim(SLCamAnim::CA_turntableYUp);
    AppCommon::devRot.zeroYawAtStart(true);
#endif

    sv->doWaitOnIdle(false); // for constant video feed
}
//-----------------------------------------------------------------------------
