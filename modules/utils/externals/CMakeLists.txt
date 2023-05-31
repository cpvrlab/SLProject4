if (NOT "${SYSTEM_NAME_UPPER}" STREQUAL "IOS" AND NOT "${SYSTEM_NAME_UPPER}" STREQUAL "EMSCRIPTEN")
    add_subdirectory(zlib)
endif ()

if (NOT "${SYSTEM_NAME_UPPER}" STREQUAL "EMSCRIPTEN")
    set(TARGET sl_ftplib)

    add_library(${TARGET} OBJECT
        ${CMAKE_CURRENT_SOURCE_DIR}/ftplibpp/ftplib.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ftplibpp/ftplib.cpp
    )

    target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ftplibpp)
    target_link_libraries(${TARGET} PUBLIC ${openssl_LIBS})
    target_compile_definitions(${TARGET} PRIVATE NOLFS)
endif ()

