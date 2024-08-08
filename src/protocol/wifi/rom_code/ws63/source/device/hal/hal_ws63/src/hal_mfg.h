/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Header file for hal_mfg.c
 */

#ifndef HAL_MFG_H
#define HAL_MFG_H
#ifdef _PRE_WLAN_SUPPORT_CCPRIV_CMD
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "hal_ext_if.h"
#include "mac_vap_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define XO_TRIM_FINE ((osal_u32)(1 << 6))
#define XO_TRIM_COARSE ((osal_u32)(1 << 3))
#define XO_TRIM_ENABLE 1
#define MFG_MODE_ENABLE 1
#define XO_TRIM_DISABLE 0
#define CMD_MAX_LEN 256
#define WLAN_CFG_CON_OFFSET_NUM 2
#define WLAN_CFG_CMU_XO_TRIM_NUM 2
#define WLAN_CFG_MAC_GROUP_NUM 4
#define WLAN_CFG_POWER_INFO_NUM 7
#define RSSI_OFFSET_SIZE 3
#define WLAN_CFG_TEMPER_GEARS_STEP 10
#define EXT_EFUSE_CURVE_FACTOR_MAX 15
#define EFUSE_GROUP_MAX 3
#define EXT_EFUSE_IDX_GROUP 48
#define EFUSE_NAME_LEN 16
#define EFUSE_MAX_LEN 24
#define EXT_EFUSE_LOCK_MAX 255
#define SIZE_2_BITS 2
#define SIZE_3_BITS 3
#define SIZE_4_BITS 4
#define SIZE_8_BITS 8
#define SIZE_5_BITS 5
#define EXT_EFUSE_LOCK_XO_TRIM_BIT_POS 7
#define EXT_EFUSE_LOCK_POWER_BIT_POS 0
#define EXT_EFUSE_LOCK_RSSI_BIT_POS 1
#define EXT_EFUSE_LOCK_TEMP_BIT_POS 6
#define EXT_EFUSE_MFG_FLAG_BIT_POS 7
#define WLAN_CFG_ONE_POWER_PARAM_NUM 8
#define WLAN_CFG_COARSE_MIN_NUM 0
#define WLAN_CFG_COARSE_MAX_NUM 15
#define WLAN_CFG_FINE_MIN_NUM 0
#define WLAN_CFG_FINE_MAX_NUM 127

typedef enum {
    EXT_EFUSE_XO_TRIM_1_ID = 144,
    EXT_EFUSE_XO_TRIM_COARSE_1_ID = 145,
    EXT_EFUSE_CURV_FACTOR_LOW_1_ID = 146,
    EXT_EFUSE_11B_HIGN_1_ID = 147,
    EXT_EFUSE_11B_LOW_1_ID = 149,
    EXT_EFUSE_OFDM_20M_HIGN_1_ID = 151,
    EXT_EFUSE_OFDM_20M_LOW_1_ID = 153,
    EXT_EFUSE_OFDM_40M_HIGN_1_ID = 153,
    EXT_EFUSE_OFDM_40M_LOW_1_ID = 153,
    EXT_EFUSE_RSSI_BAND1_1_ID = 159,
    EXT_EFUSE_RSSI_BAND3_1_ID = 160,
    EXT_EFUSE_TEMPERATURE_1_ID = 161,
    EXT_EFUSE_XO_TRIM_2_ID = 162,
    EXT_EFUSE_XO_TRIM_COARSE_2_ID = 163,
    EXT_EFUSE_CURV_FACTOR_LOW_2_ID = 164,
    EXT_EFUSE_11B_HIGN_2_ID = 165,
    EXT_EFUSE_11B_LOW_2_ID = 167,
    EXT_EFUSE_OFDM_20M_HIGN_2_ID = 169,
    EXT_EFUSE_OFDM_20M_LOW_2_ID = 171,
    EXT_EFUSE_OFDM_40M_HIGN_2_ID = 173,
    EXT_EFUSE_OFDM_40M_LOW_2_ID = 175,
    EXT_EFUSE_RSSI_BAND1_2_ID = 177,
    EXT_EFUSE_RSSI_BAND3_2_ID = 178,
    EXT_EFUSE_TEMPERATURE_2_ID = 179,
    EXT_EFUSE_XO_TRIM_3_ID = 180,
    EXT_EFUSE_XO_TRIM_COARSE_3_ID = 181,
    EXT_EFUSE_CURV_FACTOR_LOW_3_ID = 182,
    EXT_EFUSE_11B_HIGN_3_ID = 183,
    EXT_EFUSE_11B_LOW_3_ID = 185,
    EXT_EFUSE_OFDM_20M_HIGN_3_ID = 187,
    EXT_EFUSE_OFDM_20M_LOW_3_ID = 189,
    EXT_EFUSE_OFDM_40M_HIGN_3_ID = 191,
    EXT_EFUSE_OFDM_40M_LOW_3_ID = 193,
    EXT_EFUSE_RSSI_BAND1_3_ID = 195,
    EXT_EFUSE_RSSI_BAND3_3_ID = 196,
    EXT_EFUSE_TEMPERATURE_3_ID = 197,
    EXT_EFUSE_IPV4_MAC_ADDR_01_ID = 198,
    EXT_EFUSE_IPV4_MAC_ADDR_02_ID = 204,
    EXT_EFUSE_IPV4_MAC_ADDR_03_ID = 210,
    EXT_EFUSE_IPV4_MAC_ADDR_04_ID = 216,
    EXT_EFUSE_MAX = 255,
} efuse_id;

