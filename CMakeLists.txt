cmake_minimum_required(VERSION 3.5)

if(ESP_PLATFORM)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
endif()

project(httpd)

if(NOT ESP_PLATFORM)
    add_definitions(-DCONFIG_CWHTTPD_LOG_LEVEL_VERBOSE=y)
    add_definitions(-DCONFIG_CWHTTPD_MBEDTLS=y)
    #add_definitions(-DCONFIG_FROGFS_LOG_LEVEL_VERBOSE=y)
    add_definitions(-DCONFIG_FROGFS_USE_HEATSHRINK=y)

    include(${CMAKE_CURRENT_LIST_DIR}/components/cwhttpd/cmake/files.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/components/frogfs/cmake/files.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/components/cwhttpd/cmake/functions.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/components/frogfs/cmake/functions.cmake)

    list(APPEND CMAKE_MODULE_PATH ${cwhttpd_DIR}/cmake)
    include_directories(${MBEDTLS_INCLUDE_DIR})

    add_executable(httpd
        main/main.c
        main/storage.c
        main/ws.c
    )

    add_subdirectory(components/cwhttpd)

    find_library(RT_LIBRARY rt)
    find_library(BSD_LIBRARY bsd)
    find_package(Threads)
    find_package(MbedTLS)

    target_link_libraries(httpd
        cwhttpd
        ${MBEDTLS_LIBRARIES}
        ${RT_LIBRARY}
        ${BSD_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
    )

    target_compile_definitions(httpd PUBLIC -DUNIX)

    target_add_resource(httpd resources/cacert.der)
    target_add_resource(httpd resources/prvtkey.der)
    target_add_frogfs(httpd html NAME frogfs CONFIG frogfs.yaml)
endif()
