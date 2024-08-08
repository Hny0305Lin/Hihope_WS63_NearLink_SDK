/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Basic definitions
 * Author: Huawei LiteOS Team
 * Create: 2022-07-01
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
 * @defgroup los_dynshm Dynload Share Memory Framework
 * @ingroup kernel
 */

#ifndef _LOS_DYN_SHM_H
#define _LOS_DYN_SHM_H

#include "los_typedef.h"
#include "los_errno.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup los_dynshm
 * Dynamic shared memory error code: The request for memory fails.
 *
 * Value: 0x02002300.
 *
 * Solution: Increase the size of virtual space.
 */
#define LOS_ERRNO_DYN_SHM_NO_MEMORY             LOS_ERRNO_OS_ERROR(LOS_MOD_DYN_SHM, 0x00)

/**
 * @ingroup los_dynshm
 * Dynamic shared memory error code: The input is invalid.
 *
 * Value: 0x02002301.
 *
 * Solution: Check the input.
 */
#define LOS_ERRNO_DYN_SHM_INPUT_INVALID         LOS_ERRNO_OS_ERROR(LOS_MOD_DYN_SHM, 0x01)

/**
 * @ingroup los_dynshm
 * Dynamic shared memory error code: initialization failure.
 *
 * Value: 0x02002302.
 *
 * Solution: Check whether the mutex is created successfully.
 */
#define LOS_ERRNO_DYN_SHM_INIT_FAILED           LOS_ERRNO_OS_ERROR(LOS_MOD_DYN_SHM, 0x02)

/**
 * @ingroup los_dynshm
 * Dynamic shared memory error code: The input addr is already mapped before.
 *
 * Value: 0x02002303.
 *
 * Solution: Check the addr, and unmap firstly, then map it again.
 */
#define LOS_ERRNO_DYN_SHM_ALREADY_MAPPED        LOS_ERRNO_OS_ERROR(LOS_MOD_DYN_SHM, 0x03)

/**
 * @ingroup los_dynshm
 * Dynamic shared memory error code: The module has been initialized more than once.
 *
 * Value: 0x02002304.
 *
 * Solution: Check your code and make sure LOS_DynShmInit is only called once.
 */
#define LOS_ERRNO_DYN_SHM_INIT_REPEATEDLY       LOS_ERRNO_OS_ERROR(LOS_MOD_DYN_SHM, 0x04)

/**
 * @ingroup los_dynshm
 * Dynamic shared memory error code: The API is called in ISR.
 *
 * Value: 0x02002305.
 *
 * Solution: Check your code and make sure the API is only called in ISR.
 */
#define LOS_ERRNO_DYN_SHM_IN_ISR                LOS_ERRNO_OS_ERROR(LOS_MOD_DYN_SHM, 0x05)

/**
 * @ingroup los_dynshm
 * Dynamic shared memory error code: The module has not been initialized well.
 *
 * Value: 0x02002306.
 *
 * Solution: Make sure APIs which starts with LOS_DynShm in the module are called
 * after DynShm has been initialized well.
 */
#define LOS_ERRNO_DYN_SHM_NOT_READY             LOS_ERRNO_OS_ERROR(LOS_MOD_DYN_SHM, 0x06)

typedef UINT64 UADDR;

#define SHM_CACHE_MASK 0b001
#define SHM_SEC_MASK   0b010
#define SHM_MMZ_MASK   0b100

#define LOS_SHM_IS_CACHE(x) ((UINT32)(x) & SHM_CACHE_MASK)
#define LOS_SHM_IS_SEC(x)   ((UINT32)(x) & SHM_SEC_MASK)
#define LOS_SHM_IS_MMZ(x)   ((UINT32)(x) & SHM_MMZ_MASK)

#define SHM_ALIGN_UP(val, align)      (((val) + (align) - 1) & (~((UADDR)(align) - 1)))
#define SHM_ALIGN_DN(val, align)      ((val) & (~((UADDR)(align) - 1)))
#define SHM_ALIGN_OFFSET(val, align)  ((val) & ((UADDR)(align) - 1))

typedef enum {
    LOS_VA_SPACE_NOSEC_NOCACHE  = 0b000,
    LOS_VA_SPACE_NOSEC_CACHE    = 0b001,
    LOS_VA_SPACE_SEC_NOCACHE    = 0b010,
    LOS_VA_SPACE_SEC_CACHE      = 0b011,
    LOS_VA_SPACE_TYPE_MAX       = 4,
    LOS_MMZ_SPACE_NOSEC_NOCACHE = 0b100,
    LOS_MMZ_SPACE_NOSEC_CACHE   = 0b101,
    LOS_MMZ_SPACE_SEC_NOCACHE   = 0b110,
    LOS_MMZ_SPACE_SEC_CACHE     = 0b111,
    LOS_SHM_TYPE_MAX,
} DynShmMemType;

