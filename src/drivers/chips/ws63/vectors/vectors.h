/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description:  Hi113X Vector Table Definitions
 *
 * Create:  2018-10-15
 */
#ifndef VECTORS_H
#define VECTORS_H

#include <stdint.h>
#include "core.h"
#include "interrupt_porting.h"

/** @addtogroup connectivity_config_vectors VECTORS
  * @{
  */
/**
 * @brief  Defined to be the highest ISR_VECTOR supported by the core
 */
#define ISR_VECTOR_MAX_SUPPORTED (BUTT_IRQN - 1)

#define LOCIEN_IRQ_NUM    32
#define LOCIPRI_IRQ_NUM   8
#define LOCIPRI_IRQ_BITS  4
#define LOCIPRI_DEFAULT_VAL 0x11111111 /* The default interrupt priority is 1. */

/**
 * Highest priority of a hardware interrupt.
 */
#define  INTERRUPT_PRIO_HIGHEST        7

/**
 * Lowest priority of a hardware interrupt.
 */
#define  INTERRUPT_PRIO_LOWEST         1

/**
 * Count of HimiDeer system interrupt vector.
 */
#define RISCV_SYS_VECTOR_CNT        26

/**
 * Count of HimiDeer local interrupt vector 0 - 5, enabled by CSR mie 26 -31 bit.
 */
#define RISCV_MIE_IRQ_VECTOR_CNT        6

/**
 * Count of HimiDeer local interrupt vector 6~76, enabled by custom CSR locie0~2.
 */
#define RISCV_CUSTOM_IRQ_VECTOR_CNT       60

/**
 * Count of HimiDeer local IRQ interrupt vector.
 */
#define RISCV_LOCAL_IRQ_VECTOR_CNT        (RISCV_MIE_IRQ_VECTOR_CNT + RISCV_SYS_VECTOR_CNT)

/**
 * Count of himideer interrupt vector.
 */
#define RISCV_VECTOR_CNT (RISCV_LOCAL_IRQ_VECTOR_CNT + RISCV_CUSTOM_IRQ_VECTOR_CNT)

/**
 * @brief  Interrupt vector identifiers
 */
