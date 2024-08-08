/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides rtc driver source \n
 *
 * History: \n
 * 2023-03-06, Create file. \n
 */
#include <stdbool.h>
#include <limits.h>
#include "securec.h"
#include "common_def.h"
#include "interrupt/osal_interrupt.h"
#include "tcxo.h"
#include "rtc_porting.h"
#include "rtc.h"

#ifndef UINT64_MAX
#define UINT64_MAX (0xFFFFFFFFFFFFFFFFUL)
#endif

#if defined(CONFIG_RTC_USING_OLD_VERSION)
#define RTC_OLD_VERSION_LOAD_COUNT_MASK (uint64_t)0x0F
#endif

#ifdef CONFIG_RTC_MAX_RTCS_NUM
#ifndef CONFIG_RTC_MAX_RTCS_NUM_0
#define CONFIG_RTC_MAX_RTCS_NUM_0 CONFIG_RTC_MAX_RTCS_NUM
#endif
#ifndef CONFIG_RTC_MAX_RTCS_NUM_1
#define CONFIG_RTC_MAX_RTCS_NUM_1 CONFIG_RTC_MAX_RTCS_NUM
#endif
#ifndef CONFIG_RTC_MAX_RTCS_NUM_2
#define CONFIG_RTC_MAX_RTCS_NUM_2 CONFIG_RTC_MAX_RTCS_NUM
#endif
#ifndef CONFIG_RTC_MAX_RTCS_NUM_3
#define CONFIG_RTC_MAX_RTCS_NUM_3 CONFIG_RTC_MAX_RTCS_NUM
#endif
#ifndef CONFIG_RTC_MAX_RTCS_NUM_EXTRA
#define CONFIG_RTC_MAX_RTCS_NUM_EXTRA CONFIG_RTC_MAX_RTCS_NUM
#endif
#endif

typedef enum rtc_flag {
    RTC_FLAG_NORMAL,
    RTC_FLAG_PERMANENT
} rtc_flag_t;

typedef struct rtc_info {
    rtc_handle_t        key;
    rtc_index_t         index;
    bool                enable;
    bool                is_run;
    rtc_flag_t          flag;
    uint64_t            cycle;
    rtc_callback_t      callback;
    uintptr_t           data;
} rtc_info_t;

typedef struct soft_rtc_cfg {
    uintptr_t   rtc_addr;
    uint8_t     rtc_num;
} soft_rtc_cfg_t;

#define CUR_MAX_HARDWARE_RTC_NUM 4
#define RTC_SIZE_0 0
#define RTC_SIZE_1 1
#define RTC_SIZE_2 2
#define RTC_SIZE_3 3
#define RTC_SIZE_4 4

#if defined(CONFIG_RTC_MAX_NUM) && (CONFIG_RTC_MAX_NUM > RTC_SIZE_0)
static rtc_info_t g_rtclist_0[CONFIG_RTC_MAX_RTCS_NUM_0];
#endif
#if defined(CONFIG_RTC_MAX_NUM) && (CONFIG_RTC_MAX_NUM > RTC_SIZE_1)
static rtc_info_t g_rtclist_1[CONFIG_RTC_MAX_RTCS_NUM_1];
#endif
#if defined(CONFIG_RTC_MAX_NUM) && (CONFIG_RTC_MAX_NUM > RTC_SIZE_2)
static rtc_info_t g_rtclist_2[CONFIG_RTC_MAX_RTCS_NUM_2];
#endif
#if defined(CONFIG_RTC_MAX_NUM) && (CONFIG_RTC_MAX_NUM > RTC_SIZE_3)
static rtc_info_t g_rtclist_3[CONFIG_RTC_MAX_RTCS_NUM_3];
#endif
#if defined(CONFIG_RTC_MAX_NUM) && (CONFIG_RTC_MAX_NUM > RTC_SIZE_4)
static rtc_info_t g_rtclist_extra[CONFIG_RTC_MAX_NUM - CUR_MAX_HARDWARE_RTC_NUM][CONFIG_RTC_MAX_RTCS_NUM_EXTRA];
#endif
typedef struct rtcs_manager {
    uint64_t            last_rtc_load_cycle;
    rtc_info_t         *rtcs;
    uint32_t            rtc_irq_id;
    uint16_t            rtc_int_priority;
    bool                in_rtc_irq;
    uint8_t             soft_rtc_num;
#if defined(CONFIG_RTC_USING_OLD_VERSION)
    int32_t             rtc_load_cycle_deviation;
#endif
} rtcs_manager_t;

