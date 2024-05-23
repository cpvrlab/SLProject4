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
#include <SLEntities.h>
#include <SLFileStorage.h>
#include <SLAssetLoader.h>
#include <SLEnums.h>

#include <AppDemoScene2Dand3DText.h>
#include <AppDemoSceneAnimNode.h>
#include <AppDemoSceneAnimNodeMass.h>
#include <AppDemoSceneAnimNodeMass2.h>
#include <AppDemoSceneAnimSkinned.h>
#include <AppDemoSceneAnimSkinnedMass.h>
#include <AppDemoSceneEmpty.h>
#include <AppDemoSceneFigure.h>
#include <AppDemoSceneFrustum.h>
#include <AppDemoSceneGLTF.h>
#include <AppDemoSceneJansUniverse.h>
#include <AppDemoSceneMeshLoad.h>
#include <AppDemoSceneMinimal.h>
#include <AppDemoSceneLegacy.h>
#include <AppDemoSceneLevelOfDetail.h>
#include <AppDemoSceneLotsOfNodes.h>
#include <AppDemoSceneParticleComplexFire.h>
#include <AppDemoSceneParticleDustStorm.h>
#include <AppDemoSceneParticleFountain.h>
#include <AppDemoSceneParticleMany.h>
#include <AppDemoSceneParticleRingOfFire.h>
#include <AppDemoSceneParticleSimple.h>
#include <AppDemoSceneParticleSun.h>
#include <AppDemoScenePointClouds.h>
#include <AppDemoSceneRevolver.h>
#include <AppDemoSceneRTDoF.h>
#include <AppDemoSceneRTMuttenzerBox.h>
#include <AppDemoSceneRTLens.h>
#include <AppDemoSceneRTSpheres.h>
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
#include <AppDemoSceneVideoSensorAR.h>
#include <AppDemoSceneVideoTexture.h>
#include <AppDemoSceneVideoTrackAruco.h>
#include <AppDemoSceneVideoTrackChessboard.h>
#include <AppDemoSceneVideoTrackFace.h>
#include <AppDemoSceneVideoTrackFeatures.h>
#include <AppDemoSceneVideoTrackMediapipe.h>
#include <AppDemoSceneVideoTrackWAI.h>
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
//! Global pointer to dragon model for threaded loading
SLNode* gDragonModel = nullptr;
//-----------------------------------------------------------------------------
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

    if (sceneID == SID_Benchmark_LargeModel) //..............................................
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
    else if (sceneID == SID_Benchmark_SkinnedAnimations) //.......................................
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

    // Reset video and trackers
    CVCapture::instance()->videoType(VT_NONE); // turn off any video
    CVTracked::resetTimes();                   // delete all gVideoTracker times
    delete gVideoTracker;                      // delete the tracker deep
    gVideoTracker     = nullptr;
    gVideoTexture     = nullptr; // The video texture will be deleted by scene uninit
    gVideoTrackedNode = nullptr; // The tracked node will be deleted by scene uninit

    // reset existing sceneviews
    for (auto* sceneview : AppDemo::sceneViews)
        sceneview->unInit();

    // Clear all data in the asset manager
    am->clear();

    // Clear gui stuff that depends on scene and sceneview
    AppDemoGui::clear();

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
        case SID_AnimationNode: s = new AppDemoSceneAnimNode(); break;
        case SID_AnimationNodeMass: s = new AppDemoSceneAnimNodeMass(); break;
        case SID_AnimationSkinned: s = new AppDemoSceneAnimSkinned(); break;
        case SID_AnimationSkinnedMass: s = new AppDemoSceneAnimSkinnedMass(); break;
        case SID_VideoTextureFile:
        case SID_VideoTextureLive: s = new AppDemoSceneVideoTexture(sceneID); break;
        case SID_VideoTrackChessMain:
        case SID_VideoTrackChessScnd:
        case SID_VideoCalibrateMain:
        case SID_VideoCalibrateScnd: s = new AppDemoSceneVideoTrackChessboard(sceneID); break;
        case SID_VideoTrackArucoMain:
        case SID_VideoTrackArucoScnd: s = new AppDemoSceneVideoTrackAruco(sceneID); break;
        case SID_VideoTrackFaceMain:
        case SID_VideoTrackFaceScnd: s = new AppDemoSceneVideoTrackFace(sceneID); break;
        case SID_VideoTrackFeature2DMain: s = new AppDemoSceneVideoTrackFeatures(); break;
        case SID_VideoTrackMediaPipeHandsMain: s = new AppDemoSceneVideoTrackMediapipe(); break;
        case SID_VideoTrackWAI: s = new AppDemoSceneVideoTrackWAI(); break;
        case SID_VideoSensorAR: s = new AppDemoSceneVideoSensorAR(); break;
        case SID_ParticleSystem_Simple: s = new AppDemoSceneParticleSimple(); break;
        case SID_ParticleSystem_DustStorm: s = new AppDemoSceneParticleDustStorm(); break;
        case SID_ParticleSystem_Fountain: s = new AppDemoSceneParticleFountain(); break;
        case SID_ParticleSystem_Sun: s = new AppDemoSceneParticleSun(); break;
        case SID_ParticleSystem_RingOfFire: s = new AppDemoSceneParticleRingOfFire(); break;
        case SID_ParticleSystem_ComplexFire:
        case SID_Benchmark_ParticleSystemComplexFire: s = new AppDemoSceneParticleComplexFire(sceneID); break;
        case SID_ParticleSystem_Many: s = new AppDemoSceneParticleMany; break;
        case SID_RTSpheres: s = new AppDemoSceneRTSpheres(sceneID); break;
        case SID_RTSoftShadows: s = new AppDemoSceneRTSpheres(sceneID); break;
        case SID_RTMuttenzerBox: s = new AppDemoSceneRTMuttenzerBox(); break;
        case SID_RTDoF: s = new AppDemoSceneRTDoF(); break;
        case SID_RTLens: s = new AppDemoSceneRTLens(); break;
        case SID_Benchmark_JansUniverse: s = new AppDemoSceneJansUniverse(); break;
        case SID_Benchmark_NodeAnimations: s = new AppDemoSceneAnimNodeMass2(); break;
        case SID_Benchmark_LotsOfNodes: s = new AppDemoSceneLotsOfNodes(); break;
        case SID_Benchmark_ColumnsLOD: s = new AppDemoSceneLevelOfDetail(sceneID); break;
        case SID_Benchmark_ColumnsNoLOD: s = new AppDemoSceneLevelOfDetail(sceneID); break;
        default: s = new AppDemoSceneLegacy(sceneID); break;
    }

    sv->scene(s);
    al->scene(s);

    AppDemo::sceneID = sceneID;

    // Initialize all preloaded stuff from SLScene
    s->init(am);

#ifndef SL_EMSCRIPTEN
    s->initOculus(AppDemo::dataPath + "shaders/");
#endif

    // Deactivate in general the device sensors
    AppDemo::devRot.init();
    AppDemo::devLoc.init();

    // Reset the global SLGLState state
    SLGLState::instance()->initAll();

    auto onDoneLoading = [s, sv, am, startLoadMS] {
        s->assemble(am, sv);

        // Make sure the scene view has a camera
        if (!sv->camera())
            sv->camera(sv->sceneViewCamera());

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
                sv->setViewportFromRatio(videoAspect,
                                         sv->viewportAlign(),
                                         true);
            }
            else
                CVCapture::instance()->start(sv->viewportWdivH());
        }

        AppDemo::scene = s;

        s->loadTimeMS(GlobalTimer::timeMS() - startLoadMS);
    };

    s->registerAssetsToLoad(*al);

    AppDemoGui::loadingString = "Loading...";
    al->loadAssetsAsync(onDoneLoading);
}
//-----------------------------------------------------------------------------