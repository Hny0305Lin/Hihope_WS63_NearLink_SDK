/******************************************************************************
 * @brief    RISCV31 vectors module implementation
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
******************************************************************************/
.equ ccause, 0xFC2
.macro push_reg
    addi sp, sp, -32*4

    sw ra, 0 * 4(sp)
    sw sp, 1 * 4(sp)
    sw gp, 2 * 4(sp)
    sw tp, 3 * 4(sp)
    sw t0, 4 * 4(sp)
    sw t1, 5 * 4(sp)
    sw t2, 6 * 4(sp)
    sw s0, 7 * 4(sp)
    sw s1, 8 * 4(sp)
    sw a0, 9 * 4(sp)
    sw a1, 10 * 4(sp)
    sw a2, 11 * 4(sp)
    sw a3, 12 * 4(sp)
    sw a4, 13 * 4(sp)
    sw a5, 14 * 4(sp)
    sw a6, 15 * 4(sp)
    sw a7, 16 * 4(sp)
    sw s2, 17 * 4(sp)
    sw s3, 18 * 4(sp)
    sw s4, 19 * 4(sp)
    sw s5, 20 * 4(sp)
    sw s6, 21 * 4(sp)
    sw s7, 22 * 4(sp)
    sw s8, 23 * 4(sp)
    sw s9, 24 * 4(sp)
    sw s10, 25 * 4(sp)
    sw s11, 26 * 4(sp)
    sw t3, 27 * 4(sp)
    sw t4, 28 * 4(sp)
    sw t5, 29 * 4(sp)
    sw t6, 30 * 4(sp)

    csrr t0, mepc
    sw t0, 31 * 4(sp)
.endm

.macro pop_reg
    lw    t0, 31 * 4(sp)
    csrw  mepc, t0

    lw ra, 0 * 4(sp)

    lw t0, 4 * 4(sp)
    lw t1, 5 * 4(sp)
    lw t2, 6 * 4(sp)
    lw s0, 7 * 4(sp)
    lw s1, 8 * 4(sp)
    lw a0, 9 * 4(sp)
    lw a1, 10 * 4(sp)
    lw a2, 11 * 4(sp)
    lw a3, 12 * 4(sp)
    lw a4, 13 * 4(sp)
    lw a5, 14 * 4(sp)
    lw a6, 15 * 4(sp)
    lw a7, 16 * 4(sp)
    lw s2, 17 * 4(sp)
    lw s3, 18 * 4(sp)
    lw s4, 19 * 4(sp)
    lw s5, 20 * 4(sp)
    lw s6, 21 * 4(sp)
    lw s7, 22 * 4(sp)
    lw s8, 23 * 4(sp)
    lw s9, 24 * 4(sp)
    lw s10, 25 * 4(sp)
    lw s11, 26 * 4(sp)
    lw t3, 27 * 4(sp)
    lw t4, 28 * 4(sp)
    lw t5, 29 * 4(sp)
    lw t6, 30 * 4(sp)

    addi sp, sp, 32*4
.endm

.macro SAVE_ALL
    addi sp, sp, -36 * 4
    sw s11, 8 * 4(sp)
    sw s10, 9 * 4(sp)
    sw s9, 10 * 4(sp)
    sw s8, 11 * 4(sp)
    sw s7, 12 * 4(sp)
    sw s6, 13 * 4(sp)
    sw s5, 14 * 4(sp)
    sw s4, 15 * 4(sp)
    sw s3, 16 * 4(sp)
    sw s2, 17 * 4(sp)
    sw s1, 18 * 4(sp)
    sw s0, 19 * 4(sp)
    sw t6, 20 * 4(sp)
    sw t5, 21 * 4(sp)
    sw t4, 22 * 4(sp)
    sw t3, 23 * 4(sp)
    sw a7, 24 * 4(sp)
    sw a6, 25 * 4(sp)
    sw a5, 26 * 4(sp)
    sw a4, 27 * 4(sp)
    sw a3, 28 * 4(sp)
    sw a2, 29 * 4(sp)
    sw a1, 30 * 4(sp)
    sw a0, 31 * 4(sp)
    sw t2, 32 * 4(sp)
    sw t1, 33 * 4(sp)
    sw t0, 34 * 4(sp)
    sw ra, 35 * 4(sp)

    csrr  s4, ccause
    sw s4, 0 * 4(sp)
    csrr  s3, mcause
    sw s3, 1 * 4(sp)
    csrr  s2, mbadaddr
    sw s2, 2 * 4(sp)
    sw gp, 3 * 4(sp)
    csrr  s1, mstatus
    sw s1, 4 * 4(sp)
    csrr  s0, mepc
    sw s0, 5 * 4(sp)
    sw tp, 6 * 4(sp)
    sw sp, 7 * 4(sp)
.endm

.macro RESTORE_ALL
    lw t0, 5 * 4(sp)
    csrw  mepc, t0

    lw s11, 8 * 4(sp)
    lw s10, 9 * 4(sp)
    lw s9, 10 * 4(sp)
    lw s8, 11 * 4(sp)
    lw s7, 12 * 4(sp)
    lw s6, 13 * 4(sp)
    lw s5, 14 * 4(sp)
    lw s4, 15 * 4(sp)
    lw s3, 16 * 4(sp)
    lw s2, 17 * 4(sp)
    lw s1, 18 * 4(sp)
    lw s0, 19 * 4(sp)
    lw t6, 20 * 4(sp)
    lw t5, 21 * 4(sp)
    lw t4, 22 * 4(sp)
    lw t3, 23 * 4(sp)
    lw a7, 24 * 4(sp)
    lw a6, 25 * 4(sp)
    lw a5, 26 * 4(sp)
    lw a4, 27 * 4(sp)
    lw a3, 28 * 4(sp)
    lw a2, 29 * 4(sp)
    lw a1, 30 * 4(sp)
    lw a0, 31 * 4(sp)
    lw t2, 32 * 4(sp)
    lw t1, 33 * 4(sp)
    lw t0, 34 * 4(sp)
    lw ra, 35 * 4(sp)

    addi sp, sp, 36 * 4
