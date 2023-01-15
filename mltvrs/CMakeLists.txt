
set(MLTVRS_HPP ${CMAKE_CURRENT_LIST_DIR}/multiverse.hpp)
set(MLTVRS_IPP)
set(MLTVRS_CPP)

add_library(mltvrs INTERFACE ${MLTVRS_HPP} ${MLTVRS_IPP} ${MLTVRS_CPP})
target_include_directories(mltvrs INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/shop)
