if (NOT "${SYSTEM_NAME_UPPER}" STREQUAL "IOS" AND NOT "${SYSTEM_NAME_UPPER}" STREQUAL "EMSCRIPTEN")
    add_subdirectory(zlib)

    set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-covered-switch-default
            -Wno-double-promotion
            -Wno-exit-time-destructors
            -Wno-global-constructors
            -Wno-gnu-zero-variadic-macro-arguments
            -Wno-documentation
            -Wno-missing-variable-declarations
            -Wno-newline-eof
            -Wno-old-style-cast
            -Wno-switch-enum
            -Wno-unused-but-set-variable
            -Wno-unused-command-line-argument
            -Wno-unused-function
            -Wno-unused-macros
            -Wno-unused-parameter
            -Wno-unused-private-field
            -Wno-unused-variable
            -Wno-unused-value
            -Wno-used-but-marked-unused
            -Wno-extra-tokens
            -Wno-reorder
            -Wno-switch
            -Wno-char-subscripts
            -Wno-injected-class-name
            -Wno-format-security
            -Wno-invalid-noreturn

            $<$<CXX_COMPILER_ID:GNU>:
            >

            $<$<CXX_COMPILER_ID:Clang>:

            >

            $<$<PLATFORM_ID:Darwin>:
            -pthread
            >
    )
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

