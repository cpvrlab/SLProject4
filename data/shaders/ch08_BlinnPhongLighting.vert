/**
 * \file      ch08_BlinnPhongLighting.vert
 * \brief     GLSL vertex program for per fragment Blinn-Phong lighting
 * \date      July 2014
 * \authors   Carlos Arauz, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
in vec4  a_position;        // Vertex position attribute
in vec3  a_normal;          // Vertex normal attribute

uniform mat4  u_mMatrix;    // Model matrix (object to world transform)
uniform mat4  u_vMatrix;    // View matrix (world to camera transform)
uniform mat4  u_pMatrix;    // Projection matrix (camera to normalize device coords.)

out      vec3  v_P_VS;      // Point of illumination in view space (VS)
out      vec3  v_N_VS;      // Normal at P_VS in view space
//-----------------------------------------------------------------------------
void main(void)
{
    // Transform vertex position into view space
    mat4 mvMatrix = u_vMatrix * u_mMatrix;
    v_P_VS = vec3(mvMatrix * a_position);

    // Transform normal w. transposed, inverse model matrix
    mat3 invMvMatrix = mat3(inverse(mvMatrix));
    mat3 nMatrix = transpose(invMvMatrix);
    v_N_VS = vec3(nMatrix * a_normal);

    // Apply model, view and projection matrix
    gl_Position = u_pMatrix * mvMatrix * a_position;
}
//-----------------------------------------------------------------------------
