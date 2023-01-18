@PACKAGE_INIT@

if(NOT TARGET mltvrs::mltvrs)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
    include(${CMAKE_CURRENT_LIST_DIR}/mltvrsTargets.cmake)
endif()
