/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: BARRIER
 *
 * Create: 2021-07-01
 */

#ifndef ARCH_BARRIER_H
#define ARCH_BARRIER_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef dsb
#undef dsb
#endif
#define dsb() __asm__ __volatile__("fence":::"memory")

#ifdef isb
#undef isb
#endif
#define isb() __asm__ __volatile__("fence":::"memory")

#ifdef nop
#undef nop
#endif
#define nop() __asm__ __volatile__("nop");

#ifdef wfi
#undef wfi
#endif
#define wfi() __asm__ __volatile__("wfi")

#define get_int_status()  !(read_csr(mstatus) & MSTATUS_MIE)
#define int_enter_lock()  clear_csr(mstatus, MSTATUS_MIE)
#define int_exit_lock()   set_csr(mstatus, MSTATUS_MIE)

#define int_disable_fault_exception()
#define int_enable_fault_exception()

#define get_temp_lr(temp_lr) __asm volatile("mv %0, x1" : "=r"(temp_lr))
#define get_temp_pc(temp_pc) __asm volatile("csrr %0, mepc" : "=r"(temp_pc))
#define get_temp_sp(temp_sp) __asm volatile("mv %0, sp" : "=r"(temp_sp))

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif  // BARRIER_H