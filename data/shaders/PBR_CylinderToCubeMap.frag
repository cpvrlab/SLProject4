/**
 * \file      PBR_CylinderToCubeMap.frag
 * \details   GLSL fragment program which takes fragment's direction to sample
 *            the equirectangular map as if it is a cube map. Based on the
 *            physically based rendering (PBR) tutorial with GLSL by Joey de
 *            Vries on https://learnopengl.com/PBR/IBL/Diffuse-irradiance
 * \authors   Carlos Arauz, Marcus Hudritsch
 * \date      April 2018
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
in      vec3        v_P_WS;           // sample direction in world space

uniform sampler2D   u_environmentMap; // environment cube map texture

out     vec4        o_fragColor;      // output fragment color
//-----------------------------------------------------------------------------
const   vec2        invAtan = vec2(0.1591, 0.3183);
//-----------------------------------------------------------------------------
// takes the fragment's direction and does some trigonometry to give a texture
// coordinate from an equirectagular map as it it is a cube map
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

//-----------------------------------------------------------------------------
void main()
{
    vec2 uv = SampleSphericalMap(normalize(v_P_WS));
    vec3 color = texture(u_environmentMap, uv).rgb;
    
    o_fragColor = vec4(color, 1.0);
}
//-----------------------------------------------------------------------------
