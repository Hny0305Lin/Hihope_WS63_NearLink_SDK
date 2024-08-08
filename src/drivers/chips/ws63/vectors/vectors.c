/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  WS63 Application Core Vector Table
 *
 * Create: 2021-06-16
 */
#if !defined USE_CMSIS_OS
#include "vectors.h"
#include "debug_print.h"
#include "arch_encoding.h"

#define RESERVED_LEN 1

void trap_entry(void);
void nmi_handler(void);
void reserve_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void bt_int0_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void bt_int1_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void mcu_pclr_lock_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void irq_gpio_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void irq_uartl0_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void irq_uarth0_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void irq_uartl2_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void qspi0_2cs_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void qspi1_2cs_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void spi4_s_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void key_scan_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void pmu_wakeup_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void pmu_sleep_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void hal_rtc_timer_isr(void) __attribute__((weak, alias("isr_not_implemented")));
void hal_timer_isr(void) __attribute__((weak, alias("isr_not_implemented")));
void irq_sdma_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void irq_dma_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void spi_m_s_0_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void spi_m_s_1_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void spi_m_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void i2c_0_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void i2c_1_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void i2c_2_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void spi3_m_s_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void eflash_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void sec_int_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void pwm_0_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void pwm_1_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void pwm_2_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void pwm_3_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void pwm_4_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void pwm_5_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void pmu_cmu_interrupt_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void mem_sub_monitor_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void b_sub_monitor_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void irq_shareram_monitor_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void eh2h_brg_int_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void pmu_32k_cali_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void b_wdt_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void irq_tsensor_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void qdec_handler(void) __attribute__((weak, alias("isr_not_implemented")));
void usb_handler(void) __attribute__((weak, alias("isr_not_implemented")));
/**
 * Default interrupt handler, used for when no specific handler has been implemented.
 * Needed for weak aliasing (an aliased function must have static linkage).
 */
//lint -esym(528, isr_not_implemented)
static void isr_not_implemented(void)
{
    while (1) { //lint !e716
    }
}

/**
 * The interrupt vector table
 */
