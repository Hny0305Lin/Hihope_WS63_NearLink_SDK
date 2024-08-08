/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides V154 HAL adc \n
 *
 * History: \n
 * 2024-02-01， Create file. \n
 */
#include "common_def.h"
#include "tcxo.h"
#include "hal_adc.h"
#include "debug_print.h"
#include "osal_debug.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "gpio.h"
#include "adc_porting.h"
#include "hal_adc_v154.h"

#define DELAY_500US 500
#define DELAY_300US 300
#define FIFO_DATA_LENS 128
#define MANUAL_CALI_LOW_LIMIT1 0
#define MANUAL_CALI_UPPER_LIMIT1 2
#define MANUAL_CALI_LOW_LIMIT2 3
#define MANUAL_CALI_UPPER_LIMIT2 31
#define MANUAL_CALI_LOW_LIMIT3 32
#define MANUAL_CALI_UPPER_LIMIT3 60
#define MANUAL_CALI_LOW_LIMIT4 61
#define MANUAL_CALI_UPPER_LIMIT4 63
#define MANUAL_CALI_GAIN 3
#define MANUAL_CALI_THRESHOLD 63
#define COUNT_THRESHOLD  1000
#define VOLTAGE_UPPER_LIMIT 3600
#define CH_NUM 6

static hal_adc_callback_t g_adc_receive_data_callback = NULL;

uint32_t g_fifo_data[FIFO_DATA_LENS];
uint32_t g_ch_output_data[CH_NUM][FIFO_DATA_LENS];
uint32_t g_ch_data_cnt[CH_NUM];

static bool g_adc_auto_scan_status = false;

static errcode_t hal_adc_v154_init(void)
{
    if (hal_adc_v154_regs_init() != ERRCODE_SUCC) {
        return ERRCODE_ADC_REG_ADDR_INVALID;
    }
    return ERRCODE_SUCC;
}

static errcode_t hal_adc_v154_deinit(void)
{
    return ERRCODE_SUCC;
}

static void hal_adc_simulation_cfg(void)
{
    hal_adc_simu_cfg1();
    hal_adc_simu_cfg2();
    hal_adc_simu_cfg3();
    hal_adc_simu_ldo_start();
    uapi_tcxo_delay_us(DELAY_500US);
    hal_adc_simu_cfg4();
    uapi_tcxo_delay_us(DELAY_300US);
    hal_adc_simu_cfg5();
    hal_adc_simu_cfg6();
    uapi_tcxo_delay_us(DELAY_300US);
    hal_adc_simu_rst_clr();
}

static void hal_adc_offset_auto_cali(void)
{
    hal_adc_offset_cali_state_clr();
    hal_adc_offset_cali_data_spi_refresh();
    hal_adc_offset_cali_set();
    hal_adc_offset_cali_enable(1);
    uint32_t cnt = 0;
    while (hal_adc_offset_cali_sts() != 1) {
        if (cnt > COUNT_THRESHOLD) {
            break;
        }
        cnt++;
    }
    hal_adc_offset_cali_enable(0);
}

static void hal_adc_offset_manual_cali(void)
{
    uint32_t auto_cali_data;
    uint32_t temp;
    auto_cali_data = hal_adc_auto_offset_cali_data_get();
    if (auto_cali_data >= MANUAL_CALI_LOW_LIMIT1 && auto_cali_data <= MANUAL_CALI_UPPER_LIMIT1) {
        temp = 0;
    } else if (auto_cali_data >= MANUAL_CALI_LOW_LIMIT2 && auto_cali_data <= MANUAL_CALI_UPPER_LIMIT2) {
        temp = auto_cali_data - MANUAL_CALI_GAIN;
    } else if (auto_cali_data >= MANUAL_CALI_LOW_LIMIT3 && auto_cali_data <= MANUAL_CALI_UPPER_LIMIT3) {
        temp = auto_cali_data + MANUAL_CALI_GAIN;
    } else if (auto_cali_data >= MANUAL_CALI_LOW_LIMIT4 && auto_cali_data <= MANUAL_CALI_UPPER_LIMIT4) {
        temp = MANUAL_CALI_THRESHOLD;
    }
    hal_adc_manual_offset_cali_set(temp);
    hal_adc_offset_cali_enable(1);
    hal_adc_cfg_offset_cali_data_enable();
    hal_adc_offset_cali_data_spi_refresh();
    uint32_t cnt = 0;
    while (hal_adc_offset_cali_sts() != 1) {
        if (cnt > COUNT_THRESHOLD) {
            break;
        }
        cnt++;
    }
    hal_adc_offset_cali_enable(0);
}

