#
# CMake configuration for libsl
#

#add_compile_options("-w")
add_subdirectory(externals)

set(target sl)

set(platform_specific_include_dirs)


#########################
# CUDA and OPTIX Checks #
#########################

if (DEFINED ENV{CUDA_PATH} AND SL_BUILD_WITH_OPTIX)
    message(STATUS "[OptiX] CUDA_PATH is defined")
    message(STATUS "----------------------------------------------------------------")
    
    find_package(CUDA 10 REQUIRED)
    find_package(OptiX 7.2 REQUIRED)
    set(DEFAULT_COMPILE_DEFINITIONS
            ${DEFAULT_COMPILE_DEFINITIONS}
            SL_HAS_OPTIX
            SL_BUILD_WITH_OPTIX)
else ()
    if (DEFINED ENV{CUDA_PATH})
        message(STATUS "[OptiX] Has CUDA but OPTIX will not be built.")
        message(STATUS "[OptiX] Please download Optix at: https://developer.nvidia.com/designworks/optix/download")
        message(STATUS "----------------------------------------------------------------")
    endif ()
    if (SL_BUILD_WITH_OPTIX)
        if ("${SYSTEM_NAME_UPPER}" STREQUAL "WINDOWS" OR "${SYSTEM_NAME_UPPER}" STREQUAL "LINUX")
            # A warning and not a fatal error: the rest of the project builds
            # perfectly well without OptiX, and stopping the configuration would
            # leave a student who merely ticked the box unable to build at all.
            # It must be loud, though, because the build otherwise succeeds and
            # silently produces a binary whose OptiX menu entries stay greyed
            # out, which looks like a bug in the program rather than a missing
            # toolkit.
            message(WARNING
                    "[OptiX] SL_BUILD_WITH_OPTIX is ON but the environment variable "
                    "CUDA_PATH is not set, so the OptiX renderers will NOT be built "
                    "and their menu entries stay disabled. Install the NVidia CUDA "
                    "Toolkit and configure again. The OptiX headers themselves are "
                    "already in externals/lib-optix, so there is nothing else to "
                    "download.")
            message(STATUS "----------------------------------------------------------------")
        else ()
            message(STATUS "[OptiX] Optix is so far only available on Windows or Linux")
            message(STATUS "----------------------------------------------------------------")
        endif ()
    endif ()
endif ()

file(GLOB cuda
        ${CMAKE_CURRENT_SOURCE_DIR}/cuda/SLOptixPathtracerCamera.cu
        ${CMAKE_CURRENT_SOURCE_DIR}/cuda/SLOptixPathtracerShading.cu
        ${CMAKE_CURRENT_SOURCE_DIR}/cuda/SLOptixRaytracerCamera.cu
        ${CMAKE_CURRENT_SOURCE_DIR}/cuda/SLOptixRaytracerShading.cu
        )

#############################
# Source files of SLProject #
#############################

