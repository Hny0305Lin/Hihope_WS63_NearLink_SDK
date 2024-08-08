/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file of HAL power save public interfaces .
 */

#ifndef HAL_POWER_SAVE_H
#define HAL_POWER_SAVE_H

#include "hal_common_ops_rom.h"

/*****************************************************************************
 * 1 其他头文件包含
 *****************************************************************************/
/*****************************************************************************
 * STA ps feature
 *****************************************************************************/
/*****************************************************************************
 * 功能描述  : 设置MAC_P2P_NOA_CTRL寄存器中TWT的唤醒参数
 *****************************************************************************/
static INLINE__ osal_void hal_vap_set_twt(const hal_to_dmac_vap_stru *hal_vap, const twt_reg_param_stru *twt_param,
    osal_u8 count, osal_u8 trigger)
{
    hal_public_hook_func(_vap_set_twt)(hal_vap, twt_param, count, trigger);
}

/*****************************************************************************
 * 功能描述  : 设置MAC_P2P_NOA_CTRL寄存器中TWT的校准参数
 *****************************************************************************/
static INLINE__ osal_void hal_cali_twt(const hal_to_dmac_vap_stru *hal_vap)
{
    hal_public_hook_func(_cali_twt)(hal_vap);
}

#ifdef _PRE_WLAN_FEATURE_MAC_PARSE_TIM
/*****************************************************************************
 * 功能描述  : 设置bcn tim ie的偏移地址寄存器
 *****************************************************************************/
static INLINE__ osal_void hal_mac_set_bcn_tim_pos(const hal_to_dmac_vap_stru *hal_vap, osal_u16 pos)
{
    hal_public_hook_func(_mac_set_bcn_tim_pos)(hal_vap, pos);
}

/*****************************************************************************
 * 功能描述  : 设置硬件解析beacon开关
 *****************************************************************************/
static INLINE__ osal_void hal_pm_set_mac_parse_tim(osal_u8 flag)
{
    hal_public_hook_func(_pm_set_mac_parse_tim)(flag);
}

/*****************************************************************************
 * 功能描述  : 设置mac aid寄存器
 *****************************************************************************/
static INLINE__ osal_void hal_set_mac_aid(const hal_to_dmac_vap_stru *hal_vap, osal_u16 aid)
{
    hal_public_hook_func(_set_mac_aid)(hal_vap, aid);
}
#endif

/*****************************************************************************
 * 功能描述  : 设置PSM寄存器中listen_interval的值
 *****************************************************************************/
static INLINE__ osal_void hal_set_psm_listen_interval(const hal_to_dmac_vap_stru *hal_vap, osal_u16 interval)
{
    hal_public_hook_func(_set_psm_listen_interval)(hal_vap, interval);
}

/*****************************************************************************
 * 功能描述  : 设置PSM寄存器中listen_interval_count的值
 *****************************************************************************/
static INLINE__ osal_void hal_set_psm_listen_interval_count(hal_to_dmac_vap_stru *hal_vap, osal_u16 interval_count)
{
    hal_public_hook_func(_set_psm_listen_interval_count)(hal_vap, interval_count);
}

/*****************************************************************************
 * 功能描述  : 设置PSM寄存器中tbtt offset的值
 *****************************************************************************/
static INLINE__ osal_void hal_set_tbtt_offset(const hal_to_dmac_vap_stru *hal_vap, osal_u16 offset)
{
    hal_public_hook_func(_set_tbtt_offset)(hal_vap, offset);
}

#ifdef _PRE_WLAN_FEATURE_POWERSAVE
/*****************************************************************************
 * 功能描述  : 设置PSM寄存器中ext tbtt offset的值
 *****************************************************************************/
static INLINE__ osal_void hal_set_psm_ext_tbtt_offset(const hal_to_dmac_vap_stru *hal_vap, osal_u16 offset)
{
    hal_public_hook_func(_set_psm_ext_tbtt_offset)(hal_vap, offset);
}
#endif
/*****************************************************************************
 * 功能描述  : 设置beacon period寄存器
 *****************************************************************************/