static void hal_adc_cap_auto_cali(void)
{
    hal_adc_cfg_cap_cali_finish_clr();
    hal_adc_cfg_intr_gain_state_clr();
    hal_adc_date_spi_refresh_clr();
    hal_adc_cfg_cap_cali_set();
    hal_adc_cfg_cap_cali_enable(1);
    uint32_t cnt = 0;
    while (hal_adc_rpt_cap_cali_sts() != 1) {
        if (cnt > COUNT_THRESHOLD) {
            break;
        }
        cnt++;
    }
    hal_adc_cfg_cap_cali_enable(0);
}

static void hal_adc_offset_manual_cali_inc(void)
{
    uint32_t temp;
    temp = 0;
    hal_adc_manual_offset_cali_set(temp);
    hal_adc_offset_cali_enable(1);
    hal_adc_cfg_offset_cali_data_enable();
    hal_adc_offset_cali_data_spi_refresh();
    uint32_t cnt = 0;
    while (hal_adc_offset_cali_sts() != 1) {
        if (cnt > COUNT_THRESHOLD) {
            break;
        }
        cnt++;
    }
    hal_adc_offset_cali_enable(0);
}

void adc_port_calibration(void)
{
    hal_adc_offset_auto_cali();
    hal_adc_offset_manual_cali();
    hal_adc_cap_auto_cali();
    hal_adc_offset_manual_cali_inc();
}

static void adc_process_before_use_adc(void)
{
    hal_adc_cfg_gain_cali_enable();
    uint32_t cnt = 0;
    while (hal_adc_gain_unit_get() != 0) {
        if (cnt > COUNT_THRESHOLD) {
            break;
        }
        cnt++;
    }
    cnt = 0;
    while (hal_adc_reg_fifo_isnt_empty_get() == 1) {
        (void)hal_adc_reg_fifo_data_get();
        if (cnt > COUNT_THRESHOLD) {
            break;
        }
        cnt++;
    }
}

static void hal_adc_v154_power_en(afe_scan_mode_t afe_scan_mode, bool on)
{
    unused(afe_scan_mode);
    if (on) {
        hal_adc_soft_rst(0);
        hal_adc_soft_rst(1);
        hal_adc_simulation_cfg();
        adc_port_calibration();
        adc_process_before_use_adc();
        hal_adc_fifo_waterline_set();
        hal_adc_reg_irg_fifo_interrupt_mask();
    }
}

static pin_t hal_adc_channel_trans_to_gpio_get(adc_channel_t ch)
{
    switch (ch) {
        case ADC_CHANNEL_0:
            return GPIO_07;
        case ADC_CHANNEL_1:
            return GPIO_08;
        case ADC_CHANNEL_2:
            return GPIO_09;
        case ADC_CHANNEL_3:
            return GPIO_10;
        case ADC_CHANNEL_4:
            return GPIO_11;
        case ADC_CHANNEL_5:
            return GPIO_12;
        default:
            return PIN_NONE;
    }
}