typedef enum {
    USER_SOFTWARE_INT_IRQN       = 0, // !< RISCV Trap Entry
    SUPERVISOR_SOFTWARE_INT_IRQN = 1,
    RESERVED_INT2_IRQN           = 2,
    MACHINE_SOFTWARE_INT_IRQN    = 3,
    USER_TIMER_INT_IRQN          = 4,
    SUPERVISOR_TIMER_INT_IRQN    = 5,
    RESERVED_INT6_IRQN           = 6,
    MACHINE_TIMER_INT_IRQN       = 7,
    USER_EXTERNAL_INT_IRQN       = 8,
    SUPERVISOR_EXTERNAL_INT_IRQN = 9,
    RESERVED_INT10_IRQN          = 10,
    MACHINE_EXTERNAL_INT_IRQN    = 11,
    NON_MASKABLE_INT_IRQN        = 12, // !< RISCV NMI vector
    RESERVED_INT13_IRQN          = 13,
    RESERVED_INT14_IRQN          = 14,
    RESERVED_INT15_IRQN          = 15,
#if ARCH == RISCV70
    ISR_VECTOR_IRQ_0             = 16,  // !< RISCV Local Interrupt request 0 vector
#else
    RESERVED_INT16_IRQN          = 16,
    RESERVED_INT17_IRQN          = 17,
    RESERVED_INT18_IRQN          = 18,
    RESERVED_INT19_IRQN          = 19,
    RESERVED_INT20_IRQN          = 20,
    RESERVED_INT21_IRQN          = 21,
    RESERVED_INT22_IRQN          = 22,
    RESERVED_INT23_IRQN          = 23,
    RESERVED_INT24_IRQN          = 24,
    RESERVED_INT25_IRQN          = 25,
    ISR_VECTOR_IRQ_0             = 26,  // !< RISCV Local Interrupt request 0 vector
#endif
    ISR_VECTOR_IRQ_1,              // !< RISCV Local Interrupt request 1 vector
    ISR_VECTOR_IRQ_2,              // !< RISCV Local Interrupt request 2 vector
    ISR_VECTOR_IRQ_3,              // !< RISCV Local Interrupt request 3 vector
    ISR_VECTOR_IRQ_4,              // !< RISCV Local Interrupt request 4 vector
    ISR_VECTOR_IRQ_5,              // !< RISCV Local Interrupt request 5 vector
    ISR_VECTOR_IRQ_6,              // !< RISCV Local Interrupt request 6 vector
    ISR_VECTOR_IRQ_7,              // !< RISCV Local Interrupt request 7 vector
    ISR_VECTOR_IRQ_8,              // !< RISCV Local Interrupt request 8 vector
    ISR_VECTOR_IRQ_9,              // !< RISCV Local Interrupt request 9 vector
    ISR_VECTOR_IRQ_10,             // !< RISCV Local Interrupt request 10 vector
    ISR_VECTOR_IRQ_11,             // !< RISCV Local Interrupt request 11 vector
    ISR_VECTOR_IRQ_12,             // !< RISCV Local Interrupt request 12 vector
    ISR_VECTOR_IRQ_13,             // !< RISCV Local Interrupt request 13 vector
    ISR_VECTOR_IRQ_14,             // !< RISCV Local Interrupt request 14 vector
    ISR_VECTOR_IRQ_15,             // !< RISCV Local Interrupt request 15 vector
    ISR_VECTOR_IRQ_16,             // !< RISCV Local Interrupt request 16 vector
    ISR_VECTOR_IRQ_17,             // !< RISCV Local Interrupt request 17 vector
    ISR_VECTOR_IRQ_18,             // !< RISCV Local Interrupt request 18 vector
    ISR_VECTOR_IRQ_19,             // !< RISCV Local Interrupt request 19 vector
    ISR_VECTOR_IRQ_20,             // !< RISCV Local Interrupt request 20 vector
    ISR_VECTOR_IRQ_21,             // !< RISCV Local Interrupt request 21 vector
    ISR_VECTOR_IRQ_22,             // !< RISCV Local Interrupt request 22 vector
    ISR_VECTOR_IRQ_23,             // !< RISCV Local Interrupt request 23 vector
    ISR_VECTOR_IRQ_24,             // !< RISCV Local Interrupt request 24 vector
    ISR_VECTOR_IRQ_25,             // !< RISCV Local Interrupt request 25 vector

    ISR_VECTOR_IRQ_26,             // !< RISCV Local Interrupt request 26 vector
    ISR_VECTOR_IRQ_27,             // !< RISCV Local Interrupt request 27 vector
    ISR_VECTOR_IRQ_28,             // !< RISCV Local Interrupt request 28 vector
    ISR_VECTOR_IRQ_29,             // !< RISCV Local Interrupt request 29 vector
    ISR_VECTOR_IRQ_30,             // !< RISCV Local Interrupt request 30 vector
    ISR_VECTOR_IRQ_31,             // !< RISCV Local Interrupt request 31 vector

    ISR_VECTOR_IRQ_32,             // !< RISCV External Interrupt request 32 vector
    ISR_VECTOR_IRQ_33,             // !< RISCV External Interrupt request 33 vector
    ISR_VECTOR_IRQ_34,             // !< RISCV External Interrupt request 34 vector
    ISR_VECTOR_IRQ_35,             // !< RISCV External Interrupt request 35 vector
    ISR_VECTOR_IRQ_36,             // !< RISCV External Interrupt request 36 vector
    ISR_VECTOR_IRQ_37,             // !< RISCV External Interrupt request 37 vector
    ISR_VECTOR_IRQ_38,             // !< RISCV External Interrupt request 38 vector
    ISR_VECTOR_IRQ_39,             // !< RISCV External Interrupt request 39 vector
    ISR_VECTOR_IRQ_40,             // !< RISCV External Interrupt request 40 vector
    ISR_VECTOR_IRQ_41,             // !< RISCV External Interrupt request 41 vector
    ISR_VECTOR_IRQ_42,             // !< RISCV External Interrupt request 42 vector
    ISR_VECTOR_IRQ_43,             // !< RISCV External Interrupt request 43 vector
    ISR_VECTOR_IRQ_44,             // !< RISCV External Interrupt request 44 vector
    ISR_VECTOR_IRQ_45,             // !< RISCV External Interrupt request 45 vector
    ISR_VECTOR_IRQ_46,             // !< RISCV External Interrupt request 46 vector
    ISR_VECTOR_IRQ_47,             // !< RISCV External Interrupt request 47 vector
    ISR_VECTOR_IRQ_48,             // !< RISCV External Interrupt request 48 vector
    ISR_VECTOR_IRQ_49,             // !< RISCV External Interrupt request 49 vector
    ISR_VECTOR_IRQ_50,             // !< RISCV External Interrupt request 50 vector
    ISR_VECTOR_IRQ_51,             // !< RISCV External Interrupt request 51 vector
    ISR_VECTOR_IRQ_52,             // !< RISCV External Interrupt request 52 vector
    ISR_VECTOR_IRQ_53,             // !< RISCV External Interrupt request 53 vector
    ISR_VECTOR_IRQ_54,             // !< RISCV External Interrupt request 54 vector
    ISR_VECTOR_IRQ_55,             // !< RISCV External Interrupt request 55 vector
    ISR_VECTOR_IRQ_56,             // !< RISCV External Interrupt request 56 vector
    ISR_VECTOR_IRQ_57,             // !< RISCV External Interrupt request 57 vector
    ISR_VECTOR_IRQ_58,             // !< RISCV External Interrupt request 58 vector
    ISR_VECTOR_IRQ_59,             // !< RISCV External Interrupt request 59 vector
    ISR_VECTOR_IRQ_60,             // !< RISCV External Interrupt request 60 vector
    ISR_VECTOR_IRQ_61,             // !< RISCV External Interrupt request 61 vector
    ISR_VECTOR_IRQ_62,             // !< RISCV External Interrupt request 62 vector
    ISR_VECTOR_IRQ_63,             // !< RISCV External Interrupt request 63 vector
    ISR_VECTOR_IRQ_64,             // !< RISCV External Interrupt request 64 vector
    ISR_VECTOR_IRQ_65,             // !< RISCV External Interrupt request 65 vector
    ISR_VECTOR_IRQ_66,             // !< RISCV External Interrupt request 66 vector
    ISR_VECTOR_IRQ_67,             // !< RISCV External Interrupt request 67 vector
    ISR_VECTOR_IRQ_68,             // !< RISCV External Interrupt request 68 vector
    ISR_VECTOR_IRQ_69,             // !< RISCV External Interrupt request 69 vector
    ISR_VECTOR_IRQ_70,             // !< RISCV External Interrupt request 70 vector
    ISR_VECTOR_IRQ_71,             // !< RISCV External Interrupt request 71 vector
    ISR_VECTOR_IRQ_72,             // !< RISCV External Interrupt request 72 vector
    ISR_VECTOR_IRQ_73,             // !< RISCV External Interrupt request 73 vector
    ISR_VECTOR_IRQ_74,             // !< RISCV External Interrupt request 74 vector
    ISR_VECTOR_IRQ_75,             // !< RISCV External Interrupt request 75 vector
    ISR_VECTOR_IRQ_76,             // !< RISCV External Interrupt request 76 vector
    ISR_VECTOR_IRQ_77,             // !< RISCV External Interrupt request 77 vector
    ISR_VECTOR_IRQ_78,             // !< RISCV External Interrupt request 78 vector
    ISR_VECTOR_IRQ_79,             // !< RISCV External Interrupt request 79 vector
    ISR_VECTOR_IRQ_80,             // !< RISCV External Interrupt request 80 vector
    ISR_VECTOR_IRQ_81,             // !< RISCV External Interrupt request 81 vector
} isr_vector_t;

