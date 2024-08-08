/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: General interrupt controller version 3.0 (GICv3).
 * Author: Huawei LiteOS Team
 * Create: 2018-09-15
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

#ifndef _GIC_V3_H
#define _GIC_V3_H

#include "stdint.h"
#include "asm/platform.h"
#include "arch/cpu.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BIT_32(bit)     (1u << bit)
#define BIT_64(bit)     (1ul << bit)

#ifdef LOSCFG_ARCH_ARM_AARCH64

#define ICC_CTLR_EL1    "S3_0_C12_C12_4"
#define ICC_PMR_EL1     "S3_0_C4_C6_0"
#define ICC_IAR1_EL1    "S3_0_C12_C12_0"
#define ICC_SRE_EL1     "S3_0_C12_C12_5"
#define ICC_BPR0_EL1    "S3_0_C12_C8_3"
#define ICC_BPR1_EL1    "S3_0_C12_C12_3"
#define ICC_IGRPEN0_EL1 "S3_0_C12_C12_6"
#define ICC_IGRPEN1_EL1 "S3_0_C12_C12_7"
#define ICC_EOIR1_EL1   "S3_0_C12_C12_1"
#define ICC_SGI1R_EL1   "S3_0_C12_C11_5"
#define ICC_EOIR0_EL1   "S3_0_c12_c8_1"
#define ICC_IAR0_EL1    "S3_0_C12_C8_0"

#define ICC_CTLR_EL3    "S3_6_C12_C12_4"
#define ICC_SRE_EL3     "S3_6_C12_C12_5"
#define ICC_IGRPEN1_EL3 "S3_6_C12_C12_7"

#else /* LOSCFG_ARCH_ARM_AARCH32 */

#define ICC_CTLR_EL1    "c12, c12, 4"
#define ICC_PMR_EL1     "c4, c6, 0"
#define ICC_IAR1_EL1    "c12, c12, 0"
#define ICC_SRE_EL1     "c12, c12, 5"
#define ICC_BPR1_EL1    "c12, c12, 3"
#define ICC_IGRPEN1_EL1 "c12, c12, 7"
#define ICC_EOIR1_EL1   "c12, c12, 1"
#define ICC_SGI1R_EL1   "c12"

#endif

/* GICD_CTLR bit definitions */
#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
#define CTLR_ENALBE_G0      BIT_32(0)
#define CTLR_ENABLE_G1NS    BIT_32(1)
#define CTLR_ENABLE_G1S     BIT_32(2)
#define CTLR_RES0           BIT_32(3)
#define CTLR_ARE_S          BIT_32(4)
#define CTLR_ARE_NS         BIT_32(5)
#define CTLR_DS             BIT_32(6)
#define CTLR_E1NWF          BIT_32(7)
#define GICD_CTLR_RWP       BIT_32(31)
#else
#define CTLR_ENALBE_G1      BIT_32(0)
#define CTLR_ENABLE_G1A     BIT_32(1)
#define CTLR_ARE_NS         BIT_32(4)
#define GICD_CTLR_RWP       BIT_32(31)
#endif

/* peripheral identification registers */
#define GICD_CIDR0 (GICD_OFFSET + 0xfff0)
#define GICD_CIDR1 (GICD_OFFSET + 0xfff4)
#define GICD_CIDR2 (GICD_OFFSET + 0xfff8)
#define GICD_CIDR3 (GICD_OFFSET + 0xfffc)
#define GICD_PIDR0 (GICD_OFFSET + 0xffe0)
#define GICD_PIDR1 (GICD_OFFSET + 0xffe4)
#define GICD_PIDR2 (GICD_OFFSET + 0xffe8)
#define GICD_PIDR3 (GICD_OFFSET + 0xffec)

/* GICD_PIDR bit definitions and masks */
#define GICD_PIDR2_ARCHREV_SHIFT 4
#define GICD_PIDR2_ARCHREV_MASK  0xf

/* redistributor registers */
#define GICR_SGI_OFFSET     (GICR_OFFSET + 0x10000)

#define GICR_CTLR(i)        (GICR_OFFSET + GICR_STRIDE * (i) + 0x0000)
#define GICR_IIDR(i)        (GICR_OFFSET + GICR_STRIDE * (i) + 0x0004)
#define GICR_TYPER(i)       (GICR_OFFSET + GICR_STRIDE * (i) + 0x0008)
#define GICR_STATUSR(i)     (GICR_OFFSET + GICR_STRIDE * (i) + 0x0010)
#define GICR_WAKER(i)       (GICR_OFFSET + GICR_STRIDE * (i) + 0x0014)
#define GICR_PWRR(i)        (GICR_OFFSET + GICR_STRIDE * (i) + 0x0024)

