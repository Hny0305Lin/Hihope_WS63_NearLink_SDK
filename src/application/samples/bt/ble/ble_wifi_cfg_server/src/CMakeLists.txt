#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
#===============================================================================

set(SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/ble_wifi_cfg_adv.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ble_wifi_cfg_server.c"
    PARENT_SCOPE)
