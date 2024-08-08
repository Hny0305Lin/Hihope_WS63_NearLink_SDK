/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides timer driver source \n
 *
 * History: \n
 * 2022-11-07， Create file. \n
 */
#include <stdbool.h>
#include <limits.h>
#include "securec.h"
#include "common_def.h"
#include "interrupt/osal_interrupt.h"
#include "tcxo.h"
#include "timer_porting.h"
#include "timer.h"

#ifndef UINT64_MAX
#define UINT64_MAX (0xFFFFFFFFFFFFFFFFUL)
#endif

#if defined(CONFIG_TIMER_USING_OLD_VERSION)
#define TIMER_OLD_VERSION_LOAD_COUNT_MASK 0x0F
#endif

#ifdef CONFIG_TIMER_MAX_TIMERS_NUM
#ifndef CONFIG_TIMER_MAX_TIMERS_NUM_0
#define CONFIG_TIMER_MAX_TIMERS_NUM_0 CONFIG_TIMER_MAX_TIMERS_NUM
#endif
#ifndef CONFIG_TIMER_MAX_TIMERS_NUM_1
#define CONFIG_TIMER_MAX_TIMERS_NUM_1 CONFIG_TIMER_MAX_TIMERS_NUM
#endif
#ifndef CONFIG_TIMER_MAX_TIMERS_NUM_2
#define CONFIG_TIMER_MAX_TIMERS_NUM_2 CONFIG_TIMER_MAX_TIMERS_NUM
#endif
#ifndef CONFIG_TIMER_MAX_TIMERS_NUM_3
#define CONFIG_TIMER_MAX_TIMERS_NUM_3 CONFIG_TIMER_MAX_TIMERS_NUM
#endif
#ifndef CONFIG_TIMER_MAX_TIMERS_NUM_EXTRA
#define CONFIG_TIMER_MAX_TIMERS_NUM_EXTRA CONFIG_TIMER_MAX_TIMERS_NUM
#endif
#endif

typedef enum timer_flag {
    TIMER_FLAG_NORMAL,
    TIMER_FLAG_PERMANENT
} timer_flag_t;

typedef struct timer_info {
    timer_handle_t      key;
    timer_index_t       index;
    bool                enable;
    bool                is_run;
    timer_flag_t        flag;
    uint64_t            cycle;
    timer_callback_t    callback;
    uintptr_t           data;
} timer_info_t;

typedef struct soft_timer_cfg {
    uintptr_t   timer_addr;
    uint8_t     timer_num;
} soft_timer_cfg_t;

#define CUR_MAX_HARDWARE_TIMER_NUM 4
#define TIMER_SIZE_0 0
#define TIMER_SIZE_1 1
#define TIMER_SIZE_2 2
#define TIMER_SIZE_3 3
#define TIMER_SIZE_4 4

#if defined(CONFIG_TIMER_MAX_NUM) && (CONFIG_TIMER_MAX_NUM > TIMER_SIZE_0)
static timer_info_t g_timerlist_0[CONFIG_TIMER_MAX_TIMERS_NUM_0];
#endif
#if defined(CONFIG_TIMER_MAX_NUM) && (CONFIG_TIMER_MAX_NUM > TIMER_SIZE_1)
static timer_info_t g_timerlist_1[CONFIG_TIMER_MAX_TIMERS_NUM_1];
#endif
#if defined(CONFIG_TIMER_MAX_NUM) && (CONFIG_TIMER_MAX_NUM > TIMER_SIZE_2)
static timer_info_t g_timerlist_2[CONFIG_TIMER_MAX_TIMERS_NUM_2];
#endif
#if defined(CONFIG_TIMER_MAX_NUM) && (CONFIG_TIMER_MAX_NUM > TIMER_SIZE_3)
static timer_info_t g_timerlist_3[CONFIG_TIMER_MAX_TIMERS_NUM_3];
#endif
#if defined(CONFIG_TIMER_MAX_NUM) && (CONFIG_TIMER_MAX_NUM > TIMER_SIZE_4)
static timer_info_t \
    g_timerlist_extra[CONFIG_TIMER_MAX_NUM - CUR_MAX_HARDWARE_TIMER_NUM][CONFIG_TIMER_MAX_TIMERS_NUM_EXTRA];
