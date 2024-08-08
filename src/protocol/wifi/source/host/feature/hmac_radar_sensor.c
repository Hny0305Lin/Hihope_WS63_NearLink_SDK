/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:
 * Date: 2023-02-20
 */
#include "hmac_radar_sensor.h"
#include "math.h"
#include "mac_resource_ext.h"
#include "hal_radar_sensor_rom.h"
#include "hmac_hook.h"
#include "hmac_config.h"
#include "hmac_power.h"
#include "hmac_alg_notify.h"
#include "hmac_vap.h"
#include "hmac_ccpriv.h"
#include "frw_util_notifier.h"
#include "hal_mac_rom.h"
#include "hal_phy.h"
#include "hal_mac.h"
#include "hal_rf.h"
#ifdef BOARD_FPGA_WIFI
#include "cali_path.h"
#include "cali_entry.h"
#include "cali_channel.h"
#endif
#include "cali_data.h"
#include "cali_comp.h"
#include "memory_config_common.h"
#include "hmac_chan_mgmt.h"
#include "hal_chan_mgmt.h"
#ifdef _PRE_RADAR_CCA_SW_OPT
#include "hmac_alg_notify.h"
#endif
#include "frw_util_notifier.h"
#include "hmac_feature_interface.h"
#include "hal_rx_rom.h"
#include "hal_device_fsm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_RADAR_SENSOR_C

#undef  THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_HOST

#define RADAR_SENSOR_MAX_LNA_GAIN 7
#define RADAR_SENSOR_MAX_VGA_GAIN 31
#define RADAR_SENSOR_RX_DELAY_160M 1023
#define RADAR_SENSOR_RX_DELAY_80M 1023

#define RADAR_SENSOR_MAX_LPF_GAIN 7
#define RADAR_SENSOR_MAX_DAC_GAIN 3
#define RADAR_SENSOR_MAX_PA_GAIN 255
#define RADAR_SENSOR_MAX_PPA_GAIN 65535

#define RADAR_SENSOR_MAX_T1 255
#define RADAR_SENSOR_MAX_T2 255
#define RADAR_SENSOR_MAX_T3 65535
#define RADAR_SENSOR_MAX_T4 255

#define DEVICE_SUPPORT_RX_NSS_NUM 0
#define DEVICE_SUPPORT_MAX_RX_NSS_NUM 3
#define MIN_RADAR_PERIOD 1000  /* 支持配置最小1ms的雷达周期，单位us */
#define MAX_RADAR_PERIOD 10000 /* 支持配置最大10ms的雷达周期，单位us */
#define MIN_RADAR_SENSOR_PHY_CNT 1     /* 支持配置最小phy侧内部循环次数为1 */
#define MAX_RADAR_SENSOR_PHY_CNT 32    /* 支持配置最大phy侧内部循环次数为32 */
#define MAX_RADAR_SENSOR_CYCLE_CNT 3   /* 支持配置雷达循环次数最大为3次 */

#define MAX_RADAR_WORK_MIN_CHAN 1
#define MAX_RADAR_WORK_MAX_CHAN 13

#define MAX_VGA_GAIN_VALUE 30
#define MIN_VGA_GAIN_VALUE (-2)
#define DAC_OUTPUT_VALUE (-8)
#define ADC_INPUT_VALUE (4)
#define PA_CODE_VALUE 15
#define PPA_VALUE_CNT 10

#define DATA_LEN_TO_SIG_LEN_RATIO 4 /* 4:数据长度转换采样点数比例,一个采样点4个字节 */

#define RADAR_SENSOR_TX_MEM_ADDR 0xA98000
#define TX_MAX_RADAR_WAVEFORM_LEN (1024 * 2) // 发送雷达波形最大不超过2KB

#define RADAR_SENSOR_RX_MEM_ADDR RADAR_SENSOR_RX_MEM_START
#define RX_MAX_RADAR_WAVEFORM_LEN RADAR_SENSOR_RX_MEM_SIZE // 默认接收雷达波形8KB

#define RADAR_RX_DC_COMP_SHIFT_BIT  8
#define RADAR_MAX_VGA_DB    24
#define RADAR_VGA_REF_DB    32

#define RADAR_TX_PWR 11.5
#define RADAR_RX_LPF_PWR 3

OSAL_STATIC report_radar_sensor_disable_reason_cb g_report_radar_sensor_disable_reason = OSAL_NULL;
OSAL_STATIC osal_void hmac_radar_sensor_disable(osal_void);
OSAL_STATIC radar_sensor_wifi_mode_t g_wifi_mode_info = RADAR_SENSOR_WIFI_STA_MODE;

typedef struct {
    osal_u8 probe_enable;     /* 当前雷达是否处于探测 */
    osal_u8 cycle_en;         /* 是否循环发送，1：一直循环发送 0：固定次数发送 */
    osal_u16 period_cnt;       /* 循环发送的次数 */
    hmac_radar_sensor_debug_stru debug_info;
} hmac_radar_sensor_info_stru;

typedef struct {
    osal_s16 ppa_db;
    osal_s16 ppa_code;
} hmac_radar_sensor_ppa_val_stru;

hmac_radar_sensor_info_stru g_radar_sensor_info;

uintptr_t g_radar_tx_mem = RADAR_SENSOR_TX_MEM_ADDR;
uintptr_t g_radar_rx_mem = RADAR_SENSOR_RX_MEM_ADDR;

OSAL_STATIC radar_handle_cb g_radar_handle_cb = 0;
OSAL_STATIC osal_bool g_is_cb_registered = OSAL_FALSE;

static const int8_t g_lna_code_value[] = { -2, 4, 10, 16, 22, 28, 34 };
static osal_u8 g_lpf_gain;
static osal_u8 g_dac_gain;
static osal_u8 g_pa_gain;
static osal_u16 g_ppa_gain;

OSAL_STATIC void hmac_set_tx_gain_info(osal_u8 lpf_gain, osal_u8 dac_gain, osal_u8 pa_gain, osal_u16 ppa_gain)
{
    g_lpf_gain = lpf_gain;
    g_dac_gain = dac_gain;
    g_pa_gain = pa_gain;
    g_ppa_gain = ppa_gain;
    hal_set_radar_sensor_tx_gain(lpf_gain, dac_gain, pa_gain, ppa_gain);
}

/* 获取雷达探测控制信息 */
OSAL_STATIC hmac_radar_sensor_info_stru *hmac_radar_sensor_get_info(osal_void)
{
    return &g_radar_sensor_info;
}

