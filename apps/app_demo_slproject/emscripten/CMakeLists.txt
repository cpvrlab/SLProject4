sl_add_app(
    TARGET "app-Demo-SLProject"
    KIND "EMSCRIPTEN"

    COMPILE_DEFINITIONS
    "SL_STARTSCENE=SID_ShaderIBL"

    HEADERS
    ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/*.h
    ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/scenes/*.h

    SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/AppDemoMainEmscripten.cpp
    ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/*.cpp
    ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/scenes/*.cpp

    INCLUDE_DIRECTORIES
    ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/
    ${SL_PROJECT_ROOT}/apps/app_demo_slproject/source/scenes/
)

# Copy index.html and server script to output directory
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/index.html" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/app-Demo-SLProject.html" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/server.py" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/server.py" COPYONLY)
