/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Arm VIC Interrupt Register HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2021-09-01
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

#ifndef _MONO_VIC_H
#define _MONO_VIC_H

#include "los_hwi.h"
#include "asm/platform.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IRQSRC_WORD_L           31
#define IRQ_BASE                0x10140000
#define IRQ_INTEN_L             IRQ_BASE
#define IRQ_INTEN_H             0x10140004
#define IRQ_INTMASK_L           0x10140008
#define IRQ_INTMASK_H           0x1014000C
#define IRQ_INTFORCE_L          0x10140010
#define IRQ_INTFORCE_H          0x10140014
#define IRQ_RAWSTATUS_L         0x10140018
#define IRQ_STATUS_L            0x10140020
#define IRQ_MASKSTATUS_L        0x10140028
#define IRQ_FINALSTATUS_L       0x10140030
#define IRQ_FINALSTATUS_H       0x10140034
#define IRQ_PLEVEL              0x101400D8

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MONO_VIC_H */