/* 获取雷达探测调试信息 */
OSAL_STATIC hmac_radar_sensor_debug_stru *hmac_radar_sensor_get_debug_info(osal_void)
{
    return &g_radar_sensor_info.debug_info;
}

OSAL_STATIC osal_u8 *hmac_radar_sensor_get_tx_mem_addr(osal_void)
{
    return (osal_u8 *)g_radar_tx_mem;
}

OSAL_STATIC osal_u8 *hmac_radar_sensor_get_rx_mem_addr(osal_void)
{
    return (osal_u8 *)g_radar_rx_mem;
}

OSAL_STATIC osal_void hmac_radar_sensor_clear_tx_mem(osal_void)
{
    (osal_void)memset_s((osal_u8 *)g_radar_tx_mem, TX_MAX_RADAR_WAVEFORM_LEN, 0, TX_MAX_RADAR_WAVEFORM_LEN);
}

OSAL_STATIC osal_void hmac_radar_sensor_clear_rx_mem(osal_void)
{
    (osal_void)memset_s((osal_u8 *)g_radar_rx_mem, RX_MAX_RADAR_WAVEFORM_LEN, 0, RX_MAX_RADAR_WAVEFORM_LEN);
}

OSAL_STATIC osal_void report_radar_sensor_disable_reason(radar_sensor_disable_reason_enum_uint8 reason)
{
    if (g_report_radar_sensor_disable_reason != OSAL_NULL) {
        g_report_radar_sensor_disable_reason(reason);
    }
}

osal_void hmac_radar_sensor_set_report_disable_reason_fn(report_radar_sensor_disable_reason_cb cb)
{
    g_report_radar_sensor_disable_reason = cb;
}

OSAL_STATIC osal_void hmac_radar_sensor_info_init(osal_u16 period_cycle_cnt)
{
    hmac_radar_sensor_info_stru *radar_sensor_info = hmac_radar_sensor_get_info();

    if (period_cycle_cnt == 0) {
        radar_sensor_info->cycle_en = OSAL_TRUE;
    }
    radar_sensor_info->period_cnt = period_cycle_cnt;
    radar_sensor_info->probe_enable = OSAL_TRUE;
}

OSAL_STATIC osal_void hmac_radar_sensor_info_deinit(osal_void)
{
    hmac_radar_sensor_info_stru *radar_sensor_info = hmac_radar_sensor_get_info();

    radar_sensor_info->cycle_en = OSAL_FALSE;
    radar_sensor_info->period_cnt = 0;
    radar_sensor_info->probe_enable = OSAL_FALSE;
}

OSAL_STATIC osal_u32 hmac_radar_sensor_get_work_chan(osal_u8 *channel_num)
{
    hmac_device_stru *hmac_device = hmac_res_get_mac_dev_etc(0);
    hmac_vap_stru *hmac_vap = OSAL_NULL;
    /* 至少需要一个设备存在才能进行雷达探测 */
    if (hmac_device->vap_num == 0) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_radar_sensor_get_work_chan:not exist vap");
        return OAL_FAIL;
    }

    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(hmac_device->vap_id[0]);
    if (hmac_vap == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_radar_sensor_get_work_chan: vap is null");
        return OAL_FAIL;
    }
    *channel_num = hmac_vap->channel.chan_number;
    return OAL_SUCC;
}

OSAL_STATIC osal_u32 hmac_radar_sensor_set_tx_dc_val(osal_void)
{
    cali_param_stru *cali = cali_get_cali_data(0);
    cali_param_2g_stru *cali_2g = &cali->cali_param_2g;
    osal_u8 channel_num, subband_idx;
    osal_u32 ret;

    ret = hmac_radar_sensor_get_work_chan(&channel_num);
    if (ret != OAL_SUCC || channel_num == 0) {
        return OAL_FAIL;
    }
    subband_idx = cali_get_cali_subband_idx_by_chan_num(WLAN_BAND_2G, channel_num);
    // 1：TX DC 补偿档位 1档，低功率档位
    hal_radar_sensing_set_tx_dc_val(cali_2g->txdc_cmp_val[subband_idx][1].txdc_cmp_i,
        cali_2g->txdc_cmp_val[subband_idx][1].txdc_cmp_q);

    return OAL_SUCC;
}

osal_u32 hmac_radar_sensor_set_rx_dc_val(osal_u8 rxdc_gain_lna_lvl_num, osal_u8 rxdc_gain_vga_lvl_num)
{
    cali_param_stru *cali = cali_get_cali_data(0);
    cali_param_2g_stru *cali_2g = &cali->cali_param_2g;
    osal_u8 rxdc_gain_vga_signed_val = (rxdc_gain_vga_lvl_num <= RADAR_MAX_VGA_DB) ?
        rxdc_gain_vga_lvl_num : (rxdc_gain_vga_lvl_num - RADAR_VGA_REF_DB);
    osal_u8 rxdc_gain_vga_lvl = (rxdc_gain_vga_signed_val <= 2) ? 0 : 1; // 挡位转换，小于2则转为0档，大于2则转为1档
    osal_u16 rxdc_siso_cmp;

    rxdc_siso_cmp =
        cali_2g->cali_rx_dc_cmp[0].aus_analog_rxdc_siso_cmp[rxdc_gain_lna_lvl_num][rxdc_gain_vga_lvl];

    // i部为高8bit，左移8位，取高8bit，q部为低8bit,
    hal_radar_sensing_set_rx_dc_val((rxdc_siso_cmp >> RADAR_RX_DC_COMP_SHIFT_BIT) & 0xFF, rxdc_siso_cmp & 0xFF,
        cali_2g->cali_rx_dc_cmp[0].digital_rxdc_cmp_i, cali_2g->cali_rx_dc_cmp[0].digital_rxdc_cmp_q);

    return OAL_SUCC;
}

OSAL_STATIC osal_u32 hmac_radar_sensor_set_tx_iq_val(osal_void)
{
    cali_param_stru *cali = cali_get_cali_data(0);
    cali_param_2g_stru *cali_2g = &cali->cali_param_2g;
    osal_u8 channel_num, subband_idx;
    osal_u32 ret;
    cali_txiq_tone_comp_val_stru *txiq_tone_comp_val = OSAL_NULL;

    ret = hmac_radar_sensor_get_work_chan(&channel_num);
    if (ret != OAL_SUCC || channel_num == 0) {
        return OAL_FAIL;
    }
    subband_idx = cali_get_cali_subband_idx_by_chan_num(WLAN_BAND_2G, channel_num);
    // 1：TX IQ 补偿档位 1档，低功率档位
    txiq_tone_comp_val = &cali_2g->tx_iq_val[subband_idx][1];

    hal_radar_sensing_set_tx_iq_val(txiq_tone_comp_val->ls_fir);

    return OAL_SUCC;
}

