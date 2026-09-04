/**
 * \file      SLAssimpIOSystem.h
 * \brief     Adapters that let Assimp read through SLProject's SLIOStream
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
//! Adapts a single SLIOStream to the stream interface Assimp expects
/*! Assimp reads model files through its own Assimp::IOStream abstraction.
This class forwards each of those calls to an SLIOStream, so that Assimp reads
through the same storage layer as the rest of SLProject. That matters on
Emscripten in particular, where an asset is not a file on disk but data
fetched over the network or held in browser storage. */
class SLAssimpIOStream : public Assimp::IOStream
{
public:
    //! \param stream Stream to read from; ownership stays with the caller
    SLAssimpIOStream(SLIOStream* stream);

    //! Reads pCount records of pSize bytes into pvBuffer, returns records read
    size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override;

    //! Writes pCount records of pSize bytes from pvBuffer, returns bytes written
    size_t Write(const void* pvBuffer, size_t pSize, size_t pCount) override;

    //! Moves the read position to pOffset relative to pOrigin
    aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override;

    //! Returns the current read position in bytes
    size_t Tell() const override;

    //! Returns the total size of the stream in bytes
    size_t FileSize() const override;

    //! Flushes the wrapped stream
    void Flush() override;

    //! Returns the wrapped SLProject stream
    SLIOStream* stream() { return _stream; }

private:
    SLIOStream* _stream; //!< Wrapped SLProject stream, not owned
};
//-----------------------------------------------------------------------------
//! Assimp file system handler backed by SLFileStorage
/*! Installed on the Assimp importer with SetIOHandler in SLAssimpImporter::load,
so that every file Assimp opens is resolved through SLFileStorage rather than
the C runtime. Paths are looked up with the IOK_model file kind. */
class SLAssimpIOSystem : public Assimp::IOSystem
{
public:
    //! Returns true if pFile can be opened through SLFileStorage
    bool Exists(const char* pFile) const override;

    //! Returns the path separator, always '/' for SLProject paths
    char getOsSeparator() const override;

    //! Opens pFile through SLFileStorage and wraps it in an SLAssimpIOStream
    /*! \param pFile Path of the file to open
        \param pMode Assimp mode string ("rb", "r", "rt", "wb", "w", "wt").
                     Only the first character is inspected, because an
                     SLIOStream is always binary. */
    Assimp::IOStream* Open(const char* pFile, const char* pMode) override;

    //! Closes the stream and deletes the adapter created by Open
    void Close(Assimp::IOStream* pFile) override;
};
//-----------------------------------------------------------------------------
#endif // SLPROJECT_SLASSIMPIOSYSTEM_H
