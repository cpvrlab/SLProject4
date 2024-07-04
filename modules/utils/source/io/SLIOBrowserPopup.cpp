/**
 * \file      SLIOBrowserDisplay.cpp
 * \date      October 2022
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLIOBrowserPopup.h>
#include <Utils.h>
#include <cassert>

#ifdef SL_STORAGE_WEB
//-----------------------------------------------------------------------------
SLIOWriterBrowserPopup::SLIOWriterBrowserPopup(std::string path)
  : SLIOWriterMemory(path)
{
    assert(Utils::endsWithString(path, ".png") && "SLIOWriteBrowserDisplay only supports PNG files");
}
//-----------------------------------------------------------------------------
SLIOWriterBrowserPopup::~SLIOWriterBrowserPopup()
{
    SLIOMemory::clear(_path);
}
//-----------------------------------------------------------------------------
void SLIOWriterBrowserPopup::flush()
{
    std::vector<char>& buffer = SLIOMemory::get(_path);
    const char*        data   = buffer.data();
    size_t             length = buffer.size();

    std::string filename = Utils::getFileName(_path);

    // clang-format off
    MAIN_THREAD_EM_ASM({
        let path = UTF8ToString($0);
        let section = HEAPU8.subarray($1, $1 + $2);
        let array = new Uint8Array(section);
        let blob = new Blob([array], {"type": "image/png"});
        globalThis.snapshotURL = URL.createObjectURL(blob);

        let link = document.querySelector("#snapshot-download");
        link.href = snapshotURL;
        link.download = path;

        document.querySelector("#snapshot-image").src = snapshotURL;
        document.querySelector("#snapshot-overlay").classList.add("visible");
    }, filename.c_str(), data, length);
    // clang-format on
}
//-----------------------------------------------------------------------------
#endif