osal_u32 hmac_radar_sensor_set_rx_iq_val(osal_u8 lna_code)
{
    cali_param_stru *cali = cali_get_cali_data(0);
    cali_param_2g_stru *cali_2g = &cali->cali_param_2g;
    osal_u8 lna_code_num = (lna_code == 0) ? 0 : 1;
    osal_u8 channel_num, subband_idx;
    osal_u32 ret;
    cali_rxiq_tone_comp_val_stru *rxiq_tone_comp_val = OSAL_NULL;

    ret = hmac_radar_sensor_get_work_chan(&channel_num);
    if (ret != OAL_SUCC || channel_num == 0) {
        return OAL_FAIL;
    }
    subband_idx = cali_get_cali_subband_idx_by_chan_num(WLAN_BAND_2G, channel_num);
    // 雷达感知默认使用40M带宽
    rxiq_tone_comp_val = &cali_2g->rx_iq_val[subband_idx][lna_code_num][CALI_BW_40M];

    hal_radar_sensing_set_rx_iq_val(rxiq_tone_comp_val->ls_fir);

    return OAL_SUCC;
}

OSAL_STATIC osal_u32 hmac_radar_sensor_set_tx_power_val(osal_u16 *atx_pwr_cmp)
{
    cali_param_stru *cali = cali_get_cali_data(0);
    cali_param_2g_stru *cali_2g = &cali->cali_param_2g;
    osal_u8 channel_num, subband_idx;
    osal_u32 ret;

    ret = hmac_radar_sensor_get_work_chan(&channel_num);
    if (ret != OAL_SUCC || channel_num == 0) {
        return OAL_FAIL;
    }
    subband_idx = cali_get_cali_subband_idx_by_chan_num(WLAN_BAND_2G, channel_num);
    /* 1：tx pwr补偿值低功率档位 */
    *atx_pwr_cmp = cali_2g->cali_tx_power_cmp_2g[subband_idx].atx_pwr_cmp[1];

    return OAL_SUCC;
}

OSAL_STATIC osal_u32 hmac_radar_sensor_param_check(hmac_radar_sensor_cfg_stru *radar_sensor_cfg)
{
    hmac_radar_sensor_info_stru *radar_sensor_info = hmac_radar_sensor_get_info();
    osal_u8 phy_dac_adc_num_ratio;
    osal_u32 rx_mem_all;

    if (radar_sensor_info->probe_enable == OSAL_TRUE) {
        oam_error_log1(0, OAM_SF_ANY, "hmac_radar_sensor_param_check:enable[%d]", radar_sensor_info->probe_enable);
        return OAL_FAIL;
    }

    if (radar_sensor_cfg->rx_nss > DEVICE_SUPPORT_RX_NSS_NUM) {
        oam_error_log1(0, OAM_SF_ANY, "hmac_radar_sensor_enable:rx_nss[%d]", radar_sensor_cfg->rx_nss);
        return OAL_FAIL;
    }

    if (radar_sensor_cfg->radar_data_len > TX_MAX_RADAR_WAVEFORM_LEN) {
        oam_error_log1(0, OAM_SF_ANY, "hmac_radar_sensor_param_check:data_len[%d]", radar_sensor_cfg->radar_data_len);
        return OAL_FAIL;
    }

#ifdef BOARD_FPGA_WIFI
    phy_dac_adc_num_ratio = 1; // FPGA 雷达感知内部ADC/DAC降至40M,比率为1
#else
    hal_get_radar_sensor_dac_adc_num_ratio(&phy_dac_adc_num_ratio);
#endif
    if (phy_dac_adc_num_ratio == 0) {
        oam_error_log0(0, OAM_SF_ANY, "hmac_radar_sensor_param_check:adc > dac freq");
        return OAL_FAIL;
    }

    rx_mem_all = ((osal_u32)radar_sensor_cfg->radar_data_len * radar_sensor_cfg->one_per_cnt) / phy_dac_adc_num_ratio;
    if (rx_mem_all > RX_MAX_RADAR_WAVEFORM_LEN) {
        oam_error_log1(0, OAM_SF_ANY, "hmac_radar_sensor_param_check:rx data size large[%d]", rx_mem_all);
        return OAL_FAIL;
    }

    if (radar_sensor_cfg->radar_data == OSAL_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "hmac_radar_sensor_param_check:radar_data is null");
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

OSAL_STATIC osal_u32 hmac_radar_sensor_config_freq(osal_void)
{
    hmac_device_stru *hmac_device = hmac_res_get_mac_dev_etc(0);
    hmac_radar_sensor_debug_stru *debug_info = hmac_radar_sensor_get_debug_info();
    hmac_vap_stru *hmac_vap = OSAL_NULL;
    frw_msg msg;
    osal_u8 up_vap_num = 0;
    osal_u8 channel_num = debug_info->ch_num;

    (osal_void)memset_s(&msg, sizeof(frw_msg), 0, sizeof(frw_msg));

    /* 至少需要一个设备存在才能进行雷达探测 */
    if (hmac_device->vap_num == 0) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_radar_sensor_config_freq:not exist vap");
        return OAL_FAIL;
    }

    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(hmac_device->vap_id[0]);
    if (hmac_vap == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_radar_sensor_config_freq: vap is null");
        return OAL_FAIL;
    }

    if (is_legacy_ap(hmac_vap) == OSAL_TRUE) {
        g_wifi_mode_info = RADAR_SENSOR_WIFI_SOFTAP_MODE;
    } else if (is_legacy_sta(hmac_vap) == OSAL_TRUE) {
        g_wifi_mode_info = RADAR_SENSOR_WIFI_STA_MODE;
    } else {
        g_wifi_mode_info = RADAR_SENSOR_WIFI_MODE_BUTT;
    }

    hal_set_radar_config_enable(OSAL_TRUE);
    /* 存在up的vap则复用up vap的信道信息 */
    up_vap_num = (osal_u8)hmac_device_calc_up_vap_num_etc(hmac_device);
    if (up_vap_num != 0) {
        hmac_mgmt_switch_channel(hmac_vap->hal_device, &hmac_vap->channel, OSAL_FALSE);
        oam_warning_log0(0, OAM_SF_ANY, "hmac_radar_sensor_config_freq:user already vap");
        return OAL_SUCC; // STA关联状态和softAP模式时, 是直接返回, 不按照雷达设置的信道配置
    }

    msg.data = &channel_num;
    msg.data_len = sizeof(osal_u8);
    hmac_config_set_freq_etc(hmac_vap, &msg);
    /* 初始化发送功率 */
    hmac_pow_set_vap_tx_power(hmac_vap, HAL_POW_SET_TYPE_INIT);
    /* 通知算法信道变化 */
    hmac_alg_cfg_channel_notify(hmac_vap, CH_BW_CHG_TYPE_MOVE_WORK);
    hmac_mgmt_switch_channel(hmac_vap->hal_device, &hmac_vap->channel, OSAL_FALSE);

    hal_enable_machw_phy_and_pa(hmac_vap->hal_device);

    return OAL_SUCC;
}

