/**
 * \file      AppDemoSceneVideoTrackMediapipe.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneVideoTrackMediapipe.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <CVCapture.h>
#include <CVTrackedMediaPipeHands.h>
#include <CVTracked.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;
extern CVTracked*   gVideoTracker;
extern SLNode*      gVideoTrackedNode;

//-----------------------------------------------------------------------------
AppDemoSceneVideoTrackMediapipe::AppDemoSceneVideoTrackMediapipe()
  : SLScene("Mediapipe Hand Tracking")
{
    info("Mediapipe Hand Tracking");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneVideoTrackMediapipe::registerAssetsToLoad(SLAssetLoader& al)
{
#ifdef SL_BUILD_WITH_MEDIAPIPE
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppCommon::texturePath +
                        "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);

    // Create MediaPipe hand tracker
    al.addLoadTask([] {
        gVideoTracker = new CVTrackedMediaPipeHands(AppCommon::dataPath);
        gVideoTracker->drawDetection(true);
    });
#endif
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneVideoTrackMediapipe::assemble(SLAssetManager* am,
                                               SLSceneView*    sv)
{
    /*
    The tracking of markers is done in AppDemoVideo::onUpdateVideo by calling
    the specific CVTracked::track method. If a marker was found it overwrites
    the linked nodes object matrix (SLNode::_om). If the linked node is the
    active camera the found transform is additionally inversed. This would be
    the standard augmented reality use case.
    */

#ifdef SL_BUILD_WITH_MEDIAPIPE
    CVCapture::instance()->videoType(VT_MAIN);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->background().texture(gVideoTexture);

    SLNode* scene = new SLNode("Scene");
    root3D(scene);

    // The tracker moves the camera
    gVideoTrackedNode = cam1;

    sv->doWaitOnIdle(false);
    sv->camera(cam1);
#endif
}
//-----------------------------------------------------------------------------
