cmake_minimum_required(VERSION 3.23)

project(multiverse_shop LANGUAGES CXX VERSION 0.0.1)

find_package(Boost         REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(fmt           REQUIRED)

mltvrs_add_install_dependencies(
    PREFIX
        MLTVRS
    PACKAGES
        Boost
        Microsoft.GSL
        fmt
)

set(MLTVRS_SHOP_HPP ${CMAKE_CURRENT_LIST_DIR}/stripe.hpp)
set(MLTVRS_SHOP_IPP ${CMAKE_CURRENT_LIST_DIR}/ipp/stripe.ipp)
set(MLTVRS_SHOP_CPP ${CMAKE_CURRENT_LIST_DIR}/src/stripe.cpp)

add_library(mltvrs_shop ${MLTVRS_SHOP_HPP} ${MLTVRS_SHOP_IPP} ${MLTVRS_SHOP_CPP})
target_sources(
    mltvrs_shop
        INTERFACE
            FILE_SET
                mltvrs_shop_HEADERS
                TYPE
                    HEADERS
                FILES
                    ${MLTVRS_SHOP_HPP}
                    ${MLTVRS_SHOP_IPP}
                BASE_DIRS
                    ${CMAKE_SOURCE_DIR}
)
target_link_libraries(
    mltvrs_shop
        PUBLIC
            mltvrs::mltvrs
            Microsoft.GSL::GSL
            Boost::boost
            fmt::fmt-header-only
)
add_library(mltvrs::shop ALIAS mltvrs_shop)

if(ENABLE_TESTING AND MLTVRS_ENABLE_TESTING)
    add_executable(test_stripe ${CMAKE_CURRENT_LIST_DIR}/test/test_stripe.cpp)
    target_link_libraries(test_stripe PRIVATE mltvrs::shop mltvrs::test_main)
    add_test(NAME test_stripe COMMAND test_stripe)
endif()
