/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides hal dma \n
 *
 * History: \n
 * 2022-10-16， Create file. \n
 */

#include "hal_dma.h"

uintptr_t g_dma_regs = NULL;
#if defined(CONFIG_DMA_SUPPORT_SMDMA)
uintptr_t g_sdma_regs = NULL;
#endif /* CONFIG_DMA_SUPPORT_SMDMA */

static uintptr_t dma_base_addr_get(void)
{
    return g_dma_base_addr;
}

#if defined(CONFIG_DMA_SUPPORT_SMDMA)
static uintptr_t sdma_base_addr_get(void)
{
    return g_sdma_base_addr;
}
#endif

errcode_t hal_dma_regs_init(void)
{
    if (dma_base_addr_get() == 0) {
        return ERRCODE_DMA_REG_ADDR_INVALID;
    }
    g_dma_regs = dma_base_addr_get();

#if defined(CONFIG_DMA_SUPPORT_SMDMA)
    if (sdma_base_addr_get() == 0) {
        return ERRCODE_DMA_REG_ADDR_INVALID;
    }
    g_sdma_regs = sdma_base_addr_get();
#endif /* CONFIG_DMA_SUPPORT_SMDMA */

    return ERRCODE_SUCC;
}