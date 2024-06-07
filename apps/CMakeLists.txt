function(sl_add_app)
    # Parse function arguments.
    set(options)
    set(oneValueArgs TARGET KIND)
    set(multiValueArgs HEADERS SOURCES INCLUDE_DIRECTORIES COMPILE_DEFINITIONS)
    cmake_parse_arguments(APP "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    message(STATUS "Adding app: ${APP_TARGET}")

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

    if(APP_KIND STREQUAL "GLFW")
        set(sources ${sources} ${SL_PROJECT_ROOT}/apps/source/AppGLFW.cpp)
    elseif (APP_KIND STREQUAL "EMSCRIPTEN")
        set(headers ${headers} ${SL_PROJECT_ROOT}/apps/source/WebCamera.h)
        set(sources ${sources} ${SL_PROJECT_ROOT}/apps/source/WebCamera.cpp)
    endif()

    add_executable(
        ${APP_TARGET}
        ${headers}
        ${sources}
    )

    enable_warnings(${APP_TARGET})

    if(APP_KIND STREQUAL "GLFW")    
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