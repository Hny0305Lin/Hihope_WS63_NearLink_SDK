/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Mmu Map/Unmap HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-06-09
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
 * --------------------------------------------------------------------------- */

/**
 * @defgroup mmu
 * @ingroup kernel
 */

#ifndef _ARCH_MMU_H
#define _ARCH_MMU_H

#include "los_printf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup mmu
 * Mmu error code: Input virtual or physical addr is too large.
 *
 * Value: 0x02000d00
 *
 * Solution: Check parameter virtual and physical addr.
 */
#define LOS_ERRNO_MMU_INPUT_OVERFLOW        LOS_ERRNO_OS_ERROR(LOS_MOD_MMU, 0x00)

/**
 * @ingroup mmu
 * Mmu error code: Input size is zero.
 *
 * Value: 0x02000d01
 *
 * Solution: Check parameter size.
 */
#define LOS_ERRNO_MMU_INPUT_ZERO            LOS_ERRNO_OS_ERROR(LOS_MOD_MMU, 0x01)

/**
 * @ingroup mmu
 * Mmu error code: Input mmu flag is illegal.
 *
 * Value: 0x02000d02
 *
 * Solution: Check parameter flag/attribution.
 */
#define LOS_ERRNO_MMU_INPUT_FLAG_INV        LOS_ERRNO_OS_ERROR(LOS_MOD_MMU, 0x02)

/**
 * @ingroup mmu
 * Mmu error code: Mmu pagetable memory run out.
 *
 * Value: 0x03000d03
 *
 * Solution: Allocate more Mmu page talbe memory.
 */
#define LOS_ERRNO_MMU_PAGETABLE_OOM         LOS_ERRNO_OS_FATAL(LOS_MOD_MMU, 0x03)

/**
 * @ingroup mmu
 * Mmu error code: Mmu page table item has been mapped before.
 *
 * Value: 0x03000d04
 *
 * Solution: Unmap the page table, then map it again.
 */
#define LOS_ERRNO_MMU_PTE_BUSY              LOS_ERRNO_OS_FATAL(LOS_MOD_MMU, 0x04)

/**
 * @ingroup mmu
 * Mmu error code: Mmu regions number is not enough.
 *
 * Value: 0x02000d05
 *
 * Solution: Increase the number of Mmu regions.
 */
#define LOS_ERRNO_MMU_REGION_NOT_ENOUGH     LOS_ERRNO_OS_ERROR(LOS_MOD_MMU, 0x05)

/**
 * @ingroup mmu
 * Mmu error code: Mmu input region addr range is overlapped with ones before.
 *
 * Value: 0x02000d06
 *
 * Solution: Check the input parameter virtuall or physical addr.
 */
#define LOS_ERRNO_MMU_REGION_OVERLAPPED     LOS_ERRNO_OS_ERROR(LOS_MOD_MMU, 0x06)

/**
 * @ingroup mmu
 * Mmu error code: Mmu input usage addr is NULL.
 *
 * Value: 0x02000d07
 *
 * Solution: Check the input parameter of ArchMmuTableStatistics.
 */
#define LOS_ERRNO_MMU_INPUT_NULL            LOS_ERRNO_OS_ERROR(LOS_MOD_MMU, 0x07)

/**
 * @ingroup mmu
 * Mmu error code: mmu input virtual address is different from the physical address.
 *
 * Value: 0x02000d08
 *
 * Solution: Check the input parameter of ArchMmuMap.
 */
#define LOS_ERRNO_MMU_INPUT_ADDR_INV        LOS_ERRNO_OS_ERROR(LOS_MOD_MMU, 0x08)

/*
 * MAIR register support upto 8 types of mem attrs, so we reserve bit[0:2] here
 */
#define MMU_FLAGS_MEM_TYPE_MASK              7U         /* bit[0:2] */
#define MMU_FLAGS_MEM_TYPE(attr)             ((attr) & MT_TYPE_MASK)
#define MMU_FLAGS_DEVICE_STRONGLY_ORDERED    0U
#define MMU_FLAGS_DEVICE_EARLY               1U
#define MMU_FLAGS_NORMAL_WB                  2U
#define MMU_FLAGS_NORMAL_NC                  3U
#define MMU_FLAGS_NORMAL_WT                  4U