errcode_t hal_adc_v154_channel_set(adc_channel_t ch, bool on)
{
    if (ch >= ADC_CHANNEL_MAX_NUM) {
        return ERRCODE_ADC_INVALID_CH_TYPE;
    }
    pin_t pin_number;
    pin_number = hal_adc_channel_trans_to_gpio_get(ch);
    if (on) {
        errcode_t ret;
        ret = uapi_pin_set_pull(pin_number, PIN_PULL_TYPE_DISABLE);
        if (ret != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
        ret = uapi_pin_set_mode(pin_number, PIN_MODE_0);
        if (ret != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
        ret = uapi_gpio_set_dir(pin_number, GPIO_DIRECTION_INPUT);
        if (ret != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
    }
    
    return ERRCODE_SUCC;
}

static void hal_adc_fifo_data_print(void)
{
    adc_fifo_data_str_t ret;
    uint32_t code, ch, index, voltage;
    uint32_t param2 = 0x1 << 14;
    uint32_t param3 = 0x1 << 15;

    uint32_t data_s = 1, data_b = 1, data_k = 1;

    bool next = false;

    errcode_t res = adc_port_get_cali_param((uint8_t *)&data_s, (uint8_t *)&data_b, (uint8_t *)&data_k);
    if (res != ERRCODE_SUCC) {
        print_str("efuse read failed!\r\n");
    }

    memset_s(g_ch_data_cnt, sizeof(uint32_t) * CH_NUM, 0, sizeof(uint32_t) * CH_NUM);
    
    for (uint32_t i = 0; i < FIFO_DATA_LENS; i++) {
        ret.d32 = g_fifo_data[i];
        code = ret.b.data;
        ch = ret.b.channel;
        g_ch_data_cnt[ch]++;
        index = g_ch_data_cnt[ch];
        if (ret.b.data == 0) {
            break;
        }

        if (data_k == 0) {
            voltage = code * VOLTAGE_UPPER_LIMIT / param2;
        } else {
            if (data_s == 0) {
                voltage = code * VOLTAGE_UPPER_LIMIT / data_k - data_b * VOLTAGE_UPPER_LIMIT / param3;
            } else {
                voltage = code * VOLTAGE_UPPER_LIMIT / data_k + data_b * VOLTAGE_UPPER_LIMIT / param3;
            }
        }
        g_ch_output_data[ch][index] = voltage;
    }
    for (uint32_t idx = 0; idx < CH_NUM; idx++) {
        if (g_ch_data_cnt[idx] > 0 && g_adc_receive_data_callback != NULL) {
            g_adc_receive_data_callback(idx, g_ch_output_data[idx], g_ch_data_cnt[idx], &next);
        }
    }
}

static void hal_adc_v154_auto_scan_enable(bool en)
{
    if (en) {
        hal_adc_start_sample();
        g_adc_auto_scan_status = true;
    } else {
        hal_adc_stop_sample();
        g_adc_auto_scan_status = false;
    }
}

static errcode_t hal_adc_v154_auto_scan_ch_enable(adc_channel_t ch, bool en)
{
    hal_adc_auto_scan_mode_set(ch, en);
    if (!en) {
        hal_adc_fifo_data_print();
        hal_adc_v154_auto_scan_enable(false);
    }
    return ERRCODE_SUCC;
}

static errcode_t hal_adc_v154_auto_scan_ch_config(adc_channel_t ch, hal_adc_scan_config_t *config,
                                                  hal_adc_callback_t callback)
{
    unused(config);
    g_adc_receive_data_callback = callback;
    hal_adc_v154_channel_set(ch, true);
    hal_adc_v154_auto_scan_ch_enable(ch, true);
    hal_adc_v154_auto_scan_enable(true);

    return ERRCODE_SUCC;
}

static bool hal_adc_v154_auto_scan_is_enabled(void)
{
    return g_adc_auto_scan_status;
}

void hal_adc_irq_handler(void)
{
    uint32_t index = 0;
    uint32_t cnt = 0;
    while (hal_adc_reg_fifo_isnt_empty_get()) {
        if (index < FIFO_DATA_LENS) {
            g_fifo_data[index++] = hal_adc_reg_fifo_data_get();
        } else {
            hal_adc_reg_fifo_data_get();
            if (cnt > COUNT_THRESHOLD) {
                break;
            }
            cnt++;
        }
    }
}

static int32_t hal_adc_v154_manual_sample(adc_channel_t channel)
{
    unused(channel);
    return 0;
}

static hal_adc_funcs_t g_hal_adc_v154_funcs = {
    .init = hal_adc_v154_init,
    .deinit = hal_adc_v154_deinit,
    .power_en = hal_adc_v154_power_en,
    .ch_set = hal_adc_v154_channel_set,
#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    .ch_config = hal_adc_v154_auto_scan_ch_config,
    .ch_enable = hal_adc_v154_auto_scan_ch_enable,
    .enable = hal_adc_v154_auto_scan_enable,
    .isenable = hal_adc_v154_auto_scan_is_enabled,
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */
    .manual = hal_adc_v154_manual_sample
};

hal_adc_funcs_t *hal_adc_v154_funcs_get(void)
{
    return &g_hal_adc_v154_funcs;
}
