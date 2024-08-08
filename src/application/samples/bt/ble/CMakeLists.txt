#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_GATT_CLIENT_SAMPLE)
    add_subdirectory_if_exist(ble_gatt_client)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_GATT_SERVER_SAMPLE)
    add_subdirectory_if_exist(ble_gatt_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_SPEED_SERVER_SAMPLE)
    add_subdirectory_if_exist(ble_speed_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_SPEED_CLIENT_SAMPLE)
    add_subdirectory_if_exist(ble_speed_client)
endif()

if(${CHIP} MATCHES "ws63")
add_subdirectory_if_exist(ble_wifi_cfg_client)
add_subdirectory_if_exist(ble_wifi_cfg_server)
endif()
set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)