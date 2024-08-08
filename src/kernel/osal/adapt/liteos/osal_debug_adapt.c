/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2022. All rights reserved.
 * Description: osal debug adapt
 * Author:
 * Create: 2022-05-16
 */


#include <los_printf.h>
#include <los_config.h>
#include <los_printf_pri.h>
#include "soc_osal.h"
#include "los_exc.h"
#include "std_def.h"
#include "platform_core.h"

#ifdef SW_UART_DEBUG
#include "debug_print.h"
#endif
#ifdef TEST_SUITE
#include "test_suite_uart.h"
#endif
#if defined(SW_RTT_DEBUG)
#include "SEGGER_RTT.h"
#endif

#ifndef SW_UART_CHIP_DEFINE
void UartPuts(const CHAR *s, uint32_t len, BOOL isLock)
{
    if ((s == NULL) || (strlen(s) == 0)) {
        return;
    }

    UNUSED(isLock);
#ifdef SW_UART_DEBUG
    print_str(s);
#elif defined(TEST_SUITE)
    test_suite_uart_send(s);
#elif defined(SW_RTT_DEBUG)
    SEGGER_RTT_Write(0, (const char *)s, len);
#else
    UNUSED(s);
    UNUSED(len);
#endif
}
#endif

void osal_dcache_flush_all(void)
{
    ArchDCacheFlush();
}
