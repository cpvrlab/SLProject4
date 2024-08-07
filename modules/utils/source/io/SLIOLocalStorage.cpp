/**
 * \file      SLIOLocalStorage.cpp
 * \date      October 2022
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLIOLocalStorage.h>

#ifdef SL_STORAGE_WEB
//-----------------------------------------------------------------------------
bool SLIOLocalStorage::exists(std::string path)
{
    // clang-format off
    return MAIN_THREAD_EM_ASM_INT({
        let path = UTF8ToString($0);
        let key = "SL:" + path;
        return localStorage.getItem(key) !== null;
    }, path.c_str());
    // clang-format on
}
//-----------------------------------------------------------------------------
SLIOReaderLocalStorage::SLIOReaderLocalStorage(std::string path)
  : SLIOReaderMemory(path)
{
    char* data;
    int   length;

    // clang-format off
    MAIN_THREAD_EM_ASM({
        let path = UTF8ToString($0);
        let key = "SL:" + path;
        let item = localStorage.getItem(key);
        let string = atob(item);

        // Allocate one more byte than necessary because stringToAscii includes a null terminator.
        let buffer = _malloc(string.length + 1);
        stringToAscii(string, buffer, string.length + 1);

        setValue($1, buffer, "i8*");
        setValue($2, string.length, "i32");
    }, path.c_str(), &data, &length);
    // clang-format on

    std::vector<char> vector(data, data + length);
    SLIOMemory::set(path, vector);
}
//-----------------------------------------------------------------------------
SLIOReaderLocalStorage::~SLIOReaderLocalStorage()
{
    SLIOMemory::clear(_path);
}
//-----------------------------------------------------------------------------
SLIOWriterLocalStorage::SLIOWriterLocalStorage(std::string path)
  : SLIOWriterMemory(path)
{
}
//-----------------------------------------------------------------------------
void SLIOWriterLocalStorage::flush()
{
    std::vector<char>& buffer = SLIOMemory::get(_path);
    const char*        data   = buffer.data();
    size_t             length = buffer.size();

    // clang-format off
    MAIN_THREAD_EM_ASM({
        let path = UTF8ToString($0);
        let section = HEAPU8.subarray($1, $1 + $2);

        let array = new Uint8Array(section);
        let binary = "";
        for(let i = 0; i < array.byteLength; i++)
            binary += String.fromCharCode(array[i]);

        let key = "SL:" + path;
        localStorage.setItem(key, btoa(binary));
    }, _path.c_str(), data, length);
    // clang-format on
}
//-----------------------------------------------------------------------------
#endif