/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides v151 hal sio \n
 *
 * History: \n
 * 2023-03-07, Create file. \n
 */
#include <stdint.h>
#include "securec.h"
#include "soc_osal.h"
#include "tcxo.h"
#include "sio_porting.h"
#include "hal_sio_v151.h"

#define SIO_LOOP_INTMASK                0x3c
#define SIO_WRITE_INTMASK               0x3d
#define SIO_READ_INTMASK                0x3e
#define SIO_FIFO_SIZE                   8
#define SIO_RX_THRESHOLD                7
#define SIO_TX_THRESHOLD                4
#define SIO_FIRST_TRANSFER_LEN          8
#define SIO_FIRST_DELAY_US              25
#define SIO_LAST_DELAY_US               295
#define SIO_ONCE_TRANSFER_LEN           (SIO_FIFO_SIZE - SIO_TX_THRESHOLD)

typedef struct hal_i2s_trans_inf {
    bool trans_succ;
    osal_semaphore trans_sem;
} hal_i2s_trans_inf_t;

static hal_i2s_trans_inf_t g_tx_trans[I2S_MAX_NUMBER] = { 0 };
static hal_sio_rx_data_t g_sio_read_data[I2S_MAX_NUMBER] = { 0 };
static hal_sio_callback_t g_hal_sio_callbacks[I2S_MAX_NUMBER] = { 0 };
static hal_sio_config_t g_hal_sio_config[I2S_MAX_NUMBER] = { 0 };
static uint32_t g_i2s_tx_length = 0;
static uint32_t g_i2s_index = 0;
static uint32_t *g_write_left_data = NULL;
static uint32_t *g_write_right_data = NULL;

static errcode_t hal_sio_v151_init(sio_bus_t bus)
{
    if (hal_sio_regs_init(bus) != ERRCODE_SUCC) {
        return ERRCODE_SIO_REG_ADDR_INVALID;
    }
    g_i2s_tx_length = 0;
    g_i2s_index = 0;
    sio_porting_register_irq(bus);
    osal_sem_init(&(g_tx_trans[bus].trans_sem), 0);
    return ERRCODE_SUCC;
}

static void hal_sio_v151_deinit(sio_bus_t bus)
{
    hal_sio_regs_deinit(bus);
    osal_sem_destroy(&(g_tx_trans[bus].trans_sem));
}

static void hal_sio_v151_set_intmask(sio_bus_t bus, uint32_t intmask)
{
    sios_v151_regs(bus)->intmask = intmask;
}

static void hal_sio_v151_clear_interrupt(sio_bus_t bus, uint32_t intclr)
{
    sios_v151_regs(bus)->intclr = intclr;
}

static uint32_t hal_sio_v151_get_intstatus(sio_bus_t bus)
{
    return sios_v151_regs(bus)->intstatus;
}

#pragma weak hal_sio_set_crg_clock_enable = hal_sio_v151_crg_clock_enable
void hal_sio_v151_crg_clock_enable(sio_bus_t bus, bool enable)
{
    hal_sio_v151_i2s_crg_set_crg_clken(bus, (uint32_t)enable);
}

#pragma weak hal_sio_set_tx_enable = hal_sio_v151_set_tx_enable
void hal_sio_v151_set_tx_enable(sio_bus_t bus, uint32_t val)
{
    hal_sio_v151_ct_set_set_tx_enable(bus, val);
}

#pragma weak hal_sio_set_rx_enable = hal_sio_v151_set_rx_enable
void hal_sio_v151_set_rx_enable(sio_bus_t bus, uint32_t val)
{
    hal_sio_v151_ct_set_set_rx_enable(bus, val);
}

static void hal_sio_v151_extend_receive_config(sio_bus_t bus, hal_sio_channel_number_t channel)
{
    hal_sio_v151_mode_set_ext_rec_en(bus, 1);
    hal_sio_v151_mode_set_chn_num(bus, channel);
}

