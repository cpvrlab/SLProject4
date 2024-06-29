/**
 * \file      FontTex.frag
 * \brief     GLSL fragment shader for textured fonts
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
in       vec2      v_texCoord;    // Interpol. texture coordinate

uniform  sampler2D u_matTextureDiffuse0;    // Color map
uniform  vec4      u_textColor;             // Text color

out      vec4      o_fragColor;   // output fragment color
//-----------------------------------------------------------------------------
void main()
{
    // Interpolated ambient & diffuse components  
    o_fragColor = u_textColor;
   
    // componentwise multiply w. texture color
    vec4 texCol = texture(u_matTextureDiffuse0, v_texCoord);
    texCol.a = texCol.r;
    texCol.r = 1.0;
    texCol.g = 1.0;
    texCol.b = 1.0;
   
    o_fragColor *= texCol;
}
//-----------------------------------------------------------------------------