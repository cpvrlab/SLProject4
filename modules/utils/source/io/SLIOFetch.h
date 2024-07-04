/**
 * \file      SLIOFetch.h
 * \date      October 2022
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPROJECT_SLIOFETCH_H
#define SLPROJECT_SLIOFETCH_H

#include <SLFileStorage.h>
#include <SLIOMemory.h>

#ifdef SL_STORAGE_WEB
//! SLIOStream implementation for downloading files from a web server
/*!
 * The constructor downloads the file via HTTP and stores it in memory. When
 * downloading, a loading screen is displayed to the user because it blocks
 * the entire application.
 */
//-----------------------------------------------------------------------------
class SLIOReaderFetch : public SLIOReaderMemory
{
public:
    static bool exists(std::string url);

    SLIOReaderFetch(std::string url);
    ~SLIOReaderFetch();
};
//-----------------------------------------------------------------------------
#endif

#endif // SLPROJECT_SLIOFETCH_H
