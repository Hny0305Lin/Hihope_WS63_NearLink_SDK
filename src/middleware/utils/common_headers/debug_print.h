/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description:  Basic DEBUG print
 * Author:
 * Create:  2020-11-6
 */
#ifndef LIB_COMMON_HEADERS_DEBUG_PRINT_H
#define LIB_COMMON_HEADERS_DEBUG_PRINT_H

#include "core.h"
#if defined(SW_RTT_DEBUG)
#include "SEGGER_RTT.h"
#endif
#if defined(MULTIPLEXING_UART) && defined(TEST_SUITE)
#include "test_suite_log.h"
#endif

#ifndef NEWLINE
#define NEWLINE "\r\n"
#endif
#define PRINT_BT(fmt, arg...)
#define uapi_at_printf(fmt, arg...)
#if defined(SW_UART_DEBUG) || defined(FACTORY_TEST)
void sw_debug_uart_init(uint32_t baud_rate);
void sw_debug_uart_deinit(void);
void print_str(const char *str, ...);
void print_exc(const char *str, ...);

#define print_init()
#if defined(BUILD_APPLICATION_ROM)
#if CORE == MASTER_BY_ALL
    #define PRINT(fmt, arg...) print_str("ROM|" fmt, ##arg)
#elif CORE == SECURITY
    #define PRINT(fmt, arg...) print_str("SEC ROM|" fmt, ##arg)
#endif

#elif defined(BUILD_APPLICATION_SSB)
#if CORE == MASTER_BY_ALL
    #define PRINT(fmt, arg...) print_str("SSB|" fmt, ##arg)
#elif CORE == SECURITY
    #define PRINT(fmt, arg...) print_str("SEC SSB|" fmt, ##arg)
#endif

#elif defined(MULTIPLEXING_UART) && defined(TEST_SUITE)
    #define PRINT(fmt, arg...) test_suite_uart_sendf("TESTSUITE|" fmt, ##arg)
#elif defined(BUILD_APPLICATION_STANDARD) || defined(TEST_SUITE) || defined(UNIT_TEST)
#if CORE == BT
    #define PRINT(fmt, arg...) print_str("BT|" fmt, ##arg)
#elif CORE == APPS
    #define PRINT(fmt, arg...) print_str("APP|" fmt, ##arg)
#elif CORE == SECURITY
    #define PRINT(fmt, arg...) print_str("SEC|" fmt, ##arg)
#elif CORE == GNSS
    #define PRINT(fmt, arg...) print_str("GNSS|" fmt, ##arg)
#elif CORE == CONTROL_CORE
    void test_suite_uart_sendf(const char *str, ...);
    #define PRINT(fmt, arg...) test_suite_uart_sendf(fmt, ##arg)
#else
    #define PRINT(fmt, arg...)
#endif

#elif CORE == WIFI
    #define PRINT(fmt, arg...)  print_str("WIFI|" fmt, ##arg)
#elif CORE == CONTROL_CORE
    #define PRINT(fmt, arg...)  print_str("CCORE|" fmt, ##arg)
#else
    #define PRINT(fmt, arg...)
#endif

#elif BOOT_ROM_DFR_PRINT == YES
void sw_debug_uart_init(uint32_t baud_rate);
void print_str(const char *str, ...);
#define PRINT(fmt, arg...) print_str("ROM|" fmt, ##arg)

#elif defined(SW_RTT_DEBUG)
#define LOG_PROTO(type, color, format, ...)            \
        SEGGER_RTT_printf(0, "  %s%s"format"%s",   \
                          color,                       \
                          type,                        \
                          ##__VA_ARGS__,               \
                          RTT_CTRL_RESET)

#define print_init() SEGGER_RTT_Init()
#define PRINT(fmt, arg...) LOG_PROTO("", "", fmt, ##arg)

#elif defined(TEST_SUITE)
void test_suite_uart_sendf(const char *str, ...);
#define print_init()
#define PRINT(fmt, arg...)  test_suite_uart_sendf(fmt, ##arg)

#else
#define print_init()
#define PRINT(fmt, arg...)

#endif
#endif
