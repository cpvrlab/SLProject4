//#############################################################################
//  File:      AppDemoSceneVideoTexture.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneVideoTexture.h>
#include <CVCapture.h>
#include <AppDemo.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <SLSphere.h>

//-----------------------------------------------------------------------------
AppDemoSceneVideoTexture::AppDemoSceneVideoTexture(SLSceneID sid)
  : AppScene("Video Texture")
{
    // Set scene name and info string
    if (sid == SID_VideoTextureLive)
    {
        name("Live Video Texture");
        info("Minimal texture mapping example with live video source.");
        CVCapture::instance()->videoType(VT_MAIN); // on desktop, it will be the main camera
    }
    else
    {
        name("File Video Texture");
        info("Minimal texture mapping example with video file source.");
        CVCapture::instance()->videoType(VT_FILE);
        CVCapture::instance()->videoFilename = AppDemo::videoPath + "street3.mp4";
        CVCapture::instance()->videoLoops    = true;
    }
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneVideoTexture::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_videoTex, "LiveVideoError.png");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneVideoTexture::assemble(SLAssetManager* am, SLSceneView* sv)
{
    sv->viewportSameAsVideo(true);

    // Create video texture on global pointer updated in AppDemoVideo
    SLMaterial* m1 = new SLMaterial(am, "VideoMat", _videoTex);

    // Create a root scene group for all nodes
    SLNode* scene = new SLNode("scene node");
    root3D(scene);

    // Create a camera node
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 20);
    cam1->focalDist(20);
    cam1->lookAt(0, 0, 0);
    cam1->background().texture(_videoTex);
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
    scene->addChild(cam1);

    // Create rectangle meshe and nodes
    SLfloat h        = 5.0f;
    SLfloat w        = h * sv->viewportWdivH();
    SLMesh* rectMesh = new SLRectangle(am,
                                       SLVec2f(-w, -h),
                                       SLVec2f(w, h),
                                       1,
                                       1,
                                       "rect mesh",
                                       m1);
    SLNode* rectNode = new SLNode(rectMesh, "rect node");
    rectNode->translation(0, 0, -5);
    scene->addChild(rectNode);

    // Center sphere
    SLNode* sphere = new SLNode(new SLSphere(am,
                                             2,
                                             32,
                                             32,
                                             "Sphere",
                                             m1));
    sphere->rotate(-90, 1, 0, 0);
    scene->addChild(sphere);

    // Create a light source node
    SLLightSpot* light1 = new SLLightSpot(am, this, 0.3f);
    light1->translation(0, 0, 5);
    light1->lookAt(0, 0, 0);
    light1->name("light node");
    scene->addChild(light1);

    sv->camera(cam1);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
