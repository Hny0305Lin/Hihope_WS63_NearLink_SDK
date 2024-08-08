#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "bt_host")

set(BTH_RAM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTA_RAM_LIST  "" CACHE INTERNAL "" FORCE)

set(BTH_PUBLIC_HDR_LIST  "" CACHE INTERNAL "" FORCE)
set(BTH_PRIVATE_HDR_LIST  "" CACHE INTERNAL "" FORCE)

set(BTH_ROM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTA_ROM_LIST  "" CACHE INTERNAL "" FORCE)

if(DEFINED ROM_COMPONENT OR DEFINED ROM_SYM_PATH)
    set(BT_ROM_VERSION true)
else()
    set(BT_ROM_VERSION false)
endif()

add_subdirectory_if_exist(host)
add_subdirectory_if_exist(util)
if(BT_GLE_ONLY IN_LIST DEFINES)
    add_subdirectory_if_exist(ahi/ahi_b/ahi_vendor) # vendor_init方法
else()
    add_subdirectory_if_exist(ahi/ahi_b)
endif()

if("${BTH_RAM_LIST}" STREQUAL "")
    if(DEFINED CONFIG_SLE_BLE_SUPPORT AND SUPPORT_MULTI_LIBS IN_LIST DEFINES)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP}-${CONFIG_SLE_BLE_SUPPORT}/lib${COMPONENT_NAME}.a)
    elseif(DEFINED CONFIG_SUPPORT_SLE_BLE_CENTRAL_DEFAULT)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    else()
        set(BTH_RAM_LIST "__null__")
    endif()
endif()
set(SOURCES
    ${BTH_RAM_LIST}
)

if(BT_GLE_ONLY IN_LIST DEFINES)
set(PUBLIC_HEADER
    ${BTH_PUBLIC_HDR_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/ahi/ahi_b/ahi_vendor/inc
)

set(PRIVATE_HEADER
    ${BTH_PRIVATE_HDR_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/ipc
    ${CMAKE_CURRENT_SOURCE_DIR}/host/bs21/ram/sdk/include # 只要SDK头文件
)
else()
set(PUBLIC_HEADER
    ${BTH_PUBLIC_HDR_LIST}
)

set(PRIVATE_HEADER
    ${BTH_PRIVATE_HDR_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/ipc
)
endif()

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

set(COMPONENT_NAME "bt_host_rom")

if("${BTH_ROM_LIST}" STREQUAL "")
    set(BTH_ROM_LIST "__null__")
endif()

set(SOURCES
    ${BTH_ROM_LIST}
)

set(PUBLIC_HEADER
    ${BTH_PUBLIC_HDR_LIST}
)

set(PRIVATE_HEADER
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
MESSAGE("BTH_ROM_LIST:" ${BTH_ROM_LIST})
set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/bluetooth/bth/${TARGET_COMMAND})

build_component()
