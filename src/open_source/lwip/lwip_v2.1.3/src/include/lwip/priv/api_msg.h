/**
 * @file
 * netconn API lwIP internal implementations (do not use in application code)
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
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
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef LWIP_HDR_API_MSG_H
#define LWIP_HDR_API_MSG_H

#include "lwip/opt.h"

#include "lwip/arch.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/igmp.h"
#include "lwip/api.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/ip.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_NETCONN || LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */
/* Note: Netconn API is always available when sockets are enabled -
 * sockets are implemented on top of them */

#if LWIP_MPU_COMPATIBLE
#if LWIP_NETCONN_SEM_PER_THREAD
#define API_MSG_M_DEF_SEM(m)  *m
#else
#define API_MSG_M_DEF_SEM(m)  API_MSG_M_DEF(m)
#endif
#else /* LWIP_MPU_COMPATIBLE */
#define API_MSG_M_DEF_SEM(m)  API_MSG_M_DEF(m)
#endif /* LWIP_MPU_COMPATIBLE */

/* For the netconn API, these values are use as a bitmask! */
#define NETCONN_SHUT_RD   1
#define NETCONN_SHUT_WR   2
#define NETCONN_SHUT_RDWR (NETCONN_SHUT_RD | NETCONN_SHUT_WR)

/* RST is received and is pending to be handled */
#define NETCONN_PENDING_ERR_RST   1
/* Close event is pending to be handled in refused data (when recvmbox is full) */
#define NETCONN_PENDING_ERR_CLSD  2
/* RST is ready to be handled after NETCONN_PENDING_ERR_CLSD */
#define NETCONN_READY_ERR_RST     3

/* FIN and RST received in the order, yet to be processed */
#define NETCONN_PENDING_ERR_FIN_RST 4

#if LWIP_TCP
extern const u8_t netconn_reset;
extern const u8_t netconn_aborted;
extern const u8_t netconn_closed;
#endif

#if PF_PKT_SUPPORT
/* Packet Types */
#define PACKET_HOST             0      /* To us                */
#define PACKET_BROADCAST        1      /* To all               */
#define PACKET_MULTICAST        2      /* To group             */
#define PACKET_OTHERHOST        3      /* To someone else      */
#define PACKET_OUTGOING         4
#endif

#if !LWIP_LITEOS_COMPAT && !LWIP_FREERTOS_COMPAT
/* Maximum size of a fully-qualified domain name */
#define NI_MAXHOST 1025
#endif

/* IP addresses and port numbers are expected to be in
 * the same byte order as in the corresponding pcb.
 */
/** This struct includes everything that is necessary to execute a function
    for a netconn in another thread context (mainly used to process netconns
    in the tcpip_thread context to be thread safe). */
