/**
 * \file      AppDemoSceneErlebARBernChristoffel.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see:
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include <AppDemoSceneErlebARBernChristoffel.h>
#include <CVCapture.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <SLLightDirect.h>
#include <SLCoordAxis.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;

//-----------------------------------------------------------------------------
AppDemoSceneErlebARBernChristoffel::AppDemoSceneErlebARBernChristoffel()
  : SLScene("Christoffel Tower AR")
{
    info("Augmented Reality Christoffel Tower");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneErlebARBernChristoffel::registerAssetsToLoad(SLAssetLoader& al)
{
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppCommon::texturePath + "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);
    al.addNodeToLoad(_bern,
                     AppCommon::dataPath +
                       "erleb-AR/models/bern/bern-christoffel.gltf");
    al.addTextureToLoad(_cubemap,
                        AppCommon::dataPath + "erleb-AR/models/bern/Sea1+X1024.jpg",
                        AppCommon::dataPath + "erleb-AR/models/bern/Sea1-X1024.jpg",
                        AppCommon::dataPath + "erleb-AR/models/bern/Sea1+Y1024.jpg",
                        AppCommon::dataPath + "erleb-AR/models/bern/Sea1-Y1024.jpg",
                        AppCommon::dataPath + "erleb-AR/models/bern/Sea1+Z1024.jpg",
                        AppCommon::dataPath + "erleb-AR/models/bern/Sea1-Z1024.jpg");
    al.addProgramToLoad(_spRefl,
                        AppCommon::shaderPath + "Reflect.vert",
                        AppCommon::shaderPath + "Reflect.frag");

    // initialize sensor stuff before loading the geotiff
    AppCommon::devLoc.originLatLonAlt(46.94763, 7.44074, 542.2);        // Loeb Ecken
    AppCommon::devLoc.defaultLatLonAlt(46.94841, 7.43970, 542.2 + 1.7); // Bahnhof Ausgang in Augenhöhe
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Loeb Ecken, Origin",
                                                           46,
                                                           56,
                                                           51.451,
                                                           7,
                                                           26,
                                                           26.676,
                                                           542.2));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Milchgässli, Velomarkierung, (N)",
                                                           46,
                                                           56,
                                                           54.197,
                                                           7,
                                                           26,
                                                           23.366,
                                                           541.2 + 1.7));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Spitalgasse (E)",
                                                           46,
                                                           56,
                                                           51.703,
                                                           7,
                                                           26,
                                                           27.565,
                                                           542.1 + 1.7));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Tramhaltestelle UBS, eckiger Schachtd. (S)",
                                                           46,
                                                           56,
                                                           50.366,
                                                           7,
                                                           26,
                                                           24.544,
                                                           542.3 + 1.7));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Ecke Schauplatz-Christoffelgasse (S)",
                                                           46,
                                                           56,
                                                           50.139,
                                                           7,
                                                           26,
                                                           27.225,
                                                           542.1 + 1.7));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Bubenbergplatz (S)",
                                                           46,
                                                           56,
                                                           50.304,
                                                           7,
                                                           26,
                                                           22.113,
                                                           542.4 + 1.7));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Heiliggeistkirche (Dole, N-W)",
                                                           46,
                                                           56,
                                                           53.500,
                                                           7,
                                                           26,
                                                           25.499,
                                                           541.6 + 1.7));
    AppCommon::devLoc.originLatLonAlt(AppCommon::devLoc.nameLocations()[0].posWGS84LatLonAlt);
    AppCommon::devLoc.activeNamedLocation(1);   // This sets the location 1 as defaultENU
    AppCommon::devLoc.locMaxDistanceM(1000.0f); // Max. Distanz. zum Loeb Ecken
    AppCommon::devLoc.improveOrigin(false);     // Keine autom. Verbesserung vom Origin
    AppCommon::devLoc.useOriginAltitude(true);
    AppCommon::devLoc.hasOrigin(true);
    AppCommon::devLoc.offsetMode(LOM_twoFingerY);
    AppCommon::devRot.zeroYawAtStart(false);
    AppCommon::devRot.offsetMode(ROM_oneFingerX);

    // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
    al.addGeoTiffToLoad(AppCommon::devLoc,
                        AppCommon::dataPath +
                          "erleb-AR/models/bern/DEM-Bern-2600_1199-WGS84.tif");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneErlebARBernChristoffel::assemble(SLAssetManager* am, SLSceneView* sv)
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

    SLCamera* cam1 = new SLCamera("Camera 1");
    cam1->translation(0, 2, 0);
    cam1->lookAt(-10, 2, 0);
    cam1->clipNear(1);
    cam1->clipFar(700);
    cam1->setInitialState();
    cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);
    cam1->background().texture(gVideoTexture);

    // Turn on main video
    CVCapture::instance()->videoType(VT_MAIN);

    // Create directional light for the sunlight
    SLLightDirect* sunLight = new SLLightDirect(am, this, 2.0f);
    sunLight->translate(-44.89f, 18.05f, -26.07f);
    sunLight->powers(1.0f, 1.5f, 1.0f);
    sunLight->attenuation(1, 0, 0);
    sunLight->doSunPowerAdaptation(true);
    sunLight->createsShadows(true);
    sunLight->createShadowMapAutoSize(cam1,
                                      SLVec2i(2048, 2048),
                                      4);
    sunLight->shadowMap()->cascadesFactor(3.0);
    // sunLight->createShadowMap(-100, 150, SLVec2f(200, 150), SLVec2i(4096, 4096));
    sunLight->doSmoothShadows(true);
    sunLight->castsShadows(false);
    sunLight->shadowMinBias(0.001f);
    sunLight->shadowMaxBias(0.003f);

    // Let the sun be rotated by time and location
    AppCommon::devLoc.sunLightNode(sunLight); 
    AppCommon::devLoc.calculateSolarAngles(AppCommon::devLoc.originLatLonAlt(),
                                           std::time(nullptr));

    // Make city with hard edges and without shadow mapping
    SLNode* Umg = _bern->findChild<SLNode>("Umgebung-Swisstopo");
    Umg->setMeshMat(matVideoBkgd, true);
    Umg->setDrawBitsRec(SL_DB_WITHEDGES, true);
    Umg->castsShadows(false);

    // Hide some objects
    _bern->findChild<SLNode>("Baldachin-Glas")->drawBits()->set(SL_DB_HIDDEN, true);
    _bern->findChild<SLNode>("Baldachin-Stahl")->drawBits()->set(SL_DB_HIDDEN, true);

    // Set the video background shader on the baldachin and the ground with shadow mapping
    _bern->findChild<SLNode>("Baldachin-Stahl")->setMeshMat(matVideoBkgdSM, true);
    _bern->findChild<SLNode>("Baldachin-Glas")->setMeshMat(matVideoBkgdSM, true);
    _bern->findChild<SLNode>("Chr-Alt-Stadtboden")->setMeshMat(matVideoBkgdSM, true);
    _bern->findChild<SLNode>("Chr-Neu-Stadtboden")->setMeshMat(matVideoBkgdSM, true);

    // Hide the new (last) version of the Christoffel tower
    _bern->findChild<SLNode>("Chr-Neu")->drawBits()->set(SL_DB_HIDDEN, true);

    // Material for water
    SLMaterial* matWater = new SLMaterial(am,
                                          "water",
                                          SLCol4f::BLACK,
                                          SLCol4f::BLACK,
                                          100,
                                          0.1f,
                                          0.9f,
                                          1.5f);
    matWater->translucency(1000);
    matWater->transmissive(SLCol4f::WHITE);
    matWater->addTexture(_cubemap);
    matWater->program(_spRefl);
    _bern->findChild<SLNode>("Chr-Wasser")->setMeshMat(matWater, true);

    // Add axis object a world origin (Loeb Ecke)
    SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
    axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
    axis->rotate(-90, 1, 0, 0);
    axis->castsShadows(false);

    // Bridge rotation animation
    SLNode*      bridge     = _bern->findChild<SLNode>("Chr-Alt-Tor");
    SLAnimation* bridgeAnim = animManager().createNodeAnimation("Gate animation",
                                                                8.0f,
                                                                true,
                                                                EC_inOutQuint,
                                                                AL_pingPongLoop);
    bridgeAnim->createNodeAnimTrackForRotation(bridge, 90, bridge->forwardOS());

    // Gate translation animation
    SLNode*      gate     = _bern->findChild<SLNode>("Chr-Alt-Gatter");
    SLAnimation* gateAnim = animManager().createNodeAnimation("Gatter Animation",
                                                              8.0f,
                                                              true,
                                                              EC_inOutQuint,
                                                              AL_pingPongLoop);
    gateAnim->createNodeAnimTrackForTranslation(gate, SLVec3f(0.0f, -3.6f, 0.0f));

    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(sunLight);
    scene->addChild(axis);
    scene->addChild(_bern);
    scene->addChild(cam1);

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