typedef enum {
    EFUSE_OP_READ,               /* efuse读操作 */
    EFUSE_OP_WRITE,              /* efuse写操作 */
    EFUSE_OP_REMAIN,             /* efuse查询可用组数 */
    EFUSE_OP_READ_ALL_MFG_DATA   /* 读取efuse所有校准数据 */
} efuse_op;

typedef struct {
    osal_char efuse_name[EFUSE_NAME_LEN]; /* efuse功能名 */
    osal_u32 len;                         /* 数据长度 */
    osal_u8 val[CMD_MAX_LEN];             /* 操作数据 */
    osal_u8 op;                           /* 操作类型 */
    osal_u8 resv[3];                      /* 保留3字节，4字节对齐 */
} efuse_operate_stru;

typedef struct {
    osal_u8 *data;
    osal_u8 len;
} efuse_mfg_data_status;

typedef struct {
    osal_u8 curve_factor_hig;
    osal_u8 curve_factor_low;
    osal_u16 band1_hign;
    osal_u16 band1_low;
    osal_u16 ofdm_20m_hign;
    osal_u16 ofdm_20m_low;
    osal_u16 ofdm_40m_hign;
    osal_u16 ofdm_40m_low;
    osal_u8 resv[2];
} efuse_power_offset_stru;

typedef struct {
    osal_u16 xo_trim;                        /* 产测频偏校正码值 */
    osal_u8 resv[2];
    efuse_power_offset_stru pwr_offset;      /* 功率校准数据 */
    osal_u16 rssi_offset;                    /* rssi校准数据 */
    osal_u8 temp;                            /* 产测温度档位 */
    osal_u8 resv2;
} efuse_mfg_data_stru;

typedef struct {
    osal_char efuse_name[EFUSE_NAME_LEN];
    osal_u8 id[EFUSE_GROUP_MAX];
    osal_u8 lock_id[EFUSE_GROUP_MAX];
    osal_u8 bit_pos;
} efuse_id_info_stru;

osal_void hal_rf_set_default_cmu_xo_trim(osal_void);
osal_void hal_set_mfg_mode(osal_u8 mode);
osal_void hal_set_xo_trim_coarse(osal_u32 xo_trim_coarse, osal_u32 *coarse_reg);
osal_void hal_set_xo_trim_fine(osal_u32 xo_trim_fine, osal_u32 *fine_reg);
osal_void hal_get_xo_trim(osal_u32 *xo_trim_coarse, osal_u32 *xo_trim_fine);
osal_void hal_get_temp(osal_void);
osal_s32 hal_config_efuse_operate(efuse_operate_stru *param);
osal_s32 hal_read_efuse_cmu_xo_trim(osal_u8 *coarse_reg, osal_u8 *fine_reg);
osal_s32 hal_efuse_cmu_xo_trim_write(osal_u8 coarse_reg, osal_u8 fine_reg);
osal_s32 hal_set_efuse_rssi_offset(osal_u8 *rssi_param, osal_u32 len);
osal_s32 hal_get_efuse_rssi_offset(osal_void);
osal_s32 hal_efuse_write_temp(osal_u8 gears);
osal_s32 hal_efuse_read_temp(osal_void);
osal_s32 hal_efuse_status(osal_void);
osal_s32 hal_efuse_write_power_info(osal_s32 *power_info, osal_u32 len);
osal_s32 hal_read_efuse_read_power_info(osal_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
#endif
#endif
