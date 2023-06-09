//#############################################################################
//  File:      sl/SLEnums.h
//  Date:      July 2014
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef SLENUMSID_H
#define SLENUMSID_H

//-----------------------------------------------------------------------------
//! Keyboard key codes enumeration
enum SLKey
{
    K_none  = 0,
    K_space = 32,
    K_tab   = 256,
    K_enter,
    K_esc,
    K_backspace,
    K_delete,
    K_up,
    K_down,
    K_right,
    K_left,
    K_home,
    K_end,
    K_insert,
    K_pageUp,
    K_pageDown,
    K_NP0,
    K_NP1,
    K_NP2,
    K_NP3,
    K_NP4,
    K_NP5,
    K_NP6,
    K_NP7,
    K_NP8,
    K_NP9,
    K_NPDivide,
    K_NPMultiply,
    K_NPAdd,
    K_NPSubtract,
    K_NPEnter,
    K_NPDecimal,
    K_F1,
    K_F2,
    K_F3,
    K_F4,
    K_F5,
    K_F6,
    K_F7,
    K_F8,
    K_F9,
    K_F10,
    K_F11,
    K_F12,
    K_super = 0x00100000,
    K_shift = 0x00200000,
    K_ctrl  = 0x00400000,
    K_alt   = 0x00800000
};
//-----------------------------------------------------------------------------
//! Rendering type enumeration
enum SLRenderType
{
    RT_gl       = 0, //!< OpenGL
    RT_rt       = 1, //!< Ray Tracing
    RT_pt       = 2, //!< Path Tracing
    RT_optix_rt = 3, //!< Ray Tracing with OptiX
    RT_optix_pt = 4  //!< Path Tracing with OptiX
};
//-----------------------------------------------------------------------------
//! Coordinate axis enumeration
enum SLAxis
{
    A_x = 0,
    A_Y = 1,
    A_z = 2
};
//-----------------------------------------------------------------------------
//! SLCommand enumerates all possible menu and keyboard commands
enum SLSceneID
{
    SID_FromFile      = -2, // Custom asset loaded over menu
    SID_Empty         = 0,  // No data in scene
    SID_Minimal       = 1,
    SID_Figure        = 2,
    SID_MeshLoad      = 3,
    SID_Revolver      = 4,
    SID_TextureFilter = 5,
    SID_TextureBlend  = 6,
#ifdef SL_BUILD_WITH_KTX
    SID_TextureCompression = 7,
#endif
    SID_FrustumCull = 8,
    SID_2Dand3DText = 9,
    SID_PointClouds = 10,
    SID_ZFighting   = 11,

    SID_ShaderPerVertexBlinn     = 12,
    SID_ShaderPerPixelBlinn      = 13,
    SID_ShaderPerPixelCook       = 14,
    SID_ShaderIBL                = 14,
    SID_ShaderPerVertexWave      = 15,
    SID_ShaderBumpNormal         = 16,
    SID_ShaderBumpParallax       = 17,
    SID_ShaderSkyBox             = 18,
    SID_ShaderEarth              = 19,
    SID_ShadowMappingBasicScene  = 20,
    SID_ShadowMappingLightTypes  = 21,
    SID_ShadowMappingPointLights = 22,
    SID_ShadowMappingSpotLights  = 23,
    SID_ShadowMappingCascaded    = 24,

    SID_SuzannePerPixBlinn         = 25,
    SID_SuzannePerPixBlinnTm       = 26,
    SID_SuzannePerPixBlinnNm       = 27,
    SID_SuzannePerPixBlinnAo       = 28,
    SID_SuzannePerPixBlinnSm       = 29,
    SID_SuzannePerPixBlinnTmNm     = 30,
    SID_SuzannePerPixBlinnTmAo     = 31,
    SID_SuzannePerPixBlinnNmAo     = 32,
    SID_SuzannePerPixBlinnTmSm     = 33,
    SID_SuzannePerPixBlinnNmSm     = 34,
    SID_SuzannePerPixBlinnAoSm     = 35,
    SID_SuzannePerPixBlinnTmNmAo   = 36,
    SID_SuzannePerPixBlinnTmNmSm   = 37,
    SID_SuzannePerPixBlinnTmNmAoSm = 38,

    SID_glTF_DamagedHelmet = 39,
    SID_glTF_FlightHelmet  = 40,
    SID_glTF_Sponza        = 41,
    SID_glTF_WaterBottle   = 42,

    SID_Robotics_FanucCRX_FK = 43,

    SID_VolumeRayCast        = 44,
    SID_VolumeRayCastLighted = 45,

    SID_AnimationMass         = 46,
    SID_AnimationSkeletal     = 47,
    SID_AnimationNode         = 48,
    SID_AnimationAstroboyArmy = 49,

