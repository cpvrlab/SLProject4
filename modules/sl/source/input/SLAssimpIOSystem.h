/**
 * \file      SLAssimpIOStream.h
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLPROJECT_SLASSIMPIOSYSTEM_H
#define SLPROJECT_SLASSIMPIOSYSTEM_H

#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include "SLFileStorage.h"

//-----------------------------------------------------------------------------
class SLAssimpIOStream : public Assimp::IOStream
{
public:
    SLAssimpIOStream(SLIOStream* stream);
    size_t   Read(void* pvBuffer, size_t pSize, size_t pCount) override;
    size_t   Write(const void* pvBuffer, size_t pSize, size_t pCount) override;
    aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override;
    size_t   Tell() const override;
    size_t   FileSize() const override;
    void     Flush() override;

    SLIOStream* stream() { return _stream; }

private:
    SLIOStream* _stream;
};
//-----------------------------------------------------------------------------
class SLAssimpIOSystem : public Assimp::IOSystem
{
public:
    bool              Exists(const char* pFile) const override;
    char              getOsSeparator() const override;
    Assimp::IOStream* Open(const char* pFile, const char* pMode) override;
    void              Close(Assimp::IOStream* pFile) override;
};
//-----------------------------------------------------------------------------
#endif // SLPROJECT_SLASSIMPIOSYSTEM_H