OSAL_STATIC osal_u32 hmac_radar_sensor_config_soc(osal_u8 *waveform_data, osal_u16 waveform_len, osal_u8 one_per_cnt)
{
    osal_u32 tx_start_addr;
    osal_u32 tx_end_addr;
    osal_u32 rx_start_addr;
    osal_u32 rx_end_addr;
    osal_u8 phy_dac_adc_num_ratio;
    errno_t ret;
    osal_u8 *tx_mem = hmac_radar_sensor_get_tx_mem_addr();
    osal_u8 *rx_mem = hmac_radar_sensor_get_rx_mem_addr();

    /* 使能后清空发送/接收缓存 */
    hmac_radar_sensor_clear_tx_mem();
    hmac_radar_sensor_clear_rx_mem();
#ifdef BOARD_FPGA_WIFI
    phy_dac_adc_num_ratio = 1; // FPGA 雷达感知内部ADC/DAC降至40M,比率为1
#else
    hal_get_radar_sensor_dac_adc_num_ratio(&phy_dac_adc_num_ratio);
#endif
    if (phy_dac_adc_num_ratio == 0) {
        oam_error_log0(0, OAM_SF_ANY, "hmac_radar_sensor_config_soc:adc > dac freq");
        return OAL_FAIL;
    }

    ret = memcpy_s(tx_mem, TX_MAX_RADAR_WAVEFORM_LEN, waveform_data, waveform_len);
    if (ret != EOK) {
        oam_error_log1(0, OAM_SF_ANY, "hmac_radar_sensor_config_soc:memcpy_s fail[%d]", ret);
        return OAL_FAIL;
    }

    tx_start_addr = (osal_u32)tx_mem;
    /* 4:由于需要算上第一个地址，所以地址数据总长度需要减去4字节 */
    tx_end_addr = (osal_u32)(tx_mem + waveform_len - 4);

    rx_start_addr = (osal_u32)rx_mem;
    /* 4:由于需要算上第一个地址，所以地址数据总长度需要减去4字节 */
    rx_end_addr = (osal_u32)(rx_mem + ((one_per_cnt * (osal_u32)waveform_len) / phy_dac_adc_num_ratio) - 4);

    hal_set_radar_sensor_soc_txrx_addr(tx_start_addr, tx_end_addr, rx_start_addr, rx_end_addr);

    return OAL_SUCC;
}

OSAL_STATIC osal_void hmac_radar_sensor_config_phy(osal_u16 radar_data_len, osal_u8 one_per_cnt, osal_u8 rx_nss)
{
    osal_u16 radar_sig_len = radar_data_len / DATA_LEN_TO_SIG_LEN_RATIO;
    hmac_radar_sensor_debug_stru *debug_info = hmac_radar_sensor_get_debug_info();

#ifdef BOARD_FPGA_WIFI
    /* FPGA SoC时钟频率只有30Mhz，phy侧为160M，需要降频至40M,为满足原有数据长度，需要将phy侧周期数*4 */
    hal_set_radar_sensor_sig_len(radar_sig_len * 4);
#else
    hal_set_radar_sensor_sig_len(radar_sig_len);
#endif
    // 依据校准值刷新tx power
    hmac_radar_sensor_set_tx_power_val(&debug_info->ppa_gain);
    hmac_set_tx_gain_info(debug_info->lpf_gain, debug_info->dac_gain, debug_info->pa_gain, debug_info->ppa_gain);

    hal_set_radar_sensor_tx_delay(debug_info->tx_dly_160m, debug_info->tx_dly_320m);
    hal_set_radar_sensor_ch_period(one_per_cnt - 1, 0, 0, 0);
    hal_set_radar_sensor_ch_sel(RF_CH_0, RF_CH_1, RF_CH_2, RF_CH_3);
    hal_set_radar_sensor_ch_num(rx_nss);
    hal_set_radar_sensor_rx_delay(debug_info->lna_gain, debug_info->vga_gain,
        debug_info->rx_dly_160m, debug_info->rx_dly_80m);
    hal_set_radar_sensor_ch_smp_period(PHY_CH_0, 0x1, one_per_cnt - 1);
    hal_set_radar_sensor_phy_enable(OSAL_TRUE);
    hal_set_radar_sensor_smp_with_ch(OSAL_FALSE); /* 维测功能：默认不开启 */
    hal_set_radar_sensor_rx_iq_bypass(OSAL_FALSE);
}

OSAL_STATIC osal_float hmac_radar_sensor_calc_trx_chain(osal_u8 lna_gain, osal_u8 vga_gain)
{
    osal_float tx_gain = RADAR_TX_PWR;
    osal_s16 vga_gain_value = vga_gain;
    if (vga_gain_value == MAX_VGA_GAIN_VALUE) {
        vga_gain_value = MIN_VGA_GAIN_VALUE;
    }
    osal_float rx_gain = ADC_INPUT_VALUE - (g_lna_code_value[lna_gain] + vga_gain_value + RADAR_RX_LPF_PWR);

    return (tx_gain - rx_gain);
}

osal_u16 hamc_radar_sensor_get_ppa_gain(void)
{
    return g_ppa_gain;
}

osal_float hmac_radar_sensor_config_rx_gain(osal_u8 lna_gain,
    osal_u8 vga_gain, osal_u16 rx_dly_160m, osal_u16 rx_dly_80m)
{
    hal_set_radar_sensor_rx_delay(lna_gain, vga_gain, rx_dly_160m, rx_dly_80m);

    osal_float isolation_value = hmac_radar_sensor_calc_trx_chain(lna_gain, vga_gain);

    return isolation_value;
}

osal_void hmac_radar_sensor_config_ch_num(osal_u8 ch_num)
{
    hmac_radar_sensor_debug_stru *debug_info = hmac_radar_sensor_get_debug_info();
    debug_info->ch_num = ch_num;
}

