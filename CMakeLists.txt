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

mltvrs_configure_project(PREFIX MLTVRS)
mltvrs_report_project_configurations(PREFIX MLTVRS)

