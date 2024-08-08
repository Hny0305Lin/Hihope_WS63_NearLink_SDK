/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_radar_sensor_rom.c.
 */

#ifndef HAL_RADAR_SERSOR_ROM_H
#define HAL_RADAR_SERSOR_ROM_H

#include "hal_device_rom.h"
#include "hal_phy_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

osal_void hal_set_radar_sensor_period(osal_u32 period);
osal_void hal_set_radar_sensor_suspend(osal_u8 suspend_en);
osal_void hal_set_radar_sensor_timeout_val(osal_u8 val);
osal_void hal_set_radar_sensor_ifs_sel(osal_u8 ifs_sel);
osal_void hal_set_radar_sensor_switch_tx_bypass(osal_u8 bypass);
osal_void hal_set_radar_sensor_mac_enable(osal_u8 enable);
osal_void hal_set_radar_sensor_selfcts(osal_u32 phy_mode, osal_u32 data_rate, osal_u16 duration,
    osal_u8 *addr, osal_u8 addr_len);
osal_void hal_set_radar_sensor_time(osal_u16 t1, osal_u16 t2, osal_u16 t3, osal_u16 t4);
osal_void hal_radar_complete_clear(osal_void);
osal_void hal_set_radar_sensor_sig_len(osal_u16 len);
osal_void hal_set_radar_sensor_tx_delay(osal_u16 tx_dly_160m, osal_u16 tx_dly_320m);
osal_void hal_set_radar_sensor_ch_period(osal_u8 ch1_period, osal_u8 ch2_period,
    osal_u8 ch3_period, osal_u8 ch4_period);
osal_void hal_set_radar_sensor_ch_sel(osal_u8 ch0_sel, osal_u8 ch1_sel, osal_u8 ch2_sel, osal_u8 ch3_sel);
osal_void hal_set_radar_sensor_ch_num(osal_u8 ch_num);
osal_void hal_set_radar_sensor_rx_delay(osal_u8 lna_gain, osal_u8 vga_gain,
    osal_u16 rx_dly_160m, osal_u16 rx_dly_80m);
osal_void hal_set_radar_sensor_tx_gain(osal_u8 tx_lpf_gain, osal_u8 dac_gain, osal_u8 pa_gain, osal_u16 ppa_gain);
osal_void hal_set_radar_sensor_ch_smp_period(osal_u8 ch, osal_u8 start_period, osal_u8 end_period);
osal_void hal_set_radar_sensor_phy_enable(osal_u8 enable);
osal_void hal_set_radar_sensor_smp_with_ch(osal_u8 enable);
osal_void hal_set_radar_sensor_rx_iq_bypass(osal_u8 enable);
osal_void hal_get_radar_sensor_adc_freq(osal_u16 *adc_num);
osal_void hal_get_radar_sensor_dac_freq(osal_u16 *dac_num);
osal_void hal_get_radar_sensor_dac_adc_num_ratio(osal_u8 *phy_dac_adc_num_ratio);
osal_void hal_set_radar_sensor_soc_txrx_addr(osal_u32 tx_start_addr, osal_u32 tx_end_addr,
    osal_u32 rx_start_addr, osal_u32 rx_end_addr);
osal_void hal_irq_radar_sensor_det(hal_device_stru *hal_device, osal_u8 data);

typedef osal_void (*hal_irq_radar_sensor_det_cb)(hal_device_stru *hal_device, osal_u8 data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
