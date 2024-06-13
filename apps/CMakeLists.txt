# === Headers, sources, and include directories shared by all platforms ===

file(GLOB COMMON_HEADERS
    ${SL_PROJECT_ROOT}/apps/source/App.h
    ${SL_PROJECT_ROOT}/apps/source/CVCapture.h
    ${SL_PROJECT_ROOT}/apps/source/AppDemo.h
    ${SL_PROJECT_ROOT}/apps/source/AppLoad.h
    ${SL_PROJECT_ROOT}/apps/source/SLScene.h
    ${SL_PROJECT_ROOT}/apps/source/SLInterface.h
    ${SL_PROJECT_ROOT}/apps/source/Scene.h
)

file(GLOB COMMON_SOURCES
    ${SL_PROJECT_ROOT}/apps/source/CVCapture.cpp
    ${SL_PROJECT_ROOT}/apps/source/AppDemo.cpp
    ${SL_PROJECT_ROOT}/apps/source/AppLoad.cpp
    ${SL_PROJECT_ROOT}/apps/source/SLInterface.cpp
    ${SL_PROJECT_ROOT}/apps/source/SLProjectScene.cpp
)

set(COMMON_INCLUDE_DIRECTORIES
    ${SL_PROJECT_ROOT}/apps/source
)

# === GLFW headers, sources, and include directories ===

set(PLATFORM_GLFW_HEADERS
)

set(PLATFORM_GLFW_SOURCES
    ${SL_PROJECT_ROOT}/apps/source/platforms/glfw/AppGLFW.cpp
)

set(PLATFORM_GLFW_INCLUDE_DIRECTORIES
    ${SL_PROJECT_ROOT}/apps/source/platforms/glfw
)

# === Emscripten headers, sources, and include directories ===

set(PLATFORM_EMSCRIPTEN_HEADERS
    ${SL_PROJECT_ROOT}/apps/source/WebCamera.h
)

set(PLATFORM_EMSCRIPTEN_SOURCES
    ${SL_PROJECT_ROOT}/apps/source/WebCamera.cpp
    ${SL_PROJECT_ROOT}/apps/source/platforms/emscripten/AppEmscripten.cpp
)

set(PLATFORM_EMSCRIPTEN_INCLUDE_DIRECTORIES
    ${SL_PROJECT_ROOT}/apps/source/platforms/emscripten
)

# === iOS headers, sources, and include directories ===

set(PLATFORM_IOS_HEADERS
    ${SL_PROJECT_ROOT}/apps/source/platforms/ios/AppDelegate.h
    ${SL_PROJECT_ROOT}/apps/source/platforms/ios/Utils_iOS.h
    ${SL_PROJECT_ROOT}/apps/source/platforms/ios/ViewController.h
)

set(PLATFORM_IOS_SOURCES
    ${SL_PROJECT_ROOT}/apps/source/platforms/ios/AppDelegate.mm
    ${SL_PROJECT_ROOT}/apps/source/platforms/ios/ViewController.mm
    ${SL_PROJECT_ROOT}/apps/source/platforms/ios/AppIOS.mm
)

set(PLATFORM_IOS_INCLUDE_DIRECTORIES
    ${SL_PROJECT_ROOT}/apps/source/platforms/ios
)

