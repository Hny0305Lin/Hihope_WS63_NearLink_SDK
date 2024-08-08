/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: IO Memory HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2023-02-06
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
 * @defgroup los_io IO Memory
 * @ingroup kernel
 */

#ifndef _LOS_IO_H
#define _LOS_IO_H

#include "arch/io.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define WRITE8(addr, value)             ArchWrite8((UINTPTR)(addr), value)

#define WRITE16(addr, value)            ArchWrite16((UINTPTR)(addr), value)

#define WRITE32(addr, value)            ArchWrite32((UINTPTR)(addr), value)

#define WRITE64(addr, value)            ArchWrite64((UINTPTR)(addr), value)

#define WRITE8_MB(addr, value)          { dsb(); WRITE8((UINTPTR)(addr), value); }

#define WRITE16_MB(addr, value)         { dsb(); WRITE16((UINTPTR)(addr), value); }

#define WRITE32_MB(addr, value)         { dsb(); WRITE32((UINTPTR)(addr), value); }

#define WRITE64_MB(addr, value)         { dsb(); WRITE64((UINTPTR)(addr), value); }

#define READ8(addr)                     ArchRead8((UINTPTR)(addr))

#define READ16(addr)                    ArchRead16((UINTPTR)(addr))

#define READ32(addr)                    ArchRead32((UINTPTR)(addr))

#define READ64(addr)                    ArchRead64((UINTPTR)(addr))

#define READ8_MB(addr)                  ({ UINT8 r = ArchRead8((UINTPTR)(addr)); dsb(); r; })

#define READ16_MB(addr)                 ({ UINT16 r = ArchRead16((UINTPTR)(addr)); dsb(); r; })

#define READ32_MB(addr)                 ({ UINT32 r = ArchRead32((UINTPTR)(addr)); dsb(); r; })

#define READ64_MB(addr)                 ({ UINT64 r = ArchRead64((UINTPTR)(addr)); dsb(); r; })

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_IO_H */
