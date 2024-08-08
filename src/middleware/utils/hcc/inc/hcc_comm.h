/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc commom types.
 * Author:
 * Create: 2021-05-13
 */

#ifndef HCC_COMMON_TYPE_H
#define HCC_COMMON_TYPE_H

#include "td_type.h"
#if defined(HSO_SUPPORT)
#include "log_def.h"
#include "log_module_id.h"
#endif

#if !(defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION))
#if defined(HSO_SUPPORT)
#if defined(CONFIG_HCC_SUPPORT_NON_OS)
#include "log_oam_logger.h"
#else
#include "soc_diag_util.h"
#endif
#endif
#include "debug_print.h"
#endif

/* HCC 打印接口 */
#ifdef CONFIG_HCC_DEBUG_PRINT
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define hcc_printf(fmt, arg...) printk("[HCC] "fmt, ##arg)
#elif defined(CONFIG_HCC_SUPPORT_NON_OS)
#define hcc_printf(fmt, arg...)         print_str("[HCC] "fmt, ##arg)
#else
#define hcc_printf(fmt, arg...)         PRINT("[HCC] "fmt, ##arg)
#endif

#else
#define hcc_printf(fmt, arg...)
#endif /* CONFIG_HCC_DEBUG_PRINT */

#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define hcc_debug(fmt, arg...) printk("[HCC] "fmt, ##arg)
#define hcc_printf_err_log(fmt, arg...) printk("[HCC] [ERROR] "fmt, ##arg)
#define hcc_dfx_print(fmt, arg...)         printk("[HCC] [DFX] "fmt, ##arg)
#elif defined(CONFIG_HCC_SUPPORT_NON_OS)
#define hcc_debug(fmt, arg...) print_str("[HCC] " fmt, ##arg)
#if defined(HSO_SUPPORT)
#define var_args_max_12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, ...)     a12
#define var_args_cnt(unused, args...)   var_args_max_12(unused, ##args, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define hcc_printf_err_log(fmt, arg...) pf_log_alter(0, 0, 0, fmt, var_args_cnt(unused, ##arg), ##arg)
#else
#define hcc_printf_err_log(fmt, arg...) print_str("[HCC] [ERROR] "fmt, ##arg)
#endif
#define hcc_dfx_print(fmt, arg...) print_str("[HCC] " fmt, ##arg)

#else
#define hcc_debug(fmt, arg...)         PRINT(fmt, ##arg)
#if defined(HSO_SUPPORT)
#define hcc_printf_err_log(fmt, arg...) uapi_diag_error_log(0, fmt, ##arg)
#else
#define hcc_printf_err_log(fmt, arg...) PRINT("[HCC] [ERROR] "fmt, ##arg)
#endif

#define hcc_dfx_print(fmt, arg...)        PRINT(fmt, ##arg)
#endif

#ifdef HCC_PRINT_PERFORM
#include "tcxo.h"
static inline td_u64 hcc_test_get_us(td_void)
{
    return uapi_tcxo_get_us();
}
#else
static inline td_u64 hcc_test_get_us(td_void)
{
    return 0;
}
#endif
static inline td_u64 hcc_calc_time_us(td_u64 start, td_u64 stop)
{
    return start > stop ? (start - stop) : (stop - start);
}
#endif /* HCC_COMMON_TYPE_H */
