cmake_minimum_required(VERSION 3.23)

project(multiverse_async LANGUAGES CXX VERSION 0.0.1)

find_package(Boost REQUIRED)

mltvrs_add_install_dependencies(
    PREFIX
        MLTVRS
    PACKAGES
        Boost
)

set(
    MLTVRS_ASYNC_HPP
        ${CMAKE_CURRENT_LIST_DIR}/concepts.hpp
        ${CMAKE_CURRENT_LIST_DIR}/coroutine.hpp
        ${CMAKE_CURRENT_LIST_DIR}/execute.hpp
        ${CMAKE_CURRENT_LIST_DIR}/future.hpp
        ${CMAKE_CURRENT_LIST_DIR}/task.hpp
        ${CMAKE_CURRENT_LIST_DIR}/type_traits.hpp
)
set(
    MLTVRS_ASYNC_IPP
        ${CMAKE_CURRENT_LIST_DIR}/ipp/coroutine.ipp
        ${CMAKE_CURRENT_LIST_DIR}/ipp/execute.ipp
        ${CMAKE_CURRENT_LIST_DIR}/ipp/future.ipp
        ${CMAKE_CURRENT_LIST_DIR}/ipp/task.ipp
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
target_link_libraries(mltvrs_async INTERFACE mltvrs::mltvrs Boost::boost)
add_library(mltvrs::async ALIAS mltvrs_async)

if(ENABLE_TESTING AND MLTVRS_ENABLE_TESTING)
    add_executable(mltvrs_async_test_coroutine ${CMAKE_CURRENT_LIST_DIR}/test/test_coroutine.cpp)
    target_link_libraries(mltvrs_async_test_coroutine PRIVATE mltvrs::async mltvrs::test_main)
    add_test(NAME mltvrs_async_test_coroutine COMMAND mltvrs_async_test_coroutine)
    
    add_executable(mltvrs_async_test_future ${CMAKE_CURRENT_LIST_DIR}/test/test_future.cpp)
    target_link_libraries(mltvrs_async_test_future PRIVATE mltvrs::async mltvrs::test_main)
    add_test(NAME mltvrs_async_test_future COMMAND mltvrs_async_test_future)
    
    add_executable(mltvrs_async_test_post ${CMAKE_CURRENT_LIST_DIR}/test/test_post.cpp)
    target_link_libraries(mltvrs_async_test_post PRIVATE mltvrs::async mltvrs::test_main)
    add_test(NAME mltvrs_async_test_post COMMAND mltvrs_async_test_post)
    
    add_executable(mltvrs_async_test_task ${CMAKE_CURRENT_LIST_DIR}/test/test_task.cpp)
    target_link_libraries(mltvrs_async_test_task PRIVATE mltvrs::async mltvrs::test_main)
    add_test(NAME mltvrs_async_test_task COMMAND mltvrs_async_test_task)
endif()
