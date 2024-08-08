#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/ble_wifi_cfg_sample.c" PARENT_SCOPE)

set(PRIVATE_HEADER
    ${ROOT_DIR}/samples/bt/ble/ble_wifi_cfg_server/inc
    ${ROOT_DIR}/samples/bt/ble/ble_wifi_cfg_client/inc
)