/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: ARCH interface for project
 *
 * Create:  2021-07-06
 */
#ifndef ARCH_PORT_H
#define ARCH_PORT_H

#include <stdint.h>
#include <stdbool.h>
#include "chip_core_irq.h"
#include "memory_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef USE_CMSIS_OS
#define irq_prio(irq_id) m_auc_int_pri[irq_id]
#else
#define irq_prio(irq_id) g_auc_int_pri[irq_id]
#endif

extern uint8_t m_auc_int_pri[BUTT_IRQN];
extern uint8_t g_auc_int_pri[BUTT_IRQN];
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
