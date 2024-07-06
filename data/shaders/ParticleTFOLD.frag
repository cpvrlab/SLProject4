/**
 * \file      ParticleTF.frag
 * \brief     Simple GLSL fragment program for particle system updating this
 * \details   This shader is never used because of the transform feedback and 
 *            the rasterization off.
 * \date      December 2021
 * \authors   Affolter Marc
 * \copyright http://opensource.org/licenses/GPL-3.0
*/
precision highp float;

//-----------------------------------------------------------------------------
out     vec4     o_fragColor;       // output fragment color
//-----------------------------------------------------------------------------
void main()
{     
   o_fragColor = vec4(0,0,0,0); // Need to be here for the compilation
}
//-----------------------------------------------------------------------------