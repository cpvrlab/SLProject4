#
# CMake project definition for exercise cv02_ClarendonFilter
#

sl_add_exercise(cv02_ClarendonFilter
    HEADERS
    SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/cv02_ClarendonFilter.cpp
    SHADERS
    LIBRARIES
    sl_cv
    ${glfw_LIBS}
    )
