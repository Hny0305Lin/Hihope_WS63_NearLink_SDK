 /*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: dbac algorithm
 */

#ifndef ALG_DBAC_SOFTAP_ROM_H
#define ALG_DBAC_SOFTAP_ROM_H

#ifdef _PRE_WLAN_FEATURE_DBAC
/******************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "oal_types_device_rom.h"
#include "alg_main_rom.h"
#include "alg_dbac_rom.h"
#include "mac_device_rom.h"
#include "frw_ext_if_rom.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define DBAC_SOFTAP_ONE_PACKET_DURATION 20

osal_void alg_dbac_sta_softap_tbtt_isr(alg_dbac_mgr_stru *dbac_mgr, osal_bool is_led_tbtt);
osal_u32 alg_dbac_sta_softap_timer_isr(alg_dbac_mgr_stru *dbac_mgr);
osal_u32 alg_dbac_parse_type_sta_ap_vap(const dmac_vap_stru *dmac_vap0, const dmac_vap_stru *dmac_vap1,
    alg_dbac_type_enum_uint8 *dbac_type, osal_u8 *led_vapid, osal_u8 *flw_vapid);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _PRE_WLAN_FEATURE_DBAC */
#endif
