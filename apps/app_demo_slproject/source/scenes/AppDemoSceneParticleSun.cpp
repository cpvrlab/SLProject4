//#############################################################################
//  File:      AppDemoSceneParticleSun.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch, Particle System from Marc Affolter
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneParticleSun.h>
#include <SLLightSpot.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneParticleSun::AppDemoSceneParticleSun()
  : AppScene("Sun Particle System")
{
    info("This sun particle system uses the sphere shape type for distribution.\n"
         "See the properties window for the detailed settings of the particles system");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneParticleSun::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texC,
                        AppDemo::texturePath +
                          "ParticleSmoke_08_C.png");
    al.addTextureToLoad(_texFlip,
                        AppDemo::texturePath +
                          "ParticleSmoke_03_8x8_C.png");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneParticleSun::assemble(SLAssetManager* am,
                                       SLSceneView*    sv)
{
    // Create a scene group node
    SLNode* scene = new SLNode("scene node");
    root3D(scene);

    // Create meshes and nodes
    SLParticleSystem* ps = new SLParticleSystem(am,
                                                10000,
                                                SLVec3f(0.0f, 0.0f, 0.0f),
                                                SLVec3f(0.0f, 0.0f, 0.0f),
                                                4.0f,
                                                _texC,
                                                "Sun Particle System",
                                                _texFlip);

    ps->doShape(true);
    ps->shapeType(ST_Sphere);
    ps->shapeRadius(3.0f);
    ps->doBlendBrightness(true);
    ps->color(SLCol4f(0.925f, 0.238f, 0.097f, 0.199f));

    SLMesh* pSMesh = ps;
    SLNode* pSNode = new SLNode(pSMesh, "Particle Sun node");
    scene->addChild(pSNode);

    // Set background color and the root scene node
    sv->sceneViewCamera()->background().colors(SLCol4f(0.8f, 0.8f, 0.8f),
                                               SLCol4f(0.2f, 0.2f, 0.2f));
    // Save energy
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
