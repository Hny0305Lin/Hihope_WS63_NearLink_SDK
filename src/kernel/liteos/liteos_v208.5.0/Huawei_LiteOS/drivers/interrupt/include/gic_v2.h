/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: General interrupt controller version 2.0 (GICv2).
 * Author: Huawei LiteOS Team
 * Create: 2022-05-19
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

#ifndef _GIC_V2_H
#define _GIC_V2_H

#include "asm/platform.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GICC_CTLR   (GICC_OFFSET + 0x00)    /* CPU Interface Control Register */
#define GICC_PMR    (GICC_OFFSET + 0x04)    /* Interrupt Priority Mask Register */
#define GICC_BPR    (GICC_OFFSET + 0x08)    /* Binary Point Register */
#define GICC_IAR    (GICC_OFFSET + 0x0c)    /* Interrupt Acknowledge Register */
#define GICC_EOIR   (GICC_OFFSET + 0x10)    /* End of Interrupt Register */
#define GICC_RPR    (GICC_OFFSET + 0x14)    /* Running Priority Register */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _GIC_V2_H */