static bool g_rtc_inited = false;
static bool g_rtc_adapted[CONFIG_RTC_MAX_NUM];
rtcs_manager_t g_rtcs_manager[CONFIG_RTC_MAX_NUM] = { 0 };


/* 计算对齐后的cycle和偏差值 */
STATIC uint64_t rtc_get_cycle_with_mask(rtc_index_t index, uint64_t cycle, int32_t *cycle_deviation)
{
#if defined(CONFIG_RTC_USING_OLD_VERSION)
    /* 内部函数, 不进行空指针检查 */
    int32_t rtc_load_cycle_deviation = g_rtcs_manager[index].rtc_load_cycle_deviation;

    /* cycle值小于等于0xF, 或当前累积偏差值不到0xF时, cycle向上对齐到0xF */
    if (cycle <= RTC_OLD_VERSION_LOAD_COUNT_MASK
        || rtc_load_cycle_deviation <= (int32_t)RTC_OLD_VERSION_LOAD_COUNT_MASK) {
        *cycle_deviation =
            (int32_t)RTC_OLD_VERSION_LOAD_COUNT_MASK - (int32_t)(cycle & RTC_OLD_VERSION_LOAD_COUNT_MASK);
        return (cycle | RTC_OLD_VERSION_LOAD_COUNT_MASK);
    }

    /* 当前累积偏差值超过0xF时, cycle向下对齐到(0xF - 0x10) */
    *cycle_deviation = -1 - (int32_t)(cycle & RTC_OLD_VERSION_LOAD_COUNT_MASK);
    return (cycle | RTC_OLD_VERSION_LOAD_COUNT_MASK) - (RTC_OLD_VERSION_LOAD_COUNT_MASK + 1);
#else
    unused(index);
    unused(cycle_deviation);
    return cycle;
#endif
}

/* load对齐后的cycle和偏差值 */
STATIC void rtc_load_cycle_with_mask(rtc_index_t index, uint64_t cycle_with_mask, int32_t cycle_deviation)
{
#if defined(CONFIG_RTC_USING_OLD_VERSION)
    g_rtcs_manager[index].rtc_load_cycle_deviation += cycle_deviation;
#else
    unused(index);
    unused(cycle_deviation);
#endif
    hal_rtc_config_load(index, cycle_with_mask);
}

/* Reduces the rtc left for all the rtcs by rtc us amount */
STATIC bool rtc_update_rtcs_rtc(rtc_index_t index, uint64_t update_amount_cycle)
{
    bool has_new_rtc = false;
    uint32_t irq_sts = osal_irq_lock();
    uint8_t soft_rtc_num = g_rtcs_manager[index].soft_rtc_num;
    for (uint32_t i = 0; i < soft_rtc_num; i++) {
        if (g_rtcs_manager[index].rtcs[i].cycle <= update_amount_cycle) {
            g_rtcs_manager[index].rtcs[i].cycle = 0;
            if (g_rtcs_manager[index].rtcs[i].is_run == true) {
                has_new_rtc = true;
            }
        } else {
            g_rtcs_manager[index].rtcs[i].cycle -= update_amount_cycle;
        }
    }
    osal_irq_restore(irq_sts);
    return has_new_rtc;
}

/*
 * Loops through the rtcs to see if any need to be processed
 */
STATIC bool rtc_process_rtcs(rtc_index_t index)
{
    bool re_run = false;
    uint64_t start_rtc = uapi_tcxo_get_count();
    uint8_t soft_rtc_num = g_rtcs_manager[index].soft_rtc_num;
    for (uint32_t i = 0; i < soft_rtc_num; i++) {
        if (g_rtcs_manager[index].rtcs[i].enable &&
            g_rtcs_manager[index].rtcs[i].cycle == 0 &&
            g_rtcs_manager[index].rtcs[i].is_run) {
            g_rtcs_manager[index].rtcs[i].is_run = false;
            if (g_rtcs_manager[index].rtcs[i].callback) {
                g_rtcs_manager[index].rtcs[i].callback(g_rtcs_manager[index].rtcs[i].data);
            }
            re_run = true;
        }
    }

    if (re_run) {
        uint64_t end_rtc = uapi_tcxo_get_count();
        uint64_t diff = end_rtc - start_rtc;
        uint64_t cycle = 0;
        cycle = rtc_porting_compensat_by_tcxo(diff);
        re_run = rtc_update_rtcs_rtc(index, cycle);
    }

    return re_run;
}

