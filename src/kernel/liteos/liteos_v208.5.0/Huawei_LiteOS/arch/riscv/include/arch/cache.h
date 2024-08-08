/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : Cache operations HeadFile
 * Author: Huawei LiteOS Team
 * Create : 2022-12-20
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ---------------------------------------------------------------------------- */

/**
 * @defgroup cache
 * @ingroup kernel
 */

#ifndef _ARCH_CACHE_H
#define _ARCH_CACHE_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    CACHE_4KB  = 0,
    CACHE_8KB  = 2,
    CACHE_16KB = 4,
    CACHE_32KB = 6,
    CACHE_64KB = 8,
} CacheSize;

typedef enum {
    CACHE_PREF_1_LINES = 0,
    CACHE_PREF_2_LINES = 1,
    CACHE_PREF_3_LINES = 2,
    CACHE_PREF_4_LINES = 3,
    CACHE_PREF_5_LINES = 4,
    CACHE_PREF_6_LINES = 5,
    CACHE_PREF_7_LINES = 6,
    CACHE_PREF_8_LINES = 7,
    CACHE_PREF_LINES_MAX,
} CachePrefLines;

/* riscv cache register config */
/*
 * csr_bit[0]  ICEN   Instruction cache is enabled when this bit is set to 1b1.  Default is disabled.
 * Bit[2:1]    ICS    Instruction cache size: 2b00=4KB, 2b01=8KB, 2b10=16KB, 2b11=32KB.  Default is 32KB
 */
#define ICCTL                   0x7C0
#define ICCTL_ENABLE            0x1

/*
 * Bit[0]   DCEN  Data cache is enabled when this bit is set to 1b1.  Default is disabled.
 * Bit[2:1] DCS   Data cache size: 2b00=4KB, 2b01=8KB, 2b10=16KB, 2b11=32KB.  Default is 32KB
 */
#define DCCTL                   0x7C1
#define DCCTL_ENABLE            0x1

/*
 * Bit[0]  VA   Instruction cache invalidation by all or VA: 1b0=all, 1b1=VA.
 * Bit[2]  ICIV Initiate instruction cache invalidation when this bit is set to 1b1.
           When the instruction cache invalidation is by VA, the virtual address is specified in icinva CSR
*/
#define ICMAINT                 0x7C2
#define DCMAINT                 0x7C3
#define ICINCVA                 0x7C4
#define DCINCVA                 0x7C5

#define VA                      0x1
#define ICIV                    (0x1U << 2)
#define DCIV                    (0x1U << 2)
#define DCC                     (0x1U << 3)

#define ICACHE_BY_ALL           ICIV
#define ICACHE_BY_VA            (ICIV | VA)

#define DCACHE_INV_BY_VA        (DCIV | VA)
#define DCACHE_INV_BY_ALL       DCIV
#define DCACHE_CLEAN_ALL        DCC
#define DCACHE_CLEAN_BY_VA      (DCC | VA)
#define DCACHE_FLUSH_BY_VA      (DCC | DCIV | VA)
#define DCACHE_FLUSH_ALL        (DCC | DCIV)

#define APREFI                  0x7C6
#define APREFD                  0x7C7
#define IAPEN                   0x1
#define SAPEN                   (0x1U << 4)

#define CACHE_LINE_SIZE         32U

/**
 * @ingroup cache
 * @brief enable ICache.
 *
 * @par Description:
 * This API is used to enable ICache.
 * @attention
 * <ul>
 * <li>The API is just enable ICache.</li>
 * <li>.</li>
 * </ul>
 * @param  icclSize     [IN] The Instruction cache size.
 * @retval UINT32       return LOS_OK, or return failed.
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R002C00
 */
UINT32 ArchICacheEnable(CacheSize icclSize);

/**
 * @ingroup cache
 * @brief enable DCache.
 *
 * @par Description:
 * This API is used to enable DCache.
 * @attention
 * <ul>
 * <li>The API is just enable DCache.</li>
 * <li>.</li>
 * </ul>
 * @param  dcclSize     [IN] The Data cache size.
 * @retval UINT32       return LOS_OK, or return failed.
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R002C00
 */
UINT32 ArchDCacheEnable(CacheSize dcclSize);

/**
 * @ingroup cache
 * @brief Init instruction cache auto prefetch.
 *
 * @par Description:
 * This API is used to Init instruction cache auto prefetch ICL value as 1-8.
 * @attention
 * <ul>
 * <li>The API is just to Init instruction cache auto prefetch ICL value as 1-8.</li>
 * <li>.</li>
 * </ul>
 * @param  iclValue     [IN] The Instruction cache auto prefetch value.
 * @retval UINT32       return LOS_OK, or return failed.
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R002C00
 */
UINT32 ArchICachePrefetchEnable(CachePrefLines iclValue);

/**
 * @ingroup cache
 * @brief Init data cache auto prefetch.
 *
 * @par Description:
 * This API is used to Init data cache auto prefetch ICL and SCL value as 1-8.
 * @attention
 * <ul>
 * <li>The API is just to Init data cache auto prefetch ICL and SCL value as 1-8.</li>
 * <li>.</li>
 * </ul>
 * @param  iclValue     [IN] The Data cache auto prefetch value.
 * @param  sclValue     [IN] The Data cache auto prefetch value.
 * @retval UINT32       return LOS_OK, or return failed.
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R002C00
 */
UINT32 ArchDCachePrefetchEnable(CachePrefLines iclValue, CachePrefLines sclValue);

