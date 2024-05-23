//#############################################################################
//  File:      AppDemoSceneVideoTrackFace.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneVideoTrackFace.h>
#include <CVCapture.h>
#include <CVTrackedFaces.h>
#include <AppDemo.h>
#include <SLLightSpot.h>
#include <SLBox.h>
#include <SLCoordAxis.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;
extern CVTracked*   gVideoTracker;
extern SLNode*      gVideoTrackedNode;

//-----------------------------------------------------------------------------
AppDemoSceneVideoTrackFace::AppDemoSceneVideoTrackFace(SLSceneID sid)
  : AppScene("Facial Feature Tracking"),
    _sceneID(sid)
{
    if (_sceneID == SID_VideoTrackFaceMain)
        name("Track Face (main cam.)");
    else
        name("Track Face (scnd. cam.)");
    info("Face and facial landmark detection.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneVideoTrackFace::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addNodeToLoad(_glasses, "FBX/Sunglasses.fbx");

    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppDemo::texturePath,
                        "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);

    // Create a face tracker
    al.addLoadTask([]() {
        gVideoTracker = new CVTrackedFaces(AppDemo::calibIniPath + "haarcascade_frontalface_alt2.xml",
                                           AppDemo::calibIniPath + "lbfmodel.yaml",
                                           3);
        gVideoTracker->drawDetection(true);
    });
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneVideoTrackFace::assemble(SLAssetManager* am, SLSceneView* sv)
{
#ifndef SL_EMSCRIPTEN
    /*
    The tracking of markers is done in AppDemoVideo::onUpdateVideo by calling
    the specific CVTracked::track method. If a marker was found it overwrites
    the linked nodes object matrix (SLNode::_om). If the linked node is the
    active camera the found transform is additionally inversed.
    This would be the standard augmented reality use case.
    */
    if (_sceneID == SID_VideoTrackFaceMain)
        CVCapture::instance()->videoType(VT_MAIN);
    else
        CVCapture::instance()->videoType(VT_SCND);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 0.5f);
    cam1->clipNear(0.1f);
    cam1->clipFar(1000.0f);
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
    cam1->background().texture(gVideoTexture);

    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          10,
                                          10,
                                          10,
                                          1);
    light1->powers(1.0f, 1.0f, 1.0f);
    light1->attenuation(1, 0, 0);

    // configure sunglasses
    _glasses->scale(0.008f);
    _glasses->translate(0, 1.5f, 0);

    // Add axis arrows at world center
    SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
    axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
    axis->scale(0.03f);

    // Scene structure
    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(light1);
    scene->addChild(cam1);
    scene->addChild(_glasses);
    scene->addChild(axis);

    // The tracker moves the camera node
    gVideoTrackedNode = cam1;

    sv->doWaitOnIdle(false); // for constant video feed
    sv->camera(cam1);
#endif
}
//-----------------------------------------------------------------------------