static void hal_sio_v151_data_width_set(sio_bus_t bus, hal_sio_data_width_t data_width)
{
    hal_sio_v151_data_width_set_tx_mode(bus, data_width);
    hal_sio_v151_data_width_set_rx_mode(bus, data_width);
}

void hal_sio_v151_master_mode_sel(sio_bus_t bus, bool enable)
{
    if (enable) {
        hal_sio_v151_mode_set_cfg_i2s_ms_mode_sel(bus, 1);
    } else {
        hal_sio_v151_mode_set_cfg_i2s_ms_mode_sel(bus, 0);
    }
}

static void hal_sio_v151_i2s_pos(sio_bus_t bus)
{
    sios_v151_regs(bus)->i2s_start_pos = 0;
    sios_v151_regs(bus)->i2s_pos_flag = 0;
}
static void hal_sio_v151_i2s_clk_sel(sio_bus_t bus)
{
    hal_sio_v151_i2s_crg_set_bclk_sel(bus, 0);
    hal_sio_v151_i2s_crg_set_fs_sel(bus, 0);
}

static void hal_sio_v151_i2s_set_fs(sio_bus_t bus, uint8_t data_width, uint32_t number_of_channels)
{
    hal_sio_v151_crg_clock_enable(bus, false);
    hal_sio_v151_fs_div_num_set_num(bus, (data_width * number_of_channels));
    hal_sio_v151_fs_div_ratio_num_set_num(bus, (data_width * number_of_channels) / I2S_DUTY_CYCLE);
}

void hal_i2s_set_bclk(sio_bus_t bus, uint8_t data_width, uint32_t ch)
{
    uint32_t bclk_div_num;
    bclk_div_num = sio_porting_get_bclk_div_num(data_width, ch);
    hal_sio_v151_crg_clock_enable(bus, false);
    hal_sio_v151_i2s_crg_set_bclk_div_en(bus, 0);
    hal_sio_v151_bclk_div_num_set_num(bus, bclk_div_num);
    hal_sio_v151_i2s_crg_set_bclk_div_en(bus, 1);
}

void hal_sio_v151_i2s_drive_mode(sio_bus_t bus, uint8_t drive_mode, uint8_t data_width, uint8_t number_of_channels)
{
    hal_sio_v151_i2s_clk_sel(bus);
    if (drive_mode == 0) {
        hal_sio_v151_master_mode_sel(bus, false);
        return;
    }
    hal_sio_v151_master_mode_sel(bus, true);
    hal_sio_v151_i2s_set_fs(bus, data_width, number_of_channels);
    hal_i2s_set_bclk(bus, data_width, number_of_channels);
}

static void hal_i2s_config(sio_bus_t bus, hal_sio_mode_t mode)
{
    hal_sio_v151_set_intmask(bus, SIO_LOOP_INTMASK);
    hal_sio_v151_mode_set_mode(bus, mode);
    hal_sio_v151_i2s_drive_mode(bus, g_hal_sio_config[bus].drive_mode, g_hal_sio_config[bus].div_number,
                               g_hal_sio_config[bus].number_of_channels);
    hal_sio_v151_mode_set_clk_edge(bus, 1);

    if (g_hal_sio_config[bus].transfer_mode == MULTICHANNEL_MODE) {
        hal_sio_v151_extend_receive_config(bus, g_hal_sio_config[bus].channels_num);
    }

    hal_sio_v151_data_width_set(bus, g_hal_sio_config[bus].data_width);
    hal_sio_v151_i2s_pos(bus);
    hal_sio_v151_ct_clr_set_rst_n(bus, 1);
    hal_sio_v151_ct_set_set_rst_n(bus, 1);
    hal_sio_v151_ct_set_set_intr_en(bus, 1);
    hal_sio_v151_fifo_threshold_set_rx_fifo_threshold(bus, SIO_RX_THRESHOLD);
    hal_sio_v151_fifo_threshold_set_tx_fifo_threshold(bus, SIO_TX_THRESHOLD);
}

