/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: shell cmds APIs implementation about ripple
 * Author: none
 * Create: 2020
 */

#ifndef LWIP_RIPPLE_SHELL_H
#define LWIP_RIPPLE_SHELL_H
#include "lwip/opt.h"

#ifdef __cplusplus
  extern "C" {
#endif

#if LWIP_RIPPLE
u32_t os_shell_rpl(int argc, const char **argv);
u32_t os_shell_l2test(int argc, const char **argv);
u32_t os_rte_debug(int argc, const char **argv);
#if LWIP_NAT64
u32_t os_shell_nat64_debug(int argc, const char **argv);
#endif
#if LWIP_IP6IN4
u32_t os_shell_ip6in4(int argc, const char **argv);
#endif
#endif /* LWIP_RIPPLE */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_RIPPLE_SHELL_H */
