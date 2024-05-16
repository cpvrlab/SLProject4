// #############################################################################
//   File:      AppDemoSceneLoad.cpp
//   Date:      February 2018
//   Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//   Authors:   Marcus Hudritsch
//   License:   This software is provided under the GNU General Public License
//              Please visit: http://opensource.org/licenses/GPL-3.0
// #############################################################################

#include <AppScene.h>
#include <SLEnums.h>
#include <SL.h>
#include <GlobalTimer.h>

#include <CVCapture.h>
#include <CVTrackedAruco.h>
#include <CVTrackedChessboard.h>
#include <CVTrackedFaces.h>
#include <CVTrackedFeatures.h>
#include <CVTrackedMediaPipeHands.h>
#include <CVCalibrationEstimator.h>

#include <SLAlgo.h>
#include <AppDemo.h>
#include <SLAssetManager.h>
#include <SLAssimpImporter.h>
#include <SLScene.h>
#include <SLSceneView.h>
#include <SLBox.h>
#include <SLCone.h>
#include <SLCoordAxis.h>
#include <SLCylinder.h>
#include <SLDisk.h>
#include <SLGrid.h>
#include <SLLens.h>
#include <SLLightDirect.h>
#include <SLLightRect.h>
#include <SLLightSpot.h>
#include <SLParticleSystem.h>
#include <SLPoints.h>
#include <SLPolygon.h>
#include <SLRectangle.h>
#include <SLSkybox.h>
#include <SLSphere.h>
#include <SLText.h>
#include <SLTexColorLUT.h>
#include <SLScene.h>
#include <SLGLProgramManager.h>
#include <SLGLTexture.h>
#include <Profiler.h>
#include "AppDemoGui.h"
#include <SLDeviceLocation.h>
#include <SLNodeLOD.h>
#include <imgui_color_gradient.h> // For color over life, need to create own color interpolator
#include <SLEntities.h>
#include <SLFileStorage.h>
#include <SLAssetLoader.h>
#include <SLEnums.h>

#include <AppDemoScene2Dand3DText.h>
#include <AppDemoSceneAnimationNode.h>
#include <AppDemoSceneAnimationNodeMass.h>
#include <AppDemoSceneAnimationSkinned.h>
#include <AppDemoSceneAnimationSkinnedMass.h>
#include <AppDemoSceneEmpty.h>
#include <AppDemoSceneFigure.h>
#include <AppDemoSceneFrustum.h>
#include <AppDemoSceneGLTF.h>
#include <AppDemoSceneMeshLoad.h>
#include <AppDemoSceneMinimal.h>
#include <AppDemoSceneLegacy.h>
#include <AppDemoScenePointClouds.h>
#include <AppDemoSceneRevolver.h>
#include <AppDemoSceneRobot.h>
#include <AppDemoSceneSuzanne.h>
#include <AppDemoSceneShaderBlinn.h>
#include <AppDemoSceneShaderBump.h>
#include <AppDemoSceneShaderCook.h>
#include <AppDemoSceneShaderEarth.h>
#include <AppDemoSceneShaderIBL.h>
#include <AppDemoSceneShaderParallax.h>
#include <AppDemoSceneShaderSkybox.h>
#include <AppDemoSceneShaderWave.h>
#include <AppDemoSceneShadowBasic.h>
#include <AppDemoSceneShadowCascaded.h>
#include <AppDemoSceneShadowLightTypes.h>
#include <AppDemoSceneShadowLightSpot.h>
#include <AppDemoSceneShadowLightPoint.h>
#include <AppDemoSceneTextureBlend.h>
#include <AppDemoSceneTextureCompression.h>
#include <AppDemoSceneTextureFilter.h>
#include <AppDemoSceneVideoChessboard.h>
#include <AppDemoSceneVideoTexture.h>
#include <AppDemoSceneVideoTrackAruco.h>
#include <AppDemoSceneVideoTrackFace.h>
#include <AppDemoSceneVolumeRayCast.h>
#include <AppDemoSceneVolumeRayCastLighted.h>
#include <AppDemoSceneZFighting.h>

#ifdef SL_BUILD_WAI
#    include <CVTrackedWAI.h>
#endif

