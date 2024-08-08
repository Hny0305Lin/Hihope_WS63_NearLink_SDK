/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_csi.c.
 */

#ifndef __HAL_CSI_H__
#define __HAL_CSI_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "osal_types.h"
#include "wlan_types_common.h"
#include "hal_device.h"
#include "hal_common_ops.h"
#include "mac_vap_ext.h"
#include "hh503_phy_reg.h"

#include "hal_csi_rom.h"
#include "frw_dmac_rom.h"
#include "hal_main.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  宏定义
*****************************************************************************/
#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_CSI_H

#define HAL_CSI_MAX_USER_NUM  4
#define HAL_CSI_USR_0 0
#define HAL_CSI_USR_1 1
#define HAL_CSI_USR_2 2
#define HAL_CSI_USR_3 3

#define HAL_CSI_USR_ADDR_NUM 2

#define CSI_DATA_MAX_LEN 762    /* CSI内存块单个大小，默认值 762B */
#define CSI_DATA_BLK_NUM 2      /* CSI内存块个数，默认值 2 */

/* PPDU Format类型枚举值定义 */
typedef enum {
    HAL_CSI_PPDU_NONE_HT      = 0,  /* NONE HT */
    HAL_CSI_PPDU_HE_ER_SU     = 1,  /* HE(ER)SU */
    HAL_CSI_PPDU_HE_MU_MIMO   = 2,  /* HE_MU_MIMO */
    HAL_CSI_PPDU_HE_MU_OFDMA  = 3,  /* HE_MU_OFDMA */
    HAL_CSI_PPDU_HT           = 4,  /* HT 11n */
    HAL_CSI_PPDU_VHT          = 5,  /* VHT 11ac */
    HAL_CSI_PPDU_FORMAT_BUTT
} hal_csi_ppdu_format_enum;

/* CSI上报计算类型枚举值定义 */
// note that hal_csi_rpt_cnt_enum is as same as bits.cfg_csi_rpt_cnt_clr[0~4]
typedef enum {
    HAL_CSI_CNT_H_NUM_ERR     = 0,  /* PHY送来H子载波错误计数 */
    HAL_CSI_CNT_H_TIMMING_ERR = 1,  /* H上报时序异常计数 */
    HAL_CSI_CNT_TIMEOUT_ERR   = 2,  /* CSI上报超时异常计数 */
    HAL_CSI_CNT_SW_BLK_ERR    = 3,  /* CSI软件处理太慢异常计数 */
    HAL_CSI_CNT_RPT_SUCC      = 4,  /* CSI上报成功计数 */
    HAL_CSI_RPT_CNT_BUTT
} hal_csi_rpt_cnt_enum;

/*****************************************************************************
  STRUCT定义
*****************************************************************************/
/* CSI白名单用户配置 */
typedef struct {
    /* Byte0~5: user TA address */
    osal_u8 usr_addr[WLAN_MAC_ADDR_LEN];

    /* Byte6: report configurations */
    osal_u8 cfg_csi_en : 1;
    osal_u8 cfg_addr_match_bypass : 1;
    osal_u8 cfg_match_ta_ra_sel : 1;
    osal_u8 cfg_mgmt_frame_en : 1;
    osal_u8 cfg_ctrl_frame_en : 1;
    osal_u8 cfg_data_frame_en : 1;
    osal_u8 cfg_rsv : 1;
    osal_u8 frm_subtype_match_en : 1;

    /* Byte7: report frame subtype */
    osal_u8 match_frame_subtype;

    /* Byte8: report ppdu format */
    osal_u8 ppdu_non_ht_en : 1;
    osal_u8 ppdu_he_er_su_en : 1;
    osal_u8 ppdu_he_mu_mimo_en : 1;
    osal_u8 ppdu_he_mu_ofdma_en : 1;
    osal_u8 ppdu_ht_en : 1;
    osal_u8 ppdu_vht_en : 1;
    osal_u8 ppdu_rsv_en : 2;

    /* Byte9: resv */
    osal_u8 cfg_rsv2;

    /* Byte10~11: report period */
    osal_u16 sample_period_ms;
} hal_csi_usr_attr;

/* CSI buffer配置 */
typedef struct {
    osal_u8 csi_data_blk_num;
    osal_u16 csi_data_max_len;
    osal_u8 resv[1];
} hal_csi_buffer_stru;

/* CSI全局配置 */
typedef struct {
    osal_u32 cfg_rsv31 : 1;
    osal_u32 non_ht_bw_mismatch_center_freq_bypass_en : 1; /* 参考对应MAC寄存器设置该Bit位, 可写功能对外不开放 */
    osal_u32 non_ht_bw_mismatch_center_freq : 1;           /* 参考对应MAC寄存器设置该Bit位, 可写功能对外不开放 */
    osal_u32 he_grouping_en : 1;
    osal_u32 direct_resp_ctrl_flt_en : 1; /* Rx单播直接的响应控制帧时过滤CSI使能 */
    osal_u32 fcs_err_drop_en : 1;
    osal_u32 buffer_step_block_size : 10; /* RO: this field is ReadOnly, 可写功能对外不开放 */
    osal_u32 buffer_block_total : 3;      /* RO: this field is ReadOnly, 可写功能对外不开放 */
    osal_u32 timeout_thr_us_cnt : 5; /* 超时从rx_psdu_complete时刻开始计时 */
    osal_u32 cfg_rsv7_5 : 3; /* 保留调试读取csi_sw_buf_index时使用, 表示当前软件处理CSI的Buffer编号 */
    osal_u32 cfg_rsv4_0 : 5; /* 保留调试读取CSI计数清零使能值时使用 */
} hal_csi_global_cfg;

/* CSI维测计数 */
typedef struct {
    osal_u8 h_num_err_cnt;
    osal_u8 h_timing_err_cnt;
    osal_u8 timeout_err_cnt;
    osal_u8 sw_blk_err_cnt;

    osal_u32 rpt_cnt_rsv31_19 : 13;
    osal_u32 rpt_csi_blk_index : 3; /* 保留调试读取逻辑当前处理CSI的Buffer编号时使用 */
    osal_u32 rpt_csi_succ_cnt : 16;
} hal_csi_stat;

/*****************************************************************************
  函数声明
*****************************************************************************/
// csi使能禁用接口
osal_u32 hal_csi_enable(osal_u8 usr_id);
osal_u32 hal_csi_disable(osal_u8 usr_id);
osal_bool hal_csi_vap_is_close(osal_void);
osal_void hh503_csi_phy_open_channel(osal_void);
// csi用户配置接口
osal_u32 hal_csi_set_usr_attr(osal_u8 usr_id, const hal_csi_usr_attr *attr);
osal_u32 hal_csi_get_usr_attr(osal_u8 usr_id, hal_csi_usr_attr *attr);

// csi全局配置接口

osal_u32 hal_csi_set_buffer_config(hal_csi_buffer_stru *csi_buffer);
osal_u32 hal_csi_set_tsf(osal_u8 hal_vap_id);
osal_void hal_csi_set_ack_resp_flt(void);

// csi其他配置接口：统计计数
osal_u32 hal_csi_stat_clr(osal_void);


osal_u32 hal_csi_keep_mac_pa_open(void);
/*****************************************************************************
 ROM回调函数类型定义
*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_csi.h */