static INLINE__ osal_void  hal_vap_set_psm_beacon_period(const hal_to_dmac_vap_stru *hal_vap, osal_u32 beacon_period)
{
    hal_public_hook_func(_set_psm_beacon_period)(hal_vap, beacon_period);
}

/*****************************************************************************
 * 功能描述  : 设置beacon等待的超时值
 *****************************************************************************/
static INLINE__ osal_void hal_set_beacon_timeout(osal_u16 value)
{
    hal_public_hook_func(_set_beacon_timeout)(value);
}

/*****************************************************************************
 * 功能描述  : 设置DTIM寄存器
 *****************************************************************************/
static INLINE__ osal_void hal_set_sta_dtim_period(const hal_to_dmac_vap_stru *hal_vap, osal_u32 dtim_period)
{
    hal_public_hook_func(_set_sta_dtim_period)(hal_vap, dtim_period);
}

/*****************************************************************************
 函 数 名  : hal_get_psm_dtim_count
 功能描述  : 获取ditm count寄存器
*****************************************************************************/
static INLINE__ osal_u8 hal_get_psm_dtim_count(const hal_to_dmac_vap_stru *hal_vap)
{
    return hal_public_hook_func(_get_psm_dtim_count)(hal_vap);
}

/*****************************************************************************
 函 数 名  : hal_get_psm_dtim_count
 功能描述  : 获取ditm count寄存器
*****************************************************************************/
static INLINE__ osal_void hal_set_psm_dtim_count(hal_to_dmac_vap_stru *hal_vap, osal_u8 dtim_count)
{
    hal_public_hook_func(_set_psm_dtim_count)(hal_vap, dtim_count);
}

/*****************************************************************************
 * AP ps feature
 *****************************************************************************/
/*****************************************************************************
 * 功能描述  : 使能sta ps ctrl寄存器
 *****************************************************************************/
static INLINE__ osal_void hal_tx_enable_peer_sta_ps_ctrl(osal_u8 lut_index)
{
    hal_public_hook_func(_tx_enable_peer_sta_ps_ctrl)(lut_index);
}

/*****************************************************************************
 * 功能描述  : 去使能sta ps ctrl寄存器
 *****************************************************************************/
static INLINE__ osal_void hal_tx_disable_peer_sta_ps_ctrl(osal_u8 lut_index)
{
    hal_public_hook_func(_tx_disable_peer_sta_ps_ctrl)(lut_index);
}

/*****************************************************************************
 * 功能描述  : 使能resp_ps_bit_ctrl
 *****************************************************************************/
static INLINE__ osal_void  hal_tx_enable_resp_ps_bit_ctrl(osal_u8 lut_index)
{
    hal_public_hook_func(_tx_enable_resp_ps_bit_ctrl)(lut_index);
}

/*****************************************************************************
 * 功能描述  : 去使能resp_ps_bit_ctrl
 *****************************************************************************/
static INLINE__ osal_void hal_tx_disable_resp_ps_bit_ctrl(osal_u8 lut_index)
{
    hal_public_hook_func(_tx_disable_resp_ps_bit_ctrl)(lut_index);
}

/*****************************************************************************
 * 功能描述  : 使能所有resp_ps_bit_ctrl
 *****************************************************************************/
static INLINE__ osal_void  hal_tx_enable_resp_ps_bit_ctrl_all(osal_void)
{
    hal_public_hook_func(_tx_enable_resp_ps_bit_ctrl_all)();
}

/*****************************************************************************
 * 功能描述  : 去使能所有resp_ps_bit_ctrl
 *****************************************************************************/
static INLINE__ osal_void hal_tx_disable_resp_ps_bit_ctrl_all(osal_void)
{
    hal_public_hook_func(_tx_disable_resp_ps_bit_ctrl_all)();
}
/*****************************************************************************
 * 功能描述  : 去使能所有resp_ps_bit_ctrl
 *****************************************************************************/
static INLINE__ osal_void hal_set_tx_queue_suspend_mode(osal_u8 mask, osal_u8 mode)
{
    hal_public_hook_func(_set_tx_queue_suspend_mode)(mask, mode);
}
#endif /* end of hal_power_save.h */

