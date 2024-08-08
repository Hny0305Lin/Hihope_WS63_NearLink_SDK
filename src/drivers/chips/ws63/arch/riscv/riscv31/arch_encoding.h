/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  riscv encoding
 *
 * Create:  2021-07-21
 */

#ifndef ARCH_ENCODING_H
#define ARCH_ENCODING_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MCAUSE                  0x342
#define MCAUSE_INSN_MISALIGN    0x0
#define MCAUSE_INSN_FAULT       0x1
#define MCAUSE_INSN_ILLEGAL     0x2
#define MCAUSE_BRKT             0x3
#define MCSUSE_LOAD_MISALIGN    0x4
#define MCAUSE_LOAD_FAULT       0x5
#define MCAUSE_AMO_MISALIGN     0x6
#define MCAUSE_AMO_FAULT        0x7
#define MCAUSE_ECALL_U          0x8
#define MCAUSE_ECALL_M          0xB
#define MCAUSE_INSN_PAGE_FAULT  0xC
#define MCAUSE_LOAD_PAGE_FAULT  0xD
#define MCAUSE_AMO_PAGE_FAULT   0xF

#define EXC_SIZE_ON_STACK  (160)   // 16byte align

#define MSTATUS_UIE         0x00000001
#define MSTATUS_SIE         0x00000002
#define MSTATUS_HIE         0x00000004
#define MSTATUS_MIE         0x00000008
#define MSTATUS_UPIE        0x00000010
#define MSTATUS_SPIE        0x00000020
#define MSTATUS_HPIE        0x00000040
#define MSTATUS_MPIE        0x00000080
#define MSTATUS_SPP         0x00000100
#define MSTATUS_HPP         0x00000600
#define MSTATUS_MPP         0x00001800
#define MSTATUS_FS          0x00006000
#define MSTATUS_XS          0x00018000
#define MSTATUS_MPRV        0x00020000
#define MSTATUS_PUM         0x00040000
#define MSTATUS_VM          0x1F000000
#define MSTATUS32_SD        0x80000000
#define MSTATUS64_SD        0x8000000000000000

#define MCAUSE32_CAUSE       0x7FFFFFFF
#define MCAUSE64_CAUSE       0x7FFFFFFFFFFFFFFF
#define MCAUSE32_INT         0x80000000
#define MCAUSE64_INT         0x8000000000000000

#define SSTATUS_UIE         0x00000001
#define SSTATUS_SIE         0x00000002
#define SSTATUS_UPIE        0x00000010
#define SSTATUS_SPIE        0x00000020
#define SSTATUS_SPP         0x00000100
#define SSTATUS_FS          0x00006000
#define SSTATUS_XS          0x00018000
#define SSTATUS_PUM         0x00040000
#define SSTATUS32_SD        0x80000000
#define SSTATUS64_SD        0x8000000000000000

#define IRQ_S_SOFT          1
#define IRQ_H_SOFT          2
#define IRQ_M_SOFT          3
#define IRQ_S_TIMER         5
#define IRQ_H_TIMER         6
#define IRQ_M_TIMER         7
#define IRQ_S_EXT           9
#define IRQ_H_EXT           10
#define IRQ_M_EXT           11
#define IRQ_COP             12
#define IRQ_HOST            13
#define IRQ_LOCIE0          26
#define IRQ_LOCIE1          27
#define IRQ_LOCIE2          28
#define IRQ_LOCIE3          29
#define IRQ_LOCIE4          30
#define IRQ_LOCIE5          31

// rv_custom_local_interrupt 6 -31
#define IRQ_LOCIE6           0
#define IRQ_LOCIE7           1
#define IRQ_LOCIE8           2
#define IRQ_LOCIE9           3
#define IRQ_LOCIE10          4
#define IRQ_LOCIE11          5
#define IRQ_LOCIE12          6
#define IRQ_LOCIE13          7
#define IRQ_LOCIE14          8
#define IRQ_LOCIE15          9
#define IRQ_LOCIE16          10
#define IRQ_LOCIE17          11
#define IRQ_LOCIE18          12
#define IRQ_LOCIE19          13
#define IRQ_LOCIE20          14
#define IRQ_LOCIE21          15
#define IRQ_LOCIE22          16
#define IRQ_LOCIE23          17
#define IRQ_LOCIE24          18
#define IRQ_LOCIE25          19
#define IRQ_LOCIE26          20
#define IRQ_LOCIE27          21
#define IRQ_LOCIE28          22
#define IRQ_LOCIE29          23
#define IRQ_LOCIE30          24
#define IRQ_LOCIE31          25

