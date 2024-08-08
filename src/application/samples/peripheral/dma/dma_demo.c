/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: DMA Sample Source. \n
 *
 * History: \n
 * 2023-07-06, Create file. \n
 */
#include "hal_dma.h"
#include "soc_osal.h"
#include "securec.h"
#include "app_init.h"

#define DMA_TRANSFER_WORD_NUM       32
#define DMA_TRANSFER_PRIORITY       0
#define DMA_TRANSFER_WIDTH          2
#define DMA_TASK_DURATION_MS        500

#define DMA_TASK_PRIO               24
#define DMA_TASK_STACK_SIZE         0x1000

static uint32_t g_app_dma_src_data[DMA_TRANSFER_WORD_NUM] = { 0 };
static uint32_t g_app_dma_desc_data[DMA_TRANSFER_WORD_NUM] = { 0 };
static uint8_t g_dma_trans_done = 0;

static void app_dma_trans_done_callback(uint8_t int_type, uint8_t channel, uintptr_t arg)
{
    unused(arg);
    unused(channel);
    switch (int_type) {
        case HAL_DMA_INTERRUPT_TFR:
            g_dma_trans_done = 1;
            break;
        case HAL_DMA_INTERRUPT_BLOCK:
            g_dma_trans_done = 1;
            break;
        case HAL_DMA_INTERRUPT_ERR:
            osal_printk("DMA transfer error.\r\n");
            break;
        default:
            break;
    }
}

static void *dma_task(const char *arg)
{
    unused(arg);
    /* DMA init. */
    uapi_dma_init();
    uapi_dma_open();

#if defined(CONFIG_DMA_MEMORY_LLI_TRANSFER_MODE)
    dma_channel_t dma_channel = DMA_CHANNEL_NONE;
    dma_channel = uapi_dma_get_lli_channel(0, HAL_DMA_HANDSHAKING_MAX_NUM);
#endif

    for (uint32_t i = 0; i < DMA_TRANSFER_WORD_NUM; i++) {
        g_app_dma_src_data[i] = i;
    }
    memset_s(g_app_dma_desc_data, DMA_TRANSFER_WORD_NUM, 0, DMA_TRANSFER_WORD_NUM);

    dma_ch_user_memory_config_t transfer_config = { 0 };
    transfer_config.src = (uint32_t)(uintptr_t)g_app_dma_src_data;
    transfer_config.dest = (uint32_t)(uintptr_t)g_app_dma_desc_data;
    transfer_config.transfer_num = DMA_TRANSFER_WORD_NUM;
    transfer_config.priority = DMA_TRANSFER_PRIORITY;
    transfer_config.width = DMA_TRANSFER_WIDTH;

    while (1) {
        osal_msleep(DMA_TASK_DURATION_MS);
        g_dma_trans_done = 0;
#if defined(CONFIG_DMA_MEMORY_LLI_TRANSFER_MODE)
        osal_printk("dma config link list item of memory to memory start!\r\n");
        if (uapi_dma_transfer_memory_lli(dma_channel, &transfer_config, app_dma_trans_done_callback) == ERRCODE_SUCC) {
            osal_printk("dma config link list item of memory to memory succ!\r\n");
        }
        osal_printk("dma enable lli memory transfer start!\r\n");
        if (uapi_dma_enable_lli(dma_channel, app_dma_trans_done_callback, (uintptr_t)NULL) == ERRCODE_SUCC) {
            osal_printk("dma enable lli memory transfer succ!\r\n");
        }
        while (!g_dma_trans_done) {}
        if (uapi_dma_end_transfer(dma_channel) == ERRCODE_SUCC) {
            osal_printk("dma channel transfer finish!\r\n");
        }
#else
        osal_printk("dma single memory transfer start!\r\n");
        if (uapi_dma_transfer_memory_single(&transfer_config, app_dma_trans_done_callback,
                                            (uintptr_t)NULL) == ERRCODE_SUCC) {
            osal_printk("dma single memory transfer succ!\r\n");
        }
        while (!g_dma_trans_done) {}
        osal_printk("dma checking transfer from 0x%08x to 0x%08x...\r\n", transfer_config.src, transfer_config.dest);
        if (memcmp((void *)transfer_config.src, (void *)transfer_config.dest, transfer_config.transfer_num) == 0) {
            osal_printk("dma memory copy test succ, length = %d block\r\n", transfer_config.transfer_num);
        }
#endif
    }

    return NULL;
}

static void dma_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)dma_task, 0, "DmaTask", DMA_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, DMA_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the dma_entry. */
app_run(dma_entry);