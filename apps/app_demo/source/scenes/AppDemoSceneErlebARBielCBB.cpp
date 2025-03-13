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

#include <AppDemoSceneErlebARBielCBB.h>
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
    AppCommon::devLoc.originLatLonAlt(47.13052, 7.24122, 433.1);        // Nullpunkt CBB LV95: 2'585'022.31, 1'219'967.25, Alt: 433.1m
    AppCommon::devLoc.defaultLatLonAlt(47.13092, 7.24106, 437); // Baum

    AppCommon::devLoc.locMaxDistanceM(1000.0f);
    AppCommon::devLoc.improveOrigin(false);
    AppCommon::devLoc.useOriginAltitude(true);
    AppCommon::devLoc.hasOrigin(true);
    AppCommon::devLoc.offsetMode(LOM_twoFingerY);
    AppCommon::devRot.zeroYawAtStart(false);
    AppCommon::devRot.offsetMode(ROM_oneFingerX);

    /* initialize sensor stuff before loading the geotiff
    AppCommon::devLoc.nameLocations().push_back(SLLocation("Baum",
                                                           47,
                                                           07,
                                                           51.48,
                                                           7,
                                                           14,
                                                           28.21,
                                                           433.1));
    */
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneErlebARBielCBB::assemble(SLAssetManager* am, SLSceneView* sv)
{
    SLMaterial*  matVideoBkgd      = new SLMaterial(am,
                                              "matVideoBkgd",
                                              gVideoTexture,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              _spVideoBackground);

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

    // Make city with hard edges and without shadow mapping
    SLNode* Umgebung = _cbb->findChild<SLNode>("Umgebung");
    Umgebung->setMeshMat(matVideoBkgd, true);
    Umgebung->setDrawBitsRec(SL_DB_WITHEDGES, true);
    Umgebung->castsShadows(false);
    
    // Make underground floor transparent
    SLNode* Decke0 = _cbb->findChild<SLNode>("Decke0");
    Decke0->setMeshMat(matVideoBkgd, true);

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
