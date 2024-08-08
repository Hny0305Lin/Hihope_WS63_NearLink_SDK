/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: shell cmds APIs implementation about ping and ping6
 * Author: none
 * Create: 2020
 */

#ifndef LWIP_PING_H
#define LWIP_PING_H
#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined LWIP_SHELL_CMD_PING_RETRY_TIMES || defined __DOXYGEN__
#define LWIP_SHELL_CMD_PING_RETRY_TIMES     4
#endif

#if !defined LWIP_SHELL_CMD_PING_TIMEOUT || defined __DOXYGEN__
#define LWIP_SHELL_CMD_PING_TIMEOUT     2000
#endif

u32_t os_shell_ping(int argc, const char **argv);
u32_t os_shell_ping6(int argc, const char **argv);

#ifdef CONFIG_SIGMA_SUPPORT
typedef void (*ping_result_callback)(u32_t succ_cnt, u32_t failed_cnt);
void ping_add_ext_callback(ping_result_callback func);
#endif

#ifdef __cplusplus
}
#endif

#endif /* LWIP_PING_H */
