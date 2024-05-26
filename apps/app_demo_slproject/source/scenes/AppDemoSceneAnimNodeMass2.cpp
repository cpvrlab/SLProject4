//#############################################################################
//  File:      AppDemoSceneAnimNodeMass2.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneAnimNodeMass2.h>
#include <AppDemo.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLSphere.h>

//-----------------------------------------------------------------------------
AppDemoSceneAnimNodeMass2::AppDemoSceneAnimNodeMass2()
  : SLScene("Benchmark Node Animation Test Scene")
{
    info("Performance test for transform updates from many animations.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneAnimNodeMass2::registerAssetsToLoad(SLAssetLoader& al)
{
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneAnimNodeMass2::assemble(SLAssetManager* am,
                                              SLSceneView*    sv)
{
    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->clipNear(0.1f);
    cam1->clipFar(100);
    cam1->translation(0, 2.5f, 20);
    cam1->focalDist(20);
    cam1->lookAt(0, 2.5f, 0);
    cam1->background().colors(SLCol4f(0.1f, 0.1f, 0.1f));
    cam1->setInitialState();

    SLLightSpot* light1 = new SLLightSpot(am,
                                          this,
                                          15,
                                          15,
                                          15,
                                          0.3f);
    light1->powers(0.2f, 0.8f, 1.0f);
    light1->attenuation(1, 0, 0);

    SLNode* scene = new SLNode;
    root3D(scene);
    scene->addChild(cam1);
    scene->addChild(light1);

    // Generate NUM_MAT materials
    const int   NUM_MAT = 20;
    SLVMaterial mat;
    for (int i = 0; i < NUM_MAT; ++i)
    {
        SLGLTexture* texC = new SLGLTexture(am,
                                            AppDemo::texturePath + "earth2048_C_Q95.jpg"); // color map

        SLstring matName = "mat-" + std::to_string(i);
        mat.push_back(new SLMaterial(am,
                                     matName.c_str(),
                                     texC));
        SLCol4f color;
        color.hsva2rgba(SLVec4f(Utils::TWOPI * (float)i / (float)NUM_MAT,
                                1.0f,
                                1.0f));
        mat[i]->diffuse(color);
    }

    // create rotating sphere group
    SLint maxDepth = 5;

    SLint resolution = 18;
    scene->addChild(RotatingSpheres(am,
                                    this,
                                    maxDepth,
                                    0,
                                    0,
                                    0,
                                    1,
                                    resolution,
                                    mat));

    sv->camera(cam1);
    sv->doWaitOnIdle(false);
}
//-----------------------------------------------------------------------------
//! Creates a recursive rotating sphere group used for performance test
/*!
 * This performance benchmark is expensive in terms of world matrix updates
 * because all sphere groups rotate. Therefore all children need to update
 * their wm every frame.
 * @param am Pointer to the asset manager
 * @param s Pointer to project scene aka asset manager
 * @param depth Max. allowed recursion depth
 * @param x Position in x direction
 * @param y Position in y direction
 * @param z Position in z direction
 * @param scale Scale factor > 0 and < 1 for the children spheres
 * @param resolution NO. of stack and slices of the spheres
 * @param mat Reference to an vector of materials
 * @return Group node of spheres
 */
SLNode* AppDemoSceneAnimNodeMass2::RotatingSpheres(SLAssetManager* am,
                                                        SLScene*        s,
                                                        SLint           depth,
                                                        SLfloat         x,
                                                        SLfloat         y,
                                                        SLfloat         z,
                                                        SLfloat         scale,
                                                        SLuint          resolution,
                                                        SLVMaterial&    mat)
{
    assert(depth >= 0);
    assert(scale >= 0.0f && scale <= 1.0f);
    assert(resolution > 0 && resolution < 64);

    // Choose the material index randomly
    SLint iMat = Utils::random(0, (int)mat.size() - 1);

    // Generate unique names for meshes, nodes and animations
    static int sphereNum = 0;
    string     meshName  = "Mesh" + std::to_string(sphereNum);
    string     animName  = "Anim" + std::to_string(sphereNum);
    string     nodeName  = "Node" + std::to_string(sphereNum);
    sphereNum++;

    SLAnimation* nodeAnim = s->animManager().createNodeAnimation(animName,
                                                                 60,
                                                                 true,
                                                                 EC_linear,
                                                                 AL_loop);
    if (depth == 0)
    {
        SLSphere* sphere  = new SLSphere(am,
                                        5.0f * scale,
                                        resolution,
                                        resolution,
                                        meshName,
                                        mat[iMat]);
        SLNode*   sphNode = new SLNode(sphere, nodeName);
        sphNode->translate(x, y, z, TS_object);
        nodeAnim->createNodeAnimTrackForRotation360(sphNode,
                                                    SLVec3f(0, 1, 0));
        return sphNode;
    }
    else
    {
        depth--;

        // decrease resolution to reduce memory consumption
        if (resolution > 8)
            resolution -= 2;

        SLNode* sGroup = new SLNode(new SLSphere(am,
                                                 5.0f * scale,
                                                 resolution,
                                                 resolution,
                                                 meshName,
                                                 mat[iMat]),
                                    nodeName);
        sGroup->translate(x, y, z, TS_object);
        nodeAnim->createNodeAnimTrackForRotation360(sGroup, SLVec3f(0, 1, 0));
        sGroup->addChild(RotatingSpheres(am, s, depth, 6.43951f * scale, 0, 1.72546f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSpheres(am, s, depth, 1.72546f * scale, 0, 6.43951f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSpheres(am, s, depth, -4.71405f * scale, 0, 4.71405f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSpheres(am, s, depth, -6.43951f * scale, 0, -1.72546f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSpheres(am, s, depth, -1.72546f * scale, 0, -6.43951f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSpheres(am, s, depth, 4.71405f * scale, 0, -4.71405f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSpheres(am, s, depth, 2.72166f * scale, 5.44331f * scale, 2.72166f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSpheres(am, s, depth, -3.71785f * scale, 5.44331f * scale, 0.99619f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSpheres(am, s, depth, 0.99619f * scale, 5.44331f * scale, -3.71785f * scale, scale / 3.0f, resolution, mat));
        return sGroup;
    }
}
//-----------------------------------------------------------------------------
