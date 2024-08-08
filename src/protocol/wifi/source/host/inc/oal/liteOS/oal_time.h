/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: oal_time.h 的头文件
 * Create: 2019-12-01
 */

#ifndef __OAL_TIME_H__
#define __OAL_TIME_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_types.h"
#include "oal_timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* 获取毫秒级时间戳 */
#define oal_time_get_stamp_ms() oal_get_ms()

#define OAL_TIME_JIFFY    osal_adapt_get_jiffies()
#define oal_msecs_to_jiffies(_msecs)    osal_msecs_to_jiffies(_msecs)
#define oal_jiffies_to_msecs(_jiffies)      osal_adapt_jiffies_to_msecs(_jiffies)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
/*****************************************************************************
  5 消息头定义
*****************************************************************************/
/*****************************************************************************
  6 消息定义
*****************************************************************************/
/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
typedef struct {
    osal_slong i_sec;
    osal_slong i_usec;
} oal_time_us_stru;

// 与内核struct rtc_time 保持一致
typedef struct _oal_time_stru {
    osal_s32 tm_sec; /* seconds */
    osal_s32 tm_min; /* minutes */
    osal_s32 tm_hour; /* hours */
    osal_s32 tm_mday; /* day of the month */
    osal_s32 tm_mon; /* month */
    osal_s32 tm_year; /* year */
    osal_s32 tm_wday; /* day of the week */
    osal_s32 tm_yday; /* day in the year */
    osal_s32 tm_isdst; /* daylight saving time */
} oal_time_stru;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/
/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  10 函数声明
*****************************************************************************/
/*****************************************************************************
 功能描述  : 获取微妙精度级的时间戳
 输入参数  : usec: 时间结构体指针
*****************************************************************************/
static inline osal_u32 oal_get_seconds(osal_void)
{
    osal_timeval tv;
    osal_gettimeofday(&tv);
    return (osal_u32)tv.tv_sec;
}

static inline osal_u64 oal_get_ms(osal_void)
{
    osal_timeval tv;
    osal_gettimeofday(&tv);
    /* ms 单位1000 */
    return (osal_u64)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

static inline osal_u64 oal_get_us(oal_time_us_stru *usec)
{
    osal_timeval tv;
    osal_gettimeofday(&tv);
    usec->i_sec = tv.tv_sec;
    usec->i_usec =  tv.tv_usec;
    /* us 单位1000000 */
    return (osal_u64)(tv.tv_sec * 1000000 + tv.tv_usec);
}

/*****************************************************************************
 功能描述  : 判断时间戳time_a是否在time_b之后:
 输入参数  : hi_u64 time
 返 回 值  : Return: 1 time_a在time_b之后; 否则 Return: 0
*****************************************************************************/
static inline td_u32 oal_time_after(osal_u64 time_a, osal_u64 time_b)
{
    return (td_u32)((osal_s64)((osal_s64)(time_b) - (time_a)) < 0);
}

/*****************************************************************************
 功能描述  : 判断ul_time是否比当前时间早
             若早，表示超时时间已过；若不早，表明还未超时
*****************************************************************************/
static inline td_u32 oal_time_is_before(osal_u64 time)
{
    return oal_time_after(OAL_TIME_JIFFY, time);
}

static inline td_u32 oal_time_before(td_u32 time_a, td_u32 time_b)
{
    return (td_u32)((osal_s64)((osal_s64)(time_a) - (time_b)) < 0);
}

static inline td_void oal_get_real_time(oal_time_stru* pst_tm)
{
    unref_param(pst_tm);
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_time.h */