#define GICR_IGROUPR0(i)    (GICR_SGI_OFFSET + GICR_STRIDE * (i) + 0x0080)
#define GICR_IGRPMOD0(i)    (GICR_SGI_OFFSET + GICR_STRIDE * (i) + 0x0d00)
#define GICR_ISENABLER0(i)  (GICR_SGI_OFFSET + GICR_STRIDE * (i) + 0x0100)
#define GICR_ICENABLER0(i)  (GICR_SGI_OFFSET + GICR_STRIDE * (i) + 0x0180)
#define GICR_ISPENDR0(i)    (GICR_SGI_OFFSET + GICR_STRIDE * (i) + 0x0200)
#define GICR_ICPENDR0(i)    (GICR_SGI_OFFSET + GICR_STRIDE * (i) + 0x0280)
#define GICR_ISACTIVER0(i)  (GICR_SGI_OFFSET + GICR_STRIDE * (i) + 0x0300)
#define GICR_ICACTIVER0(i)  (GICR_SGI_OFFSET + GICR_STRIDE * (i) + 0x0380)
#define GICR_IPRIORITYR(i, n) (GICR_SGI_OFFSET + GICR_STRIDE * (i) + 0x0400 + (n) * 4)
#define GICR_ICFGR0(i)      (GICR_SGI_OFFSET + GICR_STRIDE * (i) + 0x0c00)
#define GICR_ICFGR1(i)      (GICR_SGI_OFFSET + GICR_STRIDE * (i) + 0x0c04)
#define GICR_NSACR(i)       (GICR_SGI_OFFSET + GICR_STRIDE * (i) + 0x0e00)

#define GICR_WAKER_PROCESSORSLEEP_LEN           1U
#define GICR_WAKER_PROCESSORSLEEP_OFFSET        1
#define GICR_WAKER_CHILDRENASLEEP_LEN           1U
#define GICR_WAKER_CHILDRENASLEEP_OFFSET        2
#define GICR_WAKER_PROCESSORSLEEP               (GICR_WAKER_PROCESSORSLEEP_LEN << GICR_WAKER_PROCESSORSLEEP_OFFSET)
#define GICR_WAKER_CHILDRENASLEEP               (GICR_WAKER_CHILDRENASLEEP_LEN << GICR_WAKER_CHILDRENASLEEP_OFFSET)

#ifdef LOSCFG_ARM_GIC_LPI_ENABLE
/**
 * Maximum number of supported LPI that generate hardware interrupts.
 */
#define OS_HWI_LPI_MAX_NUM                      65536

/**
 * Maximum interrupt number of LPI.
 */
#define OS_HWI_LPI_MAX                          ((OS_HWI_LPI_MAX_NUM) - 1)

/**
 * Minimum interrupt number of LPI.
 */
#define OS_HWI_LPI_MIN                          8192

#define OS_HWI_LPI_NUM                          (OS_HWI_LPI_MAX_NUM - OS_HWI_LPI_MIN)

/**
 * Maximum interrupt number of SPI.
 */
#define OS_HWI_SPI_MAX                          1024

#define LPI_NUM_CHECK(intNum)                   (((intNum) >= OS_HWI_LPI_MIN) && ((intNum) <= OS_HWI_LPI_MAX))

/* LPI not support */
#define GIC_LPI_CHECK_RETURN(intNum)      \
    if (LPI_NUM_CHECK(intNum)) {          \
        return LOS_OK;                    \
    }
#else
#define GIC_LPI_CHECK_RETURN(intNum)
#endif

/* Interrupt preemption config */
#define GIC_PRIORITY_MASK                       0xFFU
#define GIC_PRIORITY_OFFSET                     8U

/*
 * The number of bits to shift for an interrupt priority is dependent
 * on the number of bits implemented by the interrupt controller.
 * If the MAX_BINARY_POINT_VALUE is 7,
 * it means that interrupt preemption is not supported.
 * The macro is the binary point value that decides the maximum preemption level
 * when LOSCFG_ARCH_INTERRUPT_PREEMPTION is defined. If preemption supported, the
 * config value is [0, 1, 2, 3, 4, 5, 6], to the corresponding preemption level value
 * is [128, 64, 32, 16, 8, 4, 2].
 */
