/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_VERIFY_BOOT_H
#define BOOT_VERIFY_BOOT_H

#include <stdint.h>
#define STRUCT_VERSION                  0x10000

uint32_t sign_flashboot_verify(uint32_t head_addr);
uint32_t sign_loaderboot_verify(uint32_t head_addr);
void non_secure_boot(uint32_t addr);

#endif