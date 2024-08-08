/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2024-01-25
 */

#ifndef BOOT_MALLOC_H
#define BOOT_MALLOC_H

#include <stdint.h>

void boot_malloc_init(void);
void *boot_malloc(uint32_t size);

#endif
