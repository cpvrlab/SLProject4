//#############################################################################
//  File:      AppDemoSceneShaderSkybox.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneShaderSkybox.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLLightDirect.h>
#include <SLSphere.h>
#include <SLSkybox.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneShaderSkybox::AppDemoSceneShaderSkybox()
  : SLScene("Image Based Lighting Test Scene")
{
    info("Image-based lighting from skybox using high dynamic range images. "
         "It uses the Cook-Torrance reflection model also to calculate the "
         "ambient light part from the surrounding HDR skybox.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneShaderSkybox::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addSkyboxToLoad(_skybox,
                       "Desert+X1024_C.jpg",
                       "Desert-X1024_C.jpg",
                       "Desert+Y1024_C.jpg",
                       "Desert-Y1024_C.jpg",
                       "Desert+Z1024_C.jpg",
                       "Desert-Z1024_C.jpg");
    al.addProgramToLoad(_spRefl,
                        AppDemo::shaderPath + "Reflect.vert",
                        AppDemo::shaderPath + "Reflect.frag");
    al.addProgramToLoad(_spRefr,
                        AppDemo::shaderPath + "RefractReflect.vert",
                        AppDemo::shaderPath + "RefractReflect.frag");
    al.addNodeToLoad(_teapot,
                     AppDemo::modelPath +
                       "FBX/Teapot/Teapot.fbx");
    al.addNodeToLoad(_suzanne,
                     AppDemo::modelPath +
                       "FBX/Suzanne/Suzanne.fbx");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneShaderSkybox::assemble(SLAssetManager* am, SLSceneView* sv)
{

    // Material for mirror
    SLMaterial* refl = new SLMaterial(am,
                                      "refl",
                                      SLCol4f::BLACK,
                                      SLCol4f::WHITE,
                                      1000,
                                      1.0f);
    refl->addTexture(_skybox->environmentCubemap());
    refl->program(_spRefl);
    // Material for glass
    SLMaterial* refr = new SLMaterial(am,
                                      "refr",
                                      SLCol4f::BLACK,
                                      SLCol4f::BLACK,
                                      100,
                                      0.1f,
                                      0.9f,
                                      1.5f);
    refr->translucency(1000);
    refr->transmissive(SLCol4f::WHITE);
    refr->addTexture(_skybox->environmentCubemap());
    refr->program(_spRefr);

    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    this->root3D(scene);

    // Create camera in the center
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 5);
    cam1->setInitialState();
    scene->addChild(cam1);

    // There is no light needed in this scene. All reflections come from cube maps
    // But ray tracing needs light sources
    // Create directional light for the sunlight
    SLLightDirect* light = new SLLightDirect(am, this, 0.5f);
    light->ambientColor(SLCol4f(0.3f, 0.3f, 0.3f));
    light->attenuation(1, 0, 0);
    light->translate(1, 1, -1);
    light->lookAt(-1, -1, 1);
    scene->addChild(light);

    // Center sphere
    SLNode* sphere = new SLNode(new SLSphere(am,
                                             0.5f,
                                             32,
                                             32,
                                             "Sphere",
                                             refr));
    scene->addChild(sphere);

    // configure teapot
    _teapot->translate(-1.5f, -0.5f, 0);

    SLNode* teapot = _teapot->findChild<SLNode>("Teapot");
    teapot->setMeshMat(refl, true);
    scene->addChild(_teapot);

    // configure Suzanne
    _suzanne->translate(1.5f, -0.5f, 0);
    SLNode* suzanne = _suzanne->findChild<SLNode>("Suzanne");
    suzanne->setMeshMat(refr, true);
    scene->addChild(_suzanne);

    sv->camera(cam1);
    this->skybox(_skybox);

    // Save energy
    sv->doWaitOnIdle(true);
}
//-----------------------------------------------------------------------------
