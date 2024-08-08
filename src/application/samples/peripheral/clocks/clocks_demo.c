/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: CLOCK CALIBRATION DRIVER.
 *
 * Create: 2024-02-26
 */

#include "clock_calibration.h"
#include "app_init.h"
#include "nv.h"
#include "nv_upg.h"
#include "pmu.h"

#define CLOCK_TASK_PRIO          24
#define CLOCK_TASK_STACK_SIZE    0x1000
#define CLOCK_CTRIM_INCREASE     1
#define CLOCK_CTRIM_DECREASE     0

static void clocks_task(void)
{
    uint8_t xo_ctrim_value = 0;

    // 从flash中加载校准值到ctrim 寄存器
    calibration_xo_core_ctrim_flash_init();
    calibration_get_xo_core_ctrim_reg(&xo_ctrim_value);
    osal_printk("[clock]Current ctrim value = %x", xo_ctrim_value);

    // 调整频偏寄存器码字，范围Decimal[0, 255]
    calibration_xo_core_ctrim_algorithm(CLOCK_CTRIM_INCREASE, 0x10);
    calibration_xo_core_ctrim_algorithm(CLOCK_CTRIM_DECREASE, 0x5);
    // 获取调整后的频偏寄存器值
    calibration_get_xo_core_ctrim_reg(&xo_ctrim_value);
    // 调整后，校准码字存回flash
    calibration_xo_core_ctrim_save_flash(xo_ctrim_value);
    osal_printk("[clock]After calibration, ctrim value = %x", xo_ctrim_value);
}


/* Run the clock_task. */
app_run(clocks_task);