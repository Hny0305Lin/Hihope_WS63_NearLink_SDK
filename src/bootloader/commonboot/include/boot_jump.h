/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_JUMP_H
#define BOOT_JUMP_H

#include <stdint.h>

void jump_to_execute_addr(uint32_t addr);

#endif