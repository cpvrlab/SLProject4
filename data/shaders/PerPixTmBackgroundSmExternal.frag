
#extension GL_OES_EGL_image_external_essl3 : require

precision highp float;

#define NUM_LIGHTS 1

in vec3 v_P_VS; // Interpol. point of illumination in view space (VS)
in vec3 v_P_WS; // Interpol. point of illumination in world space (WS)
in vec3 v_N_VS; // Interpol. normal at v_P_VS in view space

uniform bool  u_lightIsOn[NUM_LIGHTS];    // flag if light is on
uniform vec4  u_lightPosVS[NUM_LIGHTS];   // position of light in view space
uniform vec4  u_lightAmbi[NUM_LIGHTS];    // ambient light intensity (Ia)
uniform vec4  u_lightDiff[NUM_LIGHTS];    // diffuse light intensity (Id)
uniform vec4  u_lightSpec[NUM_LIGHTS];    // specular light intensity (Is)
uniform vec3  u_lightSpotDir[NUM_LIGHTS]; // spot direction in view space
uniform float u_lightSpotDeg[NUM_LIGHTS]; // spot cutoff angle 1-180 degrees
uniform float u_lightSpotCos[NUM_LIGHTS]; // cosine of spot cutoff angle
uniform float u_lightSpotExp[NUM_LIGHTS]; // spot exponent
uniform vec3  u_lightAtt[NUM_LIGHTS];     // attenuation (const,linear,quadr.)
uniform bool  u_lightDoAtt[NUM_LIGHTS];   // flag if att. must be calc.
uniform vec4  u_globalAmbi;               // Global ambient scene color
uniform float u_oneOverGamma;             // 1.0f / Gamma correction value

uniform vec4  u_lightPosWS[NUM_LIGHTS];             // position of light in world space
uniform bool  u_lightCreatesShadows[NUM_LIGHTS];    // flag if light creates shadows
uniform int   u_lightNumCascades[NUM_LIGHTS];       // number of cascades for cascaded shadowmap
uniform bool  u_lightDoSmoothShadows[NUM_LIGHTS];   // flag if percentage-closer filtering is enabled
uniform int   u_lightSmoothShadowLevel[NUM_LIGHTS]; // radius of area to sample for PCF
uniform float u_lightShadowMinBias[NUM_LIGHTS];     // min. shadow bias value at 0° to N
uniform float u_lightShadowMaxBias[NUM_LIGHTS];     // min. shadow bias value at 90° to N
uniform bool  u_lightUsesCubemap[NUM_LIGHTS];       // flag if light has a cube shadow map
uniform bool  u_lightsDoColoredShadows;             // flag if shadows should be colored
uniform mat4  u_lightSpace_0;

uniform int                u_camProjType;        // type of stereo
uniform int                u_camStereoEye;       // -1=left, 0=center, 1=right
uniform mat3               u_camStereoColors;    // color filter matrix
uniform bool               u_camFogIsOn;         // flag if fog is on
uniform int                u_camFogMode;         // 0=LINEAR, 1=EXP, 2=EXP2
uniform float              u_camFogDensity;      // fog density value
uniform float              u_camFogStart;        // fog start distance
uniform float              u_camFogEnd;          // fog end distance
uniform vec4               u_camFogColor;        // fog color (usually the background)
uniform float              u_camClipNear;        // camera near plane
uniform float              u_camClipFar;         // camera far plane
uniform float              u_camBkgdWidth;       // camera background width
uniform float              u_camBkgdHeight;      // camera background height
uniform float              u_camBkgdLeft;        // camera background left
uniform float              u_camBkgdBottom;      // camera background bottom
uniform vec4               u_matAmbi;            // ambient color reflection coefficient (ka)
uniform samplerExternalOES u_matTextureDiffuse0; // Diffuse color map

uniform bool      u_matGetsShadows; // flag if material receives shadows
uniform sampler2D u_shadowMap_0;

