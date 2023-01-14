
function(mltvrs_configure_project)
    set(OPTIONS          )
    set(ONE_VALUE_ARGS   PREFIX)
    set(MULTI_VALUE_ARGS )
    cmake_parse_arguments(PARSED "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    if(DEFINED ${PARSED_PREFIX}_STDLIB AND NOT ${PARSED_PREFIX}_STDLIB STREQUAL "default")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(
                CMAKE_CXX_FLAGS
                    "${CMAKE_CXX_FLAGS} -stdlib=${${PARSED_PREFIX}_STDLIB}"
                    PARENT_SCOPE
            )
        else()
            message(FATAL_ERROR "${PARSED_PREFIX}_STDLIB must be \"default\" unless using Clang")
        endif()
    endif()

    if(DEFINED ${PARSED_PREFIX}_STDLIB_PATH AND EXISTS ${${PARSED_PREFIX}_STDLIB_PATH})
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(
                CMAKE_EXE_LINKER_FLAGS
                    "${CMAKE_EXE_LINKER_FLAGS} -L ${${PARSED_PREFIX}_STDLIB_PATH}"
                    PARENT_SCOPE
            )
            set(
                CMAKE_EXE_LINKER_FLAGS
                    "${CMAKE_EXE_LINKER_FLAGS} -rpath ${${PARSED_PREFIX}_STDLIB_PATH}"
                    PARENT_SCOPE
            )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set(
                CMAKE_CXX_FLAGS
                    "${CMAKE_CXX_FLAGS} -Wl,-rpath -Wl,${${PARSED_PREFIX}_STDLIB_PATH}"
                    PARENT_SCOPE
            )
        else()
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_STDLIB_PATH")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "setting the Standard Library link path on this compiler unsupported")
        endif()
    endif()

    if(DEFINED ${PARSED_PREFIX}_STDLIB_INCLUDE AND EXISTS ${${PARSED_PREFIX}_STDLIB_INCLUDE})
        if(
            CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        )
            set(
                CMAKE_CXX_FLAGS
                    "${CMAKE_CXX_FLAGS} -isystem ${${PARSED_PREFIX}_STDLIB_INCLUDE}"
                    PARENT_SCOPE
            )
        else()
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_STDLIB_INCLUDE")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "setting the Standard Library link path on this compiler unsupported")
        endif()
    endif()

    if(${PARSED_PREFIX}_ENABLE_STATIC_STDLIB)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(
                CMAKE_CXX_FLAGS
                    "${CMAKE_CXX_FLAGS} -static"
                    PARENT_SCOPE
            )
        else()
            message(FATAL_ERROR "${PARSED_PREFIX}_ENABLE_STATIC_STDLIB must be \"OFF\" unless using Clang")
        endif()
    endif()

    # handle enable-all-warnings
    if(${PARSED_PREFIX}_ENABLE_WALL)
        if(
            CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        )
            set(
                CMAKE_CXX_FLAGS
                    "${CMAKE_CXX_FLAGS} -Wall -Wextra"
                    PARENT_SCOPE
            )
        else()
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_ENABLE_WALL")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "enabling all warning on this compiler unsupported")
        endif()
    endif()

    # handle treat-warnings-as-errors
    if(${PARSED_PREFIX}_ENABLE_WERROR)
        if(
            CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        )
            set(
                CMAKE_CXX_FLAGS
                    "${CMAKE_CXX_FLAGS} -Werror"
                    PARENT_SCOPE
            )
        else()
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_ENABLE_WERROR")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "warnings as errors on this compiler unsupported")
        endif()
    endif()

    # handle detailed-concepts-diagnostics
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(${PARSED_PREFIX}_ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS)
            set(
                CMAKE_CXX_FLAGS
                    "${CMAKE_CXX_FLAGS} -fconcepts-diagnostics-depth=9"
                    PARENT_SCOPE
            )
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(${PARSED_PREFIX}_ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS)
            set(
                CMAKE_CXX_FLAGS
                    "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=0"
                    PARENT_SCOPE
            )
        endif()
    else()
        if(${PARSED_PREFIX}_ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS)
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "concepts diagnostics on this compiler unsupported")
        endif()
    endif()

    # handle exception disabling
    if(${PARSED_PREFIX}_DISABLE_EXCEPTIONS)
        if(
            CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        )
            set(
                CMAKE_CXX_FLAGS
                    "${CMAKE_CXX_FLAGS} -fno-exceptions"
                    PARENT_SCOPE
            )
        else()
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_DISABLE_EXCEPTIONS")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "disabling exceptions on this compiler unsupported")
        endif()
    endif()

    # handle exception disabling
    if(${PARSED_PREFIX}_DISABLE_RTTI)
        if(
            CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        )
            set(
                CMAKE_CXX_FLAGS
                    "${CMAKE_CXX_FLAGS} -fno-rtti"
                    PARENT_SCOPE
            )
        else()
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_DISABLE_RTTI")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "disabling RTTI on this compiler unsupported")
        endif()
    endif()
endfunction()

function(mltvrs_report_project_configurations)
    set(OPTIONS          )
    set(ONE_VALUE_ARGS   PREFIX)
    set(MULTI_VALUE_ARGS )
    cmake_parse_arguments(PARSED "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    message(STATUS "${PARSED_PREFIX}_STDLIB                               : ${${PARSED_PREFIX}_STDLIB}")
    message(STATUS "${PARSED_PREFIX}_STDLIB_PATH                          : ${${PARSED_PREFIX}_STDLIB_PATH}")
    message(STATUS "${PARSED_PREFIX}_STDLIB_INCLUDE                       : ${${PARSED_PREFIX}_STDLIB_INCLUDE}")
    message(STATUS "${PARSED_PREFIX}_ENABLE_STATIC_STDLIB                 : ${${PARSED_PREFIX}_ENABLE_STATIC_STDLIB}")
    message(STATUS "${PARSED_PREFIX}_ENABLE_WALL                          : ${${PARSED_PREFIX}_ENABLE_WALL}")
    message(STATUS "${PARSED_PREFIX}_ENABLE_WERROR                        : ${${PARSED_PREFIX}_ENABLE_WERROR}")
    message(STATUS "${PARSED_PREFIX}_ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS : ${${PARSED_PREFIX}_ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS}")
    message(STATUS "${PARSED_PREFIX}_DISABLE_EXCEPTIONS                   : ${${PARSED_PREFIX}_DISABLE_EXCEPTIONS}")
    message(STATUS "${PARSED_PREFIX}_DISABLE_RTTI                         : ${${PARSED_PREFIX}_DISABLE_RTTI}")
endfunction()
