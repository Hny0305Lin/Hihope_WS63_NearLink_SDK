/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file of MIB element structure and its corresponding enumeration.
 */

#ifndef WLAN_MIB_ROM_H
#define WLAN_MIB_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_types_device_rom.h"
#include "wlan_types_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
    配置命令 ID
    第一段  MIB 类配置
    第二段  非MIB类配置
*****************************************************************************/
typedef osal_u16 wlan_dmac_cfgid_enum_uint16;

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
typedef struct {
    osal_u8 dot11_station_id[WLAN_MAC_ADDR_LEN];
    osal_u8 dot11_privacy_invoked                     : 1;
    osal_u8 dot11_rsna_activated                      : 1;
    osal_u8 dot11_mgmt_option_multi_bssid_implemented : 1;
    osal_u8 dot11_heldpc_coding_in_payload            : 1;
    osal_u8 dot11_ldpc_coding_option_implemented      : 1;
    osal_u8 dot11_ldpc_coding_option_activated        : 1;
    osal_u8 dot11_vhtldpc_coding_option_implemented   : 1;
    osal_u8 resv                                      : 1;
    osal_u8  dot11_power_management_mode;
    osal_u32 dot11_beacon_period;
    osal_u32 dot11_dtim_period;
    uintptr_t *_rom; /* ROM化后资源扩展指针 */
} wlan_dmac_mib_stru;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wlan_mib_rom.h */
