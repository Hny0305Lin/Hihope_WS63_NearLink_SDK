/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: osal adapt debug
 * Create: 2023-5-17
 */
#ifdef OSAL_IRQ_RECORD_DEBUG
#ifndef __OSAL_ADAPT_DEBUG_H__
#define __OSAL_ADAPT_DEBUG_H__

#include "td_type.h"
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define RECORD_CNT_MAX 5

typedef enum {
    IRQ_LOCK = 0,
    IRQ_UNLOCK,
    IRQ_RESTORE,
    IRQ_TYPE_BUFF
} irq_type_enum;

typedef struct record {
    td_u32 caller[IRQ_TYPE_BUFF][RECORD_CNT_MAX];
    td_u32 stauts[IRQ_TYPE_BUFF][RECORD_CNT_MAX];
    td_u32 idx[IRQ_TYPE_BUFF];
} debug_irq_record;

debug_irq_record *osal_get_irq_record(void);
void osal_irq_record(irq_type_enum type, td_u32 caller, td_u32 stauts);
void osal_clear_irq_record(void);
void osal_print_irq_record(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
#endif /* #ifdef OSAL_IRQ_RECORD_DEBUG */