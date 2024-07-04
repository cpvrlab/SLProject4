/**
 * \file      ch06_ColorCube.frag
 * \brief     Simple GLSL fragment program for constant color
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
in      vec4     v_color;       // interpolated color from the vertex shader
out     vec4     o_fragColor;   // output fragment color
//-----------------------------------------------------------------------------
void main()
{     
    o_fragColor = v_color;      // Pass the color the fragment output color
}
//-----------------------------------------------------------------------------