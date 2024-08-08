/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Header file for wal_ccpriv.c.
 */

#ifndef __WAL_CCPRIV_H__
#define __WAL_CCPRIV_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_ext_if.h"
#include "hmac_ext_if.h"
#include "wal_main.h"
#include "mac_device_ext.h"
#include "wal_ccpriv_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define WAL_CCPRIV_CMD_MAX_LEN       1024

#define WAL_CCPRIV_11B_MCS_MIN          0
#define WAL_CCPRIV_11B_MCS_MAX          3
#define WAL_CCPRIV_11G_MCS_MIN          8
#define WAL_CCPRIV_11G_MCS_MAX          15
#define WAL_CCPRIV_11N_MCS_MIN          0
#define WAL_CCPRIV_11N_MCS_MAX          7
#define WAL_CCPRIV_11AX_MCS_MIN         0
#define WAL_CCPRIV_11AX_MCS_MAX         11

#define WAL_CCPRIV_NSS_MIN              1
#define WAL_CCPRIV_NSS_MAX              4
#define WAL_CCPRIV_CH_NUM               4
#define WAL_CCPRIV_SNPRINTF_DST         10

#define WAL_CCPRIV_BOOL_NIM             0
#define WAL_CCPRIV_BOOL_MAX             1
#define WAL_CCPRIV_FREQ_SKEW_ARG_NUM    8

#define WAL_CCPRIV_MS_TO_S                   1000   /* ms和s之间倍数差 */
#define WAL_CCPRIV_KEEPALIVE_INTERVAL_MIN    5000   /* 受默认老化计数器出发时间所限制 */
#define WAL_CCPRIV_KEEPALIVE_INTERVAL_MAX    0xffff /* timer间隔时间限制所致(oal_uin16) */
#define WAL_AL_TX_RATIO_LOW 1
#define WAL_AL_TX_RATIO_HIGH 10
#define WAL_SET_MONITER_ON 1
#define WAL_MONITER_SWITCH_MODE 2
#define WAL_EXTERNAL_RECORD_ENABLE_ON 1

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

/* ccpriv 参数解析结构体定义 */
typedef struct {
    td_char  cmd_param1[WAL_CCPRIV_CMD_NAME_MAX_LEN];    /* 命令第一个参数字符串 */
    td_char  cmd_param2[WAL_CCPRIV_CMD_NAME_MAX_LEN];    /* 命令第二个参数字符串 */
}wal_ccpriv_two_param_stru;

typedef struct {
    td_char  cmd_param1[WAL_CCPRIV_CMD_NAME_MAX_LEN];    /* 命令第一个参数字符串 */
    td_char  cmd_param2[WAL_CCPRIV_CMD_NAME_MAX_LEN];    /* 命令第二个参数字符串 */
    td_char  cmd_param3[WAL_CCPRIV_CMD_NAME_MAX_LEN];    /* 命令第三个参数字符串 */
}wal_ccpriv_three_param_stru;

typedef struct {
    td_char  cmd_param1[WAL_CCPRIV_CMD_NAME_MAX_LEN];    /* 命令第一个参数字符串 */
    td_char  cmd_param2[WAL_CCPRIV_CMD_NAME_MAX_LEN];    /* 命令第二个参数字符串 */
    td_char  cmd_param3[WAL_CCPRIV_CMD_NAME_MAX_LEN];    /* 命令第三个参数字符串 */
    td_char  cmd_param4[WAL_CCPRIV_CMD_NAME_MAX_LEN];    /* 命令第三个参数字符串 */
}wal_ccpriv_four_param_stru;

