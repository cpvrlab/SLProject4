/**
 * \file      AppDemoSceneVideoTexture.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see:
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include <AppDemoSceneVideoTexture.h>
#include <AppCommon.h>
#include <AppDemoSceneID.h>
#include <CVCapture.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <SLSphere.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;

//-----------------------------------------------------------------------------
AppDemoSceneVideoTexture::AppDemoSceneVideoTexture(SLSceneID sid)
  : SLScene("Texture from Video"),
    _sceneID(sid)
{
    // Set scene name and info string
    if (_sceneID == SID_VideoTextureLive)
        info("Minimal texture mapping example with live video source.");
    else
        info("Minimal texture mapping example with video file source.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneVideoTexture::registerAssetsToLoad(SLAssetLoader& al)
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
void AppDemoSceneVideoTexture::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Set scene name and info string
    if (_sceneID == SID_VideoTextureLive)
    {
        // on desktop, it will be the main camera
        CVCapture::instance()->videoType(VT_MAIN);
    }
    else
    {
        CVCapture::instance()->videoType(VT_FILE);
        CVCapture::instance()->videoFilename = AppCommon::videoPath + "street3.mp4";
        CVCapture::instance()->videoLoops    = true;
    }

    sv->viewportSameAsVideo(true);

    // Create video texture on global pointer updated in AppDemoVideo
    SLMaterial* m1 = new SLMaterial(am, "VideoMat", gVideoTexture);

    // Create a root scene group for all nodes
    SLNode* scene = new SLNode("scene node");
    root3D(scene);

    // Create a camera node
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 20);
    cam1->focalDist(20);
    cam1->lookAt(0, 0, 0);
    cam1->background().texture(gVideoTexture);
    cam1->setInitialState();
    cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);
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
