/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides hal dma mem alloc\n
 *
 * History: \n
 * 2023-04-20， Create file. \n
 */
#ifndef HAL_DMA_MEM_H
#define HAL_DMA_MEM_H

#include "errcode.h"
#include "dma_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief  Dma mem init.
 * @param  [in]  mem node size.
 * @return The result of dma mem init.
 */
errcode_t hal_dma_mem_init(uint32_t node_size);

/**
 * @brief  Dma mem deinit.
 */
void hal_dma_mem_deinit(void);

/**
 * @brief  Dma alloc mem node.
 * @param  [in]  channel The DMA channel. For details, see @ref dma_channel_t.
           [in]  mem node size.
 * @return mem node.
 */
void *hal_dma_mem_alloc(dma_channel_t ch, uint32_t node_size);

/**
 * @brief  Dma free mem node.
 * @param  [in]  channel The DMA channel. For details, see @ref dma_channel_t.
 *         [in]  free mem addr.
 */
void hal_dma_mem_free(dma_channel_t ch, void *addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
