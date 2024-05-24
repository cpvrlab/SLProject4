//#############################################################################
//  File:      AppDemoSceneShadowCascaded.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneShadowCascaded.h>
#include <SLLightDirect.h>
#include <SLBox.h>
#include <SLSpheric.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneShadowCascaded::AppDemoSceneShadowCascaded()
  : AppScene("Cascaded Shadow Mapping Demo Scene")
{
    info("Cascaded Shadow Mapping uses several cascades of shadow maps to "
         "provide higher resolution shadows near the camera and lower "
         "resolution shadows further away.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShadowCascaded::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addNodeToLoad(_teapot,
                     AppDemo::modelPath,
                     "FBX/Teapot/Teapot.fbx");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShadowCascaded::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Setup shadow mapping material
    SLMaterial* matPerPixSM = new SLMaterial(am, "m1");

    // Base root group node for the scene
    SLNode* scene = new SLNode;
    this->root3D(scene);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 7, 12);
    cam1->lookAt(0, 1, 0);
    cam1->focalDist(8);
    cam1->background().colors(SLCol4f(0.1f, 0.1f, 0.1f));
    cam1->setInitialState();
    scene->addChild(cam1);

    // Create light source
    // Do constant attenuation for directional lights since it is infinitely far away
    SLLightDirect* light = new SLLightDirect(am, this);
    light->powers(0.0f, 1.0f, 1.0f);
    light->translation(0, 5, 0);
    light->lookAt(0, 0, 0);
    light->attenuation(1, 0, 0);
    light->createsShadows(true);
    light->doCascadedShadows(true);
    light->createShadowMapAutoSize(cam1);
    light->shadowMap()->rayCount(SLVec2i(16, 16));
    light->castsShadows(false);
    scene->addChild(light);

    // Add a sphere which casts shadows
    SLNode* sphereNode = new SLNode(new SLSpheric(am,
                                                  1,
                                                  0,
                                                  180,
                                                  20,
                                                  20,
                                                  "Sphere",
                                                  matPerPixSM));
    sphereNode->translate(0, 2.0, 0);
    sphereNode->castsShadows(true);
    scene->addChild(sphereNode);

    SLAnimation* anim = this->animManager().createNodeAnimation("sphere_anim",
                                                                2.0f);
    anim->createNodeAnimTrackForEllipse(sphereNode,
                                        0.5f,
                                        A_x,
                                        0.5f,
                                        A_z);

    // Add a box which receives shadows
    SLNode* boxNode = new SLNode(new SLBox(am,
                                           -5,
                                           -1,
                                           -5,
                                           5,
                                           0,
                                           5,
                                           "Box",
                                           matPerPixSM));
    boxNode->castsShadows(false);
    scene->addChild(boxNode);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