osal_u8 hmac_radar_sensor_get_work_ch_num(osal_void)
{
    hmac_radar_sensor_debug_stru *debug_info = hmac_radar_sensor_get_debug_info();
    hmac_radar_sensor_info_stru *radar_sensor_info = hmac_radar_sensor_get_info();

    if (radar_sensor_info->probe_enable != OSAL_TRUE) {
        return 0;
    }

    return debug_info->ch_num;
}

OSAL_STATIC osal_void hmac_radar_sensor_calc_t3(osal_u16 radar_data_len, osal_u8 one_per_cnt, osal_u16 *t3)
{
    osal_u16 tmp_t3;
    osal_u16 radar_sig_len = radar_data_len / DATA_LEN_TO_SIG_LEN_RATIO;
    osal_u16 dac_num = PHY_DAC_160M_NUM;
#ifdef BOARD_FPGA_WIFI
    dac_num = 40; // FPGA 雷达感知内部DAC降至40M
#else
    hal_get_radar_sensor_dac_freq(&dac_num);
#endif
    if (dac_num == 0) {
        return;
    }

    /* 此处存在精度丢失，最大丢失精度为DAC 40M时，丢失时间为1/40 = 0.025us */
    tmp_t3 = radar_sig_len * one_per_cnt / (dac_num);
    tmp_t3 = tmp_t3 * 10 + 100; // 100:额外冗余10us，单位0.1us

    *t3 = tmp_t3;
}

OSAL_STATIC osal_void hmac_radar_sensor_config_mac(osal_u32 radar_period, osal_u16 radar_data_len, osal_u8 one_per_cnt)
{
    osal_u16 t3 = 0;
    osal_u8 *mac_addr = ((osal_u8 *)"\xFF\xFF\xFF\xFF\xFF\xFF");
    hmac_radar_sensor_debug_stru *debug_info = hmac_radar_sensor_get_debug_info();
    osal_u32 duration;

    hmac_radar_sensor_calc_t3(radar_data_len, one_per_cnt, &t3);
    if (debug_info->t3 != 0) {
        t3 = debug_info->t3;
    }

    // 注:此处存在精度丢失,最大丢失1/10 = 0.1us
    /* duration= T1:100 + T2:14 + T3:计算得出 + T4:10us + 预留50 单位：0.1us */
    duration = (debug_info->t1 + debug_info->t2 + t3 + debug_info->t4 + 50) / 10;

    hal_set_radar_sensor_period(radar_period);
    hal_set_radar_sensor_suspend(OSAL_FALSE);
    hal_set_radar_sensor_timeout_val(0x1C); /* 1.792ms超时时间     单位：64us */
    hal_set_radar_sensor_ifs_sel(0x1); /* 雷达发送采用PIFS */
    hal_set_radar_sensor_switch_tx_bypass(OSAL_FALSE);
    hal_set_radar_sensor_selfcts(0x0, (debug_info->selfcts_rate & 0x3FFFFF), (osal_u16)duration, mac_addr,
        WLAN_MAC_ADDR_LEN);

    hal_set_radar_sensor_time(debug_info->t1, debug_info->t2, t3, debug_info->t4);
    wifi_printf("hmac_radar_sensor_config_mac:t1[%d] t2[%d] t3[%d] t4[%d]\r\n",
        debug_info->t1, debug_info->t2, t3, debug_info->t4);
    hal_radar_complete_clear();
}

OSAL_STATIC osal_void hmac_radar_sensor_config_related_reg(osal_bool is_on)
{
    hal_radar_sensing_switch_abb_lo_tx_top_test_reg12(is_on);
    hal_radar_sensing_switch_linectrl(is_on);
    hal_radar_sensing_switch_radar_param2(is_on);
}

radar_sensor_wifi_mode_t hmac_radar_sensor_get_wifi_mode_info(osal_void)
{
    return g_wifi_mode_info;
}

OSAL_STATIC osal_u32 hmac_radar_sensor_enable(hmac_radar_sensor_cfg_stru *data)
{
    data->radar_data = (osal_u8 *)data->radar_data;
    data->radar_data_len = data->radar_data_len;

    osal_u32 ret = hmac_radar_sensor_config_freq();
    if (ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_radar_sensor_enable:freq set fail");
        return ret;
    }
    // 配置雷达校准值
    hmac_radar_sensor_set_tx_dc_val();
    hmac_radar_sensor_set_tx_iq_val();

    ret = hmac_radar_sensor_param_check(data);
    if (ret != OAL_SUCC) {
        hmac_radar_sensor_disable();
        report_radar_sensor_disable_reason(RADAR_SENSOR_PARAM_CHECK_FAIL);
        oam_warning_log0(0, OAM_SF_ANY, "hmac_radar_sensor_enable:param check fail");
        return ret;
    }

    ret = hmac_radar_sensor_config_soc(data->radar_data, data->radar_data_len, data->one_per_cnt);
    if (ret != OAL_SUCC) {
        hmac_radar_sensor_disable();
        report_radar_sensor_disable_reason(RADAR_SENSOR_PARAM_CHECK_FAIL);
        oam_warning_log0(0, OAM_SF_ANY, "hmac_radar_sensor_enable:config waveform mem fail");
        return ret;
    }

    hmac_radar_sensor_config_phy(data->radar_data_len, data->one_per_cnt, data->rx_nss);
    hmac_radar_sensor_config_mac(data->radar_period, data->radar_data_len, data->one_per_cnt);
    hmac_radar_sensor_config_related_reg(OSAL_TRUE);

    hmac_radar_sensor_info_init(data->period_cycle_cnt);
    hal_set_radar_sensor_mac_enable(OSAL_TRUE);

    return OAL_SUCC;
}

