/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  calculate mips.
 *
 * Create:  2018-10-15
 */
#include "hal_mips.h"
#include "stdlib.h"

hal_exception_dump_mips_callback g_exception_mips_start_callback = NULL;
hal_exception_dump_mips_callback g_exception_mips_stop_callback = NULL;

/* register mips callback function */
void hal_register_mips_start_callback(hal_exception_dump_mips_callback callback)
{
    if (callback != NULL) {
        g_exception_mips_start_callback = callback;
    }
}

/* register mips callback function */
void hal_register_mips_stop_callback(hal_exception_dump_mips_callback callback)
{
    if (callback != NULL) {
        g_exception_mips_stop_callback = callback;
    }
}

/* start calculating ticks  */
void hal_calculate_mips_start(void)
{
    if (g_exception_mips_start_callback != NULL) {
        g_exception_mips_start_callback();
    }
}

/* stop calculating ticks  */
void hal_calculate_mips_stop(void)
{
    if (g_exception_mips_stop_callback != NULL) {
        g_exception_mips_stop_callback();
    }
}
