/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: BLE UUID Server module.
 */
#ifndef BLE_WIFI_CFG_SERVER_H
#define BLE_WIFI_CFG_SERVER_H

#include "bts_def.h"

/* Characteristic Property */
#define BLE_WIFI_CFG_PROPERTIES  (GATT_CHARACTER_PROPERTY_BIT_NOTIFY | GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP)

/* Characteristic Property */
#define UUID_SERVER_IND_PROPERTIES  (GATT_CHARACTER_PROPERTY_BIT_INDICATE | GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP)

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
errcode_t ble_wifi_cfg_server_init(void);

/**
 * @if Eng
 * @brief  send data to peer device by wifi server.
 * @attention  NULL
 * @param  [in]  value  send value.
 * @param  [in]  len    Length of send value。
 * @retval BT_STATUS_SUCCESS    Excute successfully
 * @retval BT_STATUS_FAIL       Execute fail
 * @par Dependency:
 * @li bts_def.h
 * @else
 * @brief  通过wifi server 发送数据给对端。
 * @attention  NULL
 * @retval BT_STATUS_SUCCESS    执行成功
 * @retval BT_STATUS_FAIL       执行失败
 * @par 依赖:
 * @li bts_def.h
 * @endif
 */
errcode_t ble_wifi_cfg_server_send_report_by_uuid(const uint8_t *data, uint32_t len);

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
 * @brief  通过wifi server 发送数据给对端。
 * @attention  NULL
 * @retval BT_STATUS_SUCCESS    执行成功
 * @retval BT_STATUS_FAIL       执行失败
 * @par 依赖:
 * @li bts_def.h
 * @endif
 */
errcode_t ble_wifi_cfg_server_send_report_by_handle(uint16_t attr_handle, const uint8_t *data, uint8_t len);

#endif