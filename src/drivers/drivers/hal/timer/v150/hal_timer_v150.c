/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 HAL timer \n
 *
 * History: \n
 * 2022-12-08, Create file. \n
 */
#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "hal_timer_v150_regs_op.h"
#include "timer_porting.h"
#include "hal_timer_v150.h"

#define TIMER_COUNT_HIGH_32BIT_RIGHT_SHIFT 32
#define TIMER_LOAD_COUNT1_LOCK_VALUE 0x5A5A5A5A
#define TIMER_CURRENT_COUNT_LOCK_TIMEOUT 0xFFFF

static hal_timer_callback_t g_hal_timer_callback[TIMER_MAX_NUM] = {NULL};

static bool const g_hal_timer_width[TIMER_MAX_NUM + 1] = {
    CONFIG_TIMER_0_WIDTH_64 == 1 ? true : false,
#if defined(CONFIG_TIMER_1_WIDTH_64)
    CONFIG_TIMER_1_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_TIMER_1_WIDTH_64 */
#if defined(CONFIG_TIMER_2_WIDTH_64)
    CONFIG_TIMER_2_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_TIMER_2_WIDTH_64 */
#if defined(CONFIG_TIMER_3_WIDTH_64)
    CONFIG_TIMER_3_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_TIMER_3_WIDTH_64 */
#if defined(CONFIG_TIMER_4_WIDTH_64)
    CONFIG_TIMER_4_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_TIMER_4_WIDTH_64 */
#if defined(CONFIG_TIMER_5_WIDTH_64)
    CONFIG_TIMER_5_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_TIMER_5_WIDTH_64 */
#if defined(CONFIG_TIMER_6_WIDTH_64)
    CONFIG_TIMER_6_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_TIMER_6_WIDTH_64 */
#if defined(CONFIG_TIMER_7_WIDTH_64)
    CONFIG_TIMER_7_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_TIMER_7_WIDTH_64 */
    false
};

#pragma weak hal_timer_init = hal_timer_v150_init
errcode_t hal_timer_v150_init(timer_index_t index, hal_timer_callback_t callback)
{
    if ((hal_timer_regs_init(index)) != ERRCODE_SUCC) {
        return ERRCODE_TIMER_INVALID_REG_ADDR;
    }
    g_hal_timer_callback[index] = callback;
    return ERRCODE_SUCC;
}

#pragma weak hal_timer_deinit = hal_timer_v150_deinit
void hal_timer_v150_deinit(timer_index_t index)
{
    g_hal_timer_callback[index] = NULL;
    hal_timer_regs_deinit(index);
}

#pragma weak hal_timer_start = hal_timer_v150_start
void hal_timer_v150_start(timer_index_t index)
{
    hal_timer_v150_control_reg_set_enable(index, 1);
}

#pragma weak hal_timer_stop = hal_timer_v150_stop
void hal_timer_v150_stop(timer_index_t index)
{
    hal_timer_v150_control_reg_set_enable(index, 0);
}

#pragma weak hal_timer_config_load = hal_timer_v150_config_load
void hal_timer_v150_config_load(timer_index_t index, uint64_t delay_count)
{
    hal_timer_v150_control_reg_set_mode(index, (uint32_t)TIMER_V150_MODE_ONE_SHOT);
    hal_timer_v150_set_load_count0(index, (uint32_t)delay_count);
    if (g_hal_timer_width[index]) {
        hal_timer_v150_set_load_lock(index, (uint32_t)TIMER_LOAD_COUNT1_LOCK_VALUE);
        hal_timer_v150_set_load_count1(index, (uint32_t)(delay_count >> TIMER_COUNT_HIGH_32BIT_RIGHT_SHIFT));
        hal_timer_v150_set_load_lock(index, (uint32_t)0);
    }
}

#pragma weak hal_timer_get_current_value = hal_timer_v150_get_current_value
uint64_t hal_timer_v150_get_current_value(timer_index_t index)
{
    uint64_t count = 0;
    uint32_t timeout = 0;

    /*
     * TIMER_V150 使能信号无效时读取到的当前COUNT值会保持原值不变，
     * 因此判断enable不为1时认为时钟已到期，返回0。
     */
    if (hal_timer_v150_control_reg_get_enable(index) != 1) {
        return 0;
    }

    hal_timer_v150_ctrl_set_cnt_req(index);
    while (timeout < TIMER_CURRENT_COUNT_LOCK_TIMEOUT) {
        if (hal_timer_v150_ctrl_get_cnt_lock(index) == 1) {
            count = (uint64_t)hal_timer_v150_get_current_value0(index);
            if (g_hal_timer_width[index]) {
                count += ((uint64_t)hal_timer_v150_get_current_value1(index) << TIMER_COUNT_HIGH_32BIT_RIGHT_SHIFT);
            }
            break;
        }
        timeout++;
    }
    return count;
}

void hal_timer_v150_interrupt_clear(timer_index_t index)
{
    if (index < TIMER_MAX_NUM) {
        hal_timer_v150_int_clr(index);
    }
}

#pragma weak hal_timer_irq_handler = hal_timer_v150_irq_handler
void hal_timer_v150_irq_handler(timer_index_t index)
{
    /* Clear the interrupt */
    hal_timer_v150_int_clr(index);

    if (g_hal_timer_callback[index]) {
        g_hal_timer_callback[index](index);
    }
}
