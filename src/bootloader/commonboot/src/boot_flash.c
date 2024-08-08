/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: flash
 *
 * Create: 2023-01-09
 */

#include "boot_errcode.h"
#include "securec.h"
#include "boot_flash.h"

static flash_cmd_func g_flash_cmd_funcs;

flash_cmd_func *boot_get_flash_funcs(void)
{
    return &g_flash_cmd_funcs;
}

uint32_t boot_regist_flash_cmd(const flash_cmd_func *funcs)
{
    if (memcpy_s(&g_flash_cmd_funcs, sizeof(flash_cmd_func), funcs, sizeof(flash_cmd_func)) != EOK) {
        return ERRCODE_MEMCPY;
    }
    return ERRCODE_SUCC;
}