struct api_msg {
  /** The netconn which to process - always needed: it includes the semaphore
      which is used to block the application thread until the function finished. */
  struct netconn *conn;
  /** The return value of the function executed in tcpip_thread. */
  err_t err;
  /** Depending on the executed function, one of these union members is used */
  union {
    struct tcpip_conn *conn_info;
    /** used for lwip_netconn_do_send */
    struct netbuf *b;
    /** used for lwip_netconn_do_newconn */
    struct {
      u16_t proto;
    } n;
    /** used for lwip_netconn_do_bind and lwip_netconn_do_connect */
    struct {
      API_MSG_M_DEF_C(ip_addr_t, ipaddr);
      u16_t port;
      u8_t if_idx;
    } bc;
    /** used for lwip_netconn_do_getaddr */
    struct {
      ip_addr_t API_MSG_M_DEF(ipaddr);
      u16_t API_MSG_M_DEF(port);
      u8_t local;
    } ad;
#if PF_PKT_SUPPORT
    struct {
      struct pf_pkt_ll API_MSG_M_DEF(ll);
      u8_t local;
    } adpkt;
#endif
    /** used for lwip_netconn_do_write */
    struct {
      /** current vector to write */
      const struct netvector *vector;
      /** number of unwritten vectors */
      u16_t vector_cnt;
      /** offset into current vector */
      size_t vector_off;
      /** total length across vectors */
      size_t len;
      /** offset into total length/output of bytes written when err == ERR_OK */
      size_t offset;
      u8_t apiflags;
#if LWIP_SO_SNDTIMEO
      u32_t time_started;
#endif /* LWIP_SO_SNDTIMEO */
    } w;
    /** used for lwip_netconn_do_recv */
    struct {
      size_t len;
    } r;
#if LWIP_TCP
    /** used for lwip_netconn_do_close (/shutdown) */
    struct {
      u8_t shut;
#if LWIP_SO_SNDTIMEO || LWIP_SO_LINGER
      u32_t time_started;
#else /* LWIP_SO_SNDTIMEO || LWIP_SO_LINGER */
      u8_t polls_left;
#endif /* LWIP_SO_SNDTIMEO || LWIP_SO_LINGER */
    } sd;
#endif /* LWIP_TCP */
#if LWIP_IGMP || (LWIP_IPV6 && LWIP_IPV6_MLD)
    /** used for lwip_netconn_do_leave_group */
    struct {
      API_MSG_M_DEF_C(ip_addr_t, multiaddr);
      API_MSG_M_DEF_C(ip_addr_t, netif_addr);
      u8_t if_idx;
      enum netconn_igmp join_or_leave;
    } jl;
#endif /* LWIP_IGMP || (LWIP_IPV6 && LWIP_IPV6_MLD) */
#if TCP_LISTEN_BACKLOG
    struct {
      u8_t backlog;
    } lb;
#endif /* TCP_LISTEN_BACKLOG */
  } msg;
#if LWIP_NETCONN_SEM_PER_THREAD
  sys_sem_t* op_completed_sem;
#endif /* LWIP_NETCONN_SEM_PER_THREAD */
};

#if LWIP_NETCONN_SEM_PER_THREAD
#define LWIP_API_MSG_SEM(msg)          ((msg)->op_completed_sem)
#else /* LWIP_NETCONN_SEM_PER_THREAD */
#define LWIP_API_MSG_SEM(msg)          (&(msg)->conn->op_completed)
#endif /* LWIP_NETCONN_SEM_PER_THREAD */


#if LWIP_DNS
/** As lwip_netconn_do_gethostbyname requires more arguments but doesn't require a netconn,
    it has its own struct (to avoid struct api_msg getting bigger than necessary).
    lwip_netconn_do_gethostbyname must be called using tcpip_callback instead of tcpip_apimsg
    (see netconn_gethostbyname). */
struct dns_api_msg {
  /** Hostname to query or dotted IP address string */
#if LWIP_MPU_COMPATIBLE
  char name[DNS_MAX_NAME_LENGTH];
#else /* LWIP_MPU_COMPATIBLE */
  const char *name;
#endif /* LWIP_MPU_COMPATIBLE */
  /** The resolved address is stored here */
  ip_addr_t API_MSG_M_DEF(addr);
#if LWIP_IPV4 && LWIP_IPV6
  /** Type of resolve call */
  u8_t dns_addrtype;
#endif /* LWIP_IPV4 && LWIP_IPV6 */
  /** This semaphore is posted when the name is resolved, the application thread
      should wait on it. */
  sys_sem_t API_MSG_M_DEF_SEM(sem);
  /** Errors are given back here */
  err_t API_MSG_M_DEF(err);
  /* to check how many IPaddress are updated */
  u32_t *count;
};

struct reverse_dns_api_msg {
  /* The IP Address to be resolved to hostname via reverse DNS */
  ip_addr_t API_MSG_M_DEF(addr);
  /* Resolved hostname of addr */
#if LWIP_MPU_COMPATIBLE
  char hostname[NI_MAXHOST];
#else /* LWIP_MPU_COMPATIBLE */
  char *hostname;
#endif /* LWIP_MPU_COMPATIBLE */
  /* This semaphore is posted when the name is resolved, the application thread
      should wait on it. */
  sys_sem_t API_MSG_M_DEF_SEM(sem);
  /* Errors are given back here */
  err_t API_MSG_M_DEF(err);
};

#endif /* LWIP_DNS */

#if LWIP_LOWPOWER
/* check wether need to poll tcp */
u8_t poll_tcp_needed(void *arg, struct tcp_pcb *pcb);
#endif