typedef struct {
    osal_u32 back_off_time;
} al_tx_back_off_time_stru;
/*****************************************************************************
  8 UNION定义
*****************************************************************************/
/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  10 函数声明
*****************************************************************************/
osal_u8 wal_tx_data_type_debug_on(osal_void);
td_u32 wal_ccpriv_set_mcs(oal_net_device_stru *netdev, td_char *pc_param);
td_u32 wal_ccpriv_del_vap(oal_net_device_stru *netdev, td_char *pc_param);
td_u32 uapi_ccpriv_set_sleep(oal_net_device_stru *net_dev, osal_s8 *pc_param);
td_u32 uapi_ccpriv_vap_info(oal_net_device_stru *netdev, td_s8 *pc_param);
td_u32 wal_ccpriv_remove_proc(td_void);
td_u32 wal_ccpriv_get_bw(oal_net_device_stru *netdev, hal_channel_assemble_enum_uint8 *pen_bw_index);
td_u32 wal_ccpriv_set_bw(oal_net_device_stru *netdev, td_char *pc_param);
td_u32 uapi_ccpriv_getcountry(oal_net_device_stru *netdev, td_char *pc_param);
td_u32 wal_ccpriv_sta_set_psm_offset(oal_net_device_stru *netdev, td_char *param);
td_u32 wal_ccpriv_sta_set_offload_param(oal_net_device_stru *netdev, td_char *param);
td_u32 wal_ccpriv_sta_set_hw_ps_mode(oal_net_device_stru *netdev, td_char *param);
td_u32 wal_ccpriv_set_pm_switch(oal_net_device_stru *netdev, td_char *pc_param);
td_u32 uapi_ccpriv_setcountry(oal_net_device_stru *netdev, td_char *param);
td_u32 uapi_ccpriv_ampdu_tx_on(oal_net_device_stru *netdev, td_s8 *pc_param);
td_u32 wal_ccpriv_amsdu_tx_on(oal_net_device_stru *netdev, td_char *pc_param);
td_u32 wal_ccpriv_tx_proc(oal_net_device_stru *netdev, td_char *pc_param);
td_u32 wal_ccpriv_rx_proc(oal_net_device_stru *netdev, td_char *pc_param);
td_u32 uapi_ccpriv_rx_fcs_info(oal_net_device_stru *netdev, td_char *pc_param);
osal_u32 uapi_ccpriv_user_info(oal_net_device_stru *netdev, td_s8 *pc_param);
td_u32 wal_ccpriv_bsrp_dbg(oal_net_device_stru *netdev, td_char *param);
td_bool is_under_ps(td_void);
td_void set_under_ps(td_bool under_ps);
td_u32  uapi_ccpriv_send_pkt(oal_net_device_stru *net_dev,       osal_s8 *param);
osal_void wal_ccpriv_str_to_hex(osal_s8 *param, td_s16 len, osal_u8 *value);
td_u32 wal_send_custom_pkt(oal_net_device_stru *net_dev, osal_u8 *data, osal_u32 len);
#ifdef _PRE_WLAN_DFT_STAT
osal_void wal_record_wifi_external_log(const osal_u32 addr);
#endif
oal_bool_enum_uint8 check_unsigned_range(osal_s8 *value, osal_u32 min, osal_u32 max);

#ifdef _PRE_WLAN_CFGID_DEBUG
osal_u32 wal_ccpriv_parse_cmd_debug(const td_char *cmd_name, td_u8 *ccpriv_cmd_id, wal_ccpriv_cmd_func *func);
#endif

#ifdef _PRE_WLAN_RF_CALI_DEBUG
// 单音测试参数
#define SINGLE_TONE_TEST_8BITS           8
#define SINGLE_TONE_TEST_16BITS          16
#define SINGLE_TONE_TEST_24BITS          24
#define SINGLE_TONE_TEST_ACCUM_TYPE      4
#define SINGLE_TONE_TEST_FLAG_BUTT       4
#define SINGLE_TONE_TEST_TYPE_BUTT       6
#define SINGLE_TONE_TEST_CHAN_BUTT       3
#define SINGLE_TONE_TEST_TIMES_BUTT      7
#define SINGLE_TONE_TEST_PARAM3_BUTT     15
#define SINGLE_TONE_TEST_PARAM4_BUTT     0xc8
#endif

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of wal_ccpriv.h */