OSAL_STATIC osal_void hmac_radar_sensor_disable(osal_void)
{
    hal_device_stru *hal_device = (hal_device_stru *)hal_chip_get_hal_device();
    hmac_device_stru *hmac_device = hmac_res_get_mac_dev_etc(0);
    hmac_vap_stru *hmac_vap = OSAL_NULL;

    hmac_radar_sensor_info_deinit();
    /* 至少需要一个设备存在才能进行雷达探测 */
    if (hmac_device->vap_num == 0) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_radar_sensor_disable:not exist vap");
        return;
    }
    /* 雷达40M恢复流程 */
    hal_set_radar_config_enable(OSAL_FALSE);
    hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(hmac_device->vap_id[0]);
    if (hmac_vap == OSAL_NULL) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_radar_sensor_disable hmac_vap is null");
        return;
    }
    hmac_mgmt_switch_channel(hmac_vap->hal_device, &hmac_vap->channel, OSAL_FALSE);

    hal_set_radar_sensor_mac_enable(OSAL_FALSE);
    if ((hal_dev_fsm_get_curr_state() == HAL_DEVICE_IDLE_STATE) ||
        (hal_dev_fsm_get_curr_state() == HAL_DEVICE_INIT_STATE)) {
        hal_disable_machw_phy_and_pa();
        if (hal_is_hw_rx_queue_empty(&hal_device->hal_device_base) == OSAL_FALSE) {
            wlan_msg_h2d_destory_rx_dscr(&hal_device->hal_device_base);
        }
    }

    hmac_radar_sensor_config_related_reg(OSAL_FALSE);
#ifdef BOARD_FPGA_WIFI
    cali_recover_rx_iq_path(OSAL_NULL, WLAN_BAND_2G, CALI_BW_20M, 0);
    cali_offline_cali_finish(hal_device);
#endif
    oam_info_log0(0, OAM_SF_ANY, "hmac_radar_sensor_disable succ");
}

osal_s32 hmac_radar_sensor_start(hmac_radar_sensor_cfg_stru *radar_sensor_cfg)
{
    osal_u32 ret = OAL_FAIL;
    if (radar_sensor_cfg->enable == OSAL_TRUE) {
        ret = hmac_radar_sensor_enable(radar_sensor_cfg);
    } else {
        hmac_radar_sensor_disable();
        ret = OAL_SUCC;
    }

    return (osal_s32)ret;
}

OAL_STATIC osal_void hmac_radar_sensor_del_vap(osal_void)
{
    hmac_device_stru *hmac_device = hmac_res_get_mac_dev_etc(0);
    hmac_radar_sensor_info_stru *radar_sensor_info = hmac_radar_sensor_get_info();

    if (radar_sensor_info->probe_enable != OSAL_TRUE) {
        return;
    }

    if (hmac_device->vap_num == 1) {
        oam_warning_log0(0, OAM_SF_ANY, "hmac_radar_sensor_del_vap:del the last one vap, disable radar");
        hmac_radar_sensor_disable();
        report_radar_sensor_disable_reason(RADAR_SENSOR_LAST_VAP_DEL);
    }

    return;
}

OSAL_STATIC osal_s32 hmac_radar_sensor_complete_handle(hmac_vap_stru *hmac_vap, frw_msg *msg)
{
    hmac_radar_sensor_info_stru *radar_sensor_info = hmac_radar_sensor_get_info();
    hal_to_dmac_device_stru *hal_device = hal_chip_get_hal_device();

    unref_param(hmac_vap);
    unref_param(msg);

    hal_read_radar_sensing_exception();

    if (radar_sensor_info->probe_enable != OSAL_TRUE) {
        return OAL_FAIL;
    }

    if (radar_sensor_info->cycle_en != OSAL_TRUE && radar_sensor_info->period_cnt != 0 &&
        radar_sensor_info->period_cnt != 1) {
        radar_sensor_info->period_cnt--;
        if (radar_sensor_info->period_cnt == 0) {
            hmac_radar_sensor_disable();
        }
    }

    /* 消息上报使用同步消息或者函数直调,需要Wi-Fi应用返回结果后再处理 */
    osal_u32 *rx_mem = (osal_u32 *)hmac_radar_sensor_get_rx_mem_addr();
    if (g_is_cb_registered) {
        (void)g_radar_handle_cb(rx_mem, hal_device->wifi_channel_status.chan_number,
            hal_device->wifi_channel_status.en_bandwidth);
    }

    if (radar_sensor_info->cycle_en != OSAL_TRUE) { // 单次发送条件下, 由上层算法控制, 直接return
        return OAL_SUCC;
    }
    /* 单次处理完成后清空接收缓存 */
    hmac_radar_sensor_clear_rx_mem();

    hal_radar_complete_clear();

    return OAL_SUCC;
}

osal_void hmac_radar_sensor_one_subframe_start(osal_void)
{
    hmac_radar_sensor_info_stru *radar_sensor_info = hmac_radar_sensor_get_info();
    if (radar_sensor_info->probe_enable == OSAL_FALSE) {
        return;
    }
    hal_radar_sensing_one_sub_frame_start();
}

#ifdef _PRE_WLAN_SUPPORT_CCPRIV_CMD
OSAL_STATIC osal_s32 hmac_ccpriv_radar_sensor_enable(hmac_vap_stru *hmac_vap, const osal_s8 *param)
{
    osal_s32 ret;
    osal_s32 value;
    hmac_radar_sensor_cfg_stru radar_sensor_cfg;

    unref_param(hmac_vap);

    memset_s(&radar_sensor_cfg, sizeof(hmac_radar_sensor_cfg_stru), 0, sizeof(hmac_radar_sensor_cfg_stru));

    ret = hmac_ccpriv_get_u8_with_check_max(&param, 1, (osal_u8 *)&value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_enable::enable value invalid.}");
        return ret;
    }
    radar_sensor_cfg.enable = (osal_u8)value;

    if (radar_sensor_cfg.enable == OSAL_TRUE) {
        ret = hmac_ccpriv_get_u8_with_check_max(&param, DEVICE_SUPPORT_MAX_RX_NSS_NUM, (osal_u8 *)&value);
        if (ret != OAL_SUCC) {
            oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_enable:: rx_nss value invalid.}");
            return ret;
        }
        radar_sensor_cfg.rx_nss = (osal_u8)value;

        ret = hmac_ccpriv_get_digit_with_range(&param, MIN_RADAR_PERIOD, MAX_RADAR_PERIOD, &value);
        if (ret != OAL_SUCC) {
            oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_enable:: radar_period value invalid.}");
            return ret;
        }
        radar_sensor_cfg.radar_period = (osal_u16)value;

        ret = hmac_ccpriv_get_digit_with_range(&param, MIN_RADAR_SENSOR_PHY_CNT, MAX_RADAR_SENSOR_PHY_CNT, &value);
        if (ret != OAL_SUCC) {
            oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_enable:: one_per_cnt value invalid.}");
            return ret;
        }
        radar_sensor_cfg.one_per_cnt = (osal_u8)value;

        ret = hmac_ccpriv_get_u8_with_check_max(&param, MAX_RADAR_SENSOR_CYCLE_CNT, (osal_u8 *)&value);
        if (ret != OAL_SUCC) {
            oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_enable:: period_cycle_cnt value invalid.}");
            return ret;
        }
        radar_sensor_cfg.period_cycle_cnt = (osal_u8)value;
    }

    hmac_radar_sensor_start(&radar_sensor_cfg);

    return OAL_SUCC;
}

