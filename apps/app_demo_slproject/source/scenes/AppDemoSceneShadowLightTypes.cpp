/**
 * \file      AppDemoSceneShadowLightTypes.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneShadowLightTypes.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLLightRect.h>
#include <SLSpheric.h>
#include <SLBox.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneShadowLightTypes::AppDemoSceneShadowLightTypes()
  : SLScene("Shadow Mapping Types Demo Scene")
{
    info("Shadow Mapping is implemented for these light types.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShadowLightTypes::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addNodeToLoad(_teapot,
                     AppDemo::modelPath +
                       "FBX/Teapot/Teapot.fbx");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShadowLightTypes::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLMaterial* mat1 = new SLMaterial(am, "mat1");

    // Base root group node for the scene
    SLNode* scene = new SLNode;
    this->root3D(scene);

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 2, 20);
    cam1->lookAt(0, 2, 0);
    cam1->focalDist(20);
    cam1->background().colors(SLCol4f(0.1f, 0.1f, 0.1f));
    cam1->setInitialState();
    scene->addChild(cam1);

    // Create light sources
    vector<SLLight*> lights = {
      new SLLightDirect(am, this),
      new SLLightRect(am, this),
      new SLLightSpot(am, this, 0.3f, 25.0f),
      new SLLightSpot(am, this, 0.1f, 180.0f)};

    for (SLint i = 0; i < lights.size(); ++i)
    {
        SLLight* light = lights[i];
        SLNode*  node  = dynamic_cast<SLNode*>(light);
        SLfloat  x     = ((float)i - ((SLfloat)lights.size() - 1.0f) / 2.0f) * 5;

        if (i == 0) // Make direct light less bright
        {
            light->powers(0.0f, 0.4f, 0.4f);
            light->attenuation(1, 0, 0);
        }
        else
        {
            light->powers(0.0f, 2.0f, 2.0f);
            light->attenuation(0, 0, 1);
        }

        node->translation(x, 5, 0);
        node->lookAt(x, 0, 0);
        light->createsShadows(true);
        light->createShadowMap();
        light->shadowMap()->rayCount(SLVec2i(16, 16));
        scene->addChild(node);
    }

    SLAnimation* teapotAnim = this->animManager().createNodeAnimation("teapot_anim",
                                                                      8.0f,
                                                                      true,
                                                                      EC_linear,
                                                                      AL_loop);

    for (SLLight* light : lights)
    {
        SLNode* teapot = _teapot->copyRec();

        teapot->translate(light->positionWS().x, 2, 0);
        teapot->children()[0]->castsShadows(true);
        scene->addChild(teapot);

        // Create animation
        SLNodeAnimTrack* track = teapotAnim->createNodeAnimTrack();
        track->animatedNode(teapot);

        SLTransformKeyframe* frame0 = track->createNodeKeyframe(0.0f);
        frame0->translation(teapot->translationWS());
        frame0->rotation(SLQuat4f(0, 0, 0));

        SLTransformKeyframe* frame1 = track->createNodeKeyframe(4.0f);
        frame1->translation(teapot->translationWS());
        frame1->rotation(SLQuat4f(0, 1 * PI, 0));

        SLTransformKeyframe* frame2 = track->createNodeKeyframe(8.0f);
        frame2->translation(teapot->translationWS());
        frame2->rotation(SLQuat4f(0, 2 * PI, 0));
    }

    // Add a box which receives shadows
    SLfloat minx    = lights.front()->positionWS().x - 3;
    SLfloat maxx    = lights.back()->positionWS().x + 3;
    SLNode* boxNode = new SLNode(new SLBox(am,
                                           minx,
                                           -1,
                                           -5,
                                           maxx,
                                           0,
                                           5,
                                           "Box",
                                           mat1));
    boxNode->castsShadows(false);
    scene->addChild(boxNode);

    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
