//#############################################################################
//  File:      AppDemoSceneShadowLightPoint.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneShadowLightPoint.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <SLSpheric.h>
#include <SLBox.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneShadowLightPoint::AppDemoSceneShadowLightPoint()
  : SLScene("Shadow Mapping for point lights")
{
    info("Point lights use cubemaps to store shadow maps.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShadowLightPoint::registerAssetsToLoad(SLAssetLoader& al)
{
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShadowLightPoint::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Setup shadow mapping material
    SLMaterial* matPerPixSM = new SLMaterial(am, "m1");

    // Base root group node for the scene
    SLNode* scene = new SLNode;
    this->root3D(scene);

    // Create camera
    SLCamera* cam1 = new SLCamera;
    cam1->translation(0, 0, 8);
    cam1->lookAt(0, 0, 0);
    cam1->fov(27);
    cam1->focalDist(cam1->translationOS().length());
    cam1->background().colors(SLCol4f(0.1f, 0.1f, 0.1f));
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

    // Create lights
    SLAnimation* anim = this->animManager().createNodeAnimation("light_anim", 4.0f);

    for (SLint i = 0; i < 3; ++i)
    {
        SLLightSpot* light = new SLLightSpot(am, this, 0.1f);
        light->powers(0.2f,
                      1.5f,
                      1.0f,
                      SLCol4f(i == 0, i == 1, i == 2));
        light->attenuation(0, 0, 1);
        light->translate((float)i - 1.0f, (float)i - 1.0f, (float)i - 1.0f);
        light->createsShadows(true);
        light->createShadowMap();
        light->shadowMap()->rayCount(SLVec2i(16, 16));
        scene->addChild(light);
        anim->createNodeAnimTrackForEllipse(light,
                                            0.2f,
                                            A_x,
                                            0.2f,
                                            A_z);
    }

    // Create wall polygons
    SLfloat pL = -1.48f, pR = 1.48f; // left/right
    SLfloat pB = -1.25f, pT = 1.19f; // bottom/top
    SLfloat pN = 1.79f, pF = -1.55f; // near/far

    // Bottom plane
    SLNode* b = new SLNode(new SLRectangle(am,
                                           SLVec2f(pL, -pN),
                                           SLVec2f(pR, -pF),
                                           6,
                                           6,
                                           "bottom",
                                           matPerPixSM));
    b->rotate(90, -1, 0, 0);
    b->translate(0, 0, pB, TS_object);
    scene->addChild(b);

    // Top plane
    SLNode* t = new SLNode(new SLRectangle(am,
                                           SLVec2f(pL, pF),
                                           SLVec2f(pR, pN),
                                           6,
                                           6,
                                           "top",
                                           matPerPixSM));
    t->rotate(90, 1, 0, 0);
    t->translate(0, 0, -pT, TS_object);
    scene->addChild(t);

    // Far plane
    SLNode* f = new SLNode(new SLRectangle(am,
                                           SLVec2f(pL, pB),
                                           SLVec2f(pR, pT),
                                           6,
                                           6,
                                           "far",
                                           matPerPixSM));
    f->translate(0, 0, pF, TS_object);
    scene->addChild(f);

    // near plane
    SLNode* n = new SLNode(new SLRectangle(am,
                                           SLVec2f(pL, pT),
                                           SLVec2f(pR, pB),
                                           6,
                                           6,
                                           "near",
                                           matPerPixSM));
    n->translate(0, 0, pN, TS_object);
    scene->addChild(n);

    // left plane
    SLNode* l = new SLNode(new SLRectangle(am,
                                           SLVec2f(-pN, pB),
                                           SLVec2f(-pF, pT),
                                           6,
                                           6,
                                           "left",
                                           matPerPixSM));
    l->rotate(90, 0, 1, 0);
    l->translate(0, 0, pL, TS_object);
    scene->addChild(l);

    // Right plane
    SLNode* r = new SLNode(new SLRectangle(am,
                                           SLVec2f(pF, pB),
                                           SLVec2f(pN, pT),
                                           6,
                                           6,
                                           "right",
                                           matPerPixSM));
    r->rotate(90, 0, -1, 0);
    r->translate(0, 0, -pR, TS_object);
    scene->addChild(r);

    // Create cubes which cast shadows
    for (SLint i = 0; i < 64; ++i)
    {
        SLBox* box = new SLBox(am);
        box->mat(matPerPixSM);
        SLNode* boxNode = new SLNode(box);

        boxNode->scale(Utils::random(0.01f, 0.1f));
        boxNode->translate(Utils::random(pL + 0.3f, pR - 0.3f),
                           Utils::random(pB + 0.3f, pT - 0.3f),
                           Utils::random(pF + 0.3f, pN - 0.3f),
                           TS_world);
        boxNode->castsShadows(true);

        scene->addChild(boxNode);
    }

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
