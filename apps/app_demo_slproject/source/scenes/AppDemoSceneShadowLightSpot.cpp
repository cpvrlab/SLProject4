//#############################################################################
//  File:      AppDemoSceneShadowLightSpot.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneShadowLightSpot.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLBox.h>
#include <SLSpheric.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneShadowLightSpot::AppDemoSceneShadowLightSpot()
  : SLScene("Shadow Mapping for 8 Spot lights")
{
    info("8 Spot lights use a perspective projection for their light space.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShadowLightSpot::registerAssetsToLoad(SLAssetLoader& al)
{
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShadowLightSpot::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Setup shadow mapping material
    SLMaterial* matPerPixSM = new SLMaterial(am, "m1");

    // Base root group node for the scene
    SLNode* scene = new SLNode;
    this->root3D(scene);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 5, 13);
    cam1->lookAt(0, 1, 0);
    cam1->focalDist(8);
    cam1->background().colors(SLCol4f(0.1f, 0.1f, 0.1f));
    cam1->setInitialState();
    scene->addChild(cam1);

    // Create light sources
    for (int i = 0; i < SL_MAX_LIGHTS; ++i)
    {
        SLLightSpot* light = new SLLightSpot(am,
                                             this,
                                             0.3f,
                                             45.0f);
        SLCol4f      color;
        color.hsva2rgba(SLVec4f(Utils::TWOPI * (float)i / (float)SL_MAX_LIGHTS,
                                1.0f,
                                1.0f));
        light->powers(0.0f, 5.0f, 5.0f, color);
        light->translation(2 * sin((Utils::TWOPI / (float)SL_MAX_LIGHTS) * (float)i),
                           5,
                           2 * cos((Utils::TWOPI / (float)SL_MAX_LIGHTS) * (float)i));
        light->lookAt(0, 0, 0);
        light->attenuation(0, 0, 1);
        light->createsShadows(true);
        light->createShadowMap();
        light->shadowMap()->rayCount(SLVec2i(16, 16));
        scene->addChild(light);
    }

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
                                        1.0f,
                                        A_x,
                                        1.0f,
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
