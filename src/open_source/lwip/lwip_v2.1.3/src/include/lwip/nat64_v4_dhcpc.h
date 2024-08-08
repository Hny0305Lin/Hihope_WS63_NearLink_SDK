/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Description: NAT64 dhcp proxy header
 * Author: NA
 * Create: 2019
 */
#ifndef LWIP_HDR_NAT64_V4_DHCPC_H
#define LWIP_HDR_NAT64_V4_DHCPC_H
#include "lwip/opt.h"

#if LWIP_NAT64
#include "lwip/lwip_rpl.h"
#include "lwip/netif.h"

#define NAT64_DHCP_EVENT_RELEASE 0
#define NAT64_DHCP_EVENT_OFFER 1
#define NAT64_DHCP_EVENT_RENEW 2

err_t nat64_dhcp_request_ip(struct netif *ntf, const linklayer_addr_t *lladdr, const ip4_addr_t *ip);
err_t nat64_dhcp_stop(struct netif *ntf, const linklayer_addr_t *lladdr, u8_t now);
void nat64_dhcp_ip4_event(const u8_t *mac, u8_t maclen, const ip4_addr_t *ipaddr, int event);

#endif
#endif