    SID_VideoTextureLive        = 50,
    SID_VideoTextureFile        = 51,
    SID_VideoCalibrateMain      = 52,
    SID_VideoCalibrateScnd      = 53,
    SID_VideoTrackChessMain     = 54,
    SID_VideoTrackChessScnd     = 55,
    SID_VideoTrackArucoMain     = 56,
    SID_VideoTrackArucoScnd     = 57,
    SID_VideoTrackFeature2DMain = 57,
    SID_VideoTrackFaceMain      = 58,
    SID_VideoTrackFaceScnd      = 59,
#ifdef SL_BUILD_WITH_MEDIAPIPE
    SID_VideoTrackMediaPipeHandsMain = 60,
#endif
    SID_VideoSensorAR = 61,
#ifdef SL_BUILD_WAI
    SID_VideoTrackWAI = 62,
#endif

    SID_RTMuttenzerBox = 63,
    SID_RTSpheres      = 64,
    SID_RTSoftShadows  = 65,
    SID_RTDoF          = 66,
    SID_RTLens         = 67,
    SID_RTTest         = 68,

    SID_ErlebARBernChristoffel      = 69,
    SID_ErlebARBielBFH              = 70,
    SID_ErlebARAugustaRauricaTmp    = 71,
    SID_ErlebARAugustaRauricaTht    = 72,
    SID_ErlebARAugustaRauricaTmpTht = 73,
    SID_ErlebARAventicumAmphiteatre = 74,
    SID_ErlebARAventicumCigognier   = 75,
    SID_ErlebARAventicumTheatre     = 76,
    SID_ErlebARSutzKirchrain18      = 77,

    SID_ParticleSystem_First       = 78,
    SID_ParticleSystem_Demo        = 79,
    SID_ParticleSystem_DustStorm   = 80,
    SID_ParticleSystem_Fountain    = 81,
    SID_ParticleSystem_Sun         = 82,
    SID_ParticleSystem_RingOfFire  = 83,
    SID_ParticleSystem_FireComplex = 84,

    SID_Benchmark1_LargeModel                  = 85,
    SID_Benchmark2_MassiveNodes                = 86,
    SID_Benchmark3_NodeAnimations              = 87,
    SID_Benchmark4_SkinnedAnimations           = 88,
    SID_Benchmark5_ColumnsNoLOD                = 89,
    SID_Benchmark6_ColumnsLOD                  = 90,
    SID_Benchmark7_JansUniverse                = 91,
    SID_Benchmark8_ParticleSystemFireComplex   = 92,
    SID_Benchmark9_ParticleSystemManyParticles = 93,

