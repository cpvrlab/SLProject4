/**
 * \file      SLOptixDefinitions.h
 * \authors   Nic Dorner
 * \date      October 2019
 * \authors   Nic Dorner
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifdef SL_HAS_OPTIX
#    ifndef SLOPTIXDEFINITIONS_H
#        define SLOPTIXDEFINITIONS_H
#        include <vector_types.h>
#        include <optix_types.h>
#        include <cuda.h>
#        include <curand_kernel.h>

//------------------------------------------------------------------------------
//! Optix ray tracing sample struct
struct ortSamples
{
    unsigned int samplesX;
    unsigned int samplesY;
};
//------------------------------------------------------------------------------
//! Optix ray tracing material information struct
struct ortMaterial
{
    float4 diffuse_color;
    float4 ambient_color;
    float4 specular_color;
    float4 transmissiv_color;
    float4 emissive_color;
    float  shininess;
    float  kr;
    float  kt;
    float  kn;
};
//------------------------------------------------------------------------------
//! Optix ray tracing light information struct
struct ortLight
{
    float4     diffuse_color;
    float4     ambient_color;
    float4     specular_color;
    float3     position;
    float      spotCutOffDEG;
    float      spotExponent;
    float      spotCosCut;
    float3     spotDirWS;
    float      kc;
    float      kl;
    float      kq;
    ortSamples samples;
    float      radius;
};
//------------------------------------------------------------------------------
struct ortParams
{
    float4*      image;
    unsigned int width;
    unsigned int height;

    int   max_depth;
    float scene_epsilon;

    //! 1/gamma of the renderer, applied to the image by the raygen program.
    /*! The path tracer kernel used to hard code 0.5, i.e. a gamma of 2.0, while
    the CPU SLPathtracer corrects with the settable gamma() property whose
    default is 2.2. The two therefore disagreed by 6 to 9 display levels and the
    Gamma slider could not reach the OptiX image at all. It sits outside the
    union below because that union overlays the ray tracer's fields with the
    path tracer's, and this one belongs to both. */
    float oneOverGamma;

    OptixTraversableHandle handle;

    union
    {
        struct
        {
            ortLight*    lights;
            unsigned int numLights;
            float4       globalAmbientColor;
        };

        struct
        {
            unsigned int samples;
            unsigned int seed;
            curandState* states;
        };
    };
};
//------------------------------------------------------------------------------
//! Optix ray tracing ray type enumeration
enum ortRayType
{
    RAY_TYPE_RADIANCE  = 0,
    RAY_TYPE_OCCLUSION = 1,
    RAY_TYPE_COUNT
};
//------------------------------------------------------------------------------
//! Optix ray tracing camera info for pinhole camera
struct ortCamera
{
    float3 eye;
    float3 U;
    float3 V;
    float3 W;
};
//------------------------------------------------------------------------------
//! Optix ray tracing camera info for lens camera
struct ortLensCamera
{
    ortCamera  camera;
    ortSamples samples;
    float      lensDiameter;
};
//------------------------------------------------------------------------------
//! Optix ray tracing intersection miss data
struct ortMissData
{
    float4 bg_color;
};
//------------------------------------------------------------------------------
//! Optix ray tracing intersection hit data
struct ortHitData
{
    ortMaterial material;
    CUtexObject textureObject;
    int         sbtIndex;
    float3*     normals;
    short3*     indices;
    float2*     texCords;
};
//------------------------------------------------------------------------------
template<typename T>
struct SbtRecord
{
    __align__(OPTIX_SBT_RECORD_ALIGNMENT) char header[OPTIX_SBT_RECORD_HEADER_SIZE];
    T data;
};
//------------------------------------------------------------------------------
typedef SbtRecord<ortCamera>     RayGenClassicSbtRecord;
typedef SbtRecord<ortLensCamera> RayGenDistributedSbtRecord;
typedef SbtRecord<ortMissData>   MissSbtRecord;
typedef SbtRecord<ortHitData>    HitSbtRecord;
//------------------------------------------------------------------------------
#    endif // SLOPTIXDEFINITIONS_H
#endif     // SL_HAS_OPTIX