#define MMU_FLAGS_RO                    0U         /* bit[3] */
#define MMU_FLAGS_RW                    (1U << 3)
#define MMU_FLAGS_EXECUTE_ENABLE        0U         /* bit[4] */
#define MMU_FLAGS_EXECUTE_DISABLE       (1U << 4)
#define MMU_FLAGS_SECURE                0U         /* bit[5] */
#define MMU_FLAGS_NON_SECURE            (1U << 5)

#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
#define MMU_FLAGS_SECURE_DEFAULT_STATE  MMU_FLAGS_SECURE
#else
#define MMU_FLAGS_SECURE_DEFAULT_STATE  MMU_FLAGS_NON_SECURE
#endif

#define MMU_RW_NX_NORMAL_NC                 \
    (MMU_FLAGS_RW | MMU_FLAGS_EXECUTE_DISABLE | MMU_FLAGS_NORMAL_NC | MMU_FLAGS_SECURE_DEFAULT_STATE)
#define MMU_RO_X_NORMAL_WB                  \
    (MMU_FLAGS_RO | MMU_FLAGS_EXECUTE_ENABLE | MMU_FLAGS_NORMAL_WB | MMU_FLAGS_SECURE_DEFAULT_STATE)
#define MMU_RO_NX_NORMAL_WB                 \
    (MMU_FLAGS_RO | MMU_FLAGS_EXECUTE_DISABLE | MMU_FLAGS_NORMAL_WB | MMU_FLAGS_SECURE_DEFAULT_STATE)
#define MMU_RW_NX_NORMAL_WB                 \
    (MMU_FLAGS_RW | MMU_FLAGS_EXECUTE_DISABLE | MMU_FLAGS_NORMAL_WB | MMU_FLAGS_SECURE_DEFAULT_STATE)
#define MMU_RW_NX_DEVICE_MAP                \
    (MMU_FLAGS_RW | MMU_FLAGS_EXECUTE_DISABLE | MMU_FLAGS_DEVICE_EARLY | MMU_FLAGS_SECURE_DEFAULT_STATE)
#define MMU_RW_NX_DEVICE_STRONGLY_ORDERED   \
    (MMU_FLAGS_RW | MMU_FLAGS_EXECUTE_DISABLE | MMU_FLAGS_DEVICE_STRONGLY_ORDERED | MMU_FLAGS_SECURE_DEFAULT_STATE)

#define TABLE_BSS_SECTION  __attribute__((section(".bss.prebss.translation_table")))

extern CHAR __ram_vectors_vma;
/***************** Code area space *****************/
#define TEXT_START_VADDR   (UINTPTR)(&__ram_vectors_vma)
#define TEXT_START_PADDR   (UINTPTR)(&__ram_vectors_vma)
#define TEXT_SIZE          ((UINTPTR)(&__text_end) - TEXT_START_VADDR)

#define RODATA_START_VADDR (UINTPTR)(&__rodata_start)
#define RODATA_START_PADDR (UINTPTR)(&__rodata_start)
#define RODATA_SIZE        ((UINTPTR)(&__rodata_end) - RODATA_START_VADDR)

#define DATA_START_VADDR   (UINTPTR)(&__ram_data_start)
#define DATA_START_PADDR   (UINTPTR)(&__ram_data_start)
#define DATA_SIZE          (SYS_MEM_END - DATA_START_VADDR)

typedef struct {
    const CHAR *name;
    UINTPTR pAddr;      /* Physical address start value */
    UINTPTR vAddr;      /* Virtual address start value */
    UINTPTR size;       /* MMU memory size */
    UINT32 flags;       /* Memory attributes */
} MmuMapRegion;

typedef struct {
    UINT32 pageUsed;
    UINT32 pageTotal;
    UINT32 pteUsed;
    UINT32 pteTotal;
} PageTablesStat;

extern VOID ArchMmuLogLevelCfg(UINT32 level);
extern VOID ArchMmuConfig(VOID);
extern UINT32 ArchMmuRegionInsert(MmuMapRegion *rgn);
extern UINT32 ArchMmuTableStatistics(PageTablesStat *usage);
extern UINT32 ArchMmuMap(UINTPTR vAddr, UINTPTR pAddr, UINTPTR size, UINT32 attrs);
extern UINT32 ArchMmuUnmap(UINTPTR vAddr, UINTPTR size);
extern UINT32 ArchMmuTableWalk(UINTPTR vAddr, UINTPTR size);

extern VOID ArchMmuAppRegionReg(VOID);

extern UINTPTR *g_firstPageTable;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_MMU_H */
