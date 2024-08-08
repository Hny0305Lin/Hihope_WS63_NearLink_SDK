/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: RISCV32 Aslr HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-12-20
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

#ifndef _ARCH_ASLR_H
#define _ARCH_ASLR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 *           image
 * ┌──────────────────────────┐  ────►   __image_start
 * │                          │
 * │  ┌────────────────────┐  │  ────►   __laslr_start
 * │  │    aslr image      │  │
 * │  │   .laslr.init      │  │
 * │  │   .laslr.text      │  │
 * │  │   .laslr.data      │  │
 * │  └────────────────────┘  │  ────►   __laslr_end
 * │                          │
 * │  ┌────────────────────┐  │  ────►   __startup_start
 * │  │    kernel_startup  │  │
 * │  │                    │  │
 * │  │    main()          │  │
 * │  │    func ...        │  │
 * │  └────────────────────┘  │
 * │                          │
 * │  ┌────────────────────┐  │  ────►  __laslr_rel_start(reloc info for GSPR)
 * │  │   .laslr_rel       │  │
 * │  │    g_aslrTableInfo │  │
 * │  │    g_aslrRelInfo   │  │
 * │  └────────────────────┘  │  ────►   __laslr_rel_end
 * │                          │
 * └──────────────────────────┘  ────►   __image_end
 */

extern UINTPTR ArchAslrGetImageLinkInfo(UINTPTR offset);
extern UINTPTR ArchAslrGetCB(UINTPTR offset);
extern UINT32 ArchAslrRelocImp(const LD_ELF_RELA *rela, const LDRelocParam *relocParam);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_ASLR_H */