/*
 * Gets the index of the next rtc to expire
 */
STATIC uint32_t rtc_get_next_rtc(rtc_index_t index)
{
    uint64_t shortest_rtc_found = UINT64_MAX;
    uint8_t soft_rtc_num = g_rtcs_manager[index].soft_rtc_num;
    uint32_t rtc_found = soft_rtc_num;
    for (uint8_t i = 0; i < soft_rtc_num; i++) {
        if (g_rtcs_manager[index].rtcs[i].enable &&
            g_rtcs_manager[index].rtcs[i].cycle <= shortest_rtc_found &&
            g_rtcs_manager[index].rtcs[i].is_run) {
            shortest_rtc_found = g_rtcs_manager[index].rtcs[i].cycle;
            rtc_found = i;
        }
    }
    return rtc_found;
}

/*
 * Gets the rtc in us till the next interrupt, MAX_UINT32 indicates that no rtc is set
 */
STATIC uint64_t rtc_get_rtc_till_interrupt(rtc_index_t index)
{
    uint64_t current_value = hal_rtc_get_current_value(index);
    if (current_value != 0) {
        return current_value;
    }

    uint32_t next_rtc = rtc_get_next_rtc(index);
    if (next_rtc >= g_rtcs_manager[index].soft_rtc_num) {
        return UINT64_MAX;
    }

    uint64_t cycle_with_mask;
    int32_t cycle_deviation = 0;
    cycle_with_mask = rtc_get_cycle_with_mask(index, g_rtcs_manager[index].rtcs[next_rtc].cycle, &cycle_deviation);
    if (g_rtcs_manager[index].in_rtc_irq) {
        return cycle_with_mask;
    }

    hal_rtc_stop(index);
    rtc_load_cycle_with_mask(index, cycle_with_mask, cycle_deviation);
    hal_rtc_start(index);
    return cycle_with_mask;
}

/*
 * Searches through the rtcs to find the one with the lowest callback rtc
 */
STATIC void rtc_set_next_rtc_interrupt(rtc_index_t index)
{
    uint32_t irq_sts = osal_irq_lock();
    hal_rtc_stop(index);
    uint32_t rtc_found = rtc_get_next_rtc(index);
    if (rtc_found < g_rtcs_manager[index].soft_rtc_num) {
        uint64_t cycle_with_mask;
        int32_t cycle_deviation = 0;
        cycle_with_mask =
            rtc_get_cycle_with_mask(index, g_rtcs_manager[index].rtcs[rtc_found].cycle, &cycle_deviation);
        rtc_load_cycle_with_mask(index, cycle_with_mask, cycle_deviation);
        hal_rtc_start(index);
        g_rtcs_manager[index].last_rtc_load_cycle = cycle_with_mask;
    }
    osal_irq_restore(irq_sts);
}

STATIC void rtc_int_callback(rtc_index_t index)
{
    uint32_t irq_sts = osal_irq_lock();
    g_rtcs_manager[index].in_rtc_irq = true;
    rtc_update_rtcs_rtc(index, g_rtcs_manager[index].last_rtc_load_cycle);
    g_rtcs_manager[index].last_rtc_load_cycle = 0;
    osal_irq_restore(irq_sts);

    while (rtc_process_rtcs(index)) {
    }

    rtc_set_next_rtc_interrupt(index);
    g_rtcs_manager[index].in_rtc_irq = false;
}

