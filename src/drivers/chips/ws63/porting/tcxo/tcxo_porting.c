/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides tcxo port \n
 *
 * History: \n
 * 2022-08-16， Create file. \n
 */

#include "tcxo_porting.h"
#include "hal_tcxo.h"
#include "hal_tcxo_v150.h"
#include "platform_core.h"

static uintptr_t const g_tcxo_base_addr = (uintptr_t)TCXO_COUNT_BASE_ADDR;

uintptr_t tcxo_porting_base_addr_get(void)
{
    return g_tcxo_base_addr;
}

static uint32_t g_tcxo_ticks_per_usec = TCXO_TICKS_PER_U_SECOND;

uint32_t tcxo_porting_ticks_per_usec_get(void)
{
    return g_tcxo_ticks_per_usec;
}

void tcxo_porting_ticks_per_usec_set(uint32_t ticks)
{
    g_tcxo_ticks_per_usec = ticks;
}