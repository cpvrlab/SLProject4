/**
 * \file      FtpUtils.h
 * \authors   Marcus Hudritsch, Michael Göttlicher
 * \date      May 2019
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef CPLVRLAB_FTPUTILS_H
#define CPLVRLAB_FTPUTILS_H

#ifndef __EMSCRIPTEN__

#    include <string>
#    include <vector>

class ftplib;

using namespace std;

//! FtpUtils provides networking functionality via the FTP protocol
namespace FtpUtils
{
bool uploadFileLatestVersion(const string& fileDir,
                             const string& fileName,
                             const string& ftpHost,
                             const string& ftpUser,
                             const string& ftpPwd,
                             const string& ftpDir,
                             string&       errorMsg);

bool downloadFileLatestVersion(const string& fileDir,
                               const string& fileName,
                               const string& ftpHost,
                               const string& ftpUser,
                               const string& ftpPwd,
                               const string& ftpDir,
                               string&       errorMsg);

bool uploadFile(const string& fileDir,
                const string& fileName,
                const string& ftpHost,
                const string& ftpUser,
                const string& ftpPwd,
                const string& ftpDir,
                string&       errorMsg);

bool downloadFile(const string& fileDir,
                  const string& fileName,
                  const string& ftpHost,
                  const string& ftpUser,
                  const string& ftpPwd,
                  const string& ftpDir,
                  string&       errorMsg);

bool downloadAllFilesFromDir(const string& fileDir,
                             const string& ftpHost,
                             const string& ftpUser,
                             const string& ftpPwd,
                             const string& ftpDir,
                             const string& searchFileTag,
                             string&       errorMsg);

bool getAllFileNamesWithTag(ftplib&         ftp,
                            const string&   localDir,
                            const string&   searchFileTag,
                            vector<string>& retrievedFileNames,
                            string&         errorMsg);

string getLatestFilename(ftplib&       ftp,
                         const string& fileDir,
                         const string& fileName);

int getVersionInFilename(const string& filename);
};

#endif

#endif
