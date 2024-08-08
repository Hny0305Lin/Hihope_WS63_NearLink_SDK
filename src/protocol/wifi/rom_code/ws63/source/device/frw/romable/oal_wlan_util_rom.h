/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: oal util interface
 */

#ifndef OAL_WLAN_UTIL_ROM_H
#define OAL_WLAN_UTIL_ROM_H

#include "frw_util_rom.h"
#include "wlan_util_common_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 字节操作 */
static INLINE__ osal_u32 oal_rotl_w(osal_u32 val, osal_u32 bits, osal_u32 width)
{
    return (val << bits) | (val >> (width - bits));
}
static INLINE__ osal_u32 oal_rotl32(osal_u32 val, osal_u32 bits)
{
    return oal_rotl_w(val, bits, 32); /* 32 bits word */
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_wlan_util_rom.h */