file(GLOB sources
        cuda/SLOptixHelper.h
        cuda/SLOptixVectorMath.h
        externals/imgui/backends/imgui_impl_opengl3.h
        externals/imgui/backends/imgui_impl_opengl3.cpp
        source/SL.h
        source/SLBackground.cpp
        source/SLBackground.h
        source/SLDrawBits.h
        source/SLEnums.h
        source/SLEventHandler.h
        source/SLImGui.h
        source/SLImGui.cpp
        source/SLMaterial.cpp
        source/SLMaterial.h
        source/SLObject.h
        source/SLScene.cpp
        source/SLScene.h
        source/SLEntities.cpp
        source/SLEntities.h
        source/SLSceneView.cpp
        source/SLSceneView.h
        source/SLShadowMap.cpp
        source/SLShadowMap.h
        source/SLTexColorLUT.cpp
        source/SLTexColorLUT.h
        source/SLTexFont.cpp
        source/SLTexFont.h
        source/accelstruct/SLAABBox.cpp
        source/accelstruct/SLAABBox.h
        source/accelstruct/SLAccelStruct.h
        source/accelstruct/SLCompactGrid.cpp
        source/accelstruct/SLCompactGrid.h
        source/animation/SLAnimKeyframe.cpp
        source/animation/SLAnimKeyframe.h
        source/animation/SLAnimManager.cpp
        source/animation/SLAnimManager.h
        source/animation/SLAnimPlayback.cpp
        source/animation/SLAnimPlayback.h
        source/animation/SLAnimSkeleton.cpp
        source/animation/SLAnimSkeleton.h
        source/animation/SLAnimTrack.cpp
        source/animation/SLAnimTrack.h
        source/animation/SLAnimation.cpp
        source/animation/SLAnimation.h
        source/gl/SLGLDepthBuffer.cpp
        source/gl/SLGLDepthBuffer.h
        source/gl/SLGLEnums.h
        source/gl/SLGLFbo.cpp
        source/gl/SLGLFbo.h
        source/gl/SLGLFrameBuffer.cpp
        source/gl/SLGLFrameBuffer.h
        source/gl/SLGLImGui.cpp
        source/gl/SLGLImGui.h
        source/gl/SLGLOVRWorkaround.h
        source/gl/SLGLOculus.cpp
        source/gl/SLGLOculus.h
        source/gl/SLGLOculusFB.cpp
        source/gl/SLGLOculusFB.h
        source/gl/SLGLProgram.cpp
        source/gl/SLGLProgram.h
        source/gl/SLGLProgramGenerated.cpp
        source/gl/SLGLProgramGenerated.h
        source/gl/SLGLProgramGeneric.h
        source/gl/SLGLProgramManager.cpp
        source/gl/SLGLProgramManager.h
        source/gl/SLGLShader.cpp
        source/gl/SLGLShader.h
        source/gl/SLGLState.cpp
        source/gl/SLGLState.h
        source/gl/SLGLTexture.cpp
        source/gl/SLGLTexture.h
        source/gl/SLGLTextureIBL.cpp
        source/gl/SLGLTextureIBL.h
        source/gl/SLGLUniform.h
        source/gl/SLGLVertexArray.cpp
        source/gl/SLGLVertexArray.h
        source/gl/SLGLVertexArrayExt.cpp
        source/gl/SLGLVertexArrayExt.h
        source/gl/SLGLVertexBuffer.cpp
        source/gl/SLGLVertexBuffer.h
        source/input/SLAssetLoader.cpp
        source/input/SLAssetLoader.h
        source/input/SLAssetManager.cpp
        source/input/SLAssetManager.h
        source/input/SLAssimpImporter.cpp
        source/input/SLAssimpImporter.h
        source/input/SLAssimpIOSystem.cpp
        source/input/SLAssimpIOSystem.h
        source/input/SLDeviceLocation.cpp
        source/input/SLDeviceLocation.h
        source/input/SLDeviceRotation.cpp
        source/input/SLDeviceRotation.h
        source/input/SLImporter.cpp
        source/input/SLImporter.h
        source/input/SLInputDevice.cpp
        source/input/SLInputDevice.h
        source/input/SLInputEvent.h
        source/input/SLInputEventInterface.cpp
        source/input/SLInputEventInterface.h
        source/input/SLInputManager.cpp
        source/input/SLInputManager.h
        source/input/SLUiInterface.h
        source/math/SLCurveBezier.cpp
        source/math/SLCurveBezier.h
        source/math/SLRect.h
        source/mesh/SLArrow.h
        source/mesh/SLBox.cpp
        source/mesh/SLBox.h
        source/mesh/SLCircle.cpp
        source/mesh/SLCircle.h
        source/mesh/SLCone.cpp
        source/mesh/SLCone.h
        source/mesh/SLCoordAxis.cpp
        source/mesh/SLCoordAxis.h
        source/mesh/SLCoordAxisArrow.cpp
        source/mesh/SLCoordAxisArrow.h
        source/mesh/SLCylinder.cpp
        source/mesh/SLCylinder.h
        source/mesh/SLDisk.cpp
        source/mesh/SLDisk.h
        source/math/SLFrustum.h
        source/math/SLFrustum.cpp
        source/mesh/SLGrid.cpp
        source/mesh/SLGrid.h
        source/mesh/SLLens.cpp
        source/mesh/SLLens.h
        source/mesh/SLMesh.cpp
        source/mesh/SLMesh.h
        source/mesh/SLParticleSystem.cpp
        source/mesh/SLParticleSystem.h
        source/mesh/SLPoints.cpp
        source/mesh/SLPoints.h
        source/mesh/SLPolygon.cpp
        source/mesh/SLPolygon.h
        source/mesh/SLPolyline.h
        source/mesh/SLRectangle.cpp
        source/mesh/SLRectangle.h
        source/mesh/SLRevolver.cpp
        source/mesh/SLRevolver.h
        source/mesh/SLSphere.h
        source/mesh/SLSpheric.cpp
        source/mesh/SLSpheric.h
        source/mesh/SLTriangle.cpp
        source/mesh/SLTriangle.h
        source/node/SLCamera.cpp
        source/node/SLCamera.h
        source/node/SLHorizonNode.cpp
        source/node/SLHorizonNode.h
        source/node/SLJoint.cpp
        source/node/SLJoint.h
        source/node/SLKeyframeCamera.cpp
        source/node/SLKeyframeCamera.h
        source/node/SLLight.cpp
        source/node/SLLight.h
        source/node/SLLightDirect.cpp
        source/node/SLLightDirect.h
        source/node/SLLightRect.cpp
        source/node/SLLightRect.h
        source/node/SLLightSpot.cpp
        source/node/SLLightSpot.h
        source/node/SLNode.cpp
        source/node/SLNode.h
        source/node/SLNodeLOD.cpp
        source/node/SLNodeLOD.h
        source/node/SLSkybox.cpp
        source/node/SLSkybox.h
        source/node/SLText.cpp
        source/node/SLText.h
        source/node/SLTransformNode.cpp
        source/node/SLTransformNode.h
        source/optix/SLOptix.cpp
        source/optix/SLOptix.h
        source/optix/SLOptixAccelStruct.cpp
        source/optix/SLOptixCudaBuffer.h
        source/optix/SLOptixDefinitions.h
        source/optix/SLOptixHelper.cpp
        source/optix/SLOptixPathtracer.cpp
        source/optix/SLOptixPathtracer.h
        source/optix/SLOptixRaytracer.cpp
        source/optix/SLOptixRaytracer.h
        source/ray/SLPathtracer.cpp
        source/ray/SLPathtracer.h
        source/ray/SLRay.cpp
        source/ray/SLRay.h
        source/ray/SLRaySamples2D.cpp
        source/ray/SLRaySamples2D.h
        source/ray/SLRaytracer.cpp
        source/ray/SLRaytracer.h
        externals/stb/stb_image.h
        )

