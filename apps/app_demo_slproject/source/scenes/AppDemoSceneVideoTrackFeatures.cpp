/**
 * \file      AppDemoSceneVideoTrackFeatures.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneVideoTrackFeatures.h>
#include <AppDemo.h>
#include <CVCapture.h>
#include <CVTrackedFeatures.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLBox.h>
#include <SLCoordAxis.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;
extern CVTracked*   gVideoTracker;
extern SLNode*      gVideoTrackedNode;

//-----------------------------------------------------------------------------
AppDemoSceneVideoTrackFeatures::AppDemoSceneVideoTrackFeatures()
  : SLScene("2D Feature Tracking")
{
    info("Augmented Reality 2D Feature Tracking: You need to print out the "
         "stones image target from the file data/calibrations/vuforia_markers.pdf");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneVideoTrackFeatures::registerAssetsToLoad(SLAssetLoader& al)
{
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppDemo::texturePath +
                        "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);

    // Create feature tracker
    al.addLoadTask([]() {
        gVideoTracker = new CVTrackedFeatures(AppDemo::texturePath + "features_stones.jpg");
        gVideoTracker->drawDetection(true);
    });
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneVideoTrackFeatures::assemble(SLAssetManager* am,
                                              SLSceneView*    sv)
{
    /*
    The tracking of markers is done in AppDemoVideo::onUpdateVideo by calling
    the specific CVTracked::track method. If a marker was found it overwrites
    the linked nodes object matrix (SLNode::_om). If the linked node is the
    active camera the found transform is additionally inversed. This would be
    the standard augmented reality use case.
    */

    CVCapture::instance()->videoType(VT_MAIN);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 2, 60);
    cam1->lookAt(15, 15, 0);
    cam1->clipNear(0.1f);
    cam1->clipFar(1000.0f); // Increase to infinity?
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
    cam1->background().texture(gVideoTexture);

    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          420,
                                          420,
                                          420,
                                          1);
    light1->powers(1.0f, 1.0f, 1.0f);

    // Coordinate axis node
    SLNode* axis = new SLNode(new SLCoordAxis(am),
                              "Axis Node");
    axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
    axis->scale(100);
    axis->rotate(-90, 1, 0, 0);

    // Yellow center box
    SLMaterial* yellow = new SLMaterial(am,
                                        "mY",
                                        SLCol4f(1, 1, 0, 0.5f));
    SLNode*     box    = new SLNode(new SLBox(am,
                                       0,
                                       0,
                                       0,
                                       100,
                                       100,
                                       100,
                                       "Box",
                                       yellow),
                             "Box Node");
    box->rotate(-90, 1, 0, 0);

    // Scene structure
    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(light1);
    scene->addChild(axis);
    scene->addChild(box);
    scene->addChild(cam1);

    // The tracker moves the camera
    gVideoTrackedNode = cam1;

    sv->doWaitOnIdle(false); // for constant video feed
    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
