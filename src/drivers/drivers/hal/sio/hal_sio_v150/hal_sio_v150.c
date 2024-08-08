/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides sio hal source \n
 *
 * History: \n
 * 2022-08-01, Create file. \n
 */
#include <stdint.h>
#include "securec.h"
#include "common_def.h"
#include "tcxo.h"
#include "soc_osal.h"
#include "sio_porting.h"
#include "hal_sio_v150.h"

#define SIO_FIFO_SIZE           16
#define SIO_INTMASK             0x3c

static hal_sio_rx_data_t g_sio_read_data[I2S_MAX_NUMBER] = { 0 };
static hal_sio_callback_t g_hal_sio_callbacks[I2S_MAX_NUMBER] = { 0 };
static hal_sio_config_t g_hal_sio_config[I2S_MAX_NUMBER] = { 0 };
static uint32_t g_i2s_tx_num = 0;
static uint32_t g_index_i2s = 0;
static uint32_t *g_write_left_data = NULL;
static uint32_t *g_write_right_data = NULL;

static errcode_t hal_sio_v150_init(sio_bus_t bus)
{
    if (hal_sio_regs_init(bus) != ERRCODE_SUCC) {
        return ERRCODE_SIO_REG_ADDR_INVALID;
    }
    g_i2s_tx_num = 0;
    g_index_i2s = 0;
    sio_porting_register_irq(bus);
    return ERRCODE_SUCC;
}

static void hal_sio_v150_deinit(sio_bus_t bus)
{
    hal_sio_regs_deinit(bus);
}

static void hal_sio_v150_set_intmask(sio_bus_t bus, uint32_t intmask)
{
    sios_v150_regs(bus)->intmask = intmask;
}

static void hal_sio_v150_clear_interrupt(sio_bus_t bus, uint32_t intclr)
{
    sios_v150_regs(bus)->intclr = intclr;
}

static uint32_t hal_sio_v150_get_intstatus(sio_bus_t bus)
{
    return sios_v150_regs(bus)->intstatus;
}

#pragma weak hal_sio_set_crg_clock_enable = hal_sio_v150_crg_clock_enable
void hal_sio_v150_crg_clock_enable(sio_bus_t bus, bool enable)
{
    unused(bus);
    unused(enable);
}

static void hal_sio_v150_extend_receive_config(sio_bus_t bus, hal_sio_channel_number_t channels_num)
{
    hal_sio_v150_mode_set_ext_rec_en(bus, 1);
    hal_sio_v150_mode_set_chn_num(bus, channels_num);
}

static void hal_sio_v150_data_width_set(sio_bus_t bus, hal_sio_data_width_t data_width)
{
    hal_sio_v150_data_width_set_set_tx(bus, data_width);
    hal_sio_v150_data_width_set_set_rx(bus, data_width);
}

static void hal_sio_v150_i2s_pos(sio_bus_t bus)
{
    sios_v150_regs(bus)->i2s_start_pos = 0;
    sios_v150_regs(bus)->i2s_pos_flag = 0;
}

static void hal_i2s_config(sio_bus_t bus, hal_sio_mode_t mode)
{
    hal_sio_v150_set_intmask(bus, SIO_INTMASK);
    hal_sio_v150_mode_set_mode(bus, mode);

    sio_porting_i2s_drive_mode(g_hal_sio_config[bus].drive_mode, g_hal_sio_config[bus].div_number,
                               g_hal_sio_config[bus].number_of_channels);
    sio_porting_i2s_gpio_mode(g_hal_sio_config[bus].drive_mode);
    hal_sio_v150_mode_set_clk_edge(bus, 1);

    if (g_hal_sio_config[bus].transfer_mode == MULTICHANNEL_MODE) {
        hal_sio_v150_extend_receive_config(bus, g_hal_sio_config[bus].channels_num);
    }

    hal_sio_v150_data_width_set(bus, g_hal_sio_config[bus].data_width);
    hal_sio_v150_i2s_pos(bus);
    hal_sio_v150_ct_set_set_rst_n(bus, 1);
    hal_sio_v150_ct_set_set_intr_en(bus, 1);
    hal_sio_v150_ct_set_set_rx_fifo_threshold(bus, CONFIG_I2S_RX_THRESHOLD);
    hal_sio_v150_ct_set_set_tx_fifo_threshold(bus, CONFIG_I2S_TX_THRESHOLD);
}

