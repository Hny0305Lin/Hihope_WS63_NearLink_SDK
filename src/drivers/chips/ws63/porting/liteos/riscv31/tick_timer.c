/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: RISCV31 Machine timer config for LiteOS
 *
 * Create: 2021-9-27
 */
#include "soc_osal.h"
#include "hal_hwi.h"
#include "los_hwi.h"
#include "los_tick_pri.h"
#include "los_hwi_pri.h"
#include "chip_io.h"
#include "arch_barrier.h"
#include "oal_interface.h"
#include "platform_core.h"
#include "idle_config.h"
#include "core.h"
#include "interrupt_porting.h"
#include "systick.h"
#include "tick_timer.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MACHINE_INT_NUM                 NUM_HAL_INTERRUPT_TIMER

#define MACHINE_TIMER_BASE              TICK_TIMER_BASE_ADDR
#define MACHINE_TIMER_LOAD_COUNT        0x00
#define MACHINE_TIMER_LOAD_COUNT_L      0x00
#define MACHINE_TIMER_LOAD_COUNT_H      0x04
#define MACHINE_TIMER_CURRENT_VALUE_L   0x08
#define MACHINE_TIMER_CURRENT_VALUE_H   0x0C
#define MACHINE_TIMER_CONTROL_REG       0x10
#define MACHINE_TIMER_EOI               0x14
#define MACHINE_TIMER_INT_STATUS        0x18

#define MACHINE_TIMER_ENABLE_USRMODE    0x03

tick_handle_hook g_tick_handle_hook = NULL;
void hal_tick_handle_register(tick_handle_hook hook)
{
    g_tick_handle_hook = hook;
}

static inline void machine_timer_init(void)
{
    // before config timer, disable it
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), 0);

    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_LOAD_COUNT), OS_CYCLE_PER_TICK);
    // user define mode and enable it
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_ENABLE_USRMODE);
}

static inline uint32_t machine_timer_get_cur_count(void)
{
    return readl(MACHINE_TIMER_BASE + MACHINE_TIMER_CURRENT_VALUE_L);
}

static inline uint32_t machine_timer_get_load_count(void)
{
    return readl(MACHINE_TIMER_BASE + MACHINE_TIMER_LOAD_COUNT);
}

static int32_t HalTickEntry(int i, void *p)
{
    UNUSED(i);
    UNUSED(p);
    // clear the timer interrupt
    writel(MACHINE_TIMER_BASE + MACHINE_TIMER_EOI, 1);
    int_clear_pending_irq(MACHINE_INT_NUM);
    OsTickHandler();
    if (g_tick_handle_hook != NULL) {
        g_tick_handle_hook();
    }
    return 0;
}

LITE_OS_SEC_TEXT_INIT void HalClockStart(void)
{
    int32_t ret;
    uint32_t lock_int_save;

    if ((OS_SYS_CLOCK == 0) ||
        (LOSCFG_BASE_CORE_TICK_PER_SECOND == 0) ||
        (OS_SYS_CLOCK < LOSCFG_BASE_CORE_TICK_PER_SECOND )) {
        return;
    }

    lock_int_save = LOS_IntLock();
    ret = osal_irq_request(MACHINE_INT_NUM, (osal_irq_handler)HalTickEntry, NULL, NULL, NULL);
    if (ret != OSAL_SUCCESS) {
        PRINT_ERR("%s %d: osal_irq_request ret:0x%x\n", __FUNCTION__, __LINE__, ret);
    }
    osal_irq_enable(MACHINE_INT_NUM);
    machine_timer_init();
    idle_task_config();
    LOS_IntRestore(lock_int_save);
}

void HalClockInit(void)
{
    SET_SYS_CLOCK(OS_SYS_CLOCK);
}

void HalDelayUs(uint32_t usecs)
{
    uapi_systick_delay_us(usecs);
}

uint64_t HalClockGetCycles(void)
{
    uint64_t tick_count;
    uint64_t cycle;
    uint32_t cycle_in_tick;
    uint32_t lock_int_save;
    uint32_t load_tick;
    uint32_t cur_tick;

    lock_int_save = LOS_IntLock();
    tick_count = LOS_TickCountGet();
    load_tick = machine_timer_get_load_count();
    cur_tick = machine_timer_get_cur_count();
#if !CHIP_WS63
    uint8_t isPending = 0;
    if ((HalIrqPendingGet(MACHINE_INT_NUM, &isPending) == LOS_OK) && (isPending != 0)) {
        cur_tick = machine_timer_get_cur_count();
        tick_count++;
    }
#endif
    cycle_in_tick = load_tick - cur_tick;
    cycle = tick_count * load_tick + cycle_in_tick;
    LOS_IntRestore(lock_int_save);
    return cycle;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