OSAL_STATIC osal_u32 hmac_radar_sensor_set_selfcts_data_rate(hmac_radar_sensor_debug_stru *debug_info,
    osal_u8 protocol_mode, osal_u8 mcs_rate)
{
    osal_u8 legacy_mcs_rate[] = {0xb, 0xf, 0xa, 0xe, 0x9, 0xd, 0x8, 0xc};
    osal_u8 mcs_rate_11b[] = {0x0, 0x1, 0x2, 0x3};
    hh503_tx_phy_rate_stru *data_rate = (hh503_tx_phy_rate_stru *)&debug_info->selfcts_rate;

    if (protocol_mode > 1) {
        return OAL_FAIL;
    }

    data_rate->bits.tx_vector_protocol_mode = protocol_mode;

    if (protocol_mode == 0) {
        if (mcs_rate >= osal_array_size(mcs_rate_11b)) {
            return OAL_FAIL;
        }
        data_rate->bits.tx_vector_mcs_rate = mcs_rate_11b[mcs_rate];
        data_rate->bits.tx_vector_preamble = 0x1; /* 11b协议模式配置前导 */
    }

    if (protocol_mode == 1) {
        if (mcs_rate >= osal_array_size(legacy_mcs_rate)) {
            return OAL_FAIL;
        }
        data_rate->bits.tx_vector_mcs_rate = legacy_mcs_rate[mcs_rate];
    }

    return OAL_SUCC;
}

OSAL_STATIC osal_s32 hmac_ccpriv_radar_sensor_debug(hmac_vap_stru *hmac_vap, const osal_s8 *param)
{
    osal_s32 ret;
    osal_u32 value;
    osal_u8 protocol_mode;
    osal_u8 mcs_rate;
    hmac_radar_sensor_debug_stru *debug_info = hmac_radar_sensor_get_debug_info();

    unref_param(hmac_vap);

    /* 0:11b 1:11g */
    ret = hmac_ccpriv_get_digit_with_check_max(&param, 1, &value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::protocol_mode value invalid.}");
        return ret;
    }
    protocol_mode = (osal_u8)value;
    /* 11g速率最高挡位为7档 */
    ret = hmac_ccpriv_get_digit_with_check_max(&param, 7, &value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::mcs value invalid.}");
        return ret;
    }
    mcs_rate = (osal_u8)value;

    ret = (osal_s32)hmac_radar_sensor_set_selfcts_data_rate(debug_info, protocol_mode, mcs_rate);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::set selfcts value invalid.}");
        return ret;
    }

    /* 200：TX 160M延迟最大值 */
    ret = hmac_ccpriv_get_digit_with_check_max(&param, 200, &value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::mcs value invalid.}");
        return ret;
    }
    debug_info->tx_dly_160m = (osal_u16)value;

    return OAL_SUCC;
}

OSAL_STATIC osal_s32 hmac_ccpriv_radar_sensor_set_tx_gain(hmac_vap_stru *hmac_vap, const osal_s8 *param)
{
    osal_s32 ret;
    osal_u32 value;
    hmac_radar_sensor_debug_stru *debug_info = hmac_radar_sensor_get_debug_info();

    unref_param(hmac_vap);
    ret = hmac_ccpriv_get_digit_with_check_max(&param, RADAR_SENSOR_MAX_LPF_GAIN, &value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::lpf_gain value invalid.}");
        return ret;
    }
    debug_info->lpf_gain = (osal_u8)value;

    ret = hmac_ccpriv_get_digit_with_check_max(&param, RADAR_SENSOR_MAX_DAC_GAIN, &value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::dac_gain value invalid.}");
        return ret;
    }
    debug_info->dac_gain = (osal_u8)value;

    ret = hmac_ccpriv_get_digit_with_check_max(&param, RADAR_SENSOR_MAX_PA_GAIN, &value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::pa_gain value invalid.}");
        return ret;
    }
    debug_info->pa_gain = (osal_u8)value;

    ret = hmac_ccpriv_get_digit_with_check_max(&param, RADAR_SENSOR_MAX_PPA_GAIN, &value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::ppa_gain value invalid.}");
        return ret;
    }
    debug_info->ppa_gain = (osal_u16)value;

    return OAL_SUCC;
}

OSAL_STATIC osal_s32 hmac_ccpriv_radar_sensor_set_rx_gain(hmac_vap_stru *hmac_vap, const osal_s8 *param)
{
    osal_s32 ret;
    osal_u32 value;
    hmac_radar_sensor_debug_stru *debug_info = hmac_radar_sensor_get_debug_info();

    unref_param(hmac_vap);
    ret = hmac_ccpriv_get_digit_with_check_max(&param, RADAR_SENSOR_MAX_LNA_GAIN, &value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::lna_gain value invalid.}");
        return ret;
    }
    debug_info->lna_gain = (osal_u8)value;

    ret = hmac_ccpriv_get_digit_with_check_max(&param, RADAR_SENSOR_MAX_VGA_GAIN, &value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::vga_gain value invalid.}");
        return ret;
    }
    debug_info->vga_gain = (osal_u8)value;

    ret = hmac_ccpriv_get_digit_with_check_max(&param, RADAR_SENSOR_RX_DELAY_160M, &value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::rx_dly_160m value invalid.}");
        return ret;
    }
    debug_info->rx_dly_160m = (osal_u8)value;

    ret = hmac_ccpriv_get_digit_with_check_max(&param, RADAR_SENSOR_RX_DELAY_80M, &value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::rx_dly_80m value invalid.}");
        return ret;
    }
    debug_info->rx_dly_80m = (osal_u8)value;

    return OAL_SUCC;
}

OSAL_STATIC osal_s32 hmac_ccpriv_radar_sensor_set_channel_num(hmac_vap_stru *hmac_vap, const osal_s8 *param)
{
    osal_s32 ret;
    osal_s8 value;
    hmac_radar_sensor_debug_stru *debug_info = hmac_radar_sensor_get_debug_info();

    unref_param(hmac_vap);
    ret = hmac_ccpriv_get_s8_with_range(&param, MAX_RADAR_WORK_MIN_CHAN, MAX_RADAR_WORK_MAX_CHAN, &value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_debug::ch_num value invalid.}");
        return ret;
    }
    debug_info->ch_num = (osal_u8)value;

    return OAL_SUCC;
}