typedef struct {
    UINT32 baseAddr;
    UINT32 tableSize;
} PageTableConfig;

typedef struct {
    PageTableConfig nonsec;
    PageTableConfig sec;
} SysPageTable;

typedef struct {
    UINT32 startAddr;
    UINT32 size;
    union {
        UINT32 type;
        UINT32 nextRgnIdx;
    };
} DynShmRegion;

typedef struct {
    UADDR dmaBuf;    /* It's output param when allocation, while input param when free */
    UINTPTR virAddr; /* It's output param when allocation, while input param when free */
    UADDR phyAddr;   /* It's input param whenever allocation or free */
    UINT32 size;     /* It's input param whenever allocation or free */
    UINT32 type;     /* It's input param whenever allocation or free */
    CHAR *name;
    BOOL noNeedMap;
} DynShmBulkMem;

typedef UINT32 (*DynShmFlushIoMmuTlbHandler)(BOOL flushCache);
typedef UINT32 (*DynShmBulkAllocHandler)(DynShmBulkMem *mem);
typedef UINT32 (*DynShmBulkFreeHandler)(DynShmBulkMem *mem);

typedef struct {
    SysPageTable outerPageTable;
    SysPageTable innerPageTable;
    UINT32 pageSize;

    DynShmFlushIoMmuTlbHandler flushTlbHandler;
    DynShmBulkAllocHandler bulkAllocHandler;
    DynShmBulkFreeHandler bulkFreeHandler;

    UINT8 regionCnt;
#define DYN_SHM_RGN_MAX 8
    DynShmRegion regions[DYN_SHM_RGN_MAX];
} DynShmConfig;

/**
 * @ingroup los_dynshm
 * @brief Init dynamic shared memory module.
 *
 * @par Description:
 * This API is used to initialize dynamic shared memory module.
 * @attention
 * <ul>
 * <li>Do not initialize dynshm module repeatedly.</li>
 * </ul>
 *
 * @param config   [IN] The user's configuration parameter.
 *
 * @retval #LOS_ERRNO_DYN_SHM_INPUT_INVALID  The passed-in config is invalid.
 * @retval #LOS_ERRNO_DYN_SHM_INIT_FAILED  The mutex is created failed.
 * @retval #LOS_OK   The shm module is successfully inited.
 * @par Dependency:
 * los_dynshm.h: the header file that contains the API declaration.
 */
extern UINT32 LOS_DynShmInit(DynShmConfig *config);

/**
 * @ingroup los_dynshm
 * @brief Setup Mapping from input address to output address.
 *
 * @par Description:
 * This API is used to Setup Mapping from input address to output address.
 * The input address is from another device, like iommu.
 * The output address is from LiteOS which is virtual.
 * @attention
 * <ul>
 * <li>Do not call the API in ISR.</li>
 * <li>If [inAddr, inAddr + size) is overlapped with mapped areas before, API will return failure.</li>
 * <li>If [inAddr, inAddr + size) is fully covered by mapped areas before, API will return OK.</li>
 * </ul>
 *
 * @param inAddr    [IN] The input address. 0 is legal parameter since it is not LiteOS virtual address.
 * @param size      [IN] The mapping area size.
 * @param memType   [IN] The type of mapping memory, including security and cache.
 * @param outAddr   [OUT] The address is virtual and managed by LiteOS. It must be 4-bytes aligned.
 *
 * @retval #LOS_ERRNO_DYN_SHM_INPUT_INVALID  The passed-in config is invalid.
 * @retval #LOS_ERRNO_DYN_SHM_NO_MEMORY  The virtual space is not enough.
 * @retval #LOS_ERRNO_DYN_SHM_ALREADY_MAPPED  The mapping is already setup, do not repeatedly map.
 * @retval #LOS_OK   The mapping is setup successfully.
 * @par Dependency:
 * los_dynshm.h: the header file that contains the API declaration.
 * @see LOS_DynShmUnmap
 */
extern UINT32 LOS_DynShmMap(UADDR inAddr, UINT32 size, DynShmMemType memType, UINTPTR *outAddr);

