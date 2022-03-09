include(${CMAKE_CURRENT_LIST_DIR}/../components/cwhttpd/cmake/files.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../components/frogfs/cmake/files.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../components/cwhttpd/cmake/functions.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../components/frogfs/cmake/functions.cmake)

idf_component_register(
SRCS
    main.c
    storage.c
    wifi.c
    ws.c
)

target_add_resource(${COMPONENT_LIB} ../resources/cacert.der)
target_add_resource(${COMPONENT_LIB} ../resources/prvtkey.der)

set(name frogfs)
declare_frogfs_bin(../html NAME ${name} CONFIG ../frogfs.yaml)
idf_component_get_property(main_args esptool_py FLASH_ARGS)
idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
esptool_py_flash_target(${name}-flash "${main_args}" "${sub_args}" ALWAYS_PLAINTEXT)
esptool_py_flash_to_partition(${name}-flash frogfs ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${COMPONENT_LIB}.dir/frogfs_${name}.bin)
add_dependencies(${name}-flash generate_${name})
