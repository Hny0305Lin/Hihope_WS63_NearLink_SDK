/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: osal adapt header \n
 *
 */

#ifndef OSAL_ADAPT_H
#define OSAL_ADAPT_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef void (*print_cb_t)(const char *fmt, ...);

extern void test_suite_uart_send(const char *str);
void osal_dcache_flush_all(void);
void osal_register_print(print_cb_t print);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
