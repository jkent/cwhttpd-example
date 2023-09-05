cmake_minimum_required(VERSION 3.16)

if(ESP_PLATFORM)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
endif()

project(httpd)

target_add_frogfs(httpd.elf ./html NAME frogfs)