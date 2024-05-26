//#############################################################################
//  File:      AppDemoSceneErlebARSutz.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneErlebARSutz.h>
#include <CVCapture.h>
#include <AppDemo.h>
#include <SLAssetLoader.h>
#include <SLLightDirect.h>
#include <SLCoordAxis.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;

//-----------------------------------------------------------------------------
AppDemoSceneErlebARSutz::AppDemoSceneErlebARSutz()
  : SLScene("Sutz AR")
{
    info("Augmented Reality at Sutz");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneErlebARSutz::registerAssetsToLoad(SLAssetLoader& al)
{
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppDemo::texturePath + "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);

    al.addNodeToLoad(_sutz,
                     AppDemo::dataPath +
                       "erleb-AR/models/sutz/Sutz-Kirchrain18.gltf");

    al.addProgramToLoad(_spVideoBackground,
                        AppDemo::shaderPath + "PerPixTmBackground.vert",
                        AppDemo::shaderPath + "PerPixTmBackground.frag");

    // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
    al.addGeoTiffToLoad(AppDemo::devLoc,
                        AppDemo::dataPath +
                          "erleb-AR/models/sutz/Sutz-Kirchrain18-DEM-WGS84.tif");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneErlebARSutz::assemble(SLAssetManager* am, SLSceneView* sv)
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

    // Create directional light for the sunlight
    SLLightDirect* sunLight = new SLLightDirect(am, this, 5.0f);
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

    // Rotate to the true geographic rotation
    // Nothing to do here because the model is north up

    // Let the video shine through some objects
    _sutz->findChild<SLNode>("Terrain")->setMeshMat(matVideoBkgdSM, true);

    // Make buildings transparent with edges
    SLNode* buildings = _sutz->findChild<SLNode>("Buildings");
    buildings->setMeshMat(matVideoBkgd, true);
    buildings->setDrawBitsRec(SL_DB_WITHEDGES, true);

    // Add axis object a world origin
    SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
    axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
    axis->rotate(-90, 1, 0, 0);
    axis->castsShadows(false);

    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(sunLight);
    scene->addChild(axis);
    scene->addChild(_sutz);
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
//-----------------------------------------------------------------------------
