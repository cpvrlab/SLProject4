/**
 * \file      AppDemoScene2Dand3DText.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoScene2Dand3DText.h>
#include <AppCommon.h>
#include <SLAssetManager.h>
#include <SLLightSpot.h>
#include <SLText.h>
#include <SLSphere.h>

//-----------------------------------------------------------------------------
AppDemoScene2Dand3DText::AppDemoScene2Dand3DText() : SLScene("2D and 3D Text Scene")
{
    info("All 3D objects are in the _root3D scene and the center text is in the _root2D scene "
         "and rendered in orthographic projection in screen space.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoScene2Dand3DText::registerAssetsToLoad(SLAssetLoader& al)
{
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoScene2Dand3DText::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLMaterial* m1 = new SLMaterial(am, "m1", SLCol4f::RED);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->clipNear(0.1f);
    cam1->clipFar(100);
    cam1->translation(0, 0, 5);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(5);
    cam1->background().colors(SLCol4f(0.1f, 0.1f, 0.1f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);

    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          10,
                                          10,
                                          10,
                                          0.3f);
    light1->powers(0.2f, 0.8f, 1.0f);
    light1->attenuation(1, 0, 0);

    // Because all text objects get their sizes in pixels we have to scale them down
    SLfloat  scale = 0.01f;
    SLstring txt   = "This is text in 3D with font07";
    SLVec2f  size  = SLAssetManager::font07->calcTextSize(txt);
    SLNode*  t07   = new SLText(txt, SLAssetManager::font07);
    t07->translate(-size.x * 0.5f * scale, 1.0f, 0);
    t07->scale(scale);

    txt         = "This is text in 3D with font09";
    size        = SLAssetManager::font09->calcTextSize(txt);
    SLNode* t09 = new SLText(txt, SLAssetManager::font09);
    t09->translate(-size.x * 0.5f * scale, 0.8f, 0);
    t09->scale(scale);

    txt         = "This is text in 3D with font12";
    size        = SLAssetManager::font12->calcTextSize(txt);
    SLNode* t12 = new SLText(txt, SLAssetManager::font12);
    t12->translate(-size.x * 0.5f * scale, 0.6f, 0);
    t12->scale(scale);

    txt         = "This is text in 3D with font20";
    size        = SLAssetManager::font20->calcTextSize(txt);
    SLNode* t20 = new SLText(txt, SLAssetManager::font20);
    t20->translate(-size.x * 0.5f * scale, -0.8f, 0);
    t20->scale(scale);

    txt         = "This is text in 3D with font22";
    size        = SLAssetManager::font22->calcTextSize(txt);
    SLNode* t22 = new SLText(txt, SLAssetManager::font22);
    t22->translate(-size.x * 0.5f * scale, -1.2f, 0);
    t22->scale(scale);

    // Now create 2D text but don't scale it (all sizes in pixels)
    txt           = "This is text in 2D with font16";
    size          = SLAssetManager::font16->calcTextSize(txt);
    SLNode* t2D16 = new SLText(txt, SLAssetManager::font16);
    t2D16->translate(-size.x * 0.5f, 0, 0);

    // Assemble 3D scene as usual with camera and light
    SLNode* scene3D = new SLNode("root3D");
    this->root3D(scene3D);
    scene3D->addChild(cam1);
    scene3D->addChild(light1);
    scene3D->addChild(new SLNode(new SLSphere(am,
                                              0.5f,
                                              32,
                                              32,
                                              "Sphere",
                                              m1)));
    scene3D->addChild(t07);
    scene3D->addChild(t09);
    scene3D->addChild(t12);
    scene3D->addChild(t20);
    scene3D->addChild(t22);

    // Assemble 2D scene
    SLNode* scene2D = new SLNode("root2D");
    this->root2D(scene2D);
    scene2D->addChild(t2D16);

    sv->camera(cam1);
    sv->doWaitOnIdle(true);
}
//-----------------------------------------------------------------------------
