
function(mltvrs_configure_project)
    set(OPTIONS          )
    set(ONE_VALUE_ARGS   PREFIX REPORT REPORT_ALL)
    set(MULTI_VALUE_ARGS )
    cmake_parse_arguments(PARSED "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    set(OPTIONS_LIST)
    macro(mltvrs_report_option OPTION_NAME)
        if(PARSED_REPORT OR PARSED_REPORT_ALL)
            if(DEFINED ${PARSED_PREFIX}_${OPTION_NAME} OR PARSED_REPORT_ALL)
                list(APPEND OPTIONS_LIST ${PARSED_PREFIX}_${OPTION_NAME})
            endif()
        endif()
    endmacro()

    if(DEFINED ${PARSED_PREFIX}_STDLIB AND NOT ${PARSED_PREFIX}_STDLIB STREQUAL "default")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=${${PARSED_PREFIX}_STDLIB}")
        else()
            message(FATAL_ERROR "${PARSED_PREFIX}_STDLIB must be \"default\" unless using Clang")
        endif()
    endif()
    mltvrs_report_option(STDLIB)

    if(DEFINED ${PARSED_PREFIX}_STDLIB_PATH AND EXISTS ${${PARSED_PREFIX}_STDLIB_PATH})
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(
                CMAKE_EXE_LINKER_FLAGS
                    "${CMAKE_EXE_LINKER_FLAGS} -L ${${PARSED_PREFIX}_STDLIB_PATH}"
            )
            set(
                CMAKE_EXE_LINKER_FLAGS
                    "${CMAKE_EXE_LINKER_FLAGS} -rpath ${${PARSED_PREFIX}_STDLIB_PATH}"
            )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath -Wl,${${PARSED_PREFIX}_STDLIB_PATH}")
        else()
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_STDLIB_PATH")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "setting the Standard Library link path on this compiler unsupported")
        endif()
    endif()
    mltvrs_report_option(STDLIB_PATH)

    if(DEFINED ${PARSED_PREFIX}_STDLIB_INCLUDE AND EXISTS ${${PARSED_PREFIX}_STDLIB_INCLUDE})
        if(
            CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        )
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${${PARSED_PREFIX}_STDLIB_INCLUDE}")
        else()
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_STDLIB_INCLUDE")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "setting the Standard Library link path on this compiler unsupported")
        endif()
    endif()
    mltvrs_report_option(STDLIB_INCLUDE)

    if(${PARSED_PREFIX}_ENABLE_STATIC_STDLIB)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
        else()
            message(FATAL_ERROR "${PARSED_PREFIX}_ENABLE_STATIC_STDLIB must be \"OFF\" unless using Clang")
        endif()
    endif()
    mltvrs_report_option(ENABLE_STATIC_STDLIB)

    # handle enable-all-warnings
    if(${PARSED_PREFIX}_ENABLE_WALL)
        if(
            CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        )
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
        else()
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_ENABLE_WALL")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "enabling all warning on this compiler unsupported")
        endif()
    endif()
    mltvrs_report_option(ENABLE_WALL)

    # handle treat-warnings-as-errors
    if(${PARSED_PREFIX}_ENABLE_WERROR)
        if(
            CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        )
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
        else()
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_ENABLE_WERROR")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "warnings as errors on this compiler unsupported")
        endif()
    endif()
    mltvrs_report_option(ENABLE_WERROR)

    # handle detailed-concepts-diagnostics
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(${PARSED_PREFIX}_ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts-diagnostics-depth=9")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(${PARSED_PREFIX}_ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=0")
        endif()
    else()
        if(${PARSED_PREFIX}_ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS)
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "concepts diagnostics on this compiler unsupported")
        endif()
    endif()
    mltvrs_report_option(ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS)

    # handle exception disabling
    if(${PARSED_PREFIX}_DISABLE_EXCEPTIONS)
        if(
            CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        )
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
        else()
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_DISABLE_EXCEPTIONS")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "disabling exceptions on this compiler unsupported")
        endif()
    endif()
    mltvrs_report_option(DISABLE_EXCEPTIONS)

    # handle exception disabling
    if(${PARSED_PREFIX}_DISABLE_RTTI)
        if(
            CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
            CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        )
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
        else()
            message(SEND_ERROR  "option  : ${PARSED_PREFIX}_DISABLE_RTTI")
            message(SEND_ERROR  "compiler: ${CMAKE_CXX_COMPILER_ID}")
            message(FATAL_ERROR "disabling RTTI on this compiler unsupported")
        endif()
    endif()
    mltvrs_report_option(DISABLE_RTTI)

    # handle testing
    include(CTest)
    if(ENABLE_TESTING AND ${PARSED_PREFIX}_ENABLE_TESTING)
        find_package(Catch2 REQUIRED)
    endif()
    mltvrs_report_option(ENABLE_TESTING)

    set(
        CMAKE_EXE_LINKER_FLAGS
            "${CMAKE_EXE_LINKER_FLAGS}"
            CACHE
                STRING
                "Flags used by the linker during all build types."
            FORCE
    )
    set(
        CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS}"
            CACHE
                STRING
                "Flags used by the CXX compiler during all build types."
            FORCE
    )

    set(MAX_LENGTH 0)
    foreach(OPTION_NAME ${OPTIONS_LIST})
        string(LENGTH ${OPTION_NAME} LENGTH)
        if(LENGTH GREATER MAX_LENGTH)
            set(MAX_LENGTH ${LENGTH})
        endif()
    endforeach()
    foreach(OPTION_NAME ${OPTIONS_LIST})
        string(LENGTH ${OPTION_NAME} LENGTH)
        math(EXPR PADDING_LENGTH "${MAX_LENGTH} - ${LENGTH}")
        string(REPEAT " " ${PADDING_LENGTH} PADDING)
        message(STATUS "${OPTION_NAME}${PADDING} : ${${OPTION_NAME}}")
    endforeach()
endfunction()
