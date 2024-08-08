/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
* Description: Header file of main.h.
*/

#ifndef MAIN_H
#define MAIN_H

#include "osal_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    函数声明
*****************************************************************************/
int wifi_init_task(void);
int wifi_host_task(osal_void *param);
osal_u32 uapi_wifi_ccpriv(osal_s32 argc, const osal_char *argv[]);
osal_u32 wal_at_entry(osal_char *buffer, osal_u32 cmd_type);
osal_u32 uapi_wifi_at_start(osal_s32 argc, const osal_char *argv[], osal_u32 cmd_type);
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
osal_u32 uapi_wifi_mfg_ccpriv(osal_s32 argc, const osal_char *argv[], osal_u32 cmd_type);
#endif

#ifdef __cplusplus
#if __cplusplus
        }
#endif
#endif

#endif /* end of main.h */
