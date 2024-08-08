/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Header file for mac_cfg.c
 */

#ifndef __SOC_WIFI_DRIVER_API_H__
#define __SOC_WIFI_DRIVER_API_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "mac_vap_ext.h"
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "msg_wow_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  宏定义
*****************************************************************************/
#define MAC_NUM_2G_BAND             3           /* 2g band个数 */
#define MAC_NUM_2G_CH_NUM           13          /* 2g 信道个数 */
#define MAC_API_AUTO_CCA            0x7f       /* CCA自动值 */

/*****************************************************************************
  枚举定义
*****************************************************************************/
typedef enum {
    MAC_API_TPC_OFF_MODE      = 0,
    MAC_API_TPC_PROMOTE_MODE  = 1,
    MAC_API_TPC_AUTO_MODE     = 2,
    MAC_API_TPC_MODE_BUTT,
}mac_cfg_tpc_mode_enum;
typedef td_u8 mac_cfg_tpc_mode_enum_uint8;

typedef enum {
    MAC_API_RTS_AUTO_MODE      = 0,
    MAC_API_RTS_THRES_MODE     = 1,
    MAC_API_RTS_OFF_MODE       = 2,
    MAC_API_RTS_MODE_BUTT,
}mac_cfg_rts_mode_enum;
typedef td_u8 mac_cfg_rts_mode_enum_uint8;

typedef enum {
    MAC_CFG_RETRY_DATA      = 0,
    MAC_CFG_RETRY_MGMT      = 1,
    MAC_CFG_RETRY_TIMEOUT   = 2,
    MAC_CFG_RETRY_TYPE_BUTT,
}mac_cfg_retry_type_enum;
typedef td_u8 mac_cfg_retry_type_enum_uint8;

typedef enum {
    MAC_PSM_OFFSET_TBTT,
    MAC_PSM_OFFSET_EXT_TBTT,
    MAC_PSM_OFFSET_BCN_TIMEOUT,
} mac_cfg_psm_offset_type;
typedef td_u8 mac_cfg_psm_offset_type_uint8;

#ifdef _PRE_WLAN_FEATURE_MESH
typedef enum {
    EXT_PLINK_IDLE = 1,
    EXT_PLINK_OPN_SNT,
    EXT_PLINK_OPN_RCVD,
    EXT_PLINK_CNF_RCVD,
    EXT_PLINK_ESTAB,
    EXT_PLINK_HOLDING,
    EXT_PLINK_BLOCKED,

    EXT_PLINK_BUTT
}mac_mesh_plink_state_enum;
typedef td_u8 mac_mesh_plink_state_enum_uint8;
#endif


#ifdef _PRE_WLAN_FEATURE_MESH
/* mesh节点类型枚举 */
typedef enum {
    MAC_EXT_MESH_UNSPEC = 0, /* 未确定mesh节点角色 */
    MAC_EXT_MESH_STA,        /* Mesh-STA节点角色 */
    MAC_EXT_MESH_MG,         /* Mesh-MG节点角色 */
    MAC_EXT_MESH_MBR,        /* Mesh-MBR节点角色 */

    MAC_EXT_MESH_NODE_BUTT,
} mac_ext_mesh_node_type_enum;
typedef td_u8 mac_ext_mesh_node_type_enum_uint8;
#endif

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
typedef enum {
    MAC_WOW_PATTERN_ADD = 0,
    MAC_WOW_PATTERN_DEL = 1,
    MAC_WOW_PATTERN_CLR = 2,
    MAC_WOW_PATTERN_BUTT,
} mac_wow_pattern_type_enum;
typedef osal_u8 mac_wow_pattern_type_enum_uint8;
#endif
typedef enum {
    MAC_ALG_INTRF_EDCA = 0,
    MAC_ALG_INTRF_VO = 1,
    MAC_ALG_INTRF_CCA = 2,
    MAC_ALG_INTRF_11B = 3,
    MAC_ALG_INTRF_11N = 4,
    MAC_ALG_INTRF_LONG_RANGE = 5,
    MAC_ALG_INTRF_NO_11B = 6,

    MAC_ALG_INTRF_BUTT,
} mac_alg_intrf_mode_enum;
typedef osal_u8 mac_alg_intrf_mode_enum_uint8;
/* RTS mode */
typedef enum {
    ALG_RTS_MODE_ALL_ENABLE    = 0, /* rate[0..3]都开RTS */
    ALG_RTS_MODE_ALL_DISABLE   = 1, /* rate[0..3]都不开RTS */
    ALG_RTS_MODE_RATE0_DYNAMIC = 2, /* rate[0]动态RTS, rate[1..3]都开RTS */
    ALG_RTS_MODE_RATE0_DISABLE = 3, /* rate[0]不开RTS, rate[1..3]都开RTS */
    ALG_RTS_MODE_THRESHOLD     = 4, /* 根据MIB配置的RTS Threshold字节数门限确定是否开RTS */

    ALG_RTS_MODE_BUTT
} alg_rts_mode_enum;
typedef osal_u8 alg_rts_mode_enum_uint8;

/*****************************************************************************
  结构体定义
*****************************************************************************/

typedef void (*wifi_csi_data_cb)(unsigned char *csi_data, int len);
td_void wal_csi_register_data_report_cb(wifi_csi_data_cb data_cb);
td_s32 wal_csi_data_report(hmac_vap_stru *hmac_vap, frw_msg *msg);

