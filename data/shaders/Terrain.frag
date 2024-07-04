/**
 * \file      Terrain.frag
 * \brief     GLSL per vertex diffuse lighting with texturing
 * \date      September 2012 (HS12)
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
in      vec4      v_color;       // Interpol. ambient & diff. color
in      vec2      v_uv0;         // Interpol. texture coordinate

uniform sampler2D u_matTexture0; // Color map

out     vec4      o_fragColor;   // output fragment color
//-----------------------------------------------------------------------------
void main()
{  // Interpolated ambient & diffuse components  
   o_fragColor = v_color;
   
   // componentwise multiply w. texture color
   o_fragColor *= texture(u_matTexture0, v_uv0);
}
//-----------------------------------------------------------------------------