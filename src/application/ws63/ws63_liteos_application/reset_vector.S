/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * @file reset_vector.S
 * @author Huawei LiteOS
 * @brief RISC-V trap handling and startup code.
 */
#include "arch/cpu.h"

.extern TrapVector
.extern runtime_init

.global reset_vector

.local HandleReset

.section  .text.entry
.option rvc

.section .text.entry
reset_vector:
    tail HandleReset

HandleReset:
#ifdef CHIP_EDA
    li t0,0x0
    csrw pmpcfg0,t0
    li t0,0x0
    csrw pmpcfg1,t0
#endif

    la    t0, TrapVector
    csrw  mtvec, t0
    csrwi mstatus, 0
    csrwi mie, 0

/* set to initial state of FPU */
    li      t0, 0x00006000
    csrs    mstatus, t0
    fssr    x0

    .option push
    .option norelax

/* initialize global pointer */
    la      gp,     _gp_
    .option pop

/* initialize stack pointer: irq stack used for startup stack */
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

    tail    runtime_init
