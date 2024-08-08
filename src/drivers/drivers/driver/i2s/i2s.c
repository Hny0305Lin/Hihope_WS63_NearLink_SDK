/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides i2s driver source \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */
#include "common_def.h"
#include "errcode.h"
#include "soc_osal.h"
#include "hal_sio.h"
#include "i2s.h"
#if defined(CONFIG_I2S_SUPPORT_DMA)
#include "dma.h"
#include "dma_porting.h"
#include "sio_porting.h"

#define I2S_DMA_TRANS_MEMORY_TO_PERIPHERAL_DMA      1
#define I2S_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA      2
#define I2S_DMA_TRANSFER_DIR_MEM_TO_PERIPHERAL      0
#define I2S_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM      1
#define I2S_DMA_ADDRESS_INC_INCREMENT               0
#define I2S_DMA_ADDRESS_INC_NO_CHANGE               2
#define I2S_DMA_PROTECTION_CONTROL_BUFFERABLE       1

typedef struct i2s_dma_trans_inf {
    bool trans_succ;
    uint8_t channel;
    osal_semaphore dma_sem;
} i2s_dma_trans_inf_t;

static i2s_dma_trans_inf_t g_tx_dma_trans[I2S_MAX_NUMBER] = { 0 };
static i2s_dma_trans_inf_t g_rx_dma_trans[I2S_MAX_NUMBER] = { 0 };
#endif

static hal_sio_funcs_t *g_hal_funcs = NULL;
static bool g_i2s_inited = false;

static hal_sio_config_t g_hal_config = {
    .drive_mode= (hal_sio_driver_mode_t)0,
    .transfer_mode = (hal_sio_transfer_mode_t)0,
    .data_width = (hal_sio_data_width_t)0,
    .channels_num = (hal_sio_channel_number_t)0,
    .timing = (hal_sio_timing_mode_t)0,
    .clk_edge = (hal_sio_clk_edge_t)0,
    .div_number = 0,
    .number_of_channels = 0,
};

errcode_t uapi_i2s_init(sio_bus_t bus, i2s_callback_t callback)
{
    if (unlikely(g_i2s_inited)) {
        return ERRCODE_SUCC;
    }
    if (unlikely(bus >= CONFIG_I2S_BUS_MAX_NUM)) {
        return ERRCODE_INVALID_PARAM;
    }

    sio_porting_register_hal_funcs(bus);
    g_hal_funcs = hal_sio_get_funcs(bus);

    g_hal_funcs->init(bus);
    g_hal_funcs->registerfunc(bus, (i2s_callback_t)callback);

    sio_porting_clock_enable(true);
#if defined(CONFIG_I2S_SUPPORT_DMA)
    osal_sem_init(&(g_tx_dma_trans[bus].dma_sem), 0);
    osal_sem_init(&(g_rx_dma_trans[bus].dma_sem), 0);
#endif
    g_i2s_inited = true;
    return ERRCODE_SUCC;
}

errcode_t uapi_i2s_deinit(sio_bus_t bus)
{
    if (unlikely(!g_i2s_inited)) {
        return ERRCODE_SUCC;
    }

    if (unlikely(bus >= CONFIG_I2S_BUS_MAX_NUM)) {
        return ERRCODE_INVALID_PARAM;
    }

    g_hal_funcs->rx_enable(bus, false);
    g_hal_funcs->unregisterfunc(bus);
    g_hal_funcs->deinit(bus);

    sio_porting_unregister_hal_funcs(bus);
    sio_porting_unregister_irq(bus);

    sio_porting_clock_enable(false);
    g_i2s_inited = false;
    return ERRCODE_SUCC;
}

