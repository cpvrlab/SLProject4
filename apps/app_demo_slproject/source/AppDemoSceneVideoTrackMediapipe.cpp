//#############################################################################
//  File:      AppDemoSceneVideoTrackMediapipe.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneVideoTrackMediapipe.h>
#include <CVCapture.h>
#include <CVTrackedMediaPipeHands.h>
#include <AppDemo.h>
#include <CVTracked.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;
extern CVTracked*   gVideoTracker;
extern SLNode*      gVideoTrackedNode;

//-----------------------------------------------------------------------------
AppDemoSceneVideoTrackMediapipe::AppDemoSceneVideoTrackMediapipe()
  : AppScene("Mediapipe Hand Tracking")
{
    info("Mediapipe Hand Tracking");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneVideoTrackMediapipe::registerAssetsToLoad(SLAssetLoader& al)
{
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

    gVideoTexture = new SLGLTexture(am,
                                    AppDemo::texturePath + "LiveVideoError.png",
                                    GL_LINEAR,
                                    GL_LINEAR);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->background().texture(gVideoTexture);

    SLNode* scene = new SLNode("Scene");
    root3D(scene);

    gVideoTracker     = new CVTrackedMediaPipeHands(AppDemo::dataPath);
    gVideoTrackedNode = cam1;
    gVideoTracker->drawDetection(true);

    sv->doWaitOnIdle(false);
    sv->camera(cam1);
#endif
}
//-----------------------------------------------------------------------------
