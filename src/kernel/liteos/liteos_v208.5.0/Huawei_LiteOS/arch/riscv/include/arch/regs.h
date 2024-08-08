/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : CPU Register Defines Headfile
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

#ifndef _ARCH_REGS_H
#define _ARCH_REGS_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UINT64_SHIFT_32_BIT             32

#define ASM_STR(x) #x

#define READ_CSR(csrReg) ({ UINT32 tmp_;                                        \
    asm volatile ("csrr %0, " ASM_STR(csrReg) : "=r"(tmp_));                    \
    tmp_;})                                                                     \

#define WRITE_CSR(csrReg, csrVal) do {                                          \
    if (__builtin_constant_p(csrVal) && (UINT32)(csrVal) < 32) {                \
        asm volatile ("csrw " ASM_STR(csrReg) ", %0" :: "i"(csrVal));           \
    } else {                                                                    \
        asm volatile ("csrw " ASM_STR(csrReg) ", %0" :: "r"(csrVal));           \
    }                                                                           \
} while (0)

#define SET_CSR(csrReg, csrBit) ({ UINT32 tmp_;                                 \
    if (__builtin_constant_p(csrBit) && (UINT32)(csrBit) < 32) {                \
        asm volatile ("csrrs %0, " #csrReg ", %1" : "=r"(tmp_) : "i"(csrBit));  \
    } else {                                                                    \
        asm volatile ("csrrs %0, " #csrReg ", %1" : "=r"(tmp_) : "r"(csrBit));  \
    }                                                                           \
    tmp_;})

#define CLEAR_CSR(csrReg, csrBit) ({ UINT32 tmp_;                               \
    if (__builtin_constant_p(csrBit) && (UINT32)(csrBit) < 32) {                \
        asm volatile ("csrrc %0, " #csrReg ", %1" : "=r"(tmp_) : "i"(csrBit));  \
    } else {                                                                    \
        asm volatile ("csrrc %0, " #csrReg ", %1" : "=r"(tmp_) : "r"(csrBit));  \
    }                                                                           \
    tmp_; })

#define READ_CUSTOM_CSR(csrReg) ({ UINT32 tmp_;                  \
    asm volatile ("csrr %0, %1" : "=r"(tmp_) : "i"(csrReg));     \
    tmp_; })

#define WRITE_CUSTOM_CSR_VAL(csrRegAddr, csrVal) do {            \
    if (__builtin_constant_p(csrVal))  {                         \
        asm volatile("li t0," "%0" : : "i"(csrVal));             \
    } else {                                                     \
        asm volatile("mv t0," "%0" : : "r"(csrVal));             \
    }                                                            \
    asm volatile("csrw %0, t0" :: "i"(csrRegAddr));              \
} while (0)

#define SET_CUSTOM_CSR(csrRegAddr, csrBit) do {                  \
    if (__builtin_constant_p(csrBit) && (UINT32)(csrBit) < 32) { \
        asm volatile("li t0," "%0" : : "i"(csrBit));             \
    } else {                                                     \
        asm volatile("mv t0," "%0" : : "r"(csrBit));             \
    }                                                            \
    asm volatile("csrs %0, t0" :: "i"(csrRegAddr));              \
} while (0)

#define CLEAR_CUSTOM_CSR(csrRegAddr, csrBit) do {                \
    if (__builtin_constant_p(csrBit) && (UINT32)(csrBit) < 32) { \
        asm volatile("li t0," "%0" : : "i"(csrBit));             \
    } else {                                                     \
        asm volatile("mv t0," "%0" : : "r"(csrBit));             \
    }                                                            \
    asm volatile("csrc %0, t0" :: "i"(csrRegAddr));              \
} while (0)

#define READ_CSR32_BIT64(addrl, addrh, val) do {                 \
    UINT32 valueLo, valueHi, valueHiTmp;                         \
    valueHiTmp = READ_CSR(addrh);                                \
    valueLo = READ_CSR(addrl);                                   \
    valueHi = READ_CSR(addrh);                                   \
    if (valueHi != valueHiTmp) {                                 \
        valueLo = READ_CSR(addrl);                               \
    }                                                            \
    val = valueLo + (((UINT64)valueHi) << UINT64_SHIFT_32_BIT);  \
} while (0)

#define WRITE_CSR32_BIT64(addrl, addrh, val) do {                \
    WRITE_CSR(addrl, (val) & UINT32_MAX);                        \
    WRITE_CSR(addrh, ((UINT64)(val) >> 32) & UINT32_MAX);        \
} while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_CACHE_H */
