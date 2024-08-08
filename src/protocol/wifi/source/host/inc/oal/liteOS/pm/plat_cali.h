/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2015-2023. All rights reserved.
 * Description: plat_cali.c头文件
 * Create: 2015年7月16日
 */

#ifndef __PLAT_CALI_H__
#define __PLAT_CALI_H__

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "oal_types.h"
#include "oal_util.h"

#if defined(_PRE_OS_VERSION) && defined(_PRE_OS_VERSION_LITEOS) && (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "oal_plat_type.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define OAL_2G_CHANNEL_NUM         (3)
#define OAL_5G_20M_CHANNEL_NUM     (7)
#define OAL_5G_80M_CHANNEL_NUM     (7)
#define OAL_5G_160M_CHANNEL_NUM    (2)
#define OAL_5G_CHANNEL_NUM         (OAL_5G_20M_CHANNEL_NUM + OAL_5G_80M_CHANNEL_NUM + OAL_5G_160M_CHANNEL_NUM)
#define OAL_5G_DEVICE_CHANNEL_NUM  (7)
#define OAL_CALI_HCC_BUF_NUM       (3)
#define OAL_CALI_HCC_BUF_SIZE      (1500)
#define OAL_CALI_IQ_TONE_NUM       (16)
#define OAL_CALI_TXDC_GAIN_LVL_NUM (16)           /* tx dc补偿值档位数目 */
#define OAL_BT_RF_FEQ_NUM          (79)           /* total Rf frequency number */
#define OAL_BT_CHANNEL_NUM         (8)            /* total Rf frequency number */
#define OAL_BT_POWER_CALI_CHANNEL_NUM         (3)
#define OAL_BT_NVRAM_DATA_LENGTH   (104)
#define OAL_BT_NVRAM_NAME           "BTCALNV"
#define OAL_BT_NVRAM_NUMBER        (352)


#define WIFI_2_4G_ONLY              (0x2424)
#define SYS_EXCEP_REBOOT            (0xC7C7)
#define OAL_CALI_PARAM_ADDITION_LEN (8)
#define OAL_5G_IQ_CALI_TONE_NUM         (8)

#define CHECK_5G_ENABLE             "radio_cap_0"

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 全局变量定义
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
    osal_u16  analog_rxdc_cmp;
    osal_u16  digital_rxdc_cmp_i;
    osal_u16  digital_rxdc_cmp_q;
    osal_u8   reserve[2];
} oal_rx_dc_comp_val_stru;

typedef struct {
    osal_u8   upc_ppa_cmp;
    osal_s8    ac_atx_pwr_cmp;
    osal_u8   dtx_pwr_cmp;
    osal_u8   reserve[1];
} oal_2g_tx_power_comp_val_stru;

typedef struct {
    osal_u8   upc_ppa_cmp;
    osal_u8   upc_mx_cmp;
    osal_s8    ac_atx_pwr_cmp;
    osal_u8   dtx_pwr_cmp;
} oal_5g_tx_power_comp_val_stru;

typedef struct {
    osal_u16  txdc_cmp_i;
    osal_u16  txdc_cmp_q;
} oal_txdc_comp_val_stru;

typedef struct {
    osal_u8   ppf_val;
    osal_u8   reserve[3];
} oal_ppf_comp_val_stru;

typedef struct {
    osal_u16   txiq_cmp_p;
    osal_u16   txiq_cmp_e;
} oal_txiq_comp_val_stru;

typedef struct {
    osal_u16  cali_time;
    osal_u16  temperature     : 3,
                uc_5g_chan_idx1     : 5,
                uc_5g_chan_idx2     : 5,
                update_bt        : 3;
} oal_update_cali_channel_stru;

typedef struct {
    osal_u32 wifi_2_4g_only;
    osal_u32 excep_reboot;
    osal_u32 reserve[OAL_CALI_PARAM_ADDITION_LEN];
} oal_cali_param_addition_stru;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
typedef osal_u32 (*custom_cali_func)(osal_void);
void set_custom_cali_func(custom_cali_func func);
custom_cali_func get_custom_cali_func(void);
void set_custom_exit_later_func(custom_cali_func func);
custom_cali_func get_custom_exit_later_func(void);
#endif

