/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provide HAL drv rtc \n
 *
 * History: \n
 * 2023-03-02, Create file. \n
 */
#include "common_def.h"
#include "errcode.h"
#include "hal_rtc.h"

uintptr_t g_rtc_comm_regs = NULL;
uintptr_t g_rtc_regs[CONFIG_RTC_MAX_NUM] = {NULL};

errcode_t hal_rtc_regs_init(rtc_index_t index)
{
    if (unlikely(rtc_porting_base_addr_get(index) == 0)) {
        return ERRCODE_RTC_REG_ADDR_INVALID;
    }

    g_rtc_regs[index] = rtc_porting_base_addr_get(index);
    g_rtc_comm_regs = rtc_porting_comm_addr_get();

    return ERRCODE_SUCC;
}

void hal_rtc_regs_deinit(rtc_index_t index)
{
    g_rtc_regs[index] = NULL;
}