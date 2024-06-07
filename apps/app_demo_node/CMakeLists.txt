sl_add_app(
    TARGET "app-Demo-Node"
    PLATFORMS "GLFW" "EMSCRIPTEN"
    HEADERS "${SL_PROJECT_ROOT}/apps/app_demo_node/*.h"
    SOURCES "${SL_PROJECT_ROOT}/apps/app_demo_node/*.cpp"
    INCLUDE_DIRECTORIES "${SL_PROJECT_ROOT}/apps/app_demo_node/"
)
