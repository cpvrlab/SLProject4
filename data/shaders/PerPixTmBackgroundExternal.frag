/**
 * \file      PerPixTmBackground.frag
 * \brief     GLSL fragment shader for background texture mapping
 * \date      September 2020
 * \authors   Marcus Hudritsch, Michael Goettlicher
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#extension GL_OES_EGL_image_external_essl3 : enable

precision highp float;

//-----------------------------------------------------------------------------
uniform float u_camBkgdWidth;   // background width
uniform float u_camBkgdHeight;  // background height
uniform float u_camBkgdLeft;    // background left
uniform float u_camBkgdBottom;  // background bottom

uniform samplerExternalOES u_matTextureDiffuse0;      // Color map

out     vec4 o_fragColor;       // output fragment color
//-----------------------------------------------------------------------------
void main()
{
    float x = (gl_FragCoord.x - u_camBkgdLeft) / u_camBkgdWidth;
    float y = (gl_FragCoord.y - u_camBkgdBottom) / u_camBkgdHeight;

    if(x < 0.0f || y < 0.0f || x > 1.0f || y > 1.0f)
        o_fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    else
        o_fragColor = texture(u_matTextureDiffuse0, vec2(x, 1.0f - y));
}
//-----------------------------------------------------------------------------
