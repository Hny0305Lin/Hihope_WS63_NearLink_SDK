#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "bt_app")
set(BTA_RAM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTH_PUBLIC_HDR_LIST  "" CACHE INTERNAL "" FORCE)
set(BTH_PRIVATE_HDR_LIST  "" CACHE INTERNAL "" FORCE)

if(${CHIP} MATCHES "ws63|bs20|bs21|bs21e|bs21a|bs22|bs26") # 单双核差异
    add_subdirectory_if_exist(dft)
    add_subdirectory_if_exist(service)
    add_subdirectory_if_exist(ahi/ahi_a)
else()
    add_subdirectory_if_exist(dft)
    add_subdirectory_if_exist(service)
    add_subdirectory_if_exist(ahi/ahi_a)
endif()

# for sdk closed_component compile
if("${BTA_RAM_LIST}" STREQUAL "")
    if(DEFINED CONFIG_SLE_BLE_SUPPORT AND SUPPORT_MULTI_LIBS IN_LIST DEFINES)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP}-${CONFIG_SLE_BLE_SUPPORT}/lib${COMPONENT_NAME}.a)
    elseif(DEFINED CONFIG_SUPPORT_SLE_BLE_CENTRAL_DEFAULT)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    else()
        set(SOURCES "__null__")
    endif()
else()
    set(SOURCES
        ${BTA_RAM_LIST}
    )
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/log/log_def_sdk_bth.h
    )
endif()
if(${CHIP} MATCHES "ws53|ws63|bs20|bs21|bs21e|bs21a|bs22|bs26|bs25")
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/host/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble/L0
    )
else()
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
        ${CMAKE_CURRENT_SOURCE_DIR}/host/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ble/L0
        ${CMAKE_CURRENT_SOURCE_DIR}/include/br/L0
    )
endif()

set(PRIVATE_HEADER
    ${PRIVATE_HEADER}
    ${BTH_PRIVATE_HDR_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/ipc
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/bth/${TARGET_COMMAND})

build_component()
