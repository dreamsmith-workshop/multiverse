cmake_minimum_required(VERSION 3.19)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/project-config.cmake)

#########################
# Configuration Options #
#########################

project(multiverse LANGUAGES CXX VERSION 0.0.1)

option(MLTVRS_ENABLE_WALL                          OFF)
option(MLTVRS_ENABLE_WERROR                        OFF)
option(MLTVRS_ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS OFF)
option(MLTVRS_ENABLE_TESTING                       OFF)
option(MLTVRS_ENABLE_STATIC_STDLIB                 OFF)
option(MLTVRS_ENABLE_CONAN_OVERRIDE                OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

mltvrs_configure_project(PREFIX MLTVRS REPORT_ALL TRUE)

##################
# Create Targets #
##################

set(MLTVRS_HPP ${CMAKE_CURRENT_LIST_DIR}/mltvrs/multiverse.hpp)
set(MLTVRS_IPP )
set(MLTVRS_CPP )

add_library(mltvrs INTERFACE ${MLTVRS_HPP} ${MLTVRS_IPP} ${MLTVRS_CPP})
target_include_directories(
    mltvrs
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/mltvrs>
)
add_library(mltvrs::mltvrs ALIAS mltvrs)

#########################
# Create Testing Target #
#########################

if(ENABLE_TESTING AND MLTVRS_ENABLE_TESTING)
    add_library(mltvrs_test_main ${CMAKE_CURRENT_LIST_DIR}/mltvrs/test/test_main.cpp)
    target_compile_definitions(mltvrs_test_main PUBLIC CATCH_CONFIG_PREFIX_ALL)
    target_link_libraries(mltvrs_test_main PUBLIC Catch2::Catch2)
    add_library(mltvrs::mltvrs_test_main ALIAS mltvrs_test_main)
endif()

################
# Installation #
################

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    include(CMakePackageConfigHelpers)

    set(MLTVRS_CMAKE_CONFIG_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mltvrs)

    install(TARGETS mltvrs EXPORT mltvrsTargets DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(
        EXPORT
            mltvrsTargets
        NAMESPACE
            mltvrs::
        DESTINATION
            ${MLTVRS_CMAKE_CONFIG_DESTINATION}
    )
    install(
        DIRECTORY
            ${CMAKE_CURRENT_LIST_DIR}/mltvrs
        DESTINATION
            "${CMAKE_INSTALL_INCLUDEDIR}"
        REGEX
            ".+\.cpp"
            EXCLUDE
        REGEX
            "test"
            EXCLUDE
    )
    install(
        DIRECTORY
            ${CMAKE_CURRENT_LIST_DIR}/cmake
        DESTINATION
            ${MLTVRS_CMAKE_CONFIG_DESTINATION}
        REGEX
            ".+\.in\..+"
            EXCLUDE
    )

    configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/mltvrsConfig.in.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/mltvrsConfig.cmake
        INSTALL_DESTINATION
            ${MLTVRS_CMAKE_CONFIG_DESTINATION}
    )
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/mltvrsConfigVersion.cmake
        COMPATIBILITY
            SameMajorVersion
    )
    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/mltvrsConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/mltvrsConfigVersion.cmake
        DESTINATION
            ${MLTVRS_CMAKE_CONFIG_DESTINATION}
    )

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/mltvrs.in.pc
        ${CMAKE_CURRENT_BINARY_DIR}/mltvrs.pc
        @ONLY
    )
    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/mltvrs.pc
        DESTINATION
            ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig
    )

    set(CPACK_PACKAGE_CONTACT "https://github.com/dreamsmith-workshop")
    include(CPack)
endif()
