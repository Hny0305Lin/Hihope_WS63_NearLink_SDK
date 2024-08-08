/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: netstat shell cmd API implementation
 * Author: none
 * Create: 2020
 */

#ifndef LWIP_NETSTAT_H
#define LWIP_NETSTAT_H
#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

u32_t os_shell_netstat(int argc, const char **argv);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_NETSTAT_H */
