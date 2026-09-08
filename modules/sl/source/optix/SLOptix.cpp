/**
 * \file      SLOptix.cpp
 * \authors   Nic Dorner
 * \date      October 2019
 * \authors   Nic Dorner
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifdef SL_HAS_OPTIX
#    include <iomanip>
#    include <SL.h>
#    include <SLOptix.h>
#    include <SLOptixHelper.h>

//-----------------------------------------------------------------------------
// Global statics
OptixDeviceContext SLOptix::context   = {};
CUstream           SLOptix::stream    = {};
bool               SLOptix::available = false;
string             SLOptix::exePath;
//-----------------------------------------------------------------------------
//! callback function for optix
void context_log_cb(unsigned int level,
                    const char*  tag,
                    const char*  message,
                    void* /*cbdata */)
{
    std::cerr << "[" << std::setw(2) << level << "][" << std::setw(12) << tag << "]: "
              << message << "\n";
}
//-----------------------------------------------------------------------------
//! creates the optix and cuda context for the application
/*! Every failure in here is reported by OPTIX_CHECK and CUDA_CHECK as a thrown
SLOptixException, and none of them is fatal to the application: an OptiX context
is a bonus and the OpenGL, ray tracing and path tracing renderers do not need
one. So the exception is caught, SLOptix::available stays false, and the OptiX
menu entries stay greyed out. Letting it escape would abort the process before
the first window appears, which is what used to happen on a machine whose driver
no longer serves this OptiX ABI. */
void SLOptix::createStreamAndContext()
{
    try
    {
        createStreamAndContextOrThrow();
    }
    catch (std::exception& e)
    {
        SLOptix::available = false;
        SL_LOG("**** OptiX is not available on this machine ****");
        SL_LOG("%s", e.what());
        SL_LOG("The OptiX menu entries stay disabled. The usual causes are a");
        SL_LOG("display driver too old for the OptiX ABI of the headers in");
        SL_LOG("externals/lib-optix, or no NVIDIA GPU at all.");
    }
}
//-----------------------------------------------------------------------------
//! Does the work of createStreamAndContext and throws on any failure
void SLOptix::createStreamAndContextOrThrow()
{
    // Initialize CUDA
    CUcontext cu_ctx;
    CUDA_CHECK(cuInit(0));
    CUDA_CHECK(cuMemFree(0));
    // CUDA 13 renamed cuCtxCreate to cuCtxCreate_v4 and gave it a fourth
    // argument, a CUctxCreateParams* that carries the green context and
    // affinity settings this code does not use. The macro in cuda.h points
    // cuCtxCreate at whichever version the installed toolkit ships, so the
    // call has to follow it. The project declares CUDA 10 as its minimum, so
    // both forms stay reachable.
#if CUDA_VERSION >= 13000
    CUDA_CHECK(cuCtxCreate(&cu_ctx, nullptr, 0, 0));
#else
    CUDA_CHECK(cuCtxCreate(&cu_ctx, 0, 0));
#endif
    CUDA_CHECK(cuStreamCreate(&SLOptix::stream,
                              CU_STREAM_DEFAULT));

    // Initialize OptiX
    OPTIX_CHECK(optixInit());
    OptixDeviceContextOptions options = {};
    options.logCallbackFunction       = &context_log_cb;
    options.logCallbackLevel          = 4;
    OPTIX_CHECK(optixDeviceContextCreate(cu_ctx,
                                         &options,
                                         &SLOptix::context));

    SLOptix::available = true;
}
//-----------------------------------------------------------------------------
#endif