function(sl_add_app)
    # Parse function arguments.
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs PLATFORMS HEADERS SOURCES INCLUDE_DIRECTORIES COMPILE_DEFINITIONS)
    cmake_parse_arguments(APP "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    message(STATUS "Adding app: ${APP_TARGET}")
    list(JOIN APP_PLATFORMS ", " platformsString)  
    message(STATUS "  Platforms: ${platformsString}")  
    
    if(NOT SL_PLATFORM IN_LIST APP_PLATFORMS)
        message(STATUS "  Disabled on this platform")    
        return()
    endif()

    file(GLOB headers
        ${SL_PROJECT_ROOT}/apps/source/App.h
        ${SL_PROJECT_ROOT}/apps/source/CVCapture.h
        ${SL_PROJECT_ROOT}/apps/source/AppDemo.h
        ${SL_PROJECT_ROOT}/apps/source/AppLoad.h
        ${SL_PROJECT_ROOT}/apps/source/SLScene.h
        ${SL_PROJECT_ROOT}/apps/source/SLInterface.h
        ${SL_PROJECT_ROOT}/apps/source/Scene.h
        ${APP_HEADERS}
    )

    file(GLOB sources
        ${SL_PROJECT_ROOT}/apps/source/CVCapture.cpp
        ${SL_PROJECT_ROOT}/apps/source/AppDemo.cpp
        ${SL_PROJECT_ROOT}/apps/source/AppLoad.cpp
        ${SL_PROJECT_ROOT}/apps/source/SLInterface.cpp
        ${SL_PROJECT_ROOT}/apps/source/SLProjectScene.cpp
        ${APP_SOURCES}
    )

    if(SL_PLATFORM STREQUAL "GLFW")
        set(sources
            ${sources}
            ${SL_PROJECT_ROOT}/apps/source/platforms/glfw/AppGLFW.cpp
        )
    elseif(SL_PLATFORM STREQUAL "EMSCRIPTEN")
        set(headers
            ${headers}
            ${SL_PROJECT_ROOT}/apps/source/WebCamera.h
        )
        
        set(sources
            ${sources}
            ${SL_PROJECT_ROOT}/apps/source/WebCamera.cpp
            ${SL_PROJECT_ROOT}/apps/source/platforms/emscripten/AppEmscripten.cpp
        )
    endif()

    add_executable(
        ${APP_TARGET}
        ${headers}
        ${sources}
    )

    enable_warnings(${APP_TARGET})

    if(SL_PLATFORM STREQUAL "GLFW")    
        # Group source files for IDEs
        source_group_by_path("${CMAKE_CURRENT_SOURCE_DIR}" "\\\\.h$|\\\\.hpp$" "Header Files" ${headers})
        source_group_by_path("${CMAKE_CURRENT_SOURCE_DIR}" "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$" "Source Files" ${sources})
    
        set_target_properties(
            ${APP_TARGET}
            PROPERTIES
            ${DEFAULT_PROJECT_OPTIONS}
            FOLDER "apps"
        )  
    endif()

    target_include_directories(
        ${APP_TARGET}
        PRIVATE
        ${SL_PROJECT_ROOT}/apps/source
        ${APP_INCLUDE_DIRECTORIES}
    )
    
    target_compile_definitions(
        ${APP_TARGET}
        PRIVATE
        ${DEFAULT_COMPILE_DEFINITIONS}
        ${APP_COMPILE_DEFINITIONS}
    )
    
    target_compile_options(
        ${APP_TARGET}
        PRIVATE
        ${DEFAULT_COMPILE_OPTIONS}
    )
    
    target_link_libraries(
        ${APP_TARGET}
        PRIVATE
        ${DEFAULT_LINKER_OPTIONS}
        ${PlatformLinkLibs}
        sl
        sl_external
        sl_utils
        ${OpenCV_LIBS}
        ${glfw_LIBS}
    )

    if (SL_PLATFORM STREQUAL "EMSCRIPTEN")
        # Copy the HTML page and the server script to the target directory and replace
        # the string "TARGET" in them with the name of the target.
        # If you change these files, you have to re-run CMake.

        file(READ "${SL_PROJECT_ROOT}/apps/source/platforms/emscripten/index.html" INDEX_SOURCE)
        string(REPLACE "TARGET" "${APP_TARGET}" INDEX_SOURCE "${INDEX_SOURCE}")
        file(WRITE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${APP_TARGET}.html" "${INDEX_SOURCE}")

        file(READ "${SL_PROJECT_ROOT}/apps/source/platforms/emscripten/server.py" SERVER_SOURCE)
        string(REPLACE "TARGET" "${APP_TARGET}" SERVER_SOURCE "${SERVER_SOURCE}")
        file(WRITE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/serve-${APP_TARGET}.py" "${SERVER_SOURCE}")
    endif ()
endfunction()

if("${SYSTEM_NAME_UPPER}" MATCHES "ANDROID")
    add_subdirectory(app_demo_slproject)
elseif("${SYSTEM_NAME_UPPER}" MATCHES "IOS")
	add_subdirectory(app_demo_slproject)
else()
    if (SL_BUILD_EXERCISES)
        add_subdirectory(exercises)
    endif()
        add_subdirectory(app_demo_imgui)
        add_subdirectory(app_demo_node)
        add_subdirectory(app_demo_slproject)

    if (SL_BUILD_WEBGPU_DEMO AND NOT ("${SYSTEM_NAME_UPPER}" MATCHES "EMSCRIPTEN"))
        add_subdirectory(app_demo_webgpu)
    endif ()
endif()