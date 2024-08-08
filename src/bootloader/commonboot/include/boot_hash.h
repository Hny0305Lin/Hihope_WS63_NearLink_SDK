/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_HASH_H
#define BOOT_HASH_H

#include <stdint.h>

uint32_t check_sha256(const uint8_t *input, uint32_t len, uint8_t *hash_check);

#endif