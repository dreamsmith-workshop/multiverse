@PACKAGE_INIT@

if(NOT TARGET mltvrs::mltvrs)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
    include(${CMAKE_CURRENT_LIST_DIR}/mltvrs/project-depends.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/mltvrsTargets.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/mltvrs/project-config.cmake)

    if(TARGET mltvrs::mltvrs_ietf)
        add_library(mltvrs::ietf ALIAS mltvrs::mltvrs_ietf)
    endif()
    if(TARGET mltvrs::mltvrs_shop)
        add_library(mltvrs::shop ALIAS mltvrs::mltvrs_shop)
    endif()
endif()
