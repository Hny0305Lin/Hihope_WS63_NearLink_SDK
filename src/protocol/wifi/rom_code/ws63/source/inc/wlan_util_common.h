/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: oam log printing interface
 */

#ifndef __WLAN_UTIL_COMMON_H__
#define __WLAN_UTIL_COMMON_H__

#include "wlan_util_common_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
 函 数 名  : oal_bit_get_num_one_byte
 功能描述  : 获取单字节中的bit1的个数
**************************************************************************** */
static inline osal_u8 oal_bit_get_num_one_byte(osal_u8 byte)
{
    osal_u8 byte_data = byte;

    byte_data = (byte_data & 0x55) + ((byte_data >> 1) & 0x55); /* 0x55掩码, 1移位数 */
    byte_data = (byte_data & 0x33) + ((byte_data >> 2) & 0x33); /* 0x33掩码, 2移位数 */
    byte_data = (byte_data & 0x0F) + ((byte_data >> 4) & 0x0F); /* 0x0F掩码, 4移位数 */

    return byte_data;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wlan_util_common.h */
