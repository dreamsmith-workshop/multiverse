cmake_minimum_required(VERSION 3.15)

project(multiverse_shop LANGUAGES CXX VERSION 0.0.1)

set(MLTVRS_SHOP_HPP ${CMAKE_CURRENT_LIST_DIR}/stripe.hpp)
set(MLTVRS_SHOP_IPP)
set(MLTVRS_SHOP_CPP)

add_library(mltvrs_shop INTERFACE ${MLTVRS_SHOP_HPP} ${MLTVRS_SHOP_IPP} ${MLTVRS_SHOP_CPP})
target_link_libraries(mltvrs_shop INTERFACE mltvrs)

if(ENABLE_TESTING AND MLTVRS_ENABLE_TESTING)
    include(CTest)

    find_package(Catch2 REQUIRED)

    add_executable(test_stripe ${CMAKE_CURRENT_LIST_DIR}/test/test_stripe.cpp)
    target_compile_definitions(test_stripe PRIVATE CATCH_CONFIG_PREFIX_ALL)
    target_link_libraries(test_stripe PRIVATE mltvrs_shop Catch2::Catch2WithMain)
endif()
