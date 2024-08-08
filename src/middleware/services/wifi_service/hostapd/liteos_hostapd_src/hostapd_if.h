/*
 * Hostapd
 * Copyright (c) 2003-2016, Jouni Malinen <j@w1.fi>
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2014-2019. All rights reserved.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This file implements functions for registering and unregistering
 * %wpa_supplicant interfaces. In addition, this file contains number of
 * functions for managing network connections.
 */

#ifndef _HOSTAPD_IF_H_
#define _HOSTAPD_IF_H_

#include "ap/ap_config.h"
#include "ap/hostapd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void hostapd_main(const char *ifname);
void hostapd_global_deinit(void);
void hostapd_global_interfaces_deinit(void);
void hostapd_interface_deinit_free(struct hostapd_iface *iface);
int hostapd_global_ctrl_iface_init(struct hapd_interfaces *interface);
void hostapd_global_ctrl_iface_deinit(struct hapd_interfaces *interfaces);
struct hapd_interfaces * hostapd_get_interfaces(void);
struct hostapd_config * hostapd_config_read2(const char *fname);
int hostapd_stop(void);
int hostapd_ctrl_iface_init(struct hostapd_data *hapd);
void hostapd_ctrl_iface_deinit(struct hostapd_data *hapd);
void hostapd_pre_quit(struct hapd_interfaces *interfaces);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif
