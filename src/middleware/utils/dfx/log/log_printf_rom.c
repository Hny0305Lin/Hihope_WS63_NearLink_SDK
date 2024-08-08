/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   LOG PRINTF MODULE
 * Author:
 * Create:
 */

#include <stdarg.h>
#include "log_printf.h"

typedef void (* comrepss_printf_cb)(uint32_t log_addr, uint32_t log_code, uint32_t log_header, va_list args);

comrepss_printf_cb g_compress_printf_cb = compress_printf_rom_callback;
#if CHIP_LIBRA || CHIP_SOCMN1 || CHIP_BS25 || CHIP_BRANDY
comrepss_printf_cb g_compress_log_no_print_cb = compress_log_no_print_rom_callback;
#endif
void compress_printf_in_rom(uint32_t log_addr, uint32_t log_header_user, uint32_t log_header_eng, ...)
{
    va_list args;
    va_start(args, log_header_eng);
    if (g_compress_printf_cb != NULL) {
        g_compress_printf_cb(log_addr, log_header_user, log_header_eng, args);
    }
    va_end(args);
}
#if CHIP_LIBRA || CHIP_SOCMN1 || CHIP_BS25 || CHIP_BRANDY
// The log is not printed when the log is a compress log.
void compress_log_no_print_in_rom(uint32_t log_addr, uint32_t log_header_user, uint32_t log_header_eng, ...)
{
    va_list args;
    va_start(args, log_header_eng);
    if (g_compress_log_no_print_cb != NULL) {
        g_compress_log_no_print_cb(log_addr, log_header_user, log_header_eng, args);
    }
    va_end(args);
}
#endif
