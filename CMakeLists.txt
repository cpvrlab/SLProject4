#
# CMake top-level configuration of SLProject
#

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
    set(CMAKE_MACOSX_RPATH OFF)
endif (APPLE)

set(SL_PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SL_PROJECT_ROOT}/cmake/")

# Register general cmake commands (set_policy, source_group_by_path, list_extract)
include(cmake/CustomCommands.cmake) 

# The FOLDER properties are needed in Visual Studio and XCode generated projects for nested folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "")

# set_policy(CMP0111 OLD) # Avoids warnings for new policy CMP0111 from cmake 3.19 onwards

set(META_PROJECT_NAME "SLProject")

# Declare project
project(${META_PROJECT_NAME} C CXX)

# Set output directories
if (WIN32 AND NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
else ()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
endif ()

include(cmake/SetGitBranchNameAndCommitID.cmake)
include(cmake/CompileOptions.cmake)

# Determine the platform that apps will run on.
# On desktop operating systems, the platform is GLFW.
# On Android, iOS, and Emscripten, the platform name is just directly the system name.
if(SYSTEM_NAME_UPPER MATCHES "^(WINDOWS|DARWIN|LINUX)$")
	set(SL_PLATFORM "GLFW")
elseif(SYSTEM_NAME_UPPER STREQUAL "ANDROID")
	set(SL_PLATFORM "ANDROID")
elseif(SYSTEM_NAME_UPPER STREQUAL "IOS")
	set(SL_PLATFORM "IOS")
elseif(SYSTEM_NAME_UPPER STREQUAL "EMSCRIPTEN")
	set(SL_PLATFORM "EMSCRIPTEN")
else()
	set(SL_PLATFORM "UNKNOWN")
endif()

option(SL_DOWNLOAD_DATA "Specifies if the data ZIP file should be downloaded" ON)
option(SL_DOWNLOAD_PREBUILTS "Specifies if prebuilt libraries should be downloaded" ON)
option(SL_BUILD_WAI "Specifies if the WAI library should be built" ON)
option(SL_BUILD_APPS "Specifies if sample apps should be built" ON)
option(SL_BUILD_EXERCISES "Specifies if exercise apps should be built" ON)
option(SL_BUILD_VULKAN_APPS "Specifies if vulkan apps should be built" OFF)
option(SL_BUILD_WEBGPU_DEMO "Specifies if WebGPU demo should be built" ON)
option(SL_BUILD_WITH_OPTIX "Specifies if Optix renderer should be built" OFF)
option(SL_BUILD_WITH_KTX "Specifies if Kronos Texture library (ktx) should be used" ON)
option(SL_BUILD_WITH_OPENSSL "Specifies if OpenSSL should be used" ON)
option(SL_BUILD_WITH_ASSIMP "Specifies if Assimp should be used" ON)
option(SL_BUILD_WITH_MEDIAPIPE "Specifies if MediaPipe should be used" ON)

option(LIBIGL_USE_STATIC_LIBRARY "Specifies if LibIGL should be built statically" ON)

if(SYSTEM_NAME_UPPER STREQUAL "IOS")
    set(SL_BUILD_WITH_MEDIAPIPE OFF)
elseif(SYSTEM_NAME_UPPER STREQUAL "EMSCRIPTEN")
    set(SL_BUILD_WAI OFF)
    set(SL_BUILD_WITH_OPENSSL OFF)
    set(SL_BUILD_WITH_MEDIAPIPE OFF)
endif()

message(STATUS "----------------------------------------------------------------")
message(STATUS "System: ${SYSTEM_NAME_UPPER}")
message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Platform: ${SL_PLATFORM}")
message(STATUS "----------------------------------------------------------------")
message(STATUS "SL_DOWNLOAD_PREBUILTS: ${SL_DOWNLOAD_PREBUILTS}")
message(STATUS "SL_BUILD_WAI: ${SL_BUILD_WAI}")
message(STATUS "SL_BUILD_APPS: ${SL_BUILD_APPS}")
message(STATUS "SL_BUILD_EXERCISES: ${SL_BUILD_EXERCISES}")
message(STATUS "SL_BUILD_VULKAN_APPS: ${SL_BUILD_VULKAN_APPS}")
message(STATUS "SL_BUILD_WEBGPU_DEMO: ${SL_BUILD_WEBGPU_DEMO}")
message(STATUS "SL_BUILD_WITH_OPTIX: ${SL_BUILD_WITH_OPTIX}")
message(STATUS "SL_BUILD_WITH_KTX: ${SL_BUILD_WITH_KTX}")
message(STATUS "SL_BUILD_WITH_OPENSSL: ${SL_BUILD_WITH_OPENSSL}")
message(STATUS "SL_BUILD_WITH_ASSIMP: ${SL_BUILD_WITH_ASSIMP}")
message(STATUS "SL_BUILD_WITH_MEDIAPIPE: ${SL_BUILD_WITH_MEDIAPIPE}")
message(STATUS "LIBIGL_USE_STATIC_LIBRARY: ${LIBIGL_USE_STATIC_LIBRARY}")
message(STATUS "----------------------------------------------------------------")

if (SL_DOWNLOAD_DATA)
    include(cmake/DownloadData.cmake)
    message(STATUS "----------------------------------------------------------------")
endif ()

if (SL_DOWNLOAD_PREBUILTS)
    include(cmake/DownloadPrebuilts.cmake)
    message(STATUS "Finished downloading prebuilts")
    message(STATUS "----------------------------------------------------------------")
endif ()

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Android")
    message(STATUS "SL_APP: ${SL_APP}")
    option(SL_APP
            "Specifies which sample app should be built (used for android)"
            SL_APP_ALL)
endif ()

add_subdirectory(modules/sens)
add_subdirectory(modules/sl)
add_subdirectory(modules/math)
add_subdirectory(modules/utils)
add_subdirectory(modules/cv)

if (SL_BUILD_WAI)
    add_subdirectory(modules/wai)
endif ()

if (SL_BUILD_APPS)
    add_subdirectory(apps)
endif ()

message(STATUS "----------------------------------------------------------------")