static void hal_pcm_config(sio_bus_t bus, hal_sio_mode_t mode)
{
    sio_porting_register_irq(bus);
    hal_sio_v150_mode_set_mode(bus, mode);
    sio_porting_pcm_drive_mode(g_hal_sio_config[bus].drive_mode, g_hal_sio_config[bus].div_number,
                               g_hal_sio_config[bus].number_of_channels);
    hal_sio_v150_mode_set_pcm_mode(bus, g_hal_sio_config[bus].timing);

    if (g_hal_sio_config[bus].transfer_mode == MULTICHANNEL_MODE) {
        hal_sio_v150_extend_receive_config(bus, g_hal_sio_config[bus].channels_num);
        hal_sio_v150_mode_set_clk_edge(bus, g_hal_sio_config[bus].clk_edge);
    }

    hal_sio_v150_data_width_set(bus, g_hal_sio_config[bus].data_width);
    hal_sio_v150_ct_set_set_rst_n(bus, 1);
    hal_sio_v150_ct_set_set_intr_en(bus, 1);
    hal_sio_v150_ct_set_set_rx_fifo_threshold(bus, CONFIG_PCM_RX_THRESHOLD);
    hal_sio_v150_ct_set_set_tx_fifo_threshold(bus, CONFIG_PCM_TX_THRESHOLD);
}

static void hal_sio_v150_set_config(sio_bus_t bus, const hal_sio_config_t *config)
{
    (void)memcpy_s(&g_hal_sio_config[bus], sizeof(hal_sio_config_t), config, sizeof(hal_sio_config_t));
}

