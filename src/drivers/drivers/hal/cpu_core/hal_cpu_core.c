/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: BT HAL CPU CORE MODULE.
 *
 * Create: 2019-12-19
 */
#include "hal_cpu_core.h"
#include "platform_core.h"
#include "chip_io.h"

#define CMU_CLOCK_SOFT_RST2_REG                     (GLB_CTL_M_RB_BASE + 0xB8)
#define CMU_CLOCK_SOFT_RST_GLB_B_CRG_BIT            0
#define CMU_CLOCK_SOFT_RST_GLB_B_LGC_BIT            1
#define CMU_CLOCK_SOFT_RST_GLB_B_CPU_BIT            2

bool hal_cpu_is_bt_enabled(void)
{
    if ((readw(CMU_CLOCK_SOFT_RST2_REG) & BIT(CMU_CLOCK_SOFT_RST_GLB_B_CPU_BIT)) != 0) {
        return true;
    } else {
        return false;
    }
}