//-----------------------------------------------------------------------------
// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;
extern CVTracked*   gVideoTracker;
extern SLNode*      gVideoTrackedNode;
//-----------------------------------------------------------------------------
//! Global pointer to 3D MRI texture for volume rendering for threaded loading
SLGLTexture* gTexMRI3D = nullptr;
//-----------------------------------------------------------------------------
//! Global pointer to dragon model for threaded loading
SLNode* gDragonModel = nullptr;
//-----------------------------------------------------------------------------
//! Creates a recursive sphere group used for the ray tracing scenes
SLNode* SphereGroupRT(SLAssetManager* am,
                      SLint           depth, // depth of recursion
                      SLfloat         x,
                      SLfloat         y,
                      SLfloat         z,
                      SLfloat         scale,
                      SLuint          resolution,
                      SLMaterial*     matGlass,
                      SLMaterial*     matRed)
{
    PROFILE_FUNCTION();

    SLstring name = matGlass->kt() > 0 ? "GlassSphere" : "RedSphere";
    if (depth == 0)
    {
        SLSphere* sphere  = new SLSphere(am, 0.5f * scale, resolution, resolution, name, matRed);
        SLNode*   sphNode = new SLNode(sphere, "Sphere");
        sphNode->translate(x, y, z, TS_object);
        return sphNode;
    }
    else
    {
        depth--;
        SLNode* sGroup = new SLNode(new SLSphere(am, 0.5f * scale, resolution, resolution, name, matGlass), "SphereGroupRT");
        sGroup->translate(x, y, z, TS_object);
        SLuint newRes = (SLuint)std::max((SLint)resolution - 4, 8);
        sGroup->addChild(SphereGroupRT(am, depth, 0.643951f * scale, 0, 0.172546f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, 0.172546f * scale, 0, 0.643951f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, -0.471405f * scale, 0, 0.471405f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, -0.643951f * scale, 0, -0.172546f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, -0.172546f * scale, 0, -0.643951f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, 0.471405f * scale, 0, -0.471405f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, 0.272166f * scale, 0.544331f * scale, 0.272166f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, -0.371785f * scale, 0.544331f * scale, 0.099619f * scale, scale / 3, newRes, matRed, matRed));
        sGroup->addChild(SphereGroupRT(am, depth, 0.099619f * scale, 0.544331f * scale, -0.371785f * scale, scale / 3, newRes, matRed, matRed));
        return sGroup;
    }
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
SLNode* RotatingSphereGroup(SLAssetManager* am,
                            SLScene*        s,
                            SLint           depth,
                            SLfloat         x,
                            SLfloat         y,
                            SLfloat         z,
                            SLfloat         scale,
                            SLuint          resolution,
                            SLVMaterial&    mat)
{
    PROFILE_FUNCTION();
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
        SLSphere* sphere  = new SLSphere(am, 5.0f * scale, resolution, resolution, meshName, mat[iMat]);
        SLNode*   sphNode = new SLNode(sphere, nodeName);
        sphNode->translate(x, y, z, TS_object);
        nodeAnim->createNodeAnimTrackForRotation360(sphNode, SLVec3f(0, 1, 0));
        return sphNode;
    }
    else
    {
        depth--;

        // decrease resolution to reduce memory consumption
        if (resolution > 8)
            resolution -= 2;

        SLNode* sGroup = new SLNode(new SLSphere(am, 5.0f * scale, resolution, resolution, meshName, mat[iMat]), nodeName);
        sGroup->translate(x, y, z, TS_object);
        nodeAnim->createNodeAnimTrackForRotation360(sGroup, SLVec3f(0, 1, 0));
        sGroup->addChild(RotatingSphereGroup(am, s, depth, 6.43951f * scale, 0, 1.72546f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSphereGroup(am, s, depth, 1.72546f * scale, 0, 6.43951f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSphereGroup(am, s, depth, -4.71405f * scale, 0, 4.71405f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSphereGroup(am, s, depth, -6.43951f * scale, 0, -1.72546f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSphereGroup(am, s, depth, -1.72546f * scale, 0, -6.43951f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSphereGroup(am, s, depth, 4.71405f * scale, 0, -4.71405f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSphereGroup(am, s, depth, 2.72166f * scale, 5.44331f * scale, 2.72166f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSphereGroup(am, s, depth, -3.71785f * scale, 5.44331f * scale, 0.99619f * scale, scale / 3.0f, resolution, mat));
        sGroup->addChild(RotatingSphereGroup(am, s, depth, 0.99619f * scale, 5.44331f * scale, -3.71785f * scale, scale / 3.0f, resolution, mat));
        return sGroup;
    }
}
//-----------------------------------------------------------------------------
//! Adds another level to Jan's Universe scene
void addUniverseLevel(SLAssetManager* am,
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
void generateUniverse(SLAssetManager* am,
                      SLScene*        s,
                      SLNode*         parent,
                      SLint           parentID,
                      SLuint          levels,
                      SLuint          childCount,
                      SLVMaterial&    materials,
                      SLVMesh&        meshes)
{
    // Point light without mesh
    SLLightSpot* light = new SLLightSpot(am, s, 0, 0, 0, 1.0f, 180, 0, 1000, 1000, true);
    light->attenuation(1, 0, 0);
    light->scale(10, 10, 10);
    light->diffuseColor({1.0f, 1.0f, 0.5f});

    // Node animation on light node
    SLAnimation* lightAnim = s->animManager().createNodeAnimation("anim0",
                                                                  60,
                                                                  true,
                                                                  EC_linear,
                                                                  AL_loop);
    lightAnim->createNodeAnimTrackForRotation360(light, SLVec3f(0, 1, 0));

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
//-----------------------------------------------------------------------------
//! Creates a complex fire group node
SLNode* createComplexFire(SLAssetManager* am,
                          SLScene*        s,
                          SLbool          withLight,
                          SLGLTexture*    texFireCld,
                          SLGLTexture*    texFireFlm,
                          SLint           flipbookCols,
                          SLint           flipbookRows,
                          SLGLTexture*    texCircle,
                          SLGLTexture*    texSmokeB,
                          SLGLTexture*    texSmokeW)
{
    SLNode* fireComplex = new SLNode("Fire complex node");

    // Fire light node
    if (withLight)
    {
        SLLightSpot* light1 = new SLLightSpot(am, s, 0.1f, 180.0f, false);
        light1->name("Fire light node");
        light1->translate(0, 1.0f, 0);
        light1->diffuseColor(SLCol4f(1, 0.7f, 0.2f));
        light1->diffusePower(15);
        light1->attenuation(0, 0, 1);
        fireComplex->addChild(light1);
    }

    // Fire glow mesh
    {
        SLParticleSystem* fireGlowMesh = new SLParticleSystem(am,
                                                              24,
                                                              SLVec3f(-0.1f, 0.0f, -0.1f),
                                                              SLVec3f(0.1f, 0.0f, 0.1f),
                                                              4.0f,
                                                              texFireCld,
                                                              "Fire glow PS",
                                                              texFireFlm);
        fireGlowMesh->timeToLive(2.0f);
        fireGlowMesh->billboardType(BT_Camera);
        fireGlowMesh->radiusW(0.4f);
        fireGlowMesh->radiusH(0.4f);
        fireGlowMesh->doShape(false);
        fireGlowMesh->doRotation(true);
        fireGlowMesh->doRotRange(true);
        fireGlowMesh->doSizeOverLT(false);
        fireGlowMesh->doAlphaOverLT(false);
        fireGlowMesh->doColorOverLT(false);
        fireGlowMesh->doBlendBrightness(true);
        fireGlowMesh->color(SLCol4f(0.925f, 0.5f, 0.097f, 0.199f));
        fireGlowMesh->doAcceleration(false);
        SLNode* flameGlowNode = new SLNode(fireGlowMesh, "Fire glow node");
        flameGlowNode->translate(0, 0.15f, 0);
        fireComplex->addChild(flameGlowNode);
    }

    // Fire flame mesh
    {
        SLParticleSystem* fireFlameMesh = new SLParticleSystem(am,
                                                               1,
                                                               SLVec3f(0.0f, 0.0f, 0.0f),
                                                               SLVec3f(0.0f, 0.0f, 0.0f),

                                                               1.0f,
                                                               texFireCld,
                                                               "Fire flame PS",
                                                               texFireFlm);
        // Fire flame flipbook settings
        fireFlameMesh->flipbookColumns(flipbookCols);
        fireFlameMesh->flipbookRows(flipbookRows);
        fireFlameMesh->doFlipBookTexture(true);
        fireFlameMesh->doCounterGap(false); // We don't want to have flickering

        fireFlameMesh->doAlphaOverLT(false);
        fireFlameMesh->doSizeOverLT(false);
        fireFlameMesh->doRotation(false);

        fireFlameMesh->frameRateFB(64);
        fireFlameMesh->radiusW(0.6f);
        fireFlameMesh->radiusH(0.6f);
        fireFlameMesh->scale(1.2f);
        fireFlameMesh->billboardType(BT_Vertical);

        // Fire flame color
        fireFlameMesh->doColor(true);
        fireFlameMesh->color(SLCol4f(0.52f, 0.47f, 0.32f, 1.0f));
        fireFlameMesh->doBlendBrightness(true);

        // Fire flame size
        fireFlameMesh->doSizeOverLTCurve(true);
        float sizeCPArrayFl[4] = {0.0f, 1.25f, 0.0f, 1.0f};
        fireFlameMesh->bezierControlPointSize(sizeCPArrayFl);
        float sizeSEArrayFl[4] = {0.0f, 1.0f, 1.0f, 1.0f};
        fireFlameMesh->bezierStartEndPointSize(sizeSEArrayFl);
        fireFlameMesh->generateBernsteinPSize();

        // Fire flame node
        SLNode* fireFlameNode = new SLNode(fireFlameMesh, "Fire flame node");
        fireFlameNode->translate(0.0f, 0.7f, 0.0f, TS_object);
        fireComplex->addChild(fireFlameNode);
    }

    // Fire smoke black mesh
    {
        SLParticleSystem* fireSmokeB = new SLParticleSystem(am,
                                                            8,
                                                            SLVec3f(0.0f, 1.0f, 0.0f),
                                                            SLVec3f(0.0f, 0.7f, 0.0f),
                                                            2.0f,
                                                            texSmokeB,
                                                            "Fire smoke black PS",
                                                            texFireFlm);
        fireSmokeB->doColor(false);

        // Fire smoke black size
        fireSmokeB->doSizeOverLT(true);
        fireSmokeB->doSizeOverLTCurve(true);
        float sizeCPArraySB[4] = {0.0f, 1.0f, 1.0f, 2.0f};
        fireSmokeB->bezierControlPointSize(sizeCPArraySB);
        float sizeSEArraySB[4] = {0.0f, 1.0f, 1.0f, 2.0f};
        fireSmokeB->bezierStartEndPointSize(sizeSEArraySB);
        fireSmokeB->generateBernsteinPSize();

        // Fire smoke black alpha
        fireSmokeB->doAlphaOverLT(true);
        fireSmokeB->doAlphaOverLTCurve(true);
        float alphaCPArraySB[4] = {0.0f, 0.4f, 1.0f, 0.4f};
        fireSmokeB->bezierControlPointAlpha(alphaCPArraySB);
        float alphaSEArraySB[4] = {0.0f, 0.0f, 1.0f, 0.0f};
        fireSmokeB->bezierStartEndPointAlpha(alphaSEArraySB);
        fireSmokeB->generateBernsteinPAlpha();

        // Fire smoke black acceleration
        fireSmokeB->doAcceleration(true);
        fireSmokeB->doAccDiffDir(true);
        fireSmokeB->acceleration(0.0f, 0.25f, 0.3f);

        SLNode* fireSmokeBlackNode = new SLNode(fireSmokeB, "Fire smoke black node");
        fireSmokeBlackNode->translate(0.0f, 0.9f, 0.0f, TS_object);
        fireComplex->addChild(fireSmokeBlackNode);
    }

    // Fire smoke white mesh
    {
        SLParticleSystem* fireSmokeW = new SLParticleSystem(am,
                                                            40,
                                                            SLVec3f(0.0f, 0.8f, 0.0f),
                                                            SLVec3f(0.0f, 0.6f, 0.0f),
                                                            4.0f,
                                                            texSmokeW,
                                                            "Fire smoke white PS",
                                                            texFireFlm);

        fireSmokeW->doColor(false);

        // Size
        fireSmokeW->doSizeOverLT(true);
        fireSmokeW->doSizeOverLTCurve(true);
        float sizeCPArraySW[4] = {0.0f, 0.5f, 1.0f, 2.0f};
        fireSmokeW->bezierControlPointSize(sizeCPArraySW);
        float sizeSEArraySW[4] = {0.0f, 0.5f, 1.0f, 2.0f};
        fireSmokeW->bezierStartEndPointSize(sizeSEArraySW);
        fireSmokeW->generateBernsteinPSize();

        // Alpha
        fireSmokeW->doAlphaOverLT(true);
        fireSmokeW->doAlphaOverLTCurve(true);
        float alphaCPArraySW[4] = {0.0f, 0.018f, 1.0f, 0.018f};
        fireSmokeW->bezierControlPointAlpha(alphaCPArraySW);
        float alphaSEArraySW[4] = {0.0f, 0.0f, 1.0f, 0.0f};
        fireSmokeW->bezierStartEndPointAlpha(alphaSEArraySW);
        fireSmokeW->generateBernsteinPAlpha();

        // Acceleration
        fireSmokeW->doAcceleration(true);
        fireSmokeW->doAccDiffDir(true);
        fireSmokeW->acceleration(0.0f, 0.25f, 0.3f);

        SLNode* fireSmokeWNode = new SLNode(fireSmokeW, "Fire smoke white node");
        fireSmokeWNode->translate(0.0f, 0.9f, 0.0f, TS_object);
        fireComplex->addChild(fireSmokeWNode);
    }

    // Fire sparks rising mesh
    {
        SLParticleSystem* fireSparksRising = new SLParticleSystem(am,
                                                                  30,
                                                                  SLVec3f(-0.5f, 1, -0.5f),
                                                                  SLVec3f(0.5f, 2, 0.5f),
                                                                  1.2f,
                                                                  texCircle,
                                                                  "Fire sparks rising PS",
                                                                  texFireFlm);
        fireSparksRising->scale(0.05f);
        fireSparksRising->radiusH(0.8f);
        fireSparksRising->radiusW(0.3f);
        fireSparksRising->doShape(true);
        fireSparksRising->doRotation(false);
        fireSparksRising->shapeType(ST_Sphere);
        fireSparksRising->shapeRadius(0.05f);
        fireSparksRising->doAcceleration(true);
        fireSparksRising->acceleration(0, 1.5f, 0);
        fireSparksRising->doColor(true);
        fireSparksRising->doColorOverLT(true);
        fireSparksRising->doBlendBrightness(true);
        fireSparksRising->colorPoints().clear();
        fireSparksRising->colorPoints().push_back(SLColorLUTPoint(SLCol3f::WHITE, 0.0f));
        fireSparksRising->colorPoints().push_back(SLColorLUTPoint(SLCol3f::YELLOW, 0.5f));
        fireSparksRising->colorPoints().push_back(SLColorLUTPoint(SLCol3f::RED, 1.0f));
        ImGradient gradient;
        gradient.getMarks().clear();
        for (auto cp : fireSparksRising->colorPoints())
            gradient.addMark(cp.pos, ImColor(cp.color.r, cp.color.g, cp.color.b));
        fireSparksRising->colorArr(gradient.cachedValues());
        fireSparksRising->doSizeOverLT(false);
        fireSparksRising->doAlphaOverLT(false);
        fireSparksRising->doGravity(false);
        fireComplex->addChild(new SLNode(fireSparksRising, "Fire sparks rising node"));
    }

    return fireComplex;
}
//-----------------------------------------------------------------------------
SLNode* createTorchFire(SLAssetManager* am,
                        SLScene*        s,
                        SLbool          withLight,
                        SLGLTexture*    texFireCld,
                        SLGLTexture*    texFireFlm,
                        SLint           flipbookCols,
                        SLint           flipbookRows)
{

    SLNode* torchFire = new SLNode("Fire torch node");

    // Fire light node
    if (withLight)
    {
        SLLightSpot* light1 = new SLLightSpot(am, s, 0.1f, 180.0f, false);
        light1->name("Fire light node");
        light1->translate(0, 0, 0);
        light1->diffuseColor(SLCol4f(1, 0.4f, 0.0f));
        light1->diffusePower(2);
        light1->attenuation(0, 0, 1);
        torchFire->addChild(light1);
    }

    // Fire glow mesh
    {
        SLParticleSystem* fireGlow = new SLParticleSystem(am,
                                                          40,
                                                          SLVec3f(-0.1f, 0.0f, -0.1f),
                                                          SLVec3f(0.1f, 0.0f, 0.1f),
                                                          1.5f,
                                                          texFireCld,
                                                          "Torch Glow PS",
                                                          texFireFlm);
        fireGlow->color(SLCol4f(0.9f, 0.5f, 0, 0.63f));
        fireGlow->doBlendBrightness(true);
        fireGlow->radiusW(0.15f);
        fireGlow->radiusH(0.15f);
        fireGlow->doSizeOverLT(false);
        SLNode* fireGlowNode = new SLNode(fireGlow, "Torch Glow Node");
        fireGlowNode->translate(0, -0.4f, 0);
        torchFire->addChild(fireGlowNode);
    }

    // Fire torches
    {
        SLParticleSystem* torchFlame = new SLParticleSystem(am,
                                                            1,
                                                            SLVec3f(0.0f, 0.0f, 0.0f),
                                                            SLVec3f(0.0f, 0.0f, 0.0f),
                                                            4.0f,
                                                            texFireCld,
                                                            "Torch Flame PS",
                                                            texFireFlm);
        torchFlame->flipbookColumns(flipbookCols);
        torchFlame->flipbookRows(flipbookRows);
        torchFlame->doFlipBookTexture(true);
        torchFlame->doCounterGap(false); // We don't want to have flickering
        torchFlame->doAlphaOverLT(false);
        torchFlame->doSizeOverLT(false);
        torchFlame->doRotation(false);
        torchFlame->doColor(false);
        torchFlame->frameRateFB(64);
        torchFlame->radiusW(0.3f);
        torchFlame->radiusH(0.8f);
        torchFlame->billboardType(BT_Vertical);
        SLNode* torchFlameNode = new SLNode(torchFlame, "Torch Flame Node");
        torchFlameNode->translate(0, 0.3f, 0);
        torchFire->addChild(torchFlameNode);
    }

    return torchFire;
}
//-----------------------------------------------------------------------------
//! appDemoLoadScene builds a scene from source code.
/*! appDemoLoadScene builds a scene from source code. Such a function must be
 passed as a void*-pointer to slCreateScene. It will be called from within
 slCreateSceneView as soon as the view is initialized. You could separate
 different scene by a different sceneID.<br>
 The purpose is to assemble a scene by creating scenegraph objects with nodes
 (SLNode) and meshes (SLMesh). See the scene with SID_Minimal for a minimal
 example of the different steps.
*/
void appDemoLoadScene(SLAssetManager* am,
                      SLScene*        s,
                      SLSceneView*    sv,
                      SLSceneID       sceneID)
{
    PROFILE_FUNCTION();

    SLfloat startLoadMS = GlobalTimer::timeMS();

    SLGLState* stateGL = SLGLState::instance();

    SLstring texPath    = AppDemo::texturePath;
    SLstring dataPath   = AppDemo::dataPath;
    SLstring modelPath  = AppDemo::modelPath;
    SLstring shaderPath = AppDemo::shaderPath;
    SLstring configPath = AppDemo::configPath;

#ifdef SL_USE_ENTITIES_DEBUG
    SLScene::entities.dump(true);
#endif

    if (sceneID == SID_VideoTrackFeature2DMain) //............................................
    {
        /*
        The tracking of markers is done in AppDemoVideo::onUpdateVideo by calling the specific
        CVTracked::track method. If a marker was found it overwrites the linked nodes
        object matrix (SLNode::_om). If the linked node is the active camera the found
        transform is additionally inversed. This would be the standard augmented realtiy
        use case.
        */

        s->name("Track 2D Features");
        s->info("Augmented Reality 2D Feature Tracking: You need to print out the stones image target from the file data/calibrations/vuforia_markers.pdf");

        // Create video texture on global pointer updated in AppDemoVideo
        gVideoTexture = new SLGLTexture(am, texPath + "LiveVideoError.png", GL_LINEAR, GL_LINEAR);

        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 2, 60);
        cam1->lookAt(15, 15, 0);
        cam1->clipNear(0.1f);
        cam1->clipFar(1000.0f); // Increase to infinity?
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        cam1->background().texture(gVideoTexture);
        CVCapture::instance()->videoType(VT_MAIN);

        SLLightSpot* light1 = new SLLightSpot(am, s, 420, 420, 420, 1);
        light1->powers(1.0f, 1.0f, 1.0f);

        SLLightSpot* light2 = new SLLightSpot(am, s, -450, -340, 420, 1);
        light2->powers(1.0f, 1.0f, 1.0f);
        light2->attenuation(1, 0, 0);

        SLLightSpot* light3 = new SLLightSpot(am, s, 450, -370, 0, 1);
        light3->powers(1.0f, 1.0f, 1.0f);
        light3->attenuation(1, 0, 0);

        // Coordinate axis node
        SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
        axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
        axis->scale(100);
        axis->rotate(-90, 1, 0, 0);

        // Yellow center box
        SLMaterial* yellow = new SLMaterial(am, "mY", SLCol4f(1, 1, 0, 0.5f));
        SLNode*     box    = new SLNode(new SLBox(am, 0, 0, 0, 100, 100, 100, "Box", yellow), "Box Node");
        box->rotate(-90, 1, 0, 0);

        // Scene structure
        SLNode* scene = new SLNode("Scene");
        s->root3D(scene);
        scene->addChild(light1);
        scene->addChild(light2);
        scene->addChild(light3);
        scene->addChild(axis);
        scene->addChild(box);
        scene->addChild(cam1);

        // Create feature gVideoTracker and let it pose the camera for AR posing
        gVideoTracker = new CVTrackedFeatures(texPath + "features_stones.jpg");
        // gVideoTracker = new CVTrackedFeatures("features_abstract.jpg");
        gVideoTracker->drawDetection(true);
        gVideoTrackedNode = cam1;

        sv->doWaitOnIdle(false); // for constant video feed
        sv->camera(cam1);
        AppDemo::devRot.isUsed(true);
    }
#ifdef SL_BUILD_WITH_MEDIAPIPE
    else if (sceneID == SID_VideoTrackMediaPipeHandsMain) //.......................................
    {
        CVCapture::instance()->videoType(VT_MAIN);
        s->name("Tracking Hands with MediaPipe (main cam.)");
        s->info("Tracking hands with MediaPipe.");

        gVideoTexture = new SLGLTexture(am,
                                       texPath + "LiveVideoError.png",
                                       GL_LINEAR,
                                       GL_LINEAR);

        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->background().texture(gVideoTexture);

        SLNode* scene = new SLNode("Scene");
        s->root3D(scene);

        gVideoTracker     = new CVTrackedMediaPipeHands(AppDemo::dataPath);
        gVideoTrackedNode = cam1;
        gVideoTracker->drawDetection(true);

        sv->doWaitOnIdle(false);
        sv->camera(cam1);
    }
#endif
#ifdef SL_BUILD_WAI
    else if (sceneID == SID_VideoTrackWAI) //......................................................
    {
        CVCapture::instance()->videoType(VT_MAIN);
        s->name("Track WAI (main cam.)");
        s->info("Track the scene with a point cloud built with the WAI (Where Am I) library.");

        // Create video texture on global pointer updated in AppDemoVideo
        gVideoTexture = new SLGLTexture(am, texPath + "LiveVideoError.png", GL_LINEAR, GL_LINEAR);

        // Material
        SLMaterial* yellow = new SLMaterial(am, "mY", SLCol4f(1, 1, 0, 0.5f));
        SLMaterial* cyan   = new SLMaterial(am, "mY", SLCol4f(0, 1, 1, 0.5f));

        // Create a scene group node
        SLNode* scene = new SLNode("scene node");
        s->root3D(scene);

        // Create a camera node 1
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 0, 5);
        cam1->lookAt(0, 0, 0);
        cam1->fov(CVCapture::instance()->activeCamera->calibration.cameraFovVDeg());
        cam1->background().texture(gVideoTexture);
        cam1->setInitialState();
        scene->addChild(cam1);

        // Create a light source node
        SLLightSpot* light1 = new SLLightSpot(am, s, 0.02f);
        light1->translation(0.12f, 0.12f, 0.12f);
        light1->name("light node");
        scene->addChild(light1);

        // Get the half edge length of the aruco marker
        SLfloat edgeLen = 0.1f;
        SLfloat he      = edgeLen * 0.5f;

        // Build mesh & node that will be tracked by the 1st marker (camera)
        SLBox*  box1      = new SLBox(am, -he, -he, -he, he, he, he, "Box 1", yellow);
        SLNode* boxNode1  = new SLNode(box1, "Box Node 1");
        SLNode* axisNode1 = new SLNode(new SLCoordAxis(am), "Axis Node 1");
        axisNode1->setDrawBitsRec(SL_DB_MESHWIRED, false);
        axisNode1->scale(edgeLen);
        axisNode1->translate(-he, -he, -he, TS_parent);
        boxNode1->addChild(axisNode1);
        boxNode1->setDrawBitsRec(SL_DB_CULLOFF, true);
        boxNode1->translate(0.0f, 0.0f, 1.0f, TS_world);
        scene->addChild(boxNode1);

        // Create OpenCV Tracker for the box node
        std::string vocFileName;
#    if USE_FBOW
        vocFileName = "voc_fbow.bin";
#    else
        vocFileName = "ORBvoc.bin";
#    endif
        gVideoTracker = new CVTrackedWAI(Utils::findFile(vocFileName, {AppDemo::calibIniPath, AppDemo::exePath}));
        gVideoTracker->drawDetection(true);
        gVideoTrackedNode = cam1;

        sv->camera(cam1);

        // Turn on constant redraw
        sv->doWaitOnIdle(false);
    }
#endif
    else if (sceneID == SID_VideoSensorAR) //......................................................
    {
        // Set scene name and info string
        s->name("Video Sensor AR");
        s->info("Minimal scene to test the devices IMU and GPS Sensors. See the sensor information. GPS needs a few sec. to improve the accuracy.");

        // Create video texture on global pointer updated in AppDemoVideo
        gVideoTexture = new SLGLTexture(am, texPath + "LiveVideoError.png", GL_LINEAR, GL_LINEAR);

        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 0, 60);
        cam1->lookAt(0, 0, 0);
        cam1->fov(CVCapture::instance()->activeCamera->calibration.cameraFovVDeg());
        cam1->clipNear(0.1f);
        cam1->clipFar(10000.0f);
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        cam1->background().texture(gVideoTexture);

        // Turn on main video
        CVCapture::instance()->videoType(VT_MAIN);

        // Create directional light for the sunlight
        SLLightDirect* light = new SLLightDirect(am, s, 1.0f);
        light->powers(1.0f, 1.0f, 1.0f);
        light->attenuation(1, 0, 0);

        // Let the sun be rotated by time and location
        AppDemo::devLoc.sunLightNode(light);

        SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
        axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
        axis->scale(2);
        axis->rotate(-90, 1, 0, 0);

        // Yellow center box
        SLMaterial* yellow = new SLMaterial(am, "mY", SLCol4f(1, 1, 0, 0.5f));
        SLNode*     box    = new SLNode(new SLBox(am, -.5f, -.5f, -.5f, .5f, .5f, .5f, "Box", yellow), "Box Node");

        // Scene structure
        SLNode* scene = new SLNode("Scene");
        s->root3D(scene);
        scene->addChild(light);
        scene->addChild(cam1);
        scene->addChild(box);
        scene->addChild(axis);

        sv->camera(cam1);

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
        // activate rotation and gps sensor
        AppDemo::devRot.isUsed(true);
        AppDemo::devRot.zeroYawAtStart(false);
        AppDemo::devLoc.isUsed(true);
        AppDemo::devLoc.useOriginAltitude(true);
        AppDemo::devLoc.hasOrigin(false);
        cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
        cam1->camAnim(SLCamAnim::CA_turntableYUp);
        AppDemo::devRot.zeroYawAtStart(true);
#endif

        sv->doWaitOnIdle(false); // for constant video feed
    }
    else if (sceneID == SID_ParticleSystem_First) //...............................................
    {
        // Set scene name and info string
        s->name("First particle system");
        s->info("First scene with a particle system");

        // Create a scene group node
        SLNode* scene = new SLNode("scene node");
        s->root3D(scene);

        // Create and add camera
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 1.5f, 4);
        cam1->lookAt(0, 1.5f, 0);
        scene->addChild(cam1);

        // Create textures and materials
        SLGLTexture* texC        = new SLGLTexture(am,
                                            texPath + "ParticleSmoke_08_C.png");
        SLGLTexture* texFlipbook = new SLGLTexture(am,
                                                   texPath + "ParticleSmoke_03_8x8_C.png");

        // Create a light source node
        SLLightSpot* light1 = new SLLightSpot(am, s, 0.3f);
        light1->translation(5, 5, 5);
        light1->name("light node");
        scene->addChild(light1);

        // Create meshes and nodes
        SLParticleSystem* ps = new SLParticleSystem(am,
                                                    50,
                                                    SLVec3f(0.04f, 0.4f, 0.1f),
                                                    SLVec3f(-0.11f, 0.7f, -0.1f),
                                                    4.0f,
                                                    texC,
                                                    "Particle System",
                                                    texFlipbook);

        SLNode* pSNode = new SLNode(ps, "Particle system node");
        scene->addChild(pSNode);

        // Set background color and the root scene node
        sv->sceneViewCamera()->background().colors(SLCol4f(0.8f, 0.8f, 0.8f),
                                                   SLCol4f(0.2f, 0.2f, 0.2f));
        sv->camera(cam1);
        sv->doWaitOnIdle(false);
    }
    else if (sceneID == SID_ParticleSystem_Demo) //................................................
    {
        // Set scene name and info string
        s->name("Simple Demo Particle System");
        s->info("This most simple single particle system is meant to be improved by adding more and more features in the properties list.");

        // Create a scene group node
        SLNode* scene = new SLNode("scene node");
        s->root3D(scene);

        // Create textures and materials
        SLGLTexture* texC        = new SLGLTexture(am, texPath + "ParticleSmoke_08_C.png");
        SLGLTexture* texFlipbook = new SLGLTexture(am, texPath + "ParticleSmoke_03_8x8_C.png");

        // Create meshes and nodes
        SLParticleSystem* ps = new SLParticleSystem(am,
                                                    1,
                                                    SLVec3f(0.04f, 0.4f, 0.1f),
                                                    SLVec3f(-0.11f, 0.7f, -0.1f),
                                                    4.0f,
                                                    texC,
                                                    "Particle System",
                                                    texFlipbook);
        ps->doAlphaOverLT(false);
        ps->doSizeOverLT(false);
        ps->doRotation(false);
        ps->doColor(false);
        ps->acceleration(-0.5, 0.0, 0.0);
        ps->timeToLive(2.0f);
        SLMesh* pSMesh = ps;
        SLNode* pSNode = new SLNode(pSMesh, "Particle system node");
        scene->addChild(pSNode);

        // Set background color and the root scene node
        sv->sceneViewCamera()->background().colors(SLCol4f(0.8f, 0.8f, 0.8f),
                                                   SLCol4f(0.2f, 0.2f, 0.2f));
        // Save energy
        sv->doWaitOnIdle(false);
    }
    else if (sceneID == SID_ParticleSystem_DustStorm) //...........................................
    {
        // Set scene name and info string
        s->name("Dust storm particle system");
        s->info("This dust storm particle system uses the box shape type for distribution.\n"
                "See the properties window for the detailed settings of the particles system");

        // Create a scene group node
        SLNode* scene = new SLNode("scene node");
        s->root3D(scene);

        // Create and add camera
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 0, 55);
        cam1->lookAt(0, 0, 0);
        cam1->focalDist(55);
        scene->addChild(cam1);
        sv->camera(cam1);

        // Create textures and materials
        SLGLTexture* texC             = new SLGLTexture(am, texPath + "ParticleSmoke_08_C.png");
        SLGLTexture* texFlipbookSmoke = new SLGLTexture(am, texPath + "ParticleSmoke_03_8x8_C.png");

        // Create meshes and nodes
        // Dust storm
        SLParticleSystem* ps = new SLParticleSystem(am,
                                                    500,
                                                    SLVec3f(-0.1f, -0.5f, -5.0f),
                                                    SLVec3f(0.1f, 0.5f, -2.5f),
                                                    3.5f,
                                                    texC,
                                                    "DustStorm",
                                                    texFlipbookSmoke);
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
    else if (sceneID == SID_ParticleSystem_Fountain) //............................................
    {
        // Set scene name and info string
        s->name("Fountain particle system");
        s->info("This fountain particle system uses acceleration and gravity.\n"
                "See the properties window for the detailed settings of the particles system");

        // Create a scene group node
        SLNode* scene = new SLNode("scene node");
        s->root3D(scene);

        // Create and add camera
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, -1, 55);
        cam1->lookAt(0, -1, 0);
        cam1->focalDist(55);
        scene->addChild(cam1);
        sv->camera(cam1);

        // Create textures and materials
        SLGLTexture* texC        = new SLGLTexture(am, texPath + "ParticleCircle_05_C.png");
        SLGLTexture* texFlipbook = new SLGLTexture(am, texPath + "ParticleSmoke_03_8x8_C.png");
        // SLGLTexture* texFlipbook = new SLGLTexture(am, texPath + "ParticleSmoke_04_8x8_C.png");

        // Create a light source node
        SLLightSpot* light1 = new SLLightSpot(am, s, 0.3f);
        light1->translation(0, -1, 2);
        light1->name("light node");
        scene->addChild(light1);

        // Create meshes and nodes
        SLParticleSystem* ps     = new SLParticleSystem(am,
                                                    5000,
                                                    SLVec3f(5.0f, 15.0f, 5.0f),
                                                    SLVec3f(-5.0f, 17.0f, -5.0f),
                                                    5.0f,
                                                    texC,
                                                    "Fountain",
                                                    texFlipbook);
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
    else if (sceneID == SID_ParticleSystem_Sun) //.................................................
    {
        // Set scene name and info string
        s->name("Sun particle system");
        s->info("This sun particle system uses the sphere shape type for distribution.\n"
                "See the properties window for the detailed settings of the particles system");

        // Create a scene group node
        SLNode* scene = new SLNode("scene node");
        s->root3D(scene);

        // Create textures and materials
        SLGLTexture* texC        = new SLGLTexture(am, texPath + "ParticleSmoke_08_C.png");
        SLGLTexture* texFlipbook = new SLGLTexture(am, texPath + "ParticleSmoke_03_8x8_C.png");

        // Create meshes and nodes
        SLParticleSystem* ps = new SLParticleSystem(am,
                                                    10000,
                                                    SLVec3f(0.0f, 0.0f, 0.0f),
                                                    SLVec3f(0.0f, 0.0f, 0.0f),
                                                    4.0f,
                                                    texC,
                                                    "Sun Particle System",
                                                    texFlipbook);

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
    else if (sceneID == SID_ParticleSystem_RingOfFire) //..........................................
    {
        // Set scene name and info string
        s->name("Ring of fire particle system");
        s->info("This ring particle system uses the cone shape type for distribution.\n"
                "See the properties window for the settings of the particles system");

        // Create a scene group node
        SLNode* scene = new SLNode("scene node");
        s->root3D(scene);

        // Create textures and materials
        SLGLTexture* texC        = new SLGLTexture(am, texPath + "ParticleSmoke_08_C.png");
        SLGLTexture* texFlipbook = new SLGLTexture(am, texPath + "ParticleSmoke_03_8x8_C.png");

        // Create meshes and nodes
        SLParticleSystem* ps = new SLParticleSystem(am,
                                                    1000,
                                                    SLVec3f(0.0f, 0.0f, 0.0f),
                                                    SLVec3f(0.0f, 0.0f, 0.0f),
                                                    4.0f,
                                                    texC,
                                                    "Ring of fire Particle System",
                                                    texFlipbook);

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
    else if (sceneID == SID_ParticleSystem_FireComplex) //.........................................
    {
        // Set scene name and info string
        s->name("Fire Complex particle system");
        s->info("The fire particle systems contain each multiple sub particle systems.\n"
                "See the scenegraph window for the sub particles systems. "
                "See the properties window for the settings of the particles systems");

        // Create a scene group node
        SLNode* scene = new SLNode("scene node");
        s->root3D(scene);

        // Create and add camera
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 1.2f, 4.0f);
        cam1->lookAt(0, 1.2f, 0);
        cam1->focalDist(4.5f);
        cam1->setInitialState();
        scene->addChild(cam1);
        sv->camera(cam1);

        // Create textures and materials
        SLGLTexture* texFireCld  = new SLGLTexture(am, texPath + "ParticleFirecloudTransparent_C.png");
        SLGLTexture* texFireFlm  = new SLGLTexture(am, texPath + "ParticleFlames_06_8x8_C.png");
        SLGLTexture* texCircle   = new SLGLTexture(am, texPath + "ParticleCircle_05_C.png");
        SLGLTexture* texSmokeB   = new SLGLTexture(am, texPath + "ParticleCloudBlack_C.png");
        SLGLTexture* texSmokeW   = new SLGLTexture(am, texPath + "ParticleCloudWhite_C.png");
        SLGLTexture* texTorchFlm = new SLGLTexture(am, texPath + "ParticleFlames_04_16x4_C.png");
        SLGLTexture* texTorchSmk = new SLGLTexture(am, texPath + "ParticleSmoke_08_C.png");

        SLNode* complexFire = createComplexFire(am,
                                                s,
                                                true,
                                                texTorchSmk,
                                                texFireFlm,
                                                8,
                                                8,
                                                texCircle,
                                                texSmokeB,
                                                texSmokeW);
        scene->addChild(complexFire);

        // Room around
        {
            // Room parent node
            SLNode* room = new SLNode("Room");
            scene->addChild(room);

            // Back wall material
            SLGLTexture* texWallDIF = new SLGLTexture(am, texPath + "BrickLimestoneGray_1K_DIF.jpg", SL_ANISOTROPY_MAX, GL_LINEAR);
            SLGLTexture* texWallNRM = new SLGLTexture(am, texPath + "BrickLimestoneGray_1K_NRM.jpg", SL_ANISOTROPY_MAX, GL_LINEAR);
            SLMaterial*  matWall    = new SLMaterial(am, "mat3", texWallDIF, texWallNRM);
            matWall->specular(SLCol4f::BLACK);
            matWall->metalness(0);
            matWall->roughness(1);
            matWall->reflectionModel(RM_CookTorrance);

            // Room dimensions
            SLfloat pL = -2.0f, pR = 2.0f;  // left/right
            SLfloat pB = -0.01f, pT = 4.0f; // bottom/top
            SLfloat pN = 2.0f, pF = -2.0f;  // near/far

            // bottom rectangle
            SLNode* b = new SLNode(new SLRectangle(am, SLVec2f(pL, -pN), SLVec2f(pR, -pF), 10, 10, "Floor", matWall));
            b->rotate(90, -1, 0, 0);
            b->translate(0, 0, pB, TS_object);
            room->addChild(b);

            // far rectangle
            SLNode* f = new SLNode(new SLRectangle(am, SLVec2f(pL, pB), SLVec2f(pR, pT), 10, 10, "Wall far", matWall));
            f->translate(0, 0, pF, TS_object);
            room->addChild(f);

            // near rectangle
            SLNode* n = new SLNode(new SLRectangle(am, SLVec2f(pL, pB), SLVec2f(pR, pT), 10, 10, "Wall near", matWall));
            n->rotate(180, 0, 1, 0);
            n->translate(0, 0, pF, TS_object);
            room->addChild(n);

            // left rectangle
            SLNode* l = new SLNode(new SLRectangle(am, SLVec2f(-pN, pB), SLVec2f(-pF, pT), 10, 10, "Wall left", matWall));
            l->rotate(90, 0, 1, 0);
            l->translate(0, 0, pL, TS_object);
            room->addChild(l);

            // right rectangle
            SLNode* r = new SLNode(new SLRectangle(am, SLVec2f(pF, pB), SLVec2f(pN, pT), 10, 10, "Wall right", matWall));
            r->rotate(90, 0, -1, 0);
            r->translate(0, 0, -pR, TS_object);
            room->addChild(r);
        }

        // Firewood
        SLAssimpImporter importer;
        SLNode*          firewood = importer.load(s->animManager(),
                                         am,
                                         modelPath + "GLTF/Firewood/Firewood1.gltf",
                                         texPath,
                                         nullptr,
                                         false,
                                         true,
                                         nullptr,
                                         0.3f,
                                         true);
        firewood->scale(2);
        scene->addChild(firewood);

        // Torch
        SLNode* torchL = importer.load(s->animManager(),
                                       am,
                                       modelPath + "GLTF/Torch/Torch.gltf",
                                       texPath,
                                       nullptr,
                                       false,
                                       true,
                                       nullptr,
                                       0.3f,
                                       true);
        torchL->name("Torch Left");
        SLNode* torchR = torchL->copyRec();
        torchR->name("Torch Right");
        torchL->translate(-2, 1.5f, 0);
        torchL->rotate(90, 0, 1, 0);
        torchL->scale(2);
        scene->addChild(torchL);
        torchR->translate(2, 1.5f, 0);
        torchR->rotate(-90, 0, 1, 0);
        torchR->scale(2);
        scene->addChild(torchR);

        // Torch flame left
        SLNode* torchFlameNodeL = createTorchFire(am,
                                                  s,
                                                  true,
                                                  texTorchSmk,
                                                  texTorchFlm,
                                                  16,
                                                  4);
        torchFlameNodeL->translate(-1.6f, 2.25f, 0);
        torchFlameNodeL->name("Torch Fire Left");
        scene->addChild(torchFlameNodeL);

        // Torch flame right
        SLNode* torchFlameNodeR = createTorchFire(am,
                                                  s,
                                                  true,
                                                  texTorchSmk,
                                                  texTorchFlm,
                                                  16,
                                                  4);
        torchFlameNodeR->translate(1.6f, 2.25f, 0);
        torchFlameNodeR->name("Torch Fire Right");
        scene->addChild(torchFlameNodeR);

        // Set background color and the root scene node
        sv->sceneViewCamera()->background().colors(SLCol4f(0.8f, 0.8f, 0.8f),
                                                   SLCol4f(0.2f, 0.2f, 0.2f));
        // Save energy
        sv->doWaitOnIdle(false);
    }

    else if (sceneID == SID_Benchmark1_LargeModel) //..............................................
    {
        SLstring largeFile = modelPath + "PLY/xyzrgb_dragon/xyzrgb_dragon.ply";

        if (SLFileStorage::exists(largeFile, IOK_model))
        {
            s->name("Large Model Benchmark Scene");
            s->info("Large Model with 7.2 mio. triangles.");

            // Material for glass
            SLMaterial* diffuseMat = new SLMaterial(am, "diffuseMat", SLCol4f::WHITE, SLCol4f::WHITE);

            SLCamera* cam1 = new SLCamera("Camera 1");
            cam1->translation(0, 0, 220);
            cam1->lookAt(0, 0, 0);
            cam1->clipNear(1);
            cam1->clipFar(10000);
            cam1->focalDist(220);
            cam1->background().colors(SLCol4f(0.7f, 0.7f, 0.7f), SLCol4f(0.2f, 0.2f, 0.2f));
            cam1->setInitialState();
            cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

            SLLightSpot* light1 = new SLLightSpot(am, s, 200, 200, 200, 1);
            light1->powers(0.1f, 1.0f, 1.0f);
            light1->attenuation(1, 0, 0);

            SLAssimpImporter importer;
            gDragonModel = importer.load(s->animManager(),
                                         am,
                                         largeFile,
                                         texPath,
                                         nullptr,
                                         false, // delete tex images after build
                                         true,
                                         diffuseMat,
                                         0.2f,
                                         false,
                                         nullptr,
                                         SLProcess_Triangulate | SLProcess_JoinIdenticalVertices);

            SLNode* scene = new SLNode("Scene");
            s->root3D(scene);
            scene->addChild(light1);
            scene->addChild(gDragonModel);
            scene->addChild(cam1);

            sv->camera(cam1);
        }
    }
    else if (sceneID == SID_Benchmark2_MassiveNodes) //............................................
    {
        s->name("Massive Data Benchmark Scene");
        s->info(s->name());

        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->clipNear(0.1f);
        cam1->clipFar(100);
        cam1->translation(0, 0, 50);
        cam1->lookAt(0, 0, 0);
        cam1->focalDist(50);
        cam1->background().colors(SLCol4f(0.1f, 0.1f, 0.1f));
        cam1->setInitialState();

        SLLightSpot* light1 = new SLLightSpot(am, s, 15, 15, 15, 0.3f);
        light1->powers(0.2f, 0.8f, 1.0f);
        light1->attenuation(1, 0, 0);

        SLNode* scene = new SLNode;
        s->root3D(scene);
        scene->addChild(cam1);
        scene->addChild(light1);

        // Generate NUM_MAT materials
        const int   NUM_MAT = 20;
        SLVMaterial mat;
        for (int i = 0; i < NUM_MAT; ++i)
        {
            SLGLTexture* texC    = new SLGLTexture(am, texPath + "earth2048_C_Q95.jpg");
            SLstring     matName = "mat-" + std::to_string(i);
            mat.push_back(new SLMaterial(am, matName.c_str(), texC));
            SLCol4f color;
            color.hsva2rgba(SLVec4f(Utils::TWOPI * (float)i / (float)NUM_MAT, 1.0f, 1.0f));
            mat[i]->diffuse(color);
        }

        // create a 3D array of spheres
        SLint  halfSize = 10;
        SLuint n        = 0;
        for (SLint iZ = -halfSize; iZ <= halfSize; ++iZ)
        {
            for (SLint iY = -halfSize; iY <= halfSize; ++iY)
            {
                for (SLint iX = -halfSize; iX <= halfSize; ++iX)
                {
                    // Choose a random material index
                    SLuint   res      = 36;
                    SLint    iMat     = (SLint)Utils::random(0, NUM_MAT - 1);
                    SLstring nodeName = "earth-" + std::to_string(n);

                    // Create a new sphere and node and translate it
                    SLSphere* earth  = new SLSphere(am, 0.3f, res, res, nodeName, mat[iMat]);
                    SLNode*   sphere = new SLNode(earth);
                    sphere->translate(float(iX), float(iY), float(iZ), TS_object);
                    scene->addChild(sphere);
                    // SL_LOG("Earth: %000d (Mat: %00d)", n, iMat);
                    n++;
                }
            }
        }

        sv->camera(cam1);
        sv->doWaitOnIdle(false);
    }
    else if (sceneID == SID_Benchmark3_NodeAnimations) //..........................................
    {
        s->name("Massive Node Animation Benchmark Scene");
        s->info(s->name());

        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->clipNear(0.1f);
        cam1->clipFar(100);
        cam1->translation(0, 2.5f, 20);
        cam1->focalDist(20);
        cam1->lookAt(0, 2.5f, 0);
        cam1->background().colors(SLCol4f(0.1f, 0.1f, 0.1f));
        cam1->setInitialState();

        SLLightSpot* light1 = new SLLightSpot(am, s, 15, 15, 15, 0.3f);
        light1->powers(0.2f, 0.8f, 1.0f);
        light1->attenuation(1, 0, 0);

        SLNode* scene = new SLNode;
        s->root3D(scene);
        scene->addChild(cam1);
        scene->addChild(light1);

        // Generate NUM_MAT materials
        const int   NUM_MAT = 20;
        SLVMaterial mat;
        for (int i = 0; i < NUM_MAT; ++i)
        {
            SLGLTexture* texC    = new SLGLTexture(am, texPath + "earth2048_C_Q95.jpg"); // color map
            SLstring     matName = "mat-" + std::to_string(i);
            mat.push_back(new SLMaterial(am, matName.c_str(), texC));
            SLCol4f color;
            color.hsva2rgba(SLVec4f(Utils::TWOPI * (float)i / (float)NUM_MAT, 1.0f, 1.0f));
            mat[i]->diffuse(color);
        }

        // create rotating sphere group
        SLint maxDepth = 5;

        SLint resolution = 18;
        scene->addChild(RotatingSphereGroup(am,
                                            s,
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
    else if (sceneID == SID_Benchmark4_SkinnedAnimations) //.......................................
    {
        SLint  size         = 20;
        SLint  numAstroboys = size * size;
        SLchar name[512];
        snprintf(name, sizeof(name), "Massive Skinned Animation Benchmark w. %d individual Astroboys", numAstroboys);
        s->name(name);
        s->info(s->name());

        // Create materials
        SLMaterial* m1 = new SLMaterial(am, "m1", SLCol4f::GRAY);
        m1->specular(SLCol4f::BLACK);

        // Define a light
        SLLightSpot* light1 = new SLLightSpot(am, s, 100, 40, 100, 1);
        light1->powers(0.1f, 1.0f, 1.0f);
        light1->attenuation(1, 0, 0);

        // Define camera
        SLCamera* cam1 = new SLCamera;
        cam1->translation(0, 30, 0);
        cam1->lookAt(0, 0, 0);
        cam1->focalDist(cam1->translationOS().length());
        cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

        // Floor rectangle
        SLNode* rect = new SLNode(new SLRectangle(am,
                                                  SLVec2f(-20, -20),
                                                  SLVec2f(20, 20),
                                                  SLVec2f(0, 0),
                                                  SLVec2f(50, 50),
                                                  50,
                                                  50,
                                                  "Floor",
                                                  m1));
        rect->rotate(90, -1, 0, 0);

        SLAssimpImporter importer;

        // Assemble scene
        SLNode* scene = new SLNode("scene group");
        s->root3D(scene);
        scene->addChild(light1);
        scene->addChild(rect);
        scene->addChild(cam1);

        // create army with individual astroboys
        SLfloat offset = 1.0f;
        SLfloat z      = (float)(size - 1) * offset * 0.5f;

        for (SLint iZ = 0; iZ < size; ++iZ)
        {
            SLfloat x = -(float)(size - 1) * offset * 0.5f;

            for (SLint iX = 0; iX < size; ++iX)
            {
                SLNode* astroboy = importer.load(s->animManager(),
                                                 am,
                                                 modelPath + "DAE/AstroBoy/AstroBoy.dae",
                                                 texPath);

                s->animManager().lastAnimPlayback()->playForward();
                s->animManager().lastAnimPlayback()->playbackRate(Utils::random(0.5f, 1.5f));
                astroboy->translate(x, 0, z, TS_object);
                scene->addChild(astroboy);
                x += offset;
            }
            z -= offset;
        }

        sv->camera(cam1);
    }
    else if (sceneID == SID_Benchmark5_ColumnsNoLOD ||
             sceneID == SID_Benchmark6_ColumnsLOD) //..............................................
    {
        SLstring modelFile = modelPath + "GLTF/CorinthianColumn/Corinthian-Column-Round-LOD.gltf";
        SLstring texCFile  = modelPath + "GLTF/CorinthianColumn/PavementSlateSquare2_2K_DIF.jpg";
        SLstring texNFile  = modelPath + "GLTF/CorinthianColumn/PavementSlateSquare2_2K_NRM.jpg";

        if (SLFileStorage::exists(modelFile, IOK_model) &&
            SLFileStorage::exists(texCFile, IOK_image) &&
            SLFileStorage::exists(texNFile, IOK_image))
        {
            SLchar name[512];
            SLint  size;
            if (sceneID == SID_Benchmark5_ColumnsNoLOD)
            {
                size = 25;
                snprintf(name, sizeof(name), "%d corinthian columns without LOD", size * size);
                s->name(name);
            }
            else
            {
                size = 50;
                snprintf(name, sizeof(name), "%d corinthian columns with LOD", size * size);
                s->name(name);
            }
            s->info(s->name() + " with cascaded shadow mapping. In the Day-Time dialogue you can change the sun angle.");

            // Create ground material
            SLGLTexture* texFloorDif = new SLGLTexture(am, texCFile, SL_ANISOTROPY_MAX, GL_LINEAR);
            SLGLTexture* texFloorNrm = new SLGLTexture(am, texNFile, SL_ANISOTROPY_MAX, GL_LINEAR);
            SLMaterial*  matFloor    = new SLMaterial(am, "matFloor", texFloorDif, texFloorNrm);

            // Define camera
            SLCamera* cam1 = new SLCamera;
            cam1->translation(0, 1.7f, 20);
            cam1->lookAt(0, 1.7f, 0);
            cam1->focalDist(cam1->translationOS().length());
            cam1->clipFar(600);
            cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
            cam1->setInitialState();

            // Create directional light for the sunlight
            SLLightDirect* sunLight = new SLLightDirect(am, s, 1.0f);
            sunLight->powers(0.25f, 1.0f, 1.0f);
            sunLight->attenuation(1, 0, 0);
            sunLight->translation(0, 1.7f, 0);
            sunLight->lookAt(-1, 0, -1);
            sunLight->doSunPowerAdaptation(true);

            // Add cascaded shadow mapping
            sunLight->createsShadows(true);
            sunLight->createShadowMapAutoSize(cam1);
            sunLight->doSmoothShadows(true);
            sunLight->castsShadows(false);
            sunLight->shadowMinBias(0.003f);
            sunLight->shadowMaxBias(0.012f);

            // Let the sun be rotated by time and location
            AppDemo::devLoc.sunLightNode(sunLight);
            AppDemo::devLoc.originLatLonAlt(47.14271, 7.24337, 488.2);        // Ecke Giosa
            AppDemo::devLoc.defaultLatLonAlt(47.14260, 7.24310, 488.7 + 1.7); // auf Parkplatz

            // Floor rectangle
            SLNode* rect = new SLNode(new SLRectangle(am,
                                                      SLVec2f(-200, -200),
                                                      SLVec2f(200, 200),
                                                      SLVec2f(0, 0),
                                                      SLVec2f(50, 50),
                                                      50,
                                                      50,
                                                      "Floor",
                                                      matFloor));
            rect->rotate(90, -1, 0, 0);
            rect->castsShadows(false);

            // Load the corinthian column
            SLAssimpImporter importer;
            SLNode*          columnLOD = importer.load(s->animManager(),
                                              am,
                                              modelFile,
                                              texPath,
                                              nullptr,
                                              false,   // delete tex images after build
                                              true,    // only meshes
                                              nullptr, // no replacement material
                                              1.0f);   // 40% ambient reflection

            SLNode* columnL0 = columnLOD->findChild<SLNode>("Corinthian-Column-Round-L0");
            SLNode* columnL1 = columnLOD->findChild<SLNode>("Corinthian-Column-Round-L1");
            SLNode* columnL2 = columnLOD->findChild<SLNode>("Corinthian-Column-Round-L2");
            SLNode* columnL3 = columnLOD->findChild<SLNode>("Corinthian-Column-Round-L3");

            // Assemble scene
            SLNode* scene = new SLNode("Scene");
            s->root3D(scene);
            scene->addChild(sunLight);
            scene->addChild(rect);
            scene->addChild(cam1);

            // create loads of pillars
            SLint   numColumns = size * size;
            SLfloat offset     = 5.0f;
            SLfloat z          = (float)(size - 1) * offset * 0.5f;

            for (SLint iZ = 0; iZ < size; ++iZ)
            {
                SLfloat x = -(float)(size - 1) * offset * 0.5f;

                for (SLint iX = 0; iX < size; ++iX)
                {
                    SLint iZX = iZ * size + iX;

                    if (sceneID == SID_Benchmark5_ColumnsNoLOD)
                    {
                        // Without just the level 0 node
                        string  strNode = "Node" + std::to_string(iZX);
                        SLNode* column  = new SLNode(columnL1->mesh(), strNode + "-L0");
                        column->translate(x, 0, z, TS_object);
                        scene->addChild(column);
                    }
                    else
                    {
                        // With LOD parent node and 3 levels
                        string     strLOD    = "LOD" + std::to_string(iZX);
                        SLNodeLOD* lod_group = new SLNodeLOD(strLOD);
                        lod_group->translate(x, 0, z, TS_object);
                        lod_group->addChildLOD(new SLNode(columnL1->mesh(), strLOD + "-L0"), 0.1f, 3);
                        lod_group->addChildLOD(new SLNode(columnL2->mesh(), strLOD + "-L1"), 0.01f, 3);
                        lod_group->addChildLOD(new SLNode(columnL3->mesh(), strLOD + "-L2"), 0.0001f, 3);
                        scene->addChild(lod_group);
                    }
                    x += offset;
                }
                z -= offset;
            }

            // Set active camera & the root pointer
            sv->camera(cam1);
            sv->doWaitOnIdle(false);
        }
    }
    else if (sceneID == SID_Benchmark7_JansUniverse) //............................................
    {
        s->name("Jan's Universe Test Scene");
        s->info(s->name());

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
        s->root3D(scene);
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
            /*
            SLGLProgram* spTex   = new SLGLProgramGeneric(am,
                                                        shaderPath + "PerPixCookTm.vert",
                                                        shaderPath + "PerPixCookTm.frag");*/
            SLstring matName = "mat-" + std::to_string(i);
            materials[i]     = new SLMaterial(am,
                                          matName.c_str(),
                                          nullptr,
                                          new SLGLTexture(am, texPath + "rusty-metal_2048_C.jpg"),
                                          nullptr, // new SLGLTexture(am, texPath + "rusty-metal_2048_N.jpg"),
                                          new SLGLTexture(am, texPath + "rusty-metal_2048_M.jpg"),
                                          new SLGLTexture(am, texPath + "rusty-metal_2048_R.jpg"),
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
            meshes[i]         = new SLSphere(am, 1.0f, 32, 32, meshName.c_str(), materials[i % NUM_MAT_MESH]);
        }

        // Create universe
        generateUniverse(am, s, scene, 0, levels, childCount, materials, meshes);

        sv->camera(cam1);
        sv->doWaitOnIdle(false);
    }
    else if (sceneID == SID_Benchmark8_ParticleSystemFireComplex) //...............................
    {
        s->name("Fire Complex Test Scene");
        s->info(s->name());

        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->clipNear(0.1f);
        cam1->clipFar(1000);
        cam1->translation(0, 10, 40);
        cam1->focalDist(100);
        cam1->lookAt(0, 0, 0);
        cam1->background().colors(SLCol4f(0.3f, 0.3f, 0.3f));
        cam1->setInitialState();

        // Root scene node
        SLNode* root = new SLNode;
        s->root3D(root);
        root->addChild(cam1);
        const int NUM_NODES = 250;

        // Create textures and materials
        SLGLTexture* texFireCld = new SLGLTexture(am, texPath + "ParticleFirecloudTransparent_C.png");
        SLGLTexture* texFireFlm = new SLGLTexture(am, texPath + "ParticleFlames_00_8x4_C.png");
        SLGLTexture* texCircle  = new SLGLTexture(am, texPath + "ParticleCircle_05_C.png");
        SLGLTexture* texSmokeB  = new SLGLTexture(am, texPath + "ParticleCloudBlack_C.png");
        SLGLTexture* texSmokeW  = new SLGLTexture(am, texPath + "ParticleCloudWhite_C.png");

        SLVNode nodes(NUM_NODES);
        for (int i = 0; i < NUM_NODES; ++i)
        {
            SLNode* fireComplex = createComplexFire(am,
                                                    s,
                                                    false,
                                                    texFireCld,
                                                    texFireFlm,
                                                    8,
                                                    4,
                                                    texCircle,
                                                    texSmokeB,
                                                    texSmokeW);
            fireComplex->translate(-20.0f + (float)(i % 20) * 2,
                                   0.0f,
                                   -(float)((i - (float)(i % 20)) / 20) * 4,
                                   TS_object);
            root->addChild(fireComplex);
        }

        sv->camera(cam1);
        sv->doWaitOnIdle(false);
    }
    else if (sceneID == SID_Benchmark9_ParticleSystemManyParticles) //.............................
    {
        s->name("Particle System number Scene");
        s->info(s->name());

        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->clipNear(0.1f);
        cam1->clipFar(1000);
        cam1->translation(0, 0, 400);
        cam1->focalDist(400);
        cam1->lookAt(0, 0, 0);
        cam1->background().colors(SLCol4f(0.3f, 0.3f, 0.3f));
        cam1->setInitialState();

        // Root scene node
        SLNode* root = new SLNode;
        root->addChild(cam1);

        // Create textures and materials
        SLGLTexture* texC        = new SLGLTexture(am, texPath + "ParticleSmoke_08_C.png");
        SLGLTexture* texFlipbook = new SLGLTexture(am, texPath + "ParticleSmoke_03_8x8_C.png");

        // Create meshes and nodes
        SLParticleSystem* ps = new SLParticleSystem(am,
                                                    1000000,
                                                    SLVec3f(-10.0f, -10.0f, -10.0f),
                                                    SLVec3f(10.0f, 10.0f, 10.0f),
                                                    4.0f,
                                                    texC,
                                                    "Particle System",
                                                    texFlipbook);
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
        s->root3D(root);
    }

    // These scenes assets are not publicly distributed
    else if (sceneID == SID_ErlebARBernChristoffel) //.............................................
    {
        s->name("Christoffel Tower AR");
        s->info("Augmented Reality Christoffel Tower");

        // Create video texture on global pointer updated in AppDemoVideo
        gVideoTexture = new SLGLTexture(am, texPath + "LiveVideoError.png", GL_LINEAR, GL_LINEAR);
        gVideoTexture->texType(TT_videoBkgd);

        // Create see through video background material without shadow mapping
        SLMaterial* matVideoBkgd = new SLMaterial(am, "matVideoBkgd", gVideoTexture);
        matVideoBkgd->reflectionModel(RM_Custom);

        // Create see through video background material with shadow mapping
        SLMaterial* matVideoBkgdSM = new SLMaterial(am, "matVideoBkgdSM", gVideoTexture);
        matVideoBkgdSM->reflectionModel(RM_Custom);
        matVideoBkgdSM->ambient(SLCol4f(0.6f, 0.6f, 0.6f));
        matVideoBkgdSM->getsShadows(true);

        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 2, 0);
        cam1->lookAt(-10, 2, 0);
        cam1->clipNear(1);
        cam1->clipFar(700);
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        cam1->background().texture(gVideoTexture);

        // Turn on main video
        CVCapture::instance()->videoType(VT_MAIN);

        // Create directional light for the sunlight
        SLLightDirect* sunLight = new SLLightDirect(am, s, 2.0f);
        sunLight->translate(-44.89f, 18.05f, -26.07f);
        sunLight->powers(1.0f, 1.5f, 1.0f);
        sunLight->attenuation(1, 0, 0);
        sunLight->doSunPowerAdaptation(true);
        sunLight->createsShadows(true);
        sunLight->createShadowMapAutoSize(cam1, SLVec2i(2048, 2048), 4);
        sunLight->shadowMap()->cascadesFactor(3.0);
        // sunLight->createShadowMap(-100, 150, SLVec2f(200, 150), SLVec2i(4096, 4096));
        sunLight->doSmoothShadows(true);
        sunLight->castsShadows(false);
        sunLight->shadowMinBias(0.001f);
        sunLight->shadowMaxBias(0.003f);
        AppDemo::devLoc.sunLightNode(sunLight); // Let the sun be rotated by time and location

        // Import the main model
        SLAssimpImporter importer;
        SLNode*          bern = importer.load(s->animManager(),
                                     am,
                                     dataPath + "erleb-AR/models/bern/bern-christoffel.gltf",
                                     texPath,
                                     nullptr,
                                     false,
                                     true,
                                     nullptr,
                                     0.3f); // ambient factor

        // Make city with hard edges and without shadow mapping
        SLNode* Umg = bern->findChild<SLNode>("Umgebung-Swisstopo");
        Umg->setMeshMat(matVideoBkgd, true);
        Umg->setDrawBitsRec(SL_DB_WITHEDGES, true);
        Umg->castsShadows(false);

        // Hide some objects
        bern->findChild<SLNode>("Baldachin-Glas")->drawBits()->set(SL_DB_HIDDEN, true);
        bern->findChild<SLNode>("Baldachin-Stahl")->drawBits()->set(SL_DB_HIDDEN, true);

        // Set the video background shader on the baldachin and the ground with shadow mapping
        bern->findChild<SLNode>("Baldachin-Stahl")->setMeshMat(matVideoBkgdSM, true);
        bern->findChild<SLNode>("Baldachin-Glas")->setMeshMat(matVideoBkgdSM, true);
        bern->findChild<SLNode>("Chr-Alt-Stadtboden")->setMeshMat(matVideoBkgdSM, true);
        bern->findChild<SLNode>("Chr-Neu-Stadtboden")->setMeshMat(matVideoBkgdSM, true);

        // Hide the new (last) version of the Christoffel tower
        bern->findChild<SLNode>("Chr-Neu")->drawBits()->set(SL_DB_HIDDEN, true);

        // Create textures and material for water
        SLGLTexture* cubemap = new SLGLTexture(am,
                                               dataPath + "erleb-AR/models/bern/Sea1+X1024.jpg",
                                               dataPath + "erleb-AR/models/bern/Sea1-X1024.jpg",
                                               dataPath + "erleb-AR/models/bern/Sea1+Y1024.jpg",
                                               dataPath + "erleb-AR/models/bern/Sea1-Y1024.jpg",
                                               dataPath + "erleb-AR/models/bern/Sea1+Z1024.jpg",
                                               dataPath + "erleb-AR/models/bern/Sea1-Z1024.jpg");
        // Material for water
        SLMaterial* matWater = new SLMaterial(am, "water", SLCol4f::BLACK, SLCol4f::BLACK, 100, 0.1f, 0.9f, 1.5f);
        matWater->translucency(1000);
        matWater->transmissive(SLCol4f::WHITE);
        matWater->addTexture(cubemap);
        matWater->program(new SLGLProgramGeneric(am,
                                                 shaderPath + "Reflect.vert",
                                                 shaderPath + "Reflect.frag"));
        bern->findChild<SLNode>("Chr-Wasser")->setMeshMat(matWater, true);

        // Add axis object a world origin (Loeb Ecke)
        SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
        axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
        axis->rotate(-90, 1, 0, 0);
        axis->castsShadows(false);

        // Bridge rotation animation
        SLNode*      bridge     = bern->findChild<SLNode>("Chr-Alt-Tor");
        SLAnimation* bridgeAnim = s->animManager().createNodeAnimation("Gate animation", 8.0f, true, EC_inOutQuint, AL_pingPongLoop);
        bridgeAnim->createNodeAnimTrackForRotation(bridge, 90, bridge->forwardOS());

        // Gate translation animation
        SLNode*      gate     = bern->findChild<SLNode>("Chr-Alt-Gatter");
        SLAnimation* gateAnim = s->animManager().createNodeAnimation("Gatter Animation", 8.0f, true, EC_inOutQuint, AL_pingPongLoop);
        gateAnim->createNodeAnimTrackForTranslation(gate, SLVec3f(0.0f, -3.6f, 0.0f));

        SLNode* scene = new SLNode("Scene");
        s->root3D(scene);
        scene->addChild(sunLight);
        scene->addChild(axis);
        scene->addChild(bern);
        scene->addChild(cam1);

        // initialize sensor stuff
        AppDemo::devLoc.originLatLonAlt(46.94763, 7.44074, 542.2);        // Loeb Ecken
        AppDemo::devLoc.defaultLatLonAlt(46.94841, 7.43970, 542.2 + 1.7); // Bahnhof Ausgang in Augenhöhe

        AppDemo::devLoc.nameLocations().push_back(SLLocation("Loeb Ecken, Origin", 46, 56, 51.451, 7, 26, 26.676, 542.2));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Milchgässli, Velomarkierung, (N)", 46, 56, 54.197, 7, 26, 23.366, 541.2 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Spitalgasse (E)", 46, 56, 51.703, 7, 26, 27.565, 542.1 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Tramhaltestelle UBS, eckiger Schachtd. (S)", 46, 56, 50.366, 7, 26, 24.544, 542.3 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Ecke Schauplatz-Christoffelgasse (S)", 46, 56, 50.139, 7, 26, 27.225, 542.1 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Bubenbergplatz (S)", 46, 56, 50.304, 7, 26, 22.113, 542.4 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Heiliggeistkirche (Dole, N-W)", 46, 56, 53.500, 7, 26, 25.499, 541.6 + 1.7));
        AppDemo::devLoc.originLatLonAlt(AppDemo::devLoc.nameLocations()[0].posWGS84LatLonAlt);
        AppDemo::devLoc.activeNamedLocation(1);   // This sets the location 1 as defaultENU
        AppDemo::devLoc.locMaxDistanceM(1000.0f); // Max. Distanz. zum Loeb Ecken
        AppDemo::devLoc.improveOrigin(false);     // Keine autom. Verbesserung vom Origin
        AppDemo::devLoc.useOriginAltitude(true);
        AppDemo::devLoc.hasOrigin(true);
        AppDemo::devLoc.offsetMode(LOM_twoFingerY);
        AppDemo::devRot.zeroYawAtStart(false);
        AppDemo::devRot.offsetMode(ROM_oneFingerX);

        // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
        SLstring tif = dataPath + "erleb-AR/models/bern/DEM-Bern-2600_1199-WGS84.tif";
        AppDemo::devLoc.loadGeoTiff(tif);

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
        AppDemo::devLoc.isUsed(true);
        AppDemo::devRot.isUsed(true);
        cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
        AppDemo::devLoc.isUsed(false);
        AppDemo::devRot.isUsed(false);
        SLVec3d pos_d = AppDemo::devLoc.defaultENU() - AppDemo::devLoc.originENU();
        SLVec3f pos_f((SLfloat)pos_d.x, (SLfloat)pos_d.y, (SLfloat)pos_d.z);
        cam1->translation(pos_f);
        cam1->focalDist(pos_f.length());
        cam1->lookAt(SLVec3f::ZERO);
        cam1->camAnim(SLCamAnim::CA_turntableYUp);
#endif

        sv->doWaitOnIdle(false); // for constant video feed
        sv->camera(cam1);
    }
    else if (sceneID == SID_ErlebARBielBFH) //.....................................................
    {
        s->name("Biel-BFH AR");
        s->info("Augmented Reality at Biel-BFH");

        // Create video texture on global pointer updated in AppDemoVideo
        gVideoTexture = new SLGLTexture(am, texPath + "LiveVideoError.png", GL_LINEAR, GL_LINEAR);

        // Define shader that shows on all pixels the video background
        SLGLProgram* spVideoBackground = new SLGLProgramGeneric(am,
                                                                shaderPath + "PerPixTmBackground.vert",
                                                                shaderPath + "PerPixTmBackground.frag");
        SLMaterial*  matVideoBkgd      = new SLMaterial(am,
                                                  "matVideoBkgd",
                                                  gVideoTexture,
                                                  nullptr,
                                                  nullptr,
                                                  nullptr,
                                                  spVideoBackground);

        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 2, 0);
        cam1->lookAt(-10, 2, 0);
        cam1->clipNear(1);
        cam1->clipFar(1000);
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        cam1->background().texture(gVideoTexture);

        // Turn on main video
        CVCapture::instance()->videoType(VT_MAIN);

        // Create directional light for the sunlight
        SLLightDirect* sunLight = new SLLightDirect(am, s, 5.0f);
        sunLight->powers(1.0f, 1.0f, 1.0f);
        sunLight->attenuation(1, 0, 0);
        sunLight->doSunPowerAdaptation(true);
        sunLight->createsShadows(true);
        sunLight->createShadowMap(-100, 150, SLVec2f(150, 150), SLVec2i(4096, 4096));
        sunLight->doSmoothShadows(true);
        sunLight->castsShadows(false);

        // Let the sun be rotated by time and location
        AppDemo::devLoc.sunLightNode(sunLight);

        SLAssimpImporter importer;
        SLNode*          bfh = importer.load(s->animManager(),
                                    am,
                                    dataPath + "erleb-AR/models/biel/Biel-BFH-Rolex.gltf",
                                    texPath);

        bfh->setMeshMat(matVideoBkgd, true);

        // Make terrain a video shine trough
        // bfh->findChild<SLNode>("Terrain")->setMeshMat(matVideoBkgd, true);

        /* Make buildings transparent
        SLNode* buildings = bfh->findChild<SLNode>("Buildings");
        SLNode* roofs = bfh->findChild<SLNode>("Roofs");
        auto updateTranspFnc = [](SLMaterial* m) {m->kt(0.5f);};
        buildings->updateMeshMat(updateTranspFnc, true);
        roofs->updateMeshMat(updateTranspFnc, true);

        // Set ambient on all child nodes
        bfh->updateMeshMat([](SLMaterial* m) { m->ambient(SLCol4f(.2f, .2f, .2f)); }, true);
        */

        // Add axis object a world origin
        SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
        axis->scale(2);
        axis->rotate(-90, 1, 0, 0);

        SLNode* scene = new SLNode("Scene");
        s->root3D(scene);
        scene->addChild(sunLight);
        scene->addChild(axis);
        scene->addChild(bfh);
        scene->addChild(cam1);

        // initialize sensor stuff
        AppDemo::devLoc.originLatLonAlt(47.14271, 7.24337, 488.2);        // Ecke Giosa
        AppDemo::devLoc.defaultLatLonAlt(47.14260, 7.24310, 488.7 + 1.7); // auf Parkplatz
        AppDemo::devLoc.locMaxDistanceM(1000.0f);
        AppDemo::devLoc.improveOrigin(false);
        AppDemo::devLoc.useOriginAltitude(true);
        AppDemo::devLoc.hasOrigin(true);
        AppDemo::devLoc.offsetMode(LOM_twoFingerY);
        AppDemo::devRot.zeroYawAtStart(false);
        AppDemo::devRot.offsetMode(ROM_oneFingerX);

        // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
        SLstring tif = dataPath + "erleb-AR/models/biel/DEM_Biel-BFH_WGS84.tif";
        AppDemo::devLoc.loadGeoTiff(tif);

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
        AppDemo::devLoc.isUsed(true);
        AppDemo::devRot.isUsed(true);
        cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
        AppDemo::devLoc.isUsed(false);
        AppDemo::devRot.isUsed(false);
        SLVec3d pos_d = AppDemo::devLoc.defaultENU() - AppDemo::devLoc.originENU();
        SLVec3f pos_f((SLfloat)pos_d.x, (SLfloat)pos_d.y, (SLfloat)pos_d.z);
        cam1->translation(pos_f);
        cam1->focalDist(pos_f.length());
        cam1->lookAt(SLVec3f::ZERO);
        cam1->camAnim(SLCamAnim::CA_turntableYUp);
#endif

        sv->doWaitOnIdle(false); // for constant video feed
        sv->camera(cam1);
        sv->drawBits()->on(SL_DB_ONLYEDGES);
    }
    else if (sceneID == SID_ErlebARAugustaRauricaTmp) //...........................................
    {
        s->name("Augusta Raurica Temple AR");
        s->info(s->name());

        // Create video texture on global pointer updated in AppDemoVideo
        gVideoTexture = new SLGLTexture(am, texPath + "LiveVideoError.png", GL_LINEAR, GL_LINEAR);
        gVideoTexture->texType(TT_videoBkgd);

        // Create see through video background material without shadow mapping
        SLMaterial* matVideoBkgd = new SLMaterial(am, "matVideoBkgd", gVideoTexture);
        matVideoBkgd->reflectionModel(RM_Custom);

        // Create see through video background material with shadow mapping
        SLMaterial* matVideoBkgdSM = new SLMaterial(am, "matVideoBkgdSM", gVideoTexture);
        matVideoBkgdSM->reflectionModel(RM_Custom);
        matVideoBkgdSM->ambient(SLCol4f(0.6f, 0.6f, 0.6f));
        matVideoBkgdSM->getsShadows(true);

        // Set the camera
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 50, -150);
        cam1->lookAt(0, 0, 0);
        cam1->clipNear(1);
        cam1->clipFar(400);
        cam1->focalDist(150);
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        cam1->background().texture(gVideoTexture);

        // Turn on main video
        CVCapture::instance()->videoType(VT_MAIN);

        string shdDir = shaderPath;
        string texDir = texPath;
        string datDir = dataPath + "erleb-AR/models/augst/";

        // Create directional light for the sunlight
        SLLightDirect* sunLight = new SLLightDirect(am, s, 1.0f);
        sunLight->translate(-42, 10, 13);
        sunLight->powers(1.0f, 1.5f, 1.0f);
        sunLight->attenuation(1, 0, 0);
        sunLight->doSunPowerAdaptation(true);
        sunLight->createsShadows(true);
        sunLight->createShadowMapAutoSize(cam1, SLVec2i(2048, 2048), 4);
        sunLight->shadowMap()->cascadesFactor(3.0);
        // sunLight->createShadowMap(-100, 250, SLVec2f(210, 180), SLVec2i(4096, 4096));
        sunLight->doSmoothShadows(true);
        sunLight->castsShadows(false);
        sunLight->shadowMinBias(0.001f);
        sunLight->shadowMaxBias(0.001f);
        AppDemo::devLoc.sunLightNode(sunLight); // Let the sun be rotated by time and location

        // Load main model
        SLAssimpImporter importer; //(LV_diagnostic);
        SLNode*          thtAndTmp = importer.load(s->animManager(),
                                          am,
                                          datDir + "augst-thtL2-tmpL1.gltf",
                                          texDir,
                                          nullptr,
                                          true,    // delete tex images after build
                                          true,    // only meshes
                                          nullptr, // no replacement material
                                          0.4f);   // 40% ambient reflection

        // Rotate to the true geographic rotation
        thtAndTmp->rotate(16.7f, 0, 1, 0, TS_parent);

        // Let the video shine through on some objects without shadow mapping
        SLNode* tmpUnderground = thtAndTmp->findChild<SLNode>("TmpUnderground");
        if (tmpUnderground) tmpUnderground->setMeshMat(matVideoBkgd, true);
        SLNode* thtUnderground = thtAndTmp->findChild<SLNode>("ThtUnderground");
        if (thtUnderground) thtUnderground->setMeshMat(matVideoBkgd, true);

        // Let the video shine through on some objects with shadow mapping
        SLNode* tmpFloor = thtAndTmp->findChild<SLNode>("TmpFloor");
        if (tmpFloor) tmpFloor->setMeshMat(matVideoBkgdSM, true);

        SLNode* terrain = thtAndTmp->findChild<SLNode>("Terrain");
        if (terrain)
        {
            terrain->setMeshMat(matVideoBkgdSM, true);
            terrain->castsShadows(false);
        }
        SLNode* thtFrontTerrain = thtAndTmp->findChild<SLNode>("ThtFrontTerrain");
        if (thtFrontTerrain)
        {
            thtFrontTerrain->setMeshMat(matVideoBkgdSM, true);
            thtFrontTerrain->castsShadows(false);
        }

        // Add axis object a world origin
        SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
        axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
        axis->rotate(-90, 1, 0, 0);
        axis->castsShadows(false);

        // Set some ambient light
        thtAndTmp->updateMeshMat([](SLMaterial* m) { m->ambient(SLCol4f(.25f, .25f, .25f)); },
                                 true);
        SLNode* scene = new SLNode("Scene");
        s->root3D(scene);
        scene->addChild(sunLight);
        scene->addChild(axis);
        scene->addChild(thtAndTmp);
        scene->addChild(cam1);

        // initialize sensor stuff
        AppDemo::devLoc.useOriginAltitude(false);
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Center of theatre, Origin", 47, 31, 59.461, 7, 43, 19.446, 282.6));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Treppe Tempel", 47, 31, 58.933, 7, 43, 16.799, 290.5 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Abzweigung (Dolendeckel)", 47, 31, 57.969, 7, 43, 17.946, 286.5 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Marker bei Tempel", 47, 31, 59.235, 7, 43, 15.161, 293.1 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Theater 1. Rang Zugang Ost", 47, 31, 59.698, 7, 43, 20.518, 291.0 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Theater 1. Rang Nord", 47, 32, 0.216, 7, 43, 19.173, 291.0 + 1.7));
        AppDemo::devLoc.originLatLonAlt(AppDemo::devLoc.nameLocations()[0].posWGS84LatLonAlt);
        AppDemo::devLoc.activeNamedLocation(1);   // This sets the location 1 as defaultENU
        AppDemo::devLoc.locMaxDistanceM(1000.0f); // Max. allowed distance to origin
        AppDemo::devLoc.improveOrigin(false);     // No autom. origin improvement
        AppDemo::devLoc.hasOrigin(true);
        AppDemo::devLoc.offsetMode(LOM_twoFingerY);
        AppDemo::devRot.zeroYawAtStart(false);
        AppDemo::devRot.offsetMode(ROM_oneFingerX);

        // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
        SLstring tif = datDir + "DTM-Theater-Tempel-WGS84.tif";
        AppDemo::devLoc.loadGeoTiff(tif);

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
        AppDemo::devLoc.isUsed(true);
        AppDemo::devRot.isUsed(true);
        cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
        AppDemo::devLoc.isUsed(false);
        AppDemo::devRot.isUsed(false);
        SLVec3d pos_d = AppDemo::devLoc.defaultENU() - AppDemo::devLoc.originENU();
        SLVec3f pos_f((SLfloat)pos_d.x, (SLfloat)pos_d.y, (SLfloat)pos_d.z);
        cam1->translation(pos_f);
        cam1->focalDist(pos_f.length());
        cam1->lookAt(SLVec3f::ZERO);
        cam1->camAnim(SLCamAnim::CA_turntableYUp);
#endif

        sv->doWaitOnIdle(false); // for constant video feed
        sv->camera(cam1);
    }
    else if (sceneID == SID_ErlebARAugustaRauricaTht) //...........................................
    {
        s->name("Augusta Raurica Theater AR");
        s->info(s->name());

        // Create video texture on global pointer updated in AppDemoVideo
        gVideoTexture = new SLGLTexture(am, texPath + "LiveVideoError.png", GL_LINEAR, GL_LINEAR);
        gVideoTexture->texType(TT_videoBkgd);

        // Create see through video background material without shadow mapping
        SLMaterial* matVideoBkgd = new SLMaterial(am, "matVideoBkgd", gVideoTexture);
        matVideoBkgd->reflectionModel(RM_Custom);

        // Create see through video background material with shadow mapping
        SLMaterial* matVideoBkgdSM = new SLMaterial(am, "matVideoBkgdSM", gVideoTexture);
        matVideoBkgdSM->reflectionModel(RM_Custom);
        matVideoBkgdSM->ambient(SLCol4f(0.6f, 0.6f, 0.6f));
        matVideoBkgdSM->getsShadows(true);

        // Setup the camera
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 50, -150);
        cam1->lookAt(0, 0, 0);
        cam1->clipNear(1);
        cam1->clipFar(400);
        cam1->focalDist(150);
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        cam1->background().texture(gVideoTexture);

        // Turn on main video
        CVCapture::instance()->videoType(VT_MAIN);

        string shdDir = shaderPath;
        string texDir = texPath;
        string datDir = dataPath + "erleb-AR/models/augst/";

        // Create directional light for the sunlight
        SLLightDirect* sunLight = new SLLightDirect(am, s, 1.0f);
        sunLight->translate(-42, 10, 13);
        sunLight->powers(1.0f, 1.5f, 1.0f);
        sunLight->attenuation(1, 0, 0);
        sunLight->doSunPowerAdaptation(true);
        sunLight->createsShadows(true);
        sunLight->createShadowMapAutoSize(cam1, SLVec2i(2048, 2048), 4);
        sunLight->shadowMap()->cascadesFactor(3.0);
        // sunLight->createShadowMap(-100, 250, SLVec2f(210, 180), SLVec2i(4096, 4096));
        sunLight->doSmoothShadows(true);
        sunLight->castsShadows(false);
        sunLight->shadowMinBias(0.001f);
        sunLight->shadowMaxBias(0.001f);
        AppDemo::devLoc.sunLightNode(sunLight); // Let the sun be rotated by time and location

        // Load main model
        SLAssimpImporter importer; //(LV_diagnostic);
        SLNode*          thtAndTmp = importer.load(s->animManager(),
                                          am,
                                          datDir + "augst-thtL1-tmpL2.gltf",
                                          texDir,
                                          nullptr,
                                          true,    // delete tex images after build
                                          true,    // only meshes
                                          nullptr, // no replacement material
                                          0.4f);   // 40% ambient reflection

        // Rotate to the true geographic rotation
        thtAndTmp->rotate(16.7f, 0, 1, 0, TS_parent);

        // Let the video shine through on some objects without shadow mapping
        SLNode* tmpUnderground = thtAndTmp->findChild<SLNode>("TmpUnderground");
        if (tmpUnderground) tmpUnderground->setMeshMat(matVideoBkgd, true);
        SLNode* thtUnderground = thtAndTmp->findChild<SLNode>("ThtUnderground");
        if (thtUnderground) thtUnderground->setMeshMat(matVideoBkgd, true);

        // Let the video shine through on some objects with shadow mapping
        SLNode* tmpFloor = thtAndTmp->findChild<SLNode>("TmpFloor");
        if (tmpFloor) tmpFloor->setMeshMat(matVideoBkgdSM, true);

        SLNode* terrain = thtAndTmp->findChild<SLNode>("Terrain");
        if (terrain)
        {
            terrain->setMeshMat(matVideoBkgdSM, true);
            terrain->castsShadows(false);
        }
        SLNode* thtFrontTerrain = thtAndTmp->findChild<SLNode>("ThtFrontTerrain");
        if (thtFrontTerrain)
        {
            thtFrontTerrain->setMeshMat(matVideoBkgdSM, true);
            thtFrontTerrain->castsShadows(false);
        }

        // Add axis object a world origin
        SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
        axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
        axis->rotate(-90, 1, 0, 0);
        axis->castsShadows(false);

        // Set some ambient light
        thtAndTmp->updateMeshMat([](SLMaterial* m) { m->ambient(SLCol4f(.25f, .25f, .25f)); },
                                 true);
        SLNode* scene = new SLNode("Scene");
        s->root3D(scene);
        scene->addChild(sunLight);
        scene->addChild(axis);
        scene->addChild(thtAndTmp);
        scene->addChild(cam1);

        // initialize sensor stuff
        AppDemo::devLoc.useOriginAltitude(false);
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Center of theatre, Origin", 47, 31, 59.461, 7, 43, 19.446, 282.6));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Treppe Tempel", 47, 31, 58.933, 7, 43, 16.799, 290.5 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Abzweigung (Dolendeckel)", 47, 31, 57.969, 7, 43, 17.946, 286.5 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Marker bei Tempel", 47, 31, 59.235, 7, 43, 15.161, 293.1 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Theater 1. Rang Zugang Ost", 47, 31, 59.698, 7, 43, 20.518, 291.0 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Theater 1. Rang Nord", 47, 32, 0.216, 7, 43, 19.173, 291.0 + 1.7));
        AppDemo::devLoc.originLatLonAlt(AppDemo::devLoc.nameLocations()[0].posWGS84LatLonAlt);
        AppDemo::devLoc.activeNamedLocation(1);   // This sets the location 1 as defaultENU
        AppDemo::devLoc.locMaxDistanceM(1000.0f); // Max. allowed distance to origin
        AppDemo::devLoc.improveOrigin(false);     // No autom. origin improvement
        AppDemo::devLoc.hasOrigin(true);
        AppDemo::devLoc.offsetMode(LOM_twoFingerY);
        AppDemo::devRot.zeroYawAtStart(false);
        AppDemo::devRot.offsetMode(ROM_oneFingerX);

        // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
        SLstring tif = datDir + "DTM-Theater-Tempel-WGS84.tif";
        AppDemo::devLoc.loadGeoTiff(tif);

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
        AppDemo::devLoc.isUsed(true);
        AppDemo::devRot.isUsed(true);
        cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
        AppDemo::devLoc.isUsed(false);
        AppDemo::devRot.isUsed(false);
        SLVec3d pos_d = AppDemo::devLoc.defaultENU() - AppDemo::devLoc.originENU();
        SLVec3f pos_f((SLfloat)pos_d.x, (SLfloat)pos_d.y, (SLfloat)pos_d.z);
        cam1->translation(pos_f);
        cam1->focalDist(pos_f.length());
        cam1->lookAt(SLVec3f::ZERO);
        cam1->camAnim(SLCamAnim::CA_turntableYUp);
#endif

        sv->doWaitOnIdle(false); // for constant video feed
        sv->camera(cam1);
    }
    else if (sceneID == SID_ErlebARAugustaRauricaTmpTht) //........................................
    {
        s->name("Augusta Raurica AR Temple and Theater");
        s->info(s->name());

        // Create video texture on global pointer updated in AppDemoVideo
        gVideoTexture = new SLGLTexture(am, texPath + "LiveVideoError.png", GL_LINEAR, GL_LINEAR);
        gVideoTexture->texType(TT_videoBkgd);

        // Create see through video background material without shadow mapping
        SLMaterial* matVideoBkgd = new SLMaterial(am, "matVideoBkgd", gVideoTexture);
        matVideoBkgd->reflectionModel(RM_Custom);

        // Create see through video background material with shadow mapping
        SLMaterial* matVideoBkgdSM = new SLMaterial(am, "matVideoBkgdSM", gVideoTexture);
        matVideoBkgdSM->reflectionModel(RM_Custom);
        matVideoBkgdSM->ambient(SLCol4f(0.6f, 0.6f, 0.6f));
        matVideoBkgdSM->getsShadows(true);

        // Setup the camera
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 50, -150);
        cam1->lookAt(0, 0, 0);
        cam1->clipNear(1);
        cam1->clipFar(400);
        cam1->focalDist(150);
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        cam1->background().texture(gVideoTexture);

        // Turn on main video
        CVCapture::instance()->videoType(VT_MAIN);

        string shdDir = shaderPath;
        string texDir = texPath;
        string datDir = dataPath + "erleb-AR/models/augst/";

        // Create directional light for the sunlight
        SLLightDirect* sunLight = new SLLightDirect(am, s, 1.0f);
        sunLight->translate(-42, 10, 13);
        sunLight->powers(1.0f, 1.5f, 1.0f);
        sunLight->attenuation(1, 0, 0);
        sunLight->doSunPowerAdaptation(true);
        sunLight->createsShadows(true);
        sunLight->createShadowMapAutoSize(cam1, SLVec2i(2048, 2048), 4);
        sunLight->shadowMap()->cascadesFactor(3.0);
        // Old stanard single map shadow map
        // sunLight->createShadowMap(-100, 250, SLVec2f(210, 180), SLVec2i(4096, 4096));
        sunLight->doSmoothShadows(true);
        sunLight->castsShadows(false);
        sunLight->shadowMinBias(0.001f);
        sunLight->shadowMaxBias(0.001f);
        AppDemo::devLoc.sunLightNode(sunLight); // Let the sun be rotated by time and location

        // Load main model
        SLAssimpImporter importer; //(LV_diagnostic);
        SLNode*          thtAndTmp = importer.load(s->animManager(),
                                          am,
                                          datDir + "augst-thtL1L2-tmpL1L2.gltf",
                                          texDir,
                                          nullptr,
                                          true,    // delete tex images after build
                                          true,    // only meshes
                                          nullptr, // no replacement material
                                          0.4f);   // 40% ambient reflection

        // Rotate to the true geographic rotation
        thtAndTmp->rotate(16.7f, 0, 1, 0, TS_parent);

        // Let the video shine through on some objects without shadow mapping
        SLNode* tmpUnderground = thtAndTmp->findChild<SLNode>("TmpUnderground");
        if (tmpUnderground) tmpUnderground->setMeshMat(matVideoBkgd, true);
        SLNode* thtUnderground = thtAndTmp->findChild<SLNode>("ThtUnderground");
        if (thtUnderground) thtUnderground->setMeshMat(matVideoBkgd, true);

        // Let the video shine through on some objects with shadow mapping
        SLNode* tmpFloor = thtAndTmp->findChild<SLNode>("TmpFloor");
        if (tmpFloor) tmpFloor->setMeshMat(matVideoBkgdSM, true);

        SLNode* terrain = thtAndTmp->findChild<SLNode>("Terrain");
        if (terrain)
        {
            terrain->setMeshMat(matVideoBkgdSM, true);
            terrain->castsShadows(false);
        }
        SLNode* thtFrontTerrain = thtAndTmp->findChild<SLNode>("ThtFrontTerrain");
        if (thtFrontTerrain)
        {
            thtFrontTerrain->setMeshMat(matVideoBkgdSM, true);
            thtFrontTerrain->castsShadows(false);
        }

        // Add axis object a world origin
        SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
        axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
        axis->rotate(-90, 1, 0, 0);
        axis->castsShadows(false);

        // Set some ambient light
        thtAndTmp->updateMeshMat([](SLMaterial* m) { m->ambient(SLCol4f(.25f, .25f, .25f)); },
                                 true);
        SLNode* scene = new SLNode("Scene");
        s->root3D(scene);
        scene->addChild(sunLight);
        scene->addChild(axis);
        scene->addChild(thtAndTmp);
        scene->addChild(cam1);

        // initialize sensor stuff
        AppDemo::devLoc.useOriginAltitude(false);
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Center of theatre, Origin", 47, 31, 59.461, 7, 43, 19.446, 282.6));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Treppe Tempel", 47, 31, 58.933, 7, 43, 16.799, 290.5 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Abzweigung (Dolendeckel)", 47, 31, 57.969, 7, 43, 17.946, 286.5 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Marker bei Tempel", 47, 31, 59.235, 7, 43, 15.161, 293.1 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Theater 1. Rang Zugang Ost", 47, 31, 59.698, 7, 43, 20.518, 291.0 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Theater 1. Rang Nord", 47, 32, 0.216, 7, 43, 19.173, 291.0 + 1.7));
        AppDemo::devLoc.originLatLonAlt(AppDemo::devLoc.nameLocations()[0].posWGS84LatLonAlt);
        AppDemo::devLoc.activeNamedLocation(1);   // This sets the location 1 as defaultENU
        AppDemo::devLoc.locMaxDistanceM(1000.0f); // Max. allowed distance to origin
        AppDemo::devLoc.improveOrigin(false);     // No autom. origin improvement
        AppDemo::devLoc.hasOrigin(true);
        AppDemo::devLoc.offsetMode(LOM_twoFingerY);
        AppDemo::devRot.zeroYawAtStart(false);
        AppDemo::devRot.offsetMode(ROM_oneFingerX);

        // Level of Detail switch for Temple and Theater
        SLNode* tmpAltar = thtAndTmp->findChild<SLNode>("TmpAltar");
        SLNode* tmpL1    = thtAndTmp->findChild<SLNode>("Tmp-L1");
        SLNode* tmpL2    = thtAndTmp->findChild<SLNode>("Tmp-L2");
        SLNode* thtL1    = thtAndTmp->findChild<SLNode>("Tht-L1");
        SLNode* thtL2    = thtAndTmp->findChild<SLNode>("Tht-L2");
        thtL1->drawBits()->set(SL_DB_HIDDEN, false);
        thtL2->drawBits()->set(SL_DB_HIDDEN, true);
        tmpL1->drawBits()->set(SL_DB_HIDDEN, false);
        tmpL2->drawBits()->set(SL_DB_HIDDEN, true);

        // Add level of detail switch callback lambda
        cam1->onCamUpdateCB([=](SLSceneView* sv) {
                                SLVec3f posCam     = sv->camera()->updateAndGetWM().translation();
                                SLVec3f posAlt     = tmpAltar->updateAndGetWM().translation();
                                SLVec3f distCamAlt = posCam - posAlt;
                                float   tmpDist    = distCamAlt.length();
                                float   thtDist    = posCam.length();

                                // If the temple is closer than the theater activate level 1 and deactivate level 2
                                if (tmpDist < thtDist)
                                {
                                    thtL1->drawBits()->set(SL_DB_HIDDEN, true);
                                    thtL2->drawBits()->set(SL_DB_HIDDEN, false);
                                    tmpL1->drawBits()->set(SL_DB_HIDDEN, false);
                                    tmpL2->drawBits()->set(SL_DB_HIDDEN, true);
                                }
                                else
                                {
                                    thtL1->drawBits()->set(SL_DB_HIDDEN, false);
                                    thtL2->drawBits()->set(SL_DB_HIDDEN, true);
                                    tmpL1->drawBits()->set(SL_DB_HIDDEN, true);
                                    tmpL2->drawBits()->set(SL_DB_HIDDEN, false);
                                } });

        // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
        SLstring tif = datDir + "DTM-Theater-Tempel-WGS84.tif";
        AppDemo::devLoc.loadGeoTiff(tif);

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
        AppDemo::devLoc.isUsed(true);
        AppDemo::devRot.isUsed(true);
        cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
        AppDemo::devLoc.isUsed(false);
        AppDemo::devRot.isUsed(false);
        SLVec3d pos_d = AppDemo::devLoc.defaultENU() - AppDemo::devLoc.originENU();
        SLVec3f pos_f((SLfloat)pos_d.x, (SLfloat)pos_d.y, (SLfloat)pos_d.z);
        cam1->translation(pos_f);
        cam1->focalDist(pos_f.length());
        cam1->lookAt(SLVec3f::ZERO);
        cam1->camAnim(SLCamAnim::CA_turntableYUp);
