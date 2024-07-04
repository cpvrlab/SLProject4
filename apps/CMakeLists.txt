function(sl_add_app)
    # -------------------------------------------------------------------------
    # Parse CMake function arguments.
    # -------------------------------------------------------------------------
    
    set(oneValueArgs
        TARGET
        ANDROID_APP_DIR
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
    
    cmake_parse_arguments(APP "" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    # -------------------------------------------------------------------------
    # Print app information and check platform support.
    # -------------------------------------------------------------------------

    message(STATUS "Adding app: ${APP_TARGET}")
    list(JOIN APP_PLATFORMS ", " platformsString)  
    message(STATUS "  Supported platforms: ${platformsString}")  
    
    if(NOT SL_PLATFORM IN_LIST APP_PLATFORMS)
        message(STATUS "  Disabled on this platform")    
        return()
    endif()

    # -------------------------------------------------------------------------
    # Define headers and sources shared by all platforms.
    # This includes the headers and sources for the current app.
    # -------------------------------------------------------------------------

    file(GLOB COMMON_HEADERS
        ${SL_PROJECT_ROOT}/apps/source/App.h
        ${SL_PROJECT_ROOT}/apps/source/CVCapture.h
        ${SL_PROJECT_ROOT}/apps/source/AppCommon.h
        ${SL_PROJECT_ROOT}/apps/source/SLScene.h
        ${SL_PROJECT_ROOT}/apps/source/SLInterface.h
        ${SL_PROJECT_ROOT}/apps/source/Scene.h
        ${APP_HEADERS}
    )

    file(GLOB COMMON_SOURCES
        ${SL_PROJECT_ROOT}/apps/source/CVCapture.cpp
        ${SL_PROJECT_ROOT}/apps/source/AppCommon.cpp
        ${SL_PROJECT_ROOT}/apps/source/SLInterface.cpp
        ${SL_PROJECT_ROOT}/apps/source/SLProjectScene.cpp
        ${APP_SOURCES}
    )

    # -------------------------------------------------------------------------
    # Create the target and configure platform-specific things like libraries
    # to link or assets to copy on mobile platforms.
    # -------------------------------------------------------------------------
 
    if(SL_PLATFORM STREQUAL "GLFW")
        # ---------------------------------------------------------------------
        # Configuration for platform GLFW
        # ---------------------------------------------------------------------

        file(GLOB HEADERS
            ${COMMON_HEADERS}
        )

        file(GLOB SOURCES
            ${COMMON_SOURCES}
            ${SL_PROJECT_ROOT}/apps/source/platforms/glfw/AppGLFW.cpp
        )

        add_executable(
            ${APP_TARGET}
            ${HEADERS}
            ${SOURCES}
        )

        # Group source files for IDEs
        source_group_by_path("${CMAKE_CURRENT_SOURCE_DIR}" "\\\\.h$|\\\\.hpp$" "Header Files" ${HEADERS})
        source_group_by_path("${CMAKE_CURRENT_SOURCE_DIR}" "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$" "Source Files" ${SOURCES})

        set_target_properties(
            ${APP_TARGET}
            PROPERTIES
            ${DEFAULT_PROJECT_OPTIONS}
            FOLDER "apps"
        )

        target_include_directories(
            ${APP_TARGET}
            PRIVATE
            ${SL_PROJECT_ROOT}/apps/source/platforms/glfw
        )

        target_link_libraries(
            ${APP_TARGET}
            PRIVATE
            ${glfw_LIBS}
        )
    elseif(SL_PLATFORM STREQUAL "EMSCRIPTEN")
        # ---------------------------------------------------------------------
        # Configuration for platform Emscripten
        # ---------------------------------------------------------------------
        
        file(GLOB HEADERS
            ${COMMON_HEADERS}
            ${SL_PROJECT_ROOT}/apps/source/WebCamera.h
        )

        file(GLOB SOURCES
            ${COMMON_SOURCES}
            ${SL_PROJECT_ROOT}/apps/source/WebCamera.cpp
            ${SL_PROJECT_ROOT}/apps/source/platforms/emscripten/AppEmscripten.cpp
        )

        add_executable(
            ${APP_TARGET}
            ${HEADERS}
            ${SOURCES}
        )
    
        target_include_directories(
            ${APP_TARGET}
            PRIVATE
            ${SL_PROJECT_ROOT}/apps/source/platforms/emscripten
        )

        # Copy the HTML page and the server script to the target directory and replace
        # the string "TARGET" in them with the name of the target.
        # If you change these files, you have to re-run CMake.

        file(READ "${SL_PROJECT_ROOT}/apps/source/platforms/emscripten/index.html" INDEX_SOURCE)
        string(REPLACE "TARGET" "${APP_TARGET}" INDEX_SOURCE "${INDEX_SOURCE}")
        file(WRITE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${APP_TARGET}.html" "${INDEX_SOURCE}")

        file(READ "${SL_PROJECT_ROOT}/apps/source/platforms/emscripten/server.py" SERVER_SOURCE)
        string(REPLACE "TARGET" "${APP_TARGET}" SERVER_SOURCE "${SERVER_SOURCE}")
        file(WRITE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/serve-${APP_TARGET}.py" "${SERVER_SOURCE}")
    elseif(SL_PLATFORM STREQUAL "ANDROID")
        # ---------------------------------------------------------------------
        # Configuration for platform Android
        # For more information about using CMake with Android Studio, read the
        # documentation: https://d.android.com/studio/projects/add-native-code.html
        # ---------------------------------------------------------------------

        file(GLOB HEADERS
            ${COMMON_HEADERS}
        )
        
        file(GLOB SOURCES
            ${COMMON_SOURCES}
            ${SL_PROJECT_ROOT}/apps/source/platforms/android/AppAndroid.cpp
        )

        # Copy APK contents.
        include(CopyResourcesAppDemoSLProject)
        copy_resources_slprojectdemo("${APP_ANDROID_APP_DIR}/src/main/assets/data")
        
        file(GLOB_RECURSE
            MEDIAPIPE_ASSETS
            ${SL_PROJECT_ROOT}/data/mediapipe/*.tflite
            ${SL_PROJECT_ROOT}/data/mediapipe/*.txt
        )
        
        foreach (ASSET_PATH ${MEDIAPIPE_ASSETS})
            get_filename_component(ASSET_FILENAME "${ASSET_PATH}" NAME)
            file(COPY "${ASSET_PATH}" DESTINATION "${APP_ANDROID_APP_DIR}/src/main/assets")
        endforeach ()
        
        add_library(
            ${APP_TARGET}
            SHARED
            ${HEADERS}
            ${SOURCES}
        )
                
        target_link_libraries(
            ${APP_TARGET}
            PRIVATE
            libc++_shared.so
            z
        )
    elseif(SL_PLATFORM STREQUAL "IOS")
        # ---------------------------------------------------------------------
        # Configuration for platform iOS
        # ---------------------------------------------------------------------
        
        file(GLOB HEADERS
            ${COMMON_HEADERS}
            ${SL_PROJECT_ROOT}/apps/source/platforms/ios/AppDelegate.h
            ${SL_PROJECT_ROOT}/apps/source/platforms/ios/Utils_iOS.h
            ${SL_PROJECT_ROOT}/apps/source/platforms/ios/ViewController.h
        )

        file(GLOB SOURCES
            ${COMMON_SOURCES}
            ${SL_PROJECT_ROOT}/apps/source/platforms/ios/AppDelegate.mm
            ${SL_PROJECT_ROOT}/apps/source/platforms/ios/ViewController.mm
            ${SL_PROJECT_ROOT}/apps/source/platforms/ios/AppIOS.mm
        )

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

        set(BUNDLE_IDENTIFIER "ch.bfh.ti.cpvrlab.${APP_TARGET}")
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

        target_include_directories(
            ${APP_TARGET}
            PRIVATE
            ${SL_PROJECT_ROOT}/apps/source/platforms/ios
        )

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

    # -------------------------------------------------------------------------
    # Set target configuration shared by all platforms.
    # -------------------------------------------------------------------------

    enable_warnings(${APP_TARGET})

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
    )
endfunction()

add_subdirectory(app_demo)
add_subdirectory(app_node)
add_subdirectory(app_minimal)

if(NOT SYSTEM_NAME_UPPER MATCHES "^(ANDROID|IOS)$")
    if (SL_BUILD_EXERCISES)
        add_subdirectory(exercises)
    endif()

    add_subdirectory(app_imgui)

    if (SL_BUILD_WEBGPU_DEMO AND NOT ("${SYSTEM_NAME_UPPER}" MATCHES "EMSCRIPTEN"))
        add_subdirectory(app_webgpu)
    endif ()
endif()
