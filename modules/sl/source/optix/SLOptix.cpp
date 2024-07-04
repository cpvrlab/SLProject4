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
#    include <SLOptix.h>
#    include <SLOptixHelper.h>

//-----------------------------------------------------------------------------
// Global statics
OptixDeviceContext SLOptix::context = {};
CUstream           SLOptix::stream  = {};
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
void SLOptix::createStreamAndContext()
{
    // Initialize CUDA
    CUcontext cu_ctx;
    CUDA_CHECK(cuInit(0));
    CUDA_CHECK(cuMemFree(0));
    CUDA_CHECK(cuCtxCreate(&cu_ctx, 0, 0));
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
}
//-----------------------------------------------------------------------------
#endif