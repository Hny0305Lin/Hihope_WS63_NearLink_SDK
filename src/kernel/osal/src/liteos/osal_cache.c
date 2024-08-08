/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: cache
 *
 * Create: 2021-12-16
 */

#include <arch/cache.h>
#include "soc_osal.h"
#include "los_typedef.h"

/**
 * @brief flush DCache.
 * <li>The base address will be aligned to CACHE_LINE_SIZE(32Bytes) if it's not aligned to CACHE_LINE_SIZE.</li>
 * @param  phys_addr    [IN] The start address need flush.
 * @param  size         [IN] The size of flush memory.
 */
void osal_dcache_region_wb(void *kvirt, unsigned long phys_addr, unsigned long size)
{
    // Make sure that phys_addr is valid when mmu does not exist, and user set its addr in kvirt.
    phys_addr = (phys_addr == 0) ? (uintptr_t)kvirt : phys_addr;
    ArchDCacheFlushByVa((uintptr_t)phys_addr, (uint32_t)size);
}

/**
 * @brief invalid DCache.
 * <li>The base address will be aligned to CACHE_LINE_SIZE(32Bytes) if it's not aligned to CACHE_LINE_SIZE.</li>
 * @param  addr     [IN] The start address need invalid.
 * @param  size     [IN] The size of invalid memory.
 */
void osal_dcache_region_inv(void *addr, unsigned long size)
{
    ArchDCacheInvByVa((uintptr_t)addr, (uint32_t)size);
}

/**
 * @brief clean DCache.
 * @par Description: This API is used to clean DCache.
 * @attention The API will clean DCache according to based address and input size.</li>
 */
void osal_dcache_region_clean(void *addr, unsigned int size)
{
    ArchDCacheCleanByVa((uintptr_t)addr, (uint32_t)size);
}