#endif

        sv->doWaitOnIdle(false); // for constant video feed
        sv->camera(cam1);
    }
    else if (sceneID == SID_ErlebARAventicumAmphiteatre) //........................................
    {
        s->name("Aventicum Amphitheatre AR (AO)");
        s->info("Augmented Reality for Aventicum Amphitheatre (AO)");

        // Create video texture on global pointer updated in AppDemoVideo
        gVideoTexture = new SLGLTexture(am, texPath + "LiveVideoError.png", GL_LINEAR, GL_LINEAR);
        gVideoTexture->texType(TT_videoBkgd);

        // Create see through video background material without shadow mapping
        SLMaterial* matVideoBkgd = new SLMaterial(am, "matVideoBkgd", gVideoTexture);
        matVideoBkgd->reflectionModel(RM_Custom);

        // Create see through video background material with shadow mapping
        SLMaterial* matVideoBkgdSM = new SLMaterial(am, "matVideoBkgdSM", gVideoTexture);
        matVideoBkgdSM->reflectionModel(RM_Custom);
        matVideoBkgdSM->ambient(SLCol4f(0.6f, 0.6f, 0.6f));
        matVideoBkgdSM->getsShadows(true);

        // Setup the camera
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 50, -150);
        cam1->lookAt(0, 0, 0);
        cam1->clipNear(1);
        cam1->clipFar(400);
        cam1->focalDist(150);
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        cam1->background().texture(gVideoTexture);

        // Turn on main video
        CVCapture::instance()->videoType(VT_MAIN);

        // Create directional light for the sunlight
        SLLightDirect* sunLight = new SLLightDirect(am, s, 1.0f);
        sunLight->powers(1.0f, 1.5f, 1.0f);
        sunLight->attenuation(1, 0, 0);
        sunLight->translation(0, 1, 0);
        sunLight->doSunPowerAdaptation(true);
        sunLight->createsShadows(true);
        sunLight->createShadowMapAutoSize(cam1, SLVec2i(2048, 2048), 4);
        sunLight->shadowMap()->cascadesFactor(3.0);
        // sunLight->createShadowMap(-70, 70, SLVec2f(140, 100), SLVec2i(4096, 4096));
        sunLight->doSmoothShadows(true);
        sunLight->castsShadows(false);
        sunLight->shadowMinBias(0.001f);
        sunLight->shadowMaxBias(0.003f);

        // Let the sun be rotated by time and location
        AppDemo::devLoc.sunLightNode(sunLight);

        SLAssimpImporter importer;
        SLNode*          amphiTheatre = importer.load(s->animManager(),
                                             am,
                                             dataPath + "erleb-AR/models/avenches/avenches-amphitheater.gltf",
                                             texPath,
                                             nullptr,
                                             false,   // delete tex images after build
                                             true,    // only meshes
                                             nullptr, // no replacement material
                                             0.4f);   // 40% ambient reflection

        // Rotate to the true geographic rotation
        amphiTheatre->rotate(13.25f, 0, 1, 0, TS_parent);

        // Let the video shine through some objects
        amphiTheatre->findChild<SLNode>("Tht-Aussen-Untergrund")->setMeshMat(matVideoBkgd, true);
        amphiTheatre->findChild<SLNode>("Tht-Eingang-Ost-Boden")->setMeshMat(matVideoBkgdSM, true);
        amphiTheatre->findChild<SLNode>("Tht-Arenaboden")->setMeshMat(matVideoBkgdSM, true);
        // amphiTheatre->findChild<SLNode>("Tht-Aussen-Terrain")->setMeshMat(matVideoBkgdSM, true);

        // Add axis object a world origin
        SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
        axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
        axis->rotate(-90, 1, 0, 0);
        axis->castsShadows(false);

        SLNode* scene = new SLNode("Scene");
        s->root3D(scene);
        scene->addChild(sunLight);
        scene->addChild(axis);
        scene->addChild(amphiTheatre);
        scene->addChild(cam1);

        // initialize sensor stuff
        AppDemo::devLoc.useOriginAltitude(false);
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Arena Centre, Origin", 46, 52, 51.685, 7, 2, 33.458, 461.4));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Entrance East, Manhole Cover", 46, 52, 52.344, 7, 2, 37.600, 461.4 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Arena, Sewer Cover West", 46, 52, 51.484, 7, 2, 32.307, 461.3 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Arena, Sewer Cover East", 46, 52, 51.870, 7, 2, 34.595, 461.1 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Stand South, Sewer Cover", 46, 52, 50.635, 7, 2, 34.099, 471.7 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Stand West, Sewer Cover", 46, 52, 51.889, 7, 2, 31.567, 471.7 + 1.7));
        AppDemo::devLoc.originLatLonAlt(AppDemo::devLoc.nameLocations()[0].posWGS84LatLonAlt);
        AppDemo::devLoc.activeNamedLocation(1);   // This sets the location 1 as defaultENU
        AppDemo::devLoc.locMaxDistanceM(1000.0f); // Max. Distanz. zum Nullpunkt
        AppDemo::devLoc.improveOrigin(false);     // Keine autom. Verbesserung vom Origin
        AppDemo::devLoc.hasOrigin(true);
        AppDemo::devLoc.offsetMode(LOM_twoFingerY);
        AppDemo::devRot.zeroYawAtStart(false);
        AppDemo::devRot.offsetMode(ROM_oneFingerX);

        // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
        SLstring tif = dataPath + "erleb-AR/models/avenches/DTM-Aventicum-WGS84.tif";
        AppDemo::devLoc.loadGeoTiff(tif);

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
        AppDemo::devLoc.isUsed(true);
        AppDemo::devRot.isUsed(true);
        cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
        AppDemo::devLoc.isUsed(false);
        AppDemo::devRot.isUsed(false);
        SLVec3d pos_d = AppDemo::devLoc.defaultENU() - AppDemo::devLoc.originENU();
        SLVec3f pos_f((SLfloat)pos_d.x, (SLfloat)pos_d.y, (SLfloat)pos_d.z);
        cam1->translation(pos_f);
        cam1->focalDist(pos_f.length());
        cam1->lookAt(SLVec3f::ZERO);
        cam1->camAnim(SLCamAnim::CA_turntableYUp);
#endif

        sv->doWaitOnIdle(false); // for constant video feed
        sv->camera(cam1);
    }
    else if (sceneID == SID_ErlebARAventicumCigognier) //..........................................
    {
        s->name("Aventicum Cigognier AR (AO)");
        s->info("Augmented Reality for Aventicum Cigognier Temple");

        // Create video texture on global pointer updated in AppDemoVideo
        gVideoTexture = new SLGLTexture(am, texPath + "LiveVideoError.png", GL_LINEAR, GL_LINEAR);
        gVideoTexture->texType(TT_videoBkgd);

        // Create see through video background material without shadow mapping
        SLMaterial* matVideoBkgd = new SLMaterial(am, "matVideoBkgd", gVideoTexture);
        matVideoBkgd->reflectionModel(RM_Custom);

        // Create see through video background material with shadow mapping
        SLMaterial* matVideoBkgdSM = new SLMaterial(am, "matVideoBkgdSM", gVideoTexture);
        matVideoBkgdSM->reflectionModel(RM_Custom);
        matVideoBkgdSM->ambient(SLCol4f(0.6f, 0.6f, 0.6f));
        matVideoBkgdSM->getsShadows(true);

        // Setup the camera
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 50, -150);
        cam1->lookAt(0, 0, 0);
        cam1->clipNear(1);
        cam1->clipFar(400);
        cam1->focalDist(150);
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        cam1->background().texture(gVideoTexture);

        // Turn on main video
        CVCapture::instance()->videoType(VT_MAIN);

        // Create directional light for the sunlight
        SLLightDirect* sunLight = new SLLightDirect(am, s, 1.0f);
        sunLight->powers(1.0f, 1.0f, 1.0f);
        sunLight->attenuation(1, 0, 0);
        sunLight->translation(0, 10, 0);
        sunLight->lookAt(10, 0, 10);
        sunLight->doSunPowerAdaptation(true);
        sunLight->createsShadows(true);
        sunLight->createShadowMapAutoSize(cam1, SLVec2i(2048, 2048), 4);
        sunLight->shadowMap()->cascadesFactor(3.0);
        // sunLight->createShadowMap(-70, 120, SLVec2f(150, 150), SLVec2i(2048, 2048));
        sunLight->doSmoothShadows(true);
        sunLight->castsShadows(false);
        sunLight->shadowMinBias(0.001f);
        sunLight->shadowMaxBias(0.003f);

        // Let the sun be rotated by time and location
        AppDemo::devLoc.sunLightNode(sunLight);

        SLAssimpImporter importer;
        SLNode*          cigognier = importer.load(s->animManager(),
                                          am,
                                          dataPath + "erleb-AR/models/avenches/avenches-cigognier.gltf",
                                          texPath,
                                          nullptr,
                                          true,    // delete tex images after build
                                          true,    // only meshes
                                          nullptr, // no replacement material
                                          0.4f);   // 40% ambient reflection

        // Rotate to the true geographic rotation
        cigognier->rotate(-36.52f, 0, 1, 0, TS_parent);

        // Let the video shine through some objects
        cigognier->findChild<SLNode>("Tmp-Sol-Pelouse")->setMeshMat(matVideoBkgdSM, true);
        cigognier->findChild<SLNode>("Tmp-Souterrain")->setMeshMat(matVideoBkgd, true);

        // Add axis object a world origin
        SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
        axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
        axis->rotate(-90, 1, 0, 0);
        axis->castsShadows(false);

        SLNode* scene = new SLNode("Scene");
        s->root3D(scene);
        scene->addChild(sunLight);
        scene->addChild(axis);
        scene->addChild(cigognier);
        scene->addChild(cam1);

        // initialize sensor stuff
        AppDemo::devLoc.useOriginAltitude(false);
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Center of place, Origin", 46, 52, 53.245, 7, 2, 47.198, 450.9));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("At the altar", 46, 52, 53.107, 7, 2, 47.498, 450.9 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Old AR viewer", 46, 52, 53.666, 7, 2, 48.316, 451.0 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Temple Entrance in hall", 46, 52, 54.007, 7, 2, 45.702, 453.0 + 1.7));
        AppDemo::devLoc.originLatLonAlt(AppDemo::devLoc.nameLocations()[0].posWGS84LatLonAlt);
        AppDemo::devLoc.activeNamedLocation(1);   // This sets the location 1 as defaultENU
        AppDemo::devLoc.locMaxDistanceM(1000.0f); // Max. allowed distance from origin
        AppDemo::devLoc.improveOrigin(false);     // No auto improvement from
        AppDemo::devLoc.hasOrigin(true);
        AppDemo::devLoc.offsetMode(LOM_twoFingerY);
        AppDemo::devRot.zeroYawAtStart(false);
        AppDemo::devRot.offsetMode(ROM_oneFingerX);

        // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
        SLstring tif = dataPath + "erleb-AR/models/avenches/DTM-Aventicum-WGS84.tif";
        AppDemo::devLoc.loadGeoTiff(tif);

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
        AppDemo::devLoc.isUsed(true);
        AppDemo::devRot.isUsed(true);
        cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
        AppDemo::devLoc.isUsed(false);
        AppDemo::devRot.isUsed(false);
        SLVec3d pos_d = AppDemo::devLoc.defaultENU() - AppDemo::devLoc.originENU();
        SLVec3f pos_f((SLfloat)pos_d.x, (SLfloat)pos_d.y, (SLfloat)pos_d.z);
        cam1->translation(pos_f);
        cam1->focalDist(pos_f.length());
        cam1->lookAt(0, cam1->translationWS().y, 0);
        cam1->camAnim(SLCamAnim::CA_turntableYUp);
