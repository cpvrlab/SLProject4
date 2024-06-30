/**
 * \file      AppDemoSceneParticleDustStorm.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Particle System from Marc Affolter
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneParticleDustStorm.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>

//-----------------------------------------------------------------------------
AppDemoSceneParticleDustStorm::AppDemoSceneParticleDustStorm()
  : SLScene("Dust Storm Particle System")
{
    info("This dust storm particle system uses the box shape type for distribution.\n"
         "See the properties window for the detailed settings of the particles system");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneParticleDustStorm::registerAssetsToLoad(SLAssetLoader& al)
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
void AppDemoSceneParticleDustStorm::assemble(SLAssetManager* am,
                                             SLSceneView*    sv)
{
    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    root3D(scene);

    // Create and add camera
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 0, 55);
    cam1->lookAt(0, 0, 0);
    cam1->focalDist(55);
    scene->addChild(cam1);
    sv->camera(cam1);

    // Create meshes and nodes
    // Dust storm
    SLParticleSystem* ps = new SLParticleSystem(am,
                                                500,
                                                SLVec3f(-0.1f, -0.5f, -5.0f),
                                                SLVec3f(0.1f, 0.5f, -2.5f),
                                                3.5f,
                                                _texC,
                                                "DustStorm",
                                                _texFlip);
    ps->doShape(true);
    ps->shapeType(ST_Box);
    ps->shapeScale(50.0f, 1.0f, 50.0f);
    ps->scale(15.0f);
    ps->doSizeOverLT(false);
    ps->doAlphaOverLT(true);
    ps->doAlphaOverLTCurve(true);
    ps->bezierStartEndPointAlpha()[1] = 0.0f;
    ps->bezierControlPointAlpha()[1]  = 0.5f;
    ps->bezierControlPointAlpha()[2]  = 0.5f;
    ps->generateBernsteinPAlpha();
    ps->doRotRange(true);
    ps->color(SLCol4f(1.0f, 1.0f, 1.0f, 1.0f));
    ps->doBlendBrightness(false);
    ps->frameRateFB(16);

    SLMesh* pSMesh = ps;
    SLNode* pSNode = new SLNode(pSMesh, "Particle system node fire2");
    pSNode->translate(3.0f, -0.8f, 0.0f, TS_object);

    scene->addChild(pSNode);

    // Set background color and the root scene node
    sv->sceneViewCamera()->background().colors(SLCol4f(0.8f, 0.8f, 0.8f),
                                               SLCol4f(0.2f, 0.2f, 0.2f));
    // Save energy
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
