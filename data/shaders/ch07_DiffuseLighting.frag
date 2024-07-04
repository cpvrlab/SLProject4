/**
 * \file      ch07_DiffuseLighting.frag
 * \brief     GLSL fragment program for simple diffuse per vertex lighting
 * \date      September 2012 (HS12)
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
in       vec4   diffuseColor;   // interpolated color from the vertex shader
out      vec4   o_fragColor;    // output fragment color
//-----------------------------------------------------------------------------
void main()
{     
   o_fragColor = diffuseColor;
}
//-----------------------------------------------------------------------------
