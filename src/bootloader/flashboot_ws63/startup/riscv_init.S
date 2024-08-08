/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: main
 *
 * Create: 2021-03-09
 */

        .section .text.entry
        .global _start
        .option norvc
_start:
        j Reset_Handler

Reset_Handler:
        li t0,0x0
        csrw pmpcfg0,t0
        li t0,0x0
        csrw pmpcfg1,t0
        li t0,0x0
        csrw pmpcfg2,t0
        li t0,0x0
        csrw pmpcfg3,t0
        li t0,0x0
        csrw 0x7d9,t0
        la t0, __flash_boot_flag_begin__
        mv t1, a0
        lw     t3, (t1)
        sw     t3, (t0)
        la t0, trap_vector
        addi t0, t0, 1
        csrw mtvec, t0
        csrwi mstatus, 0
        csrwi mie, 0

        # initialize global pointer
        la gp, _gp_

        # initialize stack pointer
        la sp, __stack_top__

        /* init stack */
        la      t0, g_system_stack_begin
        la      t1, g_system_stack_end
        beq     t0, t1, end_set_stack_loop
        li      t2, 0xefbeadde

set_stack_loop:
        sw      t2, (t0)
        addi    t0, t0, 4
        blt     t0, t1, set_stack_loop
end_set_stack_loop:

        /* clear reg */
        li      ra, 0
        li      tp, 0
        li      s0, 0
        li      s1, 0
        li      a0, 0
        li      a1, 0
        li      a2, 0
        li      a3, 0
        li      a4, 0
        li      a5, 0
        li      a6, 0
        li      a7, 0
        li      s2, 0
        li      s3, 0
        li      s4, 0
        li      s5, 0
        li      s6, 0
        li      s7, 0
        li      s8, 0
        li      s9, 0
        li      s10, 0
        li      s11, 0
        li      t3, 0
        li      t4, 0
        li      t5, 0
        li      t6, 0

        /* clear bss section */
        la      t0, __bss_begin__
        la      t1, __bss_end__
        beq     t0, t1, end_clear_bss_loop
        li      t2, 0x00000000

clear_bss_loop:
        sw      t2, (t0)
        addi    t0, t0, 4
        blt     t0, t1, clear_bss_loop
end_clear_bss_loop:

        j       start_fastboot
