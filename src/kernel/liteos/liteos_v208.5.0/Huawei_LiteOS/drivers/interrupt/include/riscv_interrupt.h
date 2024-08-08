/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Riscv Himideerv200 Interrupt Register HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2021-02-02
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

/* ----------------------------------------------------------------------------
 * This file has been modified in accordance with Misra-C specifications.
 * ---------------------------------------------------------------------------- */
#ifndef RISCV_INTERRUPT_H
#define RISCV_INTERRUPT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* local interrupt Register */
#define LOCIPRI0                0xBC0
#define LOCIPRI1                0xBC1
#define LOCIPRI2                0xBC2
#define LOCIPRI3                0xBC3
#define LOCIPRI4                0xBC4
#define LOCIPRI5                0xBC5
#define LOCIPRI6                0xBC6
#define LOCIPRI7                0xBC7
#define LOCIPRI8                0xBC8
#define LOCIPRI9                0xBC9
#define LOCIPRI10               0xBCA
#define LOCIPRI11               0xBCB
#define LOCIPRI12               0xBCC
#define LOCIPRI13               0xBCD
#define LOCIPRI14               0xBCE
#define LOCIPRI15               0xBCF

#define LOCIEN0                 0xBE0
#define LOCIEN1                 0xBE1
#define LOCIEN2                 0xBE2
#define LOCIEN3                 0xBE3
#define LOCIPD0                 0xBE8
#define LOCIPD1                 0xBE9
#define LOCIPD2                 0xBEA
#define LOCIPD3                 0xBEB
#define LOCIPD4                 0xBEC
#define LOCIPD5                 0xBED
#define LOCIPD6                 0xBEE

#define LOCIPRI_REG0            0
#define LOCIPRI_REG1            1
#define LOCIPRI_REG2            2
#define LOCIPRI_REG3            3
#define LOCIPRI_REG4            4
#define LOCIPRI_REG5            5
#define LOCIPRI_REG6            6
#define LOCIPRI_REG7            7
#define LOCIPRI_REG8            8
#define LOCIPRI_REG9            9
#define LOCIPRI_REG10           10
#define LOCIPRI_REG11           11
#define LOCIPRI_REG12           12
#define LOCIPRI_REG13           13
#define LOCIPRI_REG14           14
#define LOCIPRI_REG15           15

#define LOCIEN_REG0             0
#define LOCIEN_REG1             1
#define LOCIEN_REG2             2
#define LOCIEN_REG3             3
#define LOCIEN_REG4             4

#define LOCIPD_REG0             0
#define LOCIPD_REG1             1
#define LOCIPD_REG2             2
#define LOCIPD_REG3             3
#define LOCIPD_REG4             4
#define LOCIPD_REG5             5
#define LOCIPD_REG6             6

/* local interrupt pending clear register */
#define LOCIPCLR                 0xBF0

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RISCV_INTERRUPT_H */
