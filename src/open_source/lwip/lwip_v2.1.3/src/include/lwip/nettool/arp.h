/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: ARP shell cmd API implementation
 * Author: none
 * Create: 2020
 */

#ifndef LWIP_ARP_H
#define LWIP_ARP_H
#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

u32_t lwip_arp(int argc, const char **argv);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_ARP_H */
