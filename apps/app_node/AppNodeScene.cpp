/**
 * \file      AppNodeScene.cpp
 * \brief     Scene creation callback for the the node demo app
 * \date      June 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppCommon.h>
#include <AppNodeScene.h>

#include <SLBox.h>
#include <SLCamera.h>
#include <SLLightSpot.h>
#include <SLNode.h>
#include <SLMaterial.h>
#include <SLSceneView.h>
#include <SLAssetLoader.h>

//-----------------------------------------------------------------------------
AppNodeScene::AppNodeScene() : SLScene("Node Scene")
{
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppNodeScene::registerAssetsToLoad(SLAssetLoader& al)
{
    // load coordinate axis arrows
    al.addNodeToLoad(_axesNode, AppCommon::modelPath + "FBX/Axes/axes_blender.fbx");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppNodeScene::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLCamera* cam1 = new SLCamera;
    cam1->translation(2, 3, 5);
    cam1->lookAt(-2, -1.0, 1);
    cam1->focalDist(6);
    cam1->background().colors(SLCol4f(0.8f, 0.8f, 0.8f));

    SLLightSpot* light1 = new SLLightSpot(am, this, 0.3f);
    light1->translation(10, 10, 10);

    SLNode* scene = new SLNode;
    scene->addChild(light1);
    scene->addChild(cam1);

    SLMaterial* rMat = new SLMaterial(am, "rMat", SLCol4f(1.0f, 0.7f, 0.7f));
    SLMaterial* gMat = new SLMaterial(am, "gMat", SLCol4f(0.7f, 1.0f, 0.7f));

    // build parent box
    SLNode* moveBox = new SLNode("Parent");
    moveBox->translation(0, 0, 2);
    moveBox->rotation(22.5f, SLVec3f(0, -1, 0));
    moveBox->addMesh(new SLBox(am, -0.3f, -0.3f, -0.3f, 0.3f, 0.3f, 0.3f, "Box", rMat));
    moveBox->setInitialState();

    // build child box
    SLNode* moveBoxChild = new SLNode("Child");
    moveBoxChild->translation(0, 1, 0);
    moveBoxChild->rotation(22.5f, SLVec3f(0, -1, 0));
    moveBoxChild->setInitialState();
    moveBoxChild->addMesh(new SLBox(am, -0.2f, -0.2f, -0.2f, 0.2f, 0.2f, 0.2f, "Box", gMat));
    moveBox->addChild(moveBoxChild);

    _axesNode->name("Axes");

    scene->addChild(moveBox);
    scene->addChild(_axesNode);

    root3D(scene);

    sv->camera(cam1);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