#endif
typedef struct timers_manager {
    uint64_t            last_time_load_cycle;
    timer_info_t        *timers;
    uint32_t            hw_timer_irq_id;
    uint16_t            hw_timer_int_priority;
    bool                in_timer_irq;
    uint8_t             soft_time_num;
} timers_manager_t;

static bool g_timer_inited = false;
static bool g_timer_adapted[CONFIG_TIMER_MAX_NUM];
timers_manager_t g_timers_manager[CONFIG_TIMER_MAX_NUM];

/*
 * Reduces the time left for all the timers by timer us amount
 */
STATIC bool timer_update_timers_time(timer_index_t index, uint64_t update_amount_cycle)
{
    bool has_new_timer = false;
    uint32_t irq_sts = osal_irq_lock();
    uint8_t soft_time_num = g_timers_manager[index].soft_time_num;
    for (uint8_t i = 0; i < soft_time_num; i++) {
        if (g_timers_manager[index].timers[i].cycle <= update_amount_cycle) {
            g_timers_manager[index].timers[i].cycle = 0;
            if (g_timers_manager[index].timers[i].is_run) {
                has_new_timer = true;
            }
        } else {
            g_timers_manager[index].timers[i].cycle -= (uint32_t)update_amount_cycle;
        }
    }
    osal_irq_restore(irq_sts);
    return has_new_timer;
}

/*
 * Loops through the timers to see if any need to be processed
 */
STATIC bool timer_process_timers(timer_index_t index)
{
    bool re_run = false;
    uint64_t start_time = uapi_tcxo_get_count();
    uint8_t soft_time_num = g_timers_manager[index].soft_time_num;
    for (uint8_t i = 0; i < soft_time_num; i++) {
        if (g_timers_manager[index].timers[i].enable &&
            g_timers_manager[index].timers[i].cycle == 0 &&
            g_timers_manager[index].timers[i].is_run) {
            g_timers_manager[index].timers[i].is_run = false;
            if (g_timers_manager[index].timers[i].callback) {
                g_timers_manager[index].timers[i].callback(g_timers_manager[index].timers[i].data);
            }
            re_run = true;
        }
    }

    if (re_run) {
        uint64_t end_time = uapi_tcxo_get_count();
        uint64_t diff = end_time - start_time;
        uint64_t cycle = 0;
        cycle = timer_porting_compensat_by_tcxo(diff);
        re_run = timer_update_timers_time(index, cycle);
    }

    return re_run;
}

/*
 * Gets the index of the next timer to expire
 */
STATIC uint32_t timer_get_next_timer(timer_index_t index)
{
    uint64_t shortest_time_found = UINT64_MAX;
    uint8_t soft_time_num = g_timers_manager[index].soft_time_num;
    uint32_t timer_found = soft_time_num;
    for (uint8_t i = 0; i < soft_time_num; i++) {
        if (g_timers_manager[index].timers[i].enable &&
            g_timers_manager[index].timers[i].cycle <= shortest_time_found &&
            g_timers_manager[index].timers[i].is_run) {
            shortest_time_found = (uint32_t)g_timers_manager[index].timers[i].cycle;
            timer_found = i;
        }
    }
    return timer_found;
}

/*
 * Gets the time in us till the next interrupt, UINT64_MAX indicates that no timer is set
 */
STATIC uint64_t timer_get_time_till_interrupt(timer_index_t index)
{
    uint64_t current_value = hal_timer_get_current_value(index);
    if (current_value != 0) {
        return (uint32_t)current_value;
    }

    uint32_t next_timer = timer_get_next_timer(index);
    if (next_timer >= g_timers_manager[index].soft_time_num) {
        return UINT64_MAX;
    }

    uint64_t cycle = (uint32_t)g_timers_manager[index].timers[next_timer].cycle;
    if (g_timers_manager[index].in_timer_irq) {
        return cycle;
    }

    hal_timer_stop(index);
    hal_timer_config_load(index, cycle);
    hal_timer_start(index);
    return cycle;
}

