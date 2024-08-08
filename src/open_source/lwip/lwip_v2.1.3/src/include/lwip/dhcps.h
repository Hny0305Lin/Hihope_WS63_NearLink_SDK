/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: implementation for IPv4 DHCP server
 * Author: none
 * Create: 2020
 */
#ifndef LWIP_HDR_DHCPS_H
#define LWIP_HDR_DHCPS_H

#include "lwip/opt.h"
#include "lwip/prot/dhcp.h"
#if LWIP_DHCPS /* don't build if not configured for use in lwipopts.h */

#include "lwip/netif.h"
#include "lwip/udp.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifndef LWIP_DHCPS_MAX_LEASE
#define LWIP_DHCPS_MAX_LEASE 30
#endif

#ifndef LWIP_DHCPS_LEASE_TIME
#define LWIP_DHCPS_LEASE_TIME  7200
#endif

/* Offer time in seconds */
#ifndef LWIP_DHCPS_OFFER_TIME
#define LWIP_DHCPS_OFFER_TIME 300
#endif

#ifndef LWIP_DHCPS_DECLINE_TIME
#define LWIP_DHCPS_DECLINE_TIME 500
#endif

err_t dhcps_start(struct netif *netif, const char *start_ip, u16_t ip_num);
void dhcps_stop(struct netif *netif);
void dhcp_common_option(struct dhcp_msg *msg_out, u8_t option_type, u8_t option_len, u16_t *options_out_len);
void dhcp_common_option_byte(struct dhcp_msg *msg_out, u8_t value, u16_t *options_out_len);
void dhcp_common_option_short(struct dhcp_msg *msg_out, u16_t value, u16_t *options_out_len);
void dhcp_common_option_long(struct dhcp_msg *msg_out, u32_t value, u16_t *options_out_len);
void dhcp_common_option_trailer(struct dhcp_msg *msg_out, u16_t *options_out_len);
#if LWIP_DHCP_BOOTP_FILE
err_t dhcps_parse_options(struct pbuf *p, char *boot_file_name);
#else
err_t dhcps_parse_options(struct pbuf *p);
#endif
err_t dhcps_find_client_lease(struct netif *netif, const u8_t *mac, u8_t maclen, ip_addr_t *ip);
void dhcps_client_disconnect(struct netif *netif, const u8_t *mac, u8_t maclen);

#ifdef __cplusplus
}
#endif
#endif /* LWIP_DHCPS */
#endif /* LWIP_HDR_DHCPS_H */
