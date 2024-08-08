/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: RISCV31 idle task config for LiteOS
 *
 * Create: 2021-10-20
 */
#include "idle_config.h"
#include "core.h"
#include "los_task.h"
#include "cmsis_os2.h"
#include "watchdog.h"

#if CORE == MASTER_BY_ALL
#if ((USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO) && defined(LIBLOG))
#include "log_oam_msg.h"
#endif
// #include "chip_watchdog.h"
#else
#include "watchdog.h"
#endif
#if (!defined(UNIT_TEST))
// #include "connectivity_sleep.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

static watchdog_port_idle_kick_callback g_wdt_kick_callback = NULL;
void watchdog_port_idle_kick_register(watchdog_port_idle_kick_callback callback)
{
    g_wdt_kick_callback = callback;
}

static void idle_task_process(void)
{
#if ((USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO) && defined(LIBLOG))
#if CORE == MASTER_BY_ALL
    log_oam_prase_message();
#endif
#endif

#ifndef TEST_SUITE
    // testsuite 版本不使能看门狗, 不进行踢狗
    if (g_wdt_kick_callback != NULL) {
        g_wdt_kick_callback();
    } else {
        uapi_watchdog_kick();
    }
#endif

#if (!defined(UNIT_TEST))
#if (ENABLE_LOW_POWER == YES)
    connectivity_sys_suppress_and_sleep();
#endif
#endif
}

void idle_task_config(void)
{
    LOS_IdleHandlerHookReg(idle_task_process);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
