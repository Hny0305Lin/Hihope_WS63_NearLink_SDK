/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: ifconfig shell cmd API implementation
 * Author: none
 * Create: 2020
 */

#ifndef LWIP_IFCONFIG_H
#define LWIP_IFCONFIG_H
#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

u32_t lwip_ifconfig(int argc, const char **argv);

u32_t os_shell_netif(int argc, const char **argv);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IFCONFIG_H */
