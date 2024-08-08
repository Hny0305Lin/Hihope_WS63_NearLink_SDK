/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: interrupt
 *
 * Create: 2021-12-16
 */

#include <los_hwi.h>
#include <hal_hwi.h>
#include <linux/interrupt.h>
#include "soc_osal.h"
#include "osal_errno.h"
#include "osal_inner.h"

#define OSAL_IRQ_FLAG_PRI1    1
#define OSAL_IRQ_FLAG_PRI2    2
#define OSAL_IRQ_FLAG_PRI3    3
#define OSAL_IRQ_FLAG_PRI4    4
#define OSAL_IRQ_FLAG_PRI5    5
#define OSAL_IRQ_FLAG_PRI6    6
#define OSAL_IRQ_FLAG_PRI7    7
#define OSAL_IRQ_FLAG_PRI_MASK  0x7
#define OSAL_IRQ_FLAG_NOT_IN_FLASH 0x10

#define OSAL_IRQ_FLAG_DEFAULT    OSAL_IRQ_FLAG_NOT_IN_FLASH

#define MAX_IRQ_NAME_LEN 32

#ifndef NULL
#define NULL ((void *)0)
#endif

void *osal_irq_get_private_dev(void *param_dev)
{
    if (param_dev == NULL) {
        osal_log("param_dev is NULL!\n");
        return NULL;
    }
    return ((HWI_IRQ_PARAM_S *)param_dev)->pDevId;
}

int osal_irq_request(unsigned int irq, osal_irq_handler handler, osal_irq_handler thread_fn,
    const char *name, void *dev)
{
    if (handler == NULL) {
        osal_log("handler is NULL !\n");
        return OSAL_FAILURE;
    }

    HWI_IRQ_PARAM_S param = {
        .pDevId = dev,
        .pName = name,
        .swIrq = irq,
    };
    int ret = LOS_HwiCreate(irq, OS_HWI_PRIO_IGNORE, 0, (HWI_PROC_FUNC)handler, &param);
    if (ret != LOS_OK) {
        osal_log("LOS_HwiCreate failed! irq[%u] ret = %#x.\n", irq, ret);
        return OSAL_FAILURE;
    }

    return OSAL_SUCCESS;
}

void osal_irq_free(unsigned int irq, void *dev)
{
    osal_unused(dev);
    int ret = LOS_HwiDelete(irq, 0);
    if (ret != LOS_OK) {
        osal_log("LOS_HwiDelete failed! ret = %#x.\n", ret);
    }
}

int osal_irq_set_affinity(unsigned int irq, const char *name, int cpu_mask)
{
    return 0;
}

int osal_in_interrupt(void)
{
    return (OS_INT_ACTIVE);
}

void osal_irq_enable(unsigned int irq)
{
    unsigned int ret = LOS_HwiEnable(irq);
    if (ret != LOS_OK) {
        if (ret == OS_ERRNO_HWI_HWINUM_UNCREATE) {
            osal_log("OS_ERRNO_HWI_HWINUM_UNCREATE!\n");
        } else {
            osal_log("LOS_HwiDelete failed! ret = %#x.\n", ret);
        }
    }
    dsb();
}

/**
 * @par Description:
 * Disable the corresponding interrupt mask of the interrupt controller,
 * so that the interrupt source can be sent to the CPU.
 *
 * @attention
 * This function depends on the hardware implementation of the interrupt controller.
 */
void osal_irq_disable(unsigned int irq)
{
    LOS_HwiDisable(irq);
    dsb();
}

unsigned int osal_irq_lock(void)
{
    unsigned int ret = LOS_IntLock();
#ifdef OSAL_IRQ_RECORD_DEBUG
    osal_irq_record(IRQ_LOCK, (td_u32)__builtin_return_address(0), ret);
#endif
    dsb();
    return ret;
}

unsigned int osal_irq_unlock(void)
{
#ifdef OSAL_IRQ_RECORD_DEBUG
    osal_irq_record(IRQ_UNLOCK, (td_u32)__builtin_return_address(0), 0);
#endif
    unsigned int  ret = LOS_IntUnLock();
    dsb();
    return ret;
}

void osal_irq_restore(unsigned int irq_status)
{
#ifdef OSAL_IRQ_RECORD_DEBUG
    osal_irq_record(IRQ_RESTORE, (td_u32)__builtin_return_address(0), irq_status);
#endif
    LOS_IntRestore(irq_status);
    dsb();
}

int osal_irq_set_priority(unsigned int irq, unsigned short priority)
{
    unsigned int ret = LOS_HwiSetPriority(irq, priority);
    if (ret != LOS_OK) {
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}

unsigned int osal_irq_clear(unsigned int vector)
{
    unsigned int ret = LOS_HwiClear(vector);
    if (ret != LOS_OK) {
        return OSAL_FAILURE;
    }
    dsb();
    return OSAL_SUCCESS;
}

/* tasklet is running only on one CPU simultaneously */
int osal_tasklet_init(osal_tasklet *tasklet)
{
    osal_unused(tasklet);
    osal_log("not supported.\n");
    return 0;
}

/*
 * tasklet update should be called after tasklet init
 **/
int osal_tasklet_update(osal_tasklet *tasklet)
{
    osal_unused(tasklet);
    return 0;
}

/*
 * add tasklet to tasklet_osal_vec and start tasklet
 **/
int osal_tasklet_schedule(osal_tasklet *tasklet)
{
    osal_unused(tasklet);
    return 0;
}

/* kill tasklet */
int osal_tasklet_kill(osal_tasklet *tasklet)
{
    osal_unused(tasklet);
    osal_log("not supported.\n");
    return 0;
}
