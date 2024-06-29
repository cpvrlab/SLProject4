/**
 * \file      SLIOLocalStorage.h
 * \date      October 2022
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPROJECT_SLIOLOCALSTORAGE_H
#define SLPROJECT_SLIOLOCALSTORAGE_H

#include <SLFileStorage.h>
#include <SLIOMemory.h>

#ifdef SL_STORAGE_WEB
//-----------------------------------------------------------------------------
//! Collection of functions for accessing browser local storage
namespace SLIOLocalStorage
{
bool exists(std::string path);
}
//-----------------------------------------------------------------------------
//! SLIOStream implementation for reading from browser local storage
/*!
 * The constructor loads the file from local storage into memory using the
 * JavaScript API provided by the web browser.
 */
class SLIOReaderLocalStorage : public SLIOReaderMemory
{
public:
    SLIOReaderLocalStorage(std::string path);
    ~SLIOReaderLocalStorage();
};
//-----------------------------------------------------------------------------
//! SLIOStream implementation for writing to browser local storage
/*!
 * When calling flush, the memory is written from memory to local storage using
 * the JavaScript API provided by the web browser.
 */
class SLIOWriterLocalStorage : public SLIOWriterMemory
{
public:
    SLIOWriterLocalStorage(std::string path);
    void flush();
};
//-----------------------------------------------------------------------------
#endif

#endif // SLPROJECT_SLIOLOCALSTORAGE_H
