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
    static void               createStreamAndContextOrThrow();
    static OptixDeviceContext context;
    static CUstream           stream;
    static string             exePath;

    //! True only once a device context exists, i.e. OptiX can really be used
    /*! Compiling with OptiX says nothing about whether this machine can run it.
    The implementation lives in the display driver and the kernels are compiled
    for the actual GPU when they are loaded, so both the driver and the card
    hold a veto that no build setting can predict. Everything that starts an
    OptiX render has to ask this first; it stays false when the context, or
    later a module or a scene, could not be built. */
    static bool available;
};
//-----------------------------------------------------------------------------
#    endif // SLOPTIX_H
#endif     // SL_HAS_OPTIX
