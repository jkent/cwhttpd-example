cmake_minimum_required(VERSION 3.5)

if(CONFIG_IDF_TARGET_ESP8266 OR ESP_PLATFORM)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
else()
    set(CONFIG_EHTTPD_LOG_LEVEL_VERBOSE y)
    set(CONFIG_EHTTPD_TLS_MBEDTLS y)
    #set(CONFIG_ESPFS_LOG_LEVEL_VERBOSE y)
    set(CONFIG_ESPFS_USE_HEATSHRINK y)
endif()

project(esphttpd)
include(components/libesphttpd/cmake/include.cmake)
include(components/libespfs/cmake/include.cmake)

if(CONFIG_IDF_TARGET_ESP8266 OR ESP_PLATFORM)
    set(target ${PROJECT_NAME}.elf)
else()
    set(target ${PROJECT_NAME})

    find_library(RT_LIBRARY rt)
    find_library(BSD_LIBRARY bsd)
    find_package(Threads)

    add_executable(${target}
        ${libesphttpd_SRCS}
        ${libespfs_SRCS}
        main/main.c
        main/storage.c
        main/ws.c
    )
    target_link_libraries(${target}
        ${MBEDTLS_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        ${RT_LIBRARY}
        ${BSD_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
    )
    target_include_directories(${target} PUBLIC
        ${libesphttpd_INCLUDE_DIRS}
        ${libespfs_INCLUDE_DIRS}
    )
    target_include_directories(${target} PRIVATE
	${libespfs_PRIV_INCLUDE_DIRS}
    )
    target_compile_definitions(${target} PUBLIC -DUNIX)
    target_config_vars(${target})
endif()

if(CONFIG_EHTTPD_TLS_MBEDTLS OR CONFIG_EHTTPD_TLS_OPENSSL)
    target_add_resources(${target} resources/cacert.der resources/prvtkey.der)
endif()

if(ESP_PLATFORM)
    define_target_espfs(espfs_bin html ${CMAKE_CURRENT_BINARY_DIR}/espfs.bin)

    idf_component_get_property(main_args esptool_py FLASH_ARGS)
    idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
    esptool_py_flash_target(espfs-flash "${main_args}" "${sub_args}" ALWAYS_PLAINTEXT)
    esptool_py_flash_to_partition(espfs-flash espfs ${CMAKE_CURRENT_BINARY_DIR}/espfs.bin)
    add_dependencies(espfs-flash espfs_bin)
else()
    target_add_espfs(${target} espfs html)
endif()
