/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: compatibility algorithm rom.
 */

#ifndef ALG_COMPATIBILITY_ROM_H
#define ALG_COMPATIBILITY_ROM_H

/* 1 其他头文件包含 */
#include "alg_main_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_COMPATIBILITY

#define ALG_COMPATIBILITY_LDPC_TX_TH 256 /* ldpc探测最大惩罚门限 */

/* 解决LDPC兼容性问题: AR算法按照协议能力协商, 此处默认设计为不支持; 如果LDPC编码发送成功一次则此处设计为True */
typedef struct {
    oal_bool_enum_uint8 ldpc_detect; /* 该用户是否需要ldpc探测 */
    oal_bool_enum_uint8 ldpc_disable;
    oal_bool_enum_uint8 en_supp_ldpc; /* 该用户是否支持LDPC */
    osal_u8 resv;

    osal_u16 cur_tx_total;  /* 当前累计发帧数 */
    osal_u16 detect_tx_th;  /* 探测切换发帧门限 */
} alg_ldpc_support_stru;

typedef struct {
    alg_ldpc_support_stru ldpc_det;

    osal_u16 lut_index;  /* 对应的USER lut index，避免回调指针 */
    osal_u16 resv;
} alg_compatibility_user_stru;

osal_void alg_compatibility_init(osal_void);
osal_void alg_compatibility_exit(osal_void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
