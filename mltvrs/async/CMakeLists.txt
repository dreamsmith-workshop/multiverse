cmake_minimum_required(VERSION 3.23)

project(multiverse_async LANGUAGES CXX VERSION 0.0.1)

set(
    MLTVRS_ASYNC_HPP
        ${CMAKE_CURRENT_LIST_DIR}/concepts.hpp
        ${CMAKE_CURRENT_LIST_DIR}/type_traits.hpp
)
set(
    MLTVRS_ASYNC_IPP
)
set(
    MLTVRS_ASYNC_CPP
)

add_library(mltvrs_async INTERFACE ${MLTVRS_ASYNC_HPP} ${MLTVRS_ASYNC_IPP} ${MLTVRS_ASYNC_CPP})
target_sources(
    mltvrs_async
        INTERFACE
            FILE_SET
                mltvrs_async_HEADERS
                TYPE
                    HEADERS
                FILES
                    ${MLTVRS_ASYNC_HPP}
                    ${MLTVRS_ASYNC_IPP}
                BASE_DIRS
                    ${CMAKE_SOURCE_DIR}
)
target_link_libraries(mltvrs_async INTERFACE mltvrs::mltvrs)
add_library(mltvrs::async ALIAS mltvrs_async)
