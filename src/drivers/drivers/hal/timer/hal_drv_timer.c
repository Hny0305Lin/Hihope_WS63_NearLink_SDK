/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL drv timer \n
 *
 * History: \n
 * 2022-06-02, Create file. \n
 */
#include "common_def.h"
#include "errcode.h"
#include "hal_drv_timer.h"

uintptr_t g_timer_comm_regs = NULL;
uintptr_t g_timer_regs[CONFIG_TIMER_MAX_NUM] = {NULL};

errcode_t hal_timer_regs_init(timer_index_t index)
{
    if (timer_porting_base_addr_get(index) == 0) {
        return ERRCODE_TIMER_INVALID_REG_ADDR;
    }

    g_timer_regs[index] = timer_porting_base_addr_get(index);
    g_timer_comm_regs = timer_porting_comm_addr_get();

    return ERRCODE_SUCC;
}

void hal_timer_regs_deinit(timer_index_t index)
{
    g_timer_regs[index] = NULL;
}