/**
 * @ingroup los_dynshm
 * @brief Destroy Mapping from input address to output address.
 *
 * @par Description:
 * This API is used to destroy Mapping from input address to output address.
 * The input address is from LiteOS which is virtual.
 * @attention
 * <ul>
 * <li>Do not call the API in ISR.</li>
 * </ul>
 *
 * @param virAddr  [IN] The input address. Zero address will return failure
 *                      since it cannot be found forever in virtual space.
 * @param size     [IN] The unmapping area size.
 *
 * @retval #LOS_ERRNO_DYN_SHM_INPUT_INVALID  The passed-in virAddr and size is invalid.
 * @retval #LOS_OK   The mapping is destroyed successfully.
 * @par Dependency:
 * los_dynshm.h: the header file that contains the API declaration.
 * @see LOS_DynShmMap
 */
extern UINT32 LOS_DynShmUnmap(UINTPTR virAddr, UINT32 size);

/*
 * INNER_VA means LiteOS running core's virtual address.
 * OUTER_VA means another device's virtual address, instead of LiteOS running core.
 * MEM_PA means real memory's physical address.
 */
typedef enum {
    INNER_VA_TO_OUTER_VA = 0,
    OUTER_VA_TO_INNER_VA,
    INNER_VA_TO_MEM_PA,
    OUTER_VA_TO_MEM_PA,
    SHM_ADDR_TRANS_TYPE_MAX,
} DynShmTransType;

/**
 * @ingroup los_dynshm
 * @brief Translate address from input to output.
 *
 * @par Description:
 * This API is used to translate address from input to output.
 * The translation has four directions, including INNER_VA_TO_OUTER_VA, OUTER_VA_TO_INNER_VA,
 * INNER_VA_TO_MEM_PA and OUTER_VA_TO_MEM_PA.
 * INNER_VA means LiteOS running core's virtual address.
 * OUTER_VA means another device's virtual address, instead of LiteOS running core.
 * MEM_PA means real memory's physical address.
 * @attention
 * <ul>
 * <li>Do not call the API in ISR.</li>
 * </ul>
 *
 * @param inAddr  [IN]  The input address.
 * @param outAddr [OUT] The virtual address corresponding to inAddr. It must be 8-bytes aligned.
 * @param optType [OUT] Translation direction.
 *
 * @retval #LOS_ERRNO_DYN_SHM_INPUT_INVALID  The passed-in inAddr and optType is invalid.
 * @retval #LOS_OK   The address translation is done successfully.
 * @par Dependency:
 * los_dynshm.h: the header file that contains the API declaration.
 */
extern UINT32 LOS_DynShmAddrTranslate(UADDR inAddr, UADDR *outAddr, DynShmTransType optType);

 /**
 * @ingroup los_dynshm
 * @brief Malloc memory in bulk.
 *
 * @par Description:
 * This API is used to malloc memory in bulk. Request for allocation of physical memory firstly,
 * and then mapping the range of physical address to virtual address by LiteOS.
 * @attention
 * <ul>
 * <li>Do not call the API in ISR.</li>
 * </ul>
 *
 * @param dmaBuf  [OUT] The allocated physical buffer by request.
 * @param virAddr [OUT] The mapped virtual address by LiteOS.
 * @param phyAddr [IN] The allocated physical address by request.
 * @param size    [IN] The size of allocated memory.
 * @param type    [IN] The type of allocated memory.
 *
 * @retval #NULL  The allocation is fail.
 * @retval #non-zero   The allocation is successful.
 * @par Dependency:
 * los_dynshm.h: the header file that contains the API declaration.
 * @see LOS_DynShmBulkFree
 */
extern VOID *LOS_DynShmBulkMalloc(DynShmBulkMem *mem);

 /**
 * @ingroup los_dynshm
 * @brief Malloc memory in bulk.
 *
 * @par Description:
 * This API is used to malloc memory in bulk. Request for allocation of physical memory firstly,
 * and then mapping the range of physical address to virtual address by LiteOS.
 * @attention
 * <ul>
 * <li>Do not call the API in ISR.</li>
 * </ul>
 *
 * @param dmaBuf  [IN] The allocated physical buffer by request.
 * @param virAddr [IN] The mapped virtual address by LiteOS.
 * @param phyAddr [IN] The allocated physical address by request.
 * @param size    [IN] The size of allocated memory.
 * @param type    [IN] The type of allocated memory.
 *
 * @retval #LOS_ERRNO_DYN_SHM_INPUT_INVALID  The passed-in virAddr and size is invalid.
 * @retval #LOS_OK   The mapping is destroyed successfully.
 * @par Dependency:
 * los_dynshm.h: the header file that contains the API declaration.
 * @see LOS_DynShmBulkMalloc
 */
extern UINT32 LOS_DynShmBulkFree(DynShmBulkMem *mem);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_DYN_SHM_H */
