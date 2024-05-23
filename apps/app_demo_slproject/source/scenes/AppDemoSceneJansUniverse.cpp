//#############################################################################
//  File:      AppDemoSceneJansUniverse.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneJansUniverse.h>
#include <AppDemo.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <SLSphere.h>

//-----------------------------------------------------------------------------
AppDemoSceneJansUniverse::AppDemoSceneJansUniverse()
  : AppScene("Jan's Universe Test Scene")
{
    info("Jan's Universe Test Scene");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneJansUniverse::registerAssetsToLoad(SLAssetLoader& al)
{
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneJansUniverse::assemble(SLAssetManager* am,
                                        SLSceneView*    sv)
{
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->clipNear(0.1f);
    cam1->clipFar(1000);
    cam1->translation(0, 0, 75);
    cam1->focalDist(75);
    cam1->lookAt(0, 0, 0);
    cam1->background().colors(SLCol4f(0.3f, 0.3f, 0.3f));
    cam1->setInitialState();

    // Root scene node
    SLNode* scene = new SLNode;
    root3D(scene);
    scene->addChild(cam1);

    // Generate NUM_MAT cook-torrance materials
#ifndef SL_GLES
    const int    NUM_MAT_MESH = 100;
    SLuint const levels       = 6;
    SLuint const childCount   = 8;
#else
    const int    NUM_MAT_MESH = 20;
    SLuint const levels       = 6;
    SLuint const childCount   = 8;
#endif
    SLVMaterial materials(NUM_MAT_MESH);
    for (int i = 0; i < NUM_MAT_MESH; ++i)
    {
        SLstring matName = "mat-" + std::to_string(i);
        materials[i]     = new SLMaterial(am,
                                      matName.c_str(),
                                      nullptr,
                                      new SLGLTexture(am,
                                                      AppDemo::texturePath + "rusty-metal_2048_C.jpg"),
                                      nullptr, // new SLGLTexture(am, texPath + "rusty-metal_2048_N.jpg"),
                                      new SLGLTexture(am,
                                                      AppDemo::texturePath + "rusty-metal_2048_M.jpg"),
                                      new SLGLTexture(am,
                                                      AppDemo::texturePath + "rusty-metal_2048_R.jpg"),
                                      nullptr,
                                      nullptr);
        SLCol4f color;
        color.hsva2rgba(SLVec4f(Utils::TWOPI * (float)i / (float)NUM_MAT_MESH, 1.0f, 1.0f));
        materials[i]->diffuse(color);
    }

    // Generate NUM_MESH sphere meshes
    SLVMesh meshes(NUM_MAT_MESH);
    for (int i = 0; i < NUM_MAT_MESH; ++i)
    {
        SLstring meshName = "mesh-" + std::to_string(i);
        meshes[i]         = new SLSphere(am,
                                 1.0f,
                                 32,
                                 32,
                                 meshName.c_str(),
                                 materials[i % NUM_MAT_MESH]);
    }

    // Create universe
    generateUniverse(am,
                     this,
                     scene,
                     0,
                     levels,
                     childCount,
                     materials,
                     meshes);

    sv->camera(cam1);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
//! Adds another level to Jan's Universe scene
void AppDemoSceneJansUniverse::addUniverseLevel(SLAssetManager* am,
                                                SLScene*        s,
                                                SLNode*         parent,
                                                SLint           parentID,
                                                SLuint          currentLevel,
                                                SLuint          levels,
                                                SLuint          childCount,
                                                SLVMaterial&    materials,
                                                SLVMesh&        meshes,
                                                SLuint&         numNodes)
{
    if (currentLevel >= levels)
        return;

    const float degPerChild = 360.0f / (float)childCount;
    SLuint      mod         = currentLevel % 3;

    float scaleFactor = 0.25f;

    for (SLuint i = 0; i < childCount; i++)
    {
        numNodes++;
        string childName = "Node" + std::to_string(numNodes) +
                           "-L" + std::to_string(currentLevel) +
                           "-C" + std::to_string(i);
        SLNode* child = new SLNode(meshes[numNodes % meshes.size()], childName);

        child->rotate((float)i * degPerChild, 0, 0, 1);
        child->translate(2, 0, 0);
        child->scale(scaleFactor);

        // Node animation on child node
        string       animName  = "Anim" + std::to_string(numNodes);
        SLAnimation* childAnim = s->animManager().createNodeAnimation(animName.c_str(),
                                                                      60,
                                                                      true,
                                                                      EC_linear,
                                                                      AL_loop);
        childAnim->createNodeAnimTrackForRotation360(child, {0, 0, 1});

        parent->addChild(child);

        addUniverseLevel(am,
                         s,
                         child,
                         parentID,
                         currentLevel + 1,
                         levels,
                         childCount,
                         materials,
                         meshes,
                         numNodes);
    }
}
//-----------------------------------------------------------------------------
//! Generates the Jan's Universe scene
void AppDemoSceneJansUniverse::generateUniverse(SLAssetManager* am,
                                                SLScene*        s,
                                                SLNode*         parent,
                                                SLint           parentID,
                                                SLuint          levels,
                                                SLuint          childCount,
                                                SLVMaterial&    materials,
                                                SLVMesh&        meshes)
{
    // Point light without mesh
    SLLightSpot* light = new SLLightSpot(am,
                                         s,
                                         0,
                                         0,
                                         0,
                                         1.0f,
                                         180,
                                         0,
                                         1000,
                                         1000,
                                         true);
    light->attenuation(1, 0, 0);
    light->scale(10, 10, 10);
    light->diffuseColor({1.0f, 1.0f, 0.5f});

    // Node animation on light node
    SLAnimation* lightAnim = s->animManager().createNodeAnimation("anim0",
                                                                  60,
                                                                  true,
                                                                  EC_linear,
                                                                  AL_loop);
    lightAnim->createNodeAnimTrackForRotation360(light,
                                                 SLVec3f(0, 1, 0));

    parent->addChild(light);

    SLuint numNodes = 1;

    addUniverseLevel(am,
                     s,
                     light,
                     parentID,
                     1,
                     levels,
                     childCount,
                     materials,
                     meshes,
                     numNodes);
}
//------------------------------------------------------------------------------