STATIC errcode_t soft_rtc_list_cfg_get(uint32_t index, soft_rtc_cfg_t *rtc_info)
{
    switch (index) {
#if defined(CONFIG_RTC_MAX_NUM) && (CONFIG_RTC_MAX_NUM > RTC_SIZE_0)
        case RTC_SIZE_0:
            rtc_info->rtc_num = CONFIG_RTC_MAX_RTCS_NUM_0;
            rtc_info->rtc_addr = (uintptr_t)g_rtclist_0;
            break;
#endif
#if defined(CONFIG_RTC_MAX_NUM) && (CONFIG_RTC_MAX_NUM > RTC_SIZE_1)
        case RTC_SIZE_1:
            rtc_info->rtc_num = CONFIG_RTC_MAX_RTCS_NUM_1;
            rtc_info->rtc_addr = (uintptr_t)g_rtclist_1;
            break;
#endif
#if defined(CONFIG_RTC_MAX_NUM) && (CONFIG_RTC_MAX_NUM > RTC_SIZE_2)
        case RTC_SIZE_2:
            rtc_info->rtc_num = CONFIG_RTC_MAX_RTCS_NUM_2;
            rtc_info->rtc_addr = (uintptr_t)g_rtclist_2;
            break;
#endif
#if defined(CONFIG_RTC_MAX_NUM) && (CONFIG_RTC_MAX_NUM > RTC_SIZE_3)
        case RTC_SIZE_3:
            rtc_info->rtc_num = CONFIG_RTC_MAX_RTCS_NUM_3;
            rtc_info->rtc_addr = (uintptr_t)g_rtclist_3;
            break;
#endif
        default:
#if defined(CONFIG_RTC_MAX_NUM) && (CONFIG_RTC_MAX_NUM > RTC_SIZE_4)
            rtc_info->rtc_num = CONFIG_RTC_MAX_RTCS_NUM_EXTRA;
            rtc_info->rtc_addr = (uintptr_t)&g_rtclist_extra[index - CUR_MAX_HARDWARE_RTC_NUM];
#endif
            break;
    }
    if (memset_s((uint8_t *)rtc_info->rtc_addr, sizeof(rtc_info_t) * rtc_info->rtc_num, 0,
        sizeof(rtc_info_t) * rtc_info->rtc_num) != EOK) {
        return ERRCODE_MEMSET;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_rtc_init(void)
{
    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(g_rtc_inited)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }

    if (unlikely(memset_s(&g_rtcs_manager, sizeof(g_rtcs_manager), 0, sizeof(g_rtcs_manager)) != EOK)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_MEMSET;
    }

    for (uint32_t i = 0; i < CONFIG_RTC_MAX_NUM; i++) {
        soft_rtc_cfg_t rtc_info;
        uint32_t ret;
        g_rtc_adapted[i] = false;
        ret = soft_rtc_list_cfg_get(i, &rtc_info);
        if (unlikely(ret != ERRCODE_SUCC)) {
            osal_irq_restore(irq_sts);
            return ret;
        }
        g_rtcs_manager[i].soft_rtc_num = rtc_info.rtc_num;
        g_rtcs_manager[i].rtcs = (rtc_info_t *)rtc_info.rtc_addr;
    }

    g_rtc_inited = true;
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_rtc_adapter(rtc_index_t index, uint32_t int_id, uint16_t int_priority)
{
    if (unlikely(index >= RTC_MAX_NUM)) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(g_rtc_inited == 0)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_RTC_NOT_INITED;
    }

    if (unlikely(g_rtc_adapted[index] != 0)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }

    errcode_t ret = hal_rtc_init(index, rtc_int_callback);
    if (ret != ERRCODE_SUCC) {
        osal_irq_restore(irq_sts);
        return ret;
    }

    rtc_port_register_irq(index, int_id, int_priority);

    g_rtcs_manager[index].rtc_irq_id = int_id;
    g_rtcs_manager[index].rtc_int_priority = int_priority;

    g_rtc_adapted[index] = true;
    osal_irq_restore(irq_sts);

    return ERRCODE_SUCC;
}

errcode_t uapi_rtc_deinit(void)
{
    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(!g_rtc_inited)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }

    for (uint32_t i = 0; i < CONFIG_RTC_MAX_NUM; i++) {
        if (g_rtc_adapted[i]) {
            hal_rtc_stop(i);
            rtc_port_unregister_irq(i, g_rtcs_manager[i].rtc_irq_id);
        }
    }

    if (unlikely(memset_s(&g_rtcs_manager, sizeof(g_rtcs_manager), 0, sizeof(g_rtcs_manager)) != EOK)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_MEMSET;
    }

    for (uint32_t i = 0; i < CONFIG_RTC_MAX_NUM; i++) {
        if (g_rtc_adapted[i]) {
            hal_rtc_deinit(i);
        }
    }

    g_rtc_inited = false;
    for (uint32_t i = 0; i < CONFIG_RTC_MAX_NUM; i++) {
        g_rtc_adapted[i] = false;
    }
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_rtc_create(rtc_index_t index, rtc_handle_t *rtc)
{
    if (unlikely(index >= RTC_MAX_NUM || rtc == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t irq_sts = osal_irq_lock();
    for (uint32_t i = 0; i < g_rtcs_manager[index].soft_rtc_num; i++) {
        if (g_rtcs_manager[index].rtcs[i].enable == false) {
            g_rtcs_manager[index].rtcs[i].enable = true;
            g_rtcs_manager[index].rtcs[i].is_run = false;
            g_rtcs_manager[index].rtcs[i].index = index;
            g_rtcs_manager[index].rtcs[i].key = &g_rtcs_manager[index].rtcs[i];

            *rtc = g_rtcs_manager[index].rtcs[i].key;
            osal_irq_restore(irq_sts);
            return ERRCODE_SUCC;
        }
    }

    *rtc = NULL;
    osal_irq_restore(irq_sts);
    return ERRCODE_RTC_NO_ENOUGH;
}

errcode_t uapi_rtc_delete(rtc_handle_t rtc)
{
    if (unlikely(rtc == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

    rtc_info_t *rtc_info = (rtc_info_t *)rtc;
    uint32_t irq_sts = osal_irq_lock();
    rtc_info->enable = false;
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

uint32_t uapi_rtc_get_max_ms(void)
{
    return RTC_MAX_MS;
}

errcode_t uapi_rtc_start(rtc_handle_t rtc, uint32_t rtc_ms, rtc_callback_t callback, uintptr_t data)
{
    rtc_index_t index;
    uint32_t rtc_max_ms = RTC_MAX_MS;
    if (unlikely(rtc == NULL) || unlikely(callback == NULL) || unlikely(rtc_ms == 0) || unlikely(rtc_ms > rtc_max_ms)) {
        return ERRCODE_INVALID_PARAM;
    }

    rtc_info_t *rtc_info = (rtc_info_t *)rtc;
    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(rtc_info->enable == false)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_RTC_NOT_CREATED;
    }

    uint64_t cycle;
    int32_t cycle_deviation = 0;
    index = rtc_info->index;
    cycle = rtc_get_cycle_with_mask(index, rtc_porting_ms_2_cycle(rtc_ms), &cycle_deviation);
    rtc_info->cycle = cycle;
    rtc_info->callback = callback;
    rtc_info->data = data;

    uint64_t remain_rtc_cycle = rtc_get_rtc_till_interrupt(index);
    if (cycle <= remain_rtc_cycle) {
        if (remain_rtc_cycle != UINT64_MAX && g_rtcs_manager[index].last_rtc_load_cycle != 0) {
            rtc_update_rtcs_rtc(index, g_rtcs_manager[index].last_rtc_load_cycle - remain_rtc_cycle);
        }
        g_rtcs_manager[index].last_rtc_load_cycle = cycle;
        hal_rtc_stop(index);
        rtc_load_cycle_with_mask(index, cycle, cycle_deviation);
        hal_rtc_start(index);
    } else {
        rtc_info->cycle += (g_rtcs_manager[index].last_rtc_load_cycle - remain_rtc_cycle);
    }

    rtc_info->is_run = true;
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_rtc_stop(rtc_handle_t rtc)
{
    if (unlikely(rtc == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

    rtc_info_t *rtc_info = (rtc_info_t *)rtc;

    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(rtc_info->enable == false)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_RTC_NOT_CREATED;
    }

    if (unlikely(rtc_info->is_run == false)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }

    rtc_info->is_run = false;
    rtc_info->cycle = 0;
    rtc_info->callback = NULL;
    rtc_info->data = 0;

    /* 检查当前是否还有在运行的定时器, 若没有定时器则停止硬件计时 */
    rtc_index_t index = rtc_info->index;
    uint32_t rtc_found = rtc_get_next_rtc(index);
    if (rtc_found >= g_rtcs_manager[index].soft_rtc_num) {
        hal_rtc_stop(index);
    }
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

uint32_t uapi_rtc_int_cnt_record_get(rtc_index_t index)
{
    if (unlikely(index >= RTC_MAX_NUM)) {
        return 0;
    }

    uint32_t int_cnt_record = hal_rtc_get_int_cnt_record();
    return int_cnt_record;
}

errcode_t uapi_rtc_get_current_time_count(rtc_index_t index, uint64_t *current_time_count)
{
    if (unlikely(index >= RTC_MAX_NUM || current_time_count == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t irq_sts = osal_irq_lock();
    uint64_t current_value = hal_rtc_get_current_value(index);
    osal_irq_restore(irq_sts);
    *current_time_count = current_value;
    return ERRCODE_SUCC;
}

errcode_t uapi_rtc_get_current_time_us(rtc_index_t index, uint32_t *current_time_us)
{
    if (unlikely(index >= RTC_MAX_NUM || current_time_us == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t irq_sts = osal_irq_lock();
    uint64_t current_value = hal_rtc_get_current_value(index);
    osal_irq_restore(irq_sts);
    *current_time_us = rtc_porting_cycle_2_us(current_value);
    return ERRCODE_SUCC;
}

errcode_t uapi_rtc_start_hw_rtc(rtc_index_t index, uint64_t rtc_ms)
{
    if (unlikely(index >= RTC_MAX_NUM)) {
        return ERRCODE_INVALID_PARAM;
    }

    uint64_t rtc_max_ms = RTC_HW_MAX_MS;
    if (unlikely(rtc_ms == 0) || unlikely(rtc_ms > rtc_max_ms)) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t irq_sts = osal_irq_lock();
    hal_rtc_stop(index);
#if defined(CONFIG_RTC_USING_OLD_VERSION)
    hal_rtc_config_load(index, rtc_hw_porting_ms_2_cycle(rtc_ms) | RTC_OLD_VERSION_LOAD_COUNT_MASK);
#else
    hal_rtc_config_load(index, rtc_hw_porting_ms_2_cycle(rtc_ms));
#endif
    hal_rtc_start(index);
    osal_irq_restore(irq_sts);

    return ERRCODE_SUCC;
}

errcode_t uapi_rtc_stop_hw_rtc(rtc_index_t index)
{
    if (unlikely(index >= RTC_MAX_NUM)) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t irq_sts = osal_irq_lock();
    hal_rtc_stop(index);
    osal_irq_restore(irq_sts);

    return ERRCODE_SUCC;
}

#if defined(CONFIG_RTC_SUPPORT_LPM)
uint32_t uapi_rtc_get_latest_timeout(void)
{
    uint64_t nxt_count = UINT64_MAX;
    uint64_t cur_count = UINT64_MAX;
    for (uint32_t index = 0; index < RTC_MAX_NUM; index++) {
        if (g_rtc_adapted[index] == false) {
            continue;
        }
        cur_count = hal_rtc_get_current_value(index);
        /* 值为0分为两种情况：timer已全部过期或在功耗调度中到期，可通过中断状态区分。前者视为超时时间为U64最大值 */
        if (cur_count == 0 && hal_rtc_get_int_status(index) == 0) {
            cur_count = UINT64_MAX;
        }
        nxt_count = cur_count < nxt_count ? cur_count : nxt_count;
    }
    uint32_t nxt_ms = rtc_porting_cycle_2_ms(nxt_count);
    return nxt_ms;
}

errcode_t uapi_rtc_suspend(uintptr_t val)
{
    unused(val);
    uint32_t int_sts = osal_irq_lock();
    for (uint32_t index = 0; index < RTC_MAX_NUM; index++) {
        if (g_rtc_adapted[index] == false) {
            continue;
        }
        rtc_update_rtcs_rtc(index,
            g_rtcs_manager[index].last_rtc_load_cycle - hal_rtc_get_current_value(index));
        g_rtcs_manager[index].last_rtc_load_cycle = 0;
        rtc_set_next_rtc_interrupt(index);
    }
    osal_irq_restore(int_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_rtc_resume(uintptr_t val)
{
    uint32_t int_sts = osal_irq_lock();
    uint64_t compansation_count = *(uint64_t *)val;
    for (uint32_t index = 0; index < RTC_MAX_NUM; index++) {
        if (g_rtc_adapted[index] == false) {
            continue;
        }
        g_rtcs_manager[index].last_rtc_load_cycle = compansation_count;
        hal_rtc_stop(index);
        hal_rtc_config_load(index, rtc_porting_ms_2_cycle(1));
        hal_rtc_start(index);
    }
    osal_irq_restore(int_sts);
    return ERRCODE_SUCC;
}
#endif /* CONFIG_RTC_SUPPORT_LPM */