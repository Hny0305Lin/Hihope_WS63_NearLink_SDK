/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: Delay Header file.
 */

#ifndef __SOC_TIMER__
#define __SOC_TIMER__
#include <td_base.h>


td_void uapi_mdelay(td_u32 us);

td_void uapi_set_cpu_cycles_in_us(td_u32 cycle);

#endif
