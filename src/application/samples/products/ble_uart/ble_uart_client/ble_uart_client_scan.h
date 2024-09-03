/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: BLE uart client scan \n
 *
 * History: \n
 * 2023-07-26, Create file. \n
 */
#ifndef BLE_UART_CLIENT_SCAN_H
#define BLE_UART_CLIENT_SCAN_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

errcode_t ble_uart_start_scan(void);
errcode_t ble_uart_set_scan_parameters(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif  /* __cplusplus */
#endif  /* __cplusplus */
#endif