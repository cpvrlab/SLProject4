/**
 * \file      ColorUniform.vert
 * \brief     GLSL vertex program for simple uniform color
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
layout (location = 0) in vec4 a_position;  // Vertex position attribute

uniform vec4  u_matDiff;    // uniform color
uniform mat4  u_mMatrix;    // Model matrix (object to world transform)
uniform mat4  u_vMatrix;    // View matrix (world to camera transform)
uniform mat4  u_pMatrix;    // Projection matrix (camera to normalize device coords.)

out     vec4  v_color;      // Resulting color per vertex
//-----------------------------------------------------------------------------
void main(void)
{
    gl_PointSize = 1.0;
    v_color = u_matDiff;
    gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * a_position;
}
//-----------------------------------------------------------------------------
