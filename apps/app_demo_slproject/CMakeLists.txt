if(SYSTEM_NAME_UPPER STREQUAL "ANDROID")
    set(IDE_FOLDER "Android")
    add_subdirectory(android/app)
elseif(SYSTEM_NAME_UPPER STREQUAL "IOS")
	add_subdirectory(iOS)
else()
    sl_add_app(
        TARGET "app-Demo-SLProject"
        
        PLATFORMS
        "GLFW"
        "ANDROID"
        "IOS"
        "EMSCRIPTEN"

        COMPILE_DEFINITIONS
        "SL_STARTSCENE=SID_ShaderIBL"

        HEADERS
        ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/*.h
        ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/scenes/*.h

        SOURCES
        ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/*.cpp
        ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/scenes/*.cpp

        INCLUDE_DIRECTORIES
        ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/
        ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/scenes/
    )
endif()
