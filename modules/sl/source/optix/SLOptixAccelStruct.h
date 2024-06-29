/**
 * \file      SLOptixAccelStruct.h
 * \authors   Nic Dorner
 * \date      October 2019
 * \authors   Nic Dorner
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifdef SL_HAS_OPTIX
#    ifndef SLOPTIXACCELSTRUCT_H
#        define SLOPTIXACCELSTRUCT_H
#        include <optix_types.h>
#        include <SLOptixCudaBuffer.h>

//------------------------------------------------------------------------------
class SLOptixAccelStruct
{
public:
    SLOptixAccelStruct();
    ~SLOptixAccelStruct();

    OptixTraversableHandle optixTraversableHandle() { return _handle; }

protected:
    void buildAccelerationStructure();
    void updateAccelerationStructure();

    OptixBuildInput          _buildInput        = {};
    OptixAccelBuildOptions   _accelBuildOptions = {};
    OptixAccelBufferSizes    _accelBufferSizes  = {};
    OptixTraversableHandle   _handle            = 0; //!< Handle for generated geometry acceleration structure
    SLOptixCudaBuffer<void>* _buffer;
};
//------------------------------------------------------------------------------
#    endif // SLOPTIXACCELSTRUCT_H
#endif     // SL_HAS_OPTIX
