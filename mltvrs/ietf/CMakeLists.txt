cmake_minimum_required(VERSION 3.23)

project(multiverse_ietf LANGUAGES CXX VERSION 0.0.1)

find_package(Microsoft.GSL REQUIRED)

mltvrs_add_install_dependencies(
    PREFIX
        MLTVRS
    PACKAGES
        Microsoft.GSL
)

set(MLTVRS_IETF_HPP ${CMAKE_CURRENT_LIST_DIR}/raw_encoding.hpp)
set(MLTVRS_IETF_IPP ${CMAKE_CURRENT_LIST_DIR}/ipp/raw_encoding.ipp)
set(MLTVRS_IETF_CPP )

add_library(mltvrs_ietf INTERFACE ${MLTVRS_IETF_HPP} ${MLTVRS_IETF_IPP} ${MLTVRS_IETF_CPP})
target_sources(
    mltvrs_ietf
        INTERFACE
            FILE_SET
                mltvrs_ietf_HEADERS
                TYPE
                    HEADERS
                FILES
                    ${MLTVRS_IETF_HPP}
                    ${MLTVRS_IETF_IPP}
                BASE_DIRS
                    ${CMAKE_SOURCE_DIR}
)
target_link_libraries(mltvrs_ietf INTERFACE mltvrs::mltvrs Microsoft.GSL::GSL)
add_library(mltvrs::ietf ALIAS mltvrs_ietf)

if(ENABLE_TESTING AND MLTVRS_ENABLE_TESTING)
    add_executable(test_raw_encoding ${CMAKE_CURRENT_LIST_DIR}/test/test_raw_encoding.cpp)
    target_link_libraries(test_raw_encoding PRIVATE mltvrs::ietf mltvrs::test_main)
    add_test(NAME test_raw_encoding COMMAND test_raw_encoding)
endif()
