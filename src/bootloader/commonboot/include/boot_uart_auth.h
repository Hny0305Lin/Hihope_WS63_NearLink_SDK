/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_UART_AUTH_H
#define BOOT_UART_AUTH_H

#include <stdint.h>
#include "boot_serial.h"

#define UART_FIRST_CHAR_TIMEOUT   1000
#define UART_PACKET_START_FLAG  0xDEADBEEF
#define UART_PACKET_PAYLOAD_MAX 1024

extern uart_param_stru g_uart_param;

enum {
    UART_TYPE_ROMBOOT_HANDSHAKE = 0xF0,
    UART_TYPE_ACK = 0xE1,
    UART_TYPE_FILE_START = 0xD2,
    UART_TYPE_FILE_END = 0xC3,
    UART_TYPE_CMD = 0xB4,
    UART_TYPE_DATA = 0xA5,
    UART_TYPE_FLASHBOOT_HANDSHAKE = 0x0F,
};

typedef struct {
    uint32_t start_flag;  /* 起始标识: 0xDEADBEEF */
    uint16_t packet_size; /* 报文长度：真实传输数据长度，要求不超过1024 */
    uint8_t type;         /* 报文类型 */
    uint8_t pad;
} packet_data_head;

typedef struct {
    packet_data_head head;
    uint8_t payload[UART_PACKET_PAYLOAD_MAX]; /**< 报文数据 */
    uint16_t check_sum;   /* 校验和 */
    uint8_t rev[2];       /* 2: rev */
} packet_data_info;

/* UART AUTH context */
typedef struct {
    uint8_t status;
    uint8_t pad;
    uint16_t received;
    packet_data_info packet;
} uart_ctx;

uint32_t uart_process(uint32_t interrupt_timeout_ms);

#ifdef CONFIG_LOADERBOOT_SUPPORT_SET_BUADRATE
uart_param_stru* uart_baudrate_rcv(const uart_ctx *ctx);
uint32_t serial_port_set_baudrate(uart_param_stru *uart);
#endif

#endif