#endif

        sv->doWaitOnIdle(false); // for constant video feed
        sv->camera(cam1);
    }
    else if (sceneID == SID_ErlebARAventicumTheatre) //............................................
    {
        s->name("Aventicum Theatre AR");
        s->info("Augmented Reality for Aventicum Theatre");

        // Create video texture on global pointer updated in AppDemoVideo
        gVideoTexture = new SLGLTexture(am, texPath + "LiveVideoError.png", GL_LINEAR, GL_LINEAR);
        gVideoTexture->texType(TT_videoBkgd);

        // Create see through video background material without shadow mapping
        SLMaterial* matVideoBkgd = new SLMaterial(am, "matVideoBkgd", gVideoTexture);
        matVideoBkgd->reflectionModel(RM_Custom);

        // Create see through video background material with shadow mapping
        SLMaterial* matVideoBkgdSM = new SLMaterial(am, "matVideoBkgdSM", gVideoTexture);
        matVideoBkgdSM->reflectionModel(RM_Custom);
        matVideoBkgdSM->ambient(SLCol4f(0.6f, 0.6f, 0.6f));
        matVideoBkgdSM->getsShadows(true);

        // Setup the camera
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 50, -150);
        cam1->lookAt(0, 0, 0);
        cam1->clipNear(1);
        cam1->clipFar(300);
        cam1->focalDist(150);
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        cam1->background().texture(gVideoTexture);

        // Turn on main video
        CVCapture::instance()->videoType(VT_MAIN);

        // Create directional light for the sunlight
        SLLightDirect* sunLight = new SLLightDirect(am, s, 1.0f);
        sunLight->powers(1.0f, 1.0f, 1.0f);
        sunLight->attenuation(1, 0, 0);
        sunLight->translation(0, 1, 0);

        sunLight->doSunPowerAdaptation(true);
        sunLight->createsShadows(true);
        sunLight->createShadowMapAutoSize(cam1, SLVec2i(2048, 2048), 4);
        sunLight->shadowMap()->cascadesFactor(3.0);
        // sunLight->createShadowMap(-80, 100, SLVec2f(130, 130), SLVec2i(4096, 4096));
        sunLight->doSmoothShadows(true);
        sunLight->castsShadows(false);
        sunLight->shadowMinBias(0.001f);
        sunLight->shadowMaxBias(0.001f);

        // Let the sun be rotated by time and location
        AppDemo::devLoc.sunLightNode(sunLight);

        SLAssimpImporter importer;
        SLNode*          theatre = importer.load(s->animManager(),
                                        am,
                                        dataPath + "erleb-AR/models/avenches/avenches-theater.gltf",
                                        texPath,
                                        nullptr,
                                        true,    // delete tex images after build
                                        true,    // only meshes
                                        nullptr, // no replacement material
                                        0.4f);   // 40% ambient reflection

        // Rotate to the true geographic rotation
        theatre->rotate(-36.7f, 0, 1, 0, TS_parent);

        // Let the video shine through some objects
        theatre->findChild<SLNode>("Tht-Rasen")->setMeshMat(matVideoBkgdSM, true);
        theatre->findChild<SLNode>("Tht-Untergrund")->setMeshMat(matVideoBkgd, true);
        theatre->findChild<SLNode>("Tht-Boden")->setMeshMat(matVideoBkgdSM, true);
        theatre->findChild<SLNode>("Tht-Boden")->setDrawBitsRec(SL_DB_WITHEDGES, true);

        // Add axis object a world origin
        SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
        axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
        axis->rotate(-90, 1, 0, 0);
        axis->castsShadows(false);

        SLNode* scene = new SLNode("Scene");
        s->root3D(scene);
        scene->addChild(sunLight);
        scene->addChild(axis);
        scene->addChild(theatre);
        scene->addChild(cam1);

        // initialize sensor stuff
        // https://map.geo.admin.ch/?lang=de&topic=ech&bgLayer=ch.swisstopo.swissimage&layers=ch.swisstopo.zeitreihen,ch.bfs.gebaeude_wohnungs_register,ch.bav.haltestellen-oev,ch.swisstopo.swisstlm3d-wanderwege&layers_opacity=1,1,1,0.8&layers_visibility=false,false,false,false&layers_timestamp=18641231,,,&E=2570281&N=1192204&zoom=13&crosshair=marker
        AppDemo::devLoc.useOriginAltitude(false);
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Center of theatre, Origin", 46, 52, 49.041, 7, 2, 55.543, 454.9));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("On the stage", 46, 52, 49.221, 7, 2, 55.206, 455.5 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("At the tree (N-E)", 46, 52, 50.791, 7, 2, 55.960, 455.5 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Over the entrance (S)", 46, 52, 48.162, 7, 2, 56.097, 464.0 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("At the 3rd tree (S-W)", 46, 52, 48.140, 7, 2, 51.506, 455.0 + 1.7));
        AppDemo::devLoc.originLatLonAlt(AppDemo::devLoc.nameLocations()[0].posWGS84LatLonAlt);
        AppDemo::devLoc.activeNamedLocation(1);   // This sets the location 1 as defaultENU
        AppDemo::devLoc.locMaxDistanceM(1000.0f); // Max. Distanz. zum Nullpunkt
        AppDemo::devLoc.improveOrigin(false);     // Keine autom. Verbesserung vom Origin
        AppDemo::devLoc.hasOrigin(true);
        AppDemo::devLoc.offsetMode(LOM_twoFingerY);
        AppDemo::devRot.zeroYawAtStart(false);
        AppDemo::devRot.offsetMode(ROM_oneFingerX);

        // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
        SLstring tif = dataPath + "erleb-AR/models/avenches/DTM-Aventicum-WGS84.tif";
        AppDemo::devLoc.loadGeoTiff(tif);

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
        AppDemo::devLoc.isUsed(true);
        AppDemo::devRot.isUsed(true);
        cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
        AppDemo::devLoc.isUsed(false);
        AppDemo::devRot.isUsed(false);
        SLVec3d pos_d = AppDemo::devLoc.defaultENU() - AppDemo::devLoc.originENU();
        SLVec3f pos_f((SLfloat)pos_d.x, (SLfloat)pos_d.y, (SLfloat)pos_d.z);
        cam1->translation(pos_f);
        cam1->focalDist(pos_f.length());
        cam1->lookAt(SLVec3f::ZERO);
        cam1->camAnim(SLCamAnim::CA_turntableYUp);
