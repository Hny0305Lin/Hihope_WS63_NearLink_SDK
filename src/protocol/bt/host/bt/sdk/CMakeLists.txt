######################################### sdk start ###########################################
set(BTA_RAM_LIST  "" CACHE INTERNAL "" FORCE)
add_subdirectory_if_exist(src)
add_subdirectory_if_exist(L1)

set(COMPONENT_NAME "bth_sdk")

if("${BTA_RAM_LIST}" STREQUAL "")
    set(BTA_RAM_LIST "__null__")
endif()
set(SOURCES
    ${BTA_RAM_LIST}
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/L1/include
    ${CMAKE_CURRENT_SOURCE_DIR}/L2/ble
    ${CMAKE_CURRENT_SOURCE_DIR}/L2/common
    ${CMAKE_CURRENT_SOURCE_DIR}/L2/br
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../host/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../host/stack_api/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bt_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data_config
    ${CMAKE_CURRENT_SOURCE_DIR}/src/keyboard_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mouse_manager  
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mpc_proc
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/common/L0
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/ble
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/ble/L0
)

set(PRIVATE_HEADER
    ${PRIVATE_HEADER}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../bg_common/include/ipc
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(WHOLE_LINK
    true
)

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/bth/${TARGET_COMMAND})

build_component()

######################################### sdk end ###########################################
