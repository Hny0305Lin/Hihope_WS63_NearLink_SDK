/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2023. All rights reserved.
 * Description: NV Storage Library debug
 */

#ifndef NV_DEBUG_H
#define NV_DEBUG_H

#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define NV_LOG_LEVEL_ALERT    0
#define NV_LOG_LEVEL_FATAL    1
#define NV_LOG_LEVEL_ERROR    2
#define NV_LOG_LEVEL_WARN     3
#define NV_LOG_LEVEL_NOTICE   4
#define NV_LOG_LEVEL_INFO     5
#define NV_LOG_LEVEL_DBG      6
#define NV_LOG_LEVEL_TRACE    7

uint32_t nv_get_debug_level(void);

void nv_set_debug_level(uint32_t level);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* NV_DEBUG_H */
