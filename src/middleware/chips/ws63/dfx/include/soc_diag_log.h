/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag print log
 */

#ifndef SOC_DIAG_LOG_H
#define SOC_DIAG_LOG_H

#include "soc_diag_wdk.h"
#include "diag_oam_log.h"
#include "log_def.h"
#include "log_module_id.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DIAG_LOG_LEVEL_ALERT    0
#define DIAG_LOG_LEVEL_FATAL    1
#define DIAG_LOG_LEVEL_ERROR    2
#define DIAG_LOG_LEVEL_WARN     3
#define DIAG_LOG_LEVEL_NOTICE   4
#define DIAG_LOG_LEVEL_INFO     5
#define DIAG_LOG_LEVEL_DBG      6
#define DIAG_LOG_LEVEL_TRACE    7

#ifndef CORE_ID
#define CORE_ID 0
#endif

#ifndef THIS_MOD_ID
#define THIS_MOD_ID 0
#endif

#if defined(HAVE_PCLINT_CHECK)
#define check_default_id(id) (id)
#else
#define check_default_id(id) (((id) == 0) ? __LINE__ : (id))
#endif

#define var_args_max_12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, ...)     a12
#define var_args_cnt(unused, args...)   var_args_max_12(unused, ##args, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

/** msg_id:
 * |-------------------------------------------------------------------------------------------------------|
 * |-H--------------------------------------------- 32 bit ----------------------------------------------L-|
 * |  1 bit(31)  |    4 bits(30 ~ 27)  |    10 bits(26 ~ 17)    |      14 bits(16 ~ 3)      | 3 bits(2 ~ 0)|
 * |  log_flg    |       mod_id        |        file_id         |          line No.         |    level     |
 * |-------------------------------------------------------------------------------------------------------|
 */
#define diag_log_msg_mk_id_e(id) (((uint32_t)(1UL << 31)) + (((uint32_t)(THIS_MOD_ID << 27)) & 0x78000000) \
    + (((uint32_t)(THIS_FILE_ID << 17)) & 0x7fe0000) + ((((uint32_t)(check_default_id(id))) << 3) & 0x1fff8) \
    + (DIAG_LOG_LEVEL_ERROR & 0x7))

#define diag_log_msg_mk_id_w(id) (((uint32_t)(1UL << 31)) + (((uint32_t)(THIS_MOD_ID << 27)) & 0x78000000) \
    + (((uint32_t)(THIS_FILE_ID << 17)) & 0x7fe0000) + ((((uint32_t)(check_default_id(id))) << 3) & 0x1fff8) \
    + (DIAG_LOG_LEVEL_WARN & 0x7))

#define diag_log_msg_mk_id_i(id) (((uint32_t)(1UL << 31)) + (((uint32_t)(THIS_MOD_ID << 27)) & 0x78000000) \
    + (((uint32_t)(THIS_FILE_ID << 17)) & 0x7fe0000) + ((((uint32_t)(check_default_id(id))) << 3) & 0x1fff8) \
    + (DIAG_LOG_LEVEL_INFO & 0x7))

#define diag_log_msg_mk_id_d(id) (((uint32_t)(1UL << 31)) + (((uint32_t)(THIS_MOD_ID << 27)) & 0x78000000) \
    + (((uint32_t)(THIS_FILE_ID << 17)) & 0x7fe0000) + ((((uint32_t)(check_default_id(id))) << 3) & 0x1fff8) \
    + (DIAG_LOG_LEVEL_DBG & 0x7))

#define check_default_mod_id(id) (((id) == 0) ? THIS_MOD_ID : (id))
#define diag_log_msg_mk_mod_id(src_mod) ((uint32_t)(((uint32_t)(check_default_mod_id(src_mod))) | \
    ((uint32_t)(CORE_ID)) << 24))

#if (defined(DIAG_PRINT_TO_SHELL))
#define diag_io_print(fmt, arg...) printf("[%s:%d] "fmt"\r\n", __func__, __LINE__, ##arg)
#else
#define diag_io_print(fmt, arg...)
#endif

#ifndef DIAG_PRINT_TO_SHELL
#define uapi_diag_error_log0(id, fmt) \
    oam_log_print0_press(diag_log_msg_mk_id_e(id))
#define uapi_diag_error_log1(id, fmt, p1) \
    oam_log_print1_press(diag_log_msg_mk_id_e(id), p1)
#define uapi_diag_error_log2(id, fmt, p1, p2) \
    oam_log_print2_press(diag_log_msg_mk_id_e(id), p1, p2)
