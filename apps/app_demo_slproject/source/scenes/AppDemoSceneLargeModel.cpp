//#############################################################################
//  File:      AppDemoSceneLargeModel.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneLargeModel.h>
#include <AppDemo.h>
#include <SLLightSpot.h>

//-----------------------------------------------------------------------------
AppDemoSceneLargeModel::AppDemoSceneLargeModel()
  : AppScene("Large Model Benchmark Scene")
{
    info("Large Model with 7.2 mio. triangles.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneLargeModel::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addNodeToLoad(_dragonModel,
                     AppDemo::modelPath +
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
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

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
