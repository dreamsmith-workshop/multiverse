cmake_minimum_required(VERSION 3.19)

project(multiverse_shop LANGUAGES CXX VERSION 0.0.1)

find_package(Boost         REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(fmt           REQUIRED)
find_package(cpprestsdk    REQUIRED)

set(MLTVRS_SHOP_HPP ${CMAKE_CURRENT_LIST_DIR}/stripe.hpp)
set(MLTVRS_SHOP_IPP ${CMAKE_CURRENT_LIST_DIR}/ipp/stripe.ipp)
set(MLTVRS_SHOP_CPP ${CMAKE_CURRENT_LIST_DIR}/src/stripe.cpp)

add_library(mltvrs_shop ${MLTVRS_SHOP_HPP} ${MLTVRS_SHOP_IPP} ${MLTVRS_SHOP_CPP})
target_link_libraries(
    mltvrs_shop
        PUBLIC
            mltvrs
            Boost::boost
            Microsoft.GSL::GSL
            fmt::fmt-header-only
            cpprestsdk::cpprest
)

if(ENABLE_TESTING AND MLTVRS_ENABLE_TESTING)
    add_executable(test_stripe ${CMAKE_CURRENT_LIST_DIR}/test/test_stripe.cpp)
    target_link_libraries(test_stripe PRIVATE mltvrs_shop mltvrs::mltvrs_test_main)
endif()
