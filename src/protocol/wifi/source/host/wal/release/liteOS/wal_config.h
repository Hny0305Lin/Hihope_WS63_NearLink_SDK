/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2023. All rights reserved.
 * Description: wal_config.c 的头文件
 * Create: 2012年11月6日
 */

#ifndef __WAL_CONFIG_H__
#define __WAL_CONFIG_H__

#include "oal_ext_if.h"
#include "wlan_types_common.h"
#include "mac_vap_ext.h"
#include "frw_ext_if.h"
#include "osal_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_CONFIG_H
/*****************************************************************************
  2 宏定义
*****************************************************************************/
typedef osal_u32  (*wal_config_get_func)(hmac_vap_stru *mac_vap, osal_u16 *pus_len, osal_u8 *param);
typedef osal_u32  (*wal_config_set_func)(hmac_vap_stru *mac_vap, osal_u16 len, osal_u8 *param);

#define WAL_BCAST_MAC_ADDR       255
#define WAL_MAX_RATE_NUM        16


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/* 速率集种类，常发使用 */
typedef enum {
    WAL_RF_TEST_11B_LEGACY_RATES,
    WAL_RF_TEST_20M_NORMAL_RATES,
    WAL_RF_TEST_20M_SHORT_GI_RATES,
    WAL_RF_TEST_40M_NORMAL_RATES,
    WAL_RF_TEST_40M_SHORT_GI_RATES,

    WAL_RF_TEST_RATES_BUTT
} wal_rf_test_enum;
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
    hmac_vap_stru                *mac_vap;
    osal_s8                     pc_param[4];      /* 查询或配置信息 */
}wal_event_stru;

/* WID对应的操作 */
typedef struct {
    wlan_cfgid_enum_uint16  cfgid;                /* wid枚举 */
    oal_bool_enum_uint8     reset;              /* 是否复位 */
    osal_u8               auc_resv[1];
    wal_config_get_func     p_get_func;            /* get函数 */
    wal_config_set_func     p_set_func;            /* set函数 */
}wal_wid_op_stru;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 宏定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/
osal_void wal_cfg_init(osal_void);

#ifdef _PRE_SUPPORT_ACS
extern osal_u32  wal_acs_netlink_recv_handle(frw_event_mem_stru *event_mem);
#endif
extern osal_u32  wal_config_get_wmm_params_etc(oal_net_device_stru *net_dev, osal_u8 *param);

extern osal_u32 hmac_config_send_event_etc(hmac_vap_stru *mac_vap,
    wlan_cfgid_enum_uint16 cfg_id, osal_u16 len,
    osal_u8 *param);
extern osal_u32 wal_send_cali_data_etc(oal_net_device_stru *net_dev);

#ifdef _PRE_WLAN_FEATURE_DFS
extern osal_u32  wal_config_get_dfs_chn_status(oal_net_device_stru *net_dev, osal_u8 *param);
#endif
#ifdef __cplusplus
#if __cplusplus
        }
#endif
#endif

#endif /* end of wal_config.h */