/*
 * Searches through the timers to find the one with the lowest callback time
 */
STATIC void timer_set_next_timer_interrupt(timer_index_t index)
{
    uint32_t irq_sts = osal_irq_lock();
    hal_timer_stop(index);
    uint32_t timer_found = timer_get_next_timer(index);
    if (timer_found < g_timers_manager[index].soft_time_num) {
        uint64_t cycle_with_mask;
#if defined(CONFIG_TIMER_USING_OLD_VERSION)
    cycle_with_mask = g_timers_manager[index].timers[timer_found].cycle | TIMER_OLD_VERSION_LOAD_COUNT_MASK;
#else
    cycle_with_mask = g_timers_manager[index].timers[timer_found].cycle;
#endif
        hal_timer_config_load(index, cycle_with_mask);
        hal_timer_start(index);
        g_timers_manager[index].last_time_load_cycle = cycle_with_mask;
    }
    osal_irq_restore(irq_sts);
}

STATIC void timer_int_callback(timer_index_t index)
{
    uint32_t irq_sts = osal_irq_lock();
    g_timers_manager[index].in_timer_irq = true;
    timer_update_timers_time(index, g_timers_manager[index].last_time_load_cycle);
    g_timers_manager[index].last_time_load_cycle = 0;
    osal_irq_restore(irq_sts);

    while (timer_process_timers(index)) {
    }

    timer_set_next_timer_interrupt(index);
    g_timers_manager[index].in_timer_irq = false;
}

