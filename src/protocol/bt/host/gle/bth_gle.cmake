#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "bth_gle")
set(BTH_RAM_LIST  "" CACHE INTERNAL "" FORCE)
set(BTH_PUBLIC_HEADER_LIST "" CACHE INTERNAL "" FORCE)
set(BTH_PRIVATE_HEADER_LIST "" CACHE INTERNAL "" FORCE)
add_subdirectory_if_exist(host)
add_subdirectory_if_exist(ahi)
add_subdirectory_if_exist(dft)
add_subdirectory_if_exist(samples)
add_subdirectory_if_exist(sdk)
add_subdirectory_if_exist(service)
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

MESSAGE("BTH_PUBLIC_HEADER_LIST=" ${BTH_PUBLIC_HEADER_LIST})

set(PUBLIC_HEADER
    ${BTH_PUBLIC_HEADER_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/L0
    ${CMAKE_CURRENT_SOURCE_DIR}/../bg_common/include/sdk_common
)

set(PRIVATE_HEADER
    ${BTH_PRIVATE_HEADER_LIST}
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
