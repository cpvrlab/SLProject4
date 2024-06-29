/**
 * \file      AppDemoSceneParticleComplexFire.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Marcus Hudritsch, Particle System from Marc Affolter
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#include <AppDemoSceneParticleComplexFire.h>
#include <AppDemo.h>
#include <SLAssetLoader.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <SLTexColorLUT.h>

//-----------------------------------------------------------------------------
AppDemoSceneParticleComplexFire::AppDemoSceneParticleComplexFire(SLSceneID sceneID)
  : SLScene("Complex Fire Particle System"),
    _sceneID(sceneID)
{
    info("The fire particle systems contain each multiple sub particle systems.\n"
         "See the scenegraph window for the sub particles systems. "
         "See the properties window for the settings of the particles systems");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneParticleComplexFire::registerAssetsToLoad(SLAssetLoader& al)
{
    al.addTextureToLoad(_texFireCld,
                        AppDemo::texturePath +
                          "ParticleFirecloudTransparent_C.png");
    al.addTextureToLoad(_texFireFlm,
                        AppDemo::texturePath +
                          "ParticleFlames_06_8x8_C.png");
    al.addTextureToLoad(_texCircle,
                        AppDemo::texturePath +
                          "ParticleCircle_05_C.png");
    al.addTextureToLoad(_texSmokeB,
                        AppDemo::texturePath +
                          "ParticleCloudBlack_C.png");
    al.addTextureToLoad(_texSmokeW,
                        AppDemo::texturePath +
                          "ParticleCloudWhite_C.png");
    al.addTextureToLoad(_texTorchFlm,
                        AppDemo::texturePath +
                          "ParticleFlames_04_16x4_C.png");
    al.addTextureToLoad(_texTorchSmk,
                        AppDemo::texturePath +
                          "ParticleSmoke_08_C.png");

    al.addTextureToLoad(_texWallDIF,
                        AppDemo::texturePath +
                          "BrickLimestoneGray_1K_DIF.jpg",
                        SL_ANISOTROPY_MAX,
                        GL_LINEAR);
    al.addTextureToLoad(_texWallNRM,
                        AppDemo::texturePath +
                          "BrickLimestoneGray_1K_NRM.jpg",
                        SL_ANISOTROPY_MAX,
                        GL_LINEAR);

    al.addNodeToLoad(_firewood,
                     AppDemo::modelPath +
                       "GLTF/Firewood/Firewood1.gltf",
                     nullptr,
                     false,
                     true,
                     nullptr,
                     0.3f,
                     true);
    al.addNodeToLoad(_torchL,
                     AppDemo::modelPath +
                       "GLTF/Torch/Torch.gltf",
                     nullptr,
                     false,
                     true,
                     nullptr,
                     0.3f,
                     true);
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneParticleComplexFire::assemble(SLAssetManager* am,
                                               SLSceneView*    sv)
{
    if (_sceneID == SID_ParticleSystem_ComplexFire)
    {
        // Create a scene group node
        SLNode* scene = new SLNode("Root Scene Node");
        root3D(scene);

        // Create and add camera
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->translation(0, 1.2f, 4.0f);
        cam1->lookAt(0, 1.2f, 0);
        cam1->focalDist(4.5f);
        cam1->setInitialState();
        scene->addChild(cam1);
        sv->camera(cam1);

        SLNode* complexFire = createComplexFire(am,
                                                this,
                                                true,
                                                _texTorchSmk,
                                                _texFireFlm,
                                                8,
                                                8,
                                                _texCircle,
                                                _texSmokeB,
                                                _texSmokeW);
        scene->addChild(complexFire);

        // Room parent node
        SLNode* room = new SLNode("Room");
        scene->addChild(room);

        // Back wall material
        SLMaterial* matWall = new SLMaterial(am,
                                             "mat3",
                                             _texWallDIF,
                                             _texWallNRM);
        matWall->specular(SLCol4f::BLACK);
        matWall->metalness(0);
        matWall->roughness(1);
        matWall->reflectionModel(RM_CookTorrance);

        // Room dimensions
        SLfloat pL = -2.0f, pR = 2.0f;  // left/right
        SLfloat pB = -0.01f, pT = 4.0f; // bottom/top
        SLfloat pN = 2.0f, pF = -2.0f;  // near/far

        // bottom rectangle
        SLNode* b = new SLNode(new SLRectangle(am,
                                               SLVec2f(pL, -pN),
                                               SLVec2f(pR, -pF),
                                               10,
                                               10,
                                               "Floor",
                                               matWall));
        b->rotate(90, -1, 0, 0);
        b->translate(0, 0, pB, TS_object);
        room->addChild(b);

        // far rectangle
        SLNode* f = new SLNode(new SLRectangle(am,
                                               SLVec2f(pL, pB),
                                               SLVec2f(pR, pT),
                                               10,
                                               10,
                                               "Wall far",
                                               matWall));
        f->translate(0, 0, pF, TS_object);
        room->addChild(f);

        // near rectangle
        SLNode* n = new SLNode(new SLRectangle(am,
                                               SLVec2f(pL, pB),
                                               SLVec2f(pR, pT),
                                               10,
                                               10,
                                               "Wall near",
                                               matWall));
        n->rotate(180, 0, 1, 0);
        n->translate(0, 0, pF, TS_object);
        room->addChild(n);

        // left rectangle
        SLNode* l = new SLNode(new SLRectangle(am,
                                               SLVec2f(-pN, pB),
                                               SLVec2f(-pF, pT),
                                               10,
                                               10,
                                               "Wall left",
                                               matWall));
        l->rotate(90, 0, 1, 0);
        l->translate(0, 0, pL, TS_object);
        room->addChild(l);

        // right rectangle
        SLNode* r = new SLNode(new SLRectangle(am,
                                               SLVec2f(pF, pB),
                                               SLVec2f(pN, pT),
                                               10,
                                               10,
                                               "Wall right",
                                               matWall));
        r->rotate(90, 0, -1, 0);
        r->translate(0, 0, -pR, TS_object);
        room->addChild(r);

        // Firewood
        _firewood->scale(2);
        scene->addChild(_firewood);

        // Torch
        _torchL->name("Torch Left");
        SLNode* torchR = _torchL->copyRec();
        torchR->name("Torch Right");
        _torchL->translate(-2, 1.5f, 0);
        _torchL->rotate(90, 0, 1, 0);
        _torchL->scale(2);
        scene->addChild(_torchL);
        torchR->translate(2, 1.5f, 0);
        torchR->rotate(-90, 0, 1, 0);
        torchR->scale(2);
        scene->addChild(torchR);

        // Torch flame left
        SLNode* torchFlameNodeL = createTorchFire(am,
                                                  this,
                                                  true,
                                                  _texTorchSmk,
                                                  _texTorchFlm,
                                                  16,
                                                  4);
        torchFlameNodeL->translate(-1.6f, 2.25f, 0);
        torchFlameNodeL->name("Torch Fire Left");
        scene->addChild(torchFlameNodeL);

        // Torch flame right
        SLNode* torchFlameNodeR = createTorchFire(am,
                                                  this,
                                                  true,
                                                  _texTorchSmk,
                                                  _texTorchFlm,
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
    else if (_sceneID == SID_Benchmark_ParticleSystemComplexFire)
    {
        SLCamera* cam1 = new SLCamera("Camera 1");
        cam1->clipNear(0.1f);
        cam1->clipFar(1000);
        cam1->translation(0, 10, 40);
        cam1->focalDist(100);
        cam1->lookAt(0, 0, 0);
        cam1->background().colors(SLCol4f(0.3f, 0.3f, 0.3f));
        cam1->setInitialState();

        // Root scene node
        SLNode* root = new SLNode("Root Scene Node");
        root3D(root);
        root->addChild(cam1);
        const int NUM_NODES = 250;

        SLVNode nodes(NUM_NODES);
        for (int i = 0; i < NUM_NODES; ++i)
        {
            SLNode* fireComplex = createComplexFire(am,
                                                    this,
                                                    false,
                                                    _texFireCld,
                                                    _texFireFlm,
                                                    8,
                                                    4,
                                                    _texCircle,
                                                    _texSmokeB,
                                                    _texSmokeW);
            fireComplex->translate(-20.0f + (float)(i % 20) * 2,
                                   0.0f,
                                   -(float)((i - (float)(i % 20)) / 20) * 4,
                                   TS_object);
            root->addChild(fireComplex);
        }

        sv->camera(cam1);
        sv->doWaitOnIdle(false);
    }
    else
        SL_EXIT_MSG("Assemble: Should not get here!");
}
//-----------------------------------------------------------------------------
//! Creates a complex fire group node
SLNode* AppDemoSceneParticleComplexFire::createComplexFire(SLAssetManager* am,
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
        SLLightSpot* light1 = new SLLightSpot(am,
                                              s,
                                              0.1f,
                                              180.0f,
                                              false);
        light1->name("Fire light node");
        light1->translate(0, 1.0f, 0);
        light1->diffuseColor(SLCol4f(1, 0.7f, 0.2f));
        light1->diffusePower(15);
        light1->attenuation(0, 0, 1);
        fireComplex->addChild(light1);
    }

    // Fire glow mesh
    {
        SLParticleSystem* glow = new SLParticleSystem(am,
                                                      24,
                                                      SLVec3f(-0.1f, 0.0f, -0.1f),
                                                      SLVec3f(0.1f, 0.0f, 0.1f),
                                                      4.0f,
                                                      texFireCld,
                                                      "Fire glow PS");
        glow->timeToLive(2.0f);
        glow->billboardType(BT_Camera);
        glow->radiusW(0.4f);
        glow->radiusH(0.4f);
        glow->doShape(false);
        glow->doRotation(true);
        glow->doRotRange(true);
        glow->doSizeOverLT(false);
        glow->doAlphaOverLT(false);
        glow->doColorOverLT(false);
        glow->doBlendBrightness(true);
        glow->color(SLCol4f(0.925f, 0.5f, 0.097f, 0.199f));
        glow->doAcceleration(false);
        SLNode* flameGlowNode = new SLNode(glow, "Fire glow node");
        flameGlowNode->translate(0, 0.15f, 0);
        fireComplex->addChild(flameGlowNode);
    }

    // Fire flame mesh
    {
        SLParticleSystem* flame = new SLParticleSystem(am,
                                                       1,
                                                       SLVec3f(0.0f, 0.0f, 0.0f),
                                                       SLVec3f(0.0f, 0.0f, 0.0f),

                                                       1.0f,
                                                       texFireCld,
                                                       "Fire flame PS",
                                                       texFireFlm);
        // Fire flame flipbook settings
        flame->flipbookColumns(flipbookCols);
        flame->flipbookRows(flipbookRows);
        flame->doFlipBookTexture(true);
        flame->doCounterGap(false); // We don't want to have flickering

        flame->doAlphaOverLT(false);
        flame->doSizeOverLT(false);
        flame->doRotation(false);

        flame->frameRateFB(64);
        flame->radiusW(0.6f);
        flame->radiusH(0.6f);
        flame->scale(1.2f);
        flame->billboardType(BT_Vertical);

        // Fire flame color
        flame->doColor(true);
        flame->color(SLCol4f(0.52f, 0.47f, 0.32f, 1.0f));
        flame->doBlendBrightness(true);

        // Fire flame size
        flame->doSizeOverLTCurve(true);
        float sizeCPArrayFl[4] = {0.0f, 1.25f, 0.0f, 1.0f};
        flame->bezierControlPointSize(sizeCPArrayFl);
        float sizeSEArrayFl[4] = {0.0f, 1.0f, 1.0f, 1.0f};
        flame->bezierStartEndPointSize(sizeSEArrayFl);
        flame->generateBernsteinPSize();

        // Fire flame node
        SLNode* fireFlameNode = new SLNode(flame, "Fire flame node");
        fireFlameNode->translate(0.0f, 0.7f, 0.0f, TS_object);
        fireComplex->addChild(fireFlameNode);
    }

    // Fire smoke black mesh
    {
        SLParticleSystem* smokeB = new SLParticleSystem(am,
                                                        8,
                                                        SLVec3f(0.0f, 1.0f, 0.0f),
                                                        SLVec3f(0.0f, 0.7f, 0.0f),
                                                        2.0f,
                                                        texSmokeB,
                                                        "Fire smoke black PS");
        smokeB->doColor(false);

        // Fire smoke black size
        smokeB->doSizeOverLT(true);
        smokeB->doSizeOverLTCurve(true);
        float sizeCPArraySB[4] = {0.0f, 1.0f, 1.0f, 2.0f};
        smokeB->bezierControlPointSize(sizeCPArraySB);
        float sizeSEArraySB[4] = {0.0f, 1.0f, 1.0f, 2.0f};
        smokeB->bezierStartEndPointSize(sizeSEArraySB);
        smokeB->generateBernsteinPSize();

        // Fire smoke black alpha
        smokeB->doAlphaOverLT(true);
        smokeB->doAlphaOverLTCurve(true);
        float alphaCPArraySB[4] = {0.0f, 0.4f, 1.0f, 0.4f};
        smokeB->bezierControlPointAlpha(alphaCPArraySB);
        float alphaSEArraySB[4] = {0.0f, 0.0f, 1.0f, 0.0f};
        smokeB->bezierStartEndPointAlpha(alphaSEArraySB);
        smokeB->generateBernsteinPAlpha();

        // Fire smoke black acceleration
        smokeB->doAcceleration(true);
        smokeB->doAccDiffDir(true);
        smokeB->acceleration(0.0f, 0.25f, 0.3f);

        SLNode* fireSmokeBlackNode = new SLNode(smokeB, "Fire smoke black node");
        fireSmokeBlackNode->translate(0.0f, 0.9f, 0.0f, TS_object);
        fireComplex->addChild(fireSmokeBlackNode);
    }

    // Fire smoke white mesh
    {
        SLParticleSystem* smokeW = new SLParticleSystem(am,
                                                        40,
                                                        SLVec3f(0.0f, 0.8f, 0.0f),
                                                        SLVec3f(0.0f, 0.6f, 0.0f),
                                                        4.0f,
                                                        texSmokeW,
                                                        "Fire smoke white PS");

        smokeW->doColor(false);

        // Size
        smokeW->doSizeOverLT(true);
        smokeW->doSizeOverLTCurve(true);
        float sizeCPArraySW[4] = {0.0f, 0.5f, 1.0f, 2.0f};
        smokeW->bezierControlPointSize(sizeCPArraySW);
        float sizeSEArraySW[4] = {0.0f, 0.5f, 1.0f, 2.0f};
        smokeW->bezierStartEndPointSize(sizeSEArraySW);
        smokeW->generateBernsteinPSize();

        // Alpha
        smokeW->doAlphaOverLT(true);
        smokeW->doAlphaOverLTCurve(true);
        float alphaCPArraySW[4] = {0.0f, 0.018f, 1.0f, 0.018f};
        smokeW->bezierControlPointAlpha(alphaCPArraySW);
        float alphaSEArraySW[4] = {0.0f, 0.0f, 1.0f, 0.0f};
        smokeW->bezierStartEndPointAlpha(alphaSEArraySW);
        smokeW->generateBernsteinPAlpha();

        // Acceleration
        smokeW->doAcceleration(true);
        smokeW->doAccDiffDir(true);
        smokeW->acceleration(0.0f, 0.25f, 0.3f);

        SLNode* fireSmokeWNode = new SLNode(smokeW, "Fire smoke white node");
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
                                                                  nullptr,
                                                                  new SLTexColorLUT(am,
                                                                                    CLUT_WYR,
                                                                                    256));
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
        fireSparksRising->doSizeOverLT(false);
        fireSparksRising->doAlphaOverLT(false);
        fireSparksRising->doGravity(false);
        fireComplex->addChild(new SLNode(fireSparksRising,
                                         "Fire sparks rising node"));
    }

    return fireComplex;
}
//-----------------------------------------------------------------------------
SLNode* AppDemoSceneParticleComplexFire::createTorchFire(SLAssetManager* am,
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
        SLLightSpot* light1 = new SLLightSpot(am,
                                              s,
                                              0.1f,
                                              180.0f,
                                              false);
        light1->name("Fire light node");
        light1->translate(0, 0, 0);
        light1->diffuseColor(SLCol4f(1, 0.4f, 0.0f));
        light1->diffusePower(2);
        light1->attenuation(0, 0, 1);
        torchFire->addChild(light1);
    }

    // Fire glow mesh
    {
        SLParticleSystem* glow = new SLParticleSystem(am,
                                                      40,
                                                      SLVec3f(-0.1f, 0.0f, -0.1f),
                                                      SLVec3f(0.1f, 0.0f, 0.1f),
                                                      1.5f,
                                                      texFireCld,
                                                      "Torch Glow PS");
        glow->color(SLCol4f(0.9f, 0.5f, 0, 0.63f));
        glow->doBlendBrightness(true);
        glow->radiusW(0.15f);
        glow->radiusH(0.15f);
        glow->doSizeOverLT(false);
        SLNode* fireGlowNode = new SLNode(glow, "Torch Glow Node");
        fireGlowNode->translate(0, -0.4f, 0);
        torchFire->addChild(fireGlowNode);
    }

    // Fire torches
    {
        SLParticleSystem* flame = new SLParticleSystem(am,
                                                       1,
                                                       SLVec3f(0.0f, 0.0f, 0.0f),
                                                       SLVec3f(0.0f, 0.0f, 0.0f),
                                                       4.0f,
                                                       texFireCld,
                                                       "Torch Flame PS",
                                                       texFireFlm);
        flame->flipbookColumns(flipbookCols);
        flame->flipbookRows(flipbookRows);
        flame->doFlipBookTexture(true);
        flame->doCounterGap(false); // We don't want to have flickering
        flame->doAlphaOverLT(false);
        flame->doSizeOverLT(false);
        flame->doRotation(false);
        flame->doColor(false);
        flame->frameRateFB(64);
        flame->radiusW(0.3f);
        flame->radiusH(0.8f);
        flame->billboardType(BT_Vertical);
        SLNode* torchFlameNode = new SLNode(flame, "Torch Flame Node");
        torchFlameNode->translate(0, 0.3f, 0);
        torchFire->addChild(torchFlameNode);
    }

    return torchFire;
}
//-----------------------------------------------------------------------------
