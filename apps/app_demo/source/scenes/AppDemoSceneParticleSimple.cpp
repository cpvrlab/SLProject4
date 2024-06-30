/**
 * \file      AppDemoSceneParticleSimple.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Marcus Hudritsch, Particle System from Marc Affolter
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#include <AppDemoSceneParticleSimple.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>

//-----------------------------------------------------------------------------
AppDemoSceneParticleSimple::AppDemoSceneParticleSimple()
  : SLScene("Simple Particle System")
{
    info("Simple Particle System with a flip book smoke particle.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneParticleSimple::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texC,
                        AppCommon::texturePath +
                          "ParticleSmoke_08_C.png");
    al.addTextureToLoad(_texFlip,
                        AppCommon::texturePath +
                          "ParticleSmoke_03_8x8_C.png");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneParticleSimple::assemble(SLAssetManager* am,
                                          SLSceneView*    sv)
{
    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    root3D(scene);

    // Create and add camera
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 1.5f, 4);
    cam1->lookAt(0, 1.5f, 0);
    scene->addChild(cam1);

    // Create a light source node
    SLLightSpot* light1 = new SLLightSpot(am, this, 0.3f);
    light1->translation(5, 5, 5);
    light1->name("light node");
    scene->addChild(light1);

    // Create meshes and nodes
    SLParticleSystem* ps = new SLParticleSystem(am,
                                                50,
                                                SLVec3f(0.04f, 0.4f, 0.1f),
                                                SLVec3f(-0.11f, 0.7f, -0.1f),
                                                4.0f,
                                                _texC,
                                                "Particle System",
                                                _texFlip);

    SLNode* pSNode = new SLNode(ps, "Particle system node");
    scene->addChild(pSNode);

    // Set background color and the root scene node
    sv->sceneViewCamera()->background().colors(SLCol4f(0.8f, 0.8f, 0.8f),
                                               SLCol4f(0.2f, 0.2f, 0.2f));
    sv->camera(cam1);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
