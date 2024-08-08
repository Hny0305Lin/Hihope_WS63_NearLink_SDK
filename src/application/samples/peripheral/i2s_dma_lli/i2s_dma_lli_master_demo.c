/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: i2s dma master lli Sample Source. \n
 *
 * History: \n
 * 2024-04-26, Create file. \n
 */
#include "i2s.h"
#include "watchdog.h"
#include "hal_sio.h"
#include "hal_dma.h"
#include "soc_osal.h"
#include "app_init.h"

#define I2S_DIV_NUMBER              32
#define I2S_CHANNEL_NUMBER          2
#define I2S_TX_INT_THRESHOLD        7
#define I2S_RX_INT_THRESHOLD        1
#define I2S_DMA_SRC_WIDTH           2
#define I2S_DMA_DEST_WIDTH          2
#define I2S_DMA_BURST_LENGTH        0
#define I2S_DMA_TRANS_STEP          2

#define I2S_TASK_PRIO               24
#define I2S_TASK_STACK_SIZE         0xc00

static uint32_t g_i2s_first_data = 0x10000000;    /* 32 bits */
static uint32_t g_i2s_send_dma_data[CONFIG_I2S_TRANSFER_LEN_OF_DMA_LLI] = { 0 };

static void i2s_dma_master_init(void)
{
    uapi_i2s_deinit(SIO_BUS_0);
    uapi_i2s_init(SIO_BUS_0, NULL);
    sio_porting_i2s_pinmux();
    i2s_config_t config = {
        .drive_mode = MASTER,
        .transfer_mode = STD_MODE,
        .data_width = THIRTY_TWO_BIT,
        .channels_num = TWO_CH,
        .timing = NONE_TIMING_MODE,
        .clk_edge = RISING_EDGE,
        .div_number = I2S_DIV_NUMBER,
        .number_of_channels = I2S_CHANNEL_NUMBER,
    };
    i2s_dma_attr_t attr = {
        .tx_dma_enable = 1,
        .tx_int_threshold = I2S_TX_INT_THRESHOLD,
        .rx_dma_enable = 0,
        .rx_int_threshold = I2S_RX_INT_THRESHOLD,
    };
    uapi_i2s_set_config(SIO_BUS_0, &config);
    uapi_i2s_dma_config(SIO_BUS_0, &attr);
}

static void *i2s_dma_master_task(const char *arg)
{
    unused(arg);
    int32_t ret = CONFIG_I2S_TRANSFER_LEN_OF_DMA_LLI;
#if defined(CONFIG_SIO_USING_V151)
    ret = ERRCODE_SUCC;
#endif
    uapi_dma_deinit();
    i2s_dma_master_init();
    for (uint32_t i = 0; i < CONFIG_I2S_TRANSFER_LEN_OF_DMA_LLI; i += I2S_DMA_TRANS_STEP) {
        g_i2s_send_dma_data[i] = g_i2s_first_data;
        g_i2s_send_dma_data[i + 1] = g_i2s_first_data;
        g_i2s_first_data++;
    }
    /* DMA init. */
    uapi_dma_init();
    uapi_dma_open();
    i2s_dma_config_t dma_cfg = {
        .src_width = I2S_DMA_SRC_WIDTH,
        .dest_width = I2S_DMA_DEST_WIDTH,
        .burst_length = I2S_DMA_BURST_LENGTH,
        .priority = 0,
    };
    osal_printk("DMA master transfer start.\r\n");
    while (1) {
        uapi_watchdog_kick();
        if (uapi_i2s_merge_write_by_dma(SIO_BUS_0, &g_i2s_send_dma_data, CONFIG_I2S_TRANSFER_LEN_OF_DMA_LLI, &dma_cfg,
            (uintptr_t)NULL, true) != ret) {
            osal_printk("master uapi_i2s_merge_write_by_dma error.\r\n");
        }
    }
    return NULL;
}

static void i2s_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)i2s_dma_master_task, 0, "I2sDmaMasterTask",
                                      I2S_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2S_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the i2s_entry. */
app_run(i2s_entry);