//lint -esym(714, vector_table) Not unused
//lint -esym(785, vector_table) Too few initialises. Other slots are unused
static isr_function g_ram_exception_table[ISR_VECTOR_MAX_SUPPORTED + 1 + \
                                          RESERVED_LEN] __attribute__((section(".isr_vector"))) = {
    [USER_SOFTWARE_INT_IRQN] = trap_entry,
    [SUPERVISOR_SOFTWARE_INT_IRQN] = default_handler,
    [RESERVED_INT2_IRQN] = default_handler,
    [MACHINE_SOFTWARE_INT_IRQN] = default_handler,
    [USER_TIMER_INT_IRQN] = default_handler,
    [SUPERVISOR_TIMER_INT_IRQN] = default_handler,
    [RESERVED_INT6_IRQN] = default_handler,
    [MACHINE_TIMER_INT_IRQN] = default_handler,
    [USER_EXTERNAL_INT_IRQN] = default_handler,
    [SUPERVISOR_EXTERNAL_INT_IRQN] = default_handler,
    [RESERVED_INT10_IRQN] = default_handler,
    [MACHINE_EXTERNAL_INT_IRQN] = default_handler,
    [NON_MASKABLE_INT_IRQN] = nmi_handler,
    [RESERVED_INT13_IRQN] = default_handler,
    [RESERVED_INT14_IRQN] = default_handler,
    [RESERVED_INT15_IRQN] = default_handler,
    [RESERVED_INT16_IRQN] = default_handler,
    [RESERVED_INT17_IRQN] = default_handler,
    [RESERVED_INT18_IRQN] = default_handler,
    [RESERVED_INT19_IRQN] = default_handler,
    [RESERVED_INT20_IRQN] = default_handler,
    [RESERVED_INT21_IRQN] = default_handler,
    [RESERVED_INT22_IRQN] = default_handler,
    [RESERVED_INT23_IRQN] = default_handler,
    [RESERVED_INT24_IRQN] = default_handler,
    [RESERVED_INT25_IRQN] = default_handler,

    [ISR_VECTOR_IRQ_0] = bt_int0_handler,
    [ISR_VECTOR_IRQ_1] = bt_int1_handler,
    [ISR_VECTOR_IRQ_2] = reserve_handler,
    [ISR_VECTOR_IRQ_3] = reserve_handler,
    [ISR_VECTOR_IRQ_4] = reserve_handler,
    [ISR_VECTOR_IRQ_5] = reserve_handler,

    [ISR_VECTOR_IRQ_6] = mcu_pclr_lock_handler,
    [ISR_VECTOR_IRQ_7] = irq_gpio_handler,
    [ISR_VECTOR_IRQ_8] = irq_gpio_handler,
    [ISR_VECTOR_IRQ_9] = irq_gpio_handler,
    [ISR_VECTOR_IRQ_10] = reserve_handler,
    [ISR_VECTOR_IRQ_11] = reserve_handler,
    [ISR_VECTOR_IRQ_12] = reserve_handler,
    [ISR_VECTOR_IRQ_13] = irq_uartl0_handler,
    [ISR_VECTOR_IRQ_14] = reserve_handler,
    [ISR_VECTOR_IRQ_15] = irq_uarth0_handler,
    [ISR_VECTOR_IRQ_16] = irq_uartl2_handler,
    [ISR_VECTOR_IRQ_17] = qspi0_2cs_handler,
    [ISR_VECTOR_IRQ_18] = qspi1_2cs_handler,
    [ISR_VECTOR_IRQ_19] = spi4_s_handler,
    [ISR_VECTOR_IRQ_20] = key_scan_handler,
    [ISR_VECTOR_IRQ_21] = pmu_wakeup_handler,
    [ISR_VECTOR_IRQ_22] = pmu_sleep_handler,
    [ISR_VECTOR_IRQ_23] = hal_rtc_timer_isr,
    [ISR_VECTOR_IRQ_24] = hal_rtc_timer_isr,
    [ISR_VECTOR_IRQ_25] = hal_rtc_timer_isr,
    [ISR_VECTOR_IRQ_26] = hal_rtc_timer_isr,
    [ISR_VECTOR_IRQ_27] = hal_timer_isr,
    [ISR_VECTOR_IRQ_28] = hal_timer_isr,
    [ISR_VECTOR_IRQ_29] = hal_timer_isr,
    [ISR_VECTOR_IRQ_30] = hal_timer_isr,
    [ISR_VECTOR_IRQ_31] = irq_sdma_handler,
    [ISR_VECTOR_IRQ_32] = irq_dma_handler,
    [ISR_VECTOR_IRQ_33] = spi_m_s_0_handler,
    [ISR_VECTOR_IRQ_34] = spi_m_s_1_handler,
    [ISR_VECTOR_IRQ_35] = spi_m_handler,
    [ISR_VECTOR_IRQ_36] = i2c_0_handler,
    [ISR_VECTOR_IRQ_37] = i2c_1_handler,
    [ISR_VECTOR_IRQ_38] = i2c_2_handler,
    [ISR_VECTOR_IRQ_39] = spi3_m_s_handler,
    [ISR_VECTOR_IRQ_40] = eflash_handler,
    [ISR_VECTOR_IRQ_41] = reserve_handler,
    [ISR_VECTOR_IRQ_42] = reserve_handler,
    [ISR_VECTOR_IRQ_43] = reserve_handler,
    [ISR_VECTOR_IRQ_44] = sec_int_handler,
    [ISR_VECTOR_IRQ_45] = pwm_0_handler,
    [ISR_VECTOR_IRQ_46] = pwm_1_handler,
    [ISR_VECTOR_IRQ_47] = pwm_2_handler,
};

const isr_function *isr_get_ramexceptiontable_addr(void)
{
    return g_ram_exception_table;
}
#endif
