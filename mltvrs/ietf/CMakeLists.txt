cmake_minimum_required(VERSION 3.15)

project(multiverse_ietf LANGUAGES CXX VERSION 0.0.1)

find_package(Microsoft.GSL REQUIRED)

set(MLTVRS_IETF_HPP ${CMAKE_CURRENT_LIST_DIR}/rfc4648.hpp)
set(MLTVRS_IETF_IPP ${CMAKE_CURRENT_LIST_DIR}/ipp/rfc4648.ipp)
set(MLTVRS_IETF_CPP )

add_library(mltvrs_ietf INTERFACE ${MLTVRS_IETF_HPP} ${MLTVRS_IETF_IPP} ${MLTVRS_IETF_CPP})
target_link_libraries(mltvrs_ietf INTERFACE mltvrs Microsoft.GSL::GSL)

if(ENABLE_TESTING AND MLTVRS_ENABLE_TESTING)
    include(CTest)

    find_package(Catch2 REQUIRED)

    add_executable(test_rfc4648 ${CMAKE_CURRENT_LIST_DIR}/test/test_rfc4648.cpp)
    target_compile_definitions(test_rfc4648 PRIVATE CATCH_CONFIG_PREFIX_ALL)
    target_link_libraries(test_rfc4648 PRIVATE mltvrs_ietf Catch2::Catch2WithMain)
endif()