static void hal_sio_v151_set_config(sio_bus_t bus, const hal_sio_config_t *config)
{
    (void)memcpy_s(&g_hal_sio_config[bus], sizeof(hal_sio_config_t), config, sizeof(hal_sio_config_t));
}

static errcode_t hal_sio_v151_get_config(sio_bus_t bus, hal_sio_config_t *config)
{
    hal_sio_config_t *config_temp = (hal_sio_config_t *)config;
    if (config_temp == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    (void)memcpy_s(config_temp, sizeof(hal_sio_config_t), &g_hal_sio_config[bus], sizeof(hal_sio_config_t));

    return ERRCODE_SUCC;
}

static void hal_sio_v151_rx_enable(sio_bus_t bus, bool en)
{
    hal_i2s_config(bus, I2S_MODE);
    hal_sio_v151_set_intmask(bus, SIO_READ_INTMASK);
    hal_sio_v151_ct_set_set_rx_enable(bus, (uint32_t)en);
}

static uint32_t hal_sio_read_data(sio_bus_t bus, hal_sio_voice_channel_t voice_channe)
{
    uint32_t data_temp = 0;
    if (voice_channe == SIO_LEFT) {
        data_temp = hal_sio_v151_i2s_left_rd_get_rx_left_data(bus);
    } else {
        data_temp = hal_sio_v151_i2s_right_rd_get_rx_right_data(bus);
    }
    return data_temp;
}

static void hal_sio_write_data(sio_bus_t bus, uint32_t data, hal_sio_voice_channel_t voice_channe)
{
    uint32_t data_temp = data;
    if (voice_channe == SIO_LEFT) {
        hal_sio_v151_i2s_left_xd_set_tx_left_data(bus, data_temp);
    } else {
        hal_sio_v151_i2s_right_xd_set_tx_right_data(bus, data_temp);
    }
}

static void hal_i2s_write(sio_bus_t bus, hal_sio_tx_data_t *data, hal_sio_mode_t mode)
{
    hal_i2s_config(bus, mode);
    hal_sio_v151_set_intmask(bus, SIO_WRITE_INTMASK);
    g_i2s_tx_length = data->length;
    g_write_left_data = data->left_buff;
    g_write_right_data = data->right_buff;

    if (g_i2s_tx_length < SIO_FIRST_TRANSFER_LEN) {
        for (uint32_t i = 0; i < g_i2s_tx_length; i++) {
            hal_sio_write_data(bus, g_write_left_data[i], SIO_LEFT);
            hal_sio_write_data(bus, g_write_right_data[i], SIO_RIGHT);
        }
        g_i2s_index = g_i2s_tx_length;
    } else {
        for (uint32_t i = 0; i < SIO_FIRST_TRANSFER_LEN; i++) {
            hal_sio_write_data(bus, g_write_left_data[i], SIO_LEFT);
            hal_sio_write_data(bus, g_write_right_data[i], SIO_RIGHT);
        }
        g_i2s_index += SIO_FIRST_TRANSFER_LEN;
    }

    hal_sio_v151_crg_clock_enable(bus, true);
    uapi_tcxo_delay_us(SIO_FIRST_DELAY_US);
    hal_sio_v151_ct_set_set_tx_enable(bus, 1);

    if (osal_sem_down(&(g_tx_trans[bus].trans_sem)) != OSAL_SUCCESS) {
        return;
    }
    if (!g_tx_trans[bus].trans_succ) {
        return;
    }
    uapi_tcxo_delay_us(SIO_LAST_DELAY_US);
    hal_sio_v151_ct_clr_set_tx_enable(bus, 1);
    hal_sio_v151_crg_clock_enable(bus, false);
    g_tx_trans[bus].trans_succ = false;
}

static hal_sio_write_t g_sio_write_func[NONE_SIO_MODE] = {
    hal_i2s_write,
};

static void hal_sio_v151_write(sio_bus_t bus, hal_sio_tx_data_t *data, hal_sio_mode_t mode)
{
    g_sio_write_func[mode](bus, data, mode);
}

static void hal_sio_v151_register(sio_bus_t bus, hal_sio_callback_t callback)
{
    g_hal_sio_callbacks[bus] = callback;
}

static void hal_sio_v151_unregister(sio_bus_t bus)
{
    g_hal_sio_callbacks[bus] = NULL;
}

static void hal_sio_v151_get_data(sio_bus_t bus, hal_sio_rx_data_t *data)
{
    *data = g_sio_read_data[bus];
}

void hal_sio_v151_irq_handler(sio_bus_t bus)
{
    uint32_t int_status;
    int_status = hal_sio_v151_get_intstatus(bus);
    if ((bit(0) & int_status) != 0) {
        uint32_t trans_frames = hal_sio_v151_rx_sta_get_rx_right_depth(bus);
        while (trans_frames-- > 0) {
            g_sio_read_data[bus].left_buff[g_sio_read_data[bus].length] = hal_sio_read_data(bus, SIO_LEFT);
            g_sio_read_data[bus].right_buff[g_sio_read_data[bus].length] = hal_sio_read_data(bus, SIO_RIGHT);
            g_sio_read_data[bus].length++;
        }
        if (g_hal_sio_callbacks[bus]) {
            g_hal_sio_callbacks[bus](g_sio_read_data[bus].left_buff, g_sio_read_data[bus].right_buff,
                                     g_sio_read_data[bus].length);
            (void)memset_s(((void *)&g_sio_read_data), sizeof(hal_sio_rx_data_t), 0,
                           sizeof(hal_sio_rx_data_t));
            g_sio_read_data[bus].length = 0;
        }
        hal_sio_v151_clear_interrupt(bus, bit(0));
    }

    if ((bit(1) & int_status) != 0) {
        if (g_i2s_index == 0) {
            hal_sio_v151_clear_interrupt(bus, bit(1));
        } else if (g_i2s_index < g_i2s_tx_length) {
            uint32_t write_len = SIO_ONCE_TRANSFER_LEN;
            if ((g_i2s_tx_length - g_i2s_index) < SIO_ONCE_TRANSFER_LEN) {
                write_len = g_i2s_tx_length - g_i2s_index;
            }
            for (uint32_t i = 0; i < write_len; i++) {
                hal_sio_write_data(bus, g_write_left_data[g_i2s_index + i], SIO_LEFT);
                hal_sio_write_data(bus, g_write_right_data[g_i2s_index + i], SIO_RIGHT);
            }
            g_i2s_index += write_len;
            hal_sio_v151_clear_interrupt(bus, bit(1));
        } else {
            hal_sio_v151_clear_interrupt(bus, bit(1));
            hal_sio_v151_set_intmask(bus, SIO_READ_INTMASK);
            g_i2s_index = 0;
            g_tx_trans[bus].trans_succ = true;
            osal_sem_up(&(g_tx_trans[bus].trans_sem));
        }
    }
}

#if defined(CONFIG_I2S_SUPPORT_LOOPBACK)
static void hal_sio_v151_loop(sio_bus_t bus, bool en)
{
    hal_sio_v151_version_set_loop(bus, (uint32_t)en);
}

void hal_sio_v151_loop_trans(sio_bus_t bus, hal_sio_tx_data_t *data, hal_sio_mode_t mode)
{
    hal_i2s_config(bus, mode);
    g_i2s_tx_length = 0;
    g_i2s_tx_length = data->length;
    g_write_left_data = data->left_buff;
    g_write_right_data = data->right_buff;

    if (g_i2s_tx_length < SIO_FIRST_TRANSFER_LEN) {
        for (uint32_t i = 0; i < g_i2s_tx_length; i++) {
            hal_sio_write_data(bus, g_write_left_data[i], SIO_LEFT);
            hal_sio_write_data(bus, g_write_right_data[i], SIO_RIGHT);
        }
        g_i2s_index = g_i2s_tx_length;
    } else {
        for (uint32_t i = 0; i < SIO_FIRST_TRANSFER_LEN; i++) {
            hal_sio_write_data(bus, g_write_left_data[i], SIO_LEFT);
            hal_sio_write_data(bus, g_write_right_data[i], SIO_RIGHT);
        }
        g_i2s_index += SIO_FIRST_TRANSFER_LEN;
    }
    hal_sio_v151_txrx_enable(bus);

    if (osal_sem_down(&(g_tx_trans[bus].trans_sem)) != OSAL_SUCCESS) {
        return;
    }
    if (!g_tx_trans[bus].trans_succ) {
        return;
    }
    uapi_tcxo_delay_us(SIO_LAST_DELAY_US);
    hal_sio_v151_txrx_disable(bus);
    g_tx_trans[bus].trans_succ = false;
}
#endif

void hal_sio_v151_txrx_disable(sio_bus_t bus)
{
    sio_v151_ct_clr_data_t clr_set;
    clr_set.d32 = sios_v151_regs(bus)->ct_clr;
    clr_set.b.tx_enable = 1;
    clr_set.b.rx_enable = 1;
    clr_set.b.rst_n = 1;
    sios_v151_regs(bus)->ct_clr = clr_set.d32;
    hal_sio_v151_crg_clock_enable(bus, false);
}

void hal_sio_v151_txrx_enable(sio_bus_t bus)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    ct_set.b.tx_enable = 1;
    ct_set.b.rx_enable = 1;
    ct_set.b.rst_n = 1;
    sios_v151_regs(bus)->ct_set = ct_set.d32;
    hal_sio_v151_crg_clock_enable(bus, true);
}