STATIC errcode_t soft_timer_list_cfg_get(uint32_t index, soft_timer_cfg_t *timer_info)
{
    switch (index) {
#if defined(CONFIG_TIMER_MAX_NUM) && (CONFIG_TIMER_MAX_NUM > TIMER_SIZE_0)
        case TIMER_SIZE_0:
            timer_info->timer_num = CONFIG_TIMER_MAX_TIMERS_NUM_0;
            timer_info->timer_addr = (uintptr_t)g_timerlist_0;
            break;
#endif
#if defined(CONFIG_TIMER_MAX_NUM) && (CONFIG_TIMER_MAX_NUM > TIMER_SIZE_1)
        case TIMER_SIZE_1:
            timer_info->timer_num = CONFIG_TIMER_MAX_TIMERS_NUM_1;
            timer_info->timer_addr = (uintptr_t)g_timerlist_1;
            break;
#endif
#if defined(CONFIG_TIMER_MAX_NUM) && (CONFIG_TIMER_MAX_NUM > TIMER_SIZE_2)
        case TIMER_SIZE_2:
            timer_info->timer_num = CONFIG_TIMER_MAX_TIMERS_NUM_2;
            timer_info->timer_addr = (uintptr_t)g_timerlist_2;
            break;
#endif
#if defined(CONFIG_TIMER_MAX_NUM) && (CONFIG_TIMER_MAX_NUM > TIMER_SIZE_3)
        case TIMER_SIZE_3:
            timer_info->timer_num = CONFIG_TIMER_MAX_TIMERS_NUM_3;
            timer_info->timer_addr = (uintptr_t)g_timerlist_3;
            break;
#endif

        default:
#if defined(CONFIG_TIMER_MAX_NUM) && (CONFIG_TIMER_MAX_NUM > TIMER_SIZE_4)
            timer_info->timer_num = CONFIG_TIMER_MAX_TIMERS_NUM_EXTRA;
            timer_info->timer_addr = (uintptr_t)&g_timerlist_extra[index - CUR_MAX_HARDWARE_TIMER_NUM];
#endif
            break;
    }
    if (memset_s((uint8_t *)timer_info->timer_addr, sizeof(timer_info_t) * timer_info->timer_num, 0,
        sizeof(timer_info_t) * timer_info->timer_num) != EOK) {
        return ERRCODE_MEMSET;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_timer_init(void)
{
    uint32_t irq_sts = osal_irq_lock();
    if (g_timer_inited) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }
    if (memset_s(&g_timers_manager, sizeof(g_timers_manager), 0, sizeof(g_timers_manager)) != EOK) {
        osal_irq_restore(irq_sts);
        return ERRCODE_MEMSET;
    }

#if defined(CONFIG_TIMER_SUPPORT_LPC)
    timer_port_clock_enable(true);
#endif

    for (uint32_t i = 0; i < CONFIG_TIMER_MAX_NUM; i++) {
        soft_timer_cfg_t time_info;
        uint32_t ret;
        g_timer_adapted[i] = false;
        ret = soft_timer_list_cfg_get(i, &time_info);
        if (ret != ERRCODE_SUCC) {
            osal_irq_restore(irq_sts);
            return ret;
        }
        g_timers_manager[i].soft_time_num = time_info.timer_num;
        g_timers_manager[i].timers = (timer_info_t *)time_info.timer_addr;
    }

    g_timer_inited = true;
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_timer_adapter(timer_index_t index, uint32_t int_id, uint16_t int_priority)
{
    uint32_t irq_sts = osal_irq_lock();
    if (!g_timer_inited) {
        osal_irq_restore(irq_sts);
        return ERRCODE_TIMER_NOT_INIT;
    }

    if (g_timer_adapted[index]) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }

    errcode_t ret = hal_timer_init(index, timer_int_callback);
    if (ret != ERRCODE_SUCC) {
        osal_irq_restore(irq_sts);
        return ret;
    }

    timer_port_register_irq(index, int_id, int_priority);

    g_timers_manager[index].hw_timer_irq_id = int_id;
    g_timers_manager[index].hw_timer_int_priority = int_priority;

    g_timer_adapted[index] = true;
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_timer_deinit(void)
{
    uint32_t irq_sts = osal_irq_lock();
    if (!g_timer_inited) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }

    for (uint32_t i = 0; i < CONFIG_TIMER_MAX_NUM; i++) {
        if (g_timer_adapted[i]) {
            hal_timer_stop(i);
            timer_port_unregister_irq(i, g_timers_manager[i].hw_timer_irq_id);
        }
    }

    if (memset_s(&g_timers_manager, sizeof(g_timers_manager), 0, sizeof(g_timers_manager)) != EOK) {
        osal_irq_restore(irq_sts);
        return ERRCODE_MEMSET;
    }

    for (uint32_t i = 0; i < CONFIG_TIMER_MAX_NUM; i++) {
        if (g_timer_adapted[i]) {
            hal_timer_deinit(i);
        }
    }

#if defined(CONFIG_TIMER_SUPPORT_LPC)
    timer_port_clock_enable(false);
#endif

    g_timer_inited = false;
    for (uint32_t i = 0; i < CONFIG_TIMER_MAX_NUM; i++) {
        g_timer_adapted[i] = false;
    }
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_timer_create(timer_index_t index, timer_handle_t *timer)
{
    if (timer == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t irq_sts = osal_irq_lock();
    for (uint32_t i = 0; i < g_timers_manager[index].soft_time_num; i++) {
        if (g_timers_manager[index].timers[i].enable == false) {
            g_timers_manager[index].timers[i].enable = true;
            g_timers_manager[index].timers[i].is_run = false;
            g_timers_manager[index].timers[i].index = index;
            g_timers_manager[index].timers[i].key = (timer_handle_t)&g_timers_manager[index].timers[i];

            *timer = g_timers_manager[index].timers[i].key;
            osal_irq_restore(irq_sts);
            return ERRCODE_SUCC;
        }
    }

    *timer = NULL;
    osal_irq_restore(irq_sts);
    return ERRCODE_TIMER_NO_ENOUGH;
}

errcode_t uapi_timer_delete(timer_handle_t timer)
{
    if (timer == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    timer_info_t *timer_info = (timer_info_t *)timer;
    uint32_t irq_sts = osal_irq_lock();
    timer_info->enable = false;
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

uint32_t uapi_timer_get_max_us(void)
{
    return timer_porting_cycle_2_us(UINT32_MAX);
}

errcode_t uapi_timer_start(timer_handle_t timer, uint32_t time_us, timer_callback_t callback, uintptr_t data)
{
    timer_index_t index;

    if (timer == NULL || callback == NULL || time_us == 0 || time_us > uapi_timer_get_max_us()) {
        return ERRCODE_INVALID_PARAM;
    }

    timer_info_t *timer_info = (timer_info_t *)timer;
    uint32_t irq_sts = osal_irq_lock();
    if (timer_info->enable == false) {
        osal_irq_restore(irq_sts);
        return ERRCODE_TIEMR_NOT_CREATED;
    }

    uint64_t cycle;
#if defined(CONFIG_TIMER_USING_OLD_VERSION)
    cycle = timer_porting_us_2_cycle(time_us) | TIMER_OLD_VERSION_LOAD_COUNT_MASK;
#else
    cycle = timer_porting_us_2_cycle(time_us);
#endif
    index = timer_info->index;
    timer_info->cycle = cycle;
    timer_info->callback = callback;
    timer_info->data = data;

    uint64_t remain_time_cycle = timer_get_time_till_interrupt(index);
    if (cycle <= remain_time_cycle) {
        if (remain_time_cycle != UINT64_MAX && g_timers_manager[index].last_time_load_cycle != 0) {
            timer_update_timers_time(index, g_timers_manager[index].last_time_load_cycle - remain_time_cycle);
        }
        g_timers_manager[index].last_time_load_cycle = cycle;
        hal_timer_stop(index);
        hal_timer_config_load(index, cycle);
        hal_timer_start(index);
    } else {
        timer_info->cycle += (g_timers_manager[index].last_time_load_cycle - remain_time_cycle);
    }
    timer_info->is_run = true;
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_timer_stop(timer_handle_t timer)
{
    if (timer == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    timer_info_t *timer_info = (timer_info_t *)timer;

    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(timer_info->enable == false)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_TIEMR_NOT_CREATED;
    }

    if (unlikely(timer_info->is_run == false)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }

    timer_info->is_run = false;
    timer_info->cycle = 0;
    timer_info->callback = NULL;
    timer_info->data = 0;

    /* 检查当前是否还有在运行的定时器, 若没有定时器则停止硬件计时 */
    timer_index_t index = timer_info->index;
    uint32_t next_timer = timer_get_next_timer(index);
    if (next_timer >= g_timers_manager[index].soft_time_num) {
        hal_timer_stop(index);
    }
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_timer_get_current_time_us(timer_index_t index, uint32_t *current_time_us)
{
    if (unlikely(index >= TIMER_MAX_NUM || current_time_us == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t irq_sts = osal_irq_lock();
    uint64_t current_value = hal_timer_get_current_value(index);
    osal_irq_restore(irq_sts);
    *current_time_us = timer_porting_cycle_2_us(current_value);
    return ERRCODE_SUCC;
}

#if defined(CONFIG_TIMER_SUPPORT_LPM)
errcode_t uapi_timer_suspend(uintptr_t val)
{
    unused(val);
    uint32_t int_sts = osal_irq_lock();
    for (uint32_t index = 0; index < TIMER_MAX_NUM; index++) {
        if (g_timer_adapted[index] == false) {
            continue;
        }
        timer_update_timers_time(index,
            g_timers_manager[index].last_time_load_cycle - hal_timer_get_current_value(index));
        g_timers_manager[index].last_time_load_cycle = 0;
        timer_set_next_timer_interrupt(index);
    }
    osal_irq_restore(int_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_timer_resume(uintptr_t val)
{
    uint32_t int_sts = osal_irq_lock();
    uint64_t compansation_count = *(uint64_t *)val;
    for (uint32_t index = 0; index < TIMER_MAX_NUM; index++) {
        if (g_timer_adapted[index] == false) {
            continue;
        }
        g_timers_manager[index].last_time_load_cycle = compansation_count;
        hal_timer_stop(index);
        hal_timer_config_load(index, timer_porting_us_2_cycle(1));
        hal_timer_start(index);
    }
    osal_irq_restore(int_sts);
    return ERRCODE_SUCC;
}
#endif /* CONFIG_TIMER_SUPPORT_LPM */