/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : Intrinsics HeadFile
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
 * --------------------------------------------------------------------------- */

#ifndef _ARCH_INTRINSICS_H
#define _ARCH_INTRINSICS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PUSH_GPR_REG    __attribute__((context_push("gpr")))
#define POP_GPR_REG     __attribute__((context_pop("gpr")))
#define MRET            __attribute__((mret))
#define NORELAX         __attribute__((norelaxvar))

#define RISCV_GET_TASK_STACK()   __builtin_get_context_base()
#define RISCV_CHG_TASK_STACK(a)  __builtin_chg_context_base(a)

#define RISCV_ADDI_FROM_GP       __builtin_linx_addi_from_gp
#define RISCV_ADDI_FROM_RA       __builtin_linx_addi_from_ra
#define RISCV_ADDI_FROM_SP       __builtin_linx_addi_from_sp
#define RISCV_ADDI_TO_GP         __builtin_linx_addi_to_gp
#define RISCV_ADDI_TO_RA         __builtin_linx_addi_to_ra
#define RISCV_ADDI_TO_SP         __builtin_linx_addi_to_sp

#define RISCV_ANDI               __builtin_linx_andi

#define RISCV_CSRRWI_MSTATUS(a)  __builtin_linx_csrrwi(0b001100000000, a)
#define RISCV_CSRRWI_MTVEC(a)    __builtin_linx_csrrwi(0b001100000101, a)

#define RISCV_CSRRC_MSCRATCH(a)  __builtin_linx_csrrc(0b001101000000, a)
#define RISCV_CSRRC_MEPC(a)      __builtin_linx_csrrc(0b001101000001, a)
#define RISCV_CSRRC_MCAUSE(a)    __builtin_linx_csrrc(0b001101000010, a)
#define RISCV_CSRRC_MTVAL(a)     __builtin_linx_csrrc(0b001101000011, a)
#define RISCV_CSRRC_MSTATUS(a)   __builtin_linx_csrrc(0b001100000000, a)
#define RISCV_CSRRC_MIE(a)       __builtin_linx_csrrc(0b001100000100, a)
#define RISCV_CSRRC_MTVEC(a)     __builtin_linx_csrrc(0b001100000101, a)

#define RISCV_CSRRCI_MSCRATCH(a) __builtin_linx_csrrci(0b001101000000, a)
#define RISCV_CSRRCI_MEPC(a)     __builtin_linx_csrrci(0b001101000001, a)
#define RISCV_CSRRCI_MCAUSE(a)   __builtin_linx_csrrci(0b001101000010, a)
#define RISCV_CSRRCI_MTVAL(a)    __builtin_linx_csrrci(0b001101000011, a)
#define RISCV_CSRRCI_MSTATUS(a)  __builtin_linx_csrrci(0b001100000000, a)
#define RISCV_CSRRCI_MIE(a)      __builtin_linx_csrrci(0b001100000100, a)
#define RISCV_CSRRCI_MTVEC(a)    __builtin_linx_csrrci(0b001100000101, a)

#define RISCV_CSRRS_MSCRATCH(a)  __builtin_linx_csrrs(0b001101000000, a)
#define RISCV_CSRRS_MEPC(a)      __builtin_linx_csrrs(0b001101000001, a)
#define RISCV_CSRRS_MCAUSE(a)    __builtin_linx_csrrs(0b001101000010, a)
#define RISCV_CSRRS_MTVAL(a)     __builtin_linx_csrrs(0b001101000011, a)
#define RISCV_CSRRS_MSTATUS(a)   __builtin_linx_csrrs(0b001100000000, a)
#define RISCV_CSRRS_MIE(a)       __builtin_linx_csrrs(0b001100000100, a)
#define RISCV_CSRRS_MTVEC(a)     __builtin_linx_csrrs(0b001100000101, a)

#define RISCV_CSRRSI_MSCRATCH(a) __builtin_linx_csrrsi(0b001101000000, a)
#define RISCV_CSRRSI_MEPC(a)     __builtin_linx_csrrsi(0b001101000001, a)
#define RISCV_CSRRSI_MCAUSE(a)   __builtin_linx_csrrsi(0b001101000010, a)
#define RISCV_CSRRSI_MTVAL(a)    __builtin_linx_csrrsi(0b001101000011, a)
#define RISCV_CSRRSI_MSTATUS(a)  __builtin_linx_csrrsi(0b001100000000, a)
#define RISCV_CSRRSI_MIE(a)      __builtin_linx_csrrsi(0b001100000100, a)
#define RISCV_CSRRSI_MTVEC(a)    __builtin_linx_csrrsi(0b001100000101, a)

#define RISCV_CSRRW_MSCRATCH(a)  __builtin_linx_csrrw(0b001101000000, a)

#define RISCV_CSRRW_MEPC(a)      __builtin_linx_csrrw(0b001101000001, a)
#define RISCV_CSRRW_MEPC_MEPC_0(a) \
    (unsigned int)(RISCV_CSRRW_MEPC((RISCV_CSRRS_MEPC(0) & ~1) | (a << 0)) & 1) >> 0
