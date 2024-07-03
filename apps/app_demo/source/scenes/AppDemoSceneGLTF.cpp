/**
 * \file      AppDemoSceneGLTF.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see:
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include <AppDemoSceneGLTF.h>
#include <AppCommon.h>
#include <AppDemoSceneID.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLSphere.h>
#include <SLLightDirect.h>

//-----------------------------------------------------------------------------
AppDemoSceneGLTF::AppDemoSceneGLTF(SLSceneID sceneID)
  : SLScene("GLTF File Demo Scene"),
    _sceneID(sceneID)
{
    info("GLTF File Format Test Scene");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneGLTF::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addSkyboxToLoad(_skybox,
                       al.modelPath() +
                         "GLTF/glTF-Sample-Models/hdris/envmap_malibu.hdr",
                       SLVec2i(256, 256),
                       "HDR Skybox");
    switch (_sceneID)
    {
        case SID_glTF_DamagedHelmet:
            _modelFile = "GLTF/glTF-Sample-Models/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf";
            this->name("glTF-Sample-Model: Damaged Helmet");
            break;
        case SID_glTF_FlightHelmet:
            _modelFile = "GLTF/glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf";
            this->name("glTF-Sample-Model: Flight Helmet");
            break;
        case SID_glTF_Sponza:
            _modelFile = "GLTF/glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf";
            this->name("glTF-Sample-Model: Sponza Palace in Dubrovnic");
            break;
        case SID_glTF_WaterBottle:
            _modelFile = "GLTF/glTF-Sample-Models/2.0/WaterBottle/glTF/WaterBottle.gltf";
            this->name("glTF-Sample-Model: WaterBottle");
            break;
    }

    al.addNodeToLoad(_modelGLTF,
                     AppCommon::modelPath + _modelFile);
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneGLTF::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLVec3f camPos, lookAt;
    SLfloat camClipFar = 100;

    switch (_sceneID)
    {
        case SID_glTF_DamagedHelmet:
        {
            camPos.set(0, 0, 3);
            lookAt.set(0, camPos.y, 0);
            camClipFar = 20;
            break;
        }
        case SID_glTF_FlightHelmet:
        {
            camPos.set(0, 0.33f, 1.1f);
            lookAt.set(0, camPos.y, 0);
            camClipFar = 20;
            break;
        }
        case SID_glTF_Sponza:
        {
            camPos.set(-8, 1.6f, 0);
            lookAt.set(0, camPos.y, 0);
            break;
        }
        case SID_glTF_WaterBottle:
        {
            camPos.set(0, 0, 0.5f);
            lookAt.set(0, camPos.y, 0);
            camClipFar = 20;
            break;
        }
    }

    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    this->root3D(scene);

    // Create camera and initialize its parameters
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(camPos);
    cam1->lookAt(lookAt);
    cam1->background().colors(SLCol4f(0.2f, 0.2f, 0.2f));
    cam1->focalDist(camPos.length());
    cam1->clipFar(camClipFar);
    cam1->setInitialState();
    scene->addChild(cam1);

    // Add directional light with a position that corresponds roughly to the sun direction
    SLLight::gamma       = 2.2f;
    SLLightDirect* light = new SLLightDirect(am,
                                             this,
                                             0.55f,
                                             1.0f,
                                             -0.2f,
                                             0.2f,
                                             0,
                                             1,
                                             1);
    light->lookAt(0, 0, 0);
    light->attenuation(1, 0, 0);
    light->createsShadows(true);
    light->createShadowMapAutoSize(cam1,
                                   SLVec2i(2048, 2048),
                                   4);
    light->shadowMap()->cascadesFactor(1.0);
    light->doSmoothShadows(true);
    light->castsShadows(false);
    light->shadowMinBias(0.001f);
    light->shadowMaxBias(0.003f);
    scene->addChild(light);

    // Update all materials and set their skybox to _skybox
    _modelGLTF->updateMeshMat([=](SLMaterial* m)
                              { m->skybox(_skybox); },
                              true);

    scene->addChild(_modelGLTF);

    this->skybox(_skybox);
    sv->camera(cam1);
    sv->doWaitOnIdle(true); // Saves energy
}
//-----------------------------------------------------------------------------
