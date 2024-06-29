/**
 * \file      AppDemoSceneVideoTrackWAI.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneVideoTrackWAI.h>
#include <AppDemo.h>
#include <CVCapture.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLBox.h>
#include <SLCoordAxis.h>

#ifdef SL_BUILD_WAI
#    include <CVTrackedWAI.h>
#endif

#ifdef SL_BUILD_WAI
// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;
extern CVTracked*   gVideoTracker;
extern SLNode*      gVideoTrackedNode;
#endif

//-----------------------------------------------------------------------------
AppDemoSceneVideoTrackWAI::AppDemoSceneVideoTrackWAI()
  : SLScene("Feature Tracking with ORBSLAM library")
{
    info("Feature Tracking with ORBSLAM library");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneVideoTrackWAI::registerAssetsToLoad(SLAssetLoader& al)
{
#ifdef SL_BUILD_WAI
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppDemo::texturePath +
                        "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);

    al.addLoadTask([] {
        // Create OpenCV Tracker for the box node
        std::string vocFileName;
#    if USE_FBOW
        vocFileName = "voc_fbow.bin";
#    else
        vocFileName = "ORBvoc.bin";
#    endif
        gVideoTracker = new CVTrackedWAI(Utils::findFile(vocFileName, {AppDemo::calibIniPath, AppDemo::exePath}));
        gVideoTracker->drawDetection(true);
    });
#endif
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneVideoTrackWAI::assemble(SLAssetManager* am,
                                         SLSceneView*    sv)
{
#ifdef SL_BUILD_WAI
    CVCapture::instance()->videoType(VT_MAIN);

    // Material
    SLMaterial* yellow = new SLMaterial(am,
                                        "mY",
                                        SLCol4f(1, 1, 0, 0.5f));
    SLMaterial* cyan   = new SLMaterial(am,
                                      "mY",
                                      SLCol4f(0, 1, 1, 0.5f));

    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    root3D(scene);

    // Create a camera node 1
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 5);
    cam1->lookAt(0, 0, 0);
    cam1->fov(CVCapture::instance()->activeCamera->calibration.cameraFovVDeg());
    cam1->background().texture(gVideoTexture);
    cam1->setInitialState();
    scene->addChild(cam1);

    // Create a light source node
    SLLightSpot* light1 = new SLLightSpot(am, this, 0.02f);
    light1->translation(0.12f, 0.12f, 0.12f);
    light1->name("light node");
    scene->addChild(light1);

    // Get the half edge length of the aruco marker
    SLfloat edgeLen = 0.1f;
    SLfloat he      = edgeLen * 0.5f;

    // Build mesh & node that will be tracked by the 1st marker (camera)
    SLBox*  box1      = new SLBox(am,
                            -he,
                            -he,
                            -he,
                            he,
                            he,
                            he,
                            "Box 1",
                            yellow);
    SLNode* boxNode1  = new SLNode(box1, "Box Node 1");
    SLNode* axisNode1 = new SLNode(new SLCoordAxis(am), "Axis Node 1");
    axisNode1->setDrawBitsRec(SL_DB_MESHWIRED, false);
    axisNode1->scale(edgeLen);
    axisNode1->translate(-he, -he, -he, TS_parent);
    boxNode1->addChild(axisNode1);
    boxNode1->setDrawBitsRec(SL_DB_CULLOFF, true);
    boxNode1->translate(0.0f, 0.0f, 1.0f, TS_world);
    scene->addChild(boxNode1);

    // The tracker moves the box node
    gVideoTrackedNode = cam1;

    sv->camera(cam1);
    sv->doWaitOnIdle(false);
#endif
}
//-----------------------------------------------------------------------------
