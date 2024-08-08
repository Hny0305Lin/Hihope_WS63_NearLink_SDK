/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  Interrupt DRIVER
 *
 * Create: 2021-07-01
 */
#include "chip_io.h"
#include "arch_encoding.h"
#include "vectors.h"
#include "arch_barrier.h"
#include "interrupt_handler.h"

/*lint -e40 -e571 -e718 -e737 -e746*/
uint32_t int_set_irq_func(int32_t irq_id, isr_function func)
{
    ((isr_function *)(isr_get_ramexceptiontable_addr()))[irq_id] = func;

    return 0;
}

void int_set_priority_grouping(uint32_t priority_group)
{
    UNUSED(priority_group);
}

uint32_t int_get_priority_grouping(void)
{
    return 0;
}

void int_enable_irq(int32_t irq_id)
{
    uint32_t irqorder;
    uint32_t locien_offset;

    // enable local interrupt 26 -31 by irq_id
    if ((irq_id < RISCV_LOCAL_IRQ_VECTOR_CNT) && (irq_id >= RISCV_SYS_VECTOR_CNT)) {
        irqorder = (uint32_t)irq_id;
        set_csr(mie, ((uint32_t)1 << irqorder));
    } else {
        irqorder = (uint32_t)((irq_id - RISCV_LOCAL_IRQ_VECTOR_CNT) % LOCIEN_IRQ_NUM);
        locien_offset = (uint32_t)((irq_id - RISCV_LOCAL_IRQ_VECTOR_CNT) / LOCIEN_IRQ_NUM);
        switch (locien_offset) {
            case EXTERNAL_INTERRUPT_GROUP0:
                set_custom_csr(LOCIEN0, (uint32_t)((uint32_t)1 << irqorder));
                break;
            case EXTERNAL_INTERRUPT_GROUP1:
                set_custom_csr(LOCIEN1, (uint32_t)((uint32_t)1 << irqorder));
                break;
            case EXTERNAL_INTERRUPT_GROUP2:
                set_custom_csr(LOCIEN2, (uint32_t)((uint32_t)1 << irqorder));
                break;
            default:
                break;
        }
    }
}

void int_disable_irq(int32_t irq_id)
{
    uint32_t irqorder;
    uint32_t locien_offset;

    // Disable local interrupt 26 -31 by irq_id
    if ((irq_id < RISCV_LOCAL_IRQ_VECTOR_CNT) && (irq_id >= RISCV_SYS_VECTOR_CNT)) {
        irqorder = (uint32_t)irq_id;
        clear_csr(mie, ((uint32_t)1 << irqorder));
    } else {
        irqorder = (uint32_t)((irq_id - RISCV_LOCAL_IRQ_VECTOR_CNT) % LOCIEN_IRQ_NUM);
        locien_offset = (uint32_t)((irq_id - RISCV_LOCAL_IRQ_VECTOR_CNT) / LOCIEN_IRQ_NUM);
        switch (locien_offset) {
            case EXTERNAL_INTERRUPT_GROUP0:
                clear_custom_csr(LOCIEN0, ((uint32_t)1 << irqorder));
                break;
            case EXTERNAL_INTERRUPT_GROUP1:
                clear_custom_csr(LOCIEN1, ((uint32_t)1 << irqorder));
                break;
            case EXTERNAL_INTERRUPT_GROUP2:
                clear_custom_csr(LOCIEN2, ((uint32_t)1 << irqorder));
                break;
            default:
                break;
        }
    }
    int_clear_pending_irq(irq_id);
}

uint32_t int_get_enable_irq(int32_t irq_id)
{
    uint32_t irqorder;
    uint32_t locien_offset;
    uint32_t enable = 0;

    if ((irq_id < RISCV_LOCAL_IRQ_VECTOR_CNT) && (irq_id >= RISCV_SYS_VECTOR_CNT)) {
        irqorder = (uint32_t)irq_id;
        enable = read_csr(mie) & (((uint32_t)1 << irqorder));
    } else {
        irqorder = (uint32_t)((irq_id - RISCV_LOCAL_IRQ_VECTOR_CNT) % LOCIEN_IRQ_NUM);
        locien_offset = (uint32_t)((irq_id - RISCV_LOCAL_IRQ_VECTOR_CNT) / LOCIEN_IRQ_NUM);
        switch (locien_offset) {
            case EXTERNAL_INTERRUPT_GROUP0:
                enable = read_custom_csr(LOCIEN0) & (((uint32_t)1 << irqorder));
                break;
            case EXTERNAL_INTERRUPT_GROUP1:
                enable = read_custom_csr(LOCIEN1) & (((uint32_t)1 << irqorder));
                break;
            case EXTERNAL_INTERRUPT_GROUP2:
                enable = read_custom_csr(LOCIEN2) & (((uint32_t)1 << irqorder));
                break;
            default:
                break;
        }
    }
    return enable;
}

