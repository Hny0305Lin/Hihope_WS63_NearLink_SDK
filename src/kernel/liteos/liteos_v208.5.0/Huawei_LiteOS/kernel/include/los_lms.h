/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: Lite Memory Sanitizer HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2020-09-01
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
 * @defgroup los_lms Lite Memory Sanitizer
 * @ingroup kernel
 */

#ifndef _LOS_LMS_H
#define _LOS_LMS_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_KERNEL_LMS

/**
 * @ingroup los_lms
 * @brief Enable a pool check.
 *
 * @par Description:
 * This API is used to add the pool to lms check pools. When the address is within the range of this pool, lms will
 * check validity.
 * @attention
 * <ul>
 * <li>The size parameter value should be less than or equal to the Memory pool size.</li>
 * <li>This API is called by LOS_MemInit by default.</li>
 * </ul>
 *
 * @param pool               [IN] Starting address of memory.
 * @param size               [IN] Memory size.
 *
 * @retval #UINT32    The remaining pool free size after allocating shadow memory for lms.
 * @par Dependency:
 * <ul>
 * <li>los_lms.h: the header file that contains the API declaration.</li>
 * </ul>
 * @since Huawei LiteOS 206.1.0
 */
UINT32 LOS_LmsAddCheckPool(const VOID *pool, UINT32 size);

/**
 * @ingroup los_lms
 * @brief Disable a pool check.
 *
 * @par Description:
 * This API is used to delete a pool from lms check pools. When the address is within the range of this pool, lms will
 * not check validity.
 *
 * @param pool               [IN] Starting address of memory.
 *
 * @retval None.
 * @par Dependency:
 * <ul>
 * <li>los_lms.h: the header file that contains the API declaration.</li>
 * </ul>
 * @since Huawei LiteOS 206.1.0
 */
VOID LOS_LmsDelCheckPool(const VOID *pool);

/**
 * @ingroup los_lms
 * @brief Protect an region.
 *
 * @par Description:
 * This API is used to prohibit access to the area from addrStart to addrEnd.
 *
 * @param addrStart          [IN] Start address of memory.
 * @param addrEnd            [IN] End address of memory.
 *
 * @retval None.
 * @par Dependency:
 * <ul>
 * <li>los_lms.h: the header file that contains the API declaration.</li>
 * </ul>
 * @since Huawei LiteOS 206.1.0
 */
VOID LOS_LmsAddrProtect(UINTPTR addrStart, UINTPTR addrEnd);

/**
 * @ingroup los_lms
 * @brief Not protect an region.
 *
 * @par Description:
 * This API is used to allow access to the area from addrStart to addrEnd.
 *
 * @param addrStart          [IN] Start address of memory.
 * @param addrEnd            [IN] End address of memory.
 *
 * @retval None.
 * @par Dependency:
 * <ul>
 * <li>los_lms.h: the header file that contains the API declaration.</li>
 * </ul>
 * @since Huawei LiteOS 206.1.0
 */
VOID LOS_LmsAddrDisableProtect(UINTPTR addrStart, UINTPTR addrEnd);

#endif /* LOSCFG_KERNEL_LMS */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_LMS_H */
