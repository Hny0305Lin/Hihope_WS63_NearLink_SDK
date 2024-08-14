/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE uart server Config. \n
 *
 * History: \n
 * 2023-07-17, Create file. \n
 */

#ifndef SLE_UART_SERVER_H
#define SLE_UART_SERVER_H

#include <stdint.h>
#include "sle_ssap_server.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Service UUID */
#define SLE_UUID_SERVER_SERVICE        0x2222

/* Property UUID */
#define SLE_UUID_SERVER_NTF_REPORT     0x2323

/* Property Property */
#define SLE_UUID_TEST_PROPERTIES  (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

/* Operation indication */
#define SLE_UUID_TEST_OPERATION_INDICATION  (SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE)

/* Descriptor Property */
#define SLE_UUID_TEST_DESCRIPTOR   (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

errcode_t sle_uart_server_init(ssaps_read_request_callback ssaps_read_callback, ssaps_write_request_callback
    ssaps_write_callback);

errcode_t sle_uart_server_send_report_by_uuid(const uint8_t *data, uint8_t len);

errcode_t sle_uart_server_send_report_by_handle(const uint8_t *data, uint16_t len);

uint16_t sle_uart_client_is_connected(void);

typedef void (*sle_uart_server_msg_queue)(uint8_t *buffer_addr, uint16_t buffer_size);

void sle_uart_server_register_msg(sle_uart_server_msg_queue sle_uart_server_msg);


void sle_uart_server_sample_set_mcs(uint16_t conn_id);

uint16_t get_connect_id(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif