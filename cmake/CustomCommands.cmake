#
# Custom cmake commands used in other cmake files
#

#------------------------------------------------------------------------------
# Set policy if policy is available
function(set_policy POL VAL)
    if(POLICY ${POL})
        cmake_policy(SET ${POL} ${VAL})
    endif()
endfunction(set_policy)
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Define function "source_group_by_path with three mandatory arguments
# (PARENT_PATH, REGEX, GROUP, ...) to group source files in folders
# (e.g. for MSVC solutions).
#
# Example: source_group_by_path("${CMAKE_CURRENT_SOURCE_DIR}/src" "\\\\.h$|\\\\.inl$|\\\\.cpp$|\\\\.c$|\\\\.ui$|\\\\.qrc$" "Source Files" ${sources})
function(source_group_by_path PARENT_PATH REGEX GROUP)

    foreach (FILENAME ${ARGN})
        
        get_filename_component(FILEPATH "${FILENAME}" REALPATH)
        file(RELATIVE_PATH FILEPATH ${PARENT_PATH} ${FILEPATH})
        get_filename_component(FILEPATH "${FILEPATH}" DIRECTORY)

        string(REPLACE "/" "\\" FILEPATH "${FILEPATH}")

	source_group("${GROUP}\\${FILEPATH}" REGULAR_EXPRESSION "${REGEX}" FILES ${FILENAME})

    endforeach()

endfunction(source_group_by_path)
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Function that extract entries matching a given regex from a list.
# ${OUTPUT} will store the list of matching filenames.
function(list_extract OUTPUT REGEX)

    foreach(FILENAME ${ARGN})
        if(${FILENAME} MATCHES "${REGEX}")
            list(APPEND ${OUTPUT} ${FILENAME})
        endif()
    endforeach()

    set(${OUTPUT} ${${OUTPUT}} PARENT_SCOPE)

endfunction(list_extract)
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
function(add_3rdparty_library name file)
    #message(STATUS "adding 3rdpary library with name ${name} at location ${file}")
    add_library(${name} SHARED IMPORTED)
    set_target_properties(${name} PROPERTIES IMPORTED_LOCATION ${file})
endfunction()
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Enable warnings for a target.
function(enable_warnings TARGET)
    if (MSVC)
        target_compile_options(${TARGET} PRIVATE /W3)
    else ()
        target_compile_options(${TARGET} PRIVATE -Wall)
    endif ()
endfunction()
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# This little macro lets you set any XCode specific property
macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro (set_xcode_property)
#------------------------------------------------------------------------------
