/**
 * \file      AppDemoSceneParticleMany.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Particle System from Marc Affolter
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneParticleMany.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>

//-----------------------------------------------------------------------------
AppDemoSceneParticleMany::AppDemoSceneParticleMany()
  : SLScene("Many Particle System")
{
    info("This particle system with 1 mio. particles uses the box shape "
         "type for distribution. See the properties window for the detailed "
         "settings of the particles system");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneParticleMany::registerAssetsToLoad(SLAssetLoader& al)
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
void AppDemoSceneParticleMany::assemble(SLAssetManager* am,
                                        SLSceneView*    sv)
{
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->clipNear(0.1f);
    cam1->clipFar(1000);
    cam1->translation(0, 0, 400);
    cam1->focalDist(400);
    cam1->lookAt(0, 0, 0);
    cam1->background().colors(SLCol4f(0.3f, 0.3f, 0.3f));
    cam1->setInitialState();

    // Root scene node
    SLNode* root = new SLNode("Root scene node");
    root3D(root);
    root->addChild(cam1);

    // Create meshes and nodes
    SLParticleSystem* ps = new SLParticleSystem(am,
                                                1000000,
                                                SLVec3f(-10.0f, -10.0f, -10.0f),
                                                SLVec3f(10.0f, 10.0f, 10.0f),
                                                4.0f,
                                                _texC,
                                                "Particle System",
                                                _texFlip);
    ps->doAlphaOverLT(false);
    ps->doSizeOverLT(false);
    ps->doRotation(false);
    ps->doShape(true);
    ps->shapeType(ST_Box);
    ps->shapeScale(100.0f, 100.0f, 100.0f);
    ps->doDirectionSpeed(true);
    ps->doBlendBrightness(true);
    ps->doColor(true);
    ps->color(SLCol4f(0.875f, 0.156f, 0.875f, 1.0f));
    ps->speed(0.0f);
    SLMesh* pSMesh = ps;
    SLNode* pSNode = new SLNode(pSMesh, "Particle system node");
    root->addChild(pSNode);

    sv->camera(cam1);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
