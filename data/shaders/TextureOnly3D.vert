/**
 * \file      TextureOnly3D.vert
 * \brief     GLSL vertex program for 3D texture mapping only
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
layout (location = 0) in vec4  a_position;     // Vertex position attribute

uniform mat4  u_mMatrix;    // Model matrix (object to world transform)
uniform mat4  u_vMatrix;    // View matrix (world to camera transform)
uniform mat4  u_pMatrix;    // Projection matrix (camera to normalize device coords.)
uniform mat4  u_tMatrix;    // texture transform matrix

out     vec4  v_texCoord3D; // texture coordinate at vertex
//-----------------------------------------------------------------------------
void main()
{
    // For 3D texturing we use the vertex position as texture coordinate
    // transformed by the texture matrix
    v_texCoord3D = u_tMatrix * a_position;
   
    // Set the transformes vertex position   
    gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * a_position;
}
//-----------------------------------------------------------------------------
