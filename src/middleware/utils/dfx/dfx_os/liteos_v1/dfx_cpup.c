/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx cpup
 * This file should be changed only infrequently and with great care.
 */

#include "dfx_cpup.h"
#include "errcode.h"
#include "dfx_os_st.h"
#include "common_def.h"
#include "los_cpup.h"

uint32_t dfx_cpup_get_all_usage(uint16_t max_num, dfx_cpup_item_usage_info_t *cpup_info, uint32_t mode, uint16_t flag)
{
#ifdef LOSCFG_KERNEL_CPUP
    return LOS_AllCpuUsage(max_num, (CPUP_INFO_S *)cpup_info, mode, flag);
#else
    unused(max_num);
    unused(cpup_info);
    unused(mode);
    unused(flag);
    return (uint32_t)-1;
#endif
}

void dfx_cpup_reset(void)
{
#ifdef LOSCFG_KERNEL_CPUP
    LOS_CpupReset();
#endif
}

void dfx_cpup_stop(void)
{
#ifdef LOSCFG_KERNEL_CPUP
    LOS_CpupStop();
#endif
}

void dfx_cpup_start(void)
{
#ifdef LOSCFG_KERNEL_CPUP
    LOS_CpupStart();
#endif
}