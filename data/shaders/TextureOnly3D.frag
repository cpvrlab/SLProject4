/**
 * \file      TextureOnly3D.frag
 * \brief     GLSL fragment shader for 3D texture mapping only
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;
precision highp sampler3D;

//-----------------------------------------------------------------------------
in      vec4      v_texCoord3D;     // Interpol. 3D texture coordinate

uniform sampler3D u_matTextureDiffuse0;    // 3D Color map
uniform float     u_oneOverGamma;   // 1.0f / Gamma correction value

out     vec4      o_fragColor;      // output fragment color
//-----------------------------------------------------------------------------
void main()
{
    o_fragColor = texture(u_matTextureDiffuse0, v_texCoord3D.xyz);

    // Apply gamma correction
    o_fragColor.rgb = pow(o_fragColor.rgb, vec3(u_oneOverGamma));
}
//-----------------------------------------------------------------------------