#endif

        sv->doWaitOnIdle(false); // for constant video feed
        sv->camera(cam1);
    }
    else if (sceneID == SID_ErlebARSutzKirchrain18) //.............................................
    {
        s->name("Sutz, Kirchrain 18");
        s->info("Augmented Reality for Sutz, Kirchrain 18");

        // Create video texture on global pointer updated in AppDemoVideo
        gVideoTexture = new SLGLTexture(am, texPath + "LiveVideoError.png", GL_LINEAR, GL_LINEAR);
        gVideoTexture->texType(TT_videoBkgd);

        // Create see through video background material without shadow mapping
        SLMaterial* matVideoBkgd = new SLMaterial(am, "matVideoBkgd", gVideoTexture);
        matVideoBkgd->reflectionModel(RM_Custom);

        // Create see through video background material with shadow mapping
        SLMaterial* matVideoBkgdSM = new SLMaterial(am, "matVideoBkgdSM", gVideoTexture);
        matVideoBkgdSM->reflectionModel(RM_Custom);
        matVideoBkgdSM->ambient(SLCol4f(0.6f, 0.6f, 0.6f));
        matVideoBkgdSM->getsShadows(true);

        // Create directional light for the sunlight
        SLLightDirect* sunLight = new SLLightDirect(am, s, 5.0f);
        sunLight->powers(1.0f, 1.0f, 1.0f);
        sunLight->attenuation(1, 0, 0);
        sunLight->translation(0, 10, 0);
        sunLight->lookAt(10, 0, 10);
        sunLight->doSunPowerAdaptation(true);
        sunLight->createsShadows(true);
        sunLight->createShadowMap(-100, 150, SLVec2f(150, 150), SLVec2i(4096, 4096));
        sunLight->doSmoothShadows(true);
        sunLight->castsShadows(false);

        // Setup the camera
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 50, -150);
        cam1->lookAt(0, 0, 0);
        cam1->clipNear(1);
        cam1->clipFar(300);
        cam1->focalDist(150);
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        cam1->background().texture(gVideoTexture);

        // Turn on main video
        CVCapture::instance()->videoType(VT_MAIN);

        // Let the sun be rotated by time and location
        AppDemo::devLoc.sunLightNode(sunLight);

        // Import main model
        SLAssimpImporter importer;
        SLNode*          sutzK18 = importer.load(s->animManager(),
                                        am,
                                        dataPath + "erleb-AR/models/sutz/Sutz-Kirchrain18.gltf",
                                        texPath,
                                        nullptr,
                                        true,    // delete tex images after build
                                        true,    // only meshes
                                        nullptr, // no replacement material
                                        0.4f);   // 40% ambient reflection

        // Rotate to the true geographic rotation
        // Nothing to do here because the model is north up

        // Let the video shine through some objects
        sutzK18->findChild<SLNode>("Terrain")->setMeshMat(matVideoBkgdSM, true);

        // Make buildings transparent with edges
        SLNode* buildings = sutzK18->findChild<SLNode>("Buildings");
        buildings->setMeshMat(matVideoBkgd, true);
        buildings->setDrawBitsRec(SL_DB_WITHEDGES, true);

        // Add axis object a world origin
        SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
        axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
        axis->rotate(-90, 1, 0, 0);
        axis->castsShadows(false);

        SLNode* scene = new SLNode("Scene");
        s->root3D(scene);
        scene->addChild(sunLight);
        scene->addChild(axis);
        scene->addChild(sutzK18);
        scene->addChild(cam1);

        // initialize sensor stuff
        // Go to https://map.geo.admin.ch and choose your origin and default point
        AppDemo::devLoc.useOriginAltitude(false);
        AppDemo::devLoc.originLatLonAlt(47.10600, 7.21772, 434.4f);        // Corner Carport
        AppDemo::devLoc.defaultLatLonAlt(47.10598, 7.21757, 433.9f + 1.7); // In the street

        AppDemo::devLoc.nameLocations().push_back(SLLocation("Corner Carport, Origin", 47, 6, 21.609, 7, 13, 3.788, 434.4));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Einfahrt (Dolendeckel)", 47, 6, 21.639, 7, 13, 2.764, 433.6 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Elektrokasten, Brunnenweg", 47, 6, 21.044, 7, 13, 4.920, 438.4 + 1.7));
        AppDemo::devLoc.nameLocations().push_back(SLLocation("Sitzbänkli am See", 47, 6, 24.537, 7, 13, 2.766, 431.2 + 1.7));
        AppDemo::devLoc.originLatLonAlt(AppDemo::devLoc.nameLocations()[0].posWGS84LatLonAlt);
        AppDemo::devLoc.activeNamedLocation(1);   // This sets the location 1 as defaultENU
        AppDemo::devLoc.locMaxDistanceM(1000.0f); // Max. Distanz. zum Nullpunkt
        AppDemo::devLoc.improveOrigin(false);     // Keine autom. Verbesserung vom Origin
        AppDemo::devLoc.hasOrigin(true);
        AppDemo::devLoc.offsetMode(LOM_twoFingerY);
        AppDemo::devRot.zeroYawAtStart(false);
        AppDemo::devRot.offsetMode(ROM_oneFingerX);

        // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
        SLstring tif = dataPath + "erleb-AR/models/sutz/Sutz-Kirchrain18-DEM-WGS84.tif";
        AppDemo::devLoc.loadGeoTiff(tif);

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
        AppDemo::devLoc.isUsed(true);
        AppDemo::devRot.isUsed(true);
        cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
        AppDemo::devLoc.isUsed(false);
        AppDemo::devRot.isUsed(false);
        SLVec3d pos_d = AppDemo::devLoc.defaultENU() - AppDemo::devLoc.originENU();
        SLVec3f pos_f((SLfloat)pos_d.x, (SLfloat)pos_d.y, (SLfloat)pos_d.z);
        cam1->translation(pos_f);
        cam1->focalDist(pos_f.length());
        cam1->lookAt(SLVec3f::ZERO);
        cam1->camAnim(SLCamAnim::CA_turntableYUp);
