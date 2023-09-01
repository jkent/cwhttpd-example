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

