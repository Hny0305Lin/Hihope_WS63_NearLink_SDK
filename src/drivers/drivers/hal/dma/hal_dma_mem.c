/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides hal dma mem alloc\n
 *
 * History: \n
 * 2023-04-20， Create file. \n
 */

#include "hal_dma_mem.h"
#include "common_def.h"
#include "memory/osal_addr.h"

#ifdef CONFIG_DMA_LLI_NODE_FIX_MEM
static void *g_hal_dma_lli_node = NULL;
static uint8_t g_hal_dma_lli_index[DMA_CHANNEL_MAX_NUM] = {0};
#endif

/**
 * @brief  Dma mem init.
 * @param  [in]  mem node size.
 * @return The result of dma mem init.
 */
errcode_t hal_dma_mem_init(uint32_t node_size)
{
#ifdef CONFIG_DMA_LLI_NODE_FIX_MEM
    if (g_hal_dma_lli_node == NULL) {
        g_hal_dma_lli_node = osal_kmalloc(node_size * DMA_CHANNEL_MAX_NUM * CONFIG_DMA_LLI_NODE_MAX, 0);
        if (g_hal_dma_lli_node == NULL) {
            return ERRCODE_MALLOC;
        }
    }
#else
    unused(node_size);
#endif
    return ERRCODE_SUCC;
}

/**
 * @brief  Dma mem deinit.
 */
void hal_dma_mem_deinit(void)
{
#ifdef CONFIG_DMA_LLI_NODE_FIX_MEM
    if (g_hal_dma_lli_node != NULL) {
        osal_kfree(g_hal_dma_lli_node);
        g_hal_dma_lli_node = NULL;
    }
#endif
}

/**
 * @brief  Dma alloc mem node.
 * @param  [in]  channel The DMA channel. For details, see @ref dma_channel_t.
           [in]  mem node size.
 * @return mem node.
 */
void *hal_dma_mem_alloc(dma_channel_t ch, uint32_t node_size)
{
#ifdef CONFIG_DMA_LLI_NODE_FIX_MEM
    if ((g_hal_dma_lli_index[ch] >= CONFIG_DMA_LLI_NODE_MAX) || (g_hal_dma_lli_node == NULL)) {
        return NULL;
    }
    g_hal_dma_lli_index[ch]++;
    return (uint8_t *)g_hal_dma_lli_node +
        ((uint8_t)ch * CONFIG_DMA_LLI_NODE_MAX + g_hal_dma_lli_index[ch] - 1) * node_size;
#else
    unused(ch);
    return osal_kmalloc(node_size, 0);
#endif
}

/**
 * @brief  Dma free mem node.
 * @param  [in]  channel The DMA channel. For details, see @ref dma_channel_t.
 *         [in]  free mem addr.
 */
void hal_dma_mem_free(dma_channel_t ch, void *addr)
{
#ifdef CONFIG_DMA_LLI_NODE_FIX_MEM
    if (g_hal_dma_lli_index[ch] > 0) {
        g_hal_dma_lli_index[ch]--;
    }
    unused(addr);
#else
    unused(ch);
    return osal_kfree(addr);
#endif
}
