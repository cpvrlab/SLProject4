/**
 * \file      AppDemoSceneLargeModel.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneLargeModel.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>

//-----------------------------------------------------------------------------
AppDemoSceneLargeModel::AppDemoSceneLargeModel()
  : SLScene("Large Model Benchmark Scene")
{
    info("Large Model with 7.2 mio. triangles.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneLargeModel::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addNodeToLoad(_dragonModel,
                     AppCommon::modelPath +
                       "PLY/xyzrgb_dragon/xyzrgb_dragon.ply",
                     nullptr,
                     false,
                     true,
                     nullptr,
                     0.2f,
                     false,
                     SLProcess_Triangulate |
                       SLProcess_JoinIdenticalVertices);
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneLargeModel::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 220);
    cam1->lookAt(0, 0, 0);
    cam1->clipNear(1);
    cam1->clipFar(10000);
    cam1->focalDist(220);
    cam1->background().colors(SLCol4f(0.7f, 0.7f, 0.7f),
                              SLCol4f(0.2f, 0.2f, 0.2f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);

    // Material for glass
    SLMaterial* diffuseMat = new SLMaterial(am,
                                            "diffuseMat",
                                            SLCol4f::WHITE,
                                            SLCol4f::WHITE);
    _dragonModel->mesh()->mat(diffuseMat);

    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          200,
                                          200,
                                          200,
                                          1);
    light1->powers(0.1f, 1.0f, 1.0f);
    light1->attenuation(1, 0, 0);

    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(light1);
    scene->addChild(_dragonModel);
    scene->addChild(cam1);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
