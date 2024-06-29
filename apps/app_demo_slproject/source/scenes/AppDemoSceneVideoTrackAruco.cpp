/**
 * \file      AppDemoSceneVideoTrackAruco.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneVideoTrackAruco.h>
#include <AppDemo.h>
#include <CVCapture.h>
#include <CVTrackedAruco.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLBox.h>
#include <SLCoordAxis.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;
extern CVTracked*   gVideoTracker;
extern SLNode*      gVideoTrackedNode;

//-----------------------------------------------------------------------------
AppDemoSceneVideoTrackAruco::AppDemoSceneVideoTrackAruco(SLSceneID sid)
  : SLScene("Aruco Marker Tracking"),
    _sceneID(sid)
{
    if (_sceneID == SID_VideoTrackArucoMain)
    {
        name("Track Aruco (main cam.)");
        info("Hold the Aruco board dictionary 0 into the field of view of "
             "the main camera. You can find the Aruco markers in the file "
             "data/Calibrations. If not all markers are tracked you may have "
             "the mirror the video horizontally.");
    }
    else
    {
        name("Track Aruco (scnd. cam.)");
        info("Hold the Aruco board dictionary 0 into the field of view of "
             "the secondary camera. You can find the Aruco markers in the file "
             "data/Calibrations. If not all markers are tracked you may have "
             "the mirror the video horizontally.");
    }
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneVideoTrackAruco::registerAssetsToLoad(SLAssetLoader& al)
{
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppDemo::texturePath +
                        "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);

    // Create an ArUco tracker
    al.addLoadTask([]() {
        gVideoTracker = new CVTrackedAruco(9, AppDemo::calibIniPath);
        gVideoTracker->drawDetection(true);
    });
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneVideoTrackAruco::assemble(SLAssetManager* am, SLSceneView* sv)
{
    /*
    The tracking of markers is done in AppDemoVideo::onUpdateVideo by calling
    the specific CVTracked::track method. If a marker was found it overwrites
    the linked nodes object matrix (SLNode::_om). If the linked node is the
    active camera the found transform is additionally inversed.
    This would be the standard augmented reality use case.
    */

    if (_sceneID == SID_VideoTrackArucoMain)
        CVCapture::instance()->videoType(VT_MAIN);
    else
        CVCapture::instance()->videoType(VT_SCND);

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
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
    scene->addChild(cam1);

    // Create a light source node
    SLLightSpot* light1 = new SLLightSpot(am, this, 0.02f);
    light1->translation(0.12f, 0.12f, 0.12f);
    light1->name("light node");
    scene->addChild(light1);

    // Get the half edge length of the aruco marker
    SLfloat edgeLen = static_cast<CVTrackedAruco*>(gVideoTracker)->params().edgeLength;
    SLfloat he      = edgeLen * 0.5f;

    // Build mesh & node that will be tracked by the 1st marker (camera)
    SLBox*  box1      = new SLBox(am,
                            -he,
                            -he,
                            0.0f,
                            he,
                            he,
                            2 * he,
                            "Box 1",
                            yellow);
    SLNode* boxNode1  = new SLNode(box1,
                                  "Box Node 1");
    SLNode* axisNode1 = new SLNode(new SLCoordAxis(am),
                                   "Axis Node 1");
    axisNode1->setDrawBitsRec(SL_DB_MESHWIRED, false);
    axisNode1->scale(edgeLen);
    boxNode1->addChild(axisNode1);
    boxNode1->setDrawBitsRec(SL_DB_CULLOFF, true);
    scene->addChild(boxNode1);

    // The tracker moves the box node
    gVideoTrackedNode = boxNode1;

    // Set active camera
    sv->camera(cam1);

    // Turn on constant redraw
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
