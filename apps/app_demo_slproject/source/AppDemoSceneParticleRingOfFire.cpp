//#############################################################################
//  File:      AppDemoSceneParticleRingOfFire.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch, Particle System from Marc Affolter
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneParticleRingOfFire.h>
#include <SLLightSpot.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneParticleRingOfFire::AppDemoSceneParticleRingOfFire()
  : AppScene("Ring of Fire Particle System")
{
    info("This ring particle system uses the cone shape type for distribution.\n"
         "See the properties window for the settings of the particles system");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneParticleRingOfFire::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texC, "ParticleSmoke_08_C.png");
    al.addTextureToLoad(_texFlip, "ParticleSmoke_03_8x8_C.png");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneParticleRingOfFire::assemble(SLAssetManager* am,
                                              SLSceneView*    sv)
{
    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    root3D(scene);

    // Create meshes and nodes
    SLParticleSystem* ps = new SLParticleSystem(am,
                                                1000,
                                                SLVec3f(0.0f, 0.0f, 0.0f),
                                                SLVec3f(0.0f, 0.0f, 0.0f),
                                                4.0f,
                                                _texC,
                                                "Ring of fire Particle System",
                                                _texFlip);

    ps->doShape(true);
    ps->shapeType(ST_Cone);
    ps->doShapeSpawnBase(true);
    ps->doShapeSurface(true);
    ps->shapeRadius(1.0f);
    ps->doBlendBrightness(true);
    ps->color(SLCol4f(0.925f, 0.238f, 0.097f, 0.503f));

    SLMesh* pSMesh = ps;
    SLNode* pSNode = new SLNode(pSMesh, "Particle Ring Fire node");
    pSNode->rotate(90, 1, 0, 0);
    scene->addChild(pSNode);

    // Set background color and the root scene node
    sv->sceneViewCamera()->background().colors(SLCol4f(0.8f, 0.8f, 0.8f),
                                               SLCol4f(0.2f, 0.2f, 0.2f));
    // Save energy
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