#if (LOSCFG_HWI_PRIO_LIMIT > 0) && (LOSCFG_HWI_PRIO_LIMIT <= 1)
#define MAX_BINARY_POINT_VALUE  7
#elif (LOSCFG_HWI_PRIO_LIMIT > 1) && (LOSCFG_HWI_PRIO_LIMIT <= 2)
#define MAX_BINARY_POINT_VALUE  6
#elif (LOSCFG_HWI_PRIO_LIMIT > 2) && (LOSCFG_HWI_PRIO_LIMIT <= 4)
#define MAX_BINARY_POINT_VALUE  5
#elif (LOSCFG_HWI_PRIO_LIMIT > 4) && (LOSCFG_HWI_PRIO_LIMIT <= 8)
#define MAX_BINARY_POINT_VALUE  4
#elif (LOSCFG_HWI_PRIO_LIMIT > 8) && (LOSCFG_HWI_PRIO_LIMIT <= 16)
#define MAX_BINARY_POINT_VALUE  3
#elif (LOSCFG_HWI_PRIO_LIMIT > 16) && (LOSCFG_HWI_PRIO_LIMIT <= 32)
#define MAX_BINARY_POINT_VALUE  2
#elif (LOSCFG_HWI_PRIO_LIMIT > 32) && (LOSCFG_HWI_PRIO_LIMIT <= 64)
#define MAX_BINARY_POINT_VALUE  1
#elif (LOSCFG_HWI_PRIO_LIMIT > 64) && (LOSCFG_HWI_PRIO_LIMIT <= 128)
#define MAX_BINARY_POINT_VALUE  0
#else
#error "LOSCFG_HWI_PRIO_LIMIT error configuration"
#endif
#define PRIORITY_SHIFT                          ((MAX_BINARY_POINT_VALUE + 1) % GIC_PRIORITY_OFFSET)

#ifdef LOSCFG_ARCH_ARM_AARCH64

STATIC INLINE VOID GiccSetCtlr(UINT32 val)
{
#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
    __asm__ volatile("msr " ICC_CTLR_EL3 ", %0" ::"r"(val));
#else
    __asm__ volatile("msr " ICC_CTLR_EL1 ", %0" ::"r"(val));
#endif
    ISB();
    DSB();
}

STATIC INLINE VOID GiccSetPmr(UINT32 val)
{
    __asm__ volatile("msr " ICC_PMR_EL1 ", %0" ::"r"(val));
    ISB();
    DSB();
}

STATIC INLINE UINT32 GiccGetPmr(VOID)
{
    UINT32 temp;
    __asm__ volatile("mrs %0, " ICC_PMR_EL1 : "=r"(temp));
    return temp;
}

STATIC INLINE VOID GiccSetIgrpen0(UINT32 val)
{
    __asm__ volatile("msr " ICC_IGRPEN0_EL1 ", %0" ::"r"(val));
    ISB();
    DSB();
}

STATIC INLINE VOID GiccSetIgrpen1(UINT32 val)
{
#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
    __asm__ volatile("msr " ICC_IGRPEN1_EL3 ", %0" ::"r"(val));
#else
    __asm__ volatile("msr " ICC_IGRPEN1_EL1 ", %0" ::"r"(val));
#endif
    ISB();
    DSB();
}

STATIC INLINE UINT32 GiccGetSre(VOID)
{
    UINT32 temp;
#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
    __asm__ volatile("mrs %0, " ICC_SRE_EL3 : "=r"(temp));
#else
    __asm__ volatile("mrs %0, " ICC_SRE_EL1 : "=r"(temp));
#endif
    return temp;
}

STATIC INLINE VOID GiccSetSre(UINT32 val)
{
#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
    __asm__ volatile("msr " ICC_SRE_EL3 ", %0" ::"r"(val));
#else
    __asm__ volatile("msr " ICC_SRE_EL1 ", %0" ::"r"(val));
#endif
    ISB();
    DSB();
}

STATIC INLINE VOID GiccSetEoir(UINT32 val)
{
#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
    __asm__ volatile("msr " ICC_EOIR0_EL1 ", %0" ::"r"(val));
#else
    __asm__ volatile("msr " ICC_EOIR1_EL1 ", %0" ::"r"(val));
#endif
    ISB();
    DSB();
}

STATIC INLINE UINT32 GiccGetIar(VOID)
{
    UINT32 temp;

#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
    __asm__ volatile("mrs %0, " ICC_IAR0_EL1 : "=r"(temp));
#else
    __asm__ volatile("mrs %0, " ICC_IAR1_EL1 : "=r"(temp));
#endif
    return temp;
}

