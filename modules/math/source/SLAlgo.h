/**
 * \file      SLAlgo.h
 * \brief     Container for general algorithm functions
 * \date      November
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Michael Goettlicher, Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 */

#ifndef SLALGO_H
#define SLALGO_H

#include <SLMath.h>
#include <SLMat3.h>
#include <SLVec3.h>

//-----------------------------------------------------------------------------
//! Collection of algorithms that may should be integrated into other namespaces
namespace SLAlgo
{
bool estimateHorizon(const SLMat3f& enuRs, const SLMat3f& sRc, SLVec3f& horizon);

//! convert geodetic datum defined in degrees, minutes and seconds to decimal
template<typename T>
T geoDegMinSec2Decimal(int degrees, int minutes, T seconds);

//! Latitude Longitude Altitude (LatLonAlt), defined in Degrees, Minutes, Secondes format to decimal
template<typename T>
SLVec3<T> geoDegMinSec2Decimal(int degreesLat,
                               int minutesLat,
                               T   secondsLat,
                               int degreesLon,
                               int minutesLon,
                               T   secondsLon,
                               T   alt);

};
//-----------------------------------------------------------------------------
#endif
