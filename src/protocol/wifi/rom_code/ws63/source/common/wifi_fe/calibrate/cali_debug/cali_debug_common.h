/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: 校准debug公共头文件
 */
#ifndef __CALI_DEBUG_COMMON_H__
#define __CALI_DEBUG_COMMON_H__
#include "osal_types.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#ifdef _PRE_WLAN_RF_CALI_DEBUG
#define FE_CALI_DEBUG_CMD_PARAM_NUM_MAX_LEN 5   // 测试命令的参数个数先固定5个值
// 测试命令的类型 与命令字符cali_debug_cali_type一一对应
enum {
    FE_CALI_DEBUG_CALI_TYPE_TX_DC,
    FE_CALI_DEBUG_CALI_TYPE_RX_DC,
    FE_CALI_DEBUG_CALI_TYPE_TX_PWR,
    FE_CALI_DEBUG_CALI_TYPE_TX_IQ,
    FE_CALI_DEBUG_CALI_TYPE_RX_IQ,
    FE_CALI_DEBUG_CALI_TYPE_RC,
    FE_CALI_DEBUG_CALI_TYPE_BUTT
};
// 校准测试命令消息内容
typedef struct {
    osal_u8 cali_type;  // 校准类型枚举
    osal_u8 cali_func_idx;  // 校准功能枚举
    osal_u8 cali_param_num; // 校准参数个数
    osal_u8 rsv;
    osal_s32 cali_param[FE_CALI_DEBUG_CMD_PARAM_NUM_MAX_LEN];  // 校准参数数值
} fe_cali_debug_cmd_param_stru;
#endif // #ifdef _PRE_WLAN_RF_CALI_DEBUG

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
