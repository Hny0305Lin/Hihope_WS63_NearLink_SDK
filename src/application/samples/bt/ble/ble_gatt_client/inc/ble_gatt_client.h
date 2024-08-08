/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 *
 * Description: BT GATT Client Moudle.
 */

/**
 * @defgroup bluetooth_bts_hid_server HID SERVER API
 * @ingroup
 * @{
 */
#ifndef BLE_GATT_CLIENT_H
#define BLE_GATT_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Use this funtion to init gatt client.
 * @par Description:init gatt client.
 * @attention  NULL
 * @param  NULL
 * @retval error code.
 * @else
 * @brief  初始化gatt 客户端。
 * @par 说明:初始化gatt 客户端。
 * @attention  NULL
 * @param  NULL
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t ble_gatt_client_init(void);

/**
 * @if Eng
 * @brief  discover all service, character and descriptor of remote device.
 * @par Description:discover all service of remote device.
 * @attention  NULL
 * @param  conn_id connection ID
 * @retval error code.
 * @else
 * @brief  发现对端设备所有服务、特征和描述符。
 * @par 说明：发现对端设备所有服务、特征和描述符。
 * @attention  NULL
 * @param  conn_id 连接 ID
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t ble_gatt_client_discover_all_service(uint16_t conn_id);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif
