sl_add_app(
    TARGET "app-Demo-SLProject"
    KIND "GLFW"

    COMPILE_DEFINITIONS
    "SL_STARTSCENE=SID_ShaderIBL"

    HEADERS
    ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/*.h
    ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/scenes/*.h

    SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/AppDemoMainGLFW.cpp
    ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/*.cpp
    ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/scenes/*.cpp

    INCLUDE_DIRECTORIES
    ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/
    ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/scenes/
)
