/**
 * \file      AppDemoSceneParticleFountain.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Marcus Hudritsch, Particle System from Marc Affolter
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#include <AppDemoSceneParticleFountain.h>
#include <AppDemo.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>

//-----------------------------------------------------------------------------
AppDemoSceneParticleFountain::AppDemoSceneParticleFountain()
  : SLScene("Fountain Particle System")
{
    info("This fountain particle system uses acceleration and gravity.\n"
         "See the properties window for the detailed settings of the particles system");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneParticleFountain::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texC,
                        AppDemo::texturePath +
                          "ParticleCircle_05_C.png");
    al.addTextureToLoad(_texFlip,
                        AppDemo::texturePath +
                          "ParticleSmoke_03_8x8_C.png");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneParticleFountain::assemble(SLAssetManager* am,
                                            SLSceneView*    sv)
{
    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    root3D(scene);

    // Create and add camera
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, -1, 55);
    cam1->lookAt(0, -1, 0);
    cam1->focalDist(55);
    scene->addChild(cam1);
    sv->camera(cam1);

    // Create a light source node
    SLLightSpot* light1 = new SLLightSpot(am, this, 0.3f);
    light1->translation(0, -1, 2);
    light1->name("light node");
    scene->addChild(light1);

    // Create meshes and nodes
    SLParticleSystem* ps     = new SLParticleSystem(am,
                                                5000,
                                                SLVec3f(5.0f, 15.0f, 5.0f),
                                                SLVec3f(-5.0f, 17.0f, -5.0f),
                                                5.0f,
                                                _texC,
                                                "Fountain",
                                                _texFlip);
    SLMesh*           pSMesh = ps;
    ps->doGravity(true);
    ps->color(SLCol4f(0.0039f, 0.14f, 0.86f, 0.33f));
    ps->doSizeOverLT(false);
    ps->doAlphaOverLT(false);
    SLNode* pSNode = new SLNode(pSMesh, "Particle system node");
    scene->addChild(pSNode);

    // Set background color and the root scene node
    sv->sceneViewCamera()->background().colors(SLCol4f(0.8f, 0.8f, 0.8f),
                                               SLCol4f(0.2f, 0.2f, 0.2f));
    // Save energy
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