uint32_t int_get_pending_irq(int32_t irq_id)
{
    UNUSED(irq_id);
    return 0;
}

void int_clear_pending_irq(int32_t irq_id)
{
    dsb();
    write_custom_csr_val(LOCIPCLR, irq_id);
    dsb();
}

void int_set_pendind_irq(int32_t irq_id)
{
    UNUSED(irq_id);
}

uint32_t int_get_active(int32_t irq_id)
{
    UNUSED(irq_id);
    return 0;
}

void int_set_priority(int32_t irq_id, uint32_t priority)
{
    uint32_t irqorder;
    uint32_t locipri_offset;

    if (priority < INTERRUPT_PRIO_LOWEST || priority > INTERRUPT_PRIO_HIGHEST) {
        return;
    }

    // set the priority of non-standard local interrupt
    if (irq_id >= RISCV_SYS_VECTOR_CNT && irq_id < RISCV_VECTOR_CNT) {
        irqorder = (uint32_t)((irq_id - RISCV_SYS_VECTOR_CNT) % LOCIPRI_IRQ_NUM);
        locipri_offset = (uint32_t)((irq_id - RISCV_SYS_VECTOR_CNT) / LOCIPRI_IRQ_NUM);
        switch (locipri_offset) {
            case EXTERNAL_INTERRUPT_GROUP0:
                set_custom_csr(LOCIPRI0, (uint32_t)(priority << (irqorder * LOCIPRI_IRQ_BITS)));
                break;
            case EXTERNAL_INTERRUPT_GROUP1:
                set_custom_csr(LOCIPRI1, priority << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP2:
                set_custom_csr(LOCIPRI2, priority << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP3:
                set_custom_csr(LOCIPRI3, priority << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP4:
                set_custom_csr(LOCIPRI4, priority << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP5:
                set_custom_csr(LOCIPRI5, priority << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP6:
                set_custom_csr(LOCIPRI6, priority << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP7:
                set_custom_csr(LOCIPRI7, priority << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            default:
                break;
        }
    }
}

uint32_t int_get_priority(int32_t irq_id)
{
    unsigned int irqorder;
    uint32_t locipri_offset;
    uint32_t priority = 0;

    // get the priority of non-standard local interrupt
    if (irq_id >= RISCV_SYS_VECTOR_CNT && irq_id < RISCV_VECTOR_CNT) {
        irqorder = (uint32_t)((irq_id - RISCV_SYS_VECTOR_CNT) % LOCIPRI_IRQ_NUM);
        locipri_offset = (uint32_t)((irq_id - RISCV_SYS_VECTOR_CNT) / LOCIPRI_IRQ_NUM);
        switch (locipri_offset) {
            case EXTERNAL_INTERRUPT_GROUP0:
                priority = read_custom_csr(LOCIPRI0) & ((uint32_t)0x3 << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP1:
                priority = read_custom_csr(LOCIPRI1) & ((uint32_t)0x3 << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP2:
                priority = read_custom_csr(LOCIPRI2) & ((uint32_t)0x3 << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP3:
                priority = read_custom_csr(LOCIPRI3) & ((uint32_t)0x3 << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP4:
                priority = read_custom_csr(LOCIPRI4) & ((uint32_t)0x3 << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP5:
                priority = read_custom_csr(LOCIPRI5) & ((uint32_t)0x3 << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP6:
                priority = read_custom_csr(LOCIPRI6) & ((uint32_t)0x3 << (irqorder * LOCIPRI_IRQ_BITS));
                break;
            case EXTERNAL_INTERRUPT_GROUP7:
                priority = read_custom_csr(LOCIPRI7) & ((uint32_t)0x3 <<  (irqorder * LOCIPRI_IRQ_BITS));
                break;
            default:
                break;
        }
    }

    return priority;
}

void int_system_reset(void)
{
}

static void int_set_default_priority(void)
{
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
}

void int_setup(void)
{
    set_csr(mstatus, MSTATUS_MIE);
    int_set_default_priority();
}

bool int_is_interrupt_context(void)
{
    return (interrupt_number_get() > 0);
}

int32_t int_get_current_irqn(void)
{
    return (int32_t)(interrupt_number_get());
}

int32_t int_get_current_priority(void)
{
    return (int32_t)(int_get_priority(int_get_current_irqn()));
}
