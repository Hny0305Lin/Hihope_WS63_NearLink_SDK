/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides timer port \n
 *
 * History: \n
 * 2022-06-06， Create file. \n
 */
#include "interrupt/osal_interrupt.h"
#include "chip_core_irq.h"
#include "common_def.h"
#include "hal_timer_v150.h"
#include "timer.h"
#include "platform_core.h"
#include "timer_porting.h"

static uintptr_t const g_timer_base_addr[TIMER_MAX_NUM] = {
    TIMER_0_BASE_ADDR,
    TIMER_1_BASE_ADDR,
    TIMER_2_BASE_ADDR,
};

uintptr_t timer_porting_comm_addr_get(void)
{
    return TIMER_BASE_ADDR;
}

uintptr_t timer_porting_base_addr_get(timer_index_t index)
{
    return g_timer_base_addr[index];
}

STATIC int timer0_irq_handler(int i, void *p)
{
    unused(i);
    unused(p);
    hal_timer_v150_interrupt_clear(TIMER_INDEX_0);
    osal_irq_clear(TIMER_0_IRQN);
    hal_timer_v150_irq_handler(TIMER_INDEX_0);
    return 0;
}

STATIC int timer1_irq_handler(int i, void *p)
{
    unused(i);
    unused(p);
    hal_timer_v150_interrupt_clear(TIMER_INDEX_1);
    osal_irq_clear(TIMER_1_IRQN);
    hal_timer_v150_irq_handler(TIMER_INDEX_1);
    return 0;
}

STATIC int timer2_irq_handler(int i, void *p)
{
    unused(i);
    unused(p);
    hal_timer_v150_interrupt_clear(TIMER_INDEX_2);
    osal_irq_clear(TIMER_2_IRQN);
    hal_timer_v150_irq_handler(TIMER_INDEX_2);
    return 0;
}

static osal_irq_handler const timer_irq_handler[TIMER_MAX_NUM] = {
    timer0_irq_handler,
    timer1_irq_handler,
    timer2_irq_handler,
};

void timer_port_register_irq(timer_index_t index, uint32_t id, uint16_t priority)
{
    osal_irq_disable(id);
    osal_irq_request(id, (osal_irq_handler)timer_irq_handler[index], NULL, NULL, NULL);
    osal_irq_set_priority(id, priority);
    osal_irq_enable(id);
}

void timer_port_unregister_irq(timer_index_t index, uint32_t id)
{
    unused(index);
    osal_irq_disable(id);
    osal_irq_free(id, NULL);
}

static uint32_t g_timer_clock_value = CONFIG_TIMER_CLOCK_VALUE;
void timer_porting_clock_value_set(uint32_t clock)
{
    g_timer_clock_value = clock;
}

uint32_t timer_porting_clock_value_get(void)
{
    return g_timer_clock_value;
}

uint64_t timer_porting_us_2_cycle(uint32_t us)
{
    uint32_t clock_value = timer_porting_clock_value_get();
    return ((uint64_t)(us) * (uint64_t)(clock_value / (MS_PER_S * US_PER_MS)));
}

uint32_t timer_porting_cycle_2_us(uint64_t cycle)
{
    uint32_t clock_value = timer_porting_clock_value_get();
    return (uint32_t)((cycle * (MS_PER_S * US_PER_MS)) / (clock_value));
}

uint64_t timer_porting_compensat_by_tcxo(uint64_t diff)
{
    if (diff < TIMER_COMPENSAT_1_CYCLE_BY_TCXO) {
        return 0;
    } else if (diff < TIMER_COMPENSAT_2_CYCLE_BY_TCXO) {
        return TIMER_1_CYCLE;
    } else if (diff < TIMER_COMPENSAT_3_CYCLE_BY_TCXO) {
        return TIMER_2_CYCLE;
    } else {
        return diff / TIMER_COMPENSAT_1_CYCLE_BY_TCXO;
    }
}