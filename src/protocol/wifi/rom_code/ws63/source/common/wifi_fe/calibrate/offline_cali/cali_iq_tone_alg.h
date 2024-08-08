/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Header file for cali_iq_tone_alg.c.
 */

#ifndef __CALI_IQ_TONE_ALG_H__
#define __CALI_IQ_TONE_ALG_H__
#include "fe_hal_phy_if.h"
#include "cali_data_type.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_CALI_IQ_TONE_ALG
/* 宏定义 */
#define CALI_IQ_LS_PARAM_NUM CALI_IQ_LS_COMP_NUM     // IQ校准补偿参数LS滤波器系数的个数
#define ALG_RF_CALI_BAND_NUM 3            // 校准频点数 根据实际算法要求设定
#define ALG_RF_CALI_IQ_TONE_NUM 5         // 用来IQ校准的单音个数 2.5, 7.5, 12.5, 17.5, 22.5M
#define CALI_IQ_BETA_PARAM_NUM (ALG_RF_CALI_IQ_TONE_NUM * 2)    // IQ校准的频域系数beta的个数 单音个数两倍
#define ALG_RF_CALI_TRXIQ_FIX_PTS_LS 12   // LS 系数定点化位数
#define ALG_RF_CALI_TRXIQ_FIX_PTS_TX_BETA 11  // TX IQ BETA 系数定点化位数 比寄存器位数少1bit(有1bit是整数部分)
#define ALG_RF_CALI_TRXIQ_FIX_PTS_RX_BETA 10  // RX IQ BETA 系数定点化位数 比寄存器位数少1bit(有1bit是整数部分)
#define ALG_RF_CALI_IQ_ITER_TIMES   2   // IQ校准单音方案的迭代次数
/* 枚举定义 */
// 校准相位
typedef enum {
    ALG_RF_CALI_IQ_PHASE_1,
    ALG_RF_CALI_IQ_PHASE_2,

    ALG_RF_CALI_IQ_PHASE_BUTT
} alg_rf_cali_iq_phase_enum;
typedef osal_u8 alg_rf_cali_iq_phase_enum_uint8;

/* 结构体定义 */
// ls 滤波器系数
typedef struct {
    cali_complex_stru ls_fir[CALI_IQ_LS_PARAM_NUM];
} alg_rf_cali_iq_ls_fir_param_stru;
// IQ校准单音方案结构
typedef struct {
    cali_complex_stru pos_tone_pos_fft;     // 正单音 正单音频点 能量值
    cali_complex_stru pos_tone_neg_fft;     // 正单音 负单音频点 能量值
    cali_complex_stru neg_tone_pos_fft;     // 负单音 正单音频点 能量值
    cali_complex_stru neg_tone_neg_fft;     // 负单音 负单音频点 能量值
} alg_rf_cali_iq_tone_fft_stru;
typedef struct {
    osal_u32 tone_freq_num;  // 单音个数 与后续数组大小一致
    cali_single_tone_stru tone_cfg[ALG_RF_CALI_IQ_TONE_NUM];    // 单音配置 正负单音
    alg_rf_cali_iq_tone_fft_stru tone_fft[ALG_RF_CALI_IQ_PHASE_BUTT][ALG_RF_CALI_IQ_TONE_NUM];    // 单音能量 两个相位 正负单音 正负频点
} alg_rf_cali_iq_tone_stru;
// beta 系数
typedef struct {
    cali_complex_stru beta_tx[CALI_IQ_BETA_PARAM_NUM];     // tx beta系数
    cali_complex_stru beta_rx[CALI_IQ_BETA_PARAM_NUM];     // rx beta系数
} alg_rf_cali_iq_beta_param_stru;
// 单音能量获取时相位偏差90° 相位数据结构 fpga 通过cap和lctune的调整实现；asic下有移相器 可直接配置寄存器
typedef struct {
    osal_u8 default_phase_cfg;  // 默认配置 TX 对应ppa cap RX 对应 lctune
    osal_u8 phase1_cfg;  // 选择的相位1的配置
    osal_u8 phase2_cfg;  // 选择的相位1的配置
    osal_u8 phase_cfg_max;  // 最大配置 TX 对应ppa cap RX 对应 lctune
} alg_rf_cali_iq_phase_stru;
// iq校准总结构
typedef struct {
    alg_rf_cali_iq_tone_stru tone_param;  // 单音参数
    alg_rf_cali_iq_phase_stru phase_param;  // 相位配置
    alg_rf_cali_iq_type_enum_uint8 iq_cali_type; // iq校准类型
    alg_rf_cali_iq_beta_param_stru beta;    // 校准beta
    alg_rf_cali_iq_ls_fir_param_stru ls_fir[CALI_RF_RX_IQ_BANDWIDTH_NUM];    // 校准结果ls系数
    osal_u32 cali_result;    // 校准返回值
} alg_rf_cali_iq_param_stru;

osal_void cali_trx_iq_alg(hal_device_stru *device, osal_u8 chn_idx,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth, osal_u8 lvl_idx, alg_rf_cali_iq_type_enum_uint8 cali_type);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of __CALI_IQ_TONE_ALG_H__ */
