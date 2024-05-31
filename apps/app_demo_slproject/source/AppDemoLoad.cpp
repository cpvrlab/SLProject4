// #############################################################################
//   File:      AppDemoSceneLoad.cpp
//   Date:      February 2018
//   Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//   Authors:   Marcus Hudritsch
//   License:   This software is provided under the GNU General Public License
//              Please visit: http://opensource.org/licenses/GPL-3.0
// #############################################################################

#include <SLEnums.h>
#include <CVCapture.h>
#include <CVTracked.h>
#include <GlobalTimer.h>

#include <SLScene.h>
#include <SLAssetManager.h>
#include <SLAssetLoader.h>

#include <AppDemo.h>
#include <AppDemoGui.h>

#include <AppDemoScene2Dand3DText.h>
#include <AppDemoSceneAnimNode.h>
#include <AppDemoSceneAnimNodeMass.h>
#include <AppDemoSceneAnimNodeMass2.h>
#include <AppDemoSceneAnimSkinned.h>
#include <AppDemoSceneAnimSkinnedMass.h>
#include <AppDemoSceneAnimSkinnedMass2.h>
#include <AppDemoSceneEmpty.h>
#include <AppDemoSceneErlebARBielBFH.h>
#include <AppDemoSceneErlebARAugustaTmpTht.h>
#include <AppDemoSceneErlebARAventicumCigognier.h>
#include <AppDemoSceneErlebARAventicumTheater.h>
#include <AppDemoSceneErlebARAventicumAmphitheater.h>
#include <AppDemoSceneErlebARChristoffel.h>
#include <AppDemoSceneErlebARSutz.h>
#include <AppDemoSceneFigure.h>
#include <AppDemoSceneFrustum.h>
#include <AppDemoSceneGLTF.h>
#include <AppDemoSceneJansUniverse.h>
#include <AppDemoSceneMeshLoad.h>
#include <AppDemoSceneMinimal.h>
#include <AppDemoSceneLargeModel.h>
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
static void onDoneLoading(SLSceneView* sv, SLScene* s, SLfloat startLoadMS);
static void onDoneAssembling(SLSceneView* sv, SLScene* s, SLfloat startLoadMS);
//-----------------------------------------------------------------------------
/*!
 * Deletes the current scene and creates a new one identified by the sceneID.
 * All assets get registered for async loading while Imgui shows a progress
 * spinner in the UI. After the parallel loading the scene gets assembled back
 * in the main thread.
 * @param sv Pointer to the sceneview
 * @param sceneID Scene identifier defined in SLEnum
 */
void appDemoSwitchScene(SLSceneView* sv, SLSceneID sceneID)
{
    SLScene*         s  = nullptr;
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
        case SID_ParticleSystem_Many: s = new AppDemoSceneParticleMany(); break;
        case SID_RTSpheres:
        case SID_RTSoftShadows: s = new AppDemoSceneRTSpheres(sceneID); break;
        case SID_RTMuttenzerBox: s = new AppDemoSceneRTMuttenzerBox(); break;
        case SID_RTDoF: s = new AppDemoSceneRTDoF(); break;
        case SID_RTLens: s = new AppDemoSceneRTLens(); break;
        case SID_Benchmark_JansUniverse: s = new AppDemoSceneJansUniverse(); break;
        case SID_Benchmark_NodeAnimations: s = new AppDemoSceneAnimNodeMass2(); break;
        case SID_Benchmark_LargeModel: s = new AppDemoSceneLargeModel(); break;
        case SID_Benchmark_LotsOfNodes: s = new AppDemoSceneLotsOfNodes(); break;
        case SID_Benchmark_ColumnsLOD:
        case SID_Benchmark_ColumnsNoLOD: s = new AppDemoSceneLevelOfDetail(sceneID); break;
        case SID_Benchmark_SkinnedAnimations: s = new AppDemoSceneAnimSkinnedMass2(); break;
        case SID_ErlebAR_BernChristoffel: s = new AppDemoSceneErlebARChristoffel(); break;
        case SID_ErlebAR_BielBFH: s = new AppDemoSceneErlebARBielBFH(); break;
        case SID_ErlebAR_AugustaRauricaTmpTht: s = new AppDemoSceneErlebARAugustaTmpTht(); break;
        case SID_ErlebAR_AventicumCigognier: s = new AppDemoSceneErlebARAventicumCigognier(); break;
        case SID_ErlebAR_AventicumTheatre: s = new AppDemoSceneErlebARAventicumTheater(); break;
        case SID_ErlebAR_AventicumAmphiteatre: s = new AppDemoSceneErlebARAventicumAmphitheater(); break;
        case SID_ErlebAR_SutzKirchrain18: s = new AppDemoSceneErlebARSutz(); break;
        default: SL_EXIT_MSG("appDemoSwitchScene: Unknown SceneID");
    }

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

    ///////////////////////////////
    // Prepare for async loading //
    ///////////////////////////////

    // Register assets on the loader that have to be loaded before assembly.
    s->registerAssetsToLoad(*al);

    // `onDone` is a wrapper around `onDoneLoading` that will be called
    // in the main thread after loading.
    auto onDone = std::bind(onDoneLoading, sv, s, startLoadMS);

    // Start loading assets asynchronously.
    al->loadAssetsAsync(onDone);
    AppDemoGui::loadingString = "Loading...";
}
//-----------------------------------------------------------------------------
static void onDoneLoading(SLSceneView* sv, SLScene* s, SLfloat startLoadMS)
{
    SLAssetManager* am = AppDemo::assetManager;
    SLAssetLoader*  al = AppDemo::assetLoader;

    // Register a task to assemble the scene.
    al->addLoadTask(std::bind(&SLScene::assemble, s, am, sv));

    // `onDone` is a wrapper around `onDoneAssembling` that will be called
    // in the main thread after loading.
    auto onDone = std::bind(onDoneAssembling, sv, s, startLoadMS);

    // Start assembling the scene asynchronously.
    al->loadAssetsAsync(onDone);
    AppDemoGui::loadingString = "Assembling...";
}
//-----------------------------------------------------------------------------
static void onDoneAssembling(SLSceneView* sv, SLScene* s, SLfloat startLoadMS)
{
    /* Assign the scene to the sceneview. The sceneview exists and is used
     * before it knows its scene. This is new since we do async loading and
     * show a spinner in the sceneview. */
    sv->scene(s);

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
}