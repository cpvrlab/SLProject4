/**
 * \file      FontTex.vert
 * \brief     GLSL vertex program for textured font
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
layout (location = 0) in vec4 a_position; // Vertex position attribute
layout (location = 2) in vec3 a_texCoord; // Vertex texture coord. attribute

uniform mat4  u_mMatrix;    // Model matrix (object to world transform)
uniform mat4  u_vMatrix;    // View matrix (world to camera transform)
uniform mat4  u_pMatrix;    // Projection matrix (camera to normalize device coords.)

out     vec2  v_texCoord;   // texture coordinate at vertex
//-----------------------------------------------------------------------------
void main()
{
    v_texCoord = a_texCoord.xy;
    gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * a_position;
}
//-----------------------------------------------------------------------------
