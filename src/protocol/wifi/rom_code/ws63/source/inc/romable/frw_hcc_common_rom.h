/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file of device and host frw_hcc_common public interface.
 */

#ifndef __FRW_HCC_COMMON_ROM_H__
#define __FRW_HCC_COMMON_ROM_H__

#include "osal_types.h"
#ifndef _PRE_WLAN_FEATURE_CENTRALIZE
#include "hcc_cfg_comm.h"
#include "hcc_if.h"
#include "hcc_comm.h"
#endif

/*****************************************************************************
  1 宏定义
*****************************************************************************/

/*****************************************************************************
  2 枚举定义
*****************************************************************************/

#ifdef _PRE_WLAN_FEATURE_CENTRALIZE
#define HCC_HEAD_SIZE 4
static INLINE__ osal_u16 hcc_get_head_len(osal_void)
{
    return HCC_HEAD_SIZE;
}
#endif

#endif // __FRW_HCC_COMMON_ROM_H__
