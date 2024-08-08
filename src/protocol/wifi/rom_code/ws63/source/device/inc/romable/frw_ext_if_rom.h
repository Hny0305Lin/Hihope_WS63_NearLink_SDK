/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: header file of the FRW module, definition of external public interfaces
 */

#ifndef FRW_EXT_IF_ROM_H
#define FRW_EXT_IF_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_ext_if_rom.h"
#include "oam_ext_if.h"
#include "frw_msg_rom.h"
#include "frw_ext_common_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_FRW_EXT_IF_ROM_H

/*****************************************************************************
  4 消息头定义
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
 函 数 名  : frw_dmac_init_timer
 功能描述  : 定时器初始化
*****************************************************************************/
static INLINE__ osal_void frw_dmac_init_timer(frw_timeout_stru *timer,
    const frw_timeout_func func, osal_void *arg, oal_bool_enum_uint8 periodic)
{
    timer->func = func;
    timer->timeout_arg = arg;
    timer->is_periodic = periodic;
}

osal_void frw_dmac_timer_create_timer(osal_u32 func_p, frw_timeout_stru *timeout, osal_u32 time_val);
osal_void frw_dmac_timer_destroy_timer(osal_u32 func_p, frw_timeout_stru *timeout);

/*****************************************************************************
 函 数 名  : frw_dmac_create_timer
 功能描述  : 创建定时器
*****************************************************************************/
static INLINE__ osal_void frw_dmac_create_timer(frw_timeout_stru *timeout, osal_u32 time_val)
{
    frw_dmac_timer_create_timer(oal_return_addr(), timeout, time_val);
}

/*****************************************************************************
 函 数 名  : frw_dmac_destroy_timer
 功能描述  : 删除定时器
*****************************************************************************/
static INLINE__ osal_void frw_dmac_destroy_timer(frw_timeout_stru *timeout)
{
    frw_dmac_timer_destroy_timer(oal_return_addr(), timeout);
}

/*****************************************************************************
  10 函数声明
*****************************************************************************/
osal_s32 frw_main_init(osal_void);
osal_void frw_main_exit(osal_void);
osal_void frw_set_init_state(frw_init_enum_uint16 init_state);
frw_init_enum_uint16 frw_get_init_state(osal_void);
osal_void frw_dmac_event_flush_event_queue(osal_u16 msg_id);
osal_void frw_dmac_event_vap_flush_event(frw_flush_msg *flush_msg);
osal_void frw_event_flush_callback(osal_u16 event_type);
osal_void frw_dmac_timer_restart_timer(frw_timeout_stru *timeout, osal_u32 timeout_val,
    oal_bool_enum_uint8 is_periodic);
osal_void frw_dmac_timer_add_timer(frw_timeout_stru *timeout);
osal_void frw_dmac_timer_stop_timer(frw_timeout_stru *timeout);
osal_void frw_dmac_timer_dump_timer(osal_void);
osal_u32 frw_dmac_get_large_free_from_ram(osal_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of frw_ext_if_rom.h */