function(sl_add_app)
    #
    # Parse CMake function arguments.
    #

    set(options
    )
    
    set(oneValueArgs
        TARGET
        IOS_INFO_PLIST
        IOS_DISPLAY_NAME
        IOS_COPYRIGHT
        IOS_ICON_NAME
    )
    
    set(multiValueArgs
        PLATFORMS
        HEADERS SOURCES
        INCLUDE_DIRECTORIES
        COMPILE_DEFINITIONS
        IOS_RESOURCES
    )
    
    cmake_parse_arguments(APP "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    #
    # Print app information and check platform support.
    #

    message(STATUS "Adding app: ${APP_TARGET}")
    list(JOIN APP_PLATFORMS ", " platformsString)  
    message(STATUS "  Supported platforms: ${platformsString}")  
    
    if(NOT SL_PLATFORM IN_LIST APP_PLATFORMS)
        message(STATUS "  Disabled on this platform")    
        return()
    endif()

    #
    # Collect headers, sources, and include directories.
    #

    file(GLOB HEADERS
        ${COMMON_HEADERS}
        ${PLATFORM_${SL_PLATFORM}_HEADERS}
        ${APP_HEADERS}
    )

    file(GLOB SOURCES
        ${COMMON_SOURCES}
        ${PLATFORM_${SL_PLATFORM}_SOURCES}
        ${APP_SOURCES}
    )

    set(INCLUDE_DIRECTORIES
        ${COMMON_INCLUDE_DIRECTORIES}
        ${PLATFORM_${SL_PLATFORM}_INCLUDE_DIRECTORIES}
        ${APP_INCLUDE_DIRECTORIES}
    )

    #
    # Add the executable target.
    #

    if(SL_PLATFORM STREQUAL "GLFW")
        add_executable(
            ${APP_TARGET}
            ${HEADERS}
            ${SOURCES}
        )
    elseif(SL_PLATFORM STREQUAL "IOS")
        # Copy defined resources to build directory and add get folder
        # reference using file(GLOB) (this is a secret trick that nobody knows).
        include(CopyResourcesAppDemoSLProject)
        set(DATA_DIR "${CMAKE_BINARY_DIR}/data")
        copy_resources_slprojectdemo("${DATA_DIR}")
        file(GLOB DATA "${DATA_DIR}")

        set(RESOURCES
            ${APP_IOS_RESOURCES}
            ${DATA}
        )

        # Group resource files in Xcode's `Resources` directory.
        source_group(Resources FILES ${RESOURCES})

        add_executable(
            ${APP_TARGET}
            MACOSX_BUNDLE
            ${HEADERS}
            ${SOURCES}
            ${RESOURCES}
        )
    endif()

    enable_warnings(${APP_TARGET})

    # 
    # Set target-specific properties.
    #

    if(SL_PLATFORM STREQUAL "GLFW")    
        # Group source files for IDEs
        source_group_by_path("${CMAKE_CURRENT_SOURCE_DIR}" "\\\\.h$|\\\\.hpp$" "Header Files" ${HEADERS})
        source_group_by_path("${CMAKE_CURRENT_SOURCE_DIR}" "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$" "Source Files" ${SOURCES})
    
        set_target_properties(
            ${APP_TARGET}
            PROPERTIES
            ${DEFAULT_PROJECT_OPTIONS}
            FOLDER "apps"
        )
    elseif(SL_PLATFORM STREQUAL "IOS")
        set(BUNDLE_IDENTIFIER "ch.bfh.ti.${APP_TARGET}")
        set(XCODE_CODESIGNIDENTITY "iPhone Developer")
        set(XCODE_DEVELOPMENTTEAM ${XCODE_DEVELOPMENTTEAM})

        message(STATUS "  iOS bundle identifier: ${BUNDLE_IDENTIFIER}")

        # Use the "new" Xcode build system.
        set(CMAKE_XCODE_BUILD_SYSTEM "12")

        # Target both iPhone (1) and iPad (2)
        set(DEVICE_FAMILY "1,2")
        
        # Configure the Info.plist file used by Xcode.
        #
        # This takes the Info.plist template specified by the app, replaces variables starting
        # with `MACOSX_BUNDLE` with their values, and writes the result to a temporary file.
        # The temporary file is specified as the app's Info.plist in the `set_target_properties` call below.
        #
        # We don't use the variables in https://cmake.org/cmake/help/v3.17/prop_tgt/MACOSX_BUNDLE_INFO_PLIST.html
        # because the native Info.plist variable functionality from CMake doesn't support all the variables we want to set.

        set(MACOSX_BUNDLE_DISPLAY_NAME "${APP_IOS_DISPLAY_NAME}")
        set(MACOSX_BUNDLE_EXECUTABLE_NAME "${APP_TARGET}")
        set(MACOSX_BUNDLE_INFO_STRING "${BUNDLE_IDENTIFIER}")
        set(MACOSX_BUNDLE_GUI_IDENTIFIER "${BUNDLE_IDENTIFIER}")
        set(MACOSX_BUNDLE_BUNDLE_NAME "${BUNDLE_IDENTIFIER}")
        set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
        set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
        set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
        set(MACOSX_BUNDLE_COPYRIGHT "${APP_IOS_COPYRIGHT}")
        set(INFO_PLIST_PATH "${CMAKE_CURRENT_BINARY_DIR}/${APP_TARGET}.Info.plist")
        configure_file("${APP_IOS_INFO_PLIST}" "${INFO_PLIST_PATH}")

        set_target_properties(
            ${APP_TARGET}
            PROPERTIES
            ${DEFAULT_PROJECT_OPTIONS}
            FOLDER "apps"

            MACOSX_BUNDLE_INFO_PLIST "${INFO_PLIST_PATH}"
            XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
            XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${SL_PROJECT_ROOT}/apps/source/platforms/ios/Prefix.pch" #this is a precompiled header!
            RESOURCE "${RESOURCES}"
            XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES"
            # XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET}
            XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY ${DEVICE_FAMILY}
            XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
            XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES NO
            XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)"
            XCODE_ATTRIBUTE_ENABLE_TESTABILITY NO
            XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN YES
            XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "${APP_IOS_ICON_NAME}" #defines icon name in asset catalog (images.xcassets)
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "${BUNDLE_IDENTIFIER}"
            # apple requires storyboard launchscreens now so the following will not come into store:
            # XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_LAUNCHIMAGE_NAME "LaunchImage"  #defines launch screen image name in asset catalog (images.xcassets)

            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${XCODE_CODESIGNIDENTITY}
            XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${XCODE_DEVELOPMENTTEAM}
        )
    endif()

    #
    # Add include directories, compile definitions, compile options, and libraries to link.
    #

    target_include_directories(
        ${APP_TARGET}
        PRIVATE
        ${INCLUDE_DIRECTORIES}
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
    )

    if(SL_PLATFORM STREQUAL "GLFW")
        target_link_libraries(${APP_TARGET} PRIVATE ${glfw_LIBS})
    elseif(SL_PLATFORM STREQUAL "IOS")
        # Find system frameworks (libraries).
        find_library(AVFOUNDATION AVFoundation required)
        find_library(COREGRAPHICS CoreGraphics required)
        find_library(COREVIDEO CoreVideo required)
        find_library(COREMOTION CoreMotion required)
        find_library(COREMEDIA CoreMedia required)
        find_library(UIKIT UIKit required)
        find_library(OPENGLES OpenGLES required)
        find_library(GLKIT GLKit required)
        find_library(CORELOCATION CoreLocation required)

        target_link_libraries(
            ${APP_TARGET}
            PRIVATE
            ${AVFOUNDATION}
            ${COREGRAPHICS}
            ${COREVIDEO}
            ${COREMOTION}
            ${COREMEDIA}
            ${UIKIT}
            ${OPENGLES}
            ${GLKIT}
            ${CORELOCATION}    
        )
    endif()

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

add_subdirectory(app_demo_slproject)
add_subdirectory(app_demo_node)

if(NOT SYSTEM_NAME_UPPER MATCHES "^(ANDROID|IOS)$")
    if (SL_BUILD_EXERCISES)
        add_subdirectory(exercises)
    endif()

    add_subdirectory(app_demo_imgui)

    if (SL_BUILD_WEBGPU_DEMO AND NOT ("${SYSTEM_NAME_UPPER}" MATCHES "EMSCRIPTEN"))
        add_subdirectory(app_demo_webgpu)
    endif ()
endif()
