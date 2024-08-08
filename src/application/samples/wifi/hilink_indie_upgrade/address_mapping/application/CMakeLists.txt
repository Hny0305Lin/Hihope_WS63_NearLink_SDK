#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "app_addr_map")

set(CMAKE_HILINK_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/app_function_mapping.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hilink_uapi/uapi_hilink.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hilink_uapi/uapi_hilink_log_manage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hilink_uapi/uapi_hilink_device_ext.c

    ${CMAKE_CURRENT_SOURCE_DIR}/hilink_uapi/uapi_ble_cfg_net_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hilink_uapi/uapi_hilink_bt_function.c

    ${CMAKE_CURRENT_SOURCE_DIR}/hilink_uapi/uapi_hilink_network_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hilink_uapi/uapi_hilink_socket_adapter.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../ohos_connect/hilink_adapt/product/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../ohos_connect/hilink_adapt/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../ohos_connect/hilink_adapt/adapter/include/

    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/hilink_uapi/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error=logical-op
    -Wno-error=sign-compare
    -Wno-error=jump-misses-init
    -Wno-sign-compare
    -Wno-jump-misses-init
    -Wno-error=unused-parameter
    -Wno-unused-parameter
    -Wno-unused-but-set-variable
    -Wno-error=unused-variable
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})

build_component()