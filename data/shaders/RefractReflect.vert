/**
 * \file      RefractReflect.vert
 * \brief     GLSL vertex program for refraction- & reflection mapping
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/#

precision highp float;

//-----------------------------------------------------------------------------
// SLGLShader::preprocessPragmas replaces #Lights by SLVLights.size()
#pragma define NUM_LIGHTS #Lights
//-----------------------------------------------------------------------------
layout (location = 0) in vec4  a_position;     // Vertex position attribute
layout (location = 1) in vec3  a_normal;       // Vertex normal attribute

uniform mat4  u_mMatrix;    // Model matrix (object to world transform)
uniform mat4  u_vMatrix;    // View matrix (world to camera transform)
uniform mat4  u_pMatrix;    // Projection matrix (camera to normalize device coords.)

uniform vec4  u_lightPosVS[NUM_LIGHTS];    // position of light in view space
uniform vec4  u_lightSpec[NUM_LIGHTS];     // specular light intensity (Is)
uniform vec4  u_matSpec;    // specular color reflection coefficient (ks)
uniform float u_matShin;    // shininess

out     vec3  v_R_OS;       // Reflected ray in object space
out     vec3  v_T_OS;       // Refracted ray in object space
out     float v_F_Theta;    // Fresnel reflection coefficient
out     vec4  v_specColor;  // Specular color at vertex
//-----------------------------------------------------------------------------
// Schlick's approximation of the Fresnel reflection coefficient
// theta: angle between normal & incident ray in radians
// F0: reflection coefficient at tetha=0
float F_theta(float theta, float F0)
{
    return F0 + (1.0-F0) * pow(1.0-theta, 5.0);
}
//-----------------------------------------------------------------------------
void main(void)
{
    mat4 mvMatrix = u_vMatrix * u_mMatrix;
    vec3 P_VS = vec3(mvMatrix * a_position);   // pos. in viewspace
    vec3 I_VS = normalize(P_VS);               // incident vector in VS

    mat3 invMvMatrix = mat3(inverse(mvMatrix));
    mat3 nMatrix = transpose(invMvMatrix);
    vec3 N_VS = normalize(nMatrix * a_normal); // normal vector in VS

    // We have to rotate the relfected & refracted ray by the inverse
    // modelview matrix back into objekt space. Without that you would see
    // always the same reflections no matter from where you look
    // Calculate reflection vector R and refracted transmission vector T
    v_R_OS = invMvMatrix * reflect(I_VS, N_VS);         // = I - 2.0*dot(N,I)*N;
    v_T_OS = invMvMatrix * refract(I_VS, N_VS, 0.6666); // eta = etaAir/etaGlass = 1/1.5

    // Schlick's approximation of the Fresnel reflection coefficient
    v_F_Theta = F_theta(max(dot(-I_VS, N_VS), 0.0), 0.2);

    // Specular color for light reflection
    vec3 E = -I_VS;                      // eye vector
    vec3 L = u_lightPosVS[0].xyz - P_VS; // Vector from P_VS to the light in VS
    vec3 H = normalize(L+E);             // Normalized halfvector between N and L
    float specFactor = pow(max(dot(N_VS,H), 0.0), u_matShin);
    v_specColor = u_lightSpec[0] * specFactor * u_matSpec;

    gl_Position = u_pMatrix * mvMatrix * a_position;
}
//-----------------------------------------------------------------------------
