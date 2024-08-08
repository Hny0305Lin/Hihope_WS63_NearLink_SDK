/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: NV Storage Library debug
 */

#include "nv_debug.h"
#include "nv_porting.h"
#include "common_def.h"

STATIC uint32_t g_nv_debug_level = NV_LOG_LEVEL_INFO;

uint32_t nv_get_debug_level(void)
{
    return g_nv_debug_level;
}

void nv_set_debug_level(uint32_t level)
{
    g_nv_debug_level = level;
}
