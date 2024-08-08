/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: debug and display shell cmd API implementation
 * Author: none
 * Create: 2020
 */

#ifndef LWIP_DIAGNOSE_H
#define LWIP_DIAGNOSE_H
#include "lwip/opt.h"

#ifdef __cplusplus
  extern "C" {
#endif

#ifdef LWIP_DEBUG_INFO
u32_t os_shell_netdebug(int argc, const char **argv);
u32_t os_shell_ipdebug(int argc, const char **argv);
#endif /* LWIP_DEBUG_INFO */
u32_t os_shell_display_version(int argc, const char **argv);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_DIAGNOSE_H */
