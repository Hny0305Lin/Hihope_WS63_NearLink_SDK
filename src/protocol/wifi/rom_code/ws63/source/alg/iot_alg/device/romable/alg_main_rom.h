/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description:  Entry, initialization, and external interface definition of the algorithm module, depending on the DMAC
 */

#ifndef ALG_MAIN_ROM_H
#define ALG_MAIN_ROM_H

/******************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "oam_ext_if.h"
#include "frw_ext_if_rom.h"
#include "frw_dmac_rom.h"
#include "frw_msg_rom.h"
#include "dmac_ext_if_rom.h"
#include "dmac_alg_if_part_rom.h"
#include "alg_common_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/******************************************************************************
  2 宏定义
******************************************************************************/
#define ALG_RSSI_MARGIN_DB 4                   /* RSSI余量，使用rssi计算时使用 */

#define alg_stru_elem_offset(_stru, _elem) ((osal_u16)((osal_u8 *)(&(((_stru *)0)->_elem)) - (osal_u8 *)0))

/* 遍历VAP/USER操作封装 */
#define alg_device_foreach_vap(_pst_vap, _pst_device, _uc_vap_index)                  \
    for ((_uc_vap_index) = 0, (_pst_vap) = ((_pst_device)->vap_num > 0) ?             \
        ((dmac_vap_stru *)mac_res_get_dmac_vap((_pst_device)->vap_id[0])) :             \
        OSAL_NULL;                                                                      \
        (_uc_vap_index) < (_pst_device)->vap_num;                                     \
        (_uc_vap_index)++, (_pst_vap) = ((_uc_vap_index) < (_pst_device)->vap_num) ?  \
        ((dmac_vap_stru *)mac_res_get_dmac_vap((_pst_device)->vap_id[_uc_vap_index])) : \
        OSAL_NULL)                                                                      \
        if ((_pst_vap) != OSAL_NULL)
#define alg_vap_foreach_user(_pst_user, _pst_vap, _pst_list_pos)                                           \
    for ((_pst_list_pos) = (_pst_vap)->mac_user_list_head.next,                                        \
        (_pst_user) = osal_list_entry((_pst_list_pos), dmac_user_stru, user_dlist);                     \
        ((_pst_list_pos) != &((_pst_vap)->mac_user_list_head)) && ((_pst_list_pos) != OSAL_NULL); \
        (_pst_list_pos) = (_pst_list_pos)->next,                                                  \
        (_pst_user) = osal_list_entry((_pst_list_pos), dmac_user_stru, user_dlist))                     \
        if ((_pst_user) != OSAL_NULL)

typedef osal_u32 (*alg_dmac_internal_hook_init_cb)(void);
typedef osal_u32 (*alg_dmac_internal_hook_exit_cb)(void);
typedef osal_u32 (*alg_dmac_config_param_cb)(dmac_vap_stru *dmac_vap, frw_msg *msg);

/******************************************************************************
  3 枚举定义
******************************************************************************/

/******************************************************************************
  4 STRUCT定义
******************************************************************************/
typedef osal_u32 (*p_alg_dmac_co_intf_notify_func)(hal_to_dmac_device_stru *hal_device,
    alg_intf_det_notify_info_stru *intf_det_notify);

typedef osal_u32 (*p_alg_dmac_distance_notify_func)(const dmac_user_stru *dmac_user,
    const dmac_alg_distance_notify_info_stru *distance_info);

typedef osal_u32 (*p_alg_dmac_intfdet_lossthr_notify_func)(hal_to_dmac_device_stru *hal_device);

typedef osal_u32 (*p_alg_dmac_param_sync_notify_func)(const alg_param_sync_stru *alg_sync);

typedef osal_u32 (*p_alg_dbac_config_func)(dmac_vap_stru *dmac_vap, frw_msg *msg);

/* 钩子函数指针数组结构体 */
typedef struct {
    p_alg_dmac_distance_notify_func pa_distance_notify_func[ALG_DISTANCE_NOTIFY_BUTT];
    p_alg_dmac_co_intf_notify_func pa_co_intf_notify_func[ALG_CO_INTF_NOTIFY_BUTT];
    osal_u8 *rom;
} alg_dmac_internal_hook_stru;

typedef osal_void *(*alg_dmac_mem_alloc_cb)(alg_mem_enum_uint8 alg_id, osal_u16 len);

/******************************************************************************
  5 函数声明
******************************************************************************/
osal_void *alg_dmac_mem_alloc(alg_mem_enum_uint8 alg_id, osal_u16 len);
/* 距离状态变化钩子类型注册、注销及通知接口 */
osal_void alg_register_distance_notify_func(alg_distance_notify_enum_uint8 notify_sub_type,
    p_alg_dmac_distance_notify_func func);
osal_u32 alg_unregister_distance_notify_func(alg_distance_notify_enum_uint8 notify_sub_type);
osal_u32 alg_distance_notify(dmac_user_stru *dmac_user, dmac_alg_distance_notify_info_stru *distance_info);

osal_s32 alg_dmac_config_param(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_s32 alg_dmac_config_multi_param(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_s32 alg_vap_up_process(dmac_vap_stru *dmac_vap, frw_msg *msg);
#ifdef _PRE_WLAN_FEATURE_INTRF_MODE
osal_s32 alg_dmac_anti_intrf(dmac_vap_stru *dmac_vap, frw_msg *msg);
#endif
/* 同频干扰状态变化钩子类型注册、注销及通知接口 */
osal_u32 alg_dmac_register_co_intf_notify_func(alg_co_intf_notify_enum_uint8 notify_sub_type,
    p_alg_dmac_co_intf_notify_func func);
osal_u32 alg_unregister_co_intf_notify_func(alg_co_intf_notify_enum_uint8 notify_sub_type);

osal_u32 alg_dmac_co_intf_notify(hal_to_dmac_device_stru *hal_device, alg_intf_det_notify_info_stru *intf_det_notify);

osal_void alg_get_vap_and_device_user_num(hal_to_dmac_device_stru *hal_device, const dmac_vap_stru *dmac_vap_cmp,
    osal_u8 *vap_user_num, osal_u8 *device_user_num);
osal_void alg_get_device_user_num_index(hal_to_dmac_device_stru *hal_device,
    osal_u8 *device_user_num, osal_u8 *first_user_lut_idx);

osal_u32 alg_dmac_register_dbac_config_func(p_alg_dbac_config_func func);
osal_u32 alg_unregister_dbac_config_func(osal_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of alg_main_rom.h */