// rv_nmi
#define IRQ_NMI             12
#define MIP_NMI             (1 << IRQ_NMI)

#define MIP_SSIE            (1 << IRQ_S_SOFT)
#define MIP_HSIE            (1 << IRQ_H_SOFT)
#define MIP_MSIE            (1 << IRQ_M_SOFT)
#define MIP_STIE            (1 << IRQ_S_TIMER)
#define MIP_HTIE            (1 << IRQ_H_TIMER)
#define MIP_MTIE            (1 << IRQ_M_TIMER)
#define MIP_SEIE            (1 << IRQ_S_EXT)
#define MIP_HEIE            (1 << IRQ_H_EXT)
#define MIP_MEIE            (1 << IRQ_M_EXT)
#define MIP_NMIE            (1 << IRQ_COP)
#define MIP_LOCIE0          (1 << IRQ_LOCIE0)
#define MIP_LOCIE1          (1 << IRQ_LOCIE1)
#define MIP_LOCIE2          (1 << IRQ_LOCIE2)
#define MIP_LOCIE3          (1 << IRQ_LOCIE3)
#define MIP_LOCIE4          (1 << IRQ_LOCIE4)
#define MIP_LOCIE5          ((uint32_t)1 << IRQ_LOCIE5)

// rv_custom_csr
#define LOCIPD0_LOCIE6           (1 << IRQ_LOCIE6)
#define LOCIPD0_LOCIE7           (1 << IRQ_LOCIE7)
#define LOCIPD0_LOCIE8           (1 << IRQ_LOCIE8)
#define LOCIPD0_LOCIE9           (1 << IRQ_LOCIE9)
#define LOCIPD0_LOCIE10         (1 << IRQ_LOCIE10)
#define LOCIPD0_LOCIE11         (1 << IRQ_LOCIE11)
#define LOCIPD0_LOCIE12         (1 << IRQ_LOCIE12)
#define LOCIPD0_LOCIE13         (1 << IRQ_LOCIE13)
#define LOCIPD0_LOCIE14         (1 << IRQ_LOCIE14)
#define LOCIPD0_LOCIE15         (1 << IRQ_LOCIE15)
#define LOCIPD0_LOCIE16         (1 << IRQ_LOCIE16)
#define LOCIPD0_LOCIE17         (1 << IRQ_LOCIE17)
#define LOCIPD0_LOCIE18         (1 << IRQ_LOCIE18)
#define LOCIPD0_LOCIE19         (1 << IRQ_LOCIE19)
#define LOCIPD0_LOCIE20         (1 << IRQ_LOCIE20)
#define LOCIPD0_LOCIE21         (1 << IRQ_LOCIE21)
#define LOCIPD0_LOCIE22         (1 << IRQ_LOCIE22)
#define LOCIPD0_LOCIE23         (1 << IRQ_LOCIE23)
#define LOCIPD0_LOCIE24         (1 << IRQ_LOCIE24)
#define LOCIPD0_LOCIE25         (1 << IRQ_LOCIE25)
#define LOCIPD0_LOCIE26         (1 << IRQ_LOCIE26)
#define LOCIPD0_LOCIE27         (1 << IRQ_LOCIE27)
#define LOCIPD0_LOCIE28         (1 << IRQ_LOCIE28)
#define LOCIPD0_LOCIE29         (1 << IRQ_LOCIE29)
#define LOCIPD0_LOCIE30         (1 << IRQ_LOCIE30)
#define LOCIPD0_LOCIE31         (1 << IRQ_LOCIE31)

#define PMPCFG0    0
#define PMPCFG1    1
#define PMPCFG2    2
#define PMPCFG3    3

