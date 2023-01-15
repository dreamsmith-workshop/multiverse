cmake_minimum_required(VERSION 3.15)

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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

mltvrs_configure_project(PREFIX MLTVRS REPORT_ALL TRUE)

##################
# Create Targets #
##################

set(
    MLTVRS_HPP
        ${CMAKE_CURRENT_LIST_DIR}/mltvrs/multiverse.hpp
        ${CMAKE_CURRENT_LIST_DIR}/mltvrs/string_literal.hpp
)
set(MLTVRS_IPP ${CMAKE_CURRENT_LIST_DIR}/mltvrs/ipp/string_literal.ipp)
set(MLTVRS_CPP)

add_library(mltvrs INTERFACE ${MLTVRS_HPP} ${MLTVRS_IPP} ${MLTVRS_CPP})
target_include_directories(mltvrs INTERFACE ${CMAKE_CURRENT_LIST_DIR})

#####################
# Add Sub-Libraries #
#####################

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/mltvrs/shop)