OSAL_STATIC osal_s32 hmac_ccpriv_radar_sensor_set_mac_t1_4(hmac_vap_stru *hmac_vap, const osal_s8 *param)
{
    osal_s32 ret;
    osal_u32 value;
    hmac_radar_sensor_debug_stru *debug_info = hmac_radar_sensor_get_debug_info();

    unref_param(hmac_vap);
    ret = hmac_ccpriv_get_digit_with_check_max(&param, RADAR_SENSOR_MAX_T1, (osal_u32 *)&value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_set_mac_t1_4::t1 invalid.}");
        return ret;
    }
    debug_info->t1 = (osal_u16)value;

    ret = hmac_ccpriv_get_digit_with_check_max(&param, RADAR_SENSOR_MAX_T2, (osal_u32 *)&value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_set_mac_t1_4::t2 invalid.}");
        return ret;
    }
    debug_info->t2 = (osal_u16)value;

    ret = hmac_ccpriv_get_digit_with_check_max(&param, RADAR_SENSOR_MAX_T3, (osal_u32 *)&value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_set_mac_t1_4::t3 invalid.}");
        return ret;
    }
    debug_info->t3 = (osal_u16)value;

    ret = hmac_ccpriv_get_digit_with_check_max(&param, RADAR_SENSOR_MAX_T4, (osal_u32 *)&value);
    if (ret != OAL_SUCC) {
        oam_error_log0(0, OAM_SF_ANY, "{hmac_ccpriv_radar_sensor_set_mac_t1_4::t4 invalid.}");
        return ret;
    }
    debug_info->t4 = (osal_u16)value;

    return OAL_SUCC;
}
#endif

osal_u32 hmac_radar_sensor_init(osal_void)
{
    hmac_radar_sensor_info_stru *radar_sensor_info = hmac_radar_sensor_get_info();
    hmac_radar_sensor_debug_stru *debug_info = hmac_radar_sensor_get_debug_info();
    hh503_tx_phy_rate_stru *data_rate = (hh503_tx_phy_rate_stru *)&debug_info->selfcts_rate;

    (osal_void)memset_s(radar_sensor_info, sizeof(hmac_radar_sensor_info_stru), 0,
        sizeof(hmac_radar_sensor_info_stru));

    debug_info->tx_dly_160m = 0x10;
    debug_info->tx_dly_320m = 0x10;
    data_rate->bits.tx_vector_protocol_mode = 0x1; // 默认11g协议模式
    data_rate->bits.tx_vector_mcs_rate = 0xb; // 默认6M速率

    debug_info->lpf_gain = 0x2;
    debug_info->dac_gain = 0x1;
    debug_info->pa_gain = 0x7F;
    debug_info->ppa_gain = 0x1331;

    debug_info->lna_gain = 0x1;
    debug_info->vga_gain = 0x2;
    debug_info->rx_dly_160m = 0x28;
    debug_info->rx_dly_80m = 0x22;
    debug_info->ch_num = 0x6;

    debug_info->t1 = 0x64;
    debug_info->t2 = 0xE;
    debug_info->t3 = 0x0;
    debug_info->t4 = 0xA;

    /* 注册消息 */
    frw_msg_hook_register(WLAN_MSG_D2H_CRX_RADAR_SENSOR, hmac_radar_sensor_complete_handle);
    /* 对外接口注册 */
    hmac_feature_hook_register(HMAC_FHOOK_RADAR_SENSOR_GET_WORK_CHAN, hmac_radar_sensor_get_work_ch_num);
    hmac_feature_hook_register(HMAC_FHOOK_RADAR_SENSOR_DEL_VAP_NOTIFY, hmac_radar_sensor_del_vap);

#ifdef _PRE_WLAN_SUPPORT_CCPRIV_CMD
    /* ccpriv命令注册 */
    hmac_ccpriv_register((const osal_s8 *)"radar_sensor_enable",  hmac_ccpriv_radar_sensor_enable);
    hmac_ccpriv_register((const osal_s8 *)"radar_sensor_debug",  hmac_ccpriv_radar_sensor_debug);
    hmac_ccpriv_register((const osal_s8 *)"radar_sensor_tx_gain",  hmac_ccpriv_radar_sensor_set_tx_gain);
    hmac_ccpriv_register((const osal_s8 *)"radar_sensor_rx_gain",  hmac_ccpriv_radar_sensor_set_rx_gain);
    hmac_ccpriv_register((const osal_s8 *)"radar_sensor_set_channel",  hmac_ccpriv_radar_sensor_set_channel_num);
    hmac_ccpriv_register((const osal_s8 *)"radar_sensor_set_t1_4",  hmac_ccpriv_radar_sensor_set_mac_t1_4);
#endif
    return OAL_SUCC;
}

osal_void hmac_radar_sensor_deinit(osal_void)
{
    /* 去注册消息 */
    frw_msg_hook_unregister(WLAN_MSG_D2H_CRX_RADAR_SENSOR);
    /* 对外接口去注册 */
    hmac_feature_hook_unregister(HMAC_FHOOK_RADAR_SENSOR_GET_WORK_CHAN);
    hmac_feature_hook_unregister(HMAC_FHOOK_RADAR_SENSOR_DEL_VAP_NOTIFY);
#ifdef _PRE_WLAN_SUPPORT_CCPRIV_CMD
    /* ccpriv命令去注册 */
    hmac_ccpriv_unregister((const osal_s8 *)"radar_sensor_enable");
    hmac_ccpriv_unregister((const osal_s8 *)"radar_sensor_debug");
    hmac_ccpriv_unregister((const osal_s8 *)"radar_sensor_tx_gain");
    hmac_ccpriv_unregister((const osal_s8 *)"radar_sensor_rx_gain");
    hmac_ccpriv_unregister((const osal_s8 *)"radar_sensor_set_channel");
    hmac_ccpriv_unregister((const osal_s8 *)"radar_sensor_set_t1_4");
#endif
}

osal_u32 hmac_radar_sensor_register_handle_cb(radar_handle_cb cb)
{
    if (cb != NULL) {
        g_radar_handle_cb = cb;
        g_is_cb_registered = OSAL_TRUE;
        return OAL_SUCC;
    } else {
        return OAL_FAIL;
    }
}

#ifdef _PRE_RADAR_CCA_SW_OPT
osal_void hmac_radar_sensor_cca_sw_opt(osal_bool radar_switch)
{
    hmac_alg_cca_opt_radar_notify(radar_switch);
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
