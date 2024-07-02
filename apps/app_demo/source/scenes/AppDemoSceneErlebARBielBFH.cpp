/**
 * \file      AppDemoSceneErlebARBielBFH.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneErlebARBielBFH.h>
#include <CVCapture.h>
#include <AppCommon.h>
#include <SLAssetLoader.h>
#include <SLLightDirect.h>
#include <SLCoordAxis.h>

// Global pointers declared in AppDemoVideo
extern SLGLTexture* gVideoTexture;

//-----------------------------------------------------------------------------
AppDemoSceneErlebARBielBFH::AppDemoSceneErlebARBielBFH()
  : SLScene("Biel-BFH AR")
{
    info("Augmented Reality at Biel-BFH");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneErlebARBielBFH::registerAssetsToLoad(SLAssetLoader& al)
{
    // Create video texture on global pointer updated in AppDemoVideo
    al.addTextureToLoad(gVideoTexture,
                        AppCommon::texturePath + "LiveVideoError.png",
                        GL_LINEAR,
                        GL_LINEAR);

    al.addNodeToLoad(_bfh,
                     AppCommon::dataPath +
                       "erleb-AR/models/biel/Biel-BFH-Rolex.gltf");

    al.addProgramToLoad(_spVideoBackground,
                        AppCommon::shaderPath + "PerPixTmBackground.vert",
                        AppCommon::shaderPath + "PerPixTmBackground.frag");

    // initialize sensor stuff before loading the geotiff
    AppCommon::devLoc.originLatLonAlt(47.14271, 7.24337, 488.2);        // Ecke Giosa
    AppCommon::devLoc.defaultLatLonAlt(47.14260, 7.24310, 488.7 + 1.7); // auf Parkplatz
    AppCommon::devLoc.locMaxDistanceM(1000.0f);
    AppCommon::devLoc.improveOrigin(false);
    AppCommon::devLoc.useOriginAltitude(true);
    AppCommon::devLoc.hasOrigin(true);
    AppCommon::devLoc.offsetMode(LOM_twoFingerY);
    AppCommon::devRot.zeroYawAtStart(false);
    AppCommon::devRot.offsetMode(ROM_oneFingerX);

    // This loads the DEM file and overwrites the altitude of originLatLonAlt and defaultLatLonAlt
    al.addGeoTiffToLoad(AppCommon::devLoc,
                        AppCommon::dataPath +
                          "erleb-AR/models/biel/DEM_Biel-BFH_WGS84.tif");
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneErlebARBielBFH::assemble(SLAssetManager* am, SLSceneView* sv)
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

    _bfh->setMeshMat(matVideoBkgd, true);

    // Make terrain a video shine trough
    // _bfh->findChild<SLNode>("Terrain")->setMeshMat(matVideoBkgd, true);

    /* Make buildings transparent
    SLNode* buildings = _bfh->findChild<SLNode>("Buildings");
    SLNode* roofs = _bfh->findChild<SLNode>("Roofs");
    auto updateTranspFnc = [](SLMaterial* m) {m->kt(0.5f);};
    buildings->updateMeshMat(updateTranspFnc, true);
    roofs->updateMeshMat(updateTranspFnc, true);

    // Set ambient on all child nodes
    _bfh->updateMeshMat([](SLMaterial* m) { m->ambient(SLCol4f(.2f, .2f, .2f)); }, true);
    */

    // Add axis object a world origin
    SLNode* axis = new SLNode(new SLCoordAxis(am), "Axis Node");
    axis->scale(2);
    axis->rotate(-90, 1, 0, 0);

    SLNode* scene = new SLNode("Scene");
    root3D(scene);
    scene->addChild(sunLight);
    scene->addChild(axis);
    scene->addChild(_bfh);
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
    sv->drawBits()->on(SL_DB_ONLYEDGES);
}
//-----------------------------------------------------------------------------