#define PMPADDR0    0
#define PMPADDR1    1
#define PMPADDR2    2
#define PMPADDR3    3
#define PMPADDR4    4
#define PMPADDR5    5
#define PMPADDR6    6
#define PMPADDR7    7
#define PMPADDR8    8
#define PMPADDR9    9
#define PMPADDR10   10
#define PMPADDR11   11
#define PMPADDR12   12
#define PMPADDR13   13
#define PMPADDR14   14
#define PMPADDR15   15

// rv_custom_csr
#define LOCIPRI0            (0xBC0)
#define LOCIPRI1            (0xBC1)
#define LOCIPRI2            (0xBC2)
#define LOCIPRI3            (0xBC3)

#define ICCTL   (0x7C0)
#define DCCTL   (0x7C1)
#define ICMAINT (0x7C2)
#define DCMAINT (0x7C3)
#define ICINVA  (0x7C4)
#define DCINCVA (0x7C5)
#define MEMATTRL    (0x7D8)
#define MEMATTRH    (0x7D9)
#define LOCIPRI4            (0xBC4)
#define LOCIPRI5            (0xBC5)
#define LOCIPRI6            (0xBC6)
#define LOCIPRI7            (0xBC7)
#define LOCIPRI8            (0xBC8)
#define LOCIPRI9            (0xBC9)
#define LOCIPRI10           (0xBCA)
#define LOCIPRI11           (0xBCB)
#define LOCIPRI12           (0xBCC)
#define LOCIPRI13           (0xBCD)
#define LOCIPRI14           (0xBCE)
#define LOCIPRI15           (0xBCF)

#define locipri(x)     LOCIPRI##x

#define EXTERNAL_INTERRUPT_GROUP0 0
#define EXTERNAL_INTERRUPT_GROUP1 1
#define EXTERNAL_INTERRUPT_GROUP2 2
#define EXTERNAL_INTERRUPT_GROUP3 3
#define EXTERNAL_INTERRUPT_GROUP4 4
#define EXTERNAL_INTERRUPT_GROUP5 5
#define EXTERNAL_INTERRUPT_GROUP6 6
#define EXTERNAL_INTERRUPT_GROUP7 7

#define LOCIEN0             0xBE0
#define LOCIEN1             0xBE1
#define LOCIEN2             0xBE2

#define LOCIPD0             0xBE8
#define LOCIPD1             0xBE9
#define LOCIPD2             0xBEA
#define LOCIPD3             0xBEB

/* local interrupt pending clear register */
#define LOCIPCLR            0xBF0
#define PRITHD              0xBFE
#define CXCPTSC             0xFC2

#define SIP_SSIP            MIP_SSIP
#define SIP_STIP            MIP_STIP

#define PRV_U               0
#define PRV_S               1
#define PRV_H               2
#define PRV_M               3

#define VM_MBARE            0
#define VM_MBB              1
#define VM_MBBID            2
#define VM_SV32             8
#define VM_SV39             9
#define VM_SV48             10

#define DEFAULT_RSTVEC      0x00001000
#define DEFAULT_NMIVEC      0x00001004
#define DEFAULT_MTVEC       0x00001010
#define CONFIG_STRING_ADDR  0x0000100C
#define EXT_IO_BASE         0x40000000
#define DRAM_BASE           0x80000000

// page table entry (PTE) fields
#define PTE_V               0x001 // Valid
#define PTE_TYPE            0x01E // Type
#define PTE_R               0x020 // Referenced
#define PTE_D               0x040 // Dirty
#define PTE_SOFT            0x380 // Reserved for Software

#define PTE_TYPE_TABLE        0x00
#define PTE_TYPE_TABLE_GLOBAL 0x02
#define PTE_TYPE_URX_SR       0x04
#define PTE_TYPE_URWX_SRW     0x06
#define PTE_TYPE_UR_SR        0x08
#define PTE_TYPE_URW_SRW      0x0A
#define PTE_TYPE_URX_SRX      0x0C
#define PTE_TYPE_URWX_SRWX    0x0E
#define PTE_TYPE_SR           0x10
#define PTE_TYPE_SRW          0x12
#define PTE_TYPE_SRX          0x14
#define PTE_TYPE_SRWX         0x16
#define PTE_TYPE_SR_GLOBAL    0x18
#define PTE_TYPE_SRW_GLOBAL   0x1A
#define PTE_TYPE_SRX_GLOBAL   0x1C
#define PTE_TYPE_SRWX_GLOBAL  0x1E