#endif

        sv->doWaitOnIdle(false); // for constant video feed
        sv->camera(cam1);
    }
    else if (sceneID == SID_RTMuttenzerBox) //.....................................................
    {
        s->name("Muttenzer Box");
        s->info("Muttenzer Box with environment mapped reflective sphere and transparent "
                "refractive glass sphere. Try ray tracing for real reflections and soft shadows.");

        // Create reflection & glass shaders
        SLGLProgram* sp1 = new SLGLProgramGeneric(am, shaderPath + "Reflect.vert", shaderPath + "Reflect.frag");
        SLGLProgram* sp2 = new SLGLProgramGeneric(am, shaderPath + "RefractReflect.vert", shaderPath + "RefractReflect.frag");

        // Create cube mapping texture
        SLGLTexture* tex1 = new SLGLTexture(am,
                                            texPath + "MuttenzerBox+X0512_C.png",
                                            texPath + "MuttenzerBox-X0512_C.png",
                                            texPath + "MuttenzerBox+Y0512_C.png",
                                            texPath + "MuttenzerBox-Y0512_C.png",
                                            texPath + "MuttenzerBox+Z0512_C.png",
                                            texPath + "MuttenzerBox-Z0512_C.png");

        SLCol4f lightEmisRGB(7.0f, 7.0f, 7.0f);
        SLCol4f grayRGB(0.75f, 0.75f, 0.75f);
        SLCol4f redRGB(0.75f, 0.25f, 0.25f);
        SLCol4f blueRGB(0.25f, 0.25f, 0.75f);
        SLCol4f blackRGB(0.00f, 0.00f, 0.00f);

        // create materials
        SLMaterial* cream = new SLMaterial(am, "cream", grayRGB, SLCol4f::BLACK, 0);
        SLMaterial* red   = new SLMaterial(am, "red", redRGB, SLCol4f::BLACK, 0);
        SLMaterial* blue  = new SLMaterial(am, "blue", blueRGB, SLCol4f::BLACK, 0);

        // Material for mirror sphere
        SLMaterial* refl = new SLMaterial(am, "refl", blackRGB, SLCol4f::WHITE, 1000, 1.0f);
        refl->addTexture(tex1);
        refl->program(sp1);

        // Material for glass sphere
        SLMaterial* refr = new SLMaterial(am, "refr", blackRGB, blackRGB, 100, 0.05f, 0.95f, 1.5f);
        refr->translucency(1000);
        refr->transmissive(SLCol4f::WHITE);
        refr->addTexture(tex1);
        refr->program(sp2);

        SLNode* sphere1 = new SLNode(new SLSphere(am, 0.5f, 32, 32, "Sphere1", refl));
        sphere1->translate(-0.65f, -0.75f, -0.55f, TS_object);

        SLNode* sphere2 = new SLNode(new SLSphere(am, 0.45f, 32, 32, "Sphere2", refr));
        sphere2->translate(0.73f, -0.8f, 0.10f, TS_object);

        SLNode* balls = new SLNode;
        balls->addChild(sphere1);
        balls->addChild(sphere2);

        // Rectangular light
        SLLightRect* lightRect = new SLLightRect(am, s, 1, 0.65f);
        lightRect->rotate(90, -1.0f, 0.0f, 0.0f);
        lightRect->translate(0.0f, -0.25f, 1.18f, TS_object);
        lightRect->spotCutOffDEG(90);
        lightRect->spotExponent(1.0);
        lightRect->ambientColor(SLCol4f::WHITE);
        lightRect->ambientPower(0.25f);
        lightRect->diffuseColor(lightEmisRGB);
        lightRect->attenuation(0, 0, 1);
        lightRect->samplesXY(11, 7);
        lightRect->createsShadows(true);
        lightRect->createShadowMap();

        SLLight::globalAmbient.set(lightEmisRGB * 0.01f);

        // create camera
        SLCamera* cam1 = new SLCamera();
        cam1->translation(0, 0, 7.2f);
        cam1->fov(27);
        cam1->focalDist(cam1->translationOS().length());
        cam1->background().colors(SLCol4f(0.0f, 0.0f, 0.0f));
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

        // assemble scene
        SLNode* scene = new SLNode;
        s->root3D(scene);
        scene->addChild(cam1);
        scene->addChild(lightRect);

        // create wall polygons
        SLfloat pL = -1.48f, pR = 1.48f; // left/right
        SLfloat pB = -1.25f, pT = 1.19f; // bottom/top
        SLfloat pN = 1.79f, pF = -1.55f; // near/far

        // bottom plane
        SLNode* b = new SLNode(new SLRectangle(am, SLVec2f(pL, -pN), SLVec2f(pR, -pF), 6, 6, "bottom", cream));
        b->rotate(90, -1, 0, 0);
        b->translate(0, 0, pB, TS_object);
        scene->addChild(b);

        // top plane
        SLNode* t = new SLNode(new SLRectangle(am, SLVec2f(pL, pF), SLVec2f(pR, pN), 6, 6, "top", cream));
        t->rotate(90, 1, 0, 0);
        t->translate(0, 0, -pT, TS_object);
        scene->addChild(t);

        // far plane
        SLNode* f = new SLNode(new SLRectangle(am, SLVec2f(pL, pB), SLVec2f(pR, pT), 6, 6, "far", cream));
        f->translate(0, 0, pF, TS_object);
        scene->addChild(f);

        // left plane
        SLNode* l = new SLNode(new SLRectangle(am, SLVec2f(-pN, pB), SLVec2f(-pF, pT), 6, 6, "left", red));
        l->rotate(90, 0, 1, 0);
        l->translate(0, 0, pL, TS_object);
        scene->addChild(l);

        // right plane
        SLNode* r = new SLNode(new SLRectangle(am, SLVec2f(pF, pB), SLVec2f(pN, pT), 6, 6, "right", blue));
        r->rotate(90, 0, -1, 0);
        r->translate(0, 0, -pR, TS_object);
        scene->addChild(r);

        scene->addChild(balls);

        sv->camera(cam1);
    }
    else if (sceneID == SID_RTSpheres) //..........................................................
    {
        s->name("Ray tracing Spheres");
        s->info("Classic ray tracing scene with transparent and reflective spheres. Be patient on mobile devices.");

        // define materials
        SLMaterial* matGla = new SLMaterial(am, "Glass", SLCol4f(0.0f, 0.0f, 0.0f), SLCol4f(0.5f, 0.5f, 0.5f), 100, 0.4f, 0.6f, 1.5f);
        SLMaterial* matRed = new SLMaterial(am, "Red", SLCol4f(0.5f, 0.0f, 0.0f), SLCol4f(0.5f, 0.5f, 0.5f), 100, 0.5f, 0.0f, 1.0f);
        SLMaterial* matYel = new SLMaterial(am, "Floor", SLCol4f(0.8f, 0.6f, 0.2f), SLCol4f(0.8f, 0.8f, 0.8f), 100, 0.5f, 0.0f, 1.0f);

        SLCamera* cam1 = new SLCamera();
        cam1->translation(0, 0.1f, 2.5f);
        cam1->lookAt(0, 0, 0);
        cam1->focalDist(cam1->translationOS().length());
        cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

        SLNode* rect = new SLNode(new SLRectangle(am, SLVec2f(-3, -3), SLVec2f(5, 4), 20, 20, "Floor", matYel));
        rect->rotate(90, -1, 0, 0);
        rect->translate(0, -1, -0.5f, TS_object);

        SLLightSpot* light1 = new SLLightSpot(am, s, 2, 2, 2, 0.1f);
        light1->powers(1, 7, 7);
        light1->attenuation(0, 0, 1);

        SLLightSpot* light2 = new SLLightSpot(am, s, 2, 2, -2, 0.1f);
        light2->powers(1, 7, 7);
        light2->attenuation(0, 0, 1);

        SLNode* scene = new SLNode;
        sv->camera(cam1);
        scene->addChild(light1);
        scene->addChild(light2);
        scene->addChild(SphereGroupRT(am, 3, 0, 0, 0, 1, 30, matGla, matRed));
        scene->addChild(rect);
        scene->addChild(cam1);

        s->root3D(scene);
    }
    else if (sceneID == SID_RTSoftShadows) //......................................................
    {
        s->name("Ray tracing soft shadows");
        s->info("Ray tracing with soft shadow light sampling. Each light source is sampled 64x per pixel. Be patient on mobile devices.");

        // Create root node
        SLNode* scene = new SLNode;
        s->root3D(scene);

        // define materials
        SLCol4f     spec(0.8f, 0.8f, 0.8f);
        SLMaterial* matBlk = new SLMaterial(am, "Glass", SLCol4f(0.0f, 0.0f, 0.0f), SLCol4f(0.5f, 0.5f, 0.5f), 100, 0.5f, 0.5f, 1.5f);
        SLMaterial* matRed = new SLMaterial(am, "Red", SLCol4f(0.5f, 0.0f, 0.0f), SLCol4f(0.5f, 0.5f, 0.5f), 100, 0.5f, 0.0f, 1.0f);
        SLMaterial* matYel = new SLMaterial(am, "Floor", SLCol4f(0.8f, 0.6f, 0.2f), SLCol4f(0.8f, 0.8f, 0.8f), 100, 0.0f, 0.0f, 1.0f);

        SLCamera* cam1 = new SLCamera;
        cam1->translation(0, 0.1f, 4);
        cam1->lookAt(0, 0, 0);
        cam1->focalDist(cam1->translationOS().length());
        cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        scene->addChild(cam1);

        SLNode* rect = new SLNode(new SLRectangle(am, SLVec2f(-5, -5), SLVec2f(5, 5), 1, 1, "Rect", matYel));
        rect->rotate(90, -1, 0, 0);
        rect->translate(0, 0, -0.5f);
        rect->castsShadows(false);
        scene->addChild(rect);

        SLLightSpot* light1 = new SLLightSpot(am, s, 2, 2, 2, 0.3f);
        light1->samples(8, 8);
        light1->attenuation(0, 0, 1);
        light1->createsShadows(true);
        light1->createShadowMap();
        scene->addChild(light1);

        SLLightSpot* light2 = new SLLightSpot(am, s, 2, 2, -2, 0.3f);
        light2->samples(8, 8);
        light2->attenuation(0, 0, 1);
        light2->createsShadows(true);
        light2->createShadowMap();
        scene->addChild(light2);

        scene->addChild(SphereGroupRT(am, 1, 0, 0, 0, 1, 32, matBlk, matRed));

        sv->camera(cam1);
    }
    else if (sceneID == SID_RTDoF) //..............................................................
    {
        s->name("Ray tracing depth of field");

        // Create root node
        SLNode* scene = new SLNode;
        s->root3D(scene);

        // Create textures and materials
        SLGLTexture* texC = new SLGLTexture(am, texPath + "Checkerboard0512_C.png", SL_ANISOTROPY_MAX, GL_LINEAR);
        SLMaterial*  mT   = new SLMaterial(am, "mT", texC);
        mT->kr(0.5f);
        SLMaterial* mW = new SLMaterial(am, "mW", SLCol4f::WHITE);
        SLMaterial* mB = new SLMaterial(am, "mB", SLCol4f::GRAY);
        SLMaterial* mY = new SLMaterial(am, "mY", SLCol4f::YELLOW);
        SLMaterial* mR = new SLMaterial(am, "mR", SLCol4f::RED);
        SLMaterial* mG = new SLMaterial(am, "mG", SLCol4f::GREEN);
        SLMaterial* mM = new SLMaterial(am, "mM", SLCol4f::MAGENTA);

#ifndef SL_GLES
        SLuint numSamples = 10;
#else
        SLuint numSamples = 4;
#endif

        stringstream ss;
        ss << "Ray tracing with depth of field blur. Each pixel is sampled " << numSamples * numSamples << "x from a lens. Be patient on mobile devices.";
        s->info(ss.str());

        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 2, 7);
        cam1->lookAt(0, 0, 0);
        cam1->focalDist(cam1->translationOS().length());
        cam1->clipFar(80);
        cam1->lensDiameter(0.4f);
        cam1->lensSamples()->samples(numSamples, numSamples);
        cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
        cam1->setInitialState();
        cam1->fogIsOn(true);
        cam1->fogMode(FM_exp);
        cam1->fogDensity(0.04f);
        scene->addChild(cam1);

        SLuint  res  = 36;
        SLNode* rect = new SLNode(new SLRectangle(am,
                                                  SLVec2f(-40, -10),
                                                  SLVec2f(40, 70),
                                                  SLVec2f(0, 0),
                                                  SLVec2f(4, 4),
                                                  2,
                                                  2,
                                                  "Rect",
                                                  mT));
        rect->rotate(90, -1, 0, 0);
        rect->translate(0, 0, -0.5f, TS_object);
        scene->addChild(rect);

        SLLightSpot* light1 = new SLLightSpot(am, s, 2, 2, 0, 0.1f);
        light1->ambiDiffPowers(0.1f, 1);
        light1->attenuation(1, 0, 0);
        scene->addChild(light1);

        SLNode* balls = new SLNode;
        SLNode* sp;
        sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S1", mW));
        sp->translate(2.0, 0, -4, TS_object);
        balls->addChild(sp);
        sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S2", mB));
        sp->translate(1.5, 0, -3, TS_object);
        balls->addChild(sp);
        sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S3", mY));
        sp->translate(1.0, 0, -2, TS_object);
        balls->addChild(sp);
        sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S4", mR));
        sp->translate(0.5, 0, -1, TS_object);
        balls->addChild(sp);
        sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S5", mG));
        sp->translate(0.0, 0, 0, TS_object);
        balls->addChild(sp);
        sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S6", mM));
        sp->translate(-0.5, 0, 1, TS_object);
        balls->addChild(sp);
        sp = new SLNode(new SLSphere(am, 0.5f, res, res, "S7", mW));
        sp->translate(-1.0, 0, 2, TS_object);
        balls->addChild(sp);
        scene->addChild(balls);

        sv->camera(cam1);
    }
    else if (sceneID == SID_RTLens) //.............................................................
    {
        s->name("Ray tracing lens test");
        s->info("Ray tracing lens test scene.");

        // Create root node
        SLNode* scene = new SLNode;
        s->root3D(scene);

        // Create textures and materials
        SLGLTexture* texC = new SLGLTexture(am, texPath + "VisionExample.jpg");
        // SLGLTexture* texC = new SLGLTexture(am, texPath + "Checkerboard0512_C.png");

        SLMaterial* mT = new SLMaterial(am, "mT", texC, nullptr, nullptr, nullptr);
        mT->kr(0.5f);

        // Glass material
        // name, ambient, specular,	shininess, kr(reflectivity), kt(transparency), kn(refraction)
        SLMaterial* matLens = new SLMaterial(am, "lens", SLCol4f(0.0f, 0.0f, 0.0f), SLCol4f(0.5f, 0.5f, 0.5f), 100, 0.5f, 0.5f, 1.5f);
        // SLGLShaderProg* sp1 = new SLGLShaderProgGeneric("RefractReflect.vert", "RefractReflect.frag");
        // matLens->shaderProg(sp1);

#ifndef APP_USES_GLES
        SLuint numSamples = 10;
#else
        SLuint numSamples = 6;
#endif

        // Scene
        SLCamera* cam1 = new SLCamera;
        cam1->translation(0, 8, 0);
        cam1->lookAt(0, 0, 0);
        cam1->focalDist(cam1->translationOS().length());
        cam1->lensDiameter(0.4f);
        cam1->lensSamples()->samples(numSamples, numSamples);
        cam1->background().colors(SLCol4f(0.1f, 0.4f, 0.8f));
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
        scene->addChild(cam1);

        // Plane
        // SLNode* rect = new SLNode(new SLRectangle(SLVec2f(-20, -20), SLVec2f(20, 20), 50, 20, "Rect", mT));
        // rect->translate(0, 0, 0, TS_Object);
        // rect->rotate(90, -1, 0, 0);

        SLLightSpot* light1 = new SLLightSpot(am, s, 1, 6, 1, 0.1f);
        light1->attenuation(0, 0, 1);
        scene->addChild(light1);

        SLuint  res  = 20;
        SLNode* rect = new SLNode(new SLRectangle(am, SLVec2f(-5, -5), SLVec2f(5, 5), res, res, "Rect", mT));
        rect->rotate(90, -1, 0, 0);
        rect->translate(0, 0, -0.0f, TS_object);
        scene->addChild(rect);

        // Lens from eye prescription card
        // SLNode* lensA = new SLNode(new SLLens(s, 0.50f, -0.50f, 4.0f, 0.0f, 32, 32, "presbyopic", matLens));   // Weitsichtig
        // lensA->translate(-2, 1, -2);
        // scene->addChild(lensA);

        // SLNode* lensB = new SLNode(new SLLens(s, -0.65f, -0.10f, 4.0f, 0.0f, 32, 32, "myopic", matLens));      // Kurzsichtig
        // lensB->translate(2, 1, -2);
        // scene->addChild(lensB);

        // Lens with radius
        // SLNode* lensC = new SLNode(new SLLens(s, 5.0, 4.0, 4.0f, 0.0f, 32, 32, "presbyopic", matLens));        // Weitsichtig
        // lensC->translate(-2, 1, 2);
        // scene->addChild(lensC);

        SLNode* lensD = new SLNode(new SLLens(am,
                                              -15.0f,
                                              -15.0f,
                                              1.0f,
                                              0.1f,
                                              res,
                                              res,
                                              "myopic",
                                              matLens)); // Kurzsichtig
        lensD->translate(0, 6, 0);
        scene->addChild(lensD);

        sv->camera(cam1);
    }
    else if (sceneID == SID_RTTest) //.............................................................
    {
        // Set scene name and info string
        s->name("Ray tracing test");
        s->info("RT Test Scene");

        // Create a camera node
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 0, 5);
        cam1->lookAt(0, 0, 0);
        cam1->background().colors(SLCol4f(0.5f, 0.5f, 0.5f));
        cam1->setInitialState();
        cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);

        // Create a light source node
        SLLightSpot* light1 = new SLLightSpot(am, s, 0.3f);
        light1->translation(5, 5, 5);
        light1->lookAt(0, 0, 0);
        light1->name("light node");

        // Material for glass sphere
        SLMaterial* matBox1  = new SLMaterial(am, "matBox1", SLCol4f(0.0f, 0.0f, 0.0f), SLCol4f(0.5f, 0.5f, 0.5f), 100, 0.0f, 0.9f, 1.5f);
        SLMesh*     boxMesh1 = new SLBox(am, -0.8f, -1, 0.02f, 1.2f, 1, 1, "boxMesh1", matBox1);
        SLNode*     boxNode1 = new SLNode(boxMesh1, "BoxNode1");

        SLMaterial* matBox2  = new SLMaterial(am, "matBox2", SLCol4f(0.0f, 0.0f, 0.0f), SLCol4f(0.5f, 0.5f, 0.5f), 100, 0.0f, 0.9f, 1.3f);
        SLMesh*     boxMesh2 = new SLBox(am, -1.2f, -1, -1, 0.8f, 1, -0.02f, "BoxMesh2", matBox2);
        SLNode*     boxNode2 = new SLNode(boxMesh2, "BoxNode2");

        // Create a scene group and add all nodes
        SLNode* scene = new SLNode("scene node");
        s->root3D(scene);
        scene->addChild(light1);
        scene->addChild(cam1);
        scene->addChild(boxNode1);
        scene->addChild(boxNode2);

        sv->camera(cam1);
    }

    /*
    // call onInitialize on all scene views to init the scenegraph and stats
    for (auto* sceneView : AppDemo::sceneViews)
        if (sceneView != nullptr)
            sceneView->onInitialize();

    if (CVCapture::instance()->videoType() != VT_NONE)
    {
        if (sv->viewportSameAsVideo())
        {
            // Pass a negative value to the start function, so that the
            // viewport aspect ratio can be adapted later to the video aspect.
            // This will be known after start.
            CVCapture::instance()->start(-1.0f);
            SLVec2i videoAspect;
            videoAspect.x = CVCapture::instance()->captureSize.width;
            videoAspect.y = CVCapture::instance()->captureSize.height;
            sv->setViewportFromRatio(videoAspect, sv->viewportAlign(), true);
        }
        else
            CVCapture::instance()->start(sv->viewportWdivH());
    }

    s->loadTimeMS(GlobalTimer::timeMS() - startLoadMS);
    */

