cmake_minimum_required(VERSION 3.19)

project(multiverse_ietf LANGUAGES CXX VERSION 0.0.1)

find_package(Microsoft.GSL REQUIRED)

set(MLTVRS_IETF_HPP ${CMAKE_CURRENT_LIST_DIR}/rfc4648.hpp)
set(MLTVRS_IETF_IPP ${CMAKE_CURRENT_LIST_DIR}/ipp/rfc4648.ipp)
set(MLTVRS_IETF_CPP )

add_library(mltvrs_ietf INTERFACE ${MLTVRS_IETF_HPP} ${MLTVRS_IETF_IPP} ${MLTVRS_IETF_CPP})
target_link_libraries(mltvrs_ietf INTERFACE mltvrs Microsoft.GSL::GSL)

if(ENABLE_TESTING AND MLTVRS_ENABLE_TESTING)
    add_executable(test_rfc4648 ${CMAKE_CURRENT_LIST_DIR}/test/test_rfc4648.cpp)
    target_link_libraries(test_rfc4648 PRIVATE mltvrs_ietf mltvrs::mltvrs_test_main)
endif()