#define PTE_PPN_SHIFT 10

#define pte_table(pte) ((0x0000000AU >> ((pte) & 0x1F)) & 1)
#define pte_ur(pte)    ((0x0000AAA0U >> ((pte) & 0x1F)) & 1)
#define pte_uw(pte)    ((0x00008880U >> ((pte) & 0x1F)) & 1)
#define pte_ux(pte)    ((0x0000A0A0U >> ((pte) & 0x1F)) & 1)
#define pte_sr(pte)    ((0xAAAAAAA0U >> ((pte) & 0x1F)) & 1)
#define pte_sw(pte)    ((0x88888880U >> ((pte) & 0x1F)) & 1)
#define pte_sx(pte)    ((0xA0A0A000U >> ((pte) & 0x1F)) & 1)

#define pte_check_perm(pte, supervisor, store, fetch) \
    ((store) ? ((supervisor) ? pte_sw(pte) : pte_uw(pte)) : \
         (fetch) ? ((supervisor) ? pte_sx(pte) : pte_ux(pte)) : \
         ((supervisor) ? pte_sr(pte) : pte_ur(pte)))

#ifdef __riscv

#ifdef __riscv64
# define MSTATUS_SD             MSTATUS64_SD
# define SSTATUS_SD             SSTATUS64_SD
# define MCAUSE_INT             MCAUSE64_INT
# define MCAUSE_CAUSE           MCAUSE64_CAUSE
# define RISCV_PGLEVEL_BITS     9
#else
# define MSTATUS_SD             MSTATUS32_SD
# define SSTATUS_SD             SSTATUS32_SD
# define RISCV_PGLEVEL_BITS     10
# define MCAUSE_INT             MCAUSE32_INT
# define MCAUSE_CAUSE           MCAUSE32_CAUSE
#endif

#define RISCV_PGSHIFT           12
#define RISCV_PGSIZE            (1 << RISCV_PGSHIFT)

#ifndef __ASSEMBLER__

#ifdef __GNUC__

#define read_csr(reg) ({ unsigned long __tmp; \
        asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
        __tmp; })

#define write_csr(reg, val) ({ \
        if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
            asm volatile ("csrw " #reg ", %0" :: "i"(val)); \
        else \
            asm volatile ("csrw " #reg ", %0" :: "r"(val)); })

#define swap_csr(reg, val) ({ unsigned long __tmp; \
        if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
            asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "i"(val)); \
        else \
            asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "r"(val)); \
        __tmp; })

#define set_csr(reg, bit) ({ unsigned long __tmp; \
        if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
            asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
        else \
            asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
        __tmp; })

#define clear_csr(reg, bit) ({ unsigned long __tmp; \
        if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
            asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
        else \
            asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
        __tmp; })

// rv_custom_csr
#define read_custom_csr(reg) ({ unsigned long __tmp; \
        asm volatile ("csrr %0, %1" : "=r"(__tmp) : "i"(reg)); \
        __tmp; })

#define write_custom_csr_val(reg_addr, val) ({ \
        if (__builtin_constant_p(val)) \
            asm volatile("li t0," "%0" : : "i"(val) : "%t0"); \
        else \
            asm volatile("mv t0," "%0" : : "r"(val) : "%t0"); \
        asm volatile("csrw %0, t0" :: "i"(reg_addr) : "%t0"); \
    })

#define set_custom_csr(reg_addr, bit) ({ \
        if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
            asm volatile("li t0," "%0" : : "i"(bit) : "%t0"); \
        else \
            asm volatile("mv t0," "%0" : : "r"(bit) : "%t0"); \
        asm volatile("csrs %0, t0" :: "i"(reg_addr) : "%t0"); \
    })

#define clear_custom_csr(reg_addr, bit) ({ \
        if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
            asm volatile("li t0," "%0" : : "i"(bit) : "%t0"); \
        else \
            asm volatile("mv t0," "%0" : : "r"(bit) : "%t0"); \
        asm volatile("csrc %0, t0" :: "i"(reg_addr) : "%t0"); \
    })

#define rdtime() read_csr(time)
#define rdcycle() read_csr(cycle)
#define rdinstret() read_csr(instret)

#endif

#endif

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

