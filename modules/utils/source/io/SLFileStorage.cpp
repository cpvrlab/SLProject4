/**
 * \file      SLFileStorage.cpp
 * \date      October 2022
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLFileStorage.h>

#include <cstring>

#if defined(SL_STORAGE_FS)
#    include <SLIONative.h>
#elif defined(SL_STORAGE_WEB)
#    include <SLIOFetch.h>
#    include <SLIOMemory.h>
#    include <SLIOLocalStorage.h>
#    include <SLIOBrowserPopup.h>
#endif

//-----------------------------------------------------------------------------
//! Creates a copy of the data in the buffer
SLIOBuffer SLIOBuffer::copy()
{
    unsigned char* copy = new unsigned char[size];
    std::memcpy(copy, data, size);
    return SLIOBuffer{copy, size};
}
//-----------------------------------------------------------------------------
//! Deallocates the data owned by the buffer
void SLIOBuffer::deallocate()
{
    delete[] data;
}
//-----------------------------------------------------------------------------
//! Opens a file stream for I/O operations
/*!
 * Opens a file stream and prepares it for reading or writing. After usage,
 * the stream should be closed by calling SLFileStorage::close. The function
 * uses the kind and the mode  to determine which kind of stream it should
 * create. For example, in a web browser, configuration files are written to
 * local storage.
 * \param path Path to file
 * \param kind Kind of file
 * \param mode Mode to open the stream in
 * \return Opened stream ready for I/O
 */
SLIOStream* SLFileStorage::open(std::string    path,
                                SLIOStreamKind kind,
                                SLIOStreamMode mode)
{
#if defined(SL_STORAGE_FS)
    if (mode == IOM_read)
        return new SLIOReaderNative(path);
    else if (mode == IOM_write)
        return new SLIOWriterNative(path);
    else
        return nullptr;
#elif defined(SL_STORAGE_WEB)
    Utils::log("I/O", "OPENING \"%s\", (%d)", path.c_str(), kind);

    if (mode == IOM_read)
    {
        if (kind == IOK_shader || kind == IOK_image || kind == IOK_model || kind == IOK_font)
        {
            // Shaders, images, models and fonts are always stored on the server.
            return new SLIOReaderFetch(path);
        }
        else if (kind == IOK_config)
        {
            // Config files written by the application (e.g. by Dear ImGUI)
            // are stored in the browsers local storage, other config files
            // are stored on the server.

            if (SLIOLocalStorage::exists(path))
                return new SLIOReaderLocalStorage(path);
            else
                return new SLIOReaderFetch(path);
        }
    }
    else if (mode == IOM_write)
    {
        // Config files are written to local storage so they can be read when
        // the website is reloaded, images (e.g. screenshots) are displayed in
        // the browser window so the user can download them.

        if (kind == IOK_config)
            return new SLIOWriterLocalStorage(path);
        else if (kind == IOK_image)
            return new SLIOWriterBrowserPopup(path);
    }

    return nullptr;
#endif
}
//-----------------------------------------------------------------------------
//! Closes and deletes a stream
/*!
 * First flushes the stream to make sure that all data is written and
 * then deletes it. Implementations of SLIOStream will then close their
 * backing streams in the destructor.
 * \param stream Stream to close
 */
void SLFileStorage::close(SLIOStream* stream)
{
    stream->flush();
    delete stream;
}
//-----------------------------------------------------------------------------
//! Checks whether a given file exists
/*!
 * Checks whether a file exists at the given path and if it is of the given
 * kind. When running in a web browser, the function may need to communicate
 * with a server to check whether the file exists, which is quite slow.
 * \param path Path to file
 * \param kind Kind of file
 * \return True if the file exists
 */
bool SLFileStorage::exists(std::string path, SLIOStreamKind kind)
{
#if defined(SL_STORAGE_FS)
    return Utils::fileExists(path);
#elif defined(SL_STORAGE_WEB)
    if (path == "")
        return false;

    if (kind == IOK_shader)
        return SLIOReaderFetch::exists(path);
    else if (kind == IOK_config)
        return SLIOLocalStorage::exists(path) || SLIOReaderFetch::exists(path);
    else
        return SLIOReaderFetch::exists(path);
#endif
}
//-----------------------------------------------------------------------------
//! Reads an entire file into memory
/*!
 * Opens a stream from the path provided in read mode, allocates a buffer
 * for its content, reads the content into the buffer and closes the stream.
 * The buffer is owned by the caller and must be deallocated with a call to
 * SLFileStorage::deleteBuffer after usage.
 * \param path Path to the file to read
 * \param kind Kind of the file to read
 * \return Buffer holding the file contents and size
 */
SLIOBuffer SLFileStorage::readIntoBuffer(std::string path, SLIOStreamKind kind)
{
    SLIOStream*    stream = open(path, kind, IOM_read);
    size_t         size   = stream->size();
    unsigned char* data   = new unsigned char[size];
    stream->read(data, size);
    close(stream);

    return SLIOBuffer{data, size};
}
//-----------------------------------------------------------------------------
//! Reads an entire file into a string
/*!
 * Opens a stream from the path provided in read mode, allocates a strings
 * for its content, reads the content into the string and closes the stream.
 * Line endings are NOT converted to LF, which means that on Windows, the
 * string may contain CRLF line endings.
 * \param path Path to the file to read
 * \param kind Kind of the file to read
 * \return String containing the contents of the file
 */
std::string SLFileStorage::readIntoString(std::string path, SLIOStreamKind kind)
{
    SLIOStream* stream = open(path, kind, IOM_read);
    size_t      size   = stream->size();
    std::string string;
    string.resize(size);
    stream->read((void*)string.data(), size);
    close(stream);

    return string;
}
//-----------------------------------------------------------------------------
//! Writes a string to a file
/*!
 * Opens a stream to the path provided in write mode, writes the string to
 * the file and closes the stream. Line endings are NOT converted to LF,
 * which means that on Windows, the file may contain LF line endings after
 * writing instead of CRLF line endings.
 * \param path The path to the file to write to
 * \param kind The kind of the file to write to
 * \param string The string to write to the file
 */
void SLFileStorage::writeString(std::string        path,
                                SLIOStreamKind     kind,
                                const std::string& string)
{
    SLIOStream* stream = open(path, kind, IOM_write);
    stream->write(string.c_str(), string.size());
    close(stream);
}
//-----------------------------------------------------------------------------