#if defined(CONFIG_I2S_SUPPORT_DMA)
#define SIO_INTMASK_TX_DISABLED 0x32
#define SIO_INTMASK_RX_DISABLED 0xd
static void hal_sio_v151_dma_cfg(sio_bus_t bus, const uintptr_t attr)
{
    hal_i2s_dma_attr_t *dma_attr = (hal_i2s_dma_attr_t *)attr;
    hal_i2s_config(bus, I2S_MODE);
    hal_sio_v151_pos_merge_set_en(bus, 1);
    if (dma_attr->tx_dma_enable) {
        hal_sio_fifo_threshold_set_tx_fifo_threshold(bus, dma_attr->tx_int_threshold);
        uint32_t mask = sios_v151_regs(bus)->intmask;
        mask = (mask | SIO_INTMASK_TX_DISABLED);
        hal_sio_set_intmask(bus, mask);
    }
    if (dma_attr->rx_dma_enable) {
        hal_sio_fifo_threshold_set_rx_fifo_threshold(bus, dma_attr->rx_int_threshold);
        uint32_t mask = sios_v151_regs(bus)->intmask;
        mask = (mask | SIO_INTMASK_RX_DISABLED);
        hal_sio_set_intmask(bus, mask);
    }
}
#endif

static hal_sio_funcs_t g_hal_sio_v151_funcs = {
    .init = hal_sio_v151_init,
    .deinit = hal_sio_v151_deinit,
    .set_config = hal_sio_v151_set_config,
    .get_config = hal_sio_v151_get_config,
    .rx_enable = hal_sio_v151_rx_enable,
    .write = hal_sio_v151_write,
    .get_data = hal_sio_v151_get_data,
    .registerfunc = hal_sio_v151_register,
    .unregisterfunc = hal_sio_v151_unregister,
#if defined(CONFIG_I2S_SUPPORT_LOOPBACK)
    .loop = hal_sio_v151_loop,
    .loop_trans = hal_sio_v151_loop_trans,
#endif
#if defined(CONFIG_I2S_SUPPORT_DMA)
    .dma_cfg = hal_sio_v151_dma_cfg,
#endif
};

hal_sio_funcs_t *hal_sio_v151_funcs_get(void)
{
    return &g_hal_sio_v151_funcs;
}