#ifdef SL_USE_ENTITIES_DEBUG
    SLScene::entities.dump(true);
#endif
}
//-----------------------------------------------------------------------------
void appDemoSwitchScene(SLSceneView* sv, SLSceneID sceneID)
{
    AppScene*        s  = nullptr;
    SLAssetManager*& am = AppDemo::assetManager;
    SLAssetLoader*&  al = AppDemo::assetLoader;

    SLfloat startLoadMS = GlobalTimer::timeMS();

    //////////////////////
    // Delete old scene //
    //////////////////////

    if (AppDemo::scene)
    {
        delete AppDemo::scene;
        AppDemo::scene = nullptr;
    }

    // Reset non CVTracked and CVCapture infos
    CVTracked::resetTimes(); // delete all gVideoTracker times
    delete gVideoTracker;
    gVideoTracker = nullptr;

    // Reset asset pointer from previous scenes
    gVideoTexture = nullptr; // The video texture will be deleted by scene uninit
    gVideoTrackedNode  = nullptr; // The tracked node will be deleted by scene uninit

    if (sceneID != SID_VolumeRayCastLighted)
        gTexMRI3D = nullptr; // The 3D MRI texture will be deleted by scene uninit

    // reset existing sceneviews
    for (auto* sceneview : AppDemo::sceneViews)
        sceneview->unInit();

    // Clear all data in the asset manager
    am->clear();

    ////////////////////
    // Init new scene //
    ////////////////////

    switch (sceneID)
    {
        case SID_Empty: s = new AppDemoSceneEmpty(); break;
        case SID_Figure: s = new AppDemoSceneFigure(); break;
        case SID_Minimal: s = new AppDemoSceneMinimal(); break;
        case SID_MeshLoad: s = new AppDemoSceneMeshLoad(); break;
        case SID_Revolver: s = new AppDemoSceneRevolver(); break;
        case SID_TextureBlend: s = new AppDemoSceneTextureBlend(); break;
        case SID_TextureFilter: s = new AppDemoSceneTextureFilter(); break;
#ifdef SL_BUILD_WITH_KTX
        case SID_TextureCompression: s = new AppDemoSceneTextureCompression(); break;
#endif
        case SID_FrustumCull: s = new AppDemoSceneFrustum(); break;
        case SID_2Dand3DText: s = new AppDemoScene2Dand3DText(); break;
        case SID_PointClouds: s = new AppDemoScenePointClouds(); break;
        case SID_ZFighting: s = new AppDemoSceneZFighting(); break;
        case SID_ShaderPerVertexBlinn: s = new AppDemoSceneShaderBlinn("Per Vertex Blinn-Phong Lighting", true); break;
        case SID_ShaderPerPixelBlinn: s = new AppDemoSceneShaderBlinn("Per Pixel Blinn-Phong Lighting", false); break;
        case SID_ShaderPerPixelCook: s = new AppDemoSceneShaderCook(); break;
        case SID_ShaderIBL: s = new AppDemoSceneShaderIBL(); break;
        case SID_ShaderWave: s = new AppDemoSceneShaderWave(); break;
        case SID_ShaderBumpNormal: s = new AppDemoSceneShaderBump(); break;
        case SID_ShaderBumpParallax: s = new AppDemoSceneShaderParallax(); break;
        case SID_ShaderSkybox: s = new AppDemoSceneShaderSkybox(); break;
        case SID_ShaderEarth: s = new AppDemoSceneShaderEarth(); break;
        case SID_ShadowMappingBasicScene: s = new AppDemoSceneShadowBasic(); break;
        case SID_ShadowMappingLightTypes: s = new AppDemoSceneShadowLightTypes(); break;
        case SID_ShadowMappingSpotLights: s = new AppDemoSceneShadowLightSpot(); break;
        case SID_ShadowMappingPointLights: s = new AppDemoSceneShadowLightPoint(); break;
        case SID_ShadowMappingCascaded: s = new AppDemoSceneShadowCascaded(); break;
        case SID_SuzannePerPixBlinn: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting and reflection colors", false, false, false, false); break;
        case SID_SuzannePerPixBlinnTm: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting and texture mapping", true, false, false, false); break;
        case SID_SuzannePerPixBlinnNm: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting and normal mapping", false, true, false, false); break;
        case SID_SuzannePerPixBlinnAo: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting and ambient occlusion", false, false, true, false); break;
        case SID_SuzannePerPixBlinnSm: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting and shadow mapping", false, false, false, true); break;
        case SID_SuzannePerPixBlinnTmNm: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting, texture and normal mapping", true, true, false, false); break;
        case SID_SuzannePerPixBlinnTmAo: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting, texture mapping and ambient occlusion", true, false, true, false); break;
        case SID_SuzannePerPixBlinnNmAo: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting, normal mapping and ambient occlusion", false, true, true, false); break;
        case SID_SuzannePerPixBlinnTmSm: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting, texture mapping and shadow mapping", true, false, false, true); break;
        case SID_SuzannePerPixBlinnNmSm: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting, normal mapping and shadow mapping", false, true, false, true); break;
        case SID_SuzannePerPixBlinnAoSm: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting, ambient occlusion and shadow mapping", false, false, true, true); break;
        case SID_SuzannePerPixBlinnTmNmAo: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting and diffuse, normal, ambient occlusion and shadow mapping", true, true, true, false); break;
        case SID_SuzannePerPixBlinnTmNmSm: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting and diffuse, normal and shadow mapping ", true, true, false, true); break;
        case SID_SuzannePerPixBlinnTmNmAoSm: s = new AppDemoSceneSuzanne("Suzanne with per pixel lighting and diffuse, normal, ambient occlusion and shadow mapping", true, true, true, true); break;
        case SID_glTF_DamagedHelmet:
        case SID_glTF_FlightHelmet:
        case SID_glTF_Sponza:
        case SID_glTF_WaterBottle: s = new AppDemoSceneGLTF(sceneID); break;
        case SID_Robotics_FanucCRX_FK: s = new AppDemoSceneRobot(); break;
        case SID_VolumeRayCast: s = new AppDemoSceneVolumeRayCast(); break;
        case SID_VolumeRayCastLighted: s = new AppDemoSceneVolumeRayCastLighted(); break;
        case SID_AnimationNode: s = new AppDemoSceneAnimationNode(); break;
        case SID_AnimationNodeMass: s = new AppDemoSceneAnimationNodeMass(); break;
        case SID_AnimationSkinned: s = new AppDemoSceneAnimationSkinned(); break;
        case SID_AnimationSkinnedMass: s = new AppDemoSceneAnimationSkinnedMass(); break;
        case SID_VideoTextureFile:
        case SID_VideoTextureLive: s = new AppDemoSceneVideoTexture(sceneID); break;
        case SID_VideoTrackChessMain:
        case SID_VideoTrackChessScnd:
        case SID_VideoCalibrateMain:
        case SID_VideoCalibrateScnd: s = new AppDemoSceneVideoChessboard(sceneID); break;
        case SID_VideoTrackArucoMain:
        case SID_VideoTrackArucoScnd: s = new AppDemoSceneVideoTrackAruco(sceneID); break;
        case SID_VideoTrackFaceMain:
        case SID_VideoTrackFaceScnd: s = new AppDemoSceneVideoTrackFace(sceneID); break;
        default: s = new AppDemoSceneLegacy(sceneID); break;
    }

    sv->scene(s);

    CVCapture::instance()->videoType(VT_NONE); // turn off any video

    // Reset non CVTracked and CVCapture infos
    CVTracked::resetTimes(); // delete all gVideoTracker times
    delete gVideoTracker;
    gVideoTracker = nullptr;

    // Reset asset pointer from previous scenes
    gVideoTexture = nullptr; // The video texture will be deleted by scene uninit
    gVideoTrackedNode  = nullptr; // The tracked node will be deleted by scene uninit

    if (sceneID != SID_VolumeRayCastLighted)
        gTexMRI3D = nullptr; // The 3D MRI texture will be deleted by scene uninit

    // reset existing sceneviews
    for (auto* sceneview : AppDemo::sceneViews)
        sceneview->unInit();

    // Clear all data in the asset manager
    am->clear();

    AppDemo::sceneID = sceneID;

    // Initialize all preloaded stuff from SLScene
    s->init(am);

    // s->initOculus(AppDemo::dataPath + "shaders/");

    CVCapture::instance()->videoType(VT_NONE); // turn off any video

    // Clear the visible materials from the last scene
    sv->visibleMaterials2D().clear();
    sv->visibleMaterials3D().clear();

    // Clear gui stuff that depends on scene and sceneview
    AppDemoGui::clear();

    // Deactivate in general the device sensors
    AppDemo::devRot.init();
    AppDemo::devLoc.init();

    // Reset the global SLGLState state
    SLGLState::instance()->initAll();

    al->scene(s);
    sv->scene(s);

    auto onDoneLoading = [s, sv, startLoadMS] {
        s->assemble(am, sv);

        // Make sure the scene view has a camera
        if (!sv->camera())
            sv->camera(sv->sceneViewCamera());

        AppDemo::scene = s;

        // call onInitialize on all scene views to init the scenegraph and stats
        for (auto* sceneView : AppDemo::sceneViews)
            if (sceneView != nullptr)
                sceneView->onInitialize();

        if (CVCapture::instance()->videoType() != VT_NONE)
        {
            if (sv->viewportSameAsVideo())
            {
                // Pass a negative value to the start function, so that the
                // viewport aspect ratio can be adapted later to the video aspect.
                // This will be known after start.
                CVCapture::instance()->start(-1.0f);
                SLVec2i videoAspect;
                videoAspect.x = CVCapture::instance()->captureSize.width;
                videoAspect.y = CVCapture::instance()->captureSize.height;
                sv->setViewportFromRatio(videoAspect, sv->viewportAlign(), true);
            }
            else
                CVCapture::instance()->start(sv->viewportWdivH());
        }

        s->loadTimeMS(GlobalTimer::timeMS() - startLoadMS);
    };

    s->registerAssetsToLoad(*al);

    al->loadAssetsAsync(onDoneLoading);
}
//-----------------------------------------------------------------------------
