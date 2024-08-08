#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2023. All rights reserved.
#===============================================================================
# Temporarily Released
set(MODULE_NAME "bth")
set(AUTO_DEF_FILE_ID FALSE)
set(COMPONENT_NAME "bg_common")

set(BTH_RAM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTH_ROM_LIST  "" CACHE INTERNAL "" FORCE)

if(DEFINED ROM_COMPONENT OR DEFINED ROM_SYM_PATH)
    set(BT_ROM_VERSION true)
else()
    set(BT_ROM_VERSION false)
endif()

if(${CHIP} MATCHES "bs20|bs21|bs21e|bs21a|bs22|bs26")
    add_subdirectory_if_exist(bs21)
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
    )
elseif((${CHIP} MATCHES "bs25") AND ("bt_host_rom" IN_LIST TARGET_COMPONENT))
    set(MODULE_NAME "bt")
    add_subdirectory(bs25)
    set(PUBLIC_HEADER
        ${BTH_PUBLIC_HDR_LIST}
    )
elseif(${CHIP} MATCHES "ws63")
    add_subdirectory_if_exist(ws63)
    add_subdirectory_if_exist(src)
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_a
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_b
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/ws63
        ${CMAKE_CURRENT_SOURCE_DIR}/include/log
        ${CMAKE_CURRENT_SOURCE_DIR}/include/log/oml
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/../host/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nv/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
        ${CMAKE_CURRENT_SOURCE_DIR}/include/power_on
    )
elseif(${CHIP} MATCHES "ws53")
    add_subdirectory_if_exist(src)
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_a
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_b
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/ws53
        ${CMAKE_CURRENT_SOURCE_DIR}/include/log
        ${CMAKE_CURRENT_SOURCE_DIR}/include/log/uapi
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/include/power_on
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nv/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
    )
else()
    add_subdirectory_if_exist(src)
    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_a
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_b
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ahi_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/gdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hci
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host
        ${CMAKE_CURRENT_SOURCE_DIR}/include/host/stack
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config/default
        ${CMAKE_CURRENT_SOURCE_DIR}/include/log
        ${CMAKE_CURRENT_SOURCE_DIR}/include/log/oml
        ${CMAKE_CURRENT_SOURCE_DIR}/include/sdk_common
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/at
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx
    )
endif()
set(PUBLIC_HEADER
        ${PUBLIC_HEADER}/
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config
     )
if("${BTH_RAM_LIST}" STREQUAL "")
    if(DEFINED CONFIG_SLE_BLE_SUPPORT AND SUPPORT_MULTI_LIBS IN_LIST DEFINES)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP}-${CONFIG_SLE_BLE_SUPPORT}/lib${COMPONENT_NAME}.a)
    elseif(DEFINED CONFIG_SUPPORT_SLE_BLE_CENTRAL_DEFAULT)
        set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    else()
        set(BTH_RAM_LIST "__null__")
    endif()
elseif(${CHIP} MATCHES "bs20|bs21|bs21e|bs21a|bs22|bs26")
    if("ROM_EC" IN_LIST DEFINES)
        set(LOG_DEF
            ${CMAKE_CURRENT_SOURCE_DIR}/bs21/include/log_ec/log_def_bth.h
            ${CMAKE_CURRENT_SOURCE_DIR}/bs21/include/log_ec/log_def_sdk_bth.h
        )
    else()
        set(LOG_DEF
            ${CMAKE_CURRENT_SOURCE_DIR}/bs21/include/log/log_def_bth.h
            ${CMAKE_CURRENT_SOURCE_DIR}/bs21/include/log/log_def_sdk_bth.h
        )
    endif()
elseif(NOT ${CHIP} MATCHES "bs25")
    set(LOG_DEF
        ${CMAKE_CURRENT_SOURCE_DIR}/include/log/log_def_bth.h
    )
endif()

set(SOURCES
    ${BTH_RAM_LIST}
)

if((NOT "ipc_old" IN_LIST TARGET_COMPONENT) AND (NOT "bs25_bgle" IN_LIST TARGET_COMPONENT))
set(PRIVATE_HEADER
    ${PRIVATE_HEADER}
    # Temporarily Released
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ipc
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


set(COMPONENT_NAME "bg_common_rom")

if("${BTH_ROM_LIST}" STREQUAL "")
    set(BTH_ROM_LIST "__null__")
endif()

MESSAGE("BG COMMON BTH_ROM_LIST:" ${BTH_ROM_LIST})
set(SOURCES
    ${BTH_ROM_LIST}
)

set(PUBLIC_HEADER
    ${BTH_PUBLIC_HDR_LIST}
)

set(PRIVATE_HEADER
    ${BTH_PRIVATE_HDR_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ipc
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
