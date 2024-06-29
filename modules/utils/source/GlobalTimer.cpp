/**
 * \file      GlobalTimer.cpp
 * \authors   Michael Goettlicher
 * \date      March 2018
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include "GlobalTimer.h"

HighResTimer GlobalTimer::_timer;

void GlobalTimer::timerStart()
{
    _timer.start();
}

float GlobalTimer::timeS()
{
    return _timer.elapsedTimeInSec();
}

float GlobalTimer::timeMS()
{
    return _timer.elapsedTimeInMilliSec();
}
