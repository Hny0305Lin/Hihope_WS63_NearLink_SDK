/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: debug
 *
 * Create: 2021-12-16
 */


#include <los_printf.h>
#include <los_config.h>
#include <los_printf_pri.h>
#include "soc_osal.h"
#include "los_exc.h"
#ifdef SW_UART_DEBUG
#include "debug_print.h"
#endif

void osal_dump_stack(void)
{
    OsBackTrace();
}

void osal_panic(const char *fmt, const char *fun, int line, const char *cond)
{
    LOS_Panic(fmt, fun, line, cond);
}

void osal_printk(const char *fmt, ...)
{
    va_list args;

    if (fmt == NULL) {
        return;
    }

    va_start(args, fmt);
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    ConsoleVprintf(fmt, args);
#else
    OsVprintf(fmt, args, CONSOLE_OUTPUT);
#endif
    va_end(args);
}

void osal_flush_cache(void)
{
    ArchDCacheFlush();
}