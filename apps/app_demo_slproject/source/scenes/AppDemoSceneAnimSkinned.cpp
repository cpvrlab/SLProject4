//#############################################################################
//  File:      AppDemoSceneAnimSkinned.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneAnimSkinned.h>
#include <AppDemo.h>
#include <SLLightSpot.h>
#include <SLGrid.h>

//-----------------------------------------------------------------------------
AppDemoSceneAnimSkinned::AppDemoSceneAnimSkinned()
  : AppScene("Skeletal Animation Test Scene")
{
    info("Skeletal Animation Test Scene");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneAnimSkinned::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addNodeToLoad(_char1,
                     AppDemo::modelPath,
                     "DAE/AstroBoy/AstroBoy.dae");
    al.addNodeToLoad(_char2,
                     AppDemo::modelPath,
                     "GLTF/Sintel/Sintel_LowRes-Rigged.gltf");
    al.addNodeToLoad(_cube1,
                     AppDemo::modelPath,
                     "DAE/SkinnedCube/skinnedcube2.dae");
    al.addNodeToLoad(_cube2,
                     AppDemo::modelPath,
                     "DAE/SkinnedCube/skinnedcube4.dae");
    al.addNodeToLoad(_cube3,
                     AppDemo::modelPath,
                     "DAE/SkinnedCube/skinnedcube5.dae");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneAnimSkinned::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Root scene node
    SLNode* scene = new SLNode("scene group");
    root3D(scene);

    // camera
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 2, 10);
    cam1->lookAt(0, 2, 0);
    cam1->focalDist(10);
    cam1->setInitialState();
    cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
    scene->addChild(cam1);

    // light
    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          10,
                                          10,
                                          5,
                                          0.5f);
    light1->powers(0.2f, 1.0f, 1.0f);
    light1->attenuation(1, 0, 0);
    scene->addChild(light1);

    // Floor grid
    SLMaterial* m2   = new SLMaterial(am, "m2", SLCol4f::WHITE);
    SLGrid*     grid = new SLGrid(am,
                              SLVec3f(-5, 0, -5),
                              SLVec3f(5, 0, 5),
                              20,
                              20,
                              "Grid",
                              m2);
    scene->addChild(new SLNode(grid, "grid"));

    // Astro boy character
    _char1->translate(-1, 0, 0);
    SLAnimPlayback* char1Anim = animManager().lastAnimPlayback();
    char1Anim->playForward();
    scene->addChild(_char1);

    // Sintel character
    _char2->translate(1, 0, 0);
    SLAnimPlayback* char2Anim = animManager().lastAnimPlayback();
    char2Anim->playForward();
    scene->addChild(_char2);

    // Skinned cube 1
    _cube1->translate(3, 0, 0);
    SLAnimPlayback* cube1Anim = animManager().lastAnimPlayback();
    cube1Anim->easing(EC_inOutSine);
    cube1Anim->playForward();
    scene->addChild(_cube1);

    // Skinned cube 2
    _cube2->translate(-3, 0, 0);
    SLAnimPlayback* cube2Anim = animManager().lastAnimPlayback();
    cube2Anim->easing(EC_inOutSine);
    cube2Anim->playForward();
    scene->addChild(_cube2);

    // Skinned cube 3
    _cube3->translate(0, 3, 0);
    SLAnimPlayback* cube3Anim = animManager().lastAnimPlayback();
    cube3Anim->loop(AL_pingPongLoop);
    cube3Anim->easing(EC_inOutCubic);
    cube3Anim->playForward();
    scene->addChild(_cube3);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
