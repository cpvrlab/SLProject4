precision highp float;

#define NUM_LIGHTS 1

layout (location = 0) in vec4  a_position;       // Vertex position attribute
layout (location = 1) in vec3  a_normal;         // Vertex normal attribute

uniform mat4  u_mMatrix;                    // Model matrix (object to world transform)
uniform mat4  u_vMatrix;                    // View matrix (world to camera transform)
uniform mat4  u_pMatrix;                    // Projection matrix (camera to normalize device coords.)
out     vec3  v_P_VS;                   // Point of illumination in view space (VS)
out     vec3  v_P_WS;                   // Point of illumination in world space (WS)
out     vec3  v_N_VS;                   // Normal at P_VS in view space (VS)
//-----------------------------------------------------------------------------
void main()
{
    mat4 mvMatrix = u_vMatrix * u_mMatrix;
    v_P_VS = vec3(mvMatrix * a_position);  // vertex position in view space
    v_P_WS = vec3(u_mMatrix * a_position); // vertex position in world space
    mat3 invMvMatrix = mat3(inverse(mvMatrix));
    mat3 nMatrix = transpose(invMvMatrix);
    v_N_VS = vec3(nMatrix * a_normal);     // vertex normal in view space

    // pass the vertex w. the fix-function transform
    gl_Position = u_pMatrix * mvMatrix * a_position;
}
