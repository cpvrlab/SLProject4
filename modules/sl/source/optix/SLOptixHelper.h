/**
 * \file      SLOptixHelper.h
 * \brief     Error checking macros and helpers for the OptiX ray tracer
 * \details   Everything in this header is compiled only when SL_HAS_OPTIX is
 *            defined, which the build sets when SL_BUILD_WITH_OPTIX is ON
 *            (OFF by default). SL_HAS_OPTIX is not in the Doxyfile PREDEFINED
 *            list either, so none of these symbols appear in the generated
 *            documentation — read the header itself.
 *
 *            The OPTIX_CHECK, OPTIX_CHECK_LOG, CUDA_CHECK and CUDA_SYNC_CHECK
 *            macros wrap a call, compare its result against the success code
 *            and throw an SLOptixException carrying the file and line on
 *            failure. They are adapted from NVIDIA's OptiX sutil samples.
 * \date      October 2019
 * \authors   Nic Dorner
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#ifdef SL_HAS_OPTIX
#    ifndef SLOPTIXHELPER_H
#        define SLOPTIXHELPER_H

#        include <iostream> // std::cout, std::ios
#        include <sstream>  // std::ostringstream
#        include <stdexcept>
#        include <string>
#        include <functional>
#        include <chrono>
#        include <SLVec3.h>
#        include <SLVec4.h>

using namespace std;

using namespace std::placeholders;
using namespace std::chrono;

// Optix error-checking and CUDA error-checking are copied from nvidia optix sutil
//------------------------------------------------------------------------------
// OptiX error-checking
//------------------------------------------------------------------------------

#        include <optix_stubs.h>
// clang-format off
//------------------------------------------------------------------------------
#define OPTIX_CHECK( call )                                                    \
    {                                                                          \
        OptixResult res = call;                                                \
        if( res != OPTIX_SUCCESS )                                             \
        {                                                                      \
            stringstream ss;                                                   \
            ss << "Optix call '" << #call << "' failed: " __FILE__ ":"         \
               << __LINE__ << ")\n";                                           \
            throw SLOptixException( res, ss.str().c_str() );                   \
        }                                                                      \
    }
//------------------------------------------------------------------------------
#define OPTIX_CHECK_LOG( call )                                                \
    {                                                                          \
        OptixResult res = call;                                                \
        if( res != OPTIX_SUCCESS )                                             \
        {                                                                      \
            stringstream ss;                                                   \
            ss << "Optix call '" << #call << "' failed: " __FILE__ ":"         \
               << __LINE__ << ")\nLog:\n" << log                               \
               << ( sizeof_log > sizeof( log ) ? "<TRUNCATED>" : "" )          \
               << "\n";                                                        \
            throw SLOptixException( res, ss.str().c_str() );                   \
        }                                                                      \
    }
//------------------------------------------------------------------------------
// CUDA error-checking
//------------------------------------------------------------------------------

#define CUDA_CHECK( call )                                                     \
    {                                                                          \
        CUresult result = call;                                                \
        if( result != CUDA_SUCCESS )                                           \
        {                                                                      \
            const char *errorstr;                                              \
            cuGetErrorString(result, &errorstr);                               \
            stringstream ss;                                                   \
            ss << "CUDA call (" << #call << " ) failed with error: '"          \
               << errorstr                                                     \
               << "' (" __FILE__ << ":" << __LINE__ << ")\n"                   \
               << result << "\n";                                              \
            throw SLOptixException( ss.str().c_str() );                        \
        }                                                                      \
    }
//------------------------------------------------------------------------------
#define CUDA_SYNC_CHECK( call )                                                \
    {                                                                          \
        CUstream stream = call;                                                \
        CUresult result = cuStreamSynchronize(stream);                         \
        if( result != CUDA_SUCCESS )                                           \
        {                                                                      \
            const char *errorstr;                                              \
            cuGetErrorString(result, &errorstr);                               \
            stringstream ss;                                                   \
            ss << "CUDA error on synchronize with error '"                     \
               << errorstr                                                     \
               << "' (" __FILE__ << ":" << __LINE__ << ")\n";                  \
            throw SLOptixException( ss.str().c_str() );                        \
        }                                                                      \
    }
//------------------------------------------------------------------------------
// clang-format on
//! Exception thrown by the OPTIX_CHECK and CUDA_CHECK macros on failure
class SLOptixException : public std::runtime_error
{
public:
    //! Constructs the exception from a ready-made message
    SLOptixException(const char* msg)
      : std::runtime_error(msg)
    {
    }

    //! Constructs the exception from an OptiX result code and a message
    /*! The result code is expanded to its OptiX error name and prefixed to
    the message, so the thrown what() reads "ERROR_NAME: message". */
    SLOptixException(OptixResult res, const char* msg)
      : std::runtime_error(createMessage(res, msg).c_str())
    {
    }

private:
    //! Prefixes msg with the OptiX error name belonging to res
    string createMessage(OptixResult res, const char* msg)
    {
        std::ostringstream os;
        os << optixGetErrorName(res) << ": " << msg;
        return os.str();
    }
};
//------------------------------------------------------------------------------
//! Reads a compiled PTX module from file, for handing to the OptiX pipeline
/*! \param filename Name of the CUDA C input file whose PTX is read
    \param log      Optional pointer to a compiler log string; no output when
                    *log is NULL
    \return The PTX source as a string */
string getPtxStringFromFile(
  string       filename,    // Cuda C input file name
  const char** log = NULL); // (Optional) pointer to compiler log string. If *log == NULL there is no output.
//------------------------------------------------------------------------------
//! Converts an SLVec4f into the CUDA float4 the device code expects
float4 make_float4(const SLVec4f& f);
//------------------------------------------------------------------------------
//! Converts an SLVec3f into the CUDA float3 the device code expects
float3 make_float3(const SLVec3f& f);
//------------------------------------------------------------------------------
#    endif // SLOPTIXHELPER_H
#endif     // SL_HAS_OPTIX
