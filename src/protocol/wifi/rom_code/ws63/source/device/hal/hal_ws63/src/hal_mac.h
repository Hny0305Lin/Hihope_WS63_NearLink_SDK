/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_mac.c.
 */

#ifndef __HAL_MAC_H__
#define __HAL_MAC_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hal_mac_rom.h"
#include "hal_dscr_rom.h"
#include "hal_ext_if_device.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_MAC_H

#ifdef _PRE_WLAN_FEATURE_DAQ
#define HAL_MAC_DIAG_MATCH_ADDR_MASK  (0xFF)       // 匹配地址0xFF [7-0]
#define HAL_MAC_DIAG_MATCH_FRM_MASK   (0x3F)       // 匹配帧类型0x3F [5-0]
#define HAL_MAC_DIAG_DAQ_OFFSET 0x0

typedef enum {
    HAL_MAC_DIAG_TEST_INIT = 0,
    HAL_MAC_DIAG_TEST_MATCH,
    HAL_MAC_DIAG_TEST_QUERY = 11,
    HAL_MAC_DIAG_TEST_STOP,
} hal_mac_diag_test_type_enum;
typedef enum {
    HAL_MAC_DIAG_DUG = 0,
    HAL_MAC_DIAG_TEST1,
    HAL_MAC_DIAG_TEST4 = 4,
    HAL_MAC_DIAG_TEST8 = 8,
} hal_mac_diag_test_item_enum;
typedef enum {
    HAL_MAC_TX_DIAG_MATCH_RA_ADDR = 0,
    HAL_MAC_TX_DIAG_MATCH_TA_ADDR,
    HAL_MAC_TX_DIAG_MATCH_FRM_TYPE,
    HAL_MAC_RX_DIAG_MATCH_RA_ADDR,
    HAL_MAC_RX_DIAG_MATCH_TA_ADDR,
    HAL_MAC_RX_DIAG_MATCH_FRM_TYPE,
    HAL_MAC_TX_RX_DIAG_MATCH_NUM,
} hal_mac_diag_test_match_enum;
typedef enum {
    HAL_MAC_DIAG4_NODE_DBAC = 0,
    HAL_MAC_DIAG4_NODE_FSM,
    HAL_MAC_DIAG4_NODE_TX_AMPDU0,
    HAL_MAC_DIAG4_NODE_TSF_VAP3 = 31,
} hal_mac_diag4_node_enum;
typedef enum {
    HAL_MAC_DIAG8_NODE_H = 0,
    HAL_MAC_DIAG8_NODE_HV,
    HAL_MAC_DIAG8_NODE_SNR,
    HAL_MAC_DIAG8_NODE_PHI_PSI,
} hal_mac_diag8_node_enum;
typedef enum {
    HAL_MAC_DIAG_SEL_PKT_RAM_2KB = 0,
    HAL_MAC_DIAG_SEL_PKT_RAM_4KB,
    HAL_MAC_DIAG_SEL_PKT_RAM_6KB,
    HAL_MAC_DIAG_SEL_PKT_RAM_8KB,
    HAL_MAC_DIAG_SEL_PKT_RAM_10KB,
    HAL_MAC_DIAG_SEL_PKT_RAM_12KB,
    HAL_MAC_DIAG_SEL_PKT_RAM_14KB,
    HAL_MAC_DIAG_SEL_PKT_RAM_16KB,
} hal_mac_diag_sel_pkt_ram_size_enum;

osal_void hal_set_mac_diag_param(osal_u8 vap_id, mac_cfg_mct_set_diag_stru *event_set_diag,
    hal_to_dmac_device_mac_test_stru *hal_mac_test_device);
osal_void hal_pkt_ram_sample_deinit(osal_void);
osal_void hal_sample_daq_prepare_data(osal_void);
osal_void hal_set_mac_diag_mode(mac_cfg_mct_set_diag_stru *event_set_diag,
    hal_to_dmac_device_mac_test_stru *hal_mac_test_device);
osal_void hal_set_mac_diag_test_query(hal_to_dmac_device_mac_test_stru *hal_mac_test_device);
osal_void hal_set_mac_diag_test_query(hal_to_dmac_device_mac_test_stru *hal_mac_test_device);
osal_u32 hal_sample_daq_get_data(osal_u32 diag_read_addr);
oal_bool_enum_uint8 hal_sample_daq_done(osal_void);
osal_void hal_show_mac_daq_reg_cfg_info(osal_void);
osal_void hal_show_daq_done_reg_info(osal_void);
#endif
#ifdef _PRE_WIFI_DEBUG
osal_void hal_show_linkloss_reg_info(osal_void);
#endif

osal_void hal_radar_sensing_switch_radar_param2(osal_bool is_on);
osal_void hal_radar_sensing_one_sub_frame_start(osal_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_mac.h */