/**
 * @ingroup cache
 * @brief flush DCache.
 *
 * @par Description:
 * This API is used to flush DCache by Va.
 * @attention
 * <ul>
 * <li>The base address will be aligned to CACHE_LINE_SIZE if it's not aligned to CACHE_LINE_SIZE.</li>
 * <li>.</li>
 * </ul>
 * @param  baseAddr     [IN] The start address need flush.
 * @param  size         [IN] The size of flush memory.
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R002C00
 */
VOID ArchDCacheFlushByVa(UINTPTR baseAddr, UINT32 size);

/**
 * @ingroup cache
 * @brief flush ICache.
 *
 * @par Description:
 *  This API is used to flush ICache by Va.
 * @attention
 * <ul>
 * <li>The base address will be aligned to CACHE_LINE_SIZE if it's not aligned to CACHE_LINE_SIZE.</li>
 * <li>.</li>
 * </ul>
 * @param  baseAddr     [IN] The start address need flush.
 * @param  size         [IN] The size of flush memory.
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R002C00
 */
VOID ArchICacheFlushByVa(UINTPTR baseAddr, UINT32 size);

/**
 * @ingroup cache
 * @brief invalid DCache.
 *
 * @par Description:
 * This API is used to invalid DCache by Va.
 * @attention
 * <ul>
 * <li>The base address will be aligned to CACHE_LINE_SIZE if it's not aligned to CACHE_LINE_SIZE.</li>
 * <li>.</li>
 * </ul>
 * @param  baseAddr     [IN] The start address need flush.
 * @param  size         [IN] The size of flush memory.
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R002C00
 */
VOID ArchDCacheInvByVa(UINTPTR baseAddr, UINT32 size);

/**
 * @ingroup cache
 * @brief clean DCache.
 *
 * @par Description:
 * This API is used to clean DCache.
 * @attention
 * <ul>
 * <li>The API will clean DCache according to based address and input size.</li>
 * <li>.</li>
 * </ul>
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R002C00
 */
VOID ArchDCacheCleanByVa(UINTPTR baseAddr, UINT32 size);

/**
 * @ingroup cache
 * @brief flush ICache by start address and end address.
 *
 * @par Description:
 *  This API is used to flush DCache by Va.
 * @attention
 * <ul>
 * <li>The startAddr or endAddr address will be aligned to CACHE_LINE_SIZE if it's not aligned to CACHE_LINE_SIZE.</li>
 * <li>.</li>
 * </ul>
 * @param  startAddr     [IN] The start address need flush.
 * @param  endAddr       [IN] The end address need flush.
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R003C00
 */
VOID ArchICacheFlushByAddr(UINTPTR startAddr, UINTPTR endAddr);

/**
 * @ingroup cache
 * @brief flush DCache by start address and end address.
 *
 * @par Description:
 *  This API is used to flush DCache by Va.
 * @attention
 * <ul>
 * <li>The startAddr or endAddr address will be aligned to CACHE_LINE_SIZE if it's not aligned to CACHE_LINE_SIZE.</li>
 * <li>.</li>
 * </ul>
 * @param  startAddr     [IN] The start address need flush.
 * @param  endAddr       [IN] The end address need flush.
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R003C00
 */
VOID ArchDCacheFlushByAddr(UINTPTR startAddr, UINTPTR endAddr);

/**
 * @ingroup cache
 * @brief clean DCache by start address and end address.
 *
 * @par Description:
 *  This API is used to clean DCache by Va.
 * @attention
 * <ul>
 * <li>The startAddr or endAddr address will be aligned to CACHE_LINE_SIZE if it's not aligned to CACHE_LINE_SIZE.</li>
 * <li>.</li>
 * </ul>
 * @param  startAddr     [IN] The start address need clean.
 * @param  endAddr       [IN] The end address need clean.
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R003C00
 */
VOID ArchDCacheCleanByAddr(UINTPTR startAddr, UINTPTR endAddr);

/**
 * @ingroup cache
 * @brief invalidate DCache by start address and end address.
 *
 * @par Description:
 *  This API is used to flush DCache by Va.
 * @attention
 * <ul>
 * <li>The startAddr or endAddr address will be aligned to CACHE_LINE_SIZE if it's not aligned to CACHE_LINE_SIZE.</li>
 * <li>.</li>
 * </ul>
 * @param  startAddr     [IN] The start address need invalidate.
 * @param  endAddr       [IN] The end address need invalidate.
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R003C00
 */
VOID ArchDCacheInvByAddr(UINTPTR startAddr, UINTPTR endAddr);

/**
 * @ingroup cache
 * @brief flush ICache.
 *
 * @par Description:
 * This API is used to flush ICache.
 * @attention
 * <ul>
 * <li>The API will flush all ICache.</li>
 * <li>.</li>
 * </ul>
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R002C00
 */
VOID ArchICacheFlush(VOID);

/**
 * @ingroup cache
 * @brief flush DCache.
 *
 * @par Description:
 * This API is used to flush DCache.
 * @attention
 * <ul>
 * <li>The API will flush all DCache, the size is 32K.</li>
 * <li>.</li>
 * </ul>
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R002C00
 */
VOID ArchDCacheFlush(VOID);

/**
 * @ingroup cache
 * @brief Invalidate DCache.
 *
 * @par Description:
 * This API is used to Invalidate DCache.
 * @attention
 * <ul>
 * <li>The API will Invalidate all DCache.</li>
 * <li>.</li>
 * </ul>
 * @par Dependency:
 * <ul><li>cache.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R002C00
 */
VOID ArchDCacheInvalidate(VOID);

#define flush_dcache(start, end) ArchDCacheFlushByAddr(start, end)
#define flush_icache() ArchICacheFlush()

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_CACHE_H */