/*****************************************************************************
  10 add for bfgx
*****************************************************************************/
/* enum定义不能超过BFGX_BT_CUST_INI_SIZE/4 (128) */
typedef enum {
    BFGX_CFG_INI_BT_MAXPOWER = 0,
    BFGX_CFG_INI_BT_EDRPOW_OFFSET,
    BFGX_CFG_INI_BT_BLEPOW_OFFSET,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_REF_NUM,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_REF_BAND1,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_REF_BAND2,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_REF_BAND3,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_REF_BAND4,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_REF_BAND5,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_REF_BAND6,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_REF_BAND7,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_REF_BAND8,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_FRE1,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_FRE2,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_FRE3,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_FRE4,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_FRE5,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_FRE6,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_FRE7,
    BFGX_CFG_INI_BT_CALI_TXPWR_PA_FRE8,
    BFGX_CFG_INI_BT_CALI_BT_TONE_AMP_GRADE,
    BFGX_CFG_INI_BT_RXDC_BAND,
    BFGX_CFG_INI_BT_DBB_SCALING_SATURATION,
    BFGX_CFG_INI_BT_PRODUCTLINE_UPCCODE_SEARCH_MAX,
    BFGX_CFG_INI_BT_PRODUCTLINE_UPCCODE_SEARCH_MIN,
    BFGX_CFG_INI_BT_DYNAMICSARCTRL_BT,
    BFGX_CFG_INI_BT_POWOFFSBT,
    BFGX_CFG_INI_BT_ELNA_2G_BT,
    BFGX_CFG_INI_BT_RXISOBTELNABYP,
    BFGX_CFG_INI_BT_RXGAINBTELNA,
    BFGX_CFG_INI_BT_RXBTEXTLOSS,
    BFGX_CFG_INI_BT_ELNA_ON2OFF_TIME_NS,
    BFGX_CFG_INI_BT_ELNA_OFF2ON_TIME_NS,
    BFGX_CFG_INI_BT_HIPOWER_MODE,
    BFGX_CFG_INI_BT_FEM_CONTROL,
    BFGX_CFG_INI_BT_FEATURE_32K_CLOCK,
    BFGX_CFG_INI_BT_FEATURE_LOG,
    BFGX_CFG_INI_BT_CALI_SWTICH_ALL,
    BFGX_CFG_INI_BT_ANT_NUM_BT,
    BFGX_CFG_INI_BT_POWER_LEVEL_CONTROL,
    BFGX_CFG_INI_BT_COUNTRY_CODE,
    BFGX_CFG_INI_BT_RESERVED1,
    BFGX_CFG_INI_BT_RESERVED2,
    BFGX_CFG_INI_BT_RESERVED3,
    BFGX_CFG_INI_BT_RESERVED4,
    BFGX_CFG_INI_BT_RESERVED5,
    BFGX_CFG_INI_BT_RESERVED6,
    BFGX_CFG_INI_BT_RESERVED7,
    BFGX_CFG_INI_BT_RESERVED8,
    BFGX_CFG_INI_BT_RESERVED9,
    BFGX_CFG_INI_BT_RESERVED10,

    BFGX_CFG_INI_BUTT
} bfgx_cfg_ini;

typedef struct {
    char*   name;
    osal_s32   init_value;
} bfgx_ini_cmd;

/* 以下5个宏定义，如果要修改长度，需要同步修改device的宏定义 */
#define BFGX_BT_CALI_DATA_SIZE               (492)
#define WIFI_CALI_DATA_FOR_FM_RC_CODE_SIZE   (20)
#define BFGX_NV_DATA_SIZE                    (128)
#define BFGX_BT_CUST_INI_SIZE                (512)
#define WIFI_CALI_DATA_FOR_BT                (896)
/* 考虑结构体总体长度考虑SDIO下载长度512对齐特性，这里长度为2048 */
typedef struct {
    osal_u8 bfgx_data[BFGX_BT_CALI_DATA_SIZE];
    osal_u8 wifi_rc_code_data[WIFI_CALI_DATA_FOR_FM_RC_CODE_SIZE];
    osal_u8 nv_data[BFGX_NV_DATA_SIZE];
    osal_u8 bt_cust_ini_data[BFGX_BT_CUST_INI_SIZE];
    osal_u8 wifi_cali_for_bt_data[WIFI_CALI_DATA_FOR_BT];
} bfgx_cali_data_stru;

#define BFGX_CALI_DATA_BUF_LEN  (sizeof(bfgx_cali_data_stru))

extern osal_s32 bt_customize_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of plat_cali.h */

