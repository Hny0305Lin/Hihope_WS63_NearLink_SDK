/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_YMODEM_H
#define BOOT_YMODEM_H

#include <stdint.h>

uint32_t  ymodem_open(void);
void ymodem_close(void);
uint32_t  ymodem_read(uint8_t* buf, uint32_t size);
uint32_t  ymodem_get_length(void);
uint32_t loader_ymodem_open(void);

#endif