    SID_Maximal
};
//-----------------------------------------------------------------------------
//! Mouse button codes
enum SLMouseButton
{
    MB_none,
    MB_left,
    MB_middle,
    MB_right
};
//-----------------------------------------------------------------------------
//! Enumeration for text alignment in a box
enum SLTextAlign
{
    TA_topLeft,
    TA_topCenter,
    TA_topRight,
    TA_centerLeft,
    TA_centerCenter,
    TA_centerRight,
    TA_bottomLeft,
    TA_bottomCenter,
    TA_bottomRight
};
//-----------------------------------------------------------------------------
//! Enumeration for available camera animation types
enum SLCamAnim
{
    CA_turntableYUp,    //!< Orbiting around central object w. turntable rotation around y & right axis.
    CA_turntableZUp,    //!< Orbiting around central object w. turntable rotation around z & right axis.
    CA_trackball,       //!< Orbiting around central object w. one rotation around one axis
    CA_walkingYUp,      //!< Walk translation with AWSD and look around rotation around y & right axis.
    CA_walkingZUp,      //!< Walk translation with AWSD and look around rotation around z & right axis.
    CA_deviceRotYUp,    //!< The device rotation controls the camera rotation.
    CA_deviceRotLocYUp, //!< The device rotation controls the camera rotation and the GPS controls the Camera Translation
    CA_off              //!< No camera animation
};
//-----------------------------------------------------------------------------
//! Enumeration for different camera projections
enum SLProjType
{
    P_monoPerspective = 0,  //!< standard mono pinhole perspective projection
    P_monoIntrinsic,        //!< standard mono pinhole perspective projection from intrinsic calibration
    P_monoOrthographic,     //!< standard mono orthographic projection
    P_stereoSideBySide,     //!< side-by-side
    P_stereoSideBySideP,    //!< side-by-side proportional for mirror stereoscopes
    P_stereoSideBySideD,    //!< side-by-side distorted for Oculus Rift like glasses
    P_stereoLineByLine,     //!< line-by-line
    P_stereoColumnByColumn, //!< column-by-column
    P_stereoPixelByPixel,   //!< checkerboard pattern (DLP3D)
    P_stereoColorRC,        //!< color masking for red-cyan anaglyphs
    P_stereoColorRG,        //!< color masking for red-green anaglyphs
    P_stereoColorRB,        //!< color masking for red-blue anaglyphs
    P_stereoColorYB         //!< color masking for yellow-blue anaglyphs (ColorCode 3D)
};
//-----------------------------------------------------------------------------
//! Enumeration for stereo eye type used for camera projection
enum SLEyeType
{
    ET_left   = -1,
    ET_center = 0,
    ET_right  = 1
};
//-----------------------------------------------------------------------------
//! Enumeration for animation modes
enum SLAnimInterpolation
{
    AI_linear,
    AI_bezier
};
//-----------------------------------------------------------------------------
//! Enumeration for animation modes
enum SLAnimLooping
{
    AL_once         = 0, //!< play once
    AL_loop         = 1, //!< loop
    AL_pingPong     = 2, //!< play once in two directions
    AL_pingPongLoop = 3  //!< loop forward and backwards
};
//-----------------------------------------------------------------------------
//! Enumeration for animation easing curves
/*!
Enumerations copied from Qt class QEasingCurve.
See http://qt-project.org/doc/qt-4.8/qeasingcurve.html#Type-enum
*/
enum SLEasingCurve
{
    EC_linear     = 0,  //!< linear easing with constant velocity
    EC_inQuad     = 1,  //!< quadratic easing in, acceleration from zero velocity
    EC_outQuad    = 2,  //!< quadratic easing out, decelerating to zero velocity
    EC_inOutQuad  = 3,  //!< quadratic easing in and then out
    EC_outInQuad  = 4,  //!< quadratic easing out and then in
    EC_inCubic    = 5,  //!< cubic in easing in, acceleration from zero velocity
    EC_outCubic   = 6,  //!< cubic easing out, decelerating to zero velocity
    EC_inOutCubic = 7,  //!< cubic easing in and then out
    EC_outInCubic = 8,  //!< cubic easing out and then in
    EC_inQuart    = 9,  //!< quartic easing in, acceleration from zero velocity
    EC_outQuart   = 10, //!< quartic easing out, decelerating to zero velocity
    EC_inOutQuart = 11, //!< quartic easing in and then out
    EC_outInQuart = 12, //!< quartic easing out and then in
    EC_inQuint    = 13, //!< quintic easing in, acceleration from zero velocity
    EC_outQuint   = 14, //!< quintic easing out, decelerating to zero velocity
    EC_inOutQuint = 15, //!< quintic easing in and then out
    EC_outInQuint = 16, //!< quintic easing out and then in
    EC_inSine     = 17, //!< sine easing in, acceleration from zero velocity
    EC_outSine    = 18, //!< sine easing out, decelerating to zero velocity
    EC_inOutSine  = 19, //!< sine easing in and then out
    EC_outInSine  = 20, //!< sine easing out and then in
};
//-----------------------------------------------------------------------------
//! Describes the relative space a transformation is applied in.
enum SLTransformSpace
{
    // Do not change order!
    TS_world = 0,
    TS_parent,
    TS_object
};
//-----------------------------------------------------------------------------
//! Skinning methods
enum SLSkinMethod
{
    SM_hardware, //!< Do vertex skinning on the GPU
    SM_software  //!< Do vertex skinning on the CPU
};
//-----------------------------------------------------------------------------
//! Shader type enumeration for vertex or fragment (pixel) shader
enum SLShaderType
{
    ST_none,
    ST_vertex,
    ST_fragment,
    ST_geometry,
    ST_tesselation
};
//-----------------------------------------------------------------------------
//! Type definition for GLSL uniform1f variables that change per frame.
enum SLUniformType
{
    UT_const,   //!< constant value
    UT_incDec,  //!< never ending loop from min to max and max to min
    UT_incInc,  //!< never ending loop from min to max
    UT_inc,     //!< never ending increment
    UT_random,  //!< random values between min and max
    UT_seconds, //!< seconds since the process has started
    UT_lambda   //!< lambda getter function
};
//-----------------------------------------------------------------------------
// @todo build a dedicated log class that defines this verbosity levels
enum SLLogVerbosity
{
    LV_quiet      = 0,
    LV_minimal    = 1,
    LV_normal     = 2,
    LV_detailed   = 3,
    LV_diagnostic = 4
};
//-----------------------------------------------------------------------------
//! Mouse button codes
enum SLViewportAlign
{
    VA_center = 0,
    VA_leftOrTop,
    VA_rightOrBottom
};
//-----------------------------------------------------------------------------
//! Corresponds to the old fog modes in OpenGL 2.1
/*! See also: https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glFog.xml
 */
enum SLFogMode
{
    FM_linear = 0,
    FM_exp,
    FM_exp2
};
//-----------------------------------------------------------------------------
//! Billboard type for its orientation used in SLParticleSystem
enum SLBillboardType
{
    BT_Camera = 0,
    BT_Vertical,
    BT_Horizontal
};
//-----------------------------------------------------------------------------
//! Particle system shape type
enum SLShapeType
{
    ST_Sphere = 0,
    ST_Box,
    ST_Cone,
    ST_Pyramid
};
//-----------------------------------------------------------------------------
//! Light Reflection Models for shader generation usd in SLMaterial
enum SLReflectionModel
{
    RM_BlinnPhong = 0,
    RM_CookTorrance,
    RM_Particle,
    RM_Custom
};
//-----------------------------------------------------------------------------
#endif
