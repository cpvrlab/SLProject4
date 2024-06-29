/**
 * \file      ErrorTex.frag
 * \brief     GLSL fragment shader for error texture mapping only
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
in       vec2      v_texCoord;    // Interpol. texture coordinate

uniform  sampler2D u_matTextureDiffuse0;    // Color map

out      vec4      o_fragColor;   // output fragment color
//-----------------------------------------------------------------------------
void main()
{     
    o_fragColor = texture(u_matTextureDiffuse0, v_texCoord);
}
//-----------------------------------------------------------------------------