#if LWIP_NETCONN_FULLDUPLEX
int lwip_netconn_is_deallocated_msg(void *msg);
#endif
int lwip_netconn_is_err_msg(void *msg, err_t *err);
void lwip_netconn_do_newconn         (void *m);
void lwip_netconn_do_delconn         (void *m);
void lwip_netconn_do_bind            (void *m);
void lwip_netconn_do_bind_if         (void *m);
void lwip_netconn_do_connect         (void *m);
void lwip_netconn_do_disconnect      (void *m);
void lwip_netconn_do_listen          (void *m);
void lwip_netconn_do_send            (void *m);
void lwip_netconn_do_recv            (void *m);
#if TCP_LISTEN_BACKLOG
void lwip_netconn_do_accepted        (void *m);
#endif /* TCP_LISTEN_BACKLOG */
void lwip_netconn_do_write           (void *m);
void lwip_netconn_do_getaddr         (void *m);
void lwip_netconn_do_getaddr_pfpkt(void *m);
void lwip_netconn_do_close           (void *m);
void lwip_netconn_do_shutdown        (void *m);
void do_getconninfo(void *m);

#if LWIP_IGMP || (LWIP_IPV6 && LWIP_IPV6_MLD)
void lwip_netconn_do_leave_group(void *m);

#endif /* LWIP_IGMP || (LWIP_IPV6 && LWIP_IPV6_MLD) */

#if LWIP_DNS
void lwip_netconn_do_gethostbyname(void *arg);
#endif /* LWIP_DNS */

#if LWIP_DNS_REVERSE
void lwip_netconn_do_getnamebyhost(void *arg);
#endif /* LWIP_DNS_REVERSE */

struct netconn* netconn_alloc(enum netconn_type t, netconn_callback callback);
void netconn_free(struct netconn *conn);

#endif /* LWIP_NETCONN || LWIP_SOCKET */

#if LWIP_NETIF_API /* don't build if not configured for use in lwipopts.h */

/* netifapi related lwIP internal definitions */

#if LWIP_MPU_COMPATIBLE
#define NETIFAPI_IPADDR_DEF(type, m)  type m
#else /* LWIP_MPU_COMPATIBLE */
#define NETIFAPI_IPADDR_DEF(type, m)  const type * m
#endif /* LWIP_MPU_COMPATIBLE */

typedef void (*netifapi_void_fn)(struct netif *netif);
typedef err_t (*netifapi_errt_fn)(struct netif *netif);
typedef err_t (*netifapi_arg_fn)(struct netif *netif, void *arg);

struct netifapi_msg {
  struct tcpip_api_call_data call;
  struct netif *netif;
  union {
    struct {
#if LWIP_IPV4
      NETIFAPI_IPADDR_DEF(ip4_addr_t, ipaddr);
      NETIFAPI_IPADDR_DEF(ip4_addr_t, netmask);
      NETIFAPI_IPADDR_DEF(ip4_addr_t, gw);
#endif /* LWIP_IPV4 */
      void *state;
    } add;

    struct {
      const ip_addr_t *ipaddr;
    } find_by_ipaddr;
    struct {
      const char *name;
    } find_by_name;
    struct {
      unsigned char ifindex;
    } find_by_ifindex;
    struct {
#if LWIP_IPV4
      ip4_addr_t *ipaddr;
      ip4_addr_t *netmask;
      ip4_addr_t *gw;
#endif /* LWIP_IPV4 */
      void *state;
      netif_init_fn init;
      netif_input_fn input;
    } add_get;

