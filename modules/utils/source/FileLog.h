/**
 * \file      FileLog.h
 * \date      March 2018
 * \authors   Michael Goettlicher
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#ifndef CPLVRLAB_FILE_LOG_H
#define CPLVRLAB_FILE_LOG_H

#include <string>
#include <fstream>

namespace Utils
{
//-----------------------------------------------------------------------------
//! File logging class
class FileLog
{
public:
    FileLog(std::string logDir, bool forceFlush);
    virtual ~FileLog();
    void flush();
    void post(const std::string& message);

private:
    std::ofstream _logFile;
    bool          _forceFlush;
};
//-----------------------------------------------------------------------------
};

#endif // CPLVRLAB_FILE_LOG_H
