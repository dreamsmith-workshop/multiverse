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

#####################
# Find Dependencies #
#####################

find_package(Microsoft.GSL REQUIRED)

##################
# Create Targets #
##################

set(
    MLTVRS_HPP
        ${CMAKE_CURRENT_LIST_DIR}/mltvrs/chrono.hpp
        ${CMAKE_CURRENT_LIST_DIR}/mltvrs/generator.hpp
        ${CMAKE_CURRENT_LIST_DIR}/mltvrs/interval.hpp
        ${CMAKE_CURRENT_LIST_DIR}/mltvrs/multiverse.hpp
        ${CMAKE_CURRENT_LIST_DIR}/mltvrs/ranges.hpp
        ${CMAKE_CURRENT_LIST_DIR}/mltvrs/string_literal.hpp
)
set(
    MLTVRS_IPP
        ${CMAKE_CURRENT_LIST_DIR}/mltvrs/ipp/generator.ipp
        ${CMAKE_CURRENT_LIST_DIR}/mltvrs/ipp/interval.ipp
        ${CMAKE_CURRENT_LIST_DIR}/mltvrs/ipp/string_literal.ipp
)
set(MLTVRS_CPP)

add_library(mltvrs INTERFACE ${MLTVRS_HPP} ${MLTVRS_IPP} ${MLTVRS_CPP})
target_include_directories(
    mltvrs
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/mltvrs>
)
target_link_libraries(mltvrs INTERFACE Microsoft.GSL::GSL)
add_library(mltvrs::mltvrs ALIAS mltvrs)

#########################
# Create Testing Target #
#########################

include(CTest)
if(ENABLE_TESTING AND MLTVRS_ENABLE_TESTING)
    find_package(Catch2 3.3.0 REQUIRED)
    add_library(mltvrs::test_main ALIAS Catch2::Catch2WithMain)
    
    add_executable(mltvrs_test_generator ${CMAKE_CURRENT_LIST_DIR}/mltvrs/test/test_generator.cpp)
    target_link_libraries(mltvrs_test_generator PRIVATE mltvrs::mltvrs mltvrs::test_main)
    add_test(NAME mltvrs_test_generator COMMAND mltvrs_test_generator)
endif()

#####################
# Add Sub-Libraries #
#####################

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/mltvrs/async)

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
