/**
 * \file      PerPixTmBackground.vert
 * \brief     GLSL vertex program for background texture mapping
 * \date      September 2020
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
layout (location = 0) in vec4  a_position;     // Vertex position attribute

uniform mat4  u_mMatrix;    // Model matrix (object to world transform)
uniform mat4  u_vMatrix;    // View matrix (world to camera transform)
uniform mat4  u_pMatrix;    // Projection matrix (camera to normalize device coords.)
//-----------------------------------------------------------------------------
void main()
{
    // Set the transformes vertex position
    mat4 mvMatrix = u_vMatrix * u_mMatrix;
    gl_Position = u_pMatrix * mvMatrix * a_position;
}
//-----------------------------------------------------------------------------
