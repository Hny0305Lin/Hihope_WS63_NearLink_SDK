#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
#===============================================================================

set(
    ble_wifi_cfg_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_wifi_cfg_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_wifi_cfg_scan.c
)
set(SOURCES "${SOURCES}" "${ble_wifi_cfg_src_list}" PARENT_SCOPE)