/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: dfx print
 */
#ifdef CONFIG_DFX_SUPPORT_PRINT
#include <stdarg.h>
#include "dfx_print.h"

static uint8_t g_print_level = DFX_PRINT_LEVEL_ERROR;
uint8_t dfx_print_get_level(void)
{
    return g_print_level;
}
void dfx_print_set_level(uint8_t level)
{
    if (level < DFX_PRINT_LEVEL_MAX) {
        g_print_level =  level;
    }
}

#endif