#define RISCV_CSRRW_MEPC_MEPC_MEPC(a) \
    (unsigned int)(RISCV_CSRRW_MEPC((RISCV_CSRRS_MEPC(0) & ~4294967294) | (a << 1)) & 4294967294) >> 1

#define RISCV_CSRRW_MCAUSE(a)    __builtin_linx_csrrw(0b001101000010, a)
#define RISCV_CSRRW_MCAUSE_MCAUSE_EXCCODE(a) \
    (unsigned int)(RISCV_CSRRW_MCAUSE((RISCV_CSRRS_MCAUSE(0) & ~4095) | (a << 0)) & 4095) >> 0
#define RISCV_CSRRW_MCAUSE_MCAUSE_UOPCNT3TO0(a) \
    (unsigned int)(RISCV_CSRRW_MCAUSE((RISCV_CSRRS_MCAUSE(0) & ~61440) | (a << 12)) & 61440) >> 12
#define RISCV_CSRRW_MCAUSE_MCAUSE_MPIL(a) \
    (unsigned int)(RISCV_CSRRW_MCAUSE((RISCV_CSRRS_MCAUSE(0) & ~16711680) | (a << 16)) & 16711680) >> 16
#define RISCV_CSRRW_MCAUSE_MCAUSE_UOPCNT4(a) \
    (unsigned int)(RISCV_CSRRW_MCAUSE((RISCV_CSRRS_MCAUSE(0) & ~16777216) | (a << 24)) & 16777216) >> 24
#define RISCV_CSRRW_MCAUSE_MCAUSE_RESERVED0(a) \
    (unsigned int)(RISCV_CSRRW_MCAUSE((RISCV_CSRRS_MCAUSE(0) & ~100663296) | (a << 25)) & 100663296) >> 25
#define RISCV_CSRRW_MCAUSE_MCAUSE_MPIE(a) \
    (unsigned int)(RISCV_CSRRW_MCAUSE((RISCV_CSRRS_MCAUSE(0) & ~134217728) | (a << 27)) & 134217728) >> 27
#define RISCV_CSRRW_MCAUSE_MCAUSE_MPP(a) \
    (unsigned int)(RISCV_CSRRW_MCAUSE((RISCV_CSRRS_MCAUSE(0) & ~805306368) | (a << 28)) & 805306368) >> 28
#define RISCV_CSRRW_MCAUSE_MCAUSE_MINHV(a) \
    (unsigned int)(RISCV_CSRRW_MCAUSE((RISCV_CSRRS_MCAUSE(0) & ~1073741824) | (a << 30)) & 1073741824) >> 30
#define RISCV_CSRRW_MCAUSE_MCAUSE_INTERRUPT(a) \
    (unsigned int)(RISCV_CSRRW_MCAUSE((RISCV_CSRRS_MCAUSE(0) & ~2147483648) | (a << 31)) & 2147483648) >> 31

#define RISCV_CSRRW_MTVAL(a)     __builtin_linx_csrrw(0b001101000011, a)

#define RISCV_CSRRW_MSTATUS(a)   __builtin_linx_csrrw(0b001100000000, a)
#define RISCV_CSRRW_MSTATUS_MSTATUS_UIE(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~1) | (a << 0)) & 1) >> 0
#define RISCV_CSRRW_MSTATUS_MSTATUS_SIE(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~2) | (a << 1)) & 2) >> 1
#define RISCV_CSRRW_MSTATUS_MSTATUS_RESERVED0(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~4) | (a << 2)) & 4) >> 2
#define RISCV_CSRRW_MSTATUS_MSTATUS_MIE(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~8) | (a << 3)) & 8) >> 3
#define RISCV_CSRRW_MSTATUS_MSTATUS_UPIE(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~16) | (a << 4)) & 16) >> 4
#define RISCV_CSRRW_MSTATUS_MSTATUS_SPIE(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~32) | (a << 5)) & 32) >> 5
#define RISCV_CSRRW_MSTATUS_MSTATUS_RESERVED1(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~64) | (a << 6)) & 64) >> 6
#define RISCV_CSRRW_MSTATUS_MSTATUS_MPIE(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~128) | (a << 7)) & 128) >> 7
#define RISCV_CSRRW_MSTATUS_MSTATUS_SPP(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~256) | (a << 8)) & 256) >> 8
#define RISCV_CSRRW_MSTATUS_MSTATUS_VS(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~1536) | (a << 9)) & 1536) >> 9
#define RISCV_CSRRW_MSTATUS_MSTATUS_MPP(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~6144) | (a << 11)) & 6144) >> 11
#define RISCV_CSRRW_MSTATUS_MSTATUS_FS(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~24576) | (a << 13)) & 24576) >> 13
#define RISCV_CSRRW_MSTATUS_MSTATUS_XS(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~98304) | (a << 15)) & 98304) >> 15
#define RISCV_CSRRW_MSTATUS_MSTATUS_MPRV(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~131072) | (a << 17)) & 131072) >> 17
#define RISCV_CSRRW_MSTATUS_MSTATUS_SUM(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~262144) | (a << 18)) & 262144) >> 18
#define RISCV_CSRRW_MSTATUS_MSTATUS_MXR(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~524288) | (a << 19)) & 524288) >> 19
#define RISCV_CSRRW_MSTATUS_MSTATUS_TVM(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~1048576) | (a << 20)) & 1048576) >> 20
#define RISCV_CSRRW_MSTATUS_MSTATUS_TW(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~2097152) | (a << 21)) & 2097152) >> 21
#define RISCV_CSRRW_MSTATUS_MSTATUS_TSR(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~4194304) | (a << 22)) & 4194304) >> 22
#define RISCV_CSRRW_MSTATUS_MSTATUS_RESERVED2(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~2139095040) | (a << 23)) & 2139095040) >> 23
#define RISCV_CSRRW_MSTATUS_MSTATUS_SD(a) \
    (unsigned int)(RISCV_CSRRW_MSTATUS((RISCV_CSRRS_MSTATUS(0) & ~2147483648) | (a << 31)) & 2147483648) >> 31

