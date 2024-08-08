/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_CRC_H
#define BOOT_CRC_H

#include <stdint.h>

uint16_t crc16_ccitt(uint16_t crc_start, uint8_t* buf, uint32_t len);

#endif