/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  RISCV31 Core interrupt handler header file
 *
 * Create:  2021-07-24
 */
#ifndef RISCV31_INTERRUPT_HANDLER_H
#define RISCV31_INTERRUPT_HANDLER_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief  Interrupt0 handler function.
 */
void interrupt0_handler(void);

/**
 * @brief  Interrupt1 handler function.
 */
void interrupt1_handler(void);

/**
 * @brief  Interrupt2 handler function.
 */
void interrupt2_handler(void);

/**
 * @brief  Interrupt3 handler function.
 */
void interrupt3_handler(void);

/**
 * @brief  Interrupt4 handler function.
 */
void interrupt4_handler(void);

/**
 * @brief  Interrupt5 handler function.
 */
void interrupt5_handler(void);

/**
 * @brief  Local interrupt handler function.
 */
void local_interrupt_handler(void);

/**
 * @brief  Init local interrupt priority.
 */
void local_interrupt_priority_init(void);

/**
 * @brief  Get interrupt number.
 * @return Interrupt number.
 */
uint32_t interrupt_number_get(void);

/**
 * @brief Get interrupt count.
 * @return Interrupt count.
 */
uint32_t interrupt_count_get(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif