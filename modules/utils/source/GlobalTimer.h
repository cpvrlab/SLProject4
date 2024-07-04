/**
 * \file      GlobalTimer.h
 * \authors   Michael Goettlicher
 * \date      March 2018
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef GLOBAL_TIMER_H
#define GLOBAL_TIMER_H

#include "HighResTimer.h"

class GlobalTimer
{
public:
    static void  timerStart();
    static float timeS();
    static float timeMS();

private:
    static HighResTimer _timer;
};

#endif // GLOBAL_TIMER_H