out vec4 o_fragColor; // output fragment color
//-----------------------------------------------------------------------------
vec4 fogBlend(vec3 P_VS, vec4 inColor)
{
    float factor   = 0.0f;
    float distance = length(P_VS);

    switch (u_camFogMode)
    {
        case 0:
            factor = (u_camFogEnd - distance) / (u_camFogEnd - u_camFogStart);
            break;
        case 1:
            factor = exp(-u_camFogDensity * distance);
            break;
        default:
            factor = exp(-u_camFogDensity * distance * u_camFogDensity * distance);
            break;
    }

    vec4 outColor = factor * inColor + (1.0 - factor) * u_camFogColor;
    outColor      = clamp(outColor, 0.0, 1.0);
    return outColor;
}
//-----------------------------------------------------------------------------
void doStereoSeparation()
{
    // See SLProjType in SLEnum.h
    if (u_camProjType > 8) // stereoColors
    {
        // Apply color filter but keep alpha
        o_fragColor.rgb = u_camStereoColors * o_fragColor.rgb;
    }
    else if (u_camProjType == 6) // stereoLineByLine
    {
        if (mod(floor(gl_FragCoord.y), 2.0) < 0.5) // even
        {
            if (u_camStereoEye == -1)
                discard;
        }
        else // odd
        {
            if (u_camStereoEye == 1)
                discard;
        }
    }
    else if (u_camProjType == 7) // stereoColByCol
    {
        if (mod(floor(gl_FragCoord.x), 2.0) < 0.5) // even
        {
            if (u_camStereoEye == -1)
                discard;
        }
        else // odd
        {
            if (u_camStereoEye == 1)
                discard;
        }
    }
    else if (u_camProjType == 8) // stereoCheckerBoard
    {
        bool h = (mod(floor(gl_FragCoord.x), 2.0) < 0.5);
        bool v = (mod(floor(gl_FragCoord.y), 2.0) < 0.5);
        if (h == v) // both even or odd
        {
            if (u_camStereoEye == -1)
                discard;
        }
        else // odd
        {
            if (u_camStereoEye == 1)
                discard;
        }
    }
}
//-----------------------------------------------------------------------------
int vectorToFace(vec3 vec) // Vector to process
{
    vec3 absVec = abs(vec);
    if (absVec.x > absVec.y && absVec.x > absVec.z)
        return vec.x > 0.0 ? 0 : 1;
    else if (absVec.y > absVec.x && absVec.y > absVec.z)
        return vec.y > 0.0 ? 2 : 3;
    else
        return vec.z > 0.0 ? 4 : 5;
}
//-----------------------------------------------------------------------------
int getCascadesDepthIndex(in int i, int numCascades)
{
    float factor;

    float fi = u_camClipNear;
    float ni;

    for (int i = 0; i < numCascades - 1; i++)
    {
        ni = fi;
        fi = factor * u_camClipNear * pow((u_camClipFar / (factor * u_camClipNear)), float(i + 1) / float(numCascades));
        if (-v_P_VS.z < fi)
            return i;
    }
    return numCascades - 1;
}
//-----------------------------------------------------------------------------
float shadowTest(in int i, in vec3 N, in vec3 lightDir)
{
    if (u_lightCreatesShadows[i])
    {
        // Calculate position in light space
        mat4 lightSpace;
        vec3 lightToFragment = v_P_WS - u_lightPosWS[i].xyz;

        if (u_lightUsesCubemap[i])
        {
        }
        else
        {
            if (i == 0) lightSpace = u_lightSpace_0;
        }

        vec4 lightSpacePosition = lightSpace * vec4(v_P_WS, 1.0);

        // Normalize lightSpacePosition
        vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;

        // Convert to texture coordinates
        projCoords = projCoords * 0.5 + 0.5;

        float currentDepth = projCoords.z;

        // Look up depth from shadow map
        float shadow = 0.0;
        float closestDepth;

        // calculate bias between min. and max. bias depending on the angle between N and lightDir
        float bias = max(u_lightShadowMaxBias[i] * (1.0 - dot(N, lightDir)), u_lightShadowMinBias[i]);

        // Use percentage-closer filtering (PCF) for softer shadows (if enabled)
        if (u_lightDoSmoothShadows[i])
        {
            int  level = u_lightSmoothShadowLevel[i];
            vec2 texelSize;
            if (i == 0) { texelSize = 1.0 / vec2(textureSize(u_shadowMap_0, 0)); }

            for (int x = -level; x <= level; ++x)
            {
                for (int y = -level; y <= level; ++y)
                {
                    if (i == 0) { closestDepth = texture(u_shadowMap_0, projCoords.xy + vec2(x, y) * texelSize).r; }

                    shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
                }
            }
            shadow /= pow(1.0 + 2.0 * float(level), 2.0);
        }
        else
        {
            if (u_lightUsesCubemap[i])
            {
            }
            else if (u_lightNumCascades[i] > 0)
            {
            }
            else
            {
                if (i == 0) closestDepth = texture(u_shadowMap_0, projCoords.xy).r;
            }

            // The fragment is in shadow if the light doesn't "see" it
            if (currentDepth > closestDepth + bias)
                shadow = 1.0;
        }

        return shadow;
    }

    return 0.0;
}
//-----------------------------------------------------------------------------
void doColoredShadows(in vec3 N)
{
    const vec3 SHADOW_COLOR[6] = vec3[6](vec3(1.0, 0.0, 0.0),
                                         vec3(0.0, 1.0, 0.0),
                                         vec3(0.0, 0.0, 1.0),
                                         vec3(1.0, 1.0, 0.0),
                                         vec3(0.0, 1.0, 1.0),
                                         vec3(1.0, 0.0, 1.0));

    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        if (u_lightIsOn[i])
        {
            if (u_lightPosVS[i].w == 0.0)
            {
                // We use the spot light direction as the light direction vector
                vec3 S = normalize(-u_lightSpotDir[i].xyz);

                // Test if the current fragment is in shadow
                float shadow = u_matGetsShadows ? shadowTest(i, N, S) : 0.0;
                if (u_lightNumCascades[i] > 0)
                {
                    int casIndex = getCascadesDepthIndex(i, u_lightNumCascades[i]);
                    o_fragColor.rgb += shadow * SHADOW_COLOR[casIndex];
                }
                else
                    o_fragColor.rgb += shadow * SHADOW_COLOR[0];
            }
            else
            {
                vec3 L = u_lightPosVS[i].xyz - v_P_VS; // Vector from v_P to light in VS

                // Test if the current fragment is in shadow
                float shadow = u_matGetsShadows ? shadowTest(i, N, L) : 0.0;
                o_fragColor.rgb += shadow * SHADOW_COLOR[0];
            }
        }
    }
}
//-----------------------------------------------------------------------------
void main()
{
    float x = (gl_FragCoord.x - u_camBkgdLeft) / u_camBkgdWidth;
    float y = (gl_FragCoord.y - u_camBkgdBottom) / u_camBkgdHeight;

    if (x < 0.0f || y < 0.0f || x > 1.0f || y > 1.0f)
        o_fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    else
        o_fragColor = texture(u_matTextureDiffuse0, vec2(x, 1.0f - y));

    vec3  N      = normalize(v_N_VS); // A input normal has not anymore unit length
    float shadow = 0.0;

    // Colorize cascaded shadows for debugging purpose
    if (u_lightsDoColoredShadows)
        doColoredShadows(N);
    else
    {
        for (int i = 0; i < NUM_LIGHTS; ++i)
        {
            if (u_lightIsOn[i])
            {
                if (u_lightPosVS[i].w == 0.0)
                {
                    // We use the spot light direction as the light direction vector
                    vec3 S = normalize(-u_lightSpotDir[i].xyz);

                    // Test if the current fragment is in shadow
                    shadow = u_matGetsShadows ? shadowTest(i, N, S) : 0.0;
                }
                else
                {
                    vec3 L = u_lightPosVS[i].xyz - v_P_VS; // Vector from v_P to light in VS

                    // Test if the current fragment is in shadow
                    shadow = u_matGetsShadows ? shadowTest(i, N, L) : 0.0;
                }
                o_fragColor = o_fragColor * min(1.0 - shadow + u_matAmbi.r, 1.0);
            }
        }
    }

    // Apply fog by blending over distance
    if (u_camFogIsOn)
        o_fragColor = fogBlend(v_P_VS, o_fragColor);

    // Apply gamma correction
    o_fragColor.rgb = pow(o_fragColor.rgb, vec3(u_oneOverGamma));

    // Apply stereo eye separation
    if (u_camProjType > 1)
        doStereoSeparation();
}
//-----------------------------------------------------------------------------
