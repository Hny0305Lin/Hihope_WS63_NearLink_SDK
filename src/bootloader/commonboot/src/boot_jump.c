/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: jump
 *
 * Create: 2023-01-09
 */

#include "boot_jump.h"
#include "boot_reset.h"

void jump_to_execute_addr(uint32_t addr)
{
    void (*entry)(void) = (void*)(uintptr_t)(addr);
    entry();
    boot_fail();
}