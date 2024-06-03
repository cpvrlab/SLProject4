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
SLScene* appCreateScene(SLSceneID sceneID)
{
    switch (sceneID)
    {
        case SID_Empty: return new AppDemoSceneEmpty();
        case SID_Figure: return new AppDemoSceneFigure();
        case SID_Minimal: return new AppDemoSceneMinimal();
        case SID_MeshLoad: return new AppDemoSceneMeshLoad();
        case SID_Revolver: return new AppDemoSceneRevolver();
        case SID_TextureBlend: return new AppDemoSceneTextureBlend();
        case SID_TextureFilter: return new AppDemoSceneTextureFilter();
#ifdef SL_BUILD_WITH_KTX
        case SID_TextureCompression: return new AppDemoSceneTextureCompression();
#endif
        case SID_FrustumCull: return new AppDemoSceneFrustum();
        case SID_2Dand3DText: return new AppDemoScene2Dand3DText();
        case SID_PointClouds: return new AppDemoScenePointClouds();
        case SID_ZFighting: return new AppDemoSceneZFighting();
        case SID_ShaderPerVertexBlinn: return new AppDemoSceneShaderBlinn("Per Vertex Blinn-Phong Lighting", true);
        case SID_ShaderPerPixelBlinn: return new AppDemoSceneShaderBlinn("Per Pixel Blinn-Phong Lighting", false);
        case SID_ShaderPerPixelCook: return new AppDemoSceneShaderCook();
        case SID_ShaderIBL: return new AppDemoSceneShaderIBL();
        case SID_ShaderWave: return new AppDemoSceneShaderWave();
        case SID_ShaderBumpNormal: return new AppDemoSceneShaderBump();
        case SID_ShaderBumpParallax: return new AppDemoSceneShaderParallax();
        case SID_ShaderSkybox: return new AppDemoSceneShaderSkybox();
        case SID_ShaderEarth: return new AppDemoSceneShaderEarth();
        case SID_ShadowMappingBasicScene: return new AppDemoSceneShadowBasic();
        case SID_ShadowMappingLightTypes: return new AppDemoSceneShadowLightTypes();
        case SID_ShadowMappingSpotLights: return new AppDemoSceneShadowLightSpot();
        case SID_ShadowMappingPointLights: return new AppDemoSceneShadowLightPoint();
        case SID_ShadowMappingCascaded: return new AppDemoSceneShadowCascaded();
        case SID_SuzannePerPixBlinn: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting and reflection colors", false, false, false, false, false);
        case SID_SuzannePerPixBlinnTm: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting and texture mapping", true, false, false, false, false);
        case SID_SuzannePerPixBlinnNm: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting and normal mapping", false, true, false, false, false);
        case SID_SuzannePerPixBlinnAo: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting and ambient occlusion", false, false, true, false, false);
        case SID_SuzannePerPixBlinnSm: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting and shadow mapping", false, false, false, true, false);
        case SID_SuzannePerPixBlinnTmNm: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting, texture and normal mapping", true, true, false, false, false);
        case SID_SuzannePerPixBlinnTmAo: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting, texture mapping and ambient occlusion", true, false, true, false, false);
        case SID_SuzannePerPixBlinnNmAo: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting, normal mapping and ambient occlusion", false, true, true, false, false);
        case SID_SuzannePerPixBlinnTmSm: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting, texture mapping and shadow mapping", true, false, false, true, false);
        case SID_SuzannePerPixBlinnNmSm: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting, normal mapping and shadow mapping", false, true, false, true, false);
        case SID_SuzannePerPixBlinnAoSm: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting, ambient occlusion and shadow mapping", false, false, true, true, false);
        case SID_SuzannePerPixBlinnTmNmAo: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting and diffuse, normal, ambient occlusion and shadow mapping", true, true, true, false, false);
        case SID_SuzannePerPixBlinnTmNmSm: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting and diffuse, normal and shadow mapping ", true, true, false, true, false);
        case SID_SuzannePerPixBlinnTmNmAoSm: return new AppDemoSceneSuzanne("Suzanne with per pixel Blinn-Phong lighting and diffuse, normal, ambient occlusion and shadow mapping", true, true, true, true, false);
        case SID_SuzannePerPixCookTmNmAoSmEm: return new AppDemoSceneSuzanne("Suzanne with per pixel Cook-Torrance lighting and diffuse, normal, ambient occlusion, shadow and environment mapping", true, true, true, true, true);
        case SID_glTF_DamagedHelmet:
        case SID_glTF_FlightHelmet:
        case SID_glTF_Sponza:
        case SID_glTF_WaterBottle: return new AppDemoSceneGLTF(sceneID);
        case SID_Robotics_FanucCRX_FK: return new AppDemoSceneRobot();
        case SID_VolumeRayCast: return new AppDemoSceneVolumeRayCast();
        case SID_VolumeRayCastLighted: return new AppDemoSceneVolumeRayCastLighted();
        case SID_AnimationNode: return new AppDemoSceneAnimNode();
        case SID_AnimationNodeMass: return new AppDemoSceneAnimNodeMass();
        case SID_AnimationSkinned: return new AppDemoSceneAnimSkinned();
        case SID_AnimationSkinnedMass: return new AppDemoSceneAnimSkinnedMass();
        case SID_VideoTextureFile:
        case SID_VideoTextureLive: return new AppDemoSceneVideoTexture(sceneID);
        case SID_VideoTrackChessMain:
        case SID_VideoTrackChessScnd:
        case SID_VideoCalibrateMain:
        case SID_VideoCalibrateScnd: return new AppDemoSceneVideoTrackChessboard(sceneID);
        case SID_VideoTrackArucoMain:
        case SID_VideoTrackArucoScnd: return new AppDemoSceneVideoTrackAruco(sceneID);
        case SID_VideoTrackFaceMain:
        case SID_VideoTrackFaceScnd: return new AppDemoSceneVideoTrackFace(sceneID);
        case SID_VideoTrackFeature2DMain: return new AppDemoSceneVideoTrackFeatures();
        case SID_VideoTrackMediaPipeHandsMain: return new AppDemoSceneVideoTrackMediapipe();
        case SID_VideoTrackWAI: return new AppDemoSceneVideoTrackWAI();
        case SID_VideoSensorAR: return new AppDemoSceneVideoSensorAR();
        case SID_ParticleSystem_Simple: return new AppDemoSceneParticleSimple();
        case SID_ParticleSystem_DustStorm: return new AppDemoSceneParticleDustStorm();
        case SID_ParticleSystem_Fountain: return new AppDemoSceneParticleFountain();
        case SID_ParticleSystem_Sun: return new AppDemoSceneParticleSun();
        case SID_ParticleSystem_RingOfFire: return new AppDemoSceneParticleRingOfFire();
        case SID_ParticleSystem_ComplexFire:
        case SID_Benchmark_ParticleSystemComplexFire: return new AppDemoSceneParticleComplexFire(sceneID);
        case SID_ParticleSystem_Many: return new AppDemoSceneParticleMany();
        case SID_RTSpheres:
        case SID_RTSoftShadows: return new AppDemoSceneRTSpheres(sceneID);
        case SID_RTMuttenzerBox: return new AppDemoSceneRTMuttenzerBox();
        case SID_RTDoF: return new AppDemoSceneRTDoF();
        case SID_RTLens: return new AppDemoSceneRTLens();
        case SID_Benchmark_JansUniverse: return new AppDemoSceneJansUniverse();
        case SID_Benchmark_NodeAnimations: return new AppDemoSceneAnimNodeMass2();
        case SID_Benchmark_LargeModel: return new AppDemoSceneLargeModel();
        case SID_Benchmark_LotsOfNodes: return new AppDemoSceneLotsOfNodes();
        case SID_Benchmark_ColumnsLOD:
        case SID_Benchmark_ColumnsNoLOD: return new AppDemoSceneLevelOfDetail(sceneID);
        case SID_Benchmark_SkinnedAnimations: return new AppDemoSceneAnimSkinnedMass2();
        case SID_ErlebAR_BernChristoffel: return new AppDemoSceneErlebARChristoffel();
        case SID_ErlebAR_BielBFH: return new AppDemoSceneErlebARBielBFH();
        case SID_ErlebAR_AugustaRauricaTmpTht: return new AppDemoSceneErlebARAugustaTmpTht();
        case SID_ErlebAR_AventicumCigognier: return new AppDemoSceneErlebARAventicumCigognier();
        case SID_ErlebAR_AventicumTheatre: return new AppDemoSceneErlebARAventicumTheater();
        case SID_ErlebAR_AventicumAmphiteatre: return new AppDemoSceneErlebARAventicumAmphitheater();
        case SID_ErlebAR_SutzKirchrain18: return new AppDemoSceneErlebARSutz();
        default: SL_EXIT_MSG("appDemoSwitchScene: Unknown SceneID");
    }
}
//-----------------------------------------------------------------------------
void appBeforeSceneDelete(SLSceneView* sv, SLScene* s)
{
    // Reset video and trackers
    CVCapture::instance()->videoType(VT_NONE); // turn off any video
    CVTracked::resetTimes();                   // delete all gVideoTracker times
    delete gVideoTracker;                      // delete the tracker deep
    gVideoTracker     = nullptr;
    gVideoTexture     = nullptr; // The video texture will be deleted by scene uninit
    gVideoTrackedNode = nullptr; // The tracked node will be deleted by scene uninit

    // Clear gui stuff that depends on scene and sceneview
    AppDemoGui::clear();
}
//-----------------------------------------------------------------------------
void appBeforeSceneLoad(SLSceneView* sv, SLScene* s)
{
    AppDemoGui::loadingString = "Loading...";
}
//-----------------------------------------------------------------------------
void appBeforeSceneAssembly(SLSceneView* sv, SLScene* s)
{
    AppDemoGui::loadingString = "Assembling...";
}
//-----------------------------------------------------------------------------
void appAfterSceneAssembly(SLSceneView* sv, SLScene* s)
{
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
}
//-----------------------------------------------------------------------------