static errcode_t hal_sio_v150_get_config(sio_bus_t bus, hal_sio_config_t *config)
{
    hal_sio_config_t *config_temp = (hal_sio_config_t *)config;
    if (config_temp == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    (void)memcpy_s(config_temp, sizeof(hal_sio_config_t), &g_hal_sio_config[bus], sizeof(hal_sio_config_t));

    return ERRCODE_SUCC;
}

static void hal_sio_v150_rx_enable(sio_bus_t bus, bool en)
{
    hal_i2s_config(bus, I2S_MODE);
    hal_sio_v150_ct_set_set_rx_enable(bus, (uint32_t)en);
}

static uint32_t hal_i2s_read_data(sio_bus_t bus, hal_sio_voice_channel_t voice_channe)
{
    uint32_t data_temp = 0;
    if (voice_channe == SIO_LEFT) {
        if (hal_sio_v150_rx_sta_get_rx_left_depth(bus) > 0) {
            data_temp = hal_sio_v150_left_rx_get_data(bus);
        }
    } else {
        if (hal_sio_v150_rx_sta_get_rx_right_depth(bus) > 0) {
            data_temp = hal_sio_v150_right_rx_get_data(bus);
        }
    }
    return data_temp;
}

static void hal_sio_write_data(sio_bus_t bus, uint32_t data, hal_sio_voice_channel_t voice_channe)
{
    uint32_t data_temp = data;
    if (voice_channe == SIO_LEFT) {
        if (hal_sio_v150_tx_sta_get_tx_left_depth(bus) < SIO_FIFO_SIZE) {
            hal_sio_v150_left_tx_set_data(bus, data_temp);
        }
    } else {
        if (hal_sio_v150_tx_sta_get_tx_right_depth(bus) < SIO_FIFO_SIZE) {
            hal_sio_v150_right_tx_set_data(bus, data_temp);
        }
    }
}

static void hal_i2s_write(sio_bus_t bus, hal_sio_tx_data_t *data, hal_sio_mode_t mode)
{
    hal_i2s_config(bus, mode);
    g_index_i2s = 0;
    g_i2s_tx_num = data->length;
    g_write_left_data = data->left_buff;
    g_write_right_data = data->right_buff;
    hal_sio_v150_ct_set_set_tx_enable(bus, 1);
    sio_porting_bclk_clock_enable(true);
}

static void hal_pcm_write(sio_bus_t bus, hal_sio_tx_data_t *data, hal_sio_mode_t mode)
{
    hal_pcm_config(bus, mode);
    uint32_t *temp_tx_right_data = data->right_buff;
    uint32_t trans_time = data->length;
    hal_sio_v150_ct_set_set_tx_enable(bus, true);
    for (uint32_t i = 0; i < trans_time; i++) {
        hal_sio_write_data(bus, temp_tx_right_data[i], SIO_RIGHT);
    }
    hal_sio_v150_ct_set_set_tx_enable(bus, false);
}

static hal_sio_write_t g_sio_write_func_t[NONE_SIO_MODE] = {
    hal_i2s_write,
    hal_pcm_write,
};

static void hal_sio_v150_write(sio_bus_t bus, hal_sio_tx_data_t *data, hal_sio_mode_t mode)
{
    g_sio_write_func_t[mode](bus, data, mode);
}

static void hal_sio_v150_register(sio_bus_t bus, hal_sio_callback_t callback)
{
    g_hal_sio_callbacks[bus] = callback;
}

static void hal_sio_v150_unregister(sio_bus_t bus)
{
    g_hal_sio_callbacks[bus] = NULL;
}

static void hal_sio_v150_loop(sio_bus_t bus, bool en)
{
    hal_sio_v150_version_set_loop(bus, (uint32_t)en);
}

static void hal_sio_v150_get_data(sio_bus_t bus, hal_sio_rx_data_t *data)
{
    *data = g_sio_read_data[bus];
}

static void hal_sio_v150_loop_trans(sio_bus_t bus, hal_sio_tx_data_t *data, hal_sio_mode_t mode)
{
    hal_i2s_config(bus, mode);
    g_index_i2s = 0;
    g_i2s_tx_num = data->length;
    g_write_left_data = data->left_buff;
    g_write_right_data = data->right_buff;

    hal_sio_v150_ct_set_set_rx_enable(bus, 1);
    hal_sio_v150_ct_set_set_tx_enable(bus, 1);
    sio_porting_bclk_clock_enable(true);
}

void hal_sio_v150_irq_handler(sio_bus_t bus)
{
    uint32_t int_status;
    int_status = hal_sio_v150_get_intstatus(bus);
    if ((bit(0) & int_status) != 0) {
        uint32_t trans_frames = hal_sio_v150_rx_sta_get_rx_right_depth(bus);
        while (trans_frames-- > 0) {
            g_sio_read_data[bus].left_buff[g_sio_read_data[bus].length] = hal_i2s_read_data(bus, SIO_LEFT);
            g_sio_read_data[bus].right_buff[g_sio_read_data[bus].length] = hal_i2s_read_data(bus, SIO_RIGHT);
            g_sio_read_data[bus].length++;
        }
        if (g_hal_sio_callbacks[bus]) {
            g_hal_sio_callbacks[bus](g_sio_read_data[bus].left_buff, g_sio_read_data[bus].right_buff,
                                     g_sio_read_data[bus].length);
            (void)memset_s(((void *)&g_sio_read_data), sizeof(hal_sio_rx_data_t), 0,
                           sizeof(hal_sio_rx_data_t));
            g_sio_read_data[bus].length = 0;
        }
        hal_sio_v150_clear_interrupt(bus, bit(0));
    }
    if ((bit(1) & int_status) != 0) {
        uint32_t len = 16 - hal_sio_v150_tx_sta_get_tx_right_depth(bus);
        if (g_index_i2s >= g_i2s_tx_num) {
            hal_sio_v150_clear_interrupt(bus, bit(1));
            hal_sio_v150_ct_clr_set_tx_enable(bus, 1);
            sio_porting_bclk_clock_enable(false);
            return;
        }
        while (len > 0) {
            hal_sio_write_data(bus, g_write_left_data[g_index_i2s], SIO_LEFT);
            hal_sio_write_data(bus, g_write_right_data[g_index_i2s], SIO_RIGHT);
            g_index_i2s++;
            len--;
        }
        hal_sio_v150_clear_interrupt(bus, bit(1));
    }
}

#if defined(CONFIG_I2S_SUPPORT_DMA)
#define SIO_INTMASK_TX_DISABLED     0x32
#define SIO_INTMASK_RX_DISABLED     0xd
static void hal_sio_v150_dma_cfg(sio_bus_t bus, const uintptr_t attr)
{
    hal_i2s_dma_attr_t *dma_attr = (hal_i2s_dma_attr_t *)attr;
    hal_i2s_config(bus, I2S_MODE);
    hal_sio_v150_pos_merge_set_en(bus, 1);
    if (dma_attr->tx_dma_enable) {
        hal_sio_v150_ct_set_set_tx_fifo_threshold(bus, dma_attr->tx_int_threshold);
        uint32_t mask = sios_v150_regs(bus)->intmask;
        mask = (mask | SIO_INTMASK_TX_DISABLED);
        hal_sio_v150_set_intmask(bus, mask);
    }
    if (dma_attr->rx_dma_enable) {
        hal_sio_v150_ct_set_set_rx_fifo_threshold(bus, dma_attr->rx_int_threshold);
        uint32_t mask = sios_v150_regs(bus)->intmask;
        mask = (mask | SIO_INTMASK_RX_DISABLED);
        hal_sio_v150_set_intmask(bus, mask);
    }
}
#endif

static hal_sio_funcs_t g_hal_sio_v150_funcs = {
    .init = hal_sio_v150_init,
    .deinit = hal_sio_v150_deinit,
    .set_config = hal_sio_v150_set_config,
    .get_config = hal_sio_v150_get_config,
    .rx_enable = hal_sio_v150_rx_enable,
    .write = hal_sio_v150_write,
    .get_data = hal_sio_v150_get_data,
    .registerfunc = hal_sio_v150_register,
    .unregisterfunc = hal_sio_v150_unregister,
    .loop = hal_sio_v150_loop,
    .loop_trans = hal_sio_v150_loop_trans,
#if defined(CONFIG_I2S_SUPPORT_DMA)
    .dma_cfg = hal_sio_v150_dma_cfg,
#endif
};

hal_sio_funcs_t *hal_sio_v150_funcs_get(void)
{
    return &g_hal_sio_v150_funcs;
}