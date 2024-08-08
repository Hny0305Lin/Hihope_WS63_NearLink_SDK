/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  RISCV31 Core interrupt handler
 *
 * Create:  2021-07-24
 */
#include "interrupt_handler.h"
#include "vectors.h"
#include "arch_encoding.h"

#define INT_NUM_MAX 0xFFF

/* Number of running interrupts, Used to determine whether an interrupt is running */
uint32_t  g_interrupt_running = 0;

/* Assembly function declaration */
uint32_t global_interrupt_lock(void);
void global_interrupt_restore(uint32_t);

/*lint -e40 -e718 -e746*/
uint32_t interrupt_count_get(void)
{
    return g_interrupt_running;
}

void nmi_default_handler(void)
{
    clear_csr(mie, MIP_NMIE);
    g_interrupt_running++;
    if (((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_COP] != default_handler) {
        ((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_COP]();
    }
    set_csr(mie, MIP_NMIE);
    g_interrupt_running--;
}

void interrupt0_handler(void)
{
    clear_csr(mie, MIP_LOCIE0);
    g_interrupt_running++;
    if (((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_LOCIE0] != default_handler) {
        ((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_LOCIE0]();
    }
    set_csr(mie, MIP_LOCIE0);
    g_interrupt_running--;
}

void interrupt1_handler(void)
{
    clear_csr(mie, MIP_LOCIE1);
    g_interrupt_running++;
    if (((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_LOCIE1] != default_handler) {
        ((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_LOCIE1]();
    }
    set_csr(mie, MIP_LOCIE1);
    g_interrupt_running--;
}

void interrupt2_handler(void)
{
    clear_csr(mie, MIP_LOCIE2);
    g_interrupt_running++;
    if (((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_LOCIE2] != default_handler) {
        ((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_LOCIE2]();
    }
    set_csr(mie, MIP_LOCIE2);
    g_interrupt_running--;
}

void interrupt3_handler(void)
{
    clear_csr(mie, MIP_LOCIE3);
    g_interrupt_running++;
    if (((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_LOCIE3] != default_handler) {
        ((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_LOCIE3]();
    }
    set_csr(mie, MIP_LOCIE3);
    g_interrupt_running--;
}

void interrupt4_handler(void)
{
    clear_csr(mie, MIP_LOCIE4);
    g_interrupt_running++;
    if (((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_LOCIE4] != default_handler) {
        ((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_LOCIE4]();
    }
    set_csr(mie, MIP_LOCIE4);
    g_interrupt_running--;
}

void interrupt5_handler(void)
{
    clear_csr(mie, MIP_LOCIE5);
    g_interrupt_running++;
    if (((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_LOCIE5] != default_handler) {
        ((isr_function *)(isr_get_ramexceptiontable_addr()))[IRQ_LOCIE5]();
    }
    set_csr(mie, MIP_LOCIE5);
    g_interrupt_running--;
}

void local_interrupt_handler(void)
{
    uint32_t interrupt_index;
    uint32_t status_save;

    status_save = global_interrupt_lock();
    g_interrupt_running++;
    global_interrupt_restore(status_save);

    interrupt_index = interrupt_number_get();
    if (((isr_function *)(isr_get_ramexceptiontable_addr()))[interrupt_index] != reserve_handler) {
        ((isr_function *)(isr_get_ramexceptiontable_addr()))[interrupt_index]();
    }

    status_save = global_interrupt_lock();
    g_interrupt_running--;
    global_interrupt_restore(status_save);
}

void local_interrupt_priority_init(void)
{
    /* The priorities of all interrupts are initialized to 1. */
    write_custom_csr_val(LOCIPRI0, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI1, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI2, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI3, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI4, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI5, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI6, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI7, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI8, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI9, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI10, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI11, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI12, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI13, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI14, LOCIPRI_DEFAULT_VAL);
    write_custom_csr_val(LOCIPRI15, LOCIPRI_DEFAULT_VAL);
    return;
}

uint32_t interrupt_number_get(void)
{
    return (read_custom_csr(MCAUSE) & INT_NUM_MAX);
}
/*lint +e40 +e718 +e746*/