#define uapi_diag_error_log3(id, fmt, p1, p2, p3) \
    do { \
        zdiag_log_msg3_t log_msg = {p1, p2, p3}; \
        oam_log_print3_press(diag_log_msg_mk_id_e(id), &log_msg); \
    } while (0)
#define uapi_diag_error_log4(id, fmt, p1, p2, p3, p4) \
    do { \
        zdiag_log_msg4_t log_msg = {p1, p2, p3, p4}; \
        oam_log_print4_press(diag_log_msg_mk_id_e(id), &log_msg); \
    } while (0)
#define uapi_diag_error_logx(id, fmt, args...) \
    do { \
        oam_log_print_alterable_press(diag_log_msg_mk_id_e(id), \
            var_args_cnt(unused, ##args), ##args); \
    } while (0)
#define uapi_diag_error_log_buff(id, log_info, data_size, fmt, type) \
    oam_log_print_buff_press(diag_log_msg_mk_id_e(id), log_info, data_size)

#define uapi_diag_warning_log0(id, fmt) \
    oam_log_print0_press(diag_log_msg_mk_id_w(id))
#define uapi_diag_warning_log1(id, fmt, p1) \
    oam_log_print1_press(diag_log_msg_mk_id_w(id), p1)
#define uapi_diag_warning_log2(id, fmt, p1, p2) \
    oam_log_print2_press(diag_log_msg_mk_id_w(id), p1, p2)
#define uapi_diag_warning_log3(id, fmt, p1, p2, p3) \
    do { \
        zdiag_log_msg3_t log_msg = {p1, p2, p3}; \
        oam_log_print3_press(diag_log_msg_mk_id_w(id), &log_msg); \
    } while (0)
#define uapi_diag_warning_log4(id, fmt, p1, p2, p3, p4) \
    do { \
        zdiag_log_msg4_t log_msg = {p1, p2, p3, p4}; \
        oam_log_print4_press(diag_log_msg_mk_id_w(id), &log_msg); \
    } while (0)
#define uapi_diag_warning_logx(id, fmt, args...) \
    do { \
        oam_log_print_alterable_press(diag_log_msg_mk_id_w(id), \
            var_args_cnt(unused, ##args), ##args); \
    } while (0)
#define uapi_diag_warning_log_buff(id, log_info, data_size, fmt, type) \
    oam_log_print_buff_press(diag_log_msg_mk_id_w(id), log_info, data_size)

#define uapi_diag_info_log0(id, fmt)  \
    oam_log_print0_press(diag_log_msg_mk_id_i(id))
#define uapi_diag_info_log1(id, fmt, p1)  \
    oam_log_print1_press(diag_log_msg_mk_id_i(id), (uint32_t)(p1))
#define uapi_diag_info_log2(id, fmt, p1, p2)  \
    oam_log_print2_press(diag_log_msg_mk_id_i(id), (uint32_t)(p1), (uint32_t)(p2))
#define uapi_diag_info_log3(id, fmt, p1, p2, p3)  \
    do { \
        zdiag_log_msg3_t log_msg = {(uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3)}; \
        oam_log_print3_press(diag_log_msg_mk_id_i(id), &log_msg); \
    } while (0)
#define uapi_diag_info_log4(id, fmt, p1, p2, p3, p4) \
    do { \
        zdiag_log_msg4_t log_msg = {(uint32_t)(p1), (uint32_t)(p2), (uint32_t)(p3), (uint32_t)(p4)}; \
        oam_log_print4_press(diag_log_msg_mk_id_i(id), &log_msg); \
    } while (0)
#define uapi_diag_info_logx(id, fmt, args...) \
    do { \
        oam_log_print_alterable_press(diag_log_msg_mk_id_i(id), \
            var_args_cnt(unused, ##args), ##args); \
    } while (0)
#define uapi_diag_info_log_buff(id, log_info, data_size, fmt, type) \
    oam_log_print_buff_press(diag_log_msg_mk_id_i(id), log_info, data_size)

#define uapi_diag_debug_log0(id, fmt)  \
    oam_log_print0_press(diag_log_msg_mk_id_d(id))
#define uapi_diag_debug_log1(id, fmt, p1)  \
    oam_log_print1_press(diag_log_msg_mk_id_d(id), p1)
#define uapi_diag_debug_log2(id, fmt, p1, p2)  \
    oam_log_print2_press(diag_log_msg_mk_id_d(id), p1, p2)
#define uapi_diag_debug_log3(id, fmt, p1, p2, p3)  \
    do { \
        zdiag_log_msg3_t log_msg = {p1, p2, p3}; \
        oam_log_print3_press(diag_log_msg_mk_id_i(id), &log_msg); \
    } while (0)
#define uapi_diag_debug_log4(id, fmt, p1, p2, p3, p4) \
    do { \
        zdiag_log_msg4_t log_msg = {p1, p2, p3, p4}; \
        oam_log_print4_press(diag_log_msg_mk_id_d(id), &log_msg); \
    } while (0)
#define uapi_diag_debug_logx(id, fmt, args...) \
    do { \
        oam_log_print_alterable_press(diag_log_msg_mk_id_d(id), \
            var_args_cnt(unused, ##args), ##args); \
    } while (0)
#define uapi_diag_debug_log_buff(id, log_info, data_size, fmt, type) \
    oam_log_print_buff_press(diag_log_msg_mk_id_d(id), log_info, data_size)

#else /* DIAG_PRINT */

#define uapi_diag_error_log0(id, fmt) diag_io_print("[ERROR]"fmt"\r\n")
#define uapi_diag_error_log1(id, fmt, p1) diag_io_print("[ERROR]"fmt" %d\r\n", p1)
#define uapi_diag_error_log2(id, fmt, p1, p2) diag_io_print("[ERROR]"fmt" %d  %d\r\n", p1, p2)
#define uapi_diag_error_log3(id, fmt, p1, p2, p3) diag_io_print("[ERROR]"fmt" %d %d %d\r\n", p1, p2, p3)
#define uapi_diag_error_log4(id, fmt, p1, p2, p3, p4) diag_io_print("[ERROR]"fmt" %d %d %d %d\r\n", p1, p2, p3, p4)
#define uapi_diag_error_logx(id, fmt, args...) diag_io_print(fmt, ##args)
#define uapi_diag_error_log_buff(id, log_info, data_size, fmt, type)

#define uapi_diag_warning_log0(id, fmt) diag_io_print("[WARNING]"fmt"\r\n")
#define uapi_diag_warning_log1(id, fmt, p1) diag_io_print("[WARNING]"fmt" %d\r\n", p1)
#define uapi_diag_warning_log2(id, fmt, p1, p2) diag_io_print("[WARNING]"fmt" %d %d\r\n", p1, p2)
#define uapi_diag_warning_log3(id, fmt, p1, p2, p3) diag_io_print("[WARNING]"fmt" %d %d %d\r\n", p1, p2, p3)
#define uapi_diag_warning_log4(id, fmt, p1, p2, p3, p4) diag_io_print("[WARNING]"fmt" %d %d %d %d\r\n", p1, p2, p3, p4)
#define uapi_diag_warning_logx(id, fmt, args...) diag_io_print(fmt, ##args)
#define uapi_diag_warning_log_buff(id, log_info, data_size, fmt, type)

#define uapi_diag_info_log0(id, fmt) diag_io_print("[INFO]"fmt"\r\n")
#define uapi_diag_info_log1(id, fmt, p1) diag_io_print("[INFO]"fmt" %d\r\n", p1)
#define uapi_diag_info_log2(id, fmt, p1, p2) diag_io_print("[INFO]"fmt" %d %d\r\n", p1, p2)
#define uapi_diag_info_log3(id, fmt, p1, p2, p3) diag_io_print("[INFO]"fmt" %d %d %d\r\n", p1, p2, p3)
#define uapi_diag_info_log4(id, fmt, p1, p2, p3, p4) diag_io_print("[INFO]"fmt" %d %d %d %d\r\n", p1, p2, p3, p4)
#define uapi_diag_info_logx(id, fmt, args...) diag_io_print(fmt, ##args)
#define uapi_diag_info_log_buff(id, log_info, data_size, fmt, type)

#define uapi_diag_debug_log0(id, fmt) diag_io_print("[INFO]"fmt"\r\n")
#define uapi_diag_debug_log1(id, fmt, p1) diag_io_print("[INFO]"fmt" %d\r\n", p1)
#define uapi_diag_debug_log2(id, fmt, p1, p2) diag_io_print("[INFO]"fmt" %d %d\r\n", p1, p2)
#define uapi_diag_debug_log3(id, fmt, p1, p2, p3) diag_io_print("[INFO]"fmt" %d %d %d\r\n", p1, p2, p3)
#define uapi_diag_debug_log4(id, fmt, p1, p2, p3, p4) diag_io_print("[INFO]"fmt" %d %d %d %d\r\n", p1, p2, p3, p4)
#define uapi_diag_debug_logx(id, fmt, args...) diag_io_print(fmt, ##args)
#define uapi_diag_debug_log_buff(id, log_info, data_size, fmt, type)

#endif /* end DIAG_PRINT */

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* SOC_DIAG_LOG_H */
