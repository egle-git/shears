# This module sets up SCRAM information
# It defines:
#   SCRAM_FOUND         If scram is found
#   SCRAM_BINARY        Path of the scram executable
#
#   SCRAM_FIND_PACKAGE  A FIND_PACKAGE-like function for scram

# Utility to extract parts of a string using a regex capture
function(_scram_regex_capture _regex _string _variable)
    if("${_string}" MATCHES "${_regex}")
        string(REGEX REPLACE "${_regex}" "\\1" _output "${_string}")
        set("${_variable}" "${_output}" PARENT_SCOPE)
    else()
        set("${_variable}" "" PARENT_SCOPE)
    endif()
endfunction()

find_program(SCRAM_PATH scram)
set(SCRAM_PATH "${SCRAM_PATH}" CACHE FILEPATH "Location of the scram executable")
if("${_scram_path}" STREQUAL "SCRAM_PATH-NOTFOUND")
    set(SCRAM_FOUND FALSE)
    return()
else()
    set(SCRAM_FOUND TRUE)
endif()

# Finds a package using scram
function(SCRAM_FIND_PACKAGE _name _version)
    # Execute scram info and parse its output
    execute_process(COMMAND ${SCRAM_PATH} tool info ${_name} OUTPUT_VARIABLE _scram_output)

    _scram_regex_capture(".*\nVersion : ([^\n]*)\n.*" "${_scram_output}" _scram_version)
    _scram_regex_capture(".*\nINCLUDE=([^\n]*)\n.*" "${_scram_output}" _scram_include)
    _scram_regex_capture(".*\nLIBDIR=([^\n]*)\n.*" "${_scram_output}" _scram_libdir)

    # Check version
    if("${_scram_version}" VERSION_LESS "${_version}")
        message(FATAL_ERROR
                "The scram version of ${_name} (${_scram_version}}) "
                "is too old (${_version} required)")
    else()
        message(STATUS "Found ${_name}: ${_scram_version} (scram)")
    endif()

    # Create the target
    add_library(${_name} SHARED IMPORTED GLOBAL)

    # Set include path
    set_property(TARGET ${_name} APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${_scram_include}")
    set_property(TARGET ${_name}
                 APPEND PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_scram_include}")

    # Set link path
    find_library(_scram_lib_path ${_name} HINTS "${_scram_libdir}")
    set_property(TARGET ${_name} PROPERTY IMPORTED_LOCATION "${_scram_lib_path}")
    unset(_scram_lib_path CACHE)
endfunction()