.endm

exc_syscall:
    /* Temporarily Unsupported */
    ret

handle_syscall:
    /*
     * Advance MEPC to avoid executing the original
     * scall instruction on mret
     */
    addi s0, s0, 0x4
    sw s0, 0 * 4(sp)

    jal exc_syscall

    RESTORE_ALL
    mret

    .section ".rodata"
    .align 4
    /* Exception vector table */
excp_vect_table:
    .word do_trap_insn_misaligned
    .word do_trap_insn_fault
    .word do_trap_insn_illegal
    .word do_trap_break
    .word do_trap_load_misaligned
    .word do_trap_load_fault
    .word do_trap_store_misaligned
    .word do_trap_store_fault
    .word do_trap_ecall_u /* system call, gets intercepted */
    .word do_trap_ecall_s
    .word do_trap_unknown
    .word do_trap_ecall_m
    .word do_insn_page_fault   /* instruction page fault */
    .word do_load_page_fault   /* load page fault */
    .word do_trap_unknown
    .word do_store_page_fault   /* store page fault */
    .word do_hard_fault
    .word do_lockup
excp_vect_table_end:

    .align 4
    .global trap_entry
trap_entry:
    SAVE_ALL

    /* Exception run with interrupts masked */
    csrc mstatus,0x00000008

    csrw mscratch, sp
    la sp, __excp_stack_top__

    /* Handle syscall */
    li t0, 0x8
    beq s3, t0, handle_syscall

    /* Handle other exceptions */
    slli t0, s3, 2
    la t1, excp_vect_table
    la t2, excp_vect_table_end
    csrr a0, mscratch
    add t0, t1, t0
    bgeu t0, t2, 1f
    lw t0, 0 (t0)
    jal do_hard_fault_handler

    csrr sp, mscratch
    # Remain in M-mode after mret
    li t0, 0x00001800
    csrs mstatus, t0

    RESTORE_ALL
    mret
1:
    tail do_trap_unknown

    .align 4
    .global trap_vector
    .option norvc
trap_vector:
    j trap_entry
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j nmi_vector
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_handler
    j default_interrupt0_handler
    j default_interrupt1_handler
    j default_interrupt2_handler
    j default_interrupt3_handler
    j default_interrupt4_handler
    j default_interrupt5_handler

    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler

    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    j default_local_interrupt_handler
    .option rvc

    .align 4
    .global nmi_vector
nmi_vector:
    csrci mstatus, 0x08
    SAVE_ALL

    csrw mscratch, sp
    la  sp, __irq_stack_top__
    csrr  a0, mscratch
    jal do_hard_fault_handler

    csrr  sp, mscratch

    /* enable global interrupts */
    li  t0, 0x1880
    csrs  mstatus, t0

    RESTORE_ALL
    mret

default_interrupt0_handler:
    csrci mstatus, 0x08
    push_reg

    csrw mscratch, sp
    la  sp, __irq_stack_top__

    jal interrupt0_handler

    csrr  sp, mscratch

    /* enable global interrupts */
    li  t0, 0x1880
    csrs  mstatus, t0

    pop_reg
    mret

default_interrupt1_handler:
    csrci mstatus, 0x08
    push_reg

    csrw mscratch, sp
    la  sp, __irq_stack_top__

    jal interrupt1_handler

    csrr  sp, mscratch

    li  t0, 0x1880
    csrs  mstatus, t0

    pop_reg
    mret

default_interrupt2_handler:
    csrci mstatus, 0x08
    push_reg

    csrw mscratch, sp
    la  sp, __irq_stack_top__

    jal interrupt2_handler

    csrr  sp, mscratch

    li  t0, 0x1880
    csrs  mstatus, t0

    pop_reg
    mret

default_interrupt3_handler:
    csrci mstatus, 0x08
    push_reg

    csrw mscratch, sp
    la  sp, __irq_stack_top__

    jal interrupt3_handler

    csrr  sp, mscratch

    li  t0, 0x1880
    csrs  mstatus, t0

    pop_reg
    mret

default_interrupt4_handler:
    csrci mstatus, 0x08
    push_reg

    csrw mscratch, sp
    la  sp, __irq_stack_top__

    jal interrupt4_handler

    csrr  sp, mscratch

    li  t0, 0x1880
    csrs  mstatus, t0

    pop_reg
    mret

default_interrupt5_handler:
    csrci mstatus, 0x08
    push_reg

    csrw mscratch, sp
    la  sp, __irq_stack_top__

    jal interrupt5_handler

    csrr  sp, mscratch

    li  t0, 0x1880
    csrs  mstatus, t0

    pop_reg
    mret

default_local_interrupt_handler:
    csrci mstatus, 0x08
    push_reg

    csrw mscratch, sp
    la  sp, __irq_stack_top__

    jal local_interrupt_handler

    csrr  sp, mscratch

    li  t0, 0x1880
    csrs  mstatus, t0

    pop_reg
    mret

    .global global_interrupt_lock
global_interrupt_lock:
    csrr    a0, mstatus
    li      t0, 0x08
    csrrc   zero, mstatus, t0
    ret

    .global global_interrupt_unlock
global_interrupt_unlock:
    csrr    a0, mstatus
    li      t0, 0x08
    csrrs   zero, mstatus, t0
    ret

    .global global_interrupt_restore
global_interrupt_restore:
    csrw mstatus, a0
    ret
