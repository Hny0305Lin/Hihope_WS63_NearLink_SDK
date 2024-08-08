/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description:  mips
 */

#include "mips.h"
#include "tcxo.h"
#include "hal_mips.h"

static uint64_t g_platform_isr_count_start = 0;
static uint64_t g_platform_isr_count_end = 0;
static uint64_t g_platform_isr_run_time = 0;
static bool g_bt_thread_is_running = false;
static bool g_bt_isr_is_running = false;

static uint64_t g_isr_time_statistics = 0;
static bool g_thread_running_status = false;

void global_thread_status_update(bool status)
{
    g_thread_running_status = status;
    g_isr_time_statistics = 0;
}

bool global_thread_status_get(void)
{
    return g_thread_running_status;
}

void global_isr_time_statistics_update(uint64_t sys_time_start, uint64_t sys_time_end)
{
    if (global_thread_status_get()) {
        g_isr_time_statistics =  g_isr_time_statistics + (sys_time_end - sys_time_start);
    }
}

uint64_t global_isr_time_statistics_get(void)
{
    return g_isr_time_statistics;
}

void mips_compute_run_time_start(void)
{
    g_platform_isr_count_start = uapi_tcxo_get_us();
}

void mips_compute_run_time_stop(void)
{
    g_platform_isr_count_end = uapi_tcxo_get_us();

    if (g_bt_thread_is_running || g_bt_isr_is_running) {
        g_platform_isr_run_time += (uint32_t)(g_platform_isr_count_end - g_platform_isr_count_start);
    }
}

/* register mips callback function */
void mips_init(void)
{
    hal_register_mips_start_callback(mips_compute_run_time_start);
    hal_register_mips_stop_callback(mips_compute_run_time_stop);
}

/* get bt thread status when compute mips */
bool mips_get_bt_thread_status(void)
{
    return g_bt_thread_is_running;
}

/* set bt thread status when compute mips */
void mips_set_bt_thread_status(bool status)
{
    g_bt_thread_is_running = status;
}

/* get bt isr status when compute mips */
bool mips_get_bt_isr_status(void)
{
    return g_bt_isr_is_running;
}

/* set bt isr status when compute mips */
void mips_set_bt_isr_status(bool status)
{
    g_bt_isr_is_running = status;
}

/* get platfrom isr run time while bt thread and bt isr running */
uint32_t mips_get_plt_isr_run_time(void)
{
    return g_platform_isr_run_time;
}

/* clear plt isr run time data */
void mips_clear_plt_isr_run_time(void)
{
    g_platform_isr_run_time = 0;
}

