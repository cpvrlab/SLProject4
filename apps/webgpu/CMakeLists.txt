if (SYSTEM_NAME_UPPER MATCHES "WINDOWS")
    set(WGPU_OS "windows")
elseif (SYSTEM_NAME_UPPER MATCHES "DARWIN")
    set(WGPU_OS "macos")
elseif (SYSTEM_NAME_UPPER MATCHES "LINUX")
    set(WGPU_OS "linux")
else ()
    message(FATAL_ERROR "[WebGPU] System '${SYSTEM_NAME_UPPER}' is not supported")
endif ()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(WGPU_ARCH "x86_64")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    set(WGPU_ARCH "arm64")
else ()
    message(FATAL_ERROR "[WebGPU] Architecture '${CMAKE_SYSTEM_PROCESSOR}' is not supported")
endif ()

set(WGPU_VERSION "v0.18.0.1")
set(WGPU_DIR "${SL_PROJECT_ROOT}/externals/prebuilt/wgpu_${WGPU_VERSION}")
set(WGPU_ZIP "wgpu-${WGPU_OS}-${WGPU_ARCH}-release.zip")

if (NOT EXISTS "${WGPU_DIR}")
    set(BASE_URL "https://github.com/gfx-rs/wgpu-native/releases/download")
    set(DOWNLOAD_URL "${BASE_URL}/${WGPU_VERSION}/${WGPU_ZIP}")

    message(STATUS "[WebGPU] Downloading ${WGPU_ZIP}...")
    file(DOWNLOAD "${DOWNLOAD_URL}" "${WGPU_DIR}/${WGPU_ZIP}")
    message(STATUS "[WebGPU] Download complete")

    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${WGPU_ZIP}" WORKING_DIRECTORY "${WGPU_DIR}")
    message(STATUS "[WebGPU] Files extracted")

    file(REMOVE "${WGPU_DIR}/commit-sha" "${WGPU_DIR}/${WGPU_ZIP}")
    message(STATUS "[WebGPU] Directory cleaned up")
else ()
    message(STATUS "[WebGPU] Directory existing")
endif ()

add_library(wgpu STATIC IMPORTED)

if (SYSTEM_NAME_UPPER MATCHES "WINDOWS")
    target_link_libraries(wgpu INTERFACE "ws2_32" "userenv" "bcrypt" "d3dcompiler" "ntdll" "opengl32")
    set_target_properties(wgpu PROPERTIES
        IMPORTED_LOCATION "${WGPU_DIR}/wgpu_native.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${WGPU_DIR}")
elseif (SYSTEM_NAME_UPPER MATCHES "LINUX")
    set_target_properties(wgpu PROPERTIES
        IMPORTED_LOCATION "${WGPU_DIR}/libwgpu_native.a"
        INTERFACE_INCLUDE_DIRECTORIES "${WGPU_DIR}")
elseif (SYSTEM_NAME_UPPER MATCHES "DARWIN")
    target_link_libraries(wgpu INTERFACE "-framework CoreFoundation" "-framework QuartzCore" "-framework Metal")
    set_target_properties(wgpu PROPERTIES
        IMPORTED_LOCATION "${WGPU_DIR}/libwgpu_native.a"
        INTERFACE_INCLUDE_DIRECTORIES "${WGPU_DIR}")
endif ()

add_executable(webgpu-demo main.cpp)
target_link_libraries(webgpu-demo PRIVATE wgpu sl_cv ${glfw_LIBS})

if (SYSTEM_NAME_UPPER MATCHES "LINUX")
    target_link_libraries(webgpu-demo PRIVATE "X11")
elseif (SYSTEM_NAME_UPPER MATCHES "DARWIN")
    target_link_libraries(webgpu-demo PRIVATE "-framework Cocoa" "-framework IOKit")
endif ()