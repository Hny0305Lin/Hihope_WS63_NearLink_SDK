/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: BLE uart client Config. \n
 *
 * History: \n
 * 2023-07-26, Create file. \n
 */

#ifndef BLE_UART_CLIENT_H
#define BLE_UART_CLIENT_H

#include <stdint.h>
#include "common_def.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
BLE_PUBLIC_DEVICE_ADDRESS,
BLE_RANDOM_DEVICE_ADDRESS,
BLE_PUBLIC_IDENTITY_ADDRESS,
BLE_RANDOM_STATIC_IDENTITY_ADDRESS
} ble_address_t;

errcode_t ble_uart_client_discover_all_service(uint16_t conn_id);
errcode_t ble_uart_client_write_cmd(uint8_t *data, uint16_t len, uint16_t hand);
errcode_t ble_uart_client_init(void);
uint16_t ble_uart_get_write_vlaue_handle(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif