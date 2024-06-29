/**
 * \file      SLIOBrowserDisplay.h
 * \date      October 2022
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPROJECT_SLIOBROWSERPOPUP_H
#define SLPROJECT_SLIOBROWSERPOPUP_H

#include <SLFileStorage.h>
#include <SLIOMemory.h>

#ifdef SL_STORAGE_WEB
//-----------------------------------------------------------------------------
//! SLIOStream implementation to display PNG files in a browser popup
/*!
 * The popup with the PNG image is opened when SLIOWriterBrowserPopup::flush
 * is called. The popup contains a link for the user to download the file.
 */
class SLIOWriterBrowserPopup : public SLIOWriterMemory
{
public:
    SLIOWriterBrowserPopup(std::string path);
    ~SLIOWriterBrowserPopup();
    void flush();
};
//-----------------------------------------------------------------------------
#endif

#endif // SLPROJECT_SLIOBROWSERPOPUP_H
