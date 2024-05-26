//#############################################################################
//  File:      AppDemoSceneVideoChessboard.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneVideoTrackChessboard.h>
#include <AppDemo.h>
#include <CVCapture.h>
#include <CVTrackedChessboard.h>
#include <CVCalibrationEstimator.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLBox.h>
#include <SLCoordAxis.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;
extern CVTracked*   gVideoTracker;
extern SLNode*      gVideoTrackedNode;

//-----------------------------------------------------------------------------
AppDemoSceneVideoTrackChessboard::AppDemoSceneVideoTrackChessboard(SLSceneID sid)
  : SLScene("Chessboard Video"),
    _sceneID(sid)
{
    switch (_sceneID)
    {
        case SID_VideoTrackChessMain: name("Track Chessboard (main cam.)"); break;
        case SID_VideoTrackChessScnd: name("Track Chessboard (scnd.cam.)"); break;
        case SID_VideoCalibrateMain: name("Calibrate Main Camera"); break;
        case SID_VideoCalibrateScnd: name("Calibrate Scnd. Camera"); break;
        default: name("Unknow SceneID");
    }
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneVideoTrackChessboard::registerAssetsToLoad(SLAssetLoader& al)
{
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppDemo::texturePath +
                        "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);

    // Create a chessboard tracker
    al.addLoadTask([]() {
        gVideoTracker = new CVTrackedChessboard(AppDemo::calibIniPath);
        gVideoTracker->drawDetection(true);
    });
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneVideoTrackChessboard::assemble(SLAssetManager* am,
                                                SLSceneView*    sv)
{
    /*
    The tracking of markers is done in AppDemoVideo::onUpdateTracking by
    calling the specific CVTracked::track method. If a marker was found it
    overwrites the linked nodes object matrix (SLNode::_om).
    If the linked node is the active camera the found transform is additionally
    inversed. This would be the standard augmented realty use case.
    The chessboard marker used in these scenes is also used for the camera
    calibration. The different calibration state changes are also handled in
    AppDemoVideo::onUpdateVideo.
    */
    // Setup here only the requested scene.
    if (_sceneID == SID_VideoTrackChessMain ||
        _sceneID == SID_VideoTrackChessScnd)
    {
        if (_sceneID == SID_VideoTrackChessMain)
            CVCapture::instance()->videoType(VT_MAIN);
        else
            CVCapture::instance()->videoType(VT_SCND);
    }
    else if (_sceneID == SID_VideoCalibrateMain)
    {
        if (AppDemo::calibrationEstimator)
        {
            delete AppDemo::calibrationEstimator;
            AppDemo::calibrationEstimator = nullptr;
        }
        CVCapture::instance()->videoType(VT_MAIN);
    }
    else if (_sceneID == SID_VideoCalibrateScnd)
    {
        if (AppDemo::calibrationEstimator)
        {
            delete AppDemo::calibrationEstimator;
            AppDemo::calibrationEstimator = nullptr;
        }
        CVCapture::instance()->videoType(VT_SCND);
    }

    // Material
    SLMaterial* yellow = new SLMaterial(am,
                                        "mY",
                                        SLCol4f(1, 1, 0, 0.5f));

    // set the edge length of a chessboard square
    SLfloat e1 = 0.028f;
    SLfloat e3 = e1 * 3.0f;
    SLfloat e9 = e3 * 3.0f;

    // Create a scene group node
    SLNode* scene = new SLNode("scene node");

    // Create a camera node
    SLCamera* cam1 = new SLCamera();
    cam1->name("camera node");
    cam1->translation(0, 0, 5);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(5);
    cam1->clipFar(10);
    cam1->fov(CVCapture::instance()->activeCamera->calibration.cameraFovVDeg());
    cam1->background().texture(gVideoTexture);
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
    scene->addChild(cam1);

    // Create a light source node
    SLLightSpot* light1 = new SLLightSpot(am, this, e1 * 0.5f);
    light1->translate(e9, e9, e9);
    light1->name("light node");
    scene->addChild(light1);

    // Build mesh & node
    if (_sceneID == SID_VideoTrackChessMain ||
        _sceneID == SID_VideoTrackChessScnd)
    {
        SLBox*  box     = new SLBox(am,
                               0.0f,
                               0.0f,
                               0.0f,
                               e3,
                               e3,
                               e3,
                               "Box",
                               yellow);
        SLNode* boxNode = new SLNode(box,
                                     "Box Node");
        boxNode->setDrawBitsRec(SL_DB_CULLOFF, true);
        SLNode* axisNode = new SLNode(new SLCoordAxis(am),
                                      "Axis Node");
        axisNode->setDrawBitsRec(SL_DB_MESHWIRED, false);
        axisNode->scale(e3);
        boxNode->addChild(axisNode);
        scene->addChild(boxNode);
    }

    // The tracker moves the camera node
    gVideoTrackedNode = cam1;

    // pass the scene group as root node
    root3D(scene);

    // Set active camera
    sv->camera(cam1);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
