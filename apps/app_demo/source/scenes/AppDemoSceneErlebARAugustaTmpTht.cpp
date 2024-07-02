/**
 * \file      AppDemoSceneErlebARAugustaTmpTht.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see:
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include <AppDemoSceneErlebARAugustaTmpTht.h>
#include <CVCapture.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <SLLightDirect.h>
#include <SLCoordAxis.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;

//-----------------------------------------------------------------------------
AppDemoSceneErlebARAugustaTmpTht::AppDemoSceneErlebARAugustaTmpTht()
  : SLScene("Augusta Raurica Temple and Theatre AR")
{
    info("Augusta Raurica Temple AR and Theatre");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneErlebARAugustaTmpTht::registerAssetsToLoad(SLAssetLoader& al)
{
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppCommon::texturePath + "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);

    al.addNodeToLoad(_thtAndTmp,
                     AppCommon::dataPath +
                       "erleb-AR/models/augst/augst-thtL1L2-tmpL1L2.gltf");

    al.addProgramToLoad(_spRefl,
                        AppCommon::shaderPath + "Reflect.vert",
                        AppCommon::shaderPath + "Reflect.frag");

    // initialize sensor stuff before loading geotiff
    AppCommon::devLoc.useOriginAltitude(false);
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Center of theatre, Origin",
                                                           47,
                                                           31,
                                                           59.461,
                                                           7,
                                                           43,
                                                           19.446,
                                                           282.6));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Treppe Tempel",
                                                           47,
                                                           31,
                                                           58.933,
                                                           7,
                                                           43,
                                                           16.799,
                                                           290.5 + 1.7));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Abzweigung (Dolendeckel)",
                                                           47,
                                                           31,
                                                           57.969,
                                                           7,
                                                           43,
                                                           17.946,
                                                           286.5 + 1.7));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Marker bei Tempel",
                                                           47,
                                                           31,
                                                           59.235,
                                                           7,
                                                           43,
                                                           15.161,
                                                           293.1 + 1.7));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Theater 1. Rang Zugang Ost",
                                                           47,
                                                           31,
                                                           59.698,
                                                           7,
                                                           43,
                                                           20.518,
                                                           291.0 + 1.7));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Theater 1. Rang Nord",
                                                           47,
                                                           32,
                                                           0.216,
                                                           7,
                                                           43,
                                                           19.173,
                                                           291.0 + 1.7));
    AppCommon::devLoc.originLatLonAlt(AppCommon::devLoc.nameLocations()[0].posWGS84LatLonAlt);
    AppCommon::devLoc.activeNamedLocation(1);   // This sets the location 1 as defaultENU
    AppCommon::devLoc.locMaxDistanceM(1000.0f); // Max. allowed distance to origin
    AppCommon::devLoc.improveOrigin(false);     // No autom. origin improvement
    AppCommon::devLoc.hasOrigin(true);
    AppCommon::devLoc.offsetMode(LOM_twoFingerY);
    AppCommon::devRot.zeroYawAtStart(false);
    AppCommon::devRot.offsetMode(ROM_oneFingerX);

    // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
    al.addGeoTiffToLoad(AppCommon::devLoc,
                        AppCommon::dataPath +
                          "erleb-AR/models/augst/DTM-Theater-Tempel-WGS84.tif");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneErlebARAugustaTmpTht::assemble(SLAssetManager* am,
                                                SLSceneView*    sv)
{
    gVideoTexture->texType(TT_videoBkgd);

    // Create see through video background material without shadow mapping
    SLMaterial* matVideoBkgd = new SLMaterial(am,
                                              "matVideoBkgd",
                                              gVideoTexture);
    matVideoBkgd->reflectionModel(RM_Custom);

    // Create see through video background material with shadow mapping
    SLMaterial* matVideoBkgdSM = new SLMaterial(am,
                                                "matVideoBkgdSM",
                                                gVideoTexture);
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
    cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);
    cam1->background().texture(gVideoTexture);

    // Turn on main video
    CVCapture::instance()->videoType(VT_MAIN);

    // Create directional light for the sunlight
    SLLightDirect* sunLight = new SLLightDirect(am, this, 1.0f);
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
    AppCommon::devLoc.sunLightNode(sunLight); // Let the sun be rotated by time and location

    // Rotate to the true geographic rotation
    _thtAndTmp->rotate(16.7f, 0, 1, 0, TS_parent);

    // Let the video shine through on some objects without shadow mapping
    SLNode* tmpUnderground = _thtAndTmp->findChild<SLNode>("TmpUnderground");
    if (tmpUnderground) tmpUnderground->setMeshMat(matVideoBkgd, true);
    SLNode* thtUnderground = _thtAndTmp->findChild<SLNode>("ThtUnderground");
    if (thtUnderground) thtUnderground->setMeshMat(matVideoBkgd, true);

    // Let the video shine through on some objects with shadow mapping
    SLNode* tmpFloor = _thtAndTmp->findChild<SLNode>("TmpFloor");
    if (tmpFloor) tmpFloor->setMeshMat(matVideoBkgdSM, true);

    SLNode* terrain = _thtAndTmp->findChild<SLNode>("Terrain");
    if (terrain)
    {
        terrain->setMeshMat(matVideoBkgdSM, true);
        terrain->castsShadows(false);
    }
    SLNode* thtFrontTerrain = _thtAndTmp->findChild<SLNode>("ThtFrontTerrain");
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
    _thtAndTmp->updateMeshMat([](SLMaterial* m)
                              { m->ambient(SLCol4f(.25f, .25f, .25f)); },
                              true);
    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(sunLight);
    scene->addChild(axis);
    scene->addChild(_thtAndTmp);
    scene->addChild(cam1);

    // Level of Detail switch for Temple and Theater
    SLNode* tmpAltar = _thtAndTmp->findChild<SLNode>("TmpAltar");
    SLNode* tmpL1    = _thtAndTmp->findChild<SLNode>("Tmp-L1");
    SLNode* tmpL2    = _thtAndTmp->findChild<SLNode>("Tmp-L2");
    SLNode* thtL1    = _thtAndTmp->findChild<SLNode>("Tht-L1");
    SLNode* thtL2    = _thtAndTmp->findChild<SLNode>("Tht-L2");
    thtL1->drawBits()->set(SL_DB_HIDDEN, false);
    thtL2->drawBits()->set(SL_DB_HIDDEN, true);
    tmpL1->drawBits()->set(SL_DB_HIDDEN, false);
    tmpL2->drawBits()->set(SL_DB_HIDDEN, true);

    // Add level of detail switch callback lambda
    cam1->onCamUpdateCB([=](SLSceneView* sv)
                        {
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

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
    AppCommon::devLoc.isUsed(true);
    AppCommon::devRot.isUsed(true);
    cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
    AppCommon::devLoc.isUsed(false);
    AppCommon::devRot.isUsed(false);
    SLVec3d pos_d = AppCommon::devLoc.defaultENU() - AppCommon::devLoc.originENU();
    SLVec3f pos_f((SLfloat)pos_d.x, (SLfloat)pos_d.y, (SLfloat)pos_d.z);
    cam1->translation(pos_f);
    cam1->focalDist(pos_f.length());
    cam1->lookAt(SLVec3f::ZERO);
    cam1->camAnim(SLCamAnim::CA_turntableYUp);
#endif

    sv->doWaitOnIdle(false); // for constant video feed
    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
