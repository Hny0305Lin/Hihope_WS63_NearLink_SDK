/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: hmac_radar_sensor.h
 * Date: 2023-02-20
 */

#ifndef __HMAC_RADAR_SENSOR_H__
#define __HMAC_RADAR_SENSOR_H__

#include "wlan_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    osal_u8       timeout_val;
    osal_u8       ifs_sel;
    osal_u8       mac_tx_bypass;
    osal_u32      selfcts_phy_mode;
    osal_u32      selfcts_rate;
    osal_u16      selfcts_duration;
    osal_u8       selfcts_addr[WLAN_MAC_ADDR_LEN];
    osal_u16      t1;
    osal_u16      t2;
    osal_u16      t3;
    osal_u16      t4;
    osal_u16      tx_dly_160m;
    osal_u16      tx_dly_320m;
    osal_u8       ch_num;
    osal_u8       ch0_sel : 2;
    osal_u8       ch1_sel : 2;
    osal_u8       ch2_sel : 2;
    osal_u8       ch3_sel : 2;
    osal_u8       ch0_period : 4;
    osal_u8       ch1_period : 4;
    osal_u8       ch2_period : 4;
    osal_u8       ch3_period : 4;
    osal_u8       ch0_smp_period_start : 4;
    osal_u8       ch0_smp_period_end : 4;
    osal_u8       ch1_smp_period_start : 4;
    osal_u8       ch1_smp_period_end : 4;
    osal_u8       ch2_smp_period_start : 4;
    osal_u8       ch2_smp_period_end : 4;
    osal_u8       ch3_smp_period_start : 4;
    osal_u8       ch3_smp_period_end : 4;
    osal_u8       lna_gain;
    osal_u8       vga_gain;
    osal_u16      rx_dly_160m;
    osal_u16      rx_dly_80m;
    osal_u8       lpf_gain;
    osal_u8       dac_gain;
    osal_u8       pa_gain;
    osal_u16      ppa_gain;
    osal_u8       print_cnt_num;
    osal_u8       resv;
} hmac_radar_sensor_debug_stru;

typedef struct {
    osal_u8        enable;
    osal_u8        rx_nss; /* 接收天线数, n - 1配置 */
    osal_u8        period_cycle_cnt; /* 0:循环发送, 其他值:循环次数 */
    osal_u8        one_per_cnt; /* 单个雷达周期中, PHY侧循环雷达周期数 */
    osal_u8        *radar_data; /* 雷达波形起始地址 */
    osal_u16       radar_data_len; /* 雷达波形长度 */
    osal_u16       radar_period; /* 雷达波形收发间隔, 默认5ms */
} hmac_radar_sensor_cfg_stru;

typedef enum {
    RADAR_SENSOR_WIFI_SCAN = 0,                          /* WIFI发起扫描，雷达停止工作 */
    RADAR_SENSOR_PARAM_CHECK_FAIL,                       /* 参数校验失败，雷达停止工作 */
    RADAR_SENSOR_STA_DISCONNECT,                         /* STA 断连，雷达停止工作 */
    RADAR_SENSOR_WIFI_CONNETING,                         /* STA 关联中，雷达停止工作 */
    RADAR_SENSOR_LAST_VAP_DEL,                           /* 最后一个VAP删除，雷达停止工作 */
    RADAR_SENSOR_REASON_BUTT,
} radar_sensor_disable_reason_enum;
typedef osal_u8 radar_sensor_disable_reason_enum_uint8;

typedef enum {
    RADAR_SENSOR_WIFI_STA_MODE = 0,                      /* WIFI STA模式 */
    RADAR_SENSOR_WIFI_SOFTAP_MODE,                       /* WIFI SOFTAP模式 */
    RADAR_SENSOR_WIFI_MODE_BUTT,
} radar_sensor_wifi_mode_t;

typedef osal_void (*radar_handle_cb)(uint32_t *rx_mem, osal_u8 chan_num, osal_u8 bandwidth);
typedef osal_u32 (*report_radar_sensor_disable_reason_cb)(radar_sensor_disable_reason_enum_uint8 reason);
typedef osal_u8 (*hmac_radar_sensor_get_work_ch_num_cb)(osal_void);
typedef osal_void (*hmac_radar_sensor_del_vap_cb)(osal_void);
static osal_u32 hmac_radar_sensor_init_weakref(osal_void)
    __attribute__ ((weakref("hmac_radar_sensor_init"), used));
static osal_void hmac_radar_sensor_deinit_weakref(osal_void)
    __attribute__ ((weakref("hmac_radar_sensor_deinit"), used));

osal_u32 hmac_radar_sensor_register_handle_cb(radar_handle_cb cb);
osal_u16 hamc_radar_sensor_get_ppa_gain(void);
osal_float hmac_radar_sensor_config_rx_gain(osal_u8 lna_gain,
    osal_u8 vga_gain, osal_u16 rx_dly_160m, osal_u16 rx_dly_80m);
osal_void hmac_radar_sensor_config_ch_num(osal_u8 ch_num);
osal_s32 hmac_radar_sensor_start(hmac_radar_sensor_cfg_stru *radar_sensor_cfg);
osal_void hmac_radar_sensor_set_report_disable_reason_fn(report_radar_sensor_disable_reason_cb cb);
osal_void hmac_radar_sensor_one_subframe_start(osal_void);
osal_u32 hmac_radar_sensor_set_rx_dc_val(osal_u8 rxdc_gain_lna_lvl_num, osal_u8 rxdc_gain_vga_lvl_num);
osal_u32 hmac_radar_sensor_set_rx_iq_val(osal_u8 lna_code);
#ifdef _PRE_RADAR_CCA_SW_OPT
osal_void hmac_radar_sensor_cca_sw_opt(osal_bool radar_switch);
#endif
radar_sensor_wifi_mode_t hmac_radar_sensor_get_wifi_mode_info(osal_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