STATIC INLINE VOID GiccSetSgi1r(UINT64 val)
{
    __asm__ volatile("msr " ICC_SGI1R_EL1 ", %0" ::"r"(val));
    ISB();
    DSB();
}

STATIC INLINE VOID GiccSetBpr0(UINT32 val)
{
    __asm__ volatile("msr " ICC_BPR0_EL1 ", %0" ::"r"(val));
    ISB();
    DSB();
}

STATIC INLINE VOID GiccSetBpr1(UINT32 val)
{
    __asm__ volatile("msr " ICC_BPR1_EL1 ", %0" ::"r"(val));
    ISB();
    DSB();
}

STATIC INLINE UINT32 GiccGetBpr1(VOID)
{
    UINT32 temp;
    __asm__ volatile("mrs %0, " ICC_BPR1_EL1 : "=r"(temp));

    return temp;
}

#define MPIDR_AFF_LEVEL0 0
#define MPIDR_AFF_LEVEL1 1
#define MPIDR_AFF_LEVEL2 2
#define MPIDR_AFF_LEVEL3 3

#define MPIDR_AFF_LEVEL1_SHIFT 8
#define MPIDR_AFF_LEVEL2_SHIFT 16
#define MPIDR_AFF_LEVEL3_SHIFT 32

STATIC INLINE UINT64 MpidrToAffinity(UINT64 mpidr)
{
    return ((MPIDR_AFF_LEVEL(mpidr, MPIDR_AFF_LEVEL3) << MPIDR_AFF_LEVEL3_SHIFT) |
            (MPIDR_AFF_LEVEL(mpidr, MPIDR_AFF_LEVEL2) << MPIDR_AFF_LEVEL2_SHIFT) |
            (MPIDR_AFF_LEVEL(mpidr, MPIDR_AFF_LEVEL1) << MPIDR_AFF_LEVEL1_SHIFT) |
            (MPIDR_AFF_LEVEL(mpidr, MPIDR_AFF_LEVEL0)));
}

#else /* LOSCFG_ARCH_ARM_AARCH32 */

STATIC INLINE VOID GiccSetCtlr(UINT32 val)
{
    __asm__ volatile("mcr p15, 0, %0, c12, c12, 4" : : "r"(val));
    ISB();
}

STATIC INLINE VOID GiccSetPmr(UINT32 val)
{
    __asm__ volatile("mcr p15, 0, %0, c4, c6, 0" : : "r"(val));
    ISB();
    DSB();
}

STATIC INLINE VOID GiccSetIgrpen1(UINT32 val)
{
    __asm__ volatile("mcr p15, 0, %0, c12, c12, 7" : : "r"(val));
    ISB();
    DSB();
}

STATIC INLINE UINT32 GiccGetSre(VOID)
{
    UINT32 temp;
    __asm__ volatile("mrc p15, 0, %0, c12, c12, 5" : "=r" (temp));
    return temp;
}

STATIC INLINE VOID GiccSetSre(UINT32 val)
{
    __asm__ volatile("mcr p15, 0, %0, c12, c12, 5" : : "r"(val));
    ISB();
}

STATIC INLINE VOID GiccSetEoir(UINT32 val)
{
    __asm__ volatile("mcr p15, 0, %0, c12, c12, 1" : : "r"(val));
    ISB();
}

STATIC INLINE UINT32 GiccGetIar(VOID)
{
    UINT32 temp;
    __asm__ volatile("mrc p15, 0, %0, c12, c12, 0" : "=r" (temp));
    DSB();
    return temp;
}

STATIC INLINE VOID GiccSetSgi1r(UINT64 val)
{
    UINT32 valLow = (UINT32)val;
    UINT32 valHigh = (UINT32)(val >> 32);
    __asm__ volatile("mcrr p15, 0, %0, %1, c12" : : "r"(valLow), "r"(valHigh));
    ISB();
    DSB();
}

STATIC INLINE VOID GiccSetBpr1(UINT32 val)
{
    __asm__ volatile("mcr p15, 0, %0, c12, c12, 3" : : "r"(val));
    ISB();
}

STATIC INLINE UINT64 MpidrToAffinity(UINT64 mpidr)
{
    /* Aff3 is not supported in AArch32 state. */
    return (mpidr & 0xffffff);
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _GIC_V3_H */
