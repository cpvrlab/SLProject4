/**
 * \file      ch09_TextureMapping.frag
 * \brief     GLSL fragment program for simple ADS per vertex lighting with texture mapping
 * \date      September 2011 (HS11)
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#endif

in      vec4      v_color;// interpolated color from the vertex shader
in      vec2      v_texCoord;// interpolated texture coordinate

uniform sampler2D u_matTexture0;// texture map

void main()
{
    // Just set the interpolated color from the vertex shader
    gl_FragColor = v_color;

    // componentwise multiply w. texture color
    gl_FragColor *= texture2D(u_matTexture0, v_texCoord);
}