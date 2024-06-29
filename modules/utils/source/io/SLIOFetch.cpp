/**
 * \file      SLIOFetch.cpp
 * \date      October 2022
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLIOFetch.h>

#ifdef SL_STORAGE_WEB
//-----------------------------------------------------------------------------
#    include <emscripten/fetch.h>
#    include <pthread.h>
#    include <cassert>
#    include <iostream>
//-----------------------------------------------------------------------------
bool SLIOReaderFetch::exists(std::string url)
{
    assert(pthread_self() != 0 && "Fetching is not allowed on the main thread");

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    std::strcpy(attr.requestMethod, "HEAD");
    attr.attributes            = EMSCRIPTEN_FETCH_SYNCHRONOUS;
    emscripten_fetch_t* fetch  = emscripten_fetch(&attr, url.c_str());
    bool                exists = fetch->status == 200;
    emscripten_fetch_close(fetch);

    return exists;
}
//-----------------------------------------------------------------------------
SLIOReaderFetch::SLIOReaderFetch(std::string url)
  : SLIOReaderMemory(url)
{
    assert(pthread_self() != 0 && "Fetching is not allowed on the main thread");

    std::cout << "FETCH \"" << url << "\"" << std::endl;

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    std::strcpy(attr.requestMethod, "GET");
    attr.attributes           = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
    emscripten_fetch_t* fetch = emscripten_fetch(&attr, url.c_str());

    int    status = fetch->status;
    size_t size   = (size_t)fetch->totalBytes;
    std::cout << "STATUS: " << status << ", SIZE: " << size << std::endl;

    if (status == 200)
    {
        SLIOMemory::set(_path, std::vector<char>(fetch->data, fetch->data + size));
    }

    emscripten_fetch_close(fetch);
}
//-----------------------------------------------------------------------------
SLIOReaderFetch::~SLIOReaderFetch()
{
    SLIOMemory::clear(_path);
}
//-----------------------------------------------------------------------------
#endif