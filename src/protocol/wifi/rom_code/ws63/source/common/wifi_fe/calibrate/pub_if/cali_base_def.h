/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 校准模块的基础公共信息定义
 */

#ifndef __CALI_BASE_DEF_H__
#define __CALI_BASE_DEF_H__

#include "cali_outer_os_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 校准RF带宽枚举 */
typedef enum {
    CALI_BW_20M,
    CALI_BW_40M,
    CALI_BW_80M,
    CALI_BW_BUTT
} cali_bandwidth_enum;
typedef osal_u8 cali_bandwidth_enum_uint8;
// IQ校准类型
typedef enum {
    ALG_RF_CALI_IQ_TYPE_TX,
    ALG_RF_CALI_IQ_TYPE_RX,

    ALG_RF_CALI_IQ_TYPE_BUTT
} alg_rf_cali_iq_type_enum;
typedef osal_u8 alg_rf_cali_iq_type_enum_uint8;
// 单音正负枚举
typedef enum {
    ALG_RF_CALI_TONE_SIGN_POS,
    ALG_RF_CALI_TONE_SIGN_NEG,

    ALG_RF_CALI_TONE_SIGN_BUTT
} alg_rf_cali_tone_sign_enum;
typedef osal_u8 alg_rf_cali_tone_sign_enum_uint8;
// 校准错误码枚举
typedef enum {
    ALG_RF_CALI_RESULT_SUCC,
    ALG_RF_CALI_RESULT_GET_PHASE_FAILED,
    ALG_RF_CALI_RESULT_GET_PHASE_TOO_SMALL,
    ALG_RF_CALI_RESULT_RF_DEV_NULL,

    ALG_RF_CALI_RESULT_BUTT
} alg_rf_cali_result_enum;
// 测试日志宏
#define CALI_IQ_DEBUG_INFO_SWITCH 0

/* 使能开关定义 */
#define HH503_CALI_CFG_ENABLE 1
#define HH503_CALI_CFG_DISABLE 0
#define HAL_CALI_CFG_ENABLE 1
#define HAL_CALI_CFG_DISABLE 0

/* 校准频段 */
#define CALI_FREQ_MASK_2G 0x01
#define CALI_FREQ_MASK_5G 0x02
typedef struct {
    osal_u32 reg_addr;
    osal_u32 cfg_val;
    osal_u32 recover_val;
} common_path_cfg_reg_stru;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
