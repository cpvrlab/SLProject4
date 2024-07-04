/**
 * \file      Utils_iOS.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef UTILS_IOS_H
#define UTILS_IOS_H

#include <string>
//-----------------------------------------------------------------------------
//! SLFileSystem provides basic filesystem functions
class Utils_iOS
{
public:
    //! Returns true if a file exists.
    static bool fileExists(std::string& pathfilename);

    //! Returns all files and folders in a directory as a vector
    static std::vector<std::string> getAllNamesInDir(const std::string& dirName);

    //! Returns the writable configuration directory
    static std::string getAppsWritableDir();

    //! Returns the working directory
    static std::string getCurrentWorkingDir();

    //! Deletes a file on the filesystem
    static bool deleteFile(std::string& pathfilename);
};
//-----------------------------------------------------------------------------
#endif
