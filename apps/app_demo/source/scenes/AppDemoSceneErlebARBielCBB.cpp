/**
 * \file      AppDemoSceneErlebARBielCBB.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see:
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#include "SLVec3.h"
#include "AppDemoSceneErlebARBielCBB.h"
#include <CVCapture.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <SLLightDirect.h>
#include <SLCoordAxis.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;

//-----------------------------------------------------------------------------
AppDemoSceneErlebARBielCBB::AppDemoSceneErlebARBielCBB()
  : SLScene("Biel-CBB AR")
{
    info("Augmented Reality at Biel-CBB");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneErlebARBielCBB::registerAssetsToLoad(SLAssetLoader& al)
{
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppCommon::texturePath + "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);

    al.addNodeToLoad(_cbb,
                     AppCommon::dataPath +
                       "erleb-AR/models/biel/Biel-CBB-AR.gltf");

    al.addProgramToLoad(_spVideoBackground,
                        AppCommon::shaderPath + "PerPixTmBackground.vert",
                        AppCommon::shaderPath + "PerPixTmBackground.frag");

    // initialize sensor stuff before loading the geotiff
    // See also locationMapBiel-CBB.yml
    AppCommon::devLoc.originLatLonAlt(47.130569, 7.2412200, 432.6); // Origin CBB https://s.geo.admin.ch/hd6yp3mxe2sz
    AppCommon::devLoc.defaultLatLonAlt(47.130911, 7.241060, 433.0); // Ecke BFB: https://s.geo.admin.ch/q4sl4pv9ptil
    AppCommon::devLoc.nameLocations().push_back(SLLocation("CBB-Origin",
                                                           SLVec3d(47.130569, 7.2412200, 432.6))); // https://s.geo.admin.ch/hd6yp3mxe2sz
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Ecke BFB",
                                                           SLVec3d(47.130922, 7.241056, 433.1))); // https://s.geo.admin.ch/q4sl4pv9ptil
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Seite BFB",
                                                           SLVec3d(47.131012, 7.241140, 433.1))); // https://s.geo.admin.ch/cugzflafv1nj
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Baum BFB",
                                                           SLVec3d(47.130964, 7.241163, 433.1))); // https://s.geo.admin.ch/oer6fbzdjahr
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Aarbergstr.13",
                                                           SLVec3d(47.129964, 7.240038, 432.7))); // https://s.geo.admin.ch/2iqvhce7nv0a
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Ecke Dr. Schneiterstr.",
                                                           SLVec3d(47.129740, 7.240179, 432.7))); // https://s.geo.admin.ch/o0al1e7s1a74
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Ecke Schule für Gestaltung",
                                                           SLVec3d(47.129369, 7.242822, 432.1))); // https://s.geo.admin.ch/4n41dnm2jibi
    AppCommon::devLoc.nameLocations().push_back(SLLocation("SIPBB, 3. Stock, Project Space",
                                                           SLVec3d(47.130533, 7.240727, 433.1 + 15.8))); // https://s.geo.admin.ch/8rtmbyxy8hbs

    AppCommon::devLoc.originLatLonAlt(AppCommon::devLoc.nameLocations()[0].posWGS84LatLonAlt);
    AppCommon::devLoc.activeNamedLocation(1);   // This sets the location 1 as defaultENU
    AppCommon::devLoc.locMaxDistanceM(1000.0f); // Max. Distanz. zum Origin
    AppCommon::devLoc.improveOrigin(false);     // Keine autom. Verbesserung vom Origin
    AppCommon::devLoc.useOriginAltitude(true);
    AppCommon::devLoc.hasOrigin(true);
    AppCommon::devLoc.offsetMode(LOM_twoFingerY);
    AppCommon::devRot.zeroYawAtStart(false);
    AppCommon::devRot.offsetMode(ROM_oneFingerX);
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneErlebARBielCBB::assemble(SLAssetManager* am, SLSceneView* sv)
{
    gVideoTexture->texType(TT_videoBkgd);

    // Create see through video background material without shadow mapping
    SLMaterial* matVideoBkgd = new SLMaterial(am,
                                              "matVideoBkgd",
                                              gVideoTexture,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              _spVideoBackground);
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
    cam1->clipFar(1000);
    cam1->setInitialState();
    cam1->devRotLoc(&AppCommon::devRot, &AppCommon::devLoc);
    cam1->background().texture(gVideoTexture);

    // Turn on main video
    CVCapture::instance()->videoType(VT_MAIN);

    // Create directional light for the sunlight
    SLLightDirect* sunLight = new SLLightDirect(am, this, 5.0f);
    sunLight->powers(1.0f, 1.0f, 1.0f);
    sunLight->attenuation(1, 0, 0);
    sunLight->doSunPowerAdaptation(true);
    sunLight->createsShadows(true);
    sunLight->createShadowMap(-100,
                              150,
                              SLVec2f(150, 150),
                              SLVec2i(4096, 4096));
    sunLight->doSmoothShadows(true);
    sunLight->castsShadows(false);

    // Let the sun be rotated by time and location
    AppCommon::devLoc.sunLightNode(sunLight);
    AppCommon::devLoc.calculateSolarAngles(AppCommon::devLoc.originLatLonAlt(),
                                           std::time(nullptr));

    // Rotate to the true geographic rotation because the model is not ENU aligned
    // This angle comes from the swisstopo reference image in the blend file
    _cbb->rotate(-52.77f, 0, 1, 0, TS_parent);

    // Make city with hard edges and without shadow mapping
    SLNode* Umgebung = _cbb->findChild<SLNode>("Umgebung");
    Umgebung->setMeshMat(matVideoBkgdSM, true);
    Umgebung->setDrawBitsRec(SL_DB_WITHEDGES, true);
    Umgebung->castsShadows(false);

    // Set glas material transparency
    SLNode* fassade_glas = _cbb->findChild<SLNode>("Fassade_Glas");
    if (fassade_glas && fassade_glas->mesh() && fassade_glas->mesh()->mat())
        fassade_glas->mesh()->mat()->kt(0.9f);

    // Add axis object a world origin
    SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
    axis->scale(2);
    axis->rotate(-90, 1, 0, 0);

    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(sunLight);
    scene->addChild(axis);
    scene->addChild(_cbb);
    scene->addChild(cam1);

#if defined(SL_OS_MACIOS) || defined(SL_OS_ANDROID)
    AppCommon::devLoc.isUsed(true);
    AppCommon::devRot.isUsed(true);
    cam1->camAnim(SLCamAnim::CA_deviceRotLocYUp);
#else
    AppCommon::devLoc.isUsed(false);
    AppCommon::devRot.isUsed(false);
    SLVec3d pos_d = AppCommon::devLoc.defaultENU() - AppCommon::devLoc.originENU();
    SLVec3f pos_f((SLfloat)pos_d.x, (SLfloat)pos_d.y + 1.7f, (SLfloat)pos_d.z);
    cam1->translation(pos_f);
    cam1->focalDist(pos_f.length());
    cam1->lookAt(0, cam1->translationWS().y, 0);
    cam1->camAnim(SLCamAnim::CA_turntableYUp);
#endif

    sv->doWaitOnIdle(false); // for constant video feed
    sv->camera(cam1);
}
//-----------------------------------------------------------------------------
