/**
 * \file      AppDemoSceneID.h
 * \brief     Definition of scene IDs in the demo app
 * \date      July 2024
 * \note      https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 */

#ifndef APPDEMOSCENEID_H
#define APPDEMOSCENEID_H

#include <SLEnums.h>

//-----------------------------------------------------------------------------
enum AppDemoSceneID : SLSceneID
{
    SID_FromFile = -2,                // Custom asset loaded over menu
    SID_Empty    = SL_EMPTY_SCENE_ID, // No data in scene
    SID_Minimal,
    SID_Figure,
    SID_MeshLoad,
    SID_Revolver,
    SID_TextureFilter,
    SID_TextureBlend,
    SID_TextureCompression,
    SID_FrustumCull,
    SID_2Dand3DText,
    SID_PointClouds,
    SID_ZFighting,

    SID_ShaderPerVertexBlinn,
    SID_ShaderPerPixelBlinn,
    SID_ShaderPerPixelCook,
    SID_ShaderIBL,
    SID_ShaderWave,
    SID_ShaderBumpNormal,
    SID_ShaderBumpParallax,
    SID_ShaderSkybox,
    SID_ShaderEarth,
    SID_ShadowMappingBasicScene,
    SID_ShadowMappingLightTypes,
    SID_ShadowMappingPointLights,
    SID_ShadowMappingSpotLights,
    SID_ShadowMappingCascaded,

    SID_SuzannePerPixBlinn,
    SID_SuzannePerPixBlinnTm,
    SID_SuzannePerPixBlinnNm,
    SID_SuzannePerPixBlinnAo,
    SID_SuzannePerPixBlinnSm,
    SID_SuzannePerPixBlinnTmNm,
    SID_SuzannePerPixBlinnTmAo,
    SID_SuzannePerPixBlinnNmAo,
    SID_SuzannePerPixBlinnTmSm,
    SID_SuzannePerPixBlinnNmSm,
    SID_SuzannePerPixBlinnAoSm,
    SID_SuzannePerPixBlinnTmNmAo,
    SID_SuzannePerPixBlinnTmNmSm,
    SID_SuzannePerPixBlinnTmNmAoSm,
    SID_SuzannePerPixCookTmNmAoSmEm,

    SID_glTF_DamagedHelmet,
    SID_glTF_FlightHelmet,
    SID_glTF_Sponza,
    SID_glTF_WaterBottle,

    SID_Robotics_FanucCRX_FK,

    SID_VolumeRayCast,
    SID_VolumeRayCastLighted,

    SID_AnimationNode,
    SID_AnimationNodeMass,
    SID_AnimationSkinned,
    SID_AnimationSkinnedMass,

    SID_VideoTextureLive,
    SID_VideoTextureFile,
    SID_VideoCalibrateMain,
    SID_VideoCalibrateScnd,
    SID_VideoTrackChessMain,
    SID_VideoTrackChessScnd,
    SID_VideoTrackArucoMain,
    SID_VideoTrackArucoScnd,
    SID_VideoTrackFeature2DMain,
    SID_VideoTrackFaceMain,
    SID_VideoTrackFaceScnd,
    SID_VideoTrackMediaPipeHandsMain,
    SID_VideoSensorAR,
    SID_VideoTrackWAI,

    SID_RTMuttenzerBox,
    SID_RTSpheres,
    SID_RTSoftShadows,
    SID_RTDoF,
    SID_RTLens,

    SID_ParticleSystem_Simple,
    SID_ParticleSystem_DustStorm,
    SID_ParticleSystem_Fountain,
    SID_ParticleSystem_Sun,
    SID_ParticleSystem_RingOfFire,
    SID_ParticleSystem_ComplexFire,
    SID_ParticleSystem_Many,

    SID_MaxNoBenchmarks,

    SID_Benchmark_LargeModel,
    SID_Benchmark_LotsOfNodes,
    SID_Benchmark_NodeAnimations,
    SID_Benchmark_SkinnedAnimations,
    SID_Benchmark_ColumnsNoLOD,
    SID_Benchmark_ColumnsLOD,
    SID_Benchmark_JansUniverse,
    SID_Benchmark_ParticleSystemComplexFire,

    SID_MaxPublicAssets,

    // These scenes are not part of the public data
    SID_ErlebAR_BernChristoffel,
    SID_ErlebAR_BielBFH,
    SID_ErlebAR_BielCBB,
    SID_ErlebAR_AugustaRauricaTmpTht,
    SID_ErlebAR_AventicumAmphiteatre,
    SID_ErlebAR_AventicumCigognier,
    SID_ErlebAR_AventicumTheatre,
    SID_ErlebAR_SutzKirchrain18,

    SID_MaxAll
};
//-----------------------------------------------------------------------------

#endif