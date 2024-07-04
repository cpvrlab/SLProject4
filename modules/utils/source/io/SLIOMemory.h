/**
 * \file      SLIOMemory.h
 * \date      October 2022
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPROJECT_SLIOMEMORY_H
#define SLPROJECT_SLIOMEMORY_H

#include <SLFileStorage.h>

#ifdef SL_STORAGE_WEB
//-----------------------------------------------------------------------------
//! Collection of functions for accessing files stored in memory
namespace SLIOMemory
{
bool               exists(std::string path);
std::vector<char>& get(std::string path);
void               set(std::string path, const std::vector<char>& data);
void               clear(std::string path);
}
//-----------------------------------------------------------------------------
//! SLIOStream implementation for reading from memory
class SLIOReaderMemory : public SLIOStream
{
public:
    SLIOReaderMemory(std::string path);
    size_t read(void* buffer, size_t size);
    size_t tell();
    bool   seek(size_t offset, Origin origin);
    size_t size();

protected:
    std::string _path;
    size_t      _position;
};
//-----------------------------------------------------------------------------
//! SLIOStream implementation for reading to memory
class SLIOWriterMemory : public SLIOStream
{
public:
    SLIOWriterMemory(std::string path);
    size_t write(const void* buffer, size_t size);
    size_t tell();
    bool   seek(size_t offset, Origin origin);
    void   flush();

protected:
    std::string _path;
    size_t      _position;
};
//-----------------------------------------------------------------------------
#endif

#endif // SLPROJECT_SLIOMEMORY_H
