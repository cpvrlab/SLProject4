/**
 * \file      PBR_CubeMap.vert
 * \brief     GLSL vertex program for rendering cube maps
 * \date      April 2018
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
layout (location = 0) in vec4  a_position;  // Vertex position attribute

uniform                  mat4  u_mvpMatrix; // Model-View-Projection matrix

out                      vec3  v_P_WS;      // texture coordinate at vertex
//-----------------------------------------------------------------------------
void main ()
{
    v_P_WS = a_position.xyz;
    gl_Position = u_mvpMatrix * a_position;
}
//-----------------------------------------------------------------------------
