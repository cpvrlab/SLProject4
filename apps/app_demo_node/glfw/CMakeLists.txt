sl_add_app(
    TARGET "app-Demo-Node"
    KIND "GLFW"

    HEADERS
    ${SL_PROJECT_ROOT}/apps/app_demo_node/source/*.h

    SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/AppNodeMainGLFW.cpp
    ${SL_PROJECT_ROOT}/apps/app_demo_node/source/*.cpp

    INCLUDE_DIRECTORIES
    ${SL_PROJECT_ROOT}/apps/app_demo_node/source/
)
