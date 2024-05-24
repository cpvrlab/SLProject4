//#############################################################################
//  File:      AppDemoSceneErlebARAventicumCigognier.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneErlebARAventicumCigognier.h>
#include <CVCapture.h>
#include <AppDemo.h>
#include <SLLightDirect.h>
#include <SLCoordAxis.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;

//-----------------------------------------------------------------------------
AppDemoSceneErlebARAventicumCigognier::AppDemoSceneErlebARAventicumCigognier()
  : AppScene("Aventicum Cigognier AR")
{
    info("Aventicum Cigognier AR");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneErlebARAventicumCigognier::registerAssetsToLoad(SLAssetLoader& al)
{
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppDemo::texturePath + "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);

    al.addNodeToLoad(_cigognier,
                     AppDemo::dataPath +
                       "erleb-AR/models/avenches/avenches-cigognier.gltf");

    // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
    al.addGeoTiffToLoad(AppDemo::devLoc,
                        AppDemo::dataPath +
                          "erleb-AR/models/avenches/DTM-Aventicum-WGS84.tif");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneErlebARAventicumCigognier::assemble(SLAssetManager* am,
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
    cam1->clipFar(400);
    cam1->focalDist(150);
    cam1->setInitialState();
    cam1->devRotLoc(&AppDemo::devRot, &AppDemo::devLoc);
    cam1->background().texture(gVideoTexture);

    // Turn on main video
    CVCapture::instance()->videoType(VT_MAIN);

    // Create directional light for the sunlight
    SLLightDirect* sunLight = new SLLightDirect(am, this, 1.0f);
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

    // Rotate to the true geographic rotation
    _cigognier->rotate(-36.52f, 0, 1, 0, TS_parent);

    // Let the video shine through some objects
    _cigognier->findChild<SLNode>("Tmp-Sol-Pelouse")->setMeshMat(matVideoBkgdSM, true);
    _cigognier->findChild<SLNode>("Tmp-Souterrain")->setMeshMat(matVideoBkgd, true);

    // Add axis object a world origin
    SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
    axis->setDrawBitsRec(SL_DB_MESHWIRED, false);
    axis->rotate(-90, 1, 0, 0);
    axis->castsShadows(false);

    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(sunLight);
    scene->addChild(axis);
    scene->addChild(_cigognier);
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
//-----------------------------------------------------------------------------
