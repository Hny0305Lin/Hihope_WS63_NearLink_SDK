/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: debug
 *
 * Create: 2023-01-09
 */

#include "boot_debug.h"
#include "boot_def.h"
#include "boot_init.h"

void set_boot_status(hi_boot_start_status status)
{
    uapi_reg_write16(STATUS_DEBUG_REG_ADDR, status);
}

uint16_t get_boot_status(void)
{
    return uapi_reg_read_val16(STATUS_DEBUG_REG_ADDR);
}