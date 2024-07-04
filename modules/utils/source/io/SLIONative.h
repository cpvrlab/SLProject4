/**
 * \file      SLIONative.h
 * \date      October 2022
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPROJECT_SLIONATIVE_H
#define SLPROJECT_SLIONATIVE_H

#include <SLFileStorage.h>

#ifdef SL_STORAGE_FS
//! SLIOStream implementation for reading from native files
//-----------------------------------------------------------------------------
class SLIOReaderNative : public SLIOStream
{
public:
    SLIOReaderNative(std::string path);
    size_t read(void* buffer, size_t size);
    size_t tell();
    bool   seek(size_t offset, Origin origin);
    size_t size();

private:
    std::ifstream _stream;
};
//-----------------------------------------------------------------------------
//! SLIOStream implementation for writing to native files
class SLIOWriterNative : public SLIOStream
{
public:
    SLIOWriterNative(std::string path);
    size_t write(const void* buffer, size_t size);
    size_t tell();
    bool   seek(size_t offset, Origin origin);
    void   flush();

private:
    std::ofstream _stream;
};
//-----------------------------------------------------------------------------
#endif

#endif // SLPROJECT_SLIONATIVE_H
