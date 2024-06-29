/**
 * \file      PBR_BRDFIntegration.vert
 * \brief     GLSL vertex program for generating a BRDF integration map, which
//             is the second part of the specular integral.
 * \date      April 2018
 * \authors   Carlos Arauz, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
layout(location = 0) in  vec3   a_position; // Vertex position attribute
layout(location = 1) in  vec2   a_uv0;      // Vertex texture coord. attribute

out vec2   v_uv0;      // Output for interpolated texture coord.
//-----------------------------------------------------------------------------
void main()
{
    v_uv0  = a_uv0;
    gl_Position = vec4(a_position, 1.0);
}
//-----------------------------------------------------------------------------