/**
 * @brief  Get the ram exception table address.
 * @return The ram exception table address.
 */
const isr_function* isr_get_ramexceptiontable_addr(void);

typedef struct {
    uint32_t mstatus;
    uint32_t mepc;
    uint32_t tp;    /* X4 */
    uint32_t sp;    /* X2 */

    uint32_t s11;   /* X27 */
    uint32_t s10;   /* X26 */
    uint32_t s9;    /* X25 */
    uint32_t s8;    /* X24 */
    uint32_t s7;    /* X23 */
    uint32_t s6;    /* X22 */
    uint32_t s5;    /* X21 */
    uint32_t s4;    /* X20 */
    uint32_t s3;    /* X19 */
    uint32_t s2;    /* X18 */

    uint32_t s1;    /* X9 */
    uint32_t s0;    /* X8 */

    uint32_t t6;    /* X31 */
    uint32_t t5;    /* X30 */
    uint32_t t4;    /* X29 */
    uint32_t t3;    /* X28 */

    uint32_t a7;    /* X17 */
    uint32_t a6;    /* X16 */
    uint32_t a5;    /* X15 */
    uint32_t a4;    /* X14 */
    uint32_t a3;    /* X13 */
    uint32_t a2;    /* X12 */
    uint32_t a1;    /* X11 */
    uint32_t a0;    /* X10 */

    uint32_t t2;    /* X7 */
    uint32_t t1;    /* X6 */
    uint32_t t0;    /* X5 */

    uint32_t ra;    /* X1 */
} task_context_t;

typedef struct {
    uint32_t ccause;
    uint32_t mcause;
    uint32_t mtval;
    uint32_t gp;
    task_context_t task_context;
} exc_context_t;

typedef struct exc_info {
    uint16_t phase;
    uint16_t type;
    uint32_t fault_addr;
    uint32_t thrd_pid;
    uint16_t nest_cnt;
    uint16_t reserved;
    exc_context_t *context;
} exc_info_t;

/**
 * @brief  Reserve handler process.
 */
void reserve_handler(void);

/**
 * @brief  Default handler process.
 */
void default_handler(void);

/**
 * @brief  Print exception info.
 * @param  exc Exception info.
 */
void exc_info_display(const exc_info_t *exc);
/**
  * @}
  */
#endif