td_s32 wal_psd_data_report(hmac_vap_stru *hmac_vap, frw_msg *msg);
/* 定制化 linkloss门限配置参数 */
typedef struct {
    td_u8                   linkloss_threshold_wlan_near;
    td_u8                   linkloss_threshold_wlan_far;
    td_u8                   linkloss_threshold_p2p;
    td_u8                   auc_resv[1];
}mac_cfg_linkloss_threshold;

/* 定制化 power ref 2g 5g配置参数 */
typedef struct {
    td_u32                  power_ref_2g;
}mac_cfg_power_ref;

/* customize rf cfg struct */
typedef struct {
    td_s8                    rf_gain_db_2g_mult4;             /* 外部PA/LNA bypass时的增益(0.25dB) */
    td_s8                    rf_gain_db_2g_mult10;            /* 外部PA/LNA bypass时的增益(0.1dB) */
}mac_cfg_gain_db_2g_band;

#ifdef _PRE_WLAN_FEATURE_MESH
typedef struct _mac_cfg_set_mesh_user_param_stru {
    td_u8 addr[WLAN_MAC_ADDR_LEN];
    mac_mesh_plink_state_enum_uint8 plink_sta;
    td_u8 set;
    td_u8 bcn_prio;
    td_u8 is_mbr;
    td_u8 mesh_initiative_peering;
}mac_cfg_set_mesh_user_param_stru;

typedef struct _mac_cfg_set_multi_mac_addr_stru {
    td_u8 addr[WLAN_MAC_ADDR_LEN];
    td_u8 set;   /* 0 - 删除某多播地址、1 - 设置某多播地址 */
    td_u8 rsv;
} mac_cfg_set_multi_mac_addr_stru;

typedef mac_cfg_set_multi_mac_addr_stru mac_cfg_unset_multi_mac_addr_stru;

typedef struct _mac_cfg_auto_peer_params_stru {
    td_s8 rssi_low;                    /* 用户数少的时候用的rssi门限 */
    td_s8 rssi_middle;                 /* 用户数适中的时候用的rssi门限 */
    td_s8 rssi_high;                   /* 用户数多的时候用的rssi门限 */
}mac_cfg_auto_peer_params_stru;

typedef struct _mac_cfg_mesh_nodeinfo_stru {
    mac_ext_mesh_node_type_enum_uint8 node_type;   /* 本节点角色 */
    td_u8 mesh_accept_sta;                          /* 是否接受sta关联 */
    td_u8 user_num;                                 /* 关联用户数 */
    td_u8 privacy;                                  /* 是否加密 */
    td_u8 chan;                                     /* 信道号 */
    td_u8 priority;                                 /* bcn优先级 */
    td_u8 rsv[2];                                   /* 2 byte保留 */
}mac_cfg_mesh_nodeinfo_stru;
#endif

typedef mac_cfg_add_user_param_stru mac_cfg_del_user_param_stru;

/* 定制化 时钟配置参数 */
typedef struct {
    td_u32                  rtc_clk_freq;
    td_u8                   clk_type;
    td_u8                   auc_resv[3]; /* 3 byte保留字段，保证word对齐 */
}mac_cfg_pm_param;

typedef struct {
    mac_cfg_psm_offset_type_uint8 type;
    td_u8  resv;
    td_u16 value;
} mac_cfg_psm_offset;

typedef struct {
    mac_cfg_retry_type_enum_uint8       type;
    td_u8                               limit;
    td_u8                               rsv[2]; /* 2 byte保留字段，保证word对齐 */
}mac_cfg_retry_param_stru;

typedef struct {
    td_u8                               mac_da[WLAN_MAC_ADDR_LEN];
    td_u8                               category;
    td_u8                               auc_resv[1];
}mac_cfg_send_action_param_stru;

typedef struct {
    td_s32   l_is_psm;                           /* 是否进入节能 */
    td_s32   l_is_qos;                           /* 是否发qosnull */
    td_s32   l_tidno;                            /* tid号 */
}mac_cfg_tx_nulldata_stru;

typedef struct {
    td_u8    user_idx;
    td_u8    resv;
    td_u16   rx_pn;
}mac_cfg_set_rx_pn_stru;

typedef struct {
    /* software_retry值 */
    td_u8   software_retry;
    /* 是否取test设置的值，为0则为正常流程所设 */
    td_u8   retry_test;
    td_u8   resv[2]; /* 2 byte保留字段，保证word对齐 */
}mac_cfg_set_soft_retry_stru;

/* =================== 以下为解析内核配置参数转化为驱动内部参数下发的结构体 ======================== */
typedef struct {
    td_u8  band_num;
    td_u8  resv[3];  /* 3 byte保留字段 */
    td_s32 offset;
} mac_cfg_cal_bpower;

typedef struct {
    td_u8  protol;
    td_u8  rate;
    td_u8  resv[2];  /* 2 byte保留字段 */
    td_s32 val;
} mac_cfg_cal_rpower;

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
typedef struct {
    osal_u16   pattern_option;
    osal_u16   pattern_index;
    osal_u32   pattern_len;
    osal_u8    pattern_value[WOW_NETPATTERN_MAX_LEN];
} mac_cfg_wow_pattern_param_stru;
#endif

/*****************************************************************************
  函数申明
*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_CENTRALIZE
osal_void device_main_init(osal_void);
#endif
td_s32 uapi_set_gpio_val(td_s8 *param);
td_u8 uapi_get_custom_cali_done_etc(osal_void);
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
#endif /* __MAC_CFG_H__ */