#define RISCV_CSRRW_MIE(a)       __builtin_linx_csrrw(0b001100000100, a)
#define RISCV_CSRRW_MIE_MIE_USIE(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~1) | (a << 0)) & 1) >> 0
#define RISCV_CSRRW_MIE_MIE_SSIE(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~2) | (a << 1)) & 2) >> 1
#define RISCV_CSRRW_MIE_MIE_RESERVED0(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~4) | (a << 2)) & 4) >> 2
#define RISCV_CSRRW_MIE_MIE_MSIE(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~8) | (a << 3)) & 8) >> 3
#define RISCV_CSRRW_MIE_MIE_UTIE(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~16) | (a << 4)) & 16) >> 4
#define RISCV_CSRRW_MIE_MIE_STIE(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~32) | (a << 5)) & 32) >> 5
#define RISCV_CSRRW_MIE_MIE_RESERVED1(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~64) | (a << 6)) & 64) >> 6
#define RISCV_CSRRW_MIE_MIE_MTIE(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~128) | (a << 7)) & 128) >> 7
#define RISCV_CSRRW_MIE_MIE_UEIE(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~256) | (a << 8)) & 256) >> 8
#define RISCV_CSRRW_MIE_MIE_SEIE(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~512) | (a << 9)) & 512) >> 9
#define RISCV_CSRRW_MIE_MIE_RESERVED2(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~1024) | (a << 10)) & 1024) >> 10
#define RISCV_CSRRW_MIE_MIE_MEIE(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~2048) | (a << 11)) & 2048) >> 11
#define RISCV_CSRRW_MIE_MIE_RESERVED3(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~61440) | (a << 12)) & 61440) >> 12
#define RISCV_CSRRW_MIE_MIE_ULIE(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~65536) | (a << 16)) & 65536) >> 16
#define RISCV_CSRRW_MIE_MIE_SLIE(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~131072) | (a << 17)) & 131072) >> 17
#define RISCV_CSRRW_MIE_MIE_RESERVED4(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~262144) | (a << 18)) & 262144) >> 18
#define RISCV_CSRRW_MIE_MIE_MLIE(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~524288) | (a << 19)) & 524288) >> 19
#define RISCV_CSRRW_MIE_MIE_RESERVED5(a) \
    (unsigned int)(RISCV_CSRRW_MIE((RISCV_CSRRS_MIE(0) & ~4293918720) | (a << 20)) & 4293918720) >> 20

#define RISCV_CSRRW_MTVEC(a)     __builtin_linx_csrrw(0b001100000101, a)
#define RISCV_CSRRW_MTVEC_MTVEC_MODE(a) \
    (unsigned int)(RISCV_CSRRW_MTVEC((RISCV_CSRRS_MTVEC(0) & ~63) | (a << 0)) & 63) >> 0
#define RISCV_CSRRW_MTVEC_MTVEC_BASE(a) \
    (unsigned int)(RISCV_CSRRW_MTVEC((RISCV_CSRRS_MTVEC(0) & ~4294967232) | (a << 6)) & 4294967232) >> 6

#define RISCV_CSRRWI_MSTATUS(a)  __builtin_linx_csrrwi(0b001100000000, a)
#define RISCV_CSRRWI_MIE(a)      __builtin_linx_csrrwi(0b001100000100, a)
#define RISCV_CSRRWI_MTVEC(a)    __builtin_linx_csrrwi(0b001100000101, a)

#define RISCV_GET_MEPC()      RISCV_CSRRS_MEPC(0)
#define RISCV_SET_MEPC(a)     RISCV_CSRRW_MEPC(a)
#define RISCV_GET_MSCRATCH()  RISCV_CSRRS_MSCRATCH(0)
#define RISCV_SET_MSCRATCH(a) RISCV_CSRRW_MSCRATCH(a)
#define RISCV_GET_MTVAL()     RISCV_CSRRS_MTVAL(0)
#define RISCV_SET_MTVAL(a)    RISCV_CSRRW_MTVAL(a)
#define RISCV_GET_MIE()       RISCV_CSRRS_MIE(0)
#define RISCV_SET_MIE(a)      RISCV_CSRRW_MIE(a)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_INTRINSICS_H */
