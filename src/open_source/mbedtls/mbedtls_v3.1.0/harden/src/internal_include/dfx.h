/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#include "mbedtls/ecp.h"
#include "mbedtls/platform.h"

#define PRINT_LINE_LEN                     16

#define mbedtls_harden_log_err(fmt, ...)   \
    mbedtls_printf("[%s:%d]" "HARDEN MBED ERR! : " fmt, __func__, __LINE__, ##__VA_ARGS__)

#define unused(x)                          (void)(x)

#if defined(MBED_HARDEN_DEBUG)
#define mbedtls_harden_log_dbg             mbedtls_printf
#else
#define mbedtls_harden_log_dbg(...)        (void)0
#endif

#define mbedtls_harden_log_func_enter()    mbedtls_harden_log_dbg("%s ===>Enter\n", __func__)
#define mbedtls_harden_log_func_exit()     mbedtls_harden_log_dbg("%s <===Exit\n", __func__)

#define MBED_HARDEN_DEBUG_BREAKPOINT       1

#if defined(MBED_HARDEN_DEBUG_BREAKPOINT)
#define mbedtls_harden_log_breakpoint()    \
    mbedtls_harden_log_dbg("your code go here %s--> LINE = %d!\n", __func__,__LINE__)
#endif

void mbedtls_dump_grp_id(mbedtls_ecp_group_id group_id);

void mbedtls_dump_hex(const char* name, const unsigned char* buf, unsigned int buf_len);

void mbedtls_dump_mpi(const char* name, const mbedtls_mpi *mpi);

void mbedtls_dump_point(const char* name, const mbedtls_ecp_point *point);