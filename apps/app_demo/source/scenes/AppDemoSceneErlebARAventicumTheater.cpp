/**
 * \file      AppDemoSceneErlebARAventicumTheater.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneErlebARAventicumTheater.h>
#include <CVCapture.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <SLLightDirect.h>
#include <SLCoordAxis.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;

//-----------------------------------------------------------------------------
AppDemoSceneErlebARAventicumTheater::AppDemoSceneErlebARAventicumTheater()
  : SLScene("Aventicum Theatre AR")
{
    info("Aventicum Theatre AR");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneErlebARAventicumTheater::registerAssetsToLoad(SLAssetLoader& al)
{
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppCommon::texturePath + "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);

    al.addNodeToLoad(_theater,
                     AppCommon::dataPath +
                       "erleb-AR/models/avenches/avenches-theater.gltf");

    // initialize sensor stuff before loading the geotiff
    // https://map.geo.admin.ch/?lang=de&topic=ech&bgLayer=ch.swisstopo.swissimage&layers=ch.swisstopo.zeitreihen,ch.bfs.gebaeude_wohnungs_register,ch.bav.haltestellen-oev,ch.swisstopo.swisstlm3d-wanderwege&layers_opacity=1,1,1,0.8&layers_visibility=false,false,false,false&layers_timestamp=18641231,,,&E=2570281&N=1192204&zoom=13&crosshair=marker
    AppCommon::devLoc.useOriginAltitude(false);
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Center of theatre, Origin", 46, 52, 49.041, 7, 2, 55.543, 454.9));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("On the stage", 46, 52, 49.221, 7, 2, 55.206, 455.5 + 1.7));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("At the tree (N-E)", 46, 52, 50.791, 7, 2, 55.960, 455.5 + 1.7));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Over the entrance (S)", 46, 52, 48.162, 7, 2, 56.097, 464.0 + 1.7));
    AppCommon::devLoc.nameLocations().push_back(SLLocation("At the 3rd tree (S-W)", 46, 52, 48.140, 7, 2, 51.506, 455.0 + 1.7));
    AppCommon::devLoc.originLatLonAlt(AppCommon::devLoc.nameLocations()[0].posWGS84LatLonAlt);
    AppCommon::devLoc.activeNamedLocation(1);   // This sets the location 1 as defaultENU
    AppCommon::devLoc.locMaxDistanceM(1000.0f); // Max. Distanz. zum Nullpunkt
    AppCommon::devLoc.improveOrigin(false);     // Keine autom. Verbesserung vom Origin
    AppCommon::devLoc.hasOrigin(true);
    AppCommon::devLoc.offsetMode(LOM_twoFingerY);
    AppCommon::devRot.zeroYawAtStart(false);
    AppCommon::devRot.offsetMode(ROM_oneFingerX);

    // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
    al.addGeoTiffToLoad(AppCommon::devLoc,
                        AppCommon::dataPath +
                          "erleb-AR/models/avenches/DTM-Aventicum-WGS84.tif");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneErlebARAventicumTheater::assemble(SLAssetManager* am,
                                                   SLSceneView*    sv)
{
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
    cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);
    cam1->background().texture(gVideoTexture);

    // Turn on main video
    CVCapture::instance()->videoType(VT_MAIN);

    // Create directional light for the sunlight
    SLLightDirect* sunLight = new SLLightDirect(am, this, 1.0f);
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
    AppCommon::devLoc.sunLightNode(sunLight);
    AppCommon::devLoc.calculateSolarAngles(AppCommon::devLoc.originLatLonAlt(),
                                           std::time(nullptr));

    // Rotate to the true geographic rotation
    _theater->rotate(-36.7f, 0, 1, 0, TS_parent);

    // Let the video shine through some objects
    _theater->findChild<SLNode>("Tht-Rasen")->setMeshMat(matVideoBkgdSM, true);
    _theater->findChild<SLNode>("Tht-Untergrund")->setMeshMat(matVideoBkgd, true);
    _theater->findChild<SLNode>("Tht-Boden")->setMeshMat(matVideoBkgdSM, true);
    _theater->findChild<SLNode>("Tht-Boden")->setDrawBitsRec(SL_DB_WITHEDGES, true);

    // Add axis object a world origin
    SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
    axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
    axis->rotate(-90, 1, 0, 0);
    axis->castsShadows(false);

    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(sunLight);
    scene->addChild(axis);
    scene->addChild(_theater);
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