    struct {
      netifapi_void_fn voidfunc;
      netifapi_errt_fn errtfunc;
    } common;
    struct {
      netifapi_arg_fn argfunc;
      void *arg;
    } arg_cb;
#if LWIP_DHCP
    struct {
      struct dhcp *dhcp;
    } dhcp_struct;
#endif
#if LWIP_DHCPS
    struct {
      char *start_ip;
      u16_t ip_num;
    } dhcp_start_params;
    struct {
      u8_t *mac;
      u8_t maclen;
      ip_addr_t *ip;
    } dhcp_get_ip_params;
#endif /* LWIP_DHCPS */
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    struct {
      netif_ext_callback_t *cb;
      netif_ext_callback_fn fn;
    } netif_ext_cb;
#endif
    struct {
      u16_t mtu;
    }netif_mtu;
    struct {
      netif_status_callback_fn link_callback;
    }netif_link_cb;
#if LWIP_NETIF_HOSTNAME
    struct {
      char *name;
      u8_t namelen;
    } hostname;
#endif /* LWIP_NETIF_HOSTNAME */
    struct {
#if LWIP_MPU_COMPATIBLE
      char name[NETIF_NAMESIZE];
#else /* LWIP_MPU_COMPATIBLE */
      char *name;
#endif /* LWIP_MPU_COMPATIBLE */
      u8_t index;
    } ifs;
    struct if_nameindex *if_list;
#if LWIP_DHCP_VENDOR_CLASS_IDENTIFIER
    struct {
      char *vci;
      u8_t *vci_len;
    } vci;
#endif /* LWIP_DHCP_VENDOR_CLASS_IDENTIFIER */
#if LWIP_IP_FILTER | LWIP_IPV6_FILTER
    struct {
      ip_filter_fn filter_fn;
      int type;
    } ip_filter;
#endif /* LWIP_IP_FILTER | LWIP_IP_FILTER */
#if LWIP_IPV6 && LWIP_NETIF_NBR_CACHE_API
    struct {
      struct ipv6_neighbor *nbrinfo;
      struct ip6_addr *nodeip;
    } ipv6_tables_access_params;
#endif
#if LWIP_IPV6
    struct {
      u8_t state;
    } ip6_state;
#endif
#if LWIP_ENABLE_ROUTER || LWIP_API_MESH
    struct {
      const struct linklayer_event_info *evt;
    } linklayer_event;
#endif
#if LWIP_LOWPOWER
    struct {
      enum lowpower_mod mod;
    } lp;
#endif
#if LWIP_IPV4 && LWIP_ARP
    struct {
      ip4_addr_t *ip;
      u8_t *mac;
      u8_t *maclen;
    } arp;
#endif
  } msg;
};

#if LWIP_DHCP_VENDOR_CLASS_IDENTIFIER
/*
Func Name: netifapi_set_vci
*/
/**
* @ingroup netifapi_dhcp4

*

* @brief
*  This API is used to set the vendor class identifier information, which is used in
*  DHCP message. Length of vendor class identifier information string
*  should be not more than DHCP_VCI_MAX_LEN(default 32), otherwise it
*  will return with ERR_ARG. vci_len is the real length of vendor class identifier
*  information string.
*
* @param[in]    vci      The new vendor class identifier information to use.
*                        It is a string of n octets [RFC2132 Section 9.13].
* @param[in]    vci_len  The length of vendor class identifier information string,
*                        should be not more than DHCP_VCI_MAX_LEN.
*
* @return
*  ERR_OK: On success \n
*  ERR_ARG: On passing invalid arguments. \n
*  ERR_VAL: On failure. \n

*/
err_t netifapi_set_vci(char *vci, u8_t vci_len);

/*
Func Name: netifapi_get_vci
*/
/**
* @ingroup netifapi_dhcp4

* @brief
*  This API is used to get the vendor class identifier information, which is used in
*  DHCP message. Length of the buffer which is used to store vendor
*  class identifier string must be not smaller than DHCP_VCI_MAX_LEN, otherwise
*  it will return with ERR_ARG. Default value of DHCP_VCI_MAX_LEN is 32.
*  If there is no vendor class identifier information related to DHCP, it will
*  return with ERR_VAL.
*
* @param[out]    vci      The buffer to store vendor class identifier string of DHCP.
* @param[in,out] vci_len  The length of buffer to store vendor class identifier string,
*                         and the real length of the vendor class identifier string.
*
* @return
*  ERR_OK: On success \n
*  ERR_ARG: On passing invalid arguments. \n
*  ERR_VAL: On failure. \n
*
*/
err_t netifapi_get_vci(char *vci, u8_t *vci_len);
#endif /* LWIP_DHCP_VENDOR_CLASS_IDENTIFIER */
#endif /* LWIP_NETIF_API */

#if LWIP_UDP
void recv_udp(void *arg, struct udp_pcb *pcb, struct pbuf *p,
              const ip_addr_t *addr, u16_t port);
#endif

#if DRIVER_STATUS_CHECK
void update_tcp_sndplus_event(void *arg, const struct tcp_pcb *pcb);
#endif

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_API_MSG_H */
