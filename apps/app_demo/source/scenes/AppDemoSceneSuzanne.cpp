/**
 * \file      AppDemoSceneSuzanne.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see:
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include <AppDemoSceneSuzanne.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <AppCommon.h>

//-----------------------------------------------------------------------------
AppDemoSceneSuzanne::AppDemoSceneSuzanne(SLstring name,
                                         bool     textureMapping,
                                         bool     normalMapping,
                                         bool     occlusionMapping,
                                         bool     shadowMapping,
                                         bool     environmentMapping)
  : SLScene(name),
    _textureMapping(textureMapping),
    _normalMapping(normalMapping),
    _occlusionMapping(occlusionMapping),
    _shadowMapping(shadowMapping),
    _environmentMapping(environmentMapping)
{
    info(name);
    _skybox = nullptr;
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneSuzanne::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addNodeToLoad(_suzanneInCube,
                     AppCommon::modelPath +
                       "GLTF/AO-Baked-Test/AO-Baked-Test.gltf",
                     nullptr,
                     false,   // delete tex images after build
                     true,    // load meshes only
                     nullptr, // override material
                     0.5f);

    if (_environmentMapping)
        al.addSkyboxToLoad(_skybox,
                           al.modelPath() +
                             "GLTF/glTF-Sample-Models/hdris/envmap_malibu.hdr",
                           SLVec2i(256, 256),
                           "HDR Skybox");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneSuzanne::assemble(SLAssetManager* am, SLSceneView* sv)
{
    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    root3D(scene);

    // Create camera in the center
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0.5f, 2);
    cam1->lookAt(0, 0.5f, 0);
    cam1->setInitialState();
    cam1->focalDist(2);
    scene->addChild(cam1);

    // Create directional light for the sunlight
    SLLightDirect* light;

    if (_environmentMapping)
    {
        SLLight::gamma = 2.0f;
        light          = new SLLightDirect(am, this, 0.1f);
        light->ambientPower(0.0f);
        light->diffusePower(3.0f);
    }
    else
    {
        SLLight::gamma = 1.0f;
        light          = new SLLightDirect(am, this, 0.1f);
        light->ambientPower(0.6f);
        light->diffusePower(0.6f);
    }

    light->attenuation(1, 0, 0);
    light->translate(0, 0, 0.5);
    light->lookAt(1, -1, 0.5);
    SLAnimation* lightAnim = animManager().createNodeAnimation("LightAnim",
                                                               4.0f,
                                                               true,
                                                               EC_inOutSine,
                                                               AL_pingPongLoop);
    lightAnim->createNodeAnimTrackForRotation(light,
                                              -180,
                                              SLVec3f(0, 1, 0));

    scene->addChild(light);

    // Add shadow mapping
    if (_shadowMapping)
    {
        light->createsShadows(true);
        light->createShadowMap(-3,
                               3,
                               SLVec2f(5, 5),
                               SLVec2i(2048, 2048));
        light->doSmoothShadows(true);
    }

    SLCol4f stoneColor(0.56f, 0.50f, 0.44f);

    // Remove unwanted textures
    auto materialUpdater = [=](SLMaterial* mat)
    {
        if (!_textureMapping)
        {
            mat->removeTextureType(TT_diffuse);
            mat->ambientDiffuse(stoneColor);
        }

        if (!_normalMapping)
            mat->removeTextureType(TT_normal);

        if (!_occlusionMapping)
            mat->removeTextureType(TT_occlusion);

        if (_environmentMapping)
        {
            mat->skybox(_skybox);
            mat->reflectionModel(RM_CookTorrance);
            this->skybox(_skybox);
        }
    };
    _suzanneInCube->updateMeshMat(materialUpdater,
                                  true);

    scene->addChild(_suzanneInCube);

    sv->camera(cam1);

    // Save energy
    sv->doWaitOnIdle(true);
}
//-----------------------------------------------------------------------------
