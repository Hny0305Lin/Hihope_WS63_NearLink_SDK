/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 *
 * Description: BLE UUID Server module.
 */

/**
 * @defgroup bluetooth_bts_hid_server HID SERVER API
 * @ingroup
 * @{
 */
#ifndef BLE_UUID_SERVER_H
#define BLE_UUID_SERVER_H

#include "bts_def.h"

/* Service UUID */
#define BLE_UUID_UUID_SERVER_SERVICE                 0xABCD
/* Characteristic UUID */
#define BLE_UUID_UUID_SERVER_REPORT                  0xCDEF
/* Client Characteristic Configuration UUID */
#define BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION 0x2902
/* Server ID */
#define BLE_UUID_SERVER_ID 1

/* Characteristic Property */
#define UUID_SERVER_PROPERTIES   (GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_NOTIFY)

/**
 * @if Eng
 * @brief  BLE uuid server inir.
 * @attention  NULL
 * @retval BT_STATUS_SUCCESS    Excute successfully
 * @retval BT_STATUS_FAIL       Execute fail
 * @par Dependency:
 * @li bts_def.h
 * @else
 * @brief  BLE UUID服务器初始化。
 * @attention  NULL
 * @retval BT_STATUS_SUCCESS    执行成功
 * @retval BT_STATUS_FAIL       执行失败
 * @par 依赖:
 * @li bts_def.h
 * @endif
 */
errcode_t ble_uuid_server_init(void);

/**
 * @if Eng
 * @brief  send data to peer device by uuid on uuid server.
 * @attention  NULL
 * @param  [in]  value  send value.
 * @param  [in]  len    Length of send value。
 * @retval BT_STATUS_SUCCESS    Excute successfully
 * @retval BT_STATUS_FAIL       Execute fail
 * @par Dependency:
 * @li bts_def.h
 * @else
 * @brief  通过uuid server 发送数据给对端。
 * @attention  NULL
 * @retval BT_STATUS_SUCCESS    执行成功
 * @retval BT_STATUS_FAIL       执行失败
 * @par 依赖:
 * @li bts_def.h
 * @endif
 */
errcode_t ble_uuid_server_send_report_by_uuid(const uint8_t *data, uint8_t len);

/**
 * @if Eng
 * @brief  send data to peer device by handle on uuid server.
 * @attention  NULL
 * @param  [in]  value  send value.
 * @param  [in]  len    Length of send value。
 * @retval BT_STATUS_SUCCESS    Excute successfully
 * @retval BT_STATUS_FAIL       Execute fail
 * @par Dependency:
 * @li bts_def.h
 * @else
 * @brief  通过uuid server 发送数据给对端。
 * @attention  NULL
 * @retval BT_STATUS_SUCCESS    执行成功
 * @retval BT_STATUS_FAIL       执行失败
 * @par 依赖:
 * @li bts_def.h
 * @endif
 */
errcode_t ble_uuid_server_send_report_by_handle(uint16_t attr_handle, const uint8_t *data, uint8_t len);

#endif