file(GLOB shaders
        ${SL_PROJECT_ROOT}/data/shaders/*.vert
        ${SL_PROJECT_ROOT}/data/shaders/*.frag
        ${SL_PROJECT_ROOT}/data/shaders/*.geom
        ${SL_PROJECT_ROOT}/data/shaders/*.glsl
        )

# Group source files
source_group_by_path("${CMAKE_CURRENT_SOURCE_DIR}/source" "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$" "Source Files" ${sources})
source_group_by_path("${SL_PROJECT_ROOT}/data/shaders" "\\\\.frag|\\\\.vert|\\\\.geom$" "Shaders" ${shaders})

include(${SL_PROJECT_ROOT}/cmake/PlatformLinkLibs.cmake)

#==============================================================================
if ("${SYSTEM_NAME_UPPER}" STREQUAL "LINUX")
    set(compile_definitions
            ${compile_definitions}
            APP_USES_CVCAPTURE
            SL_GUI_GLFW)

elseif ("${SYSTEM_NAME_UPPER}" STREQUAL "WINDOWS") #---------------------------
    set(compile_definitions
            ${compile_definitions}
            _GLFW_NO_DLOAD_GDI32
            _GLFW_NO_DLOAD_WINMM
            APP_USES_CVCAPTURE
            SL_GUI_GLFW)

elseif ("${SYSTEM_NAME_UPPER}" STREQUAL "DARWIN") #----------------------------
    set(compile_definitions
            ${compile_definitions}
            APP_USES_CVCAPTURE
            SL_GUI_GLFW)

endif ()
#==============================================================================

#==============================================================================

if (DEFINED ENV{CUDA_PATH} AND SL_BUILD_WITH_OPTIX)
    message("Has-Optics: " "${OptiX_INCLUDE}")

    # The CUDA architecture that the OptiX kernels are compiled for. This was
    # hard coded to sm_60 (Pascal), which recent toolkits reject outright --
    # CUDA 13 answers "nvcc fatal : Unsupported gpu architecture 'sm_60'" and
    # accepts compute_75 and up only -- so the OptiX build stopped working on
    # every up to date machine and stayed broken.
    #
    # The default is the LOWEST architecture the toolkit still supports, not the
    # newest, because these kernels are compiled to PTX rather than to machine
    # code. The display driver compiles that PTX for the card it actually finds
    # when the program runs, and PTX is forward compatible: sm_75 runs on
    # Turing, Ampere, Ada, Blackwell and whatever follows, whereas sm_89 would
    # fail on any card older than an RTX 40. One default therefore serves every
    # student's machine. Raise it only to use instructions a newer architecture
    # adds; there is no speed to be won here, since the driver optimises for the
    # real card either way.
    set(SL_OPTIX_CUDA_ARCH "sm_75" CACHE STRING
            "CUDA architecture of the OptiX kernels: sm_75 runs on every RTX card")

    list(FIND CUDA_NVCC_FLAGS "-arch" index)
    if (index EQUAL -1)
        list(APPEND CUDA_NVCC_FLAGS -arch ${SL_OPTIX_CUDA_ARCH})
        set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} CACHE STRING "Semi-colon delimit multiple arguments." FORCE)
    endif ()

    # nvcc compiles the .cu files with a host compiler, and on Windows it accepts
    # only MSVC's cl.exe: handed anything else it stops with "nvcc fatal : Host
    # compiler targets unsupported OS", which names neither the flag nor the
    # compiler at fault. FindCUDA defaults CUDA_HOST_COMPILER to CMAKE_C_COMPILER,
    # so every configuration that does not already use MSVC -- clang-cl, for
    # instance -- fails on the first .cu file. cl.exe is not on the PATH outside a
    # Visual Studio developer prompt, so find_program cannot see it either, and
    # vswhere is the supported way to locate an installation.
    if (WIN32 AND NOT SL_OPTIX_CUDA_HOST_COMPILER AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        find_program(SL_VSWHERE
                NAMES vswhere
                PATHS "$ENV{ProgramFiles\(x86\)}/Microsoft Visual Studio/Installer"
                NO_DEFAULT_PATH)

        if (SL_VSWHERE)
            execute_process(
                    COMMAND "${SL_VSWHERE}" -latest -products *
                    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64
                    -property installationPath
                    OUTPUT_VARIABLE SL_VS_PATH
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)

            # Newest toolset first, so a machine with several keeps the latest.
            file(GLOB SL_MSVC_DIRS "${SL_VS_PATH}/VC/Tools/MSVC/*")
            list(SORT SL_MSVC_DIRS)
            list(REVERSE SL_MSVC_DIRS)
            foreach (dir ${SL_MSVC_DIRS})
                if (EXISTS "${dir}/bin/Hostx64/x64/cl.exe")
                    set(SL_OPTIX_CUDA_HOST_COMPILER "${dir}/bin/Hostx64/x64/cl.exe"
                            CACHE FILEPATH "MSVC cl.exe that nvcc uses as its host compiler" FORCE)
                    break()
                endif ()
            endforeach ()
        endif ()

        if (NOT SL_OPTIX_CUDA_HOST_COMPILER)
            message(FATAL_ERROR
                    "[OptiX] nvcc needs MSVC's cl.exe as its host compiler and none was "
                    "found. Install the Visual Studio C++ build tools, or set "
                    "SL_OPTIX_CUDA_HOST_COMPILER to the full path of cl.exe, or turn "
                    "SL_BUILD_WITH_OPTIX off.")
        endif ()
    endif ()

    if (SL_OPTIX_CUDA_HOST_COMPILER)
        message(STATUS "[OptiX] nvcc host compiler: ${SL_OPTIX_CUDA_HOST_COMPILER}")
        set(CUDA_HOST_COMPILER "${SL_OPTIX_CUDA_HOST_COMPILER}"
                CACHE FILEPATH "Host side compiler used by NVCC" FORCE)
    endif ()

    message(STATUS "[OptiX] CUDA architecture: ${SL_OPTIX_CUDA_ARCH}")
    set(flag "--use_fast_math")
    list(FIND CUDA_NVCC_FLAGS ${flag} index)
    if (index EQUAL -1)
        list(APPEND CUDA_NVCC_FLAGS ${flag})
        set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} CACHE STRING "Semi-colon delimit multiple arguments." FORCE)
    endif ()

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND CUDA_NVCC_FLAGS -lineinfo)
        set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} CACHE STRING "Semi-colon delimit multiple arguments." FORCE)
    endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} "-DSL_HAS_OPTIX")

    cuda_include_directories(ptx
            ${OptiX_INCLUDE}
            ${CUDA_INCLUDE_DIRS}
            ${SL_PROJECT_ROOT}/modules/sl/cuda
            ${SL_PROJECT_ROOT}/modules/sl/source/optix
            ${CMAKE_CURRENT_SOURCE_DIR}/cuda
            )

    cuda_compile_ptx(ptx
            ${cuda}
            )

    add_custom_target(ptx ALL
            DEPENDS ${cuda}
            SOURCES ${cuda}
            )
endif ()

#==============================================================================
add_library(${target}
        ${sources}
        ${shaders})

add_library(${META_PROJECT_NAME}::${target}
        ALIAS
        ${target}
        )

enable_warnings(${target})

set_target_properties(${target}
        PROPERTIES
        ${DEFAULT_PROJECT_OPTIONS}
        )

target_include_directories(${target}
        PRIVATE

        PUBLIC
        ${platform_specific_include_dirs}
        ${SL_PROJECT_ROOT}/modules/sl/source/accelstruct
        ${SL_PROJECT_ROOT}/modules/sl/source/animation
        ${SL_PROJECT_ROOT}/modules/sl/source/gl
        ${SL_PROJECT_ROOT}/modules/sl/source/input
        ${SL_PROJECT_ROOT}/modules/sl/source/io
        ${SL_PROJECT_ROOT}/modules/sl/source/math
        ${SL_PROJECT_ROOT}/modules/sl/source/mesh
        ${SL_PROJECT_ROOT}/modules/sl/source/node
        ${SL_PROJECT_ROOT}/modules/sl/source/optix
        ${SL_PROJECT_ROOT}/modules/sl/source/ray
        ${SL_PROJECT_ROOT}/modules/sl/source
        ${SL_PROJECT_ROOT}/modules/sl/cuda
        ${SL_PROJECT_ROOT}/modules/math/source
        ${SL_PROJECT_ROOT}/modules/sl/externals/stb
        ${SL_PROJECT_ROOT}/modules/sl/externals/imgui/backends
        ${OptiX_INCLUDE}
        ${CUDA_INCLUDE_DIRS}
        ${SL_PROJECT_ROOT}/apps/source
        ${SL_PROJECT_ROOT}/externals/libigl/include
        ${DEFAULT_INCLUDE_DIRECTORIES}

        INTERFACE
        )

target_compile_definitions(${target}
        PRIVATE

        PUBLIC
        ${compile_definitions}
        ${DEFAULT_COMPILE_DEFINITIONS}

        INTERFACE
        )

target_compile_options(${target}
        PRIVATE

        PUBLIC
        ${DEFAULT_COMPILE_OPTIONS}

        INTERFACE
        )

if (SL_BUILD_WITH_KTX)
    set(PlatformLinkLibs
            ${PlatformLinkLibs}
            ${ktx_LIBS})
endif ()

target_link_libraries(${target}
        PRIVATE

        PUBLIC
        ${PlatformLinkLibs}
        ${assimp_LIBS}
        sl_math
        sl_cv
        sl_utils
        sl_external
        ${DEFAULT_LINKER_OPTIONS}
        INTERFACE

        )

# OptiX needs the CUDA DRIVER API (cuda.lib, CUDA_CUDA_LIBRARY) and not only the
# runtime (cudart, CUDA_LIBRARIES): SLOptix.cpp and SLOptixRaytracer.cpp are
# written against cuCtxCreate, cuMemAlloc and friends, and
# optixDeviceContextCreate takes a CUcontext that only the driver API can
# produce. OptiX itself contributes no import library -- optix_stubs.h resolves
# its entry points from the display driver at runtime, which is why optixInit()
# is where an unsupported driver shows up -- so there is nothing else to add.
#
# The guard repeats the condition of the find_package block at the top of this
# file so that the two cannot drift apart. Without CUDA these variables are
# never defined and used to expand to nothing, which is why they sat in the list
# above unguarded and unnoticed; ${optix_LIBRARY} was there too and is dropped,
# since nothing in the project ever set it.
if (DEFINED ENV{CUDA_PATH} AND SL_BUILD_WITH_OPTIX)
    target_link_libraries(${target}
            PUBLIC
            ${CUDA_LIBRARIES}
            ${CUDA_CUDA_LIBRARY})
endif ()


if (DEFINED ENV{CUDA_PATH} AND SL_BUILD_WITH_OPTIX)
    add_dependencies(${target} ptx)
endif ()

if ("${SYSTEM_NAME_UPPER}" STREQUAL "EMSCRIPTEN")
    target_link_options(${target} PUBLIC "-sFULL_ES3" "-sFETCH")
endif ()