errcode_t uapi_i2s_set_config(sio_bus_t bus, const i2s_config_t *config)
{
    if (unlikely(!g_i2s_inited)) {
        return ERRCODE_I2S_NOT_INIT;
    }
    if (unlikely(bus >= CONFIG_I2S_BUS_MAX_NUM || config == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

    hal_sio_config_t hal_config = {
        .drive_mode= (hal_sio_driver_mode_t)config->drive_mode,
        .transfer_mode = (hal_sio_transfer_mode_t)config->transfer_mode,
        .data_width = (hal_sio_data_width_t)config->data_width,
        .channels_num = (hal_sio_channel_number_t)config->channels_num,
        .timing = (hal_sio_timing_mode_t)config->timing,
        .clk_edge = (hal_sio_clk_edge_t)config->clk_edge,
        .div_number = config->div_number,
        .number_of_channels = config->number_of_channels,
    };
    g_hal_funcs->set_config(bus, &hal_config);
    return ERRCODE_SUCC;
}

errcode_t uapi_i2s_get_config(sio_bus_t bus, i2s_config_t *config)
{
    if (unlikely(!g_i2s_inited)) {
        return ERRCODE_I2S_NOT_INIT;
    }
    if (unlikely(bus >= CONFIG_I2S_BUS_MAX_NUM || config == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

    errcode_t ret = g_hal_funcs->get_config(bus, &g_hal_config);

    config->drive_mode = g_hal_config.drive_mode;
    config->transfer_mode = g_hal_config.transfer_mode;
    config->data_width = g_hal_config.data_width;
    config->channels_num = g_hal_config.channels_num;
    config->timing = g_hal_config.timing;
    config->clk_edge = g_hal_config.clk_edge;
    config->div_number = g_hal_config.div_number;
    config->number_of_channels = g_hal_config.number_of_channels;

    return ret;
}

errcode_t uapi_i2s_get_data(sio_bus_t bus, i2s_rx_data_t *data)
{
    if (unlikely(!g_i2s_inited)) {
        return ERRCODE_I2S_NOT_INIT;
    }
    if (unlikely(bus >= CONFIG_I2S_BUS_MAX_NUM)) {
        return ERRCODE_INVALID_PARAM;
    }

    if (data == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    i2s_rx_data_t *temp_data = data;

    g_hal_funcs->get_data(bus, (hal_sio_rx_data_t *)(uintptr_t)temp_data);

    return ERRCODE_SUCC;
}

errcode_t uapi_i2s_write_data(sio_bus_t bus, i2s_tx_data_t *data)
{
    if (unlikely(!g_i2s_inited)) {
        return ERRCODE_I2S_NOT_INIT;
    }
    if (unlikely(bus >= CONFIG_I2S_BUS_MAX_NUM)) {
        return ERRCODE_INVALID_PARAM;
    }

    if (data == NULL || data->left_buff == NULL || data->right_buff == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    g_hal_funcs->write(bus, (hal_sio_tx_data_t *)(uintptr_t)data, I2S_MODE);

    return ERRCODE_SUCC;
}

errcode_t uapi_i2s_read_start(sio_bus_t bus)
{
    if (unlikely(!g_i2s_inited)) {
        return ERRCODE_I2S_NOT_INIT;
    }
    if (unlikely(bus >= CONFIG_I2S_BUS_MAX_NUM)) {
        return ERRCODE_INVALID_PARAM;
    }

    g_hal_funcs->rx_enable(bus, true);

    return ERRCODE_SUCC;
}

errcode_t uapi_i2s_loop_trans(sio_bus_t bus, i2s_tx_data_t *data)
{
    if (unlikely(!g_i2s_inited)) {
        return ERRCODE_I2S_NOT_INIT;
    }
    if (unlikely(bus >= CONFIG_I2S_BUS_MAX_NUM)) {
        return ERRCODE_INVALID_PARAM;
    }

    if (data == NULL || data->left_buff == NULL || data->right_buff == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    g_hal_funcs->loop_trans(bus, (hal_sio_tx_data_t *)(uintptr_t)data, I2S_MODE);

    return ERRCODE_SUCC;
}

#if defined(CONFIG_I2S_SUPPORT_LOOPBACK) && (CONFIG_I2S_SUPPORT_LOOPBACK == 1)
errcode_t uapi_i2s_loopback(sio_bus_t bus, bool en)
{
    if (unlikely(!g_i2s_inited)) {
        return ERRCODE_I2S_NOT_INIT;
    }
    if (unlikely(bus >= CONFIG_I2S_BUS_MAX_NUM)) {
        return ERRCODE_INVALID_PARAM;
    }

    g_hal_funcs->loop(bus, en);

    return ERRCODE_SUCC;
}
#endif /* CONFIG_I2S_SUPPORT_LOOPBACK */
#if defined(CONFIG_I2S_SUPPORT_DMA)
int32_t uapi_i2s_dma_config(sio_bus_t bus, i2s_dma_attr_t *i2s_dma_cfg)
{
    if (unlikely(!g_i2s_inited)) {
        return ERRCODE_I2S_NOT_INIT;
    }
    if (unlikely(bus >= CONFIG_I2S_BUS_MAX_NUM)) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_funcs->dma_cfg(bus, (uintptr_t)i2s_dma_cfg);
    return ERRCODE_SUCC;
}

static void i2s_dma_rx_isr(uint8_t int_type, uint8_t ch, uintptr_t arg)
{
    unused(arg);
    uint8_t bus = I2S_MAX_NUMBER;
    for (uint8_t i = SIO_BUS_0; i < I2S_MAX_NUMBER; i++) {
        if (g_rx_dma_trans[i].channel == ch) {
            bus = i;
            break;
        }
    }

    if (bus != I2S_MAX_NUMBER) {
        if (int_type == 0) {
            g_rx_dma_trans[bus].trans_succ = true;
        }
        osal_sem_up(&(g_rx_dma_trans[bus].dma_sem));
    }
}

static void i2s_dma_tx_isr(uint8_t int_type, uint8_t ch, uintptr_t arg)
{
    unused(arg);
    uint8_t bus = I2S_MAX_NUMBER;
    for (uint8_t i = SIO_BUS_0; i < I2S_MAX_NUMBER; i++) {
        if (g_tx_dma_trans[i].channel == ch) {
            bus = i;
            break;
        }
    }

    if (bus != I2S_MAX_NUMBER) {
        if (int_type == 0) {
            g_tx_dma_trans[bus].trans_succ = true;
        }
        osal_sem_up(&(g_tx_dma_trans[bus].dma_sem));
    }
}

static void i2s_dma_peripheral_tx_config(dma_ch_user_peripheral_config_t *user_cfg,
    i2s_dma_config_t *dma_cfg, uint32_t length)
{
    user_cfg->transfer_num = length;
    user_cfg->src_handshaking = 0;
    user_cfg->trans_type = I2S_DMA_TRANS_MEMORY_TO_PERIPHERAL_DMA;
    user_cfg->trans_dir = I2S_DMA_TRANSFER_DIR_MEM_TO_PERIPHERAL;
    user_cfg->priority = dma_cfg->priority;
    user_cfg->src_width = dma_cfg->src_width;
    user_cfg->dest_width = dma_cfg->dest_width;
    user_cfg->burst_length = dma_cfg->burst_length;
    user_cfg->src_increment = I2S_DMA_ADDRESS_INC_INCREMENT;
    user_cfg->dest_increment = I2S_DMA_ADDRESS_INC_NO_CHANGE;
    user_cfg->protection = I2S_DMA_PROTECTION_CONTROL_BUFFERABLE;
    user_cfg->dest_handshaking = i2s_port_get_dma_trans_dest_handshaking(0);
}

int32_t uapi_i2s_merge_write_by_dma(sio_bus_t bus, const void *buffer, uint32_t length,
    i2s_dma_config_t *dma_cfg, uintptr_t arg, bool block)
{
    if (unlikely(!g_i2s_inited)) {
        return ERRCODE_I2S_NOT_INIT;
    }
    if ((bus >= I2S_MAX_NUMBER) || (dma_cfg == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }
    if ((buffer == NULL) || (length == 0)) {
        return ERRCODE_INVALID_PARAM;
    }
    dma_ch_user_peripheral_config_t user_cfg;
    uint8_t dma_ch;
    user_cfg.src = (uint32_t)(uintptr_t)buffer;
    user_cfg.dest = (uint32_t)i2s_porting_tx_merge_data_addr_get(bus);
    i2s_dma_peripheral_tx_config(&user_cfg, dma_cfg, length);
    if (user_cfg.dest_handshaking == HAL_DMA_HANDSHAKING_MAX_NUM) {
        return ERRCODE_NOT_SUPPORT;
    }
    if (uapi_dma_configure_peripheral_transfer_single(&user_cfg, &dma_ch, i2s_dma_tx_isr, arg) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    g_tx_dma_trans[bus].channel = dma_ch;
    g_tx_dma_trans[bus].trans_succ = false;

    if (uapi_dma_start_transfer(dma_ch) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    hal_sio_set_crg_clock_enable(bus, true);
    hal_sio_set_tx_enable(bus, 1);
    if (!block) {
        return ERRCODE_SUCC;
    }

    if (osal_sem_down(&(g_tx_dma_trans[bus].dma_sem)) != OSAL_SUCCESS) {
        return ERRCODE_FAIL;
    }

    if (!g_tx_dma_trans[bus].trans_succ) {
        return ERRCODE_FAIL;
    }
    return (int32_t)uapi_dma_get_block_ts(dma_ch);
}

static void i2s_dma_peripheral_rx_config(dma_ch_user_peripheral_config_t *user_cfg,
    i2s_dma_config_t *dma_cfg, uint32_t length)
{
    user_cfg->transfer_num = length;
    user_cfg->dest_handshaking = 0;
    user_cfg->trans_type = I2S_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA;
    user_cfg->trans_dir = I2S_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM;
    user_cfg->priority = dma_cfg->priority;
    user_cfg->src_width = dma_cfg->src_width;
    user_cfg->dest_width = dma_cfg->dest_width;
    user_cfg->burst_length = dma_cfg->burst_length;
    user_cfg->src_increment = I2S_DMA_ADDRESS_INC_NO_CHANGE;
    user_cfg->dest_increment = I2S_DMA_ADDRESS_INC_INCREMENT;
    user_cfg->protection = I2S_DMA_PROTECTION_CONTROL_BUFFERABLE;
    user_cfg->src_handshaking = i2s_port_get_dma_trans_src_handshaking(0);
}

int32_t uapi_i2s_merge_read_by_dma(sio_bus_t bus, const void *buffer, uint32_t length,
    i2s_dma_config_t *dma_cfg, uintptr_t arg, bool block)
{
    if (unlikely(!g_i2s_inited)) {
        return ERRCODE_I2S_NOT_INIT;
    }
    if ((bus >= I2S_MAX_NUMBER) || (dma_cfg == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }
    if ((buffer == NULL) || (length == 0)) {
        return ERRCODE_INVALID_PARAM;
    }
    dma_ch_user_peripheral_config_t user_cfg;
    uint8_t dma_ch;
    user_cfg.src = (uint32_t)i2s_porting_rx_merge_data_addr_get(bus);
    user_cfg.dest = (uint32_t)(uintptr_t)buffer;
    i2s_dma_peripheral_rx_config(&user_cfg, dma_cfg, length);
    if (user_cfg.src_handshaking == HAL_DMA_HANDSHAKING_MAX_NUM) {
        return ERRCODE_NOT_SUPPORT;
    }
    if (uapi_dma_configure_peripheral_transfer_single(&user_cfg, &dma_ch, i2s_dma_rx_isr, arg) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    g_rx_dma_trans[bus].channel = dma_ch;
    g_rx_dma_trans[bus].trans_succ = false;
    if (uapi_dma_start_transfer(dma_ch) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    hal_sio_set_rx_enable(bus, 1);
    if (!block) {
        return ERRCODE_SUCC;
    }

    if (osal_sem_down(&(g_rx_dma_trans[bus].dma_sem)) != OSAL_SUCCESS) {
        return ERRCODE_FAIL;
    }
    if (!g_rx_dma_trans[bus].trans_succ) {
        return ERRCODE_FAIL;
    }
    return (int32_t)uapi_dma_get_block_ts(dma_ch);
}
#endif