/**
 * \file      TextureOnly.frag
 * \brief     GLSL fragment shader for texture mapping only
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
in      vec2      v_uv0;            // Interpol. texture coordinate

uniform sampler2D u_matTextureDiffuse0;    // Color map
uniform float     u_oneOverGamma;   // 1.0f / Gamma correction value

out     vec4      o_fragColor;      // output fragment color
//-----------------------------------------------------------------------------
void main()
{     
    o_fragColor = texture(u_matTextureDiffuse0, v_uv0);

    // Apply gamma correction
    o_fragColor.rgb = pow(o_fragColor.rgb, vec3(u_oneOverGamma));
}
//-----------------------------------------------------------------------------
