/**
 * \file      SLOptix.h
 * \authors   Nic Dorner
 * \date      October 2019
 * \authors   Nic Dorner
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifdef SL_HAS_OPTIX
#    ifndef SLOPTIX_H
#        define SLOPTIX_H
#        include <optix_types.h>
#        include <cuda.h>
#        include <string>
#        include <SLOptixDefinitions.h>

using std::string;

//-----------------------------------------------------------------------------
//! SLOptix base instance for static Optix initialization
class SLOptix
{
public:
    // Public global static Optix objects
    static void               createStreamAndContext();
    static OptixDeviceContext context;
    static CUstream           stream;
    static string             exePath;
};
//-----------------------------------------------------------------------------
#    endif // SLOPTIX_H
#endif     // SL_HAS_OPTIX
