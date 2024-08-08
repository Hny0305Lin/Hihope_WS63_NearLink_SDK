/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_LOAD_H
#define BOOT_LOAD_H

#include <stdint.h>

uint32_t loader_serial_ymodem(uint32_t offset, uint32_t erased_size, uint32_t min, uint32_t max);

#endif