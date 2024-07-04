/**
 * \file      ZipUtils.h
 * \authors   Luc Girod
 * \date      2020
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef CPLVRLAB_ZIP_UTILS_H
#define CPLVRLAB_ZIP_UTILS_H

#include <string>
#include <functional>

//! ZipUtils provides compressing & decompressing files and folders
namespace ZipUtils
{
bool zip(string path, string zipname = "");

bool unzip(string                                         zipfile,
           function<bool(string path, string filename)>   processFile,
           function<bool(const char* data, size_t len)>   writeChunk,
           function<bool(string path)>                    processDir,
           function<int(int currentFile, int totalFiles)> progress = nullptr);

bool unzip(string                                         path,
           string                                         dest     = "",
           bool                                           override = true,
           function<int(int currentFile, int totalFiles)> progress = nullptr);
}
#endif
