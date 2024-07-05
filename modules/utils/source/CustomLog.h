/**
 * \file      CustomLog.h
 * \authors   Michael Goettlicher
 * \date      March 2018
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef UTILS_CUSTOMLOG_H
#define UTILS_CUSTOMLOG_H

#include <memory>
#include <string>

namespace Utils
{
//-----------------------------------------------------------------------------
//! Logger interface
class CustomLog
{
public:
    virtual void post(const std::string& message) = 0;
    virtual ~CustomLog() { ; }
};
//-----------------------------------------------------------------------------
}

#endif
