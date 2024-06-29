/**
 * \file      Depth.frag
 * \brief     Simple depth shader
 * \date      March 2020
 * \authors   Marcus Hudritsch, Michael Schertenleib
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

precision highp float;

//-----------------------------------------------------------------------------
void main(void)
{
    gl_FragDepth = gl_FragCoord.z;
}
//-----------------------------------------------------------------------------
