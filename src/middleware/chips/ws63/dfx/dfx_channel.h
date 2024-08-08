/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: dfx uart channel
 * This file should be changed only infrequently and with great care.
 */
#ifndef DFX_CHANNEL_H
#define DFX_CHANNEL_H

#include <stdint.h>
#include "errcode.h"

void diag_uart_rx_proc(uint8_t *buffer, uint16_t length);
errcode_t diag_register_channel(void);
#endif