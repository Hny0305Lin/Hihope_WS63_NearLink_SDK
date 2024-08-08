/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides tsensor port \n
 *
 * History: \n
 * 2023-02-28， Create file. \n
 */

#include "soc_osal.h"
#include "common_def.h"
#include "tcxo.h"
#include "hal_tsensor_v151.h"
#include "chip_core_irq.h"
#include "tsensor_porting.h"

#define TSENSOR_BASE_ADDR  0x4400E000

static int tsensor_handler(int a, void *tmp)
{
    unused(a);
    unused(tmp);
    hal_tsensor_irq_handler();
    return 0;
}

uintptr_t tsensor_port_base_addr_get(void)
{
    return (uintptr_t)TSENSOR_BASE_ADDR;
}

void tsensor_port_register_hal_funcs(void)
{
    hal_tsensor_register_funcs(hal_tsensor_v151_funcs_get());
}

void tsensor_port_unregister_hal_funcs(void)
{
    hal_tsensor_unregister_funcs();
}

void tsensor_port_register_irq(void)
{
    osal_irq_request(TSENSOR_IRQN, tsensor_handler, NULL, NULL, NULL);
    osal_irq_enable(TSENSOR_IRQN);
}

void tsensor_port_unregister_irq(void)
{
    osal_irq_disable(TSENSOR_IRQN);
    osal_irq_free(TSENSOR_IRQN, NULL);
}

uint32_t tsensor_port_irq_lock(void)
{
    return osal_irq_lock();
}

void tsensor_port_irq_unlock(uint32_t flag)
{
    osal_irq_restore(flag);
}

uint64_t tsensor_port_get_ms(void)
{
    return uapi_tcxo_get_ms();
}