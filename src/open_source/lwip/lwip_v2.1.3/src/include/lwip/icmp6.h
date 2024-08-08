/**
 * @file
 *
 * IPv6 version of ICMP, as per RFC 4443.
 */

/*
 * Copyright (c) 2010 Inico Technologies Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Ivan Delamer <delamer@inicotech.com>
 *
 *
 * Please coordinate changes and requests with Ivan Delamer
 * <delamer@inicotech.com>
 */
#ifndef LWIP_HDR_ICMP6_H
#define LWIP_HDR_ICMP6_H

#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"
#include "lwip/prot/icmp6.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_ICMP6 && LWIP_IPV6 /* don't build if not configured for use in lwipopts.h */

#if LWIP_ICMP6_ERR_RT_LMT

/* struct for queueing icmp6 error packet */
struct icmp6_err_bkt {
  u32_t icmp_bkt[ICMP6_ERR_BKT_SIZE];
  u32_t avg;
  u16_t bkt_index;
  u16_t spare;
};

extern struct icmp6_err_bkt icmp6_errbkt;

u8_t icmp6_err_update_rate_lmt(void);
void icmp6_err_rate_calc(void);

#endif

void icmp6_input(struct pbuf *p, struct netif *inp);
void icmp6_dest_unreach(struct pbuf *p, enum icmp6_dur_code c);
void icmp6_packet_too_big(struct pbuf *p, u32_t mtu);
void icmp6_time_exceeded(struct pbuf *p, enum icmp6_te_code c);
void icmp6_time_exceeded_with_addrs(struct pbuf *p, enum icmp6_te_code c,
    const ip6_addr_t *src_addr, const ip6_addr_t *dest_addr);
void icmp6_param_problem(struct pbuf *p, enum icmp6_pp_code c, const void *pointer);
#elif LWIP_IPV6
#define icmp6_param_problem(p, c, pointer)  (void)(pointer)
#endif /* LWIP_ICMP6 && LWIP_IPV6 */

#if LWIP_IPV6 && LWIP_RAW

/* raw socket checksum offset for ICMPV6 */
#define IPV6_ICMP_CHKSUM_OFFSET 2

/* raw socket checksum offset for UDP */
#define IPV6_UDP_CHKSUM_OFFSET 6

/* raw socket checksum offset for TCP */
#define IPV6_TCP_CHKSUM_OFFSET 16

/* checksum length */
#define LWIP_IPV6_CHKSUM_LEN 2

/*
 * ICMP6 structure filter length
 */
#define ICMP_FILTER_LENGTH 8  /* ICMP6 structure filter length */

/* icmp6 const filter value */
#define ICMP6_FILTER_VAL 5

/* icmp6 const filter integer bits  */
#define ICMP6_FILTER_INTVAL 31

/* icmp6 max value filter value */
#define ICMP6_FILTER_MAXVAL 0xFF

#if !LWIP_LITEOS_COMPAT && !LWIP_FREERTOS_COMPAT
/*
 * socket option for ICMP6 filering support
 */
#define ICMP6_FILTER       1 /* ICMP filter support */

/*
 * stucture to hold the sockets filter state to allow packets to application or not.
                  Max size of structure is 256 bits.
 */
struct icmp6_filter {
  u32_t icmp6_filt[ICMP_FILTER_LENGTH];  /* 8*32 = 256 bits */
};

/*
 * filter macro to check if the raw socket is blocked or not.
 */
#define ICMP6_FILTER_WILLPASS(type, filterp) \
  ((((filterp)->icmp6_filt[(type) >> ICMP6_FILTER_VAL]) & (1U << ((type) & ICMP6_FILTER_INTVAL))) == 0)

/*
 * filter macro to check if the raw socket to blocked
 */
#define ICMP6_FILTER_WILLBLOCK(type, filterp) \
  ((((filterp)->icmp6_filt[(type) >> ICMP6_FILTER_VAL]) & (1U << ((type) & ICMP6_FILTER_INTVAL))) != 0)

/*
 * filter macro to allow raw socket for giving the data to application
 */
#define ICMP6_FILTER_SETPASS(type, filterp) \
  (((filterp)->icmp6_filt[(type) >> (ICMP6_FILTER_VAL)]) &= (~(u32_t)(1U << ((type) & ICMP6_FILTER_INTVAL))))

/*
 * filter macro to block raw socket from giving the data to application
 */
#define ICMP6_FILTER_SETBLOCK(type, filterp) \
  (((filterp)->icmp6_filt[(type) >> ICMP6_FILTER_VAL]) |= (1U << ((type) & ICMP6_FILTER_INTVAL)))

/*
 * filter macro to allow all raw socket to give the data to application
 */
#define ICMP6_FILTER_SETPASSALL(filterp) do { \
  u8_t tmp_; \
  for (tmp_ = 0; tmp_ < ICMP_FILTER_LENGTH; tmp_++) { \
    (filterp)->icmp6_filt[tmp_] = 0; \
  } \
} while (0)

/*
 * filter macro to block all raw socket from giving the data to application
 */
#define ICMP6_FILTER_SETBLOCKALL(filterp) do { \
  u8_t tmp_; \
  for (tmp_ = 0; tmp_ < ICMP_FILTER_LENGTH; tmp_++) { \
    (filterp)->icmp6_filt[tmp_] = 0xFFFFFFFF; \
  } \
} while (0)
#endif /* LWIP_LITEOS_COMPAT */
#endif /* LWIP_IPV6 && LWIP_RAW */

#ifdef __cplusplus
}
#endif

#if LWIP_IPV6 && LWIP_RIPPLE
void lwip_handle_rpl_ctrl_msg
(
  const struct icmpv6_hdr *icmp_hdr,
  struct pbuf *buf,
  const ip6_addr_t *srcip,
  const ip6_addr_t *dstip,
  struct netif *intf
);
#endif

#endif /* LWIP_HDR_ICMP6_H */
