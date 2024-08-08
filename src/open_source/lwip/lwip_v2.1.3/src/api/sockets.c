/**
 * @file
 * Sockets BSD-Like API module
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
 * Improved by Marc Boucher <marc@mbsi.ca> and David Haas <dhaas@alum.rpi.edu>
 *
 */

#include "lwip/opt.h"

#if LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */

#include "lwip/sockets.h"
#include "lwip/priv/sockets_priv.h"
#include "netif/ifaddrs.h"
#include "lwip/api.h"
#include "lwip/igmp.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/memp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/priv/api_msg.h"
#include "lwip/mld6.h"

#if LWIP_IPV6_DHCP6
#include "lwip/dhcp6.h"
#include "lwip/prot/dhcp6.h"
#endif /* LWIP_IPV6_DHCP6 */

#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif

#if LWIP_CHECKSUM_ON_COPY
#include "lwip/inet_chksum.h"
#endif
#include "lwip/tcp_info.h"

#if LWIP_NETIF_ETHTOOL
#include "lwip/ethtool.h"
#endif

#if LWIP_COMPAT_SOCKETS == 2 && LWIP_POSIX_SOCKETS_IO_NAMES
#include <stdarg.h>
#endif
#include <string.h>

#ifdef LWIP_HOOK_FILENAME
#include LWIP_HOOK_FILENAME
#endif

#if LWIP_LOWPOWER
#include "lwip/lowpower.h"
#endif

/* If the netconn API is not required publicly, then we include the necessary
   files here to get the implementation */
#if !LWIP_NETCONN
#undef LWIP_NETCONN
#define LWIP_NETCONN 1
#include "api_msg.c"
#include "api_lib.c"
#include "netbuf.c"
#undef LWIP_NETCONN
#define LWIP_NETCONN 0
#endif

#if LWIP_ENABLE_NET_CAPABILITY
#include "capability_type.h"
#include "capability_api.h"
#define BIND_SERVICE_CAP_MIN_PORT 1024
#endif

#define API_SELECT_CB_VAR_REF(name)               API_VAR_REF(name)
#define API_SELECT_CB_VAR_DECLARE(name)           API_VAR_DECLARE(struct lwip_select_cb, name)
#define API_SELECT_CB_VAR_ALLOC(name, retblock)   API_VAR_ALLOC_EXT(struct lwip_select_cb, MEMP_SELECT_CB, name, retblock)
#define API_SELECT_CB_VAR_FREE(name)              API_VAR_FREE(MEMP_SELECT_CB, name)

#if LWIP_IPV4
#define IP4ADDR_PORT_TO_SOCKADDR(sin, ipaddr, port) do { \
      (sin)->sin_family = AF_INET; \
      (sin)->sin_port = lwip_htons((port)); \
      inet_addr_from_ip4addr(&(sin)->sin_addr, ipaddr); \
      memset((sin)->sin_zero, 0, SIN_ZERO_LEN); }while(0)
#define SOCKADDR4_TO_IP4ADDR_PORT(sin, ipaddr, port) do { \
    inet_addr_to_ip4addr(ip_2_ip4(ipaddr), &((sin)->sin_addr)); \
    (port) = lwip_ntohs((sin)->sin_port); }while(0)
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
/* SIN6_LEN macro is to differntiate whether stack is using 4.3BSD or 4.4BSD variants of sockaddr_in6
structure */
#if defined(SIN6_LEN)
#define IP6ADDR_PORT_TO_SOCKADDR(sin6, ipaddr, port) do { \
      (sin6)->sin6_len = sizeof(struct sockaddr_in6); \
      (sin6)->sin6_family = AF_INET6; \
      (sin6)->sin6_port = lwip_htons((port)); \
      (sin6)->sin6_flowinfo = 0; \
      inet6_addr_from_ip6addr(&(sin6)->sin6_addr, ipaddr); \
      (sin6)->sin6_scope_id = ip6_addr_zone(ipaddr); }while(0)
#else
#define IP6ADDR_PORT_TO_SOCKADDR(sin6, ipaddr, port) do { \
      (sin6)->sin6_family = AF_INET6; \
      (sin6)->sin6_port = lwip_htons((port)); \
      (sin6)->sin6_flowinfo = 0; \
      inet6_addr_from_ip6addr(&(sin6)->sin6_addr, ipaddr); \
      (sin6)->sin6_scope_id = ip6_addr_zone(ipaddr); } while (0)
#endif
#define SOCKADDR6_TO_IP6ADDR_PORT(sin6, ipaddr, port) do { \
    inet6_addr_to_ip6addr(ip_2_ip6(ipaddr), &((sin6)->sin6_addr)); \
    if (ip6_addr_has_scope(ip_2_ip6(ipaddr), IP6_UNKNOWN)) { \
      ip6_addr_set_zone(ip_2_ip6(ipaddr), (u8_t)((sin6)->sin6_scope_id)); \
    } \
    (port) = lwip_ntohs((sin6)->sin6_port); }while(0)
#endif /* LWIP_IPV6 */

#if LWIP_IPV4 && LWIP_IPV6
static void sockaddr_to_ipaddr_port(const struct sockaddr *sockaddr, ip_addr_t *ipaddr, u16_t *port);
static int lwip_sockopt_check_optlen_conn_pcb(const struct lwip_sock *sock, socklen_t optlen, socklen_t len);
static int lwip_sockopt_check_optlen_conn_pcb_type(const struct lwip_sock *sock, socklen_t optlen, socklen_t len,
                                                   enum netconn_type type, int is_v6);

#define IS_SOCK_ADDR_LEN_VALID(namelen)  (((namelen) == sizeof(struct sockaddr_in)) || \
                                         ((namelen) == sizeof(struct sockaddr_in6)))
#define IS_SOCK_ADDR_TYPE_VALID(name)    (((name)->sa_family == AF_INET) || \
                                         ((name)->sa_family == AF_INET6))
#define IS_SOCK_ADDR_TYPE_LEN_COMPAT(name, namelen) ((((name)->sa_family == AF_INET) && \
                                                       ((namelen) == sizeof(struct sockaddr_in))) || \
                                                     (((name)->sa_family == AF_INET6) && \
                                                      ((namelen) == sizeof(struct sockaddr_in6))))
#define SOCK_ADDR_TYPE_MATCH(name, sock) \
       ((((name)->sa_family == AF_INET) && !(NETCONNTYPE_ISIPV6((sock)->conn->type))) || \
       (((name)->sa_family == AF_INET6) && (NETCONNTYPE_ISIPV6((sock)->conn->type))))
#define IPADDR_PORT_TO_SOCKADDR(sockaddr, ipaddr, port) do { \
    if (IP_IS_ANY_TYPE_VAL(*ipaddr) || IP_IS_V6_VAL(*ipaddr)) { \
      IP6ADDR_PORT_TO_SOCKADDR((struct sockaddr_in6*)(void*)(sockaddr), ip_2_ip6(ipaddr), port); \
    } else { \
      IP4ADDR_PORT_TO_SOCKADDR((struct sockaddr_in*)(void*)(sockaddr), ip_2_ip4(ipaddr), port); \
    } } while(0)
#define SOCKADDR_TO_IPADDR_PORT(sockaddr, ipaddr, port) sockaddr_to_ipaddr_port(sockaddr, ipaddr, &(port))
#define DOMAIN_TO_NETCONN_TYPE(domain, type) (((domain) == AF_INET6) ? \
  (enum netconn_type)(((uint32_t)(type)) | NETCONN_TYPE_IPV6) : (type))
#elif LWIP_IPV6 /* LWIP_IPV4 && LWIP_IPV6 */
#define IS_SOCK_ADDR_LEN_VALID(namelen)  ((namelen) == sizeof(struct sockaddr_in6))
#define IS_SOCK_ADDR_TYPE_VALID(name)    ((name)->sa_family == AF_INET6)
#define SOCK_ADDR_TYPE_MATCH(name, sock) 1
#define IPADDR_PORT_TO_SOCKADDR(sockaddr, ipaddr, port) \
        IP6ADDR_PORT_TO_SOCKADDR((struct sockaddr_in6*)(void*)(sockaddr), ip_2_ip6(ipaddr), port)
#define SOCKADDR_TO_IPADDR_PORT(sockaddr, ipaddr, port) \
        SOCKADDR6_TO_IP6ADDR_PORT((const struct sockaddr_in6*)(const void*)(sockaddr), ipaddr, port)
#define DOMAIN_TO_NETCONN_TYPE(domain, netconn_type) (netconn_type)
#else /*-> LWIP_IPV4: LWIP_IPV4 && LWIP_IPV6 */
#define IS_SOCK_ADDR_LEN_VALID(namelen)  ((namelen) == sizeof(struct sockaddr_in))
#define IS_SOCK_ADDR_TYPE_VALID(name)    ((name)->sa_family == AF_INET)
#define SOCK_ADDR_TYPE_MATCH(name, sock) 1
#define IPADDR_PORT_TO_SOCKADDR(sockaddr, ipaddr, port) \
        IP4ADDR_PORT_TO_SOCKADDR((struct sockaddr_in*)(void*)(sockaddr), ip_2_ip4(ipaddr), port)
#define SOCKADDR_TO_IPADDR_PORT(sockaddr, ipaddr, port) \
        SOCKADDR4_TO_IP4ADDR_PORT((const struct sockaddr_in*)(const void*)(sockaddr), ipaddr, port)
#define DOMAIN_TO_NETCONN_TYPE(domain, netconn_type) (netconn_type)
#endif /* LWIP_IPV6 */

#define IS_SOCK_ADDR_TYPE_VALID_OR_UNSPEC(name)    (((name)->sa_family == AF_UNSPEC) || \
                                                    IS_SOCK_ADDR_TYPE_VALID(name))
#define SOCK_ADDR_TYPE_MATCH_OR_UNSPEC(name, sock) (((name)->sa_family == AF_UNSPEC) || \
                                                    SOCK_ADDR_TYPE_MATCH(name, sock))
#define IS_SOCK_ADDR_ALIGNED(name)      ((((mem_ptr_t)(name)) % LWIP_MIN(4, MEM_ALIGNMENT)) == 0)


#define LWIP_SOCKOPT_CHECK_OPTLEN(sock, optlen, opttype) do { if ((optlen) < sizeof(opttype)) { done_socket(sock); return EINVAL; }}while(0)
#define LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, optlen, opttype) do { \
  LWIP_SOCKOPT_CHECK_OPTLEN(sock, optlen, opttype); \
  if ((sock)->conn == NULL) { done_socket(sock); return EINVAL; } }while(0)
#define LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, optlen, opttype) do { \
  int ret = lwip_sockopt_check_optlen_conn_pcb(sock, optlen, (socklen_t)sizeof(opttype)); \
  if (ret != 0) { \
    done_socket(sock); \
    return ret; \
  } \
} while (0)

#define LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, opttype, netconntype) do { \
  int ret = lwip_sockopt_check_optlen_conn_pcb_type(sock, optlen, (socklen_t)sizeof(opttype), netconntype, 0); \
  if (ret != 0) { \
    done_socket(sock); \
    return ret; \
  } \
} while (0)

#if LWIP_IPV6
#define LWIP_SOCKOPT_CHECK_IPTYPE_V6(sock, contype) do { \
  if (!(NETCONNTYPE_ISIPV6(contype))) { \
    done_socket(sock); \
    return ENOPROTOOPT; \
  } \
} while (0)

#define LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, optlen, opttype, netconntype) do { \
  int ret = lwip_sockopt_check_optlen_conn_pcb_type(sock, optlen, sizeof(opttype), netconntype, 1); \
  if (ret != 0) { \
    done_socket(sock); \
    return ret; \
  } \
} while (0)
#endif /* LWIP_IPV6 */

#define LWIP_SETGETSOCKOPT_DATA_VAR_REF(name)     API_VAR_REF(name)
#define LWIP_SETGETSOCKOPT_DATA_VAR_DECLARE(name) API_VAR_DECLARE(struct lwip_setgetsockopt_data, name)
#define LWIP_SETGETSOCKOPT_DATA_VAR_FREE(name)    API_VAR_FREE(MEMP_SOCKET_SETGETSOCKOPT_DATA, name)
#if LWIP_MPU_COMPATIBLE
#define LWIP_SETGETSOCKOPT_DATA_VAR_ALLOC(name, sock) do { \
  name = (struct lwip_setgetsockopt_data *)memp_malloc(MEMP_SOCKET_SETGETSOCKOPT_DATA); \
  if (name == NULL) { \
    sock_set_errno(sock, ENOMEM); \
    done_socket(sock); \
    return -1; \
  } }while(0)
#else /* LWIP_MPU_COMPATIBLE */
#define LWIP_SETGETSOCKOPT_DATA_VAR_ALLOC(name, sock)
#endif /* LWIP_MPU_COMPATIBLE */

#if LWIP_SO_SNDRCVTIMEO_NONSTANDARD
#define LWIP_SO_SNDRCVTIMEO_OPTTYPE int
#define LWIP_SO_SNDRCVTIMEO_SET(optval, val) (*(int *)(optval) = (val))
#define LWIP_SO_SNDRCVTIMEO_GET_MS(optval)   ((long)*(const int*)(optval))
#define LWIP_SO_SNDRECVTIMO_VALIDATE(_sock, optval)

#else
#define LWIP_SO_SNDRCVTIMEO_OPTTYPE struct timeval
#define LWIP_SO_SNDRCVTIMEO_SET(optval, val)  do { \
  u32_t loc = (val); \
  ((struct timeval *)(optval))->tv_sec = (long)((loc) / 1000U); \
  ((struct timeval *)(optval))->tv_usec = (long)(((loc) % 1000U) * 1000U); }while (0)
#define LWIP_SO_SNDRCVTIMEO_GET_MS(optval) ((((const struct timeval *)(optval))->tv_sec * 1000) + (((const struct timeval *)(optval))->tv_usec / 1000))
#define LWIP_MAX_TV_SEC_VAL       ((0x7FFFFFFF / 1000)-1000)
#define LWIP_SO_SNDRECVTIMO_VALIDATE(_sock, optval) do { \
      struct timeval * tv = (struct timeval *)optval; \
        if (tv->tv_usec < 0 || tv->tv_sec < 0 || tv->tv_usec >= 1000000 \
          || tv->tv_sec >= LWIP_MAX_TV_SEC_VAL) { \
        done_socket(_sock); \
          return  EDOM; \
      } else if (tv->tv_sec == 0 && tv->tv_usec < 1000 && tv->tv_usec > 0) { \
        done_socket(_sock); \
        return EINVAL; \
      } \
    }while (0)

#endif

/** A struct sockaddr replacement that has the same alignment as sockaddr_in/
 *  sockaddr_in6 if instantiated.
 */
union sockaddr_aligned {
  struct sockaddr sa;
#if LWIP_IPV6
  struct sockaddr_in6 sin6;
#endif /* LWIP_IPV6 */
#if LWIP_IPV4
  struct sockaddr_in sin;
#endif /* LWIP_IPV4 */
};

/* Define the number of IPv4 multicast memberships, default is one per socket */
#ifndef LWIP_SOCKET_MAX_MEMBERSHIPS
#define LWIP_SOCKET_MAX_MEMBERSHIPS LWIP_CONFIG_NUM_SOCKETS
#endif

#if LWIP_IGMP
/* This is to keep track of IP_ADD_MEMBERSHIP calls to drop the membership when
   a socket is closed */
struct lwip_socket_multicast_pair {
  /** the socket */
  struct lwip_sock *sock;
  /** the interface address */
  ip4_addr_t if_addr;
  /** the group address */
  ip4_addr_t multi_addr;
};

#if LWIP_ALLOW_SOCKET_CONFIG
struct lwip_socket_multicast_pair *socket_ipv4_multicast_memberships = NULL;
#else
static struct lwip_socket_multicast_pair socket_ipv4_multicast_memberships[LWIP_SOCKET_MAX_MEMBERSHIPS];
#endif

static int  lwip_socket_register_membership(int s, const ip4_addr_t *if_addr, const ip4_addr_t *multi_addr);
static void lwip_socket_unregister_membership(int s, const ip4_addr_t *if_addr, const ip4_addr_t *multi_addr);
static void lwip_socket_drop_registered_memberships(int s);
#endif /* LWIP_IGMP */

#if LWIP_IPV6 && LWIP_IPV6_MLD
/* This is to keep track of IP_JOIN_GROUP calls to drop the membership when
   a socket is closed */
struct lwip_socket_multicast_mld6_pair {
  /** the socket */
  struct lwip_sock *sock;
  /** the interface index */
  u8_t if_idx;
  /** the group address */
  ip6_addr_t multi_addr;
};

#if LWIP_ALLOW_SOCKET_CONFIG
static struct lwip_socket_multicast_mld6_pair *socket_ipv6_multicast_memberships = NULL;
#else
static struct lwip_socket_multicast_mld6_pair socket_ipv6_multicast_memberships[LWIP_SOCKET_MAX_MEMBERSHIPS];
#endif

static int  lwip_socket_register_mld6_membership(int s, unsigned int if_idx, const ip6_addr_t *multi_addr);
static void lwip_socket_unregister_mld6_membership(int s, unsigned int if_idx, const ip6_addr_t *multi_addr);
static void lwip_socket_drop_registered_mld6_memberships(int s);
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

/** The global array of available sockets */
#if LWIP_ALLOW_SOCKET_CONFIG
static struct lwip_sock *sockets = NULL;
#else
static struct lwip_sock sockets[LWIP_CONFIG_NUM_SOCKETS];
#endif

#if LWIP_SOCKET_SELECT || LWIP_SOCKET_POLL
#if LWIP_TCPIP_CORE_LOCKING
/* protect the select_cb_list using core lock */
#define LWIP_SOCKET_SELECT_DECL_PROTECT(lev)
#define LWIP_SOCKET_SELECT_PROTECT(lev)   LOCK_TCPIP_CORE()
#define LWIP_SOCKET_SELECT_UNPROTECT(lev) UNLOCK_TCPIP_CORE()
#else /* LWIP_TCPIP_CORE_LOCKING */
/* protect the select_cb_list using SYS_LIGHTWEIGHT_PROT */
#define LWIP_SOCKET_SELECT_DECL_PROTECT(lev)  SYS_ARCH_DECL_PROTECT(lev)
#define LWIP_SOCKET_SELECT_PROTECT(lev)       SYS_ARCH_PROTECT(lev)
#define LWIP_SOCKET_SELECT_UNPROTECT(lev)     SYS_ARCH_UNPROTECT(lev)
/** This counter is increased from lwip_select when the list is changed
    and checked in select_check_waiters to see if it has changed. */
static volatile int select_cb_ctr;
#endif /* LWIP_TCPIP_CORE_LOCKING */
/** The global list of tasks waiting for select */
static struct lwip_select_cb *select_cb_list;
#endif /* LWIP_SOCKET_SELECT || LWIP_SOCKET_POLL */

#define sock_set_errno(sk, e) do { \
  const int sockerr = (e); \
  (void)atomic_set(&sk->err, sockerr); \
  set_errno(sockerr); \
} while (0)

#define VALIDATE_SET_RAW_OPTNAME_RET(_sock,_optname) do { \
      if (((_sock)->conn != NULL && NETCONNTYPE_GROUP((_sock)->conn->type) == NETCONN_RAW) && \
        (((_optname) != SO_BROADCAST) && ((_optname) != SO_RCVTIMEO) && \
        ((_optname)  != SO_RCVBUF) && ((_optname) != SO_DONTROUTE) && ((_optname) != SO_BINDTODEVICE) && \
        ((_optname)  != SO_PRIORITY))) { \
        done_socket(_sock); \
        return ENOPROTOOPT; \
      } \
    } while (0)

#define VALIDATE_GET_RAW_OPTNAME_RET(_sock,_optname) do { \
      if ((_sock)->conn != NULL && (NETCONNTYPE_GROUP((_sock)->conn->type) == NETCONN_RAW) && \
        (((_optname) != SO_BROADCAST) && ((_optname) != SO_RCVTIMEO) && \
        ((_optname) != SO_RCVBUF) && ((_optname) != SO_TYPE) && \
        ((_optname) != SO_DONTROUTE) && ((_optname) != SO_BINDTODEVICE) && \
        ((_optname) != SO_PRIORITY))) { \
        done_socket(_sock); \
        return ENOPROTOOPT;   \
      } \
    } while (0)

#if PF_PKT_SUPPORT
#define VALIDATE_SET_PF_PKT_OPTNAME_RET(_s,_sock,_level,_optname) do { \
      if ((_level) == SOL_PACKET && ((_optname) != SO_RCVTIMEO && (_optname) != SO_RCVBUF) &&  \
          (_optname) != SO_ATTACH_FILTER && (_optname) != SO_DETACH_FILTER) { \
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, SOL_PACKET, UNIMPL: optname=0x%x, ..)\n",  \
                    (_s), (_optname)));  \
        done_socket(_sock); \
        return ENOPROTOOPT;  \
      } \
    } while (0)

#define VALIDATE_GET_PF_PKT_OPTNAME_RET(_s,_sock,_level,_optname) do { \
      if ((_sock)->conn != NULL && (NETCONNTYPE_GROUP((_sock)->conn->type) == NETCONN_PKT_RAW) && \
          (_level) == SOL_PACKET && ((_optname) != SO_RCVTIMEO && (_optname) != SO_RCVBUF && (_optname) != SO_TYPE)) { \
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%p, SOL_PACKET, UNIMPL: optname=0x%x, ..)\n",  \
                    (_s), (_optname)));  \
        done_socket(_sock); \
        return  ENOPROTOOPT;  \
      } \
    } while (0)

#define VALIDATE_LEVEL_PF_PACKET(_sock,_level) \
    if ((_sock)->conn != NULL && ((NETCONNTYPE_GROUP(NETCONN_TYPE(_sock->conn)) == NETCONN_PKT_RAW \
      && SOL_SOCKET != (_level) && SOL_PACKET != (_level)) ||  \
      (NETCONNTYPE_GROUP((_sock)->conn->type) != NETCONN_PKT_RAW && SOL_PACKET == (_level))))
#endif

/* Forward declaration of some functions */
#if LWIP_SOCKET_SELECT || LWIP_SOCKET_POLL
static void event_callback(struct netconn *conn, enum netconn_evt evt, u16_t len);
#define DEFAULT_SOCKET_EVENTCB event_callback
static void select_check_waiters(int s, int has_recvevent, int has_sendevent, int has_errevent);
#else
#define DEFAULT_SOCKET_EVENTCB NULL
#endif
#if !LWIP_TCPIP_CORE_LOCKING
static void lwip_getsockopt_callback(void *arg);
static void lwip_setsockopt_callback(void *arg);
#endif
static int lwip_getsockopt_impl(int s, int level, int optname, void *optval, socklen_t *optlen);
static int lwip_setsockopt_impl(int s, int level, int optname, const void *optval, socklen_t optlen);

#if LWIP_IOCTL_ROUTE
static u8_t lwip_ioctl_internal_SIOCADDRT(const struct rtentry *rmten);
#endif /* LWIP_IOCTL_ROUTE */

#if LWIP_IOCTL_IF
static u8_t lwip_ioctl_internal_SIOCGIFCONF(struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCGIFADDR(struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCSIFADDR(const struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCDIFADDR(const struct ifreq *ifr);
#if LWIP_IPV6
static u8_t lwip_ioctl_internal_SIOCSIFADDR_6(struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCDIFADDR_6(struct ifreq *ifr);
#endif
static u8_t lwip_ioctl_internal_SIOCGIFBRDADDR(struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCGIFNETMASK(struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCSIFNETMASK(const struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCGIFHWADDR(struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCSIFHWADDR(const struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCGIFFLAGS(struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCSIFFLAGS(const struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCGIFNAME(struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCSIFNAME(struct ifreq *ifr);
static u8_t lwip_validate_ifname(const char *name, u8_t *let_pos);
static u8_t lwip_ioctl_internal_SIOCGIFINDEX(struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCSIFMTU(const struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCGIFMTU(struct ifreq *ifr);
#endif /* LWIP_IOCTL_IF */

#if LWIP_NETIF_ETHTOOL
static s32_t lwip_ioctl_internal_SIOCETHTOOL(struct ifreq *ifr);
#endif

#if LWIP_IPV6
#if LWIP_IPV6_DUP_DETECT_ATTEMPTS
static u8_t lwip_ioctl_internal_SIOCSIPV6DAD(const struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCGIPV6DAD(struct ifreq *ifr);
#endif
#if LWIP_IOCTL_IPV6DPCTD
static u8_t lwip_ioctl_internal_SIOCSIPV6DPCTD(const struct ifreq *ifr);
static u8_t lwip_ioctl_internal_SIOCGIPV6DPCTD(struct ifreq *ifr);
#endif
#endif

static int free_socket_locked(struct lwip_sock *sock, int is_tcp, struct netconn **conn,
                              union lwip_sock_lastdata *lastdata);
static void free_socket_free_elements(int is_tcp, struct netconn *conn, union lwip_sock_lastdata *lastdata);

#if LWIP_ENABLE_NET_CAPABILITY && LWIP_ENABLE_CAP_NET_BROADCAST
static u8_t ip_addr_isbroadcast_by_sock(const ip_addr_t *ipaddr, const struct lwip_sock *sock)
{
  struct netif *netif = NULL;
  u8_t res;

#if LWIP_TCPIP_CORE_LOCKING
  LOCK_TCPIP_CORE();
#else
  SYS_ARCH_DECL_PROTECT(lev);
  SYS_ARCH_PROTECT(lev);
#endif

  LWIP_ASSERT("socket invalid!", sock != NULL && sock->conn != NULL);

  if (sock->conn->pcb.ip != NULL) {
    netif = netif_get_by_index(sock->conn->pcb.ip->netif_idx);
    if (netif != NULL) {
      res = ip_addr_isbroadcast(ipaddr, netif);
      goto done;
    }
  }
  res = netif_ipaddr_isbrdcast(ipaddr);

done:
#if LWIP_TCPIP_CORE_LOCKING
  UNLOCK_TCPIP_CORE();
#else
  SYS_ARCH_UNPROTECT(lev);
#endif
  return res;
}
#endif

static int
lwip_sockopt_check_optlen_conn_pcb(const struct lwip_sock *sock, socklen_t optlen, socklen_t len)
{
  if ((optlen) < len) {
    return EINVAL;
  }

  if (((sock)->conn == NULL) || ((sock)->conn->pcb.tcp == NULL)) {
    return EINVAL;
  }

  return 0;
}

static int
lwip_sockopt_check_optlen_conn_pcb_type(const struct lwip_sock *sock, socklen_t optlen, socklen_t len,
                                        enum netconn_type type, int is_v6)
{
  int ret = lwip_sockopt_check_optlen_conn_pcb(sock, optlen, len);
  if (ret != 0) {
    return ret;
  }
#if LWIP_IPV6
  if (is_v6 && !(NETCONNTYPE_ISIPV6(sock->conn->type))) {
    return ENOPROTOOPT;
  }
#endif

  if (type != NETCONNTYPE_GROUP(NETCONN_TYPE((sock)->conn))) {
    return ENOPROTOOPT;
  }
  return 0;
}

#if LWIP_IPV4 && LWIP_IPV6
static void
sockaddr_to_ipaddr_port(const struct sockaddr *sockaddr, ip_addr_t *ipaddr, u16_t *port)
{
  if ((sockaddr->sa_family) == AF_INET6) {
    SOCKADDR6_TO_IP6ADDR_PORT((const struct sockaddr_in6 *)(const void *)(sockaddr), ipaddr, *port);
    ipaddr->type = IPADDR_TYPE_V6;
  } else {
    SOCKADDR4_TO_IP4ADDR_PORT((const struct sockaddr_in *)(const void *)(sockaddr), ipaddr, *port);
    ipaddr->type = IPADDR_TYPE_V4;
  }
}
#endif /* LWIP_IPV4 && LWIP_IPV6 */

/** LWIP_NETCONN_SEM_PER_THREAD==1: initialize thread-local semaphore */
void
lwip_socket_thread_init(void)
{
  netconn_thread_init();
}

/** LWIP_NETCONN_SEM_PER_THREAD==1: destroy thread-local semaphore */
void
lwip_socket_thread_cleanup(void)
{
  netconn_thread_cleanup();
}

int sock_init(void)
{
  int i;

#if LWIP_ALLOW_SOCKET_CONFIG
  /* this space will not be free, use it as a static pool. for memory pool configuration purpose */
  if (LWIP_CONFIG_NUM_SOCKETS > LWIP_NUM_SOCKETS_MAX) {
    return ERR_MEM;
  }

  sockets = (struct lwip_sock *)mem_malloc(LWIP_CONFIG_NUM_SOCKETS * sizeof(struct lwip_sock));
  if (sockets == NULL) {
    return ERR_MEM;
  }
  (void)memset(sockets, 0, LWIP_CONFIG_NUM_SOCKETS * sizeof(struct lwip_sock));
#endif /* LWIP_ALLOW_SOCKET_CONFIG */

  /* allocate a new socket identifier */
  for (i = 0; i < (int)LWIP_CONFIG_NUM_SOCKETS; ++i) {
    if (sys_mutex_new(&sockets[i].mutex) != ERR_OK) {
      goto FAILURE;
    }
  }

  return ERR_OK;

FAILURE:
  /* Free all resources if failure happens */
  for (--i; i >= 0; i--) {
    sys_mutex_free(&sockets[i].mutex);
  }

#if LWIP_ALLOW_SOCKET_CONFIG
  mem_free(sockets);
  sockets = NULL;
#endif
  return ERR_MEM;
}

/*
 * lock lwip_sock parameters from race condition from different thread in parallel
 *
 * @param *sock socket whos paramerters need to be protected.
 * @return void
 */
static inline void
lwip_sock_lock(struct lwip_sock *sock)
{
#if LWIP_COMPAT_MUTEX
  u32_t ret;
  ret = sys_mutex_lock(&sock->mutex);
  LWIP_ASSERT("sys_mutex_lock failed", (ret == 0));
  LWIP_UNUSED_ARG(ret);
#else
  sys_mutex_lock(&sock->mutex);
#endif
}

/*
 * unlock lwip_sock parameters for using through different thread in parallel
 *
 * @param *sock socket.
 * @return void
 */
static inline void
lwip_sock_unlock(struct lwip_sock *sock)
{
  sys_mutex_unlock(&sock->mutex);
}

#if LWIP_NETCONN_FULLDUPLEX
/* Thread-safe increment of sock->fd_used, with overflow check */
static int
sock_inc_used(struct lwip_sock *sock)
{
  int ret;
  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_ASSERT("sock != NULL", sock != NULL);

  SYS_ARCH_PROTECT(lev);
  if (sock->fd_free_pending || sock->closing || sock->conn == NULL) {
    /* prevent new usage of this socket if free is pending or finished */
    ret = 0;
  } else {
    ++sock->fd_used;
    ret = 1;
    LWIP_ASSERT("sock->fd_used != 0", sock->fd_used != 0);
  }
  SYS_ARCH_UNPROTECT(lev);
  return ret;
}

/* Like sock_inc_used(), but called under SYS_ARCH_PROTECT lock. */
static int
sock_inc_used_locked(struct lwip_sock *sock)
{
  LWIP_ASSERT("sock != NULL", sock != NULL);

  if (sock->fd_free_pending || sock->closing || sock->conn == NULL) {
    LWIP_ASSERT("sock->fd_used != 0", sock->fd_used != 0);
    return 0;
  }

  ++sock->fd_used;
  LWIP_ASSERT("sock->fd_used != 0", sock->fd_used != 0);
  return 1;
}

/* In full-duplex mode,sock->fd_used != 0 prevents a socket descriptor from being
 * released (and possibly reused) when used from more than one thread
 * (e.g. read-while-write or close-while-write, etc)
 * This function is called at the end of functions using (try)get_socket*().
 */
static void
done_socket(struct lwip_sock *sock)
{
  int freed = 0;
  int is_tcp = 0;
  struct netconn *conn = NULL;
  union lwip_sock_lastdata lastdata;
  SYS_ARCH_DECL_PROTECT(lev);
  LWIP_ASSERT("sock != NULL", sock != NULL);

  SYS_ARCH_PROTECT(lev);
  LWIP_ASSERT("sock->fd_used > 0", sock->fd_used > 0);
  if (--sock->fd_used == 0) {
    if (sock->fd_free_pending) {
      /* free the socket */
      sock->fd_used = 1;
      is_tcp = sock->fd_free_pending & LWIP_SOCK_FD_FREE_TCP;
      freed = free_socket_locked(sock, is_tcp, &conn, &lastdata);
    }
  }
  SYS_ARCH_UNPROTECT(lev);

  if (freed) {
    free_socket_free_elements(is_tcp, conn, &lastdata);
  }
}

#else /* LWIP_NETCONN_FULLDUPLEX */
#define sock_inc_used(sock)         1
#define sock_inc_used_locked(sock)  1
#define done_socket(sock)
#endif /* LWIP_NETCONN_FULLDUPLEX */

/* Translate a socket 'int' into a pointer (only fails if the index is invalid) */
static struct lwip_sock *
tryget_socket_unconn_nouse(int fd)
{
  int s = fd - LWIP_SOCKET_OFFSET;
  if ((s < 0) || (s >= (int)LWIP_CONFIG_NUM_SOCKETS)) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("tryget_socket_unconn(%d): invalid\n", fd));
    return NULL;
  }
  return &sockets[s];
}

struct lwip_sock *
lwip_socket_dbg_get_socket(int fd)
{
  return tryget_socket_unconn_nouse(fd);
}

/** Free a socket. The socket's netconn must have been
 * delete before!
 *
 * @param sock the socket to free
 * @param is_tcp != 0 for TCP sockets, used to free lastdata
 */
static void
free_socket(struct lwip_sock *sock, int is_tcp)
{
  int freed;
  struct netconn *conn;
  union lwip_sock_lastdata lastdata;
  SYS_ARCH_DECL_PROTECT(lev);

  /* Protect socket array */
  SYS_ARCH_PROTECT(lev);

  freed = free_socket_locked(sock, is_tcp, &conn, &lastdata);
  SYS_ARCH_UNPROTECT(lev);
  /* don't use 'sock' after this line, as another task might have allocated it */

  if (freed) {
    free_socket_free_elements(is_tcp, conn, &lastdata);
  }
}

/* Translate a socket 'int' into a pointer (only fails if the index is invalid) */
static struct lwip_sock *
tryget_socket_unconn(int fd)
{
  struct lwip_sock *ret = tryget_socket_unconn_nouse(fd);
  if (ret != NULL) {
    if (!sock_inc_used(ret)) {
      return NULL;
    }
  }
  return ret;
}

/* Like tryget_socket_unconn(), but called under SYS_ARCH_PROTECT lock. */
static struct lwip_sock *
tryget_socket_unconn_locked(int fd)
{
  struct lwip_sock *ret = tryget_socket_unconn_nouse(fd);
  if (ret != NULL) {
    if (!sock_inc_used_locked(ret)) {
      return NULL;
    }
  }
  return ret;
}

/**
 * Same as get_socket but doesn't set errno
 *
 * @param fd externally used socket index
 * @return struct lwip_sock for the socket or NULL if not found
 */
static struct lwip_sock *
tryget_socket(int fd)
{
  struct lwip_sock *sock = tryget_socket_unconn(fd);
  if (sock != NULL) {
    if (sock->conn) {
      return sock;
    }
    done_socket(sock);
  }
  return NULL;
}

/**
 * Map a externally used socket index to the internal socket representation.
 *
 * @param fd externally used socket index
 * @return struct lwip_sock for the socket or NULL if not found
 */
static struct lwip_sock *
get_socket(int fd)
{
  struct lwip_sock *sock = tryget_socket(fd);
  if (!sock) {
    if ((fd < LWIP_SOCKET_OFFSET) || (fd >= (int)(LWIP_SOCKET_OFFSET + LWIP_CONFIG_NUM_SOCKETS))) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("get_socket(%d): invalid\n", fd));
    }
    set_errno(EBADF);
    return NULL;
  }
  return sock;
}

/* get numbers of unused sockets */
int get_unused_socket_num(void)
{
  int unused = 0, i;
  SYS_ARCH_DECL_PROTECT(lev);

  SYS_ARCH_PROTECT(lev);

  for (i = 0; i < (int)LWIP_CONFIG_NUM_SOCKETS; i++) {
    if (!sockets[i].conn
#if LWIP_NETCONN_FULLDUPLEX
      && !sockets[i].fd_used
#endif
    ) {
      unused++;
    }
  }

  SYS_ARCH_UNPROTECT(lev);

  return unused;
}

/**
 * Allocate a new socket for a given netconn.
 *
 * @param newconn the netconn for which to allocate a socket
 * @param accepted 1 if socket has been created by accept(),
 *                 0 if socket has been created by socket()
 * @return the index of the new socket; -1 on error
 */
static int
alloc_socket(struct netconn *newconn, int accepted)
{
  int i;
  SYS_ARCH_DECL_PROTECT(lev);
  LWIP_UNUSED_ARG(accepted);

  /* allocate a new socket identifier */
  for (i = 0; i < (int)LWIP_CONFIG_NUM_SOCKETS; ++i) {
    /* Protect socket array */
    SYS_ARCH_PROTECT(lev);
    if (!sockets[i].conn) {
#if LWIP_NETCONN_FULLDUPLEX
      if (sockets[i].fd_used) {
        SYS_ARCH_UNPROTECT(lev);
        continue;
      }
      sockets[i].fd_used    = 1;
      sockets[i].fd_free_pending = 0;
      sockets[i].closing = 0;
#endif
      sockets[i].conn       = newconn;
      /* The socket is not yet known to anyone, so no need to protect
         after having marked it as used. */
      SYS_ARCH_UNPROTECT(lev);
      sockets[i].lastdata.pbuf = NULL;
      (void)atomic_set(&sockets[i].err, 0);
#if LWIP_SOCKET_SELECT || LWIP_SOCKET_POLL
      LWIP_ASSERT("sockets[i].select_waiting == 0", sockets[i].select_waiting == 0);
      sockets[i].rcvevent   = 0;
      /* TCP sendbuf is empty, but the socket is not yet writable until connected
       * (unless it has been created by accept()). */
      sockets[i].sendevent  = (NETCONNTYPE_GROUP(newconn->type) == NETCONN_TCP ? (accepted != 0) : 1);
      sockets[i].errevent   = 0;
#if LWIP_EXT_POLL_SUPPORT
#if LWIP_LITEOS_COMPAT
      __init_waitqueue_head(&sockets[i].wq);
#endif /* LWIP_LITEOS_COMPAT */
#endif
#endif /* LWIP_SOCKET_SELECT || LWIP_SOCKET_POLL */
      return i + LWIP_SOCKET_OFFSET;
    }
    SYS_ARCH_UNPROTECT(lev);
  }
  return -1;
}

/** Free a socket (under lock)
 *
 * @param sock the socket to free
 * @param is_tcp != 0 for TCP sockets, used to free lastdata
 * @param conn the socekt's netconn is stored here, must be freed externally
 * @param lastdata lastdata is stored here, must be freed externally
 */
static int
free_socket_locked(struct lwip_sock *sock, int is_tcp, struct netconn **conn,
                   union lwip_sock_lastdata *lastdata)
{
#if LWIP_NETCONN_FULLDUPLEX
  LWIP_ASSERT("sock->fd_used > 0", sock->fd_used > 0);
  sock->fd_used--;
  if (sock->fd_used > 0) {
    sock->fd_free_pending = LWIP_SOCK_FD_FREE_FREE | (is_tcp ? LWIP_SOCK_FD_FREE_TCP : 0);
    return 0;
  }
#else /* LWIP_NETCONN_FULLDUPLEX */
  LWIP_UNUSED_ARG(is_tcp);
#endif /* LWIP_NETCONN_FULLDUPLEX */

  *lastdata = sock->lastdata;
  sock->lastdata.pbuf = NULL;
  (void)atomic_set(&sock->err, 0);
  *conn = sock->conn;
  sock->conn = NULL;
  return 1;
}

/** Free a socket's leftover members.
 */
static void
free_socket_free_elements(int is_tcp, struct netconn *conn, union lwip_sock_lastdata *lastdata)
{
  if (lastdata->pbuf != NULL) {
    if (is_tcp) {
      pbuf_free(lastdata->pbuf);
    } else {
      netbuf_delete(lastdata->netbuf);
    }
  }
  if (conn != NULL) {
    /* netconn_prepare_delete() has already been called, here we only free the conn */
    netconn_delete(conn);
  }
}

/* Below this, the well-known socket functions are implemented.
 * Use google.com or opengroup.org to get a good description :-)
 *
 * Exceptions are documented!
 */

int
lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
  struct lwip_sock *sock, *nsock;
  struct netconn *newconn;
  ip_addr_t naddr;
  u16_t port = 0;
  int newsock;
  err_t err;
  int recvevent;
  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d)...\n", s));
  /* Correcting the errno to EINVAL to be consistent with Linux
   If addr is valid and addrlen is null , stack should return EINVAL
   If addr is NULL pointer and addrlen is null or valid - ignore the addrlen and function normally as
   POSIX doesnot mandate addr to be non null pointer */
  LWIP_ERROR("Invalid address space\n",
             (((addr) && (addrlen != NULL)) || (addr == NULL)),
             set_errno(EINVAL); return -1);

  LWIP_ERROR("Invalid address length\n", ((addrlen == NULL) || ((int)(*addrlen) >= 0)),
             set_errno(EINVAL); return -1);

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  /* wait for a new connection */
  err = netconn_accept(sock->conn, &newconn);
  if (err != ERR_OK) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d): netconn_acept failed, err=%d\n", s, err));
    if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) != NETCONN_TCP) {
      sock_set_errno(sock, EOPNOTSUPP);
    } else if (err == ERR_CLSD) {
      sock_set_errno(sock, EINVAL);
    } else {
      sock_set_errno(sock, err_to_errno(err));
    }
    done_socket(sock);
    return -1;
  }
  LWIP_ASSERT("newconn != NULL", newconn != NULL);

  newsock = alloc_socket(newconn, 1);
  if (newsock == -1) {
    netconn_delete(newconn);
    sock_set_errno(sock, ENFILE);
    done_socket(sock);
    return -1;
  }
  LWIP_ASSERT("invalid socket index", (newsock >= (int)(LWIP_SOCKET_OFFSET)) &&
  	          (newsock < (int)(LWIP_CONFIG_NUM_SOCKETS + LWIP_SOCKET_OFFSET)));
  nsock = &sockets[newsock - LWIP_SOCKET_OFFSET];

  /* See event_callback: If data comes in right away after an accept, even
   * though the server task might not have created a new socket yet.
   * In that case, newconn->socket is counted down (newconn->socket--),
   * so nsock->rcvevent is >= 1 here!
   */
  SYS_ARCH_PROTECT(lev);
  recvevent = (s16_t)(-1 - newconn->socket);
  newconn->socket = newsock;
  SYS_ARCH_UNPROTECT(lev);

  if (newconn->callback) {
    LOCK_TCPIP_CORE();
    while (recvevent > 0) {
      recvevent--;
      newconn->callback(newconn, NETCONN_EVT_RCVPLUS, 0);
    }
    UNLOCK_TCPIP_CORE();
  }

  /* Note that POSIX only requires us to check addr is non-NULL. addrlen must
   * not be NULL if addr is valid.
   */
  if ((addr != NULL) && (addrlen != NULL)) {
    union sockaddr_aligned tempaddr;
    /* get the IP address and port of the remote host */
    err = netconn_peer(newconn, &naddr, &port);
    if (err != ERR_OK) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d): netconn_peer failed, err=%d\n", s, err));
      err = ERR_ABRT;
      free_socket(nsock, 1);
      sock_set_errno(sock, err_to_errno(err));
      done_socket(sock);
      return -1;
    }

    IPADDR_PORT_TO_SOCKADDR(&tempaddr, &naddr, port);
    if (IP_IS_V4_VAL(naddr) && (*addrlen > (int)sizeof(struct sockaddr_in))) {
      *addrlen = sizeof(struct sockaddr_in);
    } else if (IP_IS_V6_VAL(naddr) && (*addrlen > (int)sizeof(struct sockaddr_in6))) {
      *addrlen = sizeof(struct sockaddr_in6);
    }
    MEMCPY(addr, &tempaddr, *addrlen);

    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d) returning new sock=%d addr=", s, newsock));
    ip_addr_debug_print_val(SOCKETS_DEBUG, naddr);
    LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F"\n", port));
  } else {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_accept(%d) returning new sock=%d", s, newsock));
  }

  sock_set_errno(sock, 0);
  done_socket(sock);
  done_socket(nsock);
  return newsock;
}

int
lwip_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
  struct lwip_sock *sock;
  ip_addr_t local_addr;
  u16_t local_port;
  err_t err;

#if PF_PKT_SUPPORT
  const struct sockaddr_ll *name_ll = NULL;
  u8_t local_ifindex = 0;
#endif
  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  /* check size, family and alignment of 'name' */
#if PF_PKT_SUPPORT
  if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) & NETCONN_PKT_RAW) {
    name_ll = (const struct sockaddr_ll *)(void*)name;
    LWIP_ERROR("lwip_bind: invalid address", ((name_ll != NULL)&&(namelen == sizeof(struct sockaddr_ll)) &&
               ((name_ll->sll_family) == PF_PACKET) && IS_SOCK_ADDR_ALIGNED(name) &&
               (name_ll->sll_ifindex <= LWIP_NETIF_IFINDEX_MAX)), sock_set_errno(sock, err_to_errno(ERR_VAL));
               done_socket(sock); return -1);

    ip_addr_set_any_val(IPADDR_TYPE_V4, local_addr);
    local_port = name_ll->sll_protocol;
    local_ifindex = (u8_t)name_ll->sll_ifindex;

    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_bind(%d, ", s));
    LWIP_DEBUGF(SOCKETS_DEBUG, (" ifindex=%u proto=%"U16_F")\n", local_ifindex, local_port));
  } else
#endif
  {
    LWIP_ERROR("lwip_bind: name or namelen invalid\n", ((name != NULL) && IS_SOCK_ADDR_LEN_VALID(namelen) &&
               IS_SOCK_ADDR_ALIGNED(name)), sock_set_errno(sock, err_to_errno(ERR_VAL));
               done_socket(sock); return -1);
    LWIP_ERROR("lwip_bind: sockaddr type is invalid\n",
               (IS_SOCK_ADDR_TYPE_VALID(name)), sock_set_errno(sock, err_to_errno(ERR_AFNOSUPPORT));
               done_socket(sock); return -1);
#if LWIP_IPV4 && LWIP_IPV6
    LWIP_ERROR("lwip_bind: sockaddr type not compat with length\n", (IS_SOCK_ADDR_TYPE_LEN_COMPAT(name, namelen)),
               sock_set_errno(sock, err_to_errno(ERR_VAL));
               done_socket(sock); return -1);
#endif

    if (!SOCK_ADDR_TYPE_MATCH(name, sock)) {
      /* sockaddr does not match socket type (IPv4/IPv6) */
      sock_set_errno(sock, err_to_errno(ERR_VAL));
      done_socket(sock);
      return -1;
    }

    LWIP_UNUSED_ARG(namelen);

    SOCKADDR_TO_IPADDR_PORT(name, &local_addr, local_port);
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_bind(%d, addr=", s));
    ip_addr_debug_print_val(SOCKETS_DEBUG, local_addr);
    LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F")\n", local_port));
  }

#if LWIP_ENABLE_NET_CAPABILITY
  if (local_port != 0 && local_port < BIND_SERVICE_CAP_MIN_PORT) {
    LWIP_ERROR("permission deny: NET_BIND_SERVICE\n", IsCapPermit(CAP_NET_BIND_SERVICE),
               sock_set_errno(sock, EPERM); done_socket(sock); return -1);
  }
#if LWIP_ENABLE_CAP_NET_BROADCAST
  if (ip_addr_ismulticast(&local_addr) || ip_addr_isbroadcast_by_sock(&local_addr, sock)) {
    LWIP_ERROR("lwip_bind permission deny: NET_BROADCAST\n", IsCapPermit(CAP_NET_BROADCAST),
               sock_set_errno(sock, EPERM); done_socket(sock); return -1);
  }
#endif /* LWIP_ENABLE_CAP_NET_BROADCAST */
#endif /* LWIP_ENABLE_NET_CAPABILITY */

#if LWIP_IPV4 && LWIP_IPV6
  /* Dual-stack: Unmap IPv4 mapped IPv6 addresses */
  if (IP_IS_V6_VAL(local_addr) && ip6_addr_isipv4mappedipv6(ip_2_ip6(&local_addr))) {
    unmap_ipv4_mapped_ipv6(ip_2_ip4(&local_addr), ip_2_ip6(&local_addr));
    IP_SET_TYPE_VAL(local_addr, IPADDR_TYPE_V4);
  }
#endif /* LWIP_IPV4 && LWIP_IPV6 */

#if PF_PKT_SUPPORT
  err = netconn_bind(sock->conn, &local_addr, local_port, local_ifindex);
#else
  err = netconn_bind(sock->conn, &local_addr, local_port);
#endif

  if (err != ERR_OK) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_bind(%d) failed, err=%d\n", s, err));
    sock_set_errno(sock, err_to_errno(err));
    done_socket(sock);
    return -1;
  }

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_bind(%d) succeeded\n", s));
  sock_set_errno(sock, 0);
  done_socket(sock);
  return 0;
}

int
lwip_close(int s)
{
  struct lwip_sock *sock;
  int is_tcp = 0;
  err_t err;
  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_close(%d)\n", s));

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  /* Close operation already in progress, can not process multiple close in parallel */
  SYS_ARCH_PROTECT(lev);
#if LWIP_NETCONN_FULLDUPLEX
  if (sock->closing == 1) {
    SYS_ARCH_UNPROTECT(lev);
    done_socket(sock);
    set_errno(EBADF);
    return -1;
  }
#endif
  SYS_ARCH_UNPROTECT(lev);

  if (sock->conn != NULL) {
    is_tcp = NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) == NETCONN_TCP;
  } else {
    LWIP_ASSERT("sock->lastdata == NULL", sock->lastdata.pbuf == NULL);
  }

#if LWIP_IGMP
  /* drop all possibly joined IGMP memberships */
  lwip_socket_drop_registered_memberships(s);
#endif /* LWIP_IGMP */
#if LWIP_IPV6 && LWIP_IPV6_MLD
  /* drop all possibly joined MLD6 memberships */
  lwip_socket_drop_registered_mld6_memberships(s);
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

#if LWIP_NETCONN_FULLDUPLEX
  sock->closing = 1;
#endif
  err = netconn_prepare_delete(sock->conn);
  if (err != ERR_OK) {
    sock_set_errno(sock, err_to_errno(err));
    done_socket(sock);
    return -1;
  }

  free_socket(sock, is_tcp);
  set_errno(0);
  return 0;
}

int
lwip_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
  struct lwip_sock *sock;
  err_t err;

  LWIP_ERROR("lwip_connect: invalid address", (name != NULL),
             set_errno(EINVAL);  return -1);

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

#if PF_PKT_SUPPORT
  if (NETCONN_PKT_RAW & NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn))) {
    sock_set_errno(sock, EOPNOTSUPP);
    done_socket(sock);
    return -1;
  }
#endif

  if (namelen < sizeof(struct sockaddr) || !SOCK_ADDR_TYPE_MATCH_OR_UNSPEC(name, sock)) {
    /* sockaddr does not match socket type (IPv4/IPv6) */
    sock_set_errno(sock, err_to_errno(ERR_VAL));
    done_socket(sock);
    return -1;
  }

  LWIP_UNUSED_ARG(namelen);
  if (name->sa_family == AF_UNSPEC) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_connect(%d, AF_UNSPEC)\n", s));
    err = netconn_disconnect(sock->conn);
  } else {
    ip_addr_t remote_addr;
    u16_t remote_port;

    /* check size, family and alignment of 'name' */
    LWIP_ERROR("lwip_connect: invalid address", IS_SOCK_ADDR_LEN_VALID(namelen) &&
               IS_SOCK_ADDR_TYPE_VALID_OR_UNSPEC(name) && IS_SOCK_ADDR_ALIGNED(name),
               sock_set_errno(sock, err_to_errno(ERR_VAL));
               done_socket(sock); return -1);

    LWIP_ERROR("lwip_connect: invalid address family", IS_SOCK_ADDR_TYPE_VALID_OR_UNSPEC(name),
               sock_set_errno(sock, err_to_errno(EAFNOSUPPORT));
               done_socket(sock); return -1);

    SOCKADDR_TO_IPADDR_PORT(name, &remote_addr, remote_port);
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_connect(%d, addr=", s));
    ip_addr_debug_print_val(SOCKETS_DEBUG, remote_addr);
    LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F")\n", remote_port));

#if LWIP_IPV4 && LWIP_IPV6
    /* Dual-stack: Unmap IPv4 mapped IPv6 addresses */
    if (IP_IS_V6_VAL(remote_addr) && ip6_addr_isipv4mappedipv6(ip_2_ip6(&remote_addr))) {
      unmap_ipv4_mapped_ipv6(ip_2_ip4(&remote_addr), ip_2_ip6(&remote_addr));
      IP_SET_TYPE_VAL(remote_addr, IPADDR_TYPE_V4);
    }
#endif /* LWIP_IPV4 && LWIP_IPV6 */

    err = netconn_connect(sock->conn, &remote_addr, remote_port);
  }

  if (err != ERR_OK) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_connect(%d) failed, err=%d\n", s, err));
    if (err == ERR_RST) {
      sock_set_errno(sock, ECONNREFUSED);
    } else {
      err = (err_t)((err == ERR_CLSD) ? ERR_ABRT : err);
      sock_set_errno(sock, err_to_errno(err));
    }
    done_socket(sock);
    return -1;
  }

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_connect(%d) succeeded\n", s));
  sock_set_errno(sock, 0);
  done_socket(sock);
  return 0;
}

/**
 * Set a socket into listen mode.
 * The socket may not have been used for another connection previously.
 *
 * @param s the socket to set to listening mode
 * @param backlog (ATTENTION: needs TCP_LISTEN_BACKLOG=1)
 * @return 0 on success, non-zero on failure
 */
int
lwip_listen(int s, int backlog)
{
  struct lwip_sock *sock;
  err_t err;

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_listen(%d, backlog=%d)\n", s, backlog));

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  /* limit the "backlog" parameter to fit in an u8_t */
  backlog = LWIP_MIN(LWIP_MAX(backlog, 0), TCP_DEFAULT_LISTEN_BACKLOG);

  err = netconn_listen_with_backlog(sock->conn, (u8_t)backlog);

  if (err != ERR_OK) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_listen(%d) failed, err=%d\n", s, err));
    if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) != NETCONN_TCP) {
      sock_set_errno(sock, EOPNOTSUPP);
    } else {
      sock_set_errno(sock, err_to_errno(err));
    }
    done_socket(sock);
    return -1;
  }

  sock_set_errno(sock, 0);
  done_socket(sock);
  return 0;
}

#if LWIP_TCP
/* Helper function to loop over receiving pbufs from netconn
 * until "len" bytes are received or we're otherwise done.
 * Keeps sock->lastdata for peeking or partly copying.
 */
static ssize_t
lwip_recv_tcp(struct lwip_sock *sock, void *mem, size_t len, int flags)
{
  u8_t apiflags = NETCONN_NOAUTORCVD;
  ssize_t recvd = 0;
  ssize_t recv_left = (len <= SSIZE_MAX) ? (ssize_t)len : SSIZE_MAX;

  LWIP_ASSERT("no socket given", sock != NULL);
  LWIP_ASSERT("this should be checked internally", NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) == NETCONN_TCP);

  if (flags & MSG_DONTWAIT) {
    apiflags |= NETCONN_DONTBLOCK;
  }

  /* If this is non-blocking call, then check first */
  if (atomic_read(&sock->conn->tcp_connected) != 1) {
    sock_set_errno(sock, ENOTCONN);
    return -1;
  }

  do {
    struct pbuf *p;
    err_t err;
    u16_t copylen;

    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recv_tcp: top while sock->lastdata=%p\n", (void *)sock->lastdata.pbuf));
    /* Check if there is data left from the last recv operation. */
    if (sock->lastdata.pbuf) {
      p = sock->lastdata.pbuf;
    } else {
      /* No data was left from the previous operation, so we try to get
         some from the network. */
      err = netconn_recv_tcp_pbuf_flags(sock->conn, &p, apiflags);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recv_tcp: netconn_recv err=%d, pbuf=%p\n",
                                  err, (void *)p));

      if (err != ERR_OK) {
        if (recvd > 0) {
          /* already received data, return that (this trusts in getting the same error from
             netconn layer again next time netconn_recv is called) */
          goto lwip_recv_tcp_done;
        }
        /* We should really do some error checking here. */
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recv_tcp: p == NULL, error is \"%s\"!\n",
                                    lwip_strerr(err)));
        sock_set_errno(sock, err_to_errno(err));
        if (err == ERR_CLSD) {
          return 0;
        } else {
          return -1;
        }
      }
      LWIP_ASSERT("p != NULL", p != NULL);
      sock->lastdata.pbuf = p;
    }

    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recv_tcp: buflen=%"U16_F" recv_left=%d off=%d\n",
                                p->tot_len, (int)recv_left, (int)recvd));

    if (recv_left > p->tot_len) {
      copylen = p->tot_len;
    } else {
      copylen = (u16_t)recv_left;
    }
    if (recvd + copylen < recvd) {
      /* overflow */
      copylen = (u16_t)(SSIZE_MAX - recvd);
    }

    /* copy the contents of the received buffer into
    the supplied memory pointer mem */
    pbuf_copy_partial(p, (u8_t *)mem + recvd, copylen, 0);

    recvd += copylen;

    /* TCP combines multiple pbufs for one recv */
    LWIP_ASSERT("invalid copylen, len would underflow", recv_left >= copylen);
    recv_left -= copylen;

    /* Unless we peek the incoming message... */
    if ((flags & MSG_PEEK) == 0) {
      /* ... check if there is data left in the pbuf */
      LWIP_ASSERT("invalid copylen", p->tot_len >= copylen);
      if (p->tot_len - copylen > 0) {
        /* If so, it should be saved in the sock structure for the next recv call.
           We store the pbuf but hide/free the consumed data: */
        sock->lastdata.pbuf = pbuf_free_header(p, copylen);
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recv_tcp: lastdata now pbuf=%p\n", (void *)sock->lastdata.pbuf));
      } else {
        sock->lastdata.pbuf = NULL;
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recv_tcp: deleting pbuf=%p\n", (void *)p));
        pbuf_free(p);
      }
    }
    /* once we have some data to return, only add more if we don't need to wait */
    apiflags |= NETCONN_DONTBLOCK | NETCONN_NOFIN;
    /* @todo: do we need to support peeking more than one pbuf? */
  } while ((recv_left > 0) && !(flags & MSG_PEEK));
lwip_recv_tcp_done:
  if ((recvd > 0) && !(flags & MSG_PEEK)) {
    /* ensure window update after copying all data */
    netconn_tcp_recvd(sock->conn, (size_t)recvd);
  }
  /* store data left count on this rcv operation */
  if (sock->lastdata.pbuf) {
    sock->conn->lrcv_left = (u32_t)(sock->lastdata.pbuf->tot_len);
  } else {
    sock->conn->lrcv_left = 0;
  }
  sock_set_errno(sock, 0);
  return recvd;
}
#endif

/* Convert a netbuf's address data to struct sockaddr */
static int
lwip_sock_make_addr(struct netconn *conn, ip_addr_t *fromaddr, u16_t port,
                    struct sockaddr *from, socklen_t *fromlen)
{
  int truncated = 0;
  union sockaddr_aligned saddr;
  socklen_t sa_len;

  LWIP_UNUSED_ARG(conn);

  LWIP_ASSERT("fromaddr != NULL", fromaddr != NULL);
  LWIP_ASSERT("from != NULL", from != NULL);
  LWIP_ASSERT("fromlen != NULL", fromlen != NULL);

#if LWIP_IPV4 && LWIP_IPV6
  /* Dual-stack: Map IPv4 addresses to IPv4 mapped IPv6 */
  if (NETCONNTYPE_ISIPV6(NETCONN_TYPE(conn)) && IP_IS_V4(fromaddr)) {
    ip4_2_ipv4_mapped_ipv6(ip_2_ip6(fromaddr), ip_2_ip4(fromaddr));
    IP_SET_TYPE(fromaddr, IPADDR_TYPE_V6);
  }
#endif /* LWIP_IPV4 && LWIP_IPV6 */

  IPADDR_PORT_TO_SOCKADDR(&saddr, fromaddr, port);
  if (IP_IS_V4_VAL(*fromaddr)) {
    sa_len = sizeof(struct sockaddr_in);
  } else {
    sa_len = sizeof(struct sockaddr_in6);
  }
  if (*fromlen < sa_len) {
    truncated = 1;
  } else if (*fromlen > sa_len) {
    *fromlen = sa_len;
  }
  MEMCPY(from, &saddr, *fromlen);
  return truncated;
}

#if LWIP_TCP
/* Helper function to get a tcp socket's remote address info */
static int
lwip_recv_tcp_from(struct lwip_sock *sock, struct sockaddr *from, socklen_t *fromlen, const char *dbg_fn, int dbg_s, ssize_t dbg_ret)
{
  if (sock == NULL) {
    return 0;
  }
  LWIP_UNUSED_ARG(dbg_fn);
  LWIP_UNUSED_ARG(dbg_s);
  LWIP_UNUSED_ARG(dbg_ret);

#if !SOCKETS_DEBUG
  if (from && fromlen)
#endif /* !SOCKETS_DEBUG */
  {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("%s(%d):  addr=", dbg_fn, dbg_s));
    LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F" len=%d\n", sock->conn->remote_port, (int)dbg_ret));
    ip_addr_debug_print_val(SOCKETS_DEBUG, sock->conn->remote_ip);
    if (from && fromlen) {
      return lwip_sock_make_addr(sock->conn, &sock->conn->remote_ip, sock->conn->remote_port, from, fromlen);
    }
  }
  return 0;
}
#endif

/* Helper function to receive a netbuf from a udp or raw netconn.
 * Keeps sock->lastdata for peeking.
 */
static err_t
lwip_recvfrom_udp_raw(struct lwip_sock *sock, int flags, struct msghdr *msg, u16_t *datagram_len, int dbg_s)
{
  struct netbuf *buf;
  u8_t apiflags;
  err_t err;
  u16_t buflen, copylen, copied;
  int i;

  LWIP_UNUSED_ARG(dbg_s);
  LWIP_ERROR("lwip_recvfrom_udp_raw: invalid arguments", (msg->msg_iov != NULL) || (msg->msg_iovlen <= 0), return ERR_ARG;);

  if (flags & MSG_DONTWAIT) {
    apiflags = NETCONN_DONTBLOCK;
  } else {
    apiflags = 0;
  }

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom_udp_raw[UDP/RAW]: top sock->lastdata=%p\n", (void *)sock->lastdata.netbuf));
  /* Check if there is data left from the last recv operation. */
  buf = sock->lastdata.netbuf;
  if (buf == NULL) {
    /* No data was left from the previous operation, so we try to get
        some from the network. */
    err = netconn_recv_udp_raw_netbuf_flags(sock->conn, &buf, apiflags);
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom_udp_raw[UDP/RAW]: netconn_recv err=%d, netbuf=%p\n",
                                err, (void *)buf));

    if (err != ERR_OK) {
      return err;
    }
    LWIP_ASSERT("buf != NULL", buf != NULL);
    sock->lastdata.netbuf = buf;
  }
  buflen = buf->p->tot_len;
  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom_udp_raw: buflen=%"U16_F"\n", buflen));

  copied = 0;
  /* copy the pbuf payload into the iovs */
  for (i = 0; (i < msg->msg_iovlen) && (copied < buflen); i++) {
    u16_t len_left = (u16_t)(buflen - copied);
    if (msg->msg_iov[i].iov_len > len_left) {
      copylen = len_left;
    } else {
      copylen = (u16_t)msg->msg_iov[i].iov_len;
    }

    /* copy the contents of the received buffer into
        the supplied memory buffer */
    pbuf_copy_partial(buf->p, (u8_t *)msg->msg_iov[i].iov_base, copylen, copied);
    copied = (u16_t)(copied + copylen);
  }

  /* Check to see from where the data was.*/
#if !SOCKETS_DEBUG
  if (msg->msg_name && msg->msg_namelen)
#endif /* !SOCKETS_DEBUG */
  {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom_udp_raw(%d):  addr=", dbg_s));
    ip_addr_debug_print_val(SOCKETS_DEBUG, *netbuf_fromaddr(buf));
    LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F" len=%d\n", netbuf_fromport(buf), copied));
    if (msg->msg_name && msg->msg_namelen) {
#if PF_PKT_SUPPORT
      struct sockaddr_ll sll;

      if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) & NETCONN_PKT_RAW) {
        if (msg->msg_namelen > sizeof(sll)) {
          msg->msg_namelen = sizeof(sll);
        }

        if (msg->msg_namelen) {
          (void)memset_s(&sll, sizeof(sll), 0, sizeof(sll));
          sll.sll_family = PF_PACKET;
          sll.sll_protocol = netbuf_fromport(buf);
          sll.sll_hatype = netbuf_fromhatype(buf);
          sll.sll_ifindex = netbuf_fromifindex(buf);

          if (buf->p->flags & PBUF_FLAG_LLBCAST) {
            sll.sll_pkttype = PACKET_BROADCAST;
          } else if (buf->p->flags & PBUF_FLAG_LLMCAST) {
            sll.sll_pkttype = PACKET_MULTICAST;
          } else if (buf->p->flags & PBUF_FLAG_HOST) {
            sll.sll_pkttype = PACKET_HOST;
          } else if (buf->p->flags & PBUF_FLAG_OUTGOING) {
            sll.sll_pkttype = PACKET_OUTGOING;
          } else {
            sll.sll_pkttype = PACKET_OTHERHOST;
          }

          (void)memcpy_s(msg->msg_name, msg->msg_namelen, (void *)&sll, msg->msg_namelen);

          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom_udp_raw(%d): hatype=%u", dbg_s, ntohs(sll.sll_hatype)));
          LWIP_DEBUGF(SOCKETS_DEBUG, (" packet type = %u\n", sll.sll_pkttype));
        }
      } else
#endif
      {
        lwip_sock_make_addr(sock->conn, netbuf_fromaddr(buf), netbuf_fromport(buf),
                            (struct sockaddr *)msg->msg_name, &msg->msg_namelen);
      }
    }
  }

  /* Initialize flag output */
  msg->msg_flags = 0;

  if (msg->msg_control) {
    u8_t wrote_msg = 0;
#if LWIP_NETBUF_RECVINFO
    /* Check if packet info was recorded */
    if (buf->flags & NETBUF_FLAG_DESTADDR) {
      if (IP_IS_V4(&buf->toaddr)) {
#if LWIP_IPV4
        if (msg->msg_controllen >= CMSG_SPACE(sizeof(struct in_pktinfo))) {
          struct cmsghdr *chdr = CMSG_FIRSTHDR(msg); /* This will always return a header!! */
          struct in_pktinfo *pkti = (struct in_pktinfo *)CMSG_DATA(chdr);
          chdr->cmsg_level = IPPROTO_IP;
          chdr->cmsg_type = IP_PKTINFO;
          chdr->cmsg_len = CMSG_LEN(sizeof(struct in_pktinfo));
          pkti->ipi_ifindex = buf->p->if_idx;
          inet_addr_from_ip4addr(&pkti->ipi_addr, ip_2_ip4(netbuf_destaddr(buf)));
          msg->msg_controllen = CMSG_SPACE(sizeof(struct in_pktinfo));
          wrote_msg = 1;
        } else {
          msg->msg_flags |= MSG_CTRUNC;
        }
#endif /* LWIP_IPV4 */
      }
    }
#endif /* LWIP_NETBUF_RECVINFO */

    if (!wrote_msg) {
      msg->msg_controllen = 0;
    }
  }

  /* If we don't peek the incoming message: zero lastdata pointer and free the netbuf */
  if ((flags & MSG_PEEK) == 0) {
    sock->lastdata.netbuf = NULL;
    netbuf_delete(buf);
    sock->conn->lrcv_left = 0;
  } else {
    /* store data left count on this rcv operation */
    sock->conn->lrcv_left = (u32_t)(buf->p->tot_len);
  }
  if (datagram_len) {
    *datagram_len = buflen;
  }
  return ERR_OK;
}

ssize_t
lwip_recvfrom(int s, void *mem, size_t len, int flags,
              struct sockaddr *from, socklen_t *fromlen)
{
  struct lwip_sock *sock;
  ssize_t ret;

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom(%d, %p, %"SZT_F", 0x%x, ..)\n", s, mem, len, flags));
  LWIP_ERROR("lwip_recvfrom: invalid arguments", ((mem != NULL)), set_errno(EFAULT);  return -1);
  LWIP_ERROR("lwip_recvfrom: invalid arguments",
             ((len != 0) && (flags >= 0) && ((fromlen == NULL) || (*((int*)fromlen) > 0))),
             set_errno(EINVAL); return -1);

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  unsigned int unacceptable_flags = ~(u32_t)(MSG_PEEK | MSG_DONTWAIT | MSG_NOSIGNAL);
  if (unacceptable_flags & (u32_t)flags) {
    sock_set_errno(sock, EOPNOTSUPP);
    done_socket(sock);
    return -1;
  }

  lwip_sock_lock(sock);
#if LWIP_TCP
  if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) == NETCONN_TCP) {
    ret = lwip_recv_tcp(sock, mem, len, flags);
    lwip_recv_tcp_from(sock, from, fromlen, "lwip_recvfrom", s, ret);
    lwip_sock_unlock(sock);
    done_socket(sock);
    return ret;
  } else
#endif
  {
    u16_t datagram_len = 0;
    struct iovec vec;
    struct msghdr msg;
    err_t err;
    vec.iov_base = mem;
    vec.iov_len = len;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;
    msg.msg_iov = &vec;
    msg.msg_iovlen = 1;
    msg.msg_name = from;
    msg.msg_namelen = (fromlen ? *fromlen : 0);
    err = lwip_recvfrom_udp_raw(sock, flags, &msg, &datagram_len, s);
    if (err != ERR_OK) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvfrom[UDP/RAW](%d): buf == NULL, error is \"%s\"!\n",
                                  s, lwip_strerr(err)));
      sock_set_errno(sock, err_to_errno(err));
      lwip_sock_unlock(sock);
      done_socket(sock);
      return (err == ERR_CLSD) ? 0 : -1;
    }
    ret = (ssize_t)LWIP_MIN(LWIP_MIN(len, datagram_len), SSIZE_MAX);
    if (fromlen) {
      *fromlen = msg.msg_namelen;
    }
  }

  sock_set_errno(sock, 0);
  lwip_sock_unlock(sock);
  done_socket(sock);
  return ret;
}

ssize_t
lwip_read(int s, void *mem, size_t len)
{
  return lwip_recvfrom(s, mem, len, 0, NULL, NULL);
}

ssize_t
lwip_readv(int s, const struct iovec *iov, int iovcnt)
{
  struct msghdr msg;

  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  /* Hack: we have to cast via number to cast from 'const' pointer to non-const.
     Blame the opengroup standard for this inconsistency. */
  msg.msg_iov = LWIP_CONST_CAST(struct iovec *, iov);
  msg.msg_iovlen = iovcnt;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;
  return lwip_recvmsg(s, &msg, 0);
}

ssize_t
lwip_recv(int s, void *mem, size_t len, int flags)
{
  return lwip_recvfrom(s, mem, len, flags, NULL, NULL);
}

ssize_t
lwip_recvmsg(int s, struct msghdr *message, int flags)
{
  struct lwip_sock *sock;
  int i;
  ssize_t buflen;

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvmsg(%d, message=%p, flags=0x%x)\n", s, (void *)message, flags));
  LWIP_ERROR("lwip_recvmsg: invalid message pointer", message != NULL, return ERR_ARG;);
  LWIP_ERROR("lwip_recvmsg: unsupported flags", (flags & ~(MSG_PEEK|MSG_DONTWAIT)) == 0,
             set_errno(EOPNOTSUPP); return -1;);

  if ((message->msg_iovlen <= 0) || (message->msg_iovlen > IOV_MAX)) {
    set_errno(EMSGSIZE);
    return -1;
  }

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  lwip_sock_lock(sock);
  /* check for valid vectors */
  buflen = 0;
  for (i = 0; i < message->msg_iovlen; i++) {
    if ((message->msg_iov[i].iov_base == NULL) || ((ssize_t)message->msg_iov[i].iov_len <= 0) ||
        ((size_t)(ssize_t)message->msg_iov[i].iov_len != message->msg_iov[i].iov_len) ||
        ((ssize_t)(buflen + (ssize_t)message->msg_iov[i].iov_len) <= 0)) {
      sock_set_errno(sock, err_to_errno(ERR_VAL));
      lwip_sock_unlock(sock);
      done_socket(sock);
      return -1;
    }
    buflen = (ssize_t)(buflen + (ssize_t)message->msg_iov[i].iov_len);
  }

  if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) == NETCONN_TCP) {
#if LWIP_TCP
    int recv_flags = flags;
    message->msg_flags = 0;
    /* recv the data */
    buflen = 0;
    for (i = 0; i < message->msg_iovlen; i++) {
      /* try to receive into this vector's buffer */
      ssize_t recvd_local = lwip_recv_tcp(sock, message->msg_iov[i].iov_base, message->msg_iov[i].iov_len, recv_flags);
      if (recvd_local > 0) {
        /* sum up received bytes */
        buflen += recvd_local;
      }
      if ((recvd_local <= 0) || (recvd_local < (int)message->msg_iov[i].iov_len) ||
          (flags & MSG_PEEK)) {
        /* returned prematurely (or peeking, which might actually be limitated to the first iov) */
        if (buflen <= 0) {
          /* nothing received at all, propagate the error */
          buflen = recvd_local;
        }
        break;
      }
      /* pass MSG_DONTWAIT to lwip_recv_tcp() to prevent waiting for more data */
      recv_flags |= MSG_DONTWAIT;
    }
    if (buflen > 0) {
      /* reset socket error since we have received something */
      sock_set_errno(sock, 0);
    }
    /* " If the socket is connected, the msg_name and msg_namelen members shall be ignored." */
    lwip_sock_unlock(sock);
    done_socket(sock);
    return buflen;
#else /* LWIP_TCP */
    sock_set_errno(sock, err_to_errno(ERR_ARG));
    lwip_sock_unlock(sock);
    done_socket(sock);
    return -1;
#endif /* LWIP_TCP */
  }
  /* else, UDP and RAW NETCONNs */
#if LWIP_UDP || LWIP_RAW
  {
    u16_t datagram_len = 0;
    err_t err;
    err = lwip_recvfrom_udp_raw(sock, flags, message, &datagram_len, s);
    if (err != ERR_OK) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_recvmsg[UDP/RAW](%d): buf == NULL, error is \"%s\"!\n",
                                  s, lwip_strerr(err)));
      sock_set_errno(sock, err_to_errno(err));
      lwip_sock_unlock(sock);
      done_socket(sock);
      return (err == ERR_CLSD) ? 0 : -1;
    }
    if (datagram_len > buflen) {
      message->msg_flags |= MSG_TRUNC;
    }

    sock_set_errno(sock, 0);
    lwip_sock_unlock(sock);
    done_socket(sock);
    return (int)datagram_len;
  }
#else /* LWIP_UDP || LWIP_RAW */
  sock_set_errno(sock, err_to_errno(ERR_ARG));
  lwip_sock_unlock(sock);
  done_socket(sock);
  return -1;
#endif /* LWIP_UDP || LWIP_RAW */
}

ssize_t
lwip_send(int s, const void *data, size_t size, int flags)
{
  struct lwip_sock *sock;
  err_t err;
  u8_t write_flags;
  size_t written;
  unsigned int acceptable_flags;

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_send(%d, data=%p, size=%"SZT_F", flags=0x%x)\n",
                              s, data, size, flags));

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) != NETCONN_TCP) {
#if (LWIP_UDP || LWIP_RAW)
    done_socket(sock);
    return lwip_sendto(s, data, size, flags, NULL, 0);
#else /* (LWIP_UDP || LWIP_RAW) */
    sock_set_errno(sock, err_to_errno(ERR_ARG));
    done_socket(sock);
    return -1;
#endif /* (LWIP_UDP || LWIP_RAW) */
  }

  LWIP_ERROR("lwip_send: invalid arguments", ((data != NULL) && (size != 0) && (flags >= 0)),
              sock_set_errno(sock, err_to_errno(ERR_VAL)); done_socket(sock); return -1;);

  acceptable_flags = MSG_DONTWAIT | MSG_MORE | MSG_NOSIGNAL;
  LWIP_ERROR("lwip_send: unsupported flags", !((~acceptable_flags) & flags),
              sock_set_errno(sock, EOPNOTSUPP); done_socket(sock); return -1;);

  write_flags = (u8_t)(NETCONN_COPY |
                       ((flags & MSG_MORE)     ? NETCONN_MORE      : 0) |
                       ((flags & MSG_DONTWAIT) ? NETCONN_DONTBLOCK : 0));
  written = 0;
  err = netconn_write_partly(sock->conn, data, size, write_flags, &written);

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_send(%d) err=%d written=%"SZT_F"\n", s, err, written));
  if (err != ERR_OK) {
    sock_set_errno(sock, err_to_errno(err));
  }
  done_socket(sock);
  /* casting 'written' to ssize_t is OK here since the netconn API limits it to SSIZE_MAX */
  return (err == ERR_OK ? (ssize_t)written : -1);
}

ssize_t
lwip_sendmsg(int s, const struct msghdr *msg, int flags)
{
  struct lwip_sock *sock;
#if LWIP_TCP
  u8_t write_flags;
  size_t written;
  unsigned int acceptable_flags = 0;
#endif
  err_t err = ERR_OK;

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  LWIP_ERROR("lwip_sendmsg: invalid msghdr", msg != NULL,
             sock_set_errno(sock, err_to_errno(ERR_ARG)); done_socket(sock); return -1;);
  LWIP_ERROR("lwip_sendmsg: invalid msghdr iov", msg->msg_iov != NULL,
             sock_set_errno(sock, err_to_errno(ERR_ARG)); done_socket(sock); return -1;);
  LWIP_ERROR("lwip_sendmsg: maximum iovs exceeded", (msg->msg_iovlen > 0) && (msg->msg_iovlen <= IOV_MAX),
             sock_set_errno(sock, EMSGSIZE); done_socket(sock); return -1;);
  LWIP_ERROR("lwip_sendmsg: unsupported flags", (flags & ~(MSG_DONTWAIT | MSG_MORE)) == 0,
             sock_set_errno(sock, EOPNOTSUPP); done_socket(sock); return -1;);

  LWIP_UNUSED_ARG(msg->msg_control);
  LWIP_UNUSED_ARG(msg->msg_controllen);
  LWIP_UNUSED_ARG(msg->msg_flags);

  if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) == NETCONN_TCP) {
#if LWIP_TCP

    acceptable_flags = MSG_DONTWAIT | MSG_MORE | MSG_NOSIGNAL;
    LWIP_ERROR("lwip_send: unsupported flags", !((~acceptable_flags) & flags),
                sock_set_errno(sock, EOPNOTSUPP); done_socket(sock); return -1;);

    write_flags = (u8_t)(NETCONN_COPY |
                         ((flags & MSG_MORE)     ? NETCONN_MORE      : 0) |
                         ((flags & MSG_DONTWAIT) ? NETCONN_DONTBLOCK : 0));

    written = 0;
    err = netconn_write_vectors_partly(sock->conn, (struct netvector *)msg->msg_iov, (u16_t)msg->msg_iovlen, write_flags, &written);
    if (err != ERR_OK) {
      sock_set_errno(sock, err_to_errno(err));
    }
    done_socket(sock);
    /* casting 'written' to ssize_t is OK here since the netconn API limits it to SSIZE_MAX */
    return (err == ERR_OK ? (ssize_t)written : -1);
#else /* LWIP_TCP */
    sock_set_errno(sock, err_to_errno(ERR_ARG));
    done_socket(sock);
    return -1;
#endif /* LWIP_TCP */
  }
  /* else, UDP and RAW NETCONNs */
#if LWIP_UDP || LWIP_RAW
  {
    struct netbuf chain_buf;
    int i;
    ssize_t size = 0;

    LWIP_UNUSED_ARG(flags);
    LWIP_ERROR("lwip_sendmsg: invalid msghdr name", (((msg->msg_name == NULL) && (msg->msg_namelen == 0)) ||
               IS_SOCK_ADDR_LEN_VALID(msg->msg_namelen)),
               sock_set_errno(sock, err_to_errno(ERR_ARG)); done_socket(sock); return -1;);

    /* initialize chain buffer with destination */
    memset(&chain_buf, 0, sizeof(struct netbuf));
    if (msg->msg_name) {
      u16_t remote_port;
      SOCKADDR_TO_IPADDR_PORT((const struct sockaddr *)msg->msg_name, &chain_buf.addr, remote_port);
      netbuf_fromport(&chain_buf) = remote_port;
    }
#if LWIP_NETIF_TX_SINGLE_PBUF
    for (i = 0; i < msg->msg_iovlen; i++) {
      size += msg->msg_iov[i].iov_len;
      if ((msg->msg_iov[i].iov_len > INT_MAX) || (size < (int)msg->msg_iov[i].iov_len)) {
        /* overflow */
        goto sendmsg_emsgsize;
      }
    }
    if (size > 0xFFFF) {
      /* overflow */
      goto sendmsg_emsgsize;
    }
    /* Allocate a new netbuf and copy the data into it. */
    if (netbuf_alloc(&chain_buf, (u16_t)size, NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn))) == NULL) {
      err = ERR_MEM;
    } else {
      /* flatten the IO vectors */
      size_t offset = 0;
      for (i = 0; i < msg->msg_iovlen; i++) {
        MEMCPY(&((u8_t *)chain_buf.p->payload)[offset], msg->msg_iov[i].iov_base, msg->msg_iov[i].iov_len);
        offset += msg->msg_iov[i].iov_len;
      }
#if LWIP_CHECKSUM_ON_COPY
      {
        /* This can be improved by using LWIP_CHKSUM_COPY() and aggregating the checksum for each IO vector */
        u16_t chksum = (u16_t)(~inet_chksum_pbuf(chain_buf.p));
        netbuf_set_chksum(&chain_buf, chksum);
      }
#endif /* LWIP_CHECKSUM_ON_COPY */
      err = ERR_OK;
    }
#else /* LWIP_NETIF_TX_SINGLE_PBUF */
    /* create a chained netbuf from the IO vectors. NOTE: we assemble a pbuf chain
       manually to avoid having to allocate, chain, and delete a netbuf for each iov */
    for (i = 0; i < msg->msg_iovlen; i++) {
      struct pbuf *p;
      if (msg->msg_iov[i].iov_len > 0xFFFF) {
        /* overflow */
        goto sendmsg_emsgsize;
      }
      p = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_REF);
      if (p == NULL) {
        err = ERR_MEM; /* let netbuf_delete() cleanup chain_buf */
        break;
      }
      p->payload = msg->msg_iov[i].iov_base;
      p->len = p->tot_len = (u16_t)msg->msg_iov[i].iov_len;
      /* netbuf empty, add new pbuf */
      if (chain_buf.p == NULL) {
        chain_buf.p = chain_buf.ptr = p;
        /* add pbuf to existing pbuf chain */
      } else {
        if (chain_buf.p->tot_len + p->len > 0xffff) {
          /* overflow */
          pbuf_free(p);
          goto sendmsg_emsgsize;
        }
        pbuf_cat(chain_buf.p, p);
      }
    }
    /* save size of total chain */
    if (err == ERR_OK) {
      size = netbuf_len(&chain_buf);
    }
#endif /* LWIP_NETIF_TX_SINGLE_PBUF */

    if (err == ERR_OK) {
#if LWIP_IPV4 && LWIP_IPV6
      /* Dual-stack: Unmap IPv4 mapped IPv6 addresses */
      if (IP_IS_V6_VAL(chain_buf.addr) && ip6_addr_isipv4mappedipv6(ip_2_ip6(&chain_buf.addr))) {
        unmap_ipv4_mapped_ipv6(ip_2_ip4(&chain_buf.addr), ip_2_ip6(&chain_buf.addr));
        IP_SET_TYPE_VAL(chain_buf.addr, IPADDR_TYPE_V4);
      }
#endif /* LWIP_IPV4 && LWIP_IPV6 */

      /* send the data */
      err = netconn_send(sock->conn, &chain_buf);
    }

    /* deallocated the buffer */
    netbuf_free(&chain_buf);

    if (err != ERR_OK) {
      sock_set_errno(sock, err_to_errno(err));
    }
    done_socket(sock);
    return (err == ERR_OK ? size : -1);
sendmsg_emsgsize:
    sock_set_errno(sock, EMSGSIZE);
    netbuf_free(&chain_buf);
    done_socket(sock);
    return -1;
  }
#else /* LWIP_UDP || LWIP_RAW */
  sock_set_errno(sock, err_to_errno(ERR_ARG));
  done_socket(sock);
  return -1;
#endif /* LWIP_UDP || LWIP_RAW */
}

ssize_t
lwip_sendto(int s, const void *data, size_t size, int flags,
            const struct sockaddr *to, socklen_t tolen)
{
  struct lwip_sock *sock;
  err_t err;
  u16_t short_size;
  u16_t remote_port = 0;
  struct netbuf buf;
#if PF_PKT_SUPPORT
  const struct sockaddr_ll *to_ll = NULL;
#endif
  const struct sockaddr_in *to_in = NULL;

#if LWIP_UDP || LWIP_RAW
  SYS_ARCH_DECL_PROTECT(lev);
#endif

  unsigned int acceptable_flags = 0;

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) == NETCONN_TCP) {
#if LWIP_TCP
    done_socket(sock);
    return lwip_send(s, data, size, flags);
#else /* LWIP_TCP */
    LWIP_UNUSED_ARG(flags);
    sock_set_errno(sock, err_to_errno(ERR_ARG));
    done_socket(sock);
    return -1;
#endif /* LWIP_TCP */
  }

#if PF_PKT_SUPPORT
  if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn))  & NETCONN_PKT_RAW) {
    to_ll = (const struct sockaddr_ll *)(void *)to;

    LWIP_ERROR("lwip_sendto: invalid address", (((data != NULL) && (size!=0)) &&
               (((to_ll != NULL) && (tolen == sizeof(struct sockaddr_ll)))  &&
               ((to_ll->sll_family) == PF_PACKET) && ((((mem_ptr_t)to_ll) % 4) == 0)) &&
               (to_ll->sll_ifindex <= LWIP_NETIF_IFINDEX_MAX)),
               sock_set_errno(sock, err_to_errno(ERR_VAL));
               done_socket(sock); return -1);

    LWIP_ERROR("lwip_sendto: invalid address family", ((to_ll->sll_family) == PF_PACKET),
               sock_set_errno(sock, err_to_errno(ERR_AFNOSUPP));
               done_socket(sock); return -1);

    LWIP_ERROR("lwip_sendto: invalid flags. Should be 0", (flags == 0),
               sock_set_errno(sock, err_to_errno(ERR_OPNOTSUPP));
               done_socket(sock); return -1);

    if (size > LWIP_MAX_PF_RAW_SEND_SIZE) {
      sock_set_errno(sock, EMSGSIZE);
      done_socket(sock);
      return -1;
    }
  } else
#endif
  {
    to_in = (const struct sockaddr_in *)(void*)to;
    acceptable_flags = acceptable_flags | MSG_MORE | MSG_DONTWAIT | MSG_NOSIGNAL;
    if ((~acceptable_flags) & (unsigned int)flags) {
      sock_set_errno(sock, err_to_errno(ERR_OPNOTSUPP));
      done_socket(sock);
      return -1;
    }

    LWIP_ERROR("lwip_sendto: invalid address", ((data != NULL) && (size!=0) && (flags>=0)),
               sock_set_errno(sock, err_to_errno(ERR_VAL));
               done_socket(sock); return -1);

    LWIP_ERROR("lwip_sendto: invalid address", (((to_in == NULL) && (tolen == 0)) ||
               (((to_in == NULL) && (tolen != 0)) || IS_SOCK_ADDR_LEN_VALID(tolen))),
               sock_set_errno(sock, err_to_errno(ERR_VAL));
               done_socket(sock);
               return -1);

#if LWIP_CHECK_ADDR_ALIGN
    LWIP_ERROR("lwip_sendto: invalid address", (IS_SOCK_ADDR_ALIGNED(to_in)),
               sock_set_errno(sock, err_to_errno(ERR_VAL));
               done_socket(sock);
               return -1);
#endif

    LWIP_ERROR("lwip_sendto: invalid address", (to == NULL || IS_SOCK_ADDR_TYPE_VALID(to)),
               sock_set_errno(sock, err_to_errno(ERR_AFNOSUPPORT));
               done_socket(sock);
               return -1);

    if (size > LWIP_MAX_UDP_RAW_SEND_SIZE) {
      sock_set_errno(sock, EMSGSIZE);
      done_socket(sock);
      return -1;
    }
  }

  /* size must fit in u16_t */
  short_size = (u16_t)size;
  LWIP_UNUSED_ARG(tolen);

  /* initialize a buffer */
  buf.p = buf.ptr = NULL;
#if LWIP_CHECKSUM_ON_COPY || PF_PKT_SUPPORT
  buf.flags = 0;
#endif /* LWIP_CHECKSUM_ON_COPY || PF_PKT_SUPPORT */
#if PF_PKT_SUPPORT
  buf.netifindex = 0;
#endif
  if (to_in) {
    SOCKADDR_TO_IPADDR_PORT(to, &buf.addr, remote_port);
    netbuf_fromport(&buf) = remote_port;
#if LWIP_ENABLE_NET_CAPABILITY && LWIP_ENABLE_CAP_NET_BROADCAST
    if (ip_addr_ismulticast(&buf.addr) || ip_addr_isbroadcast_by_sock(&buf.addr, sock)) {
      LWIP_ERROR("lwip_sendto permission deny: NET_BROADCAST\n", IsCapPermit(CAP_NET_BROADCAST),
                 sock_set_errno(sock, EPERM); done_socket(sock); return -1);
    }
#endif
  } else {
#if PF_PKT_SUPPORT
    if (to_ll != NULL) {
      buf.flags |= NETBUF_FLAG_IFINDEX;
      buf.netifindex = (u8_t)to_ll->sll_ifindex;
    } else
#endif
    {
#if LWIP_UDP
      if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) == NETCONN_UDP) {
        SYS_ARCH_PROTECT(lev);
        if (sock->conn->pcb.udp != NULL && (sock->conn->pcb.udp->flags & UDP_FLAGS_CONNECTED)) {
          buf.addr = sock->conn->pcb.udp->remote_ip;
          remote_port           = sock->conn->pcb.udp->remote_port;
          SYS_ARCH_UNPROTECT(lev);
          netbuf_fromport(&buf) = remote_port;
        } else {
          SYS_ARCH_UNPROTECT(lev);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("No address provide for UDP unconnect socket\n"));
          sock_set_errno(sock, err_to_errno(ERR_NODEST));
          done_socket(sock);
          return -1;
        }
      }
#endif
#if LWIP_RAW
      if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) & NETCONN_RAW) {
        SYS_ARCH_PROTECT(lev);
        if (sock->conn->pcb.raw != NULL && (sock->conn->pcb.raw->flags & RAW_FLAGS_CONNECTED)) {
          buf.addr = sock->conn->pcb.raw->remote_ip;
          SYS_ARCH_UNPROTECT(lev);
        } else {
          SYS_ARCH_UNPROTECT(lev);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("No address provide for unconnect RAW socket\n"));
          sock_set_errno(sock, err_to_errno(ERR_NODEST));
          done_socket(sock);
          return -1;
        }
      }
#endif
    }
  }

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_sendto(%d, data=%p, short_size=%"U16_F", flags=0x%x to=",
                              s, data, short_size, flags));
#if PF_PKT_SUPPORT
  if (buf.flags & NETBUF_FLAG_IFINDEX) {
    LWIP_DEBUGF(SOCKETS_DEBUG, (" netifindex = %d\n", buf.netifindex));
  } else
#endif
  {
    ip_addr_debug_print_val(SOCKETS_DEBUG, buf.addr);
    LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F"\n", remote_port));
  }
  /* make the buffer point to the data that should be sent */
#if LWIP_NETIF_TX_SINGLE_PBUF
  /* Allocate a new netbuf and copy the data into it. */
  if (netbuf_alloc(&buf, short_size, NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn))) == NULL) {
    err = ERR_MEM;
  } else {
#if LWIP_CHECKSUM_ON_COPY
#if PF_PKT_SUPPORT
    if ((NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) != NETCONN_PKT_RAW) &&
        (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) != NETCONN_RAW))
#else
    if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) != NETCONN_RAW)
#endif
    {
      u16_t chksum = LWIP_CHKSUM_COPY(buf.p->payload, data, short_size);
      netbuf_set_chksum(&buf, chksum);
    } else
#endif /* LWIP_CHECKSUM_ON_COPY */
    {
      MEMCPY(buf.p->payload, data, short_size);
    }
    err = ERR_OK;
  }
#else /* LWIP_NETIF_TX_SINGLE_PBUF */
  err = netbuf_ref(&buf, data, short_size);
#endif /* LWIP_NETIF_TX_SINGLE_PBUF */
  if (err == ERR_OK) {
#if LWIP_IPV4 && LWIP_IPV6
    /* Dual-stack: Unmap IPv4 mapped IPv6 addresses */
    if (IP_IS_V6_VAL(buf.addr) && ip6_addr_isipv4mappedipv6(ip_2_ip6(&buf.addr))) {
      unmap_ipv4_mapped_ipv6(ip_2_ip4(&buf.addr), ip_2_ip6(&buf.addr));
      IP_SET_TYPE_VAL(buf.addr, IPADDR_TYPE_V4);
    }
#endif /* LWIP_IPV4 && LWIP_IPV6 */

    /* send the data */
    err = netconn_send(sock->conn, &buf);
  }

  /* deallocated the buffer */
  netbuf_free(&buf);

  sock_set_errno(sock, err_to_errno(err));
  done_socket(sock);
  return (err == ERR_OK ? short_size : -1);
}

int
lwip_socket(int domain, int type, int protocol)
{
  struct netconn *conn;
  int i;

  LWIP_UNUSED_ARG(domain); /* @todo: check this */
  LWIP_ERROR("domain invalid\n",  (LWIP_IS_VALID_DOMAIN(domain)),
             set_errno(EAFNOSUPPORT); return -1);

  LWIP_ERROR("flag invalid\n", !(type & ~SOCK_TYPE_MASK),
             set_errno(EINVAL); return -1);

#if PF_PKT_SUPPORT
  LWIP_ERROR("Invalid socket type for PF_PACKET domain\n", ((domain != PF_PACKET) || (type == SOCK_RAW)),
             set_errno(ESOCKTNOSUPPORT); return -1);
#endif

  /* create a netconn */
  switch (type) {
    case SOCK_RAW:
#if LWIP_ENABLE_NET_CAPABILITY
      LWIP_ERROR("permission deny: NET_RAW\n", IsCapPermit(CAP_NET_RAW),
                 set_errno(EPERM); return -1);
#endif
#if PF_PKT_SUPPORT
      if (domain == PF_PACKET) {
        conn = netconn_new_with_proto_and_callback(DOMAIN_TO_NETCONN_TYPE(domain, NETCONN_PKT_RAW),
                                                   (u16_t)protocol, event_callback);
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_socket(%s, SOCK_RAW, %d) = ", "PF_PACKET", protocol));
      } else
#endif
      {
        conn = netconn_new_with_proto_and_callback(DOMAIN_TO_NETCONN_TYPE(domain, NETCONN_RAW),
               (u16_t)protocol, DEFAULT_SOCKET_EVENTCB);
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_socket(%s, SOCK_RAW, %d) = ",
                                    domain == PF_INET ? "PF_INET" : "UNKNOWN", protocol));
      }
      break;
    case SOCK_DGRAM:
      LWIP_ERROR("protocol invalid\n",
                 (protocol == IPPROTO_UDPLITE || protocol == IPPROTO_UDP || LWIP_IS_IPPROTOCOL(protocol)),
                 set_errno(EPROTONOSUPPORT); return -1;);
      conn = netconn_new_with_callback(DOMAIN_TO_NETCONN_TYPE(domain,
                                       ((protocol == IPPROTO_UDPLITE) ? NETCONN_UDPLITE : NETCONN_UDP)),
                                       DEFAULT_SOCKET_EVENTCB);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_socket(%s, SOCK_DGRAM, %d) = ",
                                  domain == PF_INET ? "PF_INET" : "UNKNOWN", protocol));
#if LWIP_NETBUF_RECVINFO
      if (conn) {
        /* netconn layer enables pktinfo by default, sockets default to off */
        conn->flags &= ~NETCONN_FLAG_PKTINFO;
      }
#endif /* LWIP_NETBUF_RECVINFO */
      break;
    case SOCK_STREAM:
      LWIP_ERROR("protocol invalid\n", (protocol == IPPROTO_TCP || LWIP_IS_IPPROTOCOL(protocol)),
                                     set_errno(EPROTONOSUPPORT); return -1;);
      conn = netconn_new_with_callback(DOMAIN_TO_NETCONN_TYPE(domain, NETCONN_TCP), DEFAULT_SOCKET_EVENTCB);
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_socket(%s, SOCK_STREAM, %d) = ",
                                  domain == PF_INET ? "PF_INET" : "UNKNOWN", protocol));
      break;
    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_socket(%d, %d/UNKNOWN, %d) = -1\n",
                                  domain, type, protocol));
      set_errno(ESOCKTNOSUPPORT);
      return -1;
  }

  if (!conn) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("-1 / ENOBUFS (could not create netconn)\n"));
    set_errno(ENOBUFS);
    return -1;
  }

  i = alloc_socket(conn, 0);

  if (i == -1) {
    netconn_delete(conn);
    set_errno(ENFILE);
    return -1;
  }
  conn->socket = i;
  done_socket(&sockets[i - LWIP_SOCKET_OFFSET]);
  LWIP_DEBUGF(SOCKETS_DEBUG, ("%d\n", i));
  set_errno(0);
  return i;
}

ssize_t
lwip_write(int s, const void *data, size_t size)
{
  return lwip_send(s, data, size, 0);
}

ssize_t
lwip_writev(int s, const struct iovec *iov, int iovcnt)
{
  struct msghdr msg;

  LWIP_ERROR("iovcnt invalid\n", (iovcnt >= 0),
             set_errno(EINVAL); return -1);
  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  /* Hack: we have to cast via number to cast from 'const' pointer to non-const.
     Blame the opengroup standard for this inconsistency. */
  msg.msg_iov = LWIP_CONST_CAST(struct iovec *, iov);
  msg.msg_iovlen = iovcnt;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;
  return lwip_sendmsg(s, &msg, 0);
}

#if LWIP_SOCKET_SELECT || LWIP_SOCKET_POLL
/* Add select_cb to select_cb_list. */
static void
lwip_link_select_cb(struct lwip_select_cb *select_cb)
{
  LWIP_SOCKET_SELECT_DECL_PROTECT(lev);

  /* Protect the select_cb_list */
  LWIP_SOCKET_SELECT_PROTECT(lev);

  /* Put this select_cb on top of list */
  select_cb->next = select_cb_list;
  if (select_cb_list != NULL) {
    select_cb_list->prev = select_cb;
  }
  select_cb_list = select_cb;
#if !LWIP_TCPIP_CORE_LOCKING
  /* Increasing this counter tells select_check_waiters that the list has changed. */
  select_cb_ctr++;
#endif

  /* Now we can safely unprotect */
  LWIP_SOCKET_SELECT_UNPROTECT(lev);
}

/* Remove select_cb from select_cb_list. */
static void
lwip_unlink_select_cb(struct lwip_select_cb *select_cb)
{
  LWIP_SOCKET_SELECT_DECL_PROTECT(lev);

  /* Take us off the list */
  LWIP_SOCKET_SELECT_PROTECT(lev);
  if (select_cb->next != NULL) {
    select_cb->next->prev = select_cb->prev;
  }
  if (select_cb_list == select_cb) {
    LWIP_ASSERT("select_cb->prev == NULL", select_cb->prev == NULL);
    select_cb_list = select_cb->next;
  } else {
    LWIP_ASSERT("select_cb->prev != NULL", select_cb->prev != NULL);
    select_cb->prev->next = select_cb->next;
  }
#if !LWIP_TCPIP_CORE_LOCKING
  /* Increasing this counter tells select_check_waiters that the list has changed. */
  select_cb_ctr++;
#endif
  LWIP_SOCKET_SELECT_UNPROTECT(lev);
}
#endif /* LWIP_SOCKET_SELECT || LWIP_SOCKET_POLL */

#define LWIP_MAX_SELECT_TIMEOUT_VAL_MSEC 0xFFFFFFFEU // in milliseconds

#if LWIP_SOCKET_SELECT
/**
 * Go through the readset and writeset lists and see which socket of the sockets
 * set in the sets has events. On return, readset, writeset and exceptset have
 * the sockets enabled that had events.
 *
 * @param maxfdp1 the highest socket index in the sets
 * @param readset_in    set of sockets to check for read events
 * @param writeset_in   set of sockets to check for write events
 * @param exceptset_in  set of sockets to check for error events
 * @param readset_out   set of sockets that had read events
 * @param writeset_out  set of sockets that had write events
 * @param exceptset_out set os sockets that had error events
 * @return number of sockets that had events (read/write/exception) (>= 0)
 */
static int
lwip_selscan(int maxfdp1, fd_set *readset_in, fd_set *writeset_in, fd_set *exceptset_in,
             fd_set *readset_out, fd_set *writeset_out, fd_set *exceptset_out)
{
  int i, nready = 0;
  fd_set lreadset, lwriteset, lexceptset;
  struct lwip_sock *sock;
  SYS_ARCH_DECL_PROTECT(lev);

  FD_ZERO(&lreadset);
  FD_ZERO(&lwriteset);
  FD_ZERO(&lexceptset);

  /* Go through each socket in each list to count number of sockets which
     currently match */
  for (i = LWIP_SOCKET_OFFSET; i < maxfdp1; i++) {
    /* if this FD is not in the set, continue */
    if (!(readset_in && FD_ISSET(i, readset_in)) &&
        !(writeset_in && FD_ISSET(i, writeset_in)) &&
        !(exceptset_in && FD_ISSET(i, exceptset_in))) {
      continue;
    }
    /* First get the socket's status (protected)... */
    SYS_ARCH_PROTECT(lev);
    sock = tryget_socket_unconn_locked(i);
    if (sock != NULL) {
      void *lastdata = sock->lastdata.pbuf;
      s16_t rcvevent = sock->rcvevent;
      u16_t sendevent = sock->sendevent;
      u16_t errevent = sock->errevent;
      SYS_ARCH_UNPROTECT(lev);

      /* ... then examine it: */
      /* See if netconn of this socket is ready for read */
      if (readset_in && FD_ISSET(i, readset_in) && ((lastdata != NULL) || (rcvevent > 0))) {
        FD_SET(i, &lreadset);
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_selscan: fd=%d ready for reading\n", i));
        nready++;
      }
      /* See if netconn of this socket is ready for write */
      if (writeset_in && FD_ISSET(i, writeset_in) && (sendevent != 0)) {
        FD_SET(i, &lwriteset);
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_selscan: fd=%d ready for writing\n", i));
        nready++;
      }
      /* See if netconn of this socket had an error */
      if (exceptset_in && FD_ISSET(i, exceptset_in) && (errevent != 0)) {
        FD_SET(i, &lexceptset);
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_selscan: fd=%d ready for exception\n", i));
        nready++;
      }
      done_socket(sock);
    } else {
      SYS_ARCH_UNPROTECT(lev);
      /* no a valid open socket */
      return -1;
    }
  }
  /* copy local sets to the ones provided as arguments */
  *readset_out = lreadset;
  *writeset_out = lwriteset;
  *exceptset_out = lexceptset;

  LWIP_ASSERT("nready >= 0", nready >= 0);
  return nready;
}

#if LWIP_NETCONN_FULLDUPLEX
static void
lwip_select_dec_sockets_used(int maxfdp, fd_set *used_sockets);
/* Mark all of the set sockets in one of the three fdsets passed to select as used.
 * All sockets are marked (and later unmarked), whether they are open or not.
 * This is OK as lwip_selscan aborts select when non-open sockets are found.
 */
static err_t
lwip_select_inc_sockets_used_set(int maxfdp, fd_set *fdset, fd_set *used_sockets)
{
  SYS_ARCH_DECL_PROTECT(lev);
  if (fdset) {
    int i;
    for (i = LWIP_SOCKET_OFFSET; i < maxfdp; i++) {
      /* if this FD is in the set, lock it (unless already done) */
      if (FD_ISSET(i, fdset) && !FD_ISSET(i, used_sockets)) {
        struct lwip_sock *sock;
        SYS_ARCH_PROTECT(lev);
        sock = tryget_socket_unconn_locked(i);
        if (sock != NULL) {
          /* leave the socket used until released by lwip_select_dec_sockets_used */
          FD_SET(i, used_sockets);
        } else {
          SYS_ARCH_UNPROTECT(lev);
          lwip_select_dec_sockets_used(maxfdp, used_sockets);
          return ERR_VAL;
        }
        SYS_ARCH_UNPROTECT(lev);
      }
    }
  }

  return ERR_OK;
}

/* Mark all sockets passed to select as used to prevent them from being freed
 * from other threads while select is running.
 * Marked sockets are added to 'used_sockets' to mark them only once an be able
 * to unmark them correctly.
 */
static err_t
lwip_select_inc_sockets_used(int maxfdp, fd_set *fdset1, fd_set *fdset2, fd_set *fdset3, fd_set *used_sockets)
{
  err_t retval;
  int i;
  u32_t used = 0;
  FD_ZERO(used_sockets);
  retval = lwip_select_inc_sockets_used_set(maxfdp, fdset1, used_sockets);
  if (retval != ERR_OK) {
    return retval;
  }

  retval = lwip_select_inc_sockets_used_set(maxfdp, fdset2, used_sockets);
  if (retval != ERR_OK) {
    return retval;
  }

  retval = lwip_select_inc_sockets_used_set(maxfdp, fdset3, used_sockets);
  if (retval != ERR_OK) {
    return retval;
  }

  for (i = LWIP_SOCKET_OFFSET; i < maxfdp; i++) {
    /* if this FD is not in the set, continue */
    if (FD_ISSET(i, used_sockets)) {
      used++;
    }
  }

  if (used == 0) {
    return ERR_VAL;
  }
  return ERR_OK;
}

/* Let go all sockets that were marked as used when starting select */
static void
lwip_select_dec_sockets_used(int maxfdp, fd_set *used_sockets)
{
  int i;
  for (i = LWIP_SOCKET_OFFSET; i < maxfdp; i++) {
    /* if this FD is not in the set, continue */
    if (FD_ISSET(i, used_sockets)) {
      struct lwip_sock *sock = tryget_socket_unconn_nouse(i);
      LWIP_ASSERT("socket gone at the end of select", sock != NULL);
      if (sock != NULL) {
        done_socket(sock);
      }
    }
  }
}
#else /* LWIP_NETCONN_FULLDUPLEX */
#define lwip_select_inc_sockets_used(maxfdp1, readset, writeset, exceptset, used_sockets)
#define lwip_select_dec_sockets_used(maxfdp1, used_sockets)
#endif /* LWIP_NETCONN_FULLDUPLEX */

int
lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            struct timeval *timeout)
{
  u32_t waitres = 0;
  int nready;
  fd_set lreadset, lwriteset, lexceptset;
  u32_t msectimeout;
  u64_t msectimeout_temp = 0;
  int i;
  int maxfdp2;
#if LWIP_NETCONN_SEM_PER_THREAD
  int waited = 0;
#endif
#if LWIP_NETCONN_FULLDUPLEX
  fd_set used_sockets;
#endif
  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_ERROR("lwip_select: invalid maxfdp1",
             (maxfdp1 > (int)LWIP_SOCKET_OFFSET) && (maxfdp1 <= (int)(LWIP_SOCKET_OFFSET + LWIP_CONFIG_NUM_SOCKETS)),
             set_errno(EINVAL); return -1);

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select(%d, %p, %p, %p, tvsec=%"S32_F" tvusec=%"S32_F")\n",
                              maxfdp1, (void *)readset, (void *) writeset, (void *) exceptset,
                              timeout ? (s32_t)timeout->tv_sec : (s32_t) - 1,
                              timeout ? (s32_t)timeout->tv_usec : (s32_t) - 1));
  if (timeout != NULL) {
    /* avoid overflow of timeout duration with very large value of sec or usec */
    if ((timeout->tv_sec < 0) || (timeout->tv_usec < 0)) {
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: Invalid value of timeout"));
        set_errno(EINVAL);
        return -1;
    }

    msectimeout_temp = ((u64_t)(unsigned long)timeout->tv_sec) * MS_PER_SECOND;
    if (msectimeout_temp > LWIP_MAX_SELECT_TIMEOUT_VAL_MSEC) {
      set_errno(EINVAL);
      return -1;
    }

    /* add 500us to round off to ms */
    if (((((u64_t)(unsigned long)timeout->tv_usec + (US_PER_MSECOND >> 1)) / US_PER_MSECOND) + msectimeout_temp) >
        LWIP_MAX_SELECT_TIMEOUT_VAL_MSEC) {
      set_errno(EINVAL);
      return -1;
    }

    msectimeout = (u32_t)(msectimeout_temp + (((unsigned long)timeout->tv_usec + 500) / US_PER_MSECOND));
    if (msectimeout == 0) {
      /* wait for atleast 1 ms */
      msectimeout = 1;
    }
  }

#if LWIP_NETCONN_FULLDUPLEX
  nready = lwip_select_inc_sockets_used(maxfdp1, readset, writeset, exceptset, &used_sockets);
  if (nready != ERR_OK) {
    set_errno(EBADF);
    return -1;
  }
#endif

  /* Go through each socket in each list to count number of sockets which
     currently match */
  nready = lwip_selscan(maxfdp1, readset, writeset, exceptset, &lreadset, &lwriteset, &lexceptset);

  if (nready < 0) {
    /* one of the sockets in one of the fd_sets was invalid */
    set_errno(EBADF);
    lwip_select_dec_sockets_used(maxfdp1, &used_sockets);
    return -1;
  } else if (nready > 0) {
    /* one or more sockets are set, no need to wait */
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: nready=%d\n", nready));
  } else {
    /* If we don't have any current events, then suspend if we are supposed to */
    if (timeout && timeout->tv_sec == 0 && timeout->tv_usec == 0) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: no timeout, returning 0\n"));
      /* This is OK as the local fdsets are empty and nready is zero,
         or we would have returned earlier. */
    } else {
      /* None ready: add our semaphore to list:
         We don't actually need any dynamic memory. Our entry on the
         list is only valid while we are in this function, so it's ok
         to use local variables (unless we're running in MPU compatible
         mode). */
      API_SELECT_CB_VAR_DECLARE(select_cb);
      API_SELECT_CB_VAR_ALLOC(select_cb, set_errno(ENOMEM); lwip_select_dec_sockets_used(maxfdp1, &used_sockets); return -1);
      memset(&API_SELECT_CB_VAR_REF(select_cb), 0, sizeof(struct lwip_select_cb));

      API_SELECT_CB_VAR_REF(select_cb).readset = readset;
      API_SELECT_CB_VAR_REF(select_cb).writeset = writeset;
      API_SELECT_CB_VAR_REF(select_cb).exceptset = exceptset;
#if LWIP_NETCONN_SEM_PER_THREAD
      API_SELECT_CB_VAR_REF(select_cb).sem = LWIP_NETCONN_THREAD_SEM_GET();
#else /* LWIP_NETCONN_SEM_PER_THREAD */
      if (sys_sem_new(&API_SELECT_CB_VAR_REF(select_cb).sem, 0) != ERR_OK) {
        /* failed to create semaphore */
        set_errno(ENOMEM);
        lwip_select_dec_sockets_used(maxfdp1, &used_sockets);
        API_SELECT_CB_VAR_FREE(select_cb);
        return -1;
      }
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

      lwip_link_select_cb(&API_SELECT_CB_VAR_REF(select_cb));

      /* Increase select_waiting for each socket we are interested in */
      maxfdp2 = maxfdp1;
      for (i = LWIP_SOCKET_OFFSET; i < maxfdp1; i++) {
        if ((readset && FD_ISSET(i, readset)) ||
            (writeset && FD_ISSET(i, writeset)) ||
            (exceptset && FD_ISSET(i, exceptset))) {
          struct lwip_sock *sock;
          SYS_ARCH_PROTECT(lev);
          sock = tryget_socket_unconn_locked(i);
          if (sock != NULL) {
            sock->select_waiting++;
            if (sock->select_waiting == 0) {
              /* overflow - too many threads waiting */
              sock->select_waiting--;
              nready = -1;
              maxfdp2 = i;
              SYS_ARCH_UNPROTECT(lev);
              done_socket(sock);
              set_errno(EBUSY);
              break;
            }
            SYS_ARCH_UNPROTECT(lev);
            done_socket(sock);
          } else {
            /* Not a valid socket */
            nready = -1;
            maxfdp2 = i;
            SYS_ARCH_UNPROTECT(lev);
            set_errno(EBADF);
            break;
          }
        }
      }

      if (nready >= 0) {
        /* Call lwip_selscan again: there could have been events between
           the last scan (without us on the list) and putting us on the list! */
        nready = lwip_selscan(maxfdp1, readset, writeset, exceptset, &lreadset, &lwriteset, &lexceptset);
        if (nready < 0) {
          set_errno(EBADF);
        } else if (!nready) {
          /* Still none ready, just wait to be woken */
          if (timeout == 0) {
            /* Wait forever */
            msectimeout = 0;
          } else {
            long msecs_long = ((timeout->tv_sec * 1000) + ((timeout->tv_usec + 500) / 1000));
            if (msecs_long <= 0) {
              /* Wait 1ms at least (0 means wait forever) */
              msectimeout = 1;
            } else {
              msectimeout = (u32_t)msecs_long;
            }
          }

          waitres = sys_arch_sem_wait(SELECT_SEM_PTR(API_SELECT_CB_VAR_REF(select_cb).sem), msectimeout);
#if LWIP_NETCONN_SEM_PER_THREAD
          waited = 1;
#endif
        }
      }

      /* Decrease select_waiting for each socket we are interested in */
      for (i = LWIP_SOCKET_OFFSET; i < maxfdp2; i++) {
        if ((readset && FD_ISSET(i, readset)) ||
            (writeset && FD_ISSET(i, writeset)) ||
            (exceptset && FD_ISSET(i, exceptset))) {
          struct lwip_sock *sock;
          SYS_ARCH_PROTECT(lev);
          sock = tryget_socket_unconn_nouse(i);
          LWIP_ASSERT("socket gone at the end of select", sock != NULL);
          if (sock != NULL) {
            /* for now, handle select_waiting==0... */
            LWIP_ASSERT("sock->select_waiting > 0", sock->select_waiting > 0);
            if (sock->select_waiting > 0) {
              sock->select_waiting--;
            }
            SYS_ARCH_UNPROTECT(lev);
          } else {
            SYS_ARCH_UNPROTECT(lev);
            /* Not a valid socket */
            nready = -1;
            set_errno(EBADF);
          }
        }
      }

      lwip_unlink_select_cb(&API_SELECT_CB_VAR_REF(select_cb));

#if LWIP_NETCONN_SEM_PER_THREAD
      if (API_SELECT_CB_VAR_REF(select_cb).sem_signalled && (!waited || (waitres == SYS_ARCH_TIMEOUT))) {
        /* don't leave the thread-local semaphore signalled */
        sys_arch_sem_wait(API_SELECT_CB_VAR_REF(select_cb).sem, 1);
      }
#else /* LWIP_NETCONN_SEM_PER_THREAD */
      sys_sem_free(&API_SELECT_CB_VAR_REF(select_cb).sem);
#endif /* LWIP_NETCONN_SEM_PER_THREAD */
      API_SELECT_CB_VAR_FREE(select_cb);

      if (nready < 0) {
        /* This happens when a socket got closed while waiting */
        lwip_select_dec_sockets_used(maxfdp1, &used_sockets);
        return -1;
      }

      if (waitres == SYS_ARCH_TIMEOUT) {
        /* Timeout */
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: timeout expired\n"));
        /* This is OK as the local fdsets are empty and nready is zero,
           or we would have returned earlier. */
      } else {
        /* See what's set now after waiting */
        nready = lwip_selscan(maxfdp1, readset, writeset, exceptset, &lreadset, &lwriteset, &lexceptset);
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: nready=%d\n", nready));
        if (nready < 0) {
          set_errno(EBADF);
          lwip_select_dec_sockets_used(maxfdp1, &used_sockets);
          return -1;
        }
      }
    }
  }

  lwip_select_dec_sockets_used(maxfdp1, &used_sockets);
  set_errno(0);
  if (readset) {
    *readset = lreadset;
  }
  if (writeset) {
    *writeset = lwriteset;
  }
  if (exceptset) {
    *exceptset = lexceptset;
  }
  return nready;
}
#endif /* LWIP_SOCKET_SELECT */

#if LWIP_SOCKET_POLL
#if LWIP_EXT_POLL_SUPPORT
int lwip_poll(int sockfd, poll_table *wait)
{
  struct lwip_sock *sock = NULL;
  struct netconn *conn = NULL;
  pollevent_t mask = 0;
  pollevent_t ret;

  SYS_ARCH_DECL_PROTECT(lev);
  LWIP_ASSERT("wait!= NULL", wait != NULL);

  sock = get_socket(sockfd);
  if (sock == NULL) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_poll: Invalid socket"));
    set_errno(EBADF);
    return -EBADF; /* compatible with file poll */
  }

  LOCK_TCPIP_CORE();
  SYS_ARCH_PROTECT(lev);

  conn = sock->conn;

  /* listen socket need special poll impl */
  if ((conn->type == SOCK_STREAM) && (conn->state == NETCONN_LISTEN)) {
    /* POLLIN if new connection establised by tcp stack */
    if (sock->rcvevent > 0) {
      mask |= POLLIN | POLLRDNORM;
      goto done;
    }
  }

  /* SHUTDOWN_MASK means both direction was closed due to some errors, eg. RST recved */
  if (conn->shutdown == SHUTDOWN_MASK) {
    mask |= POLLHUP | POLLIN | POLLRDNORM | POLLRDHUP | POLLOUT | POLLWRNORM;
  }

  /* FIN segment recved */
  if (conn->shutdown == RCV_SHUTDOWN) {
    mask |= POLLIN | POLLRDNORM | POLLRDHUP;
  }

  /* normal data readable, socket option SO_RCVLOWAT not support now */
  if ((sock->lastdata.pbuf != NULL) || (sock->rcvevent > 0)) {
    mask |= POLLIN | POLLRDNORM;
  }

  /* make socket writeable if any space usable */
  if ((conn->shutdown == SND_SHUTDOWN) || (sock->sendevent != 0)) {
    mask |= POLLOUT | POLLWRNORM;
  }

  if (sock->errevent != 0) {
    mask |= POLLERR;
  }

done:
  ret = mask & wait->key;
  if (!ret) {
    /* add socket to wait queue if no event got */
    poll_wait(NULL, &sock->wq, wait);
  }
  SYS_ARCH_UNPROTECT(lev);
  UNLOCK_TCPIP_CORE();

  done_socket(sock);
  return ret;
}
#else /* LWIP_EXT_POLL_SUPPORT */
/** Options for the lwip_pollscan function. */
enum lwip_pollscan_opts
{
  /** Clear revents in each struct pollfd. */
  LWIP_POLLSCAN_CLEAR = 1,

  /** Increment select_waiting in each struct lwip_sock. */
  LWIP_POLLSCAN_INC_WAIT = 2,

  /** Decrement select_waiting in each struct lwip_sock. */
  LWIP_POLLSCAN_DEC_WAIT = 4
};

/**
 * Update revents in each struct pollfd.
 * Optionally update select_waiting in struct lwip_sock.
 *
 * @param fds          array of structures to update
 * @param nfds         number of structures in fds
 * @param opts         what to update and how
 * @return number of structures that have revents != 0
 */
static int
lwip_pollscan(struct pollfd *fds, nfds_t nfds, enum lwip_pollscan_opts opts)
{
  int nready = 0;
  nfds_t fdi;
  struct lwip_sock *sock;
  SYS_ARCH_DECL_PROTECT(lev);

  /* Go through each struct pollfd in the array. */
  for (fdi = 0; fdi < nfds; fdi++) {
    if ((opts & LWIP_POLLSCAN_CLEAR) != 0) {
      fds[fdi].revents = 0;
    }

    /* Negative fd means the caller wants us to ignore this struct.
       POLLNVAL means we already detected that the fd is invalid;
       if another thread has since opened a new socket with that fd,
       we must not use that socket. */
    if (fds[fdi].fd >= 0 && (fds[fdi].revents & POLLNVAL) == 0) {
      /* First get the socket's status (protected)... */
      SYS_ARCH_PROTECT(lev);
      sock = tryget_socket_unconn_locked(fds[fdi].fd);
      if (sock != NULL) {
        void* lastdata = sock->lastdata.pbuf;
        s16_t rcvevent = sock->rcvevent;
        u16_t sendevent = sock->sendevent;
        u16_t errevent = sock->errevent;

        if ((opts & LWIP_POLLSCAN_INC_WAIT) != 0) {
          sock->select_waiting++;
          if (sock->select_waiting == 0) {
            /* overflow - too many threads waiting */
            sock->select_waiting--;
            nready = -1;
            SYS_ARCH_UNPROTECT(lev);
            done_socket(sock);
            break;
          }
        } else if ((opts & LWIP_POLLSCAN_DEC_WAIT) != 0) {
          /* for now, handle select_waiting==0... */
          LWIP_ASSERT("sock->select_waiting > 0", sock->select_waiting > 0);
          if (sock->select_waiting > 0) {
            sock->select_waiting--;
          }
        }
        SYS_ARCH_UNPROTECT(lev);
        done_socket(sock);

        /* ... then examine it: */
        /* See if netconn of this socket is ready for read */
        if ((fds[fdi].events & POLLIN) != 0 && ((lastdata != NULL) || (rcvevent > 0))) {
          fds[fdi].revents |= POLLIN;
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_pollscan: fd=%d ready for reading\n", fds[fdi].fd));
        }
        /* See if netconn of this socket is ready for write */
        if ((fds[fdi].events & POLLOUT) != 0 && (sendevent != 0)) {
          fds[fdi].revents |= POLLOUT;
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_pollscan: fd=%d ready for writing\n", fds[fdi].fd));
        }
        /* See if netconn of this socket had an error */
        if (errevent != 0) {
          /* POLLERR is output only. */
          fds[fdi].revents |= POLLERR;
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_pollscan: fd=%d ready for exception\n", fds[fdi].fd));
        }
      } else {
        /* Not a valid socket */
        SYS_ARCH_UNPROTECT(lev);
        /* POLLNVAL is output only. */
        fds[fdi].revents |= POLLNVAL;
        return -1;
      }
    }

    /* Will return the number of structures that have events,
       not the number of events. */
    if (fds[fdi].revents != 0) {
      nready++;
    }
  }

  LWIP_ASSERT("nready >= 0", nready >= 0);
  return nready;
}

#if LWIP_NETCONN_FULLDUPLEX
/* Mark all sockets as used.
 *
 * All sockets are marked (and later unmarked), whether they are open or not.
 * This is OK as lwip_pollscan aborts select when non-open sockets are found.
 */
static void
lwip_poll_inc_sockets_used(struct pollfd *fds, nfds_t nfds)
{
  nfds_t fdi;

  if(fds) {
    /* Go through each struct pollfd in the array. */
    for (fdi = 0; fdi < nfds; fdi++) {
      /* Increase the reference counter */
      tryget_socket_unconn(fds[fdi].fd);
    }
  }
}

/* Let go all sockets that were marked as used when starting poll */
static void
lwip_poll_dec_sockets_used(struct pollfd *fds, nfds_t nfds)
{
  nfds_t fdi;

  if(fds) {
    /* Go through each struct pollfd in the array. */
    for (fdi = 0; fdi < nfds; fdi++) {
      struct lwip_sock *sock = tryget_socket_unconn_nouse(fds[fdi].fd);
      if (sock != NULL) {
        done_socket(sock);
      }
    }
  }
}
#else /* LWIP_NETCONN_FULLDUPLEX */
#define lwip_poll_inc_sockets_used(fds, nfds)
#define lwip_poll_dec_sockets_used(fds, nfds)
#endif /* LWIP_NETCONN_FULLDUPLEX */

int
lwip_poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
  u32_t waitres = 0;
  int nready;
  u32_t msectimeout;
#if LWIP_NETCONN_SEM_PER_THREAD
  int waited = 0;
#endif

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_poll(%p, %d, %d)\n",
                  (void*)fds, (int)nfds, timeout));
  LWIP_ERROR("lwip_poll: invalid fds", ((fds != NULL && nfds > 0) || (fds == NULL && nfds == 0)),
             set_errno(EINVAL); return -1;);

  lwip_poll_inc_sockets_used(fds, nfds);

  /* Go through each struct pollfd to count number of structures
     which currently match */
  nready = lwip_pollscan(fds, nfds, LWIP_POLLSCAN_CLEAR);

  if (nready < 0) {
    lwip_poll_dec_sockets_used(fds, nfds);
    return -1;
  }

  /* If we don't have any current events, then suspend if we are supposed to */
  if (!nready) {
    API_SELECT_CB_VAR_DECLARE(select_cb);

    if (timeout == 0) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_poll: no timeout, returning 0\n"));
      goto return_success;
    }
    API_SELECT_CB_VAR_ALLOC(select_cb, set_errno(EAGAIN); lwip_poll_dec_sockets_used(fds, nfds); return -1);
    memset(&API_SELECT_CB_VAR_REF(select_cb), 0, sizeof(struct lwip_select_cb));

    /* None ready: add our semaphore to list:
       We don't actually need any dynamic memory. Our entry on the
       list is only valid while we are in this function, so it's ok
       to use local variables. */

    API_SELECT_CB_VAR_REF(select_cb).poll_fds = fds;
    API_SELECT_CB_VAR_REF(select_cb).poll_nfds = nfds;
#if LWIP_NETCONN_SEM_PER_THREAD
    API_SELECT_CB_VAR_REF(select_cb).sem = LWIP_NETCONN_THREAD_SEM_GET();
#else /* LWIP_NETCONN_SEM_PER_THREAD */
    if (sys_sem_new(&API_SELECT_CB_VAR_REF(select_cb).sem, 0) != ERR_OK) {
      /* failed to create semaphore */
      set_errno(EAGAIN);
      lwip_poll_dec_sockets_used(fds, nfds);
      API_SELECT_CB_VAR_FREE(select_cb);
      return -1;
    }
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

    lwip_link_select_cb(&API_SELECT_CB_VAR_REF(select_cb));

    /* Increase select_waiting for each socket we are interested in.
       Also, check for events again: there could have been events between
       the last scan (without us on the list) and putting us on the list! */
    nready = lwip_pollscan(fds, nfds, LWIP_POLLSCAN_INC_WAIT);

    if (!nready) {
      /* Still none ready, just wait to be woken */
      if (timeout < 0) {
        /* Wait forever */
        msectimeout = 0;
      } else {
        /* timeout == 0 would have been handled earlier. */
        LWIP_ASSERT("timeout > 0", timeout > 0);
        msectimeout = timeout;
      }
      waitres = sys_arch_sem_wait(SELECT_SEM_PTR(API_SELECT_CB_VAR_REF(select_cb).sem), msectimeout);
#if LWIP_NETCONN_SEM_PER_THREAD
      waited = 1;
#endif
    }

    /* Decrease select_waiting for each socket we are interested in,
       and check which events occurred while we waited. */
    nready = lwip_pollscan(fds, nfds, LWIP_POLLSCAN_DEC_WAIT);

    lwip_unlink_select_cb(&API_SELECT_CB_VAR_REF(select_cb));

#if LWIP_NETCONN_SEM_PER_THREAD
    if (select_cb.sem_signalled && (!waited || (waitres == SYS_ARCH_TIMEOUT))) {
      /* don't leave the thread-local semaphore signalled */
      sys_arch_sem_wait(API_SELECT_CB_VAR_REF(select_cb).sem, 1);
    }
#else /* LWIP_NETCONN_SEM_PER_THREAD */
    sys_sem_free(&API_SELECT_CB_VAR_REF(select_cb).sem);
#endif /* LWIP_NETCONN_SEM_PER_THREAD */
    API_SELECT_CB_VAR_FREE(select_cb);

    if (nready < 0) {
      /* This happens when a socket got closed while waiting */
      lwip_poll_dec_sockets_used(fds, nfds);
      return -1;
    }

    if (waitres == SYS_ARCH_TIMEOUT) {
      /* Timeout */
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_poll: timeout expired\n"));
      goto return_success;
    }
  }

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_poll: nready=%d\n", nready));
return_success:
  lwip_poll_dec_sockets_used(fds, nfds);
  set_errno(0);
  return nready;
}

/**
 * Check whether event_callback should wake up a thread waiting in
 * lwip_poll.
 */
static int
lwip_poll_should_wake(const struct lwip_select_cb *scb, int fd, int has_recvevent, int has_sendevent, int has_errevent)
{
  nfds_t fdi;
  for (fdi = 0; fdi < scb->poll_nfds; fdi++) {
    const struct pollfd *pollfd = &scb->poll_fds[fdi];
    if (pollfd->fd == fd) {
      /* Do not update pollfd->revents right here;
         that would be a data race because lwip_pollscan
         accesses revents without protecting. */
      if (has_recvevent && (pollfd->events & POLLIN) != 0) {
        return 1;
      }
      if (has_sendevent && (pollfd->events & POLLOUT) != 0) {
        return 1;
      }
      if (has_errevent) {
        /* POLLERR is output only. */
        return 1;
      }
    }
  }
  return 0;
}
#endif /* LWIP_EXT_POLL_SUPPORT */
#endif /* LWIP_SOCKET_POLL */

#if LWIP_SOCKET_SELECT || LWIP_SOCKET_POLL
/**
 * Callback registered in the netconn layer for each socket-netconn.
 * Processes recvevent (data available) and wakes up tasks waiting for select.
 *
 * @note for LWIP_TCPIP_CORE_LOCKING any caller of this function
 * must have the core lock held when signaling the following events
 * as they might cause select_list_cb to be checked:
 *   NETCONN_EVT_RCVPLUS
 *   NETCONN_EVT_SENDPLUS
 *   NETCONN_EVT_ERROR
 * This requirement will be asserted in select_check_waiters()
 */
static void
event_callback(struct netconn *conn, enum netconn_evt evt, u16_t len)
{
  int s, check_waiters;
  struct lwip_sock *sock;
  SYS_ARCH_DECL_PROTECT(lev);
#if LWIP_EXT_POLL_SUPPORT
  unsigned int mask = 0;
#endif
  LWIP_UNUSED_ARG(len);

  /* Get socket */
  if (conn) {
    s = conn->socket;
    if (s < 0) {
      /* Data comes in right away after an accept, even though
       * the server task might not have created a new socket yet.
       * Just count down (or up) if that's the case and we
       * will use the data later. Note that only receive events
       * can happen before the new socket is set up. */
      SYS_ARCH_PROTECT(lev);
      if (conn->socket < 0) {
        if (evt == NETCONN_EVT_RCVPLUS) {
          /* conn->socket is -1 on initialization
             lwip_accept adjusts sock->recvevent if conn->socket < -1 */
          conn->socket--;
        }
        SYS_ARCH_UNPROTECT(lev);
        return;
      }
      s = conn->socket;
      SYS_ARCH_UNPROTECT(lev);
    }

    sock = tryget_socket_unconn_nouse(s);
    if (!sock) {
      return;
    }
  } else {
    return;
  }

  check_waiters = 1;
  SYS_ARCH_PROTECT(lev);
  /* Set event as required */
  switch (evt) {
    case NETCONN_EVT_RCVPLUS:
      sock->rcvevent++;
      if (sock->rcvevent > 1) {
        check_waiters = 0;
      }
      break;
    case NETCONN_EVT_RCVMINUS:
      sock->rcvevent--;
      check_waiters = 0;
      break;
    case NETCONN_EVT_SENDPLUS:
      if (sock->sendevent) {
        check_waiters = 0;
      }
      sock->sendevent = 1;
      break;
    case NETCONN_EVT_SENDMINUS:
      sock->sendevent = 0;
      check_waiters = 0;
      break;
    case NETCONN_EVT_ERROR:
      sock->errevent = 1;
      break;
    default:
      LWIP_ASSERT("unknown event", 0);
      break;
  }

#if LWIP_EXT_POLL_SUPPORT
  /* check shutdown mask firstly */
  switch (conn->shutdown) {
    case SHUTDOWN_MASK:
      mask |= POLLHUP | POLLIN | POLLRDNORM | POLLRDHUP | POLLOUT | POLLWRNORM;
      break;
    case RCV_SHUTDOWN:
      mask |= POLLIN | POLLRDNORM | POLLRDHUP;
      break;
    case SND_SHUTDOWN:
      mask |= POLLOUT | POLLWRNORM;
      break;
    default:
      mask = 0;
  }

  /* wakeup poll task if any data readable, for both listen socket and connected socket */
  if (sock->rcvevent > 0) {
    mask |= (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND);
  }

  if (sock->sendevent != 0) {
    mask |= (POLLOUT | POLLWRNORM | POLLWRBAND);
  }

  if (sock->errevent != 0) {
    mask |= POLLERR;
  }

#if LWIP_LITEOS_COMPAT
  /* try to wakeup the pending task if any */
  if (mask && !LOS_ListEmpty(&(sock->wq.poll_queue))) {
    wake_up_interruptible_poll(&sock->wq, mask);
  }
#endif /* LWIP_LITEOS_COMPAT */
#endif

  if (sock->select_waiting && check_waiters) {
    /* Save which events are active */
    int has_recvevent, has_sendevent, has_errevent;
    has_recvevent = sock->rcvevent > 0;
    has_sendevent = sock->sendevent != 0;
    has_errevent = sock->errevent != 0;
    SYS_ARCH_UNPROTECT(lev);
    /* Check any select calls waiting on this socket */
    select_check_waiters(s, has_recvevent, has_sendevent, has_errevent);
  } else {
    SYS_ARCH_UNPROTECT(lev);
  }
}

/**
 * Check if any select waiters are waiting on this socket and its events
 *
 * @note on synchronization of select_cb_list:
 * LWIP_TCPIP_CORE_LOCKING: the select_cb_list must only be accessed while holding
 * the core lock. We do a single pass through the list and signal any waiters.
 * Core lock should already be held when calling here!!!!

 * !LWIP_TCPIP_CORE_LOCKING: we use SYS_ARCH_PROTECT but unlock on each iteration
 * of the loop, thus creating a possibility where a thread could modify the
 * select_cb_list during our UNPROTECT/PROTECT. We use a generational counter to
 * detect this change and restart the list walk. The list is expected to be small
 */
static void select_check_waiters(int s, int has_recvevent, int has_sendevent, int has_errevent)
{
  struct lwip_select_cb *scb;
#if !LWIP_TCPIP_CORE_LOCKING
  int last_select_cb_ctr;
  SYS_ARCH_DECL_PROTECT(lev);
#endif /* !LWIP_TCPIP_CORE_LOCKING */

  LWIP_ASSERT_CORE_LOCKED();

#if !LWIP_TCPIP_CORE_LOCKING
  SYS_ARCH_PROTECT(lev);
again:
  /* remember the state of select_cb_list to detect changes */
  last_select_cb_ctr = select_cb_ctr;
#endif /* !LWIP_TCPIP_CORE_LOCKING */
  for (scb = select_cb_list; scb != NULL; scb = scb->next) {
    if (scb->sem_signalled == 0) {
      /* semaphore not signalled yet */
      int do_signal = 0;
#if LWIP_SOCKET_POLL && !LWIP_EXT_POLL_SUPPORT
      if (scb->poll_fds != NULL) {
        do_signal = lwip_poll_should_wake(scb, s, has_recvevent, has_sendevent, has_errevent);
      }
#endif /* LWIP_SOCKET_POLL */
#if LWIP_SOCKET_SELECT && LWIP_SOCKET_POLL && !LWIP_EXT_POLL_SUPPORT
      else
#endif /* LWIP_SOCKET_SELECT && LWIP_SOCKET_POLL */
#if LWIP_SOCKET_SELECT
      {
        /* Test this select call for our socket */
        if (has_recvevent) {
          if (scb->readset && FD_ISSET(s, scb->readset)) {
            do_signal = 1;
          }
        }
        if (has_sendevent) {
          if (!do_signal && scb->writeset && FD_ISSET(s, scb->writeset)) {
            do_signal = 1;
          }
        }
        if (has_errevent) {
          if (!do_signal && scb->exceptset && FD_ISSET(s, scb->exceptset)) {
            do_signal = 1;
          }
        }
      }
#endif /* LWIP_SOCKET_SELECT */
      if (do_signal) {
        scb->sem_signalled = 1;
        /* For !LWIP_TCPIP_CORE_LOCKING, we don't call SYS_ARCH_UNPROTECT() before signaling
           the semaphore, as this might lead to the select thread taking itself off the list,
           invalidating the semaphore. */
        sys_sem_signal(SELECT_SEM_PTR(scb->sem));
      }
    }
#if LWIP_TCPIP_CORE_LOCKING
  }
#else
    /* unlock interrupts with each step */
    SYS_ARCH_UNPROTECT(lev);
    /* this makes sure interrupt protection time is short */
    SYS_ARCH_PROTECT(lev);
    if (last_select_cb_ctr != select_cb_ctr) {
      /* someone has changed select_cb_list, restart at the beginning */
      goto again;
    }
    /* remember the state of select_cb_list to detect changes */
    last_select_cb_ctr = select_cb_ctr;
  }
  SYS_ARCH_UNPROTECT(lev);
#endif
}
#endif /* LWIP_SOCKET_SELECT || LWIP_SOCKET_POLL */

/**
 * Close one end of a full-duplex connection.
 */
int
lwip_shutdown(int s, int how)
{
  struct lwip_sock *sock;
  err_t err;
  u8_t shut_rx = 0, shut_tx = 0;

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_shutdown(%d, how=%d)\n", s, how));

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

  if (sock->conn != NULL) {
    if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) != NETCONN_TCP) {
      sock_set_errno(sock, ENOTCONN);
      done_socket(sock);
      return -1;
    }
  } else {
    sock_set_errno(sock, ENOTCONN);
    done_socket(sock);
    return -1;
  }

  if (how == SHUT_RD) {
    shut_rx = 1;
  } else if (how == SHUT_WR) {
    shut_tx = 1;
  } else if (how == SHUT_RDWR) {
    shut_rx = 1;
    shut_tx = 1;
  } else {
    sock_set_errno(sock, EINVAL);
    done_socket(sock);
    return -1;
  }
  err = netconn_shutdown(sock->conn, shut_rx, shut_tx);
  if (err != ERR_OK) {
    sock_set_errno(sock, err_to_errno(err));
  }
  done_socket(sock);
  return (err == ERR_OK ? 0 : -1);
}

static int
lwip_getaddrname(int s, struct sockaddr *name, socklen_t *namelen, u8_t local)
{
  struct lwip_sock *sock;
  union sockaddr_aligned saddr;
  ip_addr_t naddr;
  u16_t port;
  err_t err;
  socklen_t sa_len;

#if PF_PKT_SUPPORT
  struct sockaddr_ll addr_sin;
  struct pf_pkt_ll ll;
  socklen_t outlen;
#endif

  LWIP_ERROR("lwip_getaddrname: invalid arguments", ((name != NULL) && (namelen != NULL)),
             set_errno(EINVAL); return -1);

  sock = get_socket(s);
  if (!sock) {
    return -1;
  }

#if PF_PKT_SUPPORT
  if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) == NETCONN_PKT_RAW) {
    err = netconn_getaddr_pfpkt(sock->conn, &ll, local);
    if (err != ERR_OK) {
      sock_set_errno(sock, err_to_errno(err));
      done_socket(sock);
      return -1;
    }

    (void)memset_s(&addr_sin, sizeof(struct sockaddr_ll), 0, sizeof(struct sockaddr_ll));
    addr_sin.sll_family = PF_PACKET;
    addr_sin.sll_protocol = ll.sll_protocol;
    addr_sin.sll_pkttype = 0;
    addr_sin.sll_ifindex = ll.if_idx;
    addr_sin.sll_hatype = ll.sll_hatype;
    addr_sin.sll_halen = ll.sll_halen;

    if ((ll.sll_halen > 0) && (memcpy_s(addr_sin.sll_addr, addr_sin.sll_halen, ll.sll_addr, ll.sll_halen) != EOK)) {
      sock_set_errno(sock, err_to_errno(ERR_MEM));
      done_socket(sock);
      return -1;
    }

    outlen = sizeof(struct sockaddr_ll);
    if (outlen > *namelen) {
      outlen = *namelen;
    }
    if (memcpy_s(name, *namelen, &addr_sin, outlen) != EOK) {
      sock_set_errno(sock, err_to_errno(ERR_MEM));
      done_socket(sock);
      return -1;
    }
    *namelen = outlen;
    done_socket(sock);
    return 0;
  }
#endif /* PF_PKT_SUPPORT */

  /* get the IP address and port */
  err = netconn_getaddr(sock->conn, &naddr, &port, local);
  if (err != ERR_OK) {
    sock_set_errno(sock, err_to_errno(err));
    done_socket(sock);
    return -1;
  }

#if LWIP_IPV4 && LWIP_IPV6
  /* Dual-stack: Map IPv4 addresses to IPv4 mapped IPv6 */
  if (NETCONNTYPE_ISIPV6(NETCONN_TYPE(sock->conn)) &&
      IP_IS_V4_VAL(naddr)) {
    ip4_2_ipv4_mapped_ipv6(ip_2_ip6(&naddr), ip_2_ip4(&naddr));
    IP_SET_TYPE_VAL(naddr, IPADDR_TYPE_V6);
  }
#endif /* LWIP_IPV4 && LWIP_IPV6 */

  IPADDR_PORT_TO_SOCKADDR(&saddr, &naddr, port);

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getaddrname(%d, addr=", s));
  ip_addr_debug_print_val(SOCKETS_DEBUG, naddr);
  LWIP_DEBUGF(SOCKETS_DEBUG, (" port=%"U16_F")\n", port));
  if (IP_IS_V4_VAL(naddr)) {
    sa_len = sizeof(struct sockaddr_in);
  } else {
    sa_len = sizeof(struct sockaddr_in6);
  }

  if (*namelen > sa_len) {
    *namelen = sa_len;
  }
  MEMCPY(name, &saddr, *namelen);

  sock_set_errno(sock, 0);
  done_socket(sock);
  return 0;
}

int
lwip_getpeername(int s, struct sockaddr *name, socklen_t *namelen)
{
  return lwip_getaddrname(s, name, namelen, 0);
}

int
lwip_getsockname(int s, struct sockaddr *name, socklen_t *namelen)
{
  return lwip_getaddrname(s, name, namelen, 1);
}

int
lwip_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)
{
  int err;
  struct lwip_sock *sock = get_socket(s);
#if !LWIP_TCPIP_CORE_LOCKING
  err_t cberr;
  LWIP_SETGETSOCKOPT_DATA_VAR_DECLARE(data);
#endif /* !LWIP_TCPIP_CORE_LOCKING */

  if (!sock) {
    return -1;
  }

  if ((NULL == optval) || (NULL == optlen)) {
    sock_set_errno(sock, EFAULT);
    done_socket(sock);
    return -1;
  }

#if PF_PKT_SUPPORT
  VALIDATE_LEVEL_PF_PACKET(sock, level) {
    sock_set_errno(sock, EINVAL);
    done_socket(sock);
    return -1;
  }
#endif /* PF_PKT_SUPPORT  */

#if LWIP_TCPIP_CORE_LOCKING
  /* core-locking can just call the -impl function */
  LOCK_TCPIP_CORE();
  err = lwip_getsockopt_impl(s, level, optname, optval, optlen);
  UNLOCK_TCPIP_CORE();

#else /* LWIP_TCPIP_CORE_LOCKING */

#if LWIP_MPU_COMPATIBLE
  /* MPU_COMPATIBLE copies the optval data, so check for max size here */
  if (*optlen > LWIP_SETGETSOCKOPT_MAXOPTLEN) {
    sock_set_errno(sock, ENOBUFS);
    done_socket(sock);
    return -1;
  }
#endif /* LWIP_MPU_COMPATIBLE */

  LWIP_SETGETSOCKOPT_DATA_VAR_ALLOC(data, sock);
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).s = s;
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).level = level;
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).optname = optname;
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).optlen = *optlen;
#if !LWIP_MPU_COMPATIBLE
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).optval.p = optval;
#endif /* !LWIP_MPU_COMPATIBLE */
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).err = 0;
#if LWIP_NETCONN_SEM_PER_THREAD
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).completed_sem = LWIP_NETCONN_THREAD_SEM_GET();
#else
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).completed_sem = &sock->conn->op_completed;
#endif
  cberr = tcpip_callback(lwip_getsockopt_callback, &LWIP_SETGETSOCKOPT_DATA_VAR_REF(data));
  if (cberr != ERR_OK) {
    LWIP_SETGETSOCKOPT_DATA_VAR_FREE(data);
    sock_set_errno(sock, err_to_errno(cberr));
    done_socket(sock);
    return -1;
  }
  sys_arch_sem_wait((sys_sem_t *)(LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).completed_sem), 0);

  /* write back optlen and optval */
  *optlen = LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).optlen;
#if LWIP_MPU_COMPATIBLE
  MEMCPY(optval, LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).optval,
         LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).optlen);
#endif /* LWIP_MPU_COMPATIBLE */

  /* maybe lwip_getsockopt_internal has changed err */
  err = LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).err;
  LWIP_SETGETSOCKOPT_DATA_VAR_FREE(data);
#endif /* LWIP_TCPIP_CORE_LOCKING */
  if (err != ERR_OK) {
    sock_set_errno(sock, err);
  }
  done_socket(sock);
  return err ? -1 : 0;
}

#if !LWIP_TCPIP_CORE_LOCKING
/** lwip_getsockopt_callback: only used without CORE_LOCKING
 * to get into the tcpip_thread
 */
static void
lwip_getsockopt_callback(void *arg)
{
  struct lwip_setgetsockopt_data *data;
  LWIP_ASSERT("arg != NULL", arg != NULL);
  data = (struct lwip_setgetsockopt_data *)arg;

  data->err = lwip_getsockopt_impl(data->s, data->level, data->optname,
#if LWIP_MPU_COMPATIBLE
                                   data->optval,
#else /* LWIP_MPU_COMPATIBLE */
                                   data->optval.p,
#endif /* LWIP_MPU_COMPATIBLE */
                                   &data->optlen);

  sys_sem_signal((sys_sem_t *)(data->completed_sem));
}
#endif  /* LWIP_TCPIP_CORE_LOCKING */

static int
lwip_sockopt_to_ipopt(int optname)
{
  /* Map SO_* values to our internal SOF_* values
   * We should not rely on #defines in socket.h
   * being in sync with ip.h.
   */
  switch (optname) {
  case SO_BROADCAST:
    return SOF_BROADCAST;
  case SO_KEEPALIVE:
    return SOF_KEEPALIVE;
  case SO_REUSEADDR:
    return SOF_REUSEADDR;
  case SO_DONTROUTE:
    return SOF_DONTROUTE;
  default:
    LWIP_ASSERT("Unknown socket option", 0);
    return 0;
  }
}

/** lwip_sockopt_getval: update optval and optlen.follow linux getsockopt behavior!
 * warning: this function is only suit for getting u8_t type socket options.
 */
static inline void
lwip_sockopt_getval(void *outval, socklen_t *outlen, const u8_t *inval)
{
  u32_t len = (*outlen < sizeof(u32_t)) ? sizeof(u8_t) : sizeof(u32_t);
  u32_t val = *(const u8_t *)inval;
  (void)memcpy_s(outval, len, &val, len);
  *outlen = len;
}

/** lwip_getsockopt_impl: the actual implementation of getsockopt:
 * same argument as lwip_getsockopt, either called directly or through callback
 */
static int
lwip_getsockopt_impl(int s, int level, int optname, void *optval, socklen_t *optlen)
{
  int err = 0;
  struct lwip_sock *sock = tryget_socket(s);
  if (!sock) {
    return EBADF;
  }

#ifdef LWIP_HOOK_SOCKETS_GETSOCKOPT
  if (LWIP_HOOK_SOCKETS_GETSOCKOPT(s, sock, level, optname, optval, optlen, &err)) {
    return err;
  }
#endif

  switch (level) {
    /* Level: SOL_SOCKET */
    case SOL_SOCKET:
#if PF_PKT_SUPPORT
    case SOL_PACKET:
      VALIDATE_GET_PF_PKT_OPTNAME_RET(s, sock, level, optname);
#endif /* PF_PKT_SUPPORT  */

      VALIDATE_GET_RAW_OPTNAME_RET(sock, optname);
      switch (optname) {
    case SO_BINDTODEVICE:
      if ((*optlen < NETIF_NAMESIZE) || (sock->conn == NULL) || (sock->conn->pcb.ip == NULL)) {
        done_socket(sock);
        return EINVAL;
      }
      if (sock->conn->pcb.ip->netif_idx != NETIF_NO_INDEX) {
        struct netif *netif = netif_find_by_ifindex(sock->conn->pcb.ip->netif_idx);
        if (netif == NULL) {
          *(optlen) = 0;
          done_socket(sock);
          return ENODEV;
        } else {
          int len = snprintf_s((char *)optval, NETIF_NAMESIZE, NETIF_NAMESIZE - 1, "%s%hhu", netif->name, netif->num);
          if (len <= 0) {
            *(optlen) = 0;
            done_socket(sock);
            return ENOMEM;
          }
          *(optlen) = 1 + (socklen_t)len;
        }
      } else {
        *(optlen) = 0;
        done_socket(sock);
        return EINVAL;
      }
      break;
#if LWIP_TCP
        case SO_ACCEPTCONN:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, *optlen, int);
          if ((sock->conn != NULL) && NETCONNTYPE_GROUP(sock->conn->type) != NETCONN_TCP) {
            done_socket(sock);
            return ENOPROTOOPT;
          }
          if ((sock->conn->pcb.tcp != NULL) && (sock->conn->pcb.tcp->state == LISTEN)) {
            *(int *)optval = 1;
          } else {
            *(int *)optval = 0;
          }
          break;
#endif /* LWIP_TCP */

        /* The option flags */
        case SO_BROADCAST:
#if LWIP_SO_DONTROUTE
        case SO_DONTROUTE:
#endif /* LWIP_SO_DONTROUTE */
        case SO_KEEPALIVE:
#if SO_REUSE
        case SO_REUSEADDR:
#endif /* SO_REUSE */
          if (sock->conn == NULL) {
            done_socket(sock);
            return EINVAL;
          }
          if ((optname == SO_BROADCAST) &&
              (NETCONNTYPE_GROUP(sock->conn->type) == NETCONN_TCP)) {
            done_socket(sock);
            return ENOPROTOOPT;
          }

          optname = lwip_sockopt_to_ipopt(optname);

          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, *optlen, int);
          if (ip_get_option(sock->conn->pcb.ip, optname)) {
            *(int *)optval = 1;
          } else {
            *(int *)optval = 0;
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, SOL_SOCKET, optname=0x%x, ..) = %s\n",
                                      s, optname, (*(int *)optval ? "on" : "off")));
          break;

        case SO_TYPE:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, *optlen, int);
          switch (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn))) {
#if PF_PKT_SUPPORT
            case NETCONN_PKT_RAW:
#endif
            case NETCONN_RAW:
              *(int *)optval = SOCK_RAW;
              break;
            case NETCONN_TCP:
              *(int *)optval = SOCK_STREAM;
              break;
            case NETCONN_UDP:
              *(int *)optval = SOCK_DGRAM;
              break;
            default: /* unrecognized socket type */
              *(int *)optval = NETCONN_TYPE(sock->conn);
#if PF_PKT_SUPPORT
              LWIP_DEBUGF(SOCKETS_DEBUG,
                          ("lwip_getsockopt(%d, %s, SO_TYPE): unrecognized socket type %d\n",
                          s, (NETCONN_PKT_RAW != NETCONNTYPE_GROUP(sock->conn->type)) ? "SOL_SOCKET" : "SOL_PACKET",
                          *(int *)optval));
#else
              LWIP_DEBUGF(SOCKETS_DEBUG,
                          ("lwip_getsockopt(%d, SOL_SOCKET, SO_TYPE): unrecognized socket type %d\n",
                           s, *(int *)optval));
#endif
          }  /* switch (NETCONN_TYPE(sock->conn)) */
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, SOL_SOCKET, SO_TYPE) = %d\n",
                                      s, *(int *)optval));
          break;

        case SO_ERROR:
          LWIP_SOCKOPT_CHECK_OPTLEN(sock, *optlen, int);
          /* only overwrite ERR_OK or temporary errors */
          if (((atomic_read(&sock->err) == 0) || (atomic_read(&sock->err) == EINPROGRESS)) && (sock->conn != NULL)) {
            sock_set_errno(sock, err_to_errno(sock->conn->last_err));
          }
          *(int *)optval = (atomic_read(&sock->err) == 0xFF ? (int)-1 : (int)atomic_read(&sock->err));
          (void)atomic_set(&sock->err, 0);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, SOL_SOCKET, SO_ERROR) = %d\n",
                                      s, *(int *)optval));
          break;
#if LWIP_TCP && LWIP_SO_SNDBUF
        case SO_SNDBUF:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, *optlen, int, NETCONN_TCP);
          *(u32_t *)optval = tcp_get_sendbufsize(sock->conn->pcb.tcp);
          break;
#endif /* LWIP_SO_SNDBUF */
#if LWIP_SO_SNDTIMEO
        case SO_SNDTIMEO:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, *optlen, LWIP_SO_SNDRCVTIMEO_OPTTYPE);
          LWIP_SO_SNDRCVTIMEO_SET(optval, netconn_get_sendtimeout(sock->conn));
          break;
#endif /* LWIP_SO_SNDTIMEO */
#if LWIP_SO_RCVTIMEO
        case SO_RCVTIMEO:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, *optlen, LWIP_SO_SNDRCVTIMEO_OPTTYPE);
          LWIP_SO_SNDRCVTIMEO_SET(optval, netconn_get_recvtimeout(sock->conn));
          break;
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
        case SO_RCVBUF:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, *optlen, int);
          *(int *)optval = netconn_get_recvbufsize(sock->conn);
          break;
#endif /* LWIP_SO_RCVBUF */
#if LWIP_SO_LINGER
        case SO_LINGER: {
          s16_t conn_linger;
          struct linger *linger = (struct linger *)optval;
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, *optlen, struct linger);
          conn_linger = sock->conn->linger;
          if (conn_linger >= 0) {
            linger->l_onoff = 1;
            linger->l_linger = (int)conn_linger;
          } else {
            linger->l_onoff = 0;
            linger->l_linger = 0;
          }
        }
        break;
#endif /* LWIP_SO_LINGER */
#if LWIP_UDP
        case SO_NO_CHECK:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, *optlen, int, NETCONN_UDP);
#if LWIP_UDPLITE
          if (udp_is_flag_set(sock->conn->pcb.udp, UDP_FLAGS_UDPLITE)) {
            /* this flag is only available for UDP, not for UDP lite */
            done_socket(sock);
            return ENOPROTOOPT;
          }
#endif /* LWIP_UDPLITE */
          *(int *)optval = udp_is_flag_set(sock->conn->pcb.udp, UDP_FLAGS_NOCHKSUM) ? 1 : 0;
          break;
#endif /* LWIP_UDP */
#if LWIP_SO_PRIORITY
        case SO_PRIORITY: {
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, *optlen, prio_t);

#if LWIP_UDP
          if (NETCONNTYPE_GROUP(sock->conn->type) == NETCONN_UDP) {
            *(prio_t *)optval = sock->conn->pcb.udp->priority;
          } else
#endif
#if LWIP_TCP
          if (NETCONNTYPE_GROUP(sock->conn->type) == NETCONN_TCP) {
            *(prio_t *)optval = sock->conn->pcb.tcp->priority;
          } else
#endif
#if LWIP_RAW
          if (NETCONNTYPE_GROUP(sock->conn->type) == NETCONN_RAW) {
            *(prio_t *)optval = sock->conn->pcb.raw->priority;
          } else
#endif
          {
            LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt Invalid socket type \n"));
            return ENOPROTOOPT;
          }
          break;
        }

#endif /* LWIP_SO_PRIORITY */

        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, SOL_SOCKET, UNIMPL: optname=0x%x, ..)\n",
                                      s, optname));
          err = ENOPROTOOPT;
          break;
      }  /* switch (optname) */
      break;

    /* Level: IPPROTO_IP */
    case IPPROTO_IP:
      switch (optname) {
        case IP_TTL:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, *optlen, u8_t);
          lwip_sockopt_getval(optval, optlen, &sock->conn->pcb.ip->ttl);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_TTL) = %u\n",
                                      s, *(u8_t *)optval));
          break;
        case IP_TOS:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, *optlen, u8_t);
          lwip_sockopt_getval(optval, optlen, &sock->conn->pcb.ip->tos);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_TOS) = %u\n",
                                      s, *(u8_t *)optval));
          break;
#if LWIP_RAW
        case IP_HDRINCL: {
            u8_t raw_flag;
            LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, *optlen, u8_t, NETCONN_RAW);
            if (NETCONNTYPE_ISIPV6(sock->conn->type)) {
              done_socket(sock);
              return ENOPROTOOPT;
            }

            raw_flag = raw_is_flag_set(sock->conn->pcb.raw, RAW_FLAGS_HDRINCL);
            lwip_sockopt_getval(optval, optlen, &raw_flag);
            LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IP_HDRINCL) = %u\n",
                        s, *(u8_t *)optval));
          }
          break;
#endif
#if LWIP_IPV4 && LWIP_MULTICAST_TX_OPTIONS && LWIP_UDP
        case IP_MULTICAST_TTL: {
            u8_t ttl;
            LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, *optlen, u8_t);
            if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) != NETCONN_UDP) {
              done_socket(sock);
              return ENOPROTOOPT;
            }
            ttl = udp_get_multicast_ttl(sock->conn->pcb.udp);
            lwip_sockopt_getval(optval, optlen, &ttl);
            LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_MULTICAST_TTL) = %u\n",
                                        s, *(u8_t *)optval));
          }
          break;
        case IP_MULTICAST_IF:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, *optlen, struct in_addr);
          if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) != NETCONN_UDP) {
            done_socket(sock);
            return ENOPROTOOPT;
          }
          inet_addr_from_ip4addr((struct in_addr *)optval, udp_get_multicast_netif_addr(sock->conn->pcb.udp));
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_MULTICAST_IF) = 0x%"X32_F"\n",
                                      s, *(u32_t *)optval));
          break;
        case IP_MULTICAST_LOOP:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, *optlen, u8_t, NETCONN_UDP);
          if ((sock->conn->pcb.udp->flags & UDP_FLAGS_MULTICAST_LOOP) != 0) {
            *(u8_t *)optval = 1;
          } else {
            *(u8_t *)optval = 0;
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, IP_MULTICAST_LOOP) = %d\n",
                                      s, *(int *)optval));
          break;
#endif /* LWIP_IPV4 && LWIP_MULTICAST_TX_OPTIONS && LWIP_UDP */
        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IP, UNIMPL: optname=0x%x, ..)\n",
                                      s, optname));
          err = ENOPROTOOPT;
          break;
      }  /* switch (optname) */
      break;

#if LWIP_TCP
    /* Level: IPPROTO_TCP */
    case IPPROTO_TCP:
#if LWIP_TCP_INFO
      if (optname == TCP_INFO) {
        LWIP_SOCKOPT_CHECK_OPTLEN(sock, *optlen, struct tcp_info);
        if (sock->conn == NULL) {
          return EINVAL;
        }

        if (NETCONNTYPE_GROUP(NETCONN_TYPE((sock)->conn)) != NETCONN_TCP) {
          return ENOPROTOOPT;
        }
        if ((sock->conn == NULL) || (sock->conn->pcb.tcp == NULL)) {
          ((struct tcp_info *)optval)->tcpi_state = TCP_CLOSE;
          break;
        }

        tcp_get_info(sock->conn->pcb.tcp, (struct tcp_info *)optval);
        break;
      }
#endif
      /* Special case: all IPPROTO_TCP option take an int */
      LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, *optlen, int, NETCONN_TCP);
      if ((sock->conn->pcb.tcp->state == LISTEN)
#if LWIP_TCP_MAXSEG
          && (optname != TCP_MAXSEG)
#endif /* LWIP_TCP_MAXSEG */
         ) {
        done_socket(sock);
        return EINVAL;
      }
      switch (optname) {
        case TCP_NODELAY:
          *(int *)optval = tcp_nagle_disabled(sock->conn->pcb.tcp);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_TCP, TCP_NODELAY) = %s\n",
                                      s, (*(int *)optval) ? "on" : "off") );
          break;
#if LWIP_TCP_KEEPALIVE
        case TCP_KEEPIDLE:
          *(int*)optval = (int)(sock->conn->pcb.tcp->keep_idle / MS_PER_SECOND);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_TCP, TCP_KEEPIDLE) = %d\n",
                      s, *(int *)optval));
          break;
        case TCP_KEEPINTVL:
          *(int*)optval = (int)(sock->conn->pcb.tcp->keep_intvl / MS_PER_SECOND);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_TCP, TCP_KEEPINTVL) = %d\n",
                      s, *(int *)optval));
          break;
        case TCP_KEEPCNT:
          *(int*)optval = (int)sock->conn->pcb.tcp->keep_cnt;
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_TCP, TCP_KEEPCNT) = %d\n",
                      s, *(int *)optval));
          break;
#endif /* LWIP_TCP_KEEPALIVE */
#if LWIP_SOCK_OPT_TCP_QUEUE_SEQ
        case TCP_QUEUE_SEQ:
          if (*(unsigned int*)optval == TCP_RECV_QUEUE) {
              *(unsigned int*)optval = sock->conn->pcb.tcp->rcv_nxt;
          } else if (*(int*)optval == TCP_SEND_QUEUE) {
              *(unsigned int*)optval = sock->conn->pcb.tcp->snd_nxt;
          } else {
            done_socket(sock);
            return EINVAL;
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_TCP, TCP_QUEUE_SEQ) = %d\n",
                      s, *(int *)optval));
          break;
#endif /* LWIP_SOCK_OPT_TCP_QUEUE_SEQ */
#if LWIP_TCP_MAXSEG
        case TCP_MAXSEG:
          *(int*)optval = (int)sock->conn->pcb.tcp->usr_mss;
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_TCP, TCP_MAXSEG) = %d\n",
                      s, *(int *)optval));
          break;
#endif /* LWIP_TCP_MAXSEG */

        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_TCP, UNIMPL: optname=0x%x, ..)\n",
                                      s, optname));
          err = ENOPROTOOPT;
          break;
      }  /* switch (optname) */
      break;
#endif /* LWIP_TCP */

#if LWIP_IPV6
    /* Level: IPPROTO_IPV6 */
    case IPPROTO_IPV6:
      switch (optname) {
        case IPV6_V6ONLY:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, *optlen, int);
          LWIP_SOCKOPT_CHECK_IPTYPE_V6(sock, sock->conn->type);

          *(int*)optval = (netconn_get_ipv6only(sock->conn) ? 1 : 0);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IPV6, IPV6_V6ONLY) = %d\n",
                                      s, *(int *)optval));
          break;
#if LWIP_IPV6 && LWIP_RAW
        case IPV6_CHECKSUM:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, *optlen, int, NETCONN_RAW);
          if (sock->conn->pcb.raw->chksum_reqd == 0) {
            *(int *)optval = -1;
          } else {
            *(int *)optval = sock->conn->pcb.raw->chksum_offset;
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IPV6, IPV6_CHECKSUM) = %d\n",
                      s, (*(int*)optval)));
          break;
#endif
#if LWIP_SOCK_OPT_IPV6_UNICAST_HOPS
        case IPV6_UNICAST_HOPS:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, *optlen, int);
          LWIP_SOCKOPT_CHECK_IPTYPE_V6(sock, sock->conn->type);

          *(int *)optval = (int)sock->conn->pcb.ip->ttl;
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IPV6, IPV6_UNICAST_HOPS) = %d\n",
                      s, *(int *)optval));
          break;
#endif
#if LWIP_MULTICAST_TX_OPTIONS
        case IPV6_MULTICAST_HOPS:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, *optlen, int, NETCONN_UDP);

          *(int *)optval = (int)udp_get_multicast_ttl(sock->conn->pcb.udp);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IPV6, IPV6_MULTICAST_HOPS) = %d\n",
                      s, *(int *)optval));
          break;
        case IPV6_MULTICAST_IF:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, *optlen, u8_t, NETCONN_UDP);
          *(u8_t*)optval = udp_get_multicast_netif_index(sock->conn->pcb.udp);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IPV6, IPV6_MULTICAST_IF) = %d\n",
                      s, *(u8_t *)optval));
          break;
        case IPV6_MULTICAST_LOOP:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, *optlen, u8_t, NETCONN_UDP);
          if ((sock->conn->pcb.udp->flags & UDP_FLAGS_MULTICAST_LOOP) != 0) {
            *(u8_t*)optval = 1;
          } else {
            *(u8_t*)optval = 0;
          }
          break;
#endif /* LWIP_MULTICAST_TX_OPTIONS */
        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_IPV6, UNIMPL: optname=0x%x, ..)\n",
                                      s, optname));
          err = ENOPROTOOPT;
          break;
      }  /* switch (optname) */
      break;
#endif /* LWIP_IPV6 */

#if LWIP_UDP && LWIP_UDPLITE
    /* Level: IPPROTO_UDPLITE */
    case IPPROTO_UDPLITE:
      /* Special case: all IPPROTO_UDPLITE option take an int */
      LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, *optlen, int);
      /* If this is no UDP lite socket, ignore any options. */
      if (!NETCONNTYPE_ISUDPLITE(NETCONN_TYPE(sock->conn))) {
        done_socket(sock);
        return ENOPROTOOPT;
      }
      switch (optname) {
        case UDPLITE_SEND_CSCOV:
          *(int *)optval = sock->conn->pcb.udp->chksum_len_tx;
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_UDPLITE, UDPLITE_SEND_CSCOV) = %d\n",
                                      s, (*(int *)optval)) );
          break;
        case UDPLITE_RECV_CSCOV:
          *(int *)optval = sock->conn->pcb.udp->chksum_len_rx;
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_UDPLITE, UDPLITE_RECV_CSCOV) = %d\n",
                                      s, (*(int *)optval)) );
          break;
        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_UDPLITE, UNIMPL: optname=0x%x, ..)\n",
                                      s, optname));
          err = ENOPROTOOPT;
          break;
      }  /* switch (optname) */
      break;
#endif /* LWIP_UDP */
    /* Level: IPPROTO_RAW */
    case IPPROTO_RAW:
      switch (optname) {
#if LWIP_IPV6 && LWIP_RAW
        case IPV6_CHECKSUM:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, *optlen, int, NETCONN_RAW);
          if (sock->conn->pcb.raw->chksum_reqd == 0) {
            *(int *)optval = -1;
          } else {
            *(int *)optval = sock->conn->pcb.raw->chksum_offset;
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_RAW, IPV6_CHECKSUM) = %d\n",
                                      s, (*(int *)optval)) );
          break;
#endif /* LWIP_IPV6 && LWIP_RAW */
        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_RAW, UNIMPL: optname=0x%x, ..)\n",
                                      s, optname));
          err = ENOPROTOOPT;
          break;
      }  /* switch (optname) */
      break;

      /* [rfc2292 section3.2][12-06-2018][Start] */
      /* Below feature is enabled  only if ipv6 raw socket is enabled */
#if LWIP_IPV6 && LWIP_RAW && LWIP_SOCK_OPT_ICMP6_FILTER
    case IPPROTO_ICMPV6:
      switch (optname) {
        case ICMP6_FILTER:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, *optlen, struct icmp6_filter, NETCONN_RAW);
          if (memcpy_s(optval, *optlen, &sock->conn->pcb.raw->icmp6_filter, sizeof(struct icmp6_filter)) != EOK) {
            err = ENOMEM;
          }
          break;
        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, IPPROTO_ICMPV6, UNIMPL: optname=0x%x, ..)\n",
                      s, optname));
          err = ENOPROTOOPT;
          break;
      }
      break;
#endif /* LWIP_IPV6 && LWIP_RAW */
    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_getsockopt(%d, level=0x%x, UNIMPL: optname=0x%x, ..)\n",
                                  s, level, optname));
      err = ENOPROTOOPT;
      break;
  } /* switch (level) */

  done_socket(sock);
  return err;
}

/**
    @page RFC-2553_RFC-3493 RFC-2553/3493

    @par Compliant Section
    Section 5.1 Unicast Hop Limit
    @par Behavior Description
    This option controls the hop limit used in outgoing unicast IPv6 packets.\n
    Stack allows the IPV6_UNICAST_HOPS value to be retrieved using the lwip_getsockopt() function.\n
    Currently, the stack does not allow the Set option for the same. The stack sets the error as ENOPROTOOPT.
    */
/**
    @page RFC-2553_RFC-3493 RFC-2553/3493

    @par Compliant Section
    Section 5.2 Sending and Receiving Multicast Packets

    @par Behavior Description
    IPV6_MULTICAST_HOPS - Set the hop limit to use for outgoing multicast packets.\n
    IPV6_JOIN_GROUP - Join a multicast group on a specified local interface. If the interface index is specified as 0,
    no interface will be selected
    and EADDRNOTAVAIL will be set. The maximum value of index supported is 254 as of now. \n
    IPV6_LEAVE_GROUP - Leave a multicast group on a specified interface.\n

    */
/**
    @page RFC-2553_RFC-3493 RFC-2553/3493
    @par Non-Compliant Section
    Section 6.1 Nodename-to-Address Translation

    @par Behavior Description
    getipnodebyname:\n
    The commonly used function gethostbyname() is inadequate for many applications due to the following reasons:\n
    - It provides no way for the caller to specify anything about the types of\n
    addresses desired (IPv4 only,   IPv6 only, IPv4-mapped IPv6 are OK)\n
    - Many   implementations of this function are not thread safe.\n
    The stack does not implement the function. RFC-2553 introduces the\n
    functions ,but RFC-3493 deprecates the same in favor of getaddrinfo() and getnameinfo().
    */
/**
    @page RFC-2553_RFC-3493 RFC-2553/3493
    @par Non-Compliant Section
    Section 6.2 Address-To-Nodename Translation
    @par Behavior Description
    getipnodebyaddr:\n
    As with getipnodebyname(), getipnodebyaddr() must be thread safe and is an alternative to
    gethostbyaddr() \n
    The stack does not implement the function. RFC-2553 introduces the
    functions, but RFC-3493 deprecates the same in favor of getaddrinfo() and getnameinfo().
    */
/**
   @page RFC-2553_RFC-3493 RFC-2553/3493

    @par Non-Compliant Section
    Section 6.3 Freeing memory for getipnodebyname and getipnodebyaddr
    @par Behavior Description
    freehostent:\n
    The function frees the memory dynamically allocated by getipnodebyname and getipnodebyaddr\n
    The stack does not implement the function. RFC-2553 introduces the\n
    functions, but RFC-3493 deprecates the same in favor of getaddrinfo() and getnameinfo().
    */

/**
    @page RFC-3493 RFC-3493
    @par RFC-3493 Compliance Information
    @par Compliant Section
    Section 5.3  IPV6_V6ONLY option for AF_INET6 Sockets
    @par Behavior Description
    This socket option restricts AF_INET6 sockets to IPv6 communications only.\n
    Stack allows this option for all sock types and is not restricted to TCP type.
    */

int
lwip_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
  int err = 0;
#if LWIP_SOCK_FILTER
  int detachfilteroverride = 0;
#endif
  struct lwip_sock *sock = get_socket(s);
#if !LWIP_TCPIP_CORE_LOCKING
  err_t cberr;
  LWIP_SETGETSOCKOPT_DATA_VAR_DECLARE(data);
#endif /* !LWIP_TCPIP_CORE_LOCKING */

  if (!sock) {
    return -1;
  }

#if LWIP_SOCK_FILTER
  if (optname == SO_DETACH_FILTER) {
    optval  = (void*)&detachfilteroverride;
  }
#endif

  if (NULL == optval) {
    sock_set_errno(sock, EFAULT);
    done_socket(sock);
    return -1;
  }

#if PF_PKT_SUPPORT
  VALIDATE_LEVEL_PF_PACKET(sock, level)
  {
    sock_set_errno(sock, EINVAL);
    done_socket(sock);
    return -1;
  }
#endif /* PF_PKT_SUPPORT  */

#if LWIP_TCPIP_CORE_LOCKING
  /* core-locking can just call the -impl function */
  LOCK_TCPIP_CORE();
  err = lwip_setsockopt_impl(s, level, optname, optval, optlen);
  UNLOCK_TCPIP_CORE();
#if LWIP_LOWPOWER
  tcpip_send_msg_na(LOW_NON_BLOCK);
#endif

#else /* LWIP_TCPIP_CORE_LOCKING */

#if LWIP_MPU_COMPATIBLE
  /* MPU_COMPATIBLE copies the optval data, so check for max size here */
  if (optlen > LWIP_SETGETSOCKOPT_MAXOPTLEN) {
    sock_set_errno(sock, ENOBUFS);
    done_socket(sock);
    return -1;
  }
#endif /* LWIP_MPU_COMPATIBLE */

  LWIP_SETGETSOCKOPT_DATA_VAR_ALLOC(data, sock);
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).s = s;
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).level = level;
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).optname = optname;
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).optlen = optlen;
#if LWIP_MPU_COMPATIBLE
  MEMCPY(LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).optval, optval, optlen);
#else /* LWIP_MPU_COMPATIBLE */
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).optval.pc = (const void *)optval;
#endif /* LWIP_MPU_COMPATIBLE */
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).err = 0;
#if LWIP_NETCONN_SEM_PER_THREAD
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).completed_sem = LWIP_NETCONN_THREAD_SEM_GET();
#else
  LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).completed_sem = &sock->conn->op_completed;
#endif
  cberr = tcpip_callback(lwip_setsockopt_callback, &LWIP_SETGETSOCKOPT_DATA_VAR_REF(data));
  if (cberr != ERR_OK) {
    LWIP_SETGETSOCKOPT_DATA_VAR_FREE(data);
    sock_set_errno(sock, err_to_errno(cberr));
    done_socket(sock);
    return -1;
  }
  sys_arch_sem_wait((sys_sem_t *)(LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).completed_sem), 0);

  /* maybe lwip_getsockopt_internal has changed err */
  err = LWIP_SETGETSOCKOPT_DATA_VAR_REF(data).err;
  LWIP_SETGETSOCKOPT_DATA_VAR_FREE(data);
#endif  /* LWIP_TCPIP_CORE_LOCKING */

  if (err != ERR_OK) {
    sock_set_errno(sock, err);
  }
  done_socket(sock);
  return err ? -1 : 0;
}

#if !LWIP_TCPIP_CORE_LOCKING
/** lwip_setsockopt_callback: only used without CORE_LOCKING
 * to get into the tcpip_thread
 */
static void
lwip_setsockopt_callback(void *arg)
{
  struct lwip_setgetsockopt_data *data;
  LWIP_ASSERT("arg != NULL", arg != NULL);
  data = (struct lwip_setgetsockopt_data *)arg;

  data->err = lwip_setsockopt_impl(data->s, data->level, data->optname,
#if LWIP_MPU_COMPATIBLE
                                   data->optval,
#else /* LWIP_MPU_COMPATIBLE */
                                   data->optval.pc,
#endif /* LWIP_MPU_COMPATIBLE */
                                   data->optlen);

  sys_sem_signal((sys_sem_t *)(data->completed_sem));
}
#endif  /* LWIP_TCPIP_CORE_LOCKING */

/** lwip_getoptval: get input parameter optval.
 * warning: this function is only suit for setting u8_t type socket options.
 */
static inline s32_t
lwip_getoptval(const void *optval, socklen_t optlen)
{
  int val = 0;
  int len = (optlen < sizeof(int)) ? sizeof(u8_t) : sizeof(int);
  (void)memcpy_s(&val, sizeof(int), optval, len);
  return val;
}

/** lwip_setsockopt_impl: the actual implementation of setsockopt:
 * same argument as lwip_setsockopt, either called directly or through callback
 */
static int
lwip_setsockopt_impl(int s, int level, int optname, const void *optval, socklen_t optlen)
{
  int err = 0;
  struct lwip_sock *sock = tryget_socket(s);
  if (!sock) {
    return EBADF;
  }

#ifdef LWIP_HOOK_SOCKETS_SETSOCKOPT
  if (LWIP_HOOK_SOCKETS_SETSOCKOPT(s, sock, level, optname, optval, optlen, &err)) {
    return err;
  }
#endif

  switch (level) {

    /* Level: SOL_SOCKET */
    case SOL_SOCKET:
#if PF_PKT_SUPPORT
    case SOL_PACKET:
      VALIDATE_SET_PF_PKT_OPTNAME_RET(s, sock, level, optname);
#endif /* PF_PKT_SUPPORT */

      VALIDATE_SET_RAW_OPTNAME_RET(sock, optname);
      switch (optname) {

        /* SO_ACCEPTCONN is get-only */
#if LWIP_SOCK_FILTER
      case SO_ATTACH_FILTER:
        {
          struct sock_fprog *filter = NULL;
          /* only AF_PACKET RAW socket support sock filter now */
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, struct sock_fprog, NETCONN_PKT_RAW);
          filter = (struct sock_fprog *)optval;
          err = (u8_t)sock_attach_filter(filter, sock->conn);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, SOL_SOCKET, optname=0x%x, ..) = %u\n",
                      s, SO_ATTACH_FILTER, err));
          break;
        }
      case SO_DETACH_FILTER:
        /* only AF_PACKET RAW socket support sock filter now */
        if (sock->conn == NULL) {
          done_socket(sock);
          return EINVAL;
        }
        if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) != NETCONN_PKT_RAW) {
          done_socket(sock);
          return ENOPROTOOPT;
        }
        err = (u8_t)sock_detach_filter(sock->conn);
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, SOL_SOCKET, optname=0x%x, ..) = %u\n",
                    s, SO_DETACH_FILTER, err));
        break;
#endif /* LWIP_SOCK_FILTER && PF_PKT_SUPPORT */
        /* The option flags */
        case SO_BROADCAST:
#if LWIP_SO_DONTROUTE
        case SO_DONTROUTE:
#endif /* LWIP_SO_DONTROUTE */
        case SO_KEEPALIVE:
#if SO_REUSE
        case SO_REUSEADDR:
#endif /* SO_REUSE */
          if (optname == SO_BROADCAST) {
#if LWIP_ENABLE_NET_CAPABILITY && LWIP_ENABLE_CAP_NET_BROADCAST
            LWIP_ERROR("lwip_setsockopt_impl permission deny: NET_BROADCAST\n",
                       IsCapPermit(CAP_NET_BROADCAST), err = EPERM; break);
#endif
            if (NETCONNTYPE_GROUP(sock->conn->type) == NETCONN_TCP) {
              done_socket(sock);
              return ENOPROTOOPT;
            }
          }

          optname = lwip_sockopt_to_ipopt(optname);

          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, optlen, int);
          if (*(const int *)optval) {
            ip_set_option(sock->conn->pcb.ip, optname);
          } else {
            ip_reset_option(sock->conn->pcb.ip, optname);
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, SOL_SOCKET, optname=0x%x, ..) -> %s\n",
                                      s, optname, (*(const int *)optval ? "on" : "off")));
          break;

          /* SO_TYPE is get-only */
          /* SO_ERROR is get-only */

#if LWIP_SO_SNDTIMEO
        case SO_SNDTIMEO: {
          long ms_long;
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, optlen, LWIP_SO_SNDRCVTIMEO_OPTTYPE);
          LWIP_SO_SNDRECVTIMO_VALIDATE(sock, optval);
          ms_long = LWIP_SO_SNDRCVTIMEO_GET_MS(optval);
          if (ms_long < 0) {
            done_socket(sock);
            return EINVAL;
          }
          netconn_set_sendtimeout(sock->conn, ms_long);
          break;
        }
#endif /* LWIP_SO_SNDTIMEO */
#if LWIP_SO_RCVTIMEO
        case SO_RCVTIMEO: {
          long ms_long;
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, optlen, LWIP_SO_SNDRCVTIMEO_OPTTYPE);
          LWIP_SO_SNDRECVTIMO_VALIDATE(sock, optval);
          ms_long = LWIP_SO_SNDRCVTIMEO_GET_MS(optval);
          if (ms_long < 0) {
            done_socket(sock);
            return EINVAL;
          }
          netconn_set_recvtimeout(sock->conn, (u32_t)ms_long);
          break;
        }
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
        case SO_RCVBUF:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, optlen, int);
          if ((*(int*)optval) < RECV_BUFSIZE_MIN) {
            LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, SOL_SOCKET, optname=0x%x) Invalid value \n",
                        s, SO_RCVBUF));
            done_socket(sock);
            return EINVAL;
          }
          netconn_set_recvbufsize(sock->conn, *(const int *)optval);
          break;
#endif /* LWIP_SO_RCVBUF */
#if LWIP_TCP && LWIP_SO_SNDBUF
      case SO_SNDBUF:
        LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, int, NETCONN_TCP);
        if (((sock->conn->pcb.tcp->state != CLOSED) && (sock->conn->pcb.tcp->state != LISTEN))) {
          LWIP_DEBUGF(SOCKETS_DEBUG,
                      ("lwip_setsockopt(%d, SOL_SOCKET, optname=0x%x) Unsupported state \n", s, SO_SNDBUF));
          done_socket(sock);
          return EOPNOTSUPP;
        }

        if (((*(int*)optval) < SEND_BUFSIZE_MIN) || ((*(int*)optval) > SEND_BUFSIZE_MAX)) {
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, SOL_SOCKET, optname=0x%x) Invalid value \n", s, SO_SNDBUF));
          done_socket(sock);
          return EINVAL;
        } else {
          tcp_set_sendbufsize(sock->conn->pcb.tcp, *(u32_t*)optval);

          if (sock->conn->pcb.tcp->state == CLOSED) {
            sock->conn->pcb.tcp->snd_buf = *(u32_t*)optval;
          }
        }
        break;
#endif /* LWIP_TCP && LWIP_SO_SNDBUF */
#if LWIP_SO_LINGER
        case SO_LINGER: {
          const struct linger *linger = (const struct linger *)optval;
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, optlen, struct linger);
          if (linger->l_onoff) {
            int lingersec = linger->l_linger;
            if (lingersec < 0) {
              done_socket(sock);
              return EINVAL;
            }
            if (lingersec > 0xFFFF) {
              lingersec = 0xFFFF;
            }
            sock->conn->linger = (s16_t)lingersec;
          } else {
            sock->conn->linger = -1;
          }
        }
        break;
#endif /* LWIP_SO_LINGER */
#if LWIP_UDP
        case SO_NO_CHECK:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, int, NETCONN_UDP);
#if LWIP_UDPLITE
          if (udp_is_flag_set(sock->conn->pcb.udp, UDP_FLAGS_UDPLITE)) {
            /* this flag is only available for UDP, not for UDP lite */
            done_socket(sock);
            return EAFNOSUPPORT;
          }
#endif /* LWIP_UDPLITE */
          if (*(const int *)optval) {
            udp_set_flags(sock->conn->pcb.udp, UDP_FLAGS_NOCHKSUM);
          } else {
            udp_clear_flags(sock->conn->pcb.udp, UDP_FLAGS_NOCHKSUM);
          }
          break;
#endif /* LWIP_UDP */
        case SO_BINDTODEVICE: {
          const struct ifreq *iface;
          struct netif *n = NULL;

          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, optlen, struct ifreq);

          iface = (const struct ifreq *)optval;
          if (iface->ifr_name[0] != 0) {
            n = netif_find(iface->ifr_name);
            if (n == NULL) {
              done_socket(sock);
              return ENODEV;
            }
          }

          switch (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn))) {
#if LWIP_TCP
            case NETCONN_TCP:
              tcp_bind_netif(sock->conn->pcb.tcp, n);
              break;
#endif
#if LWIP_UDP
            case NETCONN_UDP:
              udp_bind_netif(sock->conn->pcb.udp, n);
              break;
#endif
#if LWIP_RAW
            case NETCONN_RAW:
              raw_bind_netif(sock->conn->pcb.raw, n);
              break;
#endif
            default:
              LWIP_ASSERT("Unhandled netconn type in SO_BINDTODEVICE", 0);
              break;
          }
        }
        break;
#if LWIP_SO_PRIORITY
        case SO_PRIORITY: {
#if LWIP_ENABLE_NET_CAPABILITY
          LWIP_ERROR("permission deny: NET_ADMIN\n", IsCapPermit(CAP_NET_ADMIN), err = EPERM; break);
#endif
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN(sock, optlen, prio_t);
          if ((*(prio_t *)optval) > LWIP_PKT_PRIORITY_MAX) {
            LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, SO_PRIORITY, optname=0x%x) Invalid value \n", s,
                                        SO_PRIORITY));
            return EINVAL;
          }
#if LWIP_UDP
          if (NETCONNTYPE_GROUP(sock->conn->type) == NETCONN_UDP) {
            sock->conn->pcb.udp->priority = *(const prio_t *)optval;
          } else
#endif
#if LWIP_TCP
          if (NETCONNTYPE_GROUP(sock->conn->type) == NETCONN_TCP) {
            sock->conn->pcb.tcp->priority = *(const prio_t *)optval;
          } else
#endif
#if LWIP_RAW
          if (NETCONNTYPE_GROUP(sock->conn->type) == NETCONN_RAW) {
            sock->conn->pcb.raw->priority = *(const prio_t *)optval;
          } else
#endif
          {
            LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt Invalid socket type \n"));
            return ENOPROTOOPT;
          }
          break;
        }
#endif /* LWIP_SO_PRIORITY */

        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, SOL_SOCKET, UNIMPL: optname=0x%x, ..)\n",
                                      s, optname));
          err = ENOPROTOOPT;
          break;
      }  /* switch (optname) */
      break;

    /* Level: IPPROTO_IP */
    case IPPROTO_IP:
      switch (optname) {
        case IP_TTL:
          {
            s32_t ip_ttl;
            LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, optlen, u8_t);
            ip_ttl = lwip_getoptval(optval, optlen);
            if ((IP_USE_DEFAULT_TTL <= ip_ttl) && (ip_ttl <= IP_MAX_TTL)) {
              sock->conn->pcb.ip->ttl = (ip_ttl == IP_USE_DEFAULT_TTL ? IP_DEFAULT_TTL : (u8_t)ip_ttl);
              LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IP, IP_TTL, ..) -> %"U8_F"\n",
                          s, sock->conn->pcb.ip->ttl));
            } else {
              err = EINVAL;
            }
          }
          break;
        case IP_TOS:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, optlen, u8_t);
          sock->conn->pcb.ip->tos = (u8_t)lwip_getoptval(optval, optlen);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IP, IP_TOS, ..)-> %u\n",
                                      s, sock->conn->pcb.ip->tos));
          break;
#if LWIP_RAW
      case IP_HDRINCL:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, u8_t, NETCONN_RAW);
          if (NETCONNTYPE_ISIPV6(sock->conn->type)) {
            done_socket(sock);
            return ENOPROTOOPT;
          }

          if (lwip_getoptval(optval, optlen)) {
            raw_set_flags(sock->conn->pcb.raw, RAW_FLAGS_HDRINCL);
          } else {
            raw_clear_flags(sock->conn->pcb.raw, RAW_FLAGS_HDRINCL);
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IP_HDRINCL)-> %u\n",
                      s, *(u8_t *)optval));
          break;
#endif
#if LWIP_NETBUF_RECVINFO
        case IP_PKTINFO:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, u8_t, NETCONN_UDP);
          if (lwip_getoptval(optval, optlen)) {
            sock->conn->flags |= NETCONN_FLAG_PKTINFO;
          } else {
            sock->conn->flags &= ~NETCONN_FLAG_PKTINFO;
          }
          break;
#endif /* LWIP_NETBUF_RECVINFO */
#if LWIP_IPV4 && LWIP_MULTICAST_TX_OPTIONS && LWIP_UDP
        case IP_MULTICAST_TTL:
          {
            s32_t mc_ttl;
            LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, u8_t, NETCONN_UDP);
            mc_ttl = lwip_getoptval(optval, optlen);
            if ((IP_USE_DEFAULT_TTL <= mc_ttl) && (mc_ttl <= IP_MAX_TTL)) {
              udp_set_multicast_ttl(sock->conn->pcb.udp, mc_ttl == IP_USE_DEFAULT_TTL ? MC_TTL : (u8_t)mc_ttl);
              LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IP, IP_MULTICAST_TTL, ..) -> %"U8_F"\n",
                          s, sock->conn->pcb.udp->mcast_ttl));
            } else {
              err = EINVAL;
            }
          }
          break;
        case IP_MULTICAST_IF:
          {
            struct netif *netif = NULL;
            ip_addr_t if_addr;
            (void)memset_s(&if_addr, sizeof(ip_addr_t), 0, sizeof(ip_addr_t));
            IP_SET_TYPE_VAL(if_addr, IPADDR_TYPE_V4);
            LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, struct in_addr, NETCONN_UDP);
            inet_addr_to_ip4addr(ip_2_ip4(&if_addr), (const struct in_addr*)optval);
            netif = netif_find_by_ipaddr(&if_addr);
            if ((netif == NULL) ||
                (sock->conn->pcb.ip->netif_idx && (sock->conn->pcb.ip->netif_idx != netif->ifindex))) {
              err = EADDRNOTAVAIL;
            } else {
              udp_set_multicast_netif_addr(sock->conn->pcb.udp, ip_2_ip4(&if_addr));
            }
          }
          break;
        case IP_MULTICAST_LOOP:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, u8_t, NETCONN_UDP);
          if (*(const u8_t *)optval) {
            udp_set_flags(sock->conn->pcb.udp, UDP_FLAGS_MULTICAST_LOOP);
          } else {
            udp_clear_flags(sock->conn->pcb.udp, UDP_FLAGS_MULTICAST_LOOP);
          }
          break;
#endif /* LWIP_IPV4 && LWIP_MULTICAST_TX_OPTIONS && LWIP_UDP */
#if LWIP_IGMP
        case IP_ADD_MEMBERSHIP:
        case IP_DROP_MEMBERSHIP: {
          /* If this is a TCP or a RAW socket, ignore these options. */
          err_t igmp_err;
          const struct ip_mreq *imr = (const struct ip_mreq *)optval;
          ip4_addr_t if_addr;
          ip4_addr_t multi_addr;
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, struct ip_mreq, NETCONN_UDP);
          inet_addr_to_ip4addr(&if_addr, &imr->imr_interface);
          inet_addr_to_ip4addr(&multi_addr, &imr->imr_multiaddr);
          if (optname == IP_ADD_MEMBERSHIP) {
            if (!lwip_socket_register_membership(s, &if_addr, &multi_addr)) {
              /* cannot track membership (out of memory) */
              err = ENOMEM;
              igmp_err = ERR_OK;
            } else {
              igmp_err = igmp_joingroup(&if_addr, &multi_addr);
            }
          } else {
            igmp_err = igmp_leavegroup(&if_addr, &multi_addr);
            lwip_socket_unregister_membership(s, &if_addr, &multi_addr);
          }
          if (igmp_err != ERR_OK) {
            err = EADDRNOTAVAIL;
          }
        }
        break;
#endif /* LWIP_IGMP */
        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IP, UNIMPL: optname=0x%x, ..)\n",
                                      s, optname));
          err = ENOPROTOOPT;
          break;
      }  /* switch (optname) */
      break;

#if LWIP_TCP
    /* Level: IPPROTO_TCP */
    case IPPROTO_TCP:
      /* Special case: all IPPROTO_TCP option take an int */
      LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, int, NETCONN_TCP);
      if ((sock->conn->pcb.tcp->state == LISTEN)
#if LWIP_TCP_MAXSEG
          && (optname != TCP_MAXSEG)
#endif
         ) {
        done_socket(sock);
        return EINVAL;
      }
      switch (optname) {
        case TCP_NODELAY:
          if (*(const int *)optval) {
            tcp_nagle_disable(sock->conn->pcb.tcp);
          } else {
            tcp_nagle_enable(sock->conn->pcb.tcp);
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_NODELAY) -> %s\n",
                                      s, (*(const int *)optval) ? "on" : "off") );
          break;
#if LWIP_TCP_KEEPALIVE
        case TCP_KEEPIDLE:
          sock->conn->pcb.tcp->keep_idle = MS_PER_SECOND * (u32_t)(*(const int*)optval);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPIDLE) -> %"U32_F"\n",
                                      s, sock->conn->pcb.tcp->keep_idle));
          break;
        case TCP_KEEPINTVL:
          sock->conn->pcb.tcp->keep_intvl = MS_PER_SECOND * (u32_t)(*(const int*)optval);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPINTVL) -> %"U32_F"\n",
                                      s, sock->conn->pcb.tcp->keep_intvl));
          break;
        case TCP_KEEPCNT:
          sock->conn->pcb.tcp->keep_cnt = (u32_t)(*(const int *)optval);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_KEEPCNT) -> %"U32_F"\n",
                                      s, sock->conn->pcb.tcp->keep_cnt));
          break;
#endif /* LWIP_TCP_KEEPALIVE */

#if LWIP_TCP_MAXSEG
        case TCP_MAXSEG:
        {
          s32_t usr_mss = *(const int*)optval;
          if ((usr_mss < TCP_MIN_MSS) || (usr_mss > TCP_MAX_MSS)) {
            done_socket(sock);
            return EINVAL;
          }
          sock->conn->pcb.tcp->usr_mss = (u16_t)(*(const int*)optval);
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, TCP_MAXSEG) -> %"U32_F"\n",
                      s, sock->conn->pcb.tcp->usr_mss));
          break;
        }
#endif /* LWIP_TCP_MAXSEG */
        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_TCP, UNIMPL: optname=0x%x, ..)\n",
                                      s, optname));
          err = ENOPROTOOPT;
          break;
      }  /* switch (optname) */
      break;
#endif /* LWIP_TCP*/

#if LWIP_IPV6
    /* Level: IPPROTO_IPV6 */
    case IPPROTO_IPV6:
      switch (optname) {
        case IPV6_V6ONLY:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, optlen, int);
          /* The below check is to ensure that the option is used for AF_INET6 only */
          LWIP_SOCKOPT_CHECK_IPTYPE_V6(sock, sock->conn->type);
          if (*(const int *)optval) {
            netconn_set_ipv6only(sock->conn, 1);
          } else {
            netconn_set_ipv6only(sock->conn, 0);
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IPV6, IPV6_V6ONLY, ..) -> %d\n",
                                      s, (netconn_get_ipv6only(sock->conn) ? 1 : 0)));
          break;
#if LWIP_RAW
        case IPV6_CHECKSUM:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, optlen, int, NETCONN_RAW);
          if (sock->conn->pcb.raw->raw_proto == IPPROTO_ICMPV6) {
            done_socket(sock);
            LWIP_DEBUGF(SOCKETS_DEBUG,
                        ("lwip_setsockopt(%d, socket type IPPROTO_ICMPV6, \
                        opttype - IPV6_CHECKSUM, ..) -> %d Not allowed \n",
                        s, sock->conn->pcb.raw->chksum_reqd));
            return EINVAL;
          }

          if (*(const int *)optval < 0) {
            sock->conn->pcb.raw->chksum_reqd = 0;
            /* reseting the offset of icmp value to 2 */
            sock->conn->pcb.raw->chksum_offset = RAW_CHKSUM_OFFSET;
          } else if ((unsigned int)(*(const int *)optval) & 1) {
            done_socket(sock);
              /* Per RFC3542, odd offsets are not allowed */
            return EINVAL;
          } else if (*(const int *)optval > 0xFFFF) {
            done_socket(sock);
            /* Check added to avoid overflow or avoid big invalid value. */
              return EINVAL;
          } else {
              sock->conn->pcb.raw->chksum_reqd = 1;
              sock->conn->pcb.raw->chksum_offset = (u16_t)(*(const int *)optval);
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IPV6, IPV6_CHECKSUM, ..) -> %d\n",
                      s, sock->conn->pcb.raw->chksum_reqd));
          break;
#endif
#if LWIP_IPV6_MLD
        case IPV6_JOIN_GROUP:
        case IPV6_LEAVE_GROUP: {
          /* If this is a TCP or a RAW socket, ignore these options. */
          err_t mld6_err;
          struct netif *netif;
          ip6_addr_t multi_addr;
          const struct ipv6_mreq *imr = (const struct ipv6_mreq *)optval;
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, struct ipv6_mreq, NETCONN_UDP);
          inet6_addr_to_ip6addr(&multi_addr, &imr->ipv6mr_multiaddr);

          /* Check if the address is multicast or not */
          if (!ip6_addr_ismulticast(&multi_addr)) {
            err = EADDRNOTAVAIL;
            break;
          }

          /* Stack as of now doesnot handle the 0 interface index scenario */
          if (imr->ipv6mr_interface > LWIP_NETIF_IFINDEX_MAX_EX) {
              LWIP_DEBUGF(SOCKETS_DEBUG,
                          ("lwip_setsockopt(%d, Invalid Interface index IPPROTO_IPV6, \
                          IPV6_JOIN_GROUP/IPV6_LEAVE_GROUP) -> %d\n",
                          s, (imr->ipv6mr_interface)));
              err = ENXIO;
              break;
          }

          LWIP_ASSERT("Invalid netif index", imr->ipv6mr_interface <= 0xFFu);
          netif = netif_get_by_index((u8_t)imr->ipv6mr_interface);
          if (netif == NULL) {
            err = EADDRNOTAVAIL;
            break;
          }

          if (optname == IPV6_JOIN_GROUP) {
            if (!lwip_socket_register_mld6_membership(s, imr->ipv6mr_interface, &multi_addr)) {
              /* cannot track membership (out of memory) */
              err = ENOMEM;
              mld6_err = ERR_OK;
            } else {
              mld6_err = mld6_joingroup_netif(netif, &multi_addr);
            }
          } else {
            mld6_err = mld6_leavegroup_netif(netif, &multi_addr);
            lwip_socket_unregister_mld6_membership(s, imr->ipv6mr_interface, &multi_addr);
          }
          if (mld6_err != ERR_OK) {
            err = EADDRNOTAVAIL;
          }
        }
        break;
#endif /* LWIP_IPV6_MLD */
#if LWIP_MULTICAST_TX_OPTIONS
      case IPV6_MULTICAST_HOPS:
        {
          s32_t mc_hops;
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, optlen, int, NETCONN_UDP);
          mc_hops = (s32_t)(*(const s32_t *)optval);
          if ((IP_USE_DEFAULT_TTL <= mc_hops) && (mc_hops <= IP_MAX_TTL)) {
            udp_set_multicast_ttl(sock->conn->pcb.udp, mc_hops == IP_USE_DEFAULT_TTL ? MC_TTL : (u8_t)mc_hops);
          } else {
            err = EINVAL;
          }
        }
        break;
      case IPV6_MULTICAST_IF:
        LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, optlen, u8_t, NETCONN_UDP);
        udp_set_multicast_netif_index(sock->conn->pcb.udp, (u8_t)(*(const u8_t *)optval));
        break;
      case IPV6_MULTICAST_LOOP:
        LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, optlen, u8_t, NETCONN_UDP);
        if (*(const u8_t*)optval) {
          udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) | UDP_FLAGS_MULTICAST_LOOP);
        } else {
          udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) & ~UDP_FLAGS_MULTICAST_LOOP);
        }
        break;
#endif /* LWIP_MULTICAST_TX_OPTIONS */

#if LWIP_NETBUF_RECVINFO
      case IPV6_RECVPKTINFO:
        LWIP_SOCKOPT_CHECK_IPTYPE_V6(sock, sock->conn->type);
        LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE(sock, optlen, int, NETCONN_UDP);
        if (*(const int *)optval) {
          sock->conn->flags |= NETCONN_FLAG_PKTINFO;
        } else {
          sock->conn->flags &= ~NETCONN_FLAG_PKTINFO;
        }
        break;
#endif /* LWIP_NETBUF_RECVINFO */
        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_IPV6, UNIMPL: optname=0x%x, ..)\n",
                                      s, optname));
          err = ENOPROTOOPT;
          break;
      }  /* switch (optname) */
      break;
#endif /* LWIP_IPV6 */

#if LWIP_UDP && LWIP_UDPLITE
    /* Level: IPPROTO_UDPLITE */
    case IPPROTO_UDPLITE:
      /* Special case: all IPPROTO_UDPLITE option take an int */
      LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB(sock, optlen, int);
      /* If this is no UDP lite socket, ignore any options. */
      if (!NETCONNTYPE_ISUDPLITE(NETCONN_TYPE(sock->conn))) {
        done_socket(sock);
        return ENOPROTOOPT;
      }
      switch (optname) {
        case UDPLITE_SEND_CSCOV:
          if ((*(const int *)optval != 0) && ((*(const int *)optval < 8) || (*(const int *)optval > 0xffff))) {
            /* don't allow illegal values! */
            sock->conn->pcb.udp->chksum_len_tx = 8;
          } else {
            sock->conn->pcb.udp->chksum_len_tx = (u16_t) * (const int *)optval;
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_UDPLITE, UDPLITE_SEND_CSCOV) -> %d\n",
                                      s, (*(const int *)optval)) );
          break;
        case UDPLITE_RECV_CSCOV:
          if ((*(const int *)optval != 0) && ((*(const int *)optval < 8) || (*(const int *)optval > 0xffff))) {
            /* don't allow illegal values! */
            sock->conn->pcb.udp->chksum_len_rx = 8;
          } else {
            sock->conn->pcb.udp->chksum_len_rx = (u16_t) * (const int *)optval;
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_UDPLITE, UDPLITE_RECV_CSCOV) -> %d\n",
                                      s, (*(const int *)optval)) );
          break;
        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_UDPLITE, UNIMPL: optname=0x%x, ..)\n",
                                      s, optname));
          err = ENOPROTOOPT;
          break;
      }  /* switch (optname) */
      break;
#endif /* LWIP_UDP */
    /* Level: IPPROTO_RAW */
    case IPPROTO_RAW:
      switch (optname) {
#if LWIP_IPV6 && LWIP_RAW
        case IPV6_CHECKSUM:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, optlen, int, NETCONN_RAW);
          /* It should not be possible to disable the checksum generation with ICMPv6
           * as per RFC 3542 chapter 3.1 */
          if (sock->conn->pcb.raw->raw_proto == IPPROTO_ICMPV6) {
            done_socket(sock);
            return EINVAL;
          }

          if (*(const int *)optval < 0) {
            sock->conn->pcb.raw->chksum_reqd = 0;
            sock->conn->pcb.raw->chksum_offset = RAW_CHKSUM_OFFSET;
          } else if (*(const int *)optval & 1) {
            /* Per RFC3542, odd offsets are not allowed */
            done_socket(sock);
            return EINVAL;
          } else if (*(const int *)optval > 0xFFFF) {
            sock->conn->pcb.raw->chksum_reqd = 1;
            /* Check added to avoid overflow or avoid  big invalid value. */
            done_socket(sock);
            return EINVAL;
          } else {
            sock->conn->pcb.raw->chksum_reqd = 1;
            sock->conn->pcb.raw->chksum_offset = (u16_t) * (const int *)optval;
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_RAW, IPV6_CHECKSUM, ..) -> %d\n",
                                      s, sock->conn->pcb.raw->chksum_reqd));
          break;
#endif /* LWIP_IPV6 && LWIP_RAW */
        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_RAW, UNIMPL: optname=0x%x, ..)\n",
                                      s, optname));
          err = ENOPROTOOPT;
          break;
      }  /* switch (optname) */
      break;

  /* Below feature is enabled  only if ipv6 raw socket is enabled */
#if LWIP_IPV6 && LWIP_RAW && LWIP_SOCK_OPT_ICMP6_FILTER
    /* Level: IPPROTO_ICMPV6 */
    case IPPROTO_ICMPV6:
      switch (optname) {
        case ICMP6_FILTER:
          LWIP_SOCKOPT_CHECK_OPTLEN_CONN_PCB_TYPE_IPV6(sock, optlen, struct icmp6_filter, NETCONN_RAW);
          if (optlen > sizeof(struct icmp6_filter)) {
            optlen = sizeof(struct icmp6_filter);
          }

          if (memcpy_s(&sock->conn->pcb.raw->icmp6_filter, sizeof(struct icmp6_filter),
                       optval, optlen) != 0) {
            LWIP_DEBUGF(SOCKETS_DEBUG,
                        ("lwip_setsockopt(%d, IPPROTO_ICMPV6, ICMP6_FILTER:  ..) mem error\n", s));
            err = EINVAL;
            break;
          }
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_ICMPV6, ICMP6_FILTER:  ..)\n", s));
          break;
        default:
          LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, IPPROTO_ICMPV6, UNIMPL: optname=0x%x, ..)\n",
                            s, optname));
          err = ENOPROTOOPT;
          break;
      }
      break;
#endif /* LWIP_IPV6 && LWIP_RAW */
    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_setsockopt(%d, level=0x%x, UNIMPL: optname=0x%x, ..)\n",
                                  s, level, optname));
      err = ENOPROTOOPT;
      break;
  }  /* switch (level) */

  done_socket(sock);
  return err;
}

#if LWIP_IOCTL_ROUTE
static u8_t lwip_ioctl_internal_SIOCADDRT(const struct rtentry *rmten)
{
  struct netif *netif = NULL;
  ip_addr_t rtgw_addr;
  u16_t rtgw_port;

#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCADDRT: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif

  SOCKADDR_TO_IPADDR_PORT(&rmten->rt_gateway, &rtgw_addr, rtgw_port);
  LWIP_UNUSED_ARG(rtgw_port);

  if (!IP_IS_V4_VAL(rtgw_addr)) {
    return EINVAL;
  }

  /* check if multicast/0/loopback */
  if (ip_addr_ismulticast_val(rtgw_addr) || ip_addr_isany_val(rtgw_addr) ||
      ip_addr_isloopback_val(rtgw_addr)) {
    return EINVAL;
  }

  /* check if reachable */
  for (netif = netif_list; netif != NULL; netif = netif->next) {
    if (ip_addr_netcmp_val(rtgw_addr, netif->ip_addr, ip_2_ip4(&netif->netmask))) {
      break;
    }
  }

  if (netif == NULL) {
    return EHOSTUNREACH;
  }

  /* check if broadcast */
  if (ip_addr_isbroadcast_val(rtgw_addr, netif)) {
    return EINVAL;
  }

  /* Check flags */
  if (!(rmten->rt_flags & RTF_GATEWAY)) {
    return EINVAL;
  }

  /* Add validation */
  if (netif_default != netif) {
    if (netif_default != NULL) {
      ip_addr_set_zero(&netif_default->gw);
    }
    (void)netif_set_default(netif);
  }
  netif_set_gw(netif, ip_2_ip4(&rtgw_addr));

  return 0;
}
#endif /* LWIP_IOCTL_ROUTE */

#if LWIP_IOCTL_IF
static u8_t lwip_ioctl_internal_SIOCGIFCONF(struct ifreq *ifr)
{
  struct ifconf *ifc = NULL;
  struct netif *netif = NULL;
  struct ifreq ifreq;
  struct sockaddr_in *sock_in = NULL;
  int pos;
  int len;
  int ret;

  /* Format the caller's buffer. */
  ifc = (struct ifconf*)ifr;
  len = ifc->ifc_len;

  /* Loop over the interfaces, and write an info block for each. */
  pos = 0;
  for (netif = netif_list; netif != NULL; netif = netif->next) {
    if (ifc->ifc_buf == NULL) {
      pos =  (pos + (int)sizeof(struct ifreq));
      continue;
    }

    if (len < (int)sizeof(ifreq)) {
      break;
    }

    (void)memset_s(&ifreq, sizeof(struct ifreq), 0, sizeof(struct ifreq));
    if (netif->link_layer_type == LOOPBACK_IF) {
      ret = snprintf_s(ifreq.ifr_name, NETIF_NAMESIZE, (NETIF_NAMESIZE - 1), "%s", netif->name);
      if ((ret <= 0) || (ret >= IFNAMSIZ)) {
        LWIP_DEBUGF(NETIF_DEBUG, ("lwip_ioctl: snprintf_s ifr_name failed."));
        return ENOBUFS;
      }
    } else {
      ret = snprintf_s(ifreq.ifr_name, NETIF_NAMESIZE, NETIF_NAMESIZE - 1, "%s%"U8_F, netif->name, netif->num);
      if ((ret <= 0) || (ret >= NETIF_NAMESIZE)) {
        LWIP_DEBUGF(NETIF_DEBUG, ("lwip_ioctl: snprintf_s ifr_name failed."));
        return ENOBUFS;
      }
    }

    sock_in = (struct sockaddr_in *)&ifreq.ifr_addr;
    sock_in->sin_family = AF_INET;
    sock_in->sin_addr.s_addr = ip_2_ip4(&netif->ip_addr)->addr;
    if (memcpy_s(ifc->ifc_buf + pos, sizeof(struct ifreq), &ifreq, sizeof(struct ifreq)) != EOK) {
      return ENOBUFS;
    }
    pos = pos + (int)sizeof(struct ifreq);
    len = len - (int)sizeof(struct ifreq);
  }

  ifc->ifc_len = pos;

  return 0;
}

static u8_t lwip_ioctl_internal_SIOCGIFADDR(struct ifreq *ifr)
{
  struct netif *netif = NULL;
  struct sockaddr_in *sock_in = NULL;

  /* get netif ipaddr */
  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  } else {
    sock_in = (struct sockaddr_in *)&ifr->ifr_addr;
    sock_in->sin_family = AF_INET;
    sock_in->sin_addr.s_addr = ip_2_ip4(&netif->ip_addr)->addr;
    return 0;
  }
}

#if LWIP_IPV6
static u8_t lwip_ioctl_internal_SIOCSIFADDR_6(struct ifreq *ifr)
{
  struct netif *netif = NULL;
  err_t err;
  struct in6_ifreq *ifr6 = NULL;
  ip_addr_t target_addr;
  s8_t idx;

#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCSIFADDR_6: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif

  ifr6 = (struct in6_ifreq *)ifr;

  /* Supports only fixed length LWIP_IPV6_PREFIX_LEN of prefix */
  if ((ifr6->ifr6_prefixlen != LWIP_IPV6_PREFIX_LEN) ||
      (ifr6->ifr6_ifindex > LWIP_NETIF_IFINDEX_MAX)) {
    return EINVAL;
  }

  netif = netif_get_by_index((u8_t)ifr6->ifr6_ifindex);
  if (netif == NULL) {
    return ENODEV;
  }
#if LWIP_HAVE_LOOPIF
  /* Most likely unhittable code.. since loopif is global address.. remove code after confirmation */
  else if (netif->link_layer_type == LOOPBACK_IF) {
    return EPERM;
  }
#endif

  ip_addr_set_zero_ip6_val(target_addr);
  inet6_addr_to_ip6addr(ip_2_ip6(&target_addr), &ifr6->ifr6_addr);

  /* Do no allow multicast or any ip address or loopback address to be set as interface address */
  if (ip_addr_ismulticast_val(target_addr) ||
      ip_addr_isany_val(target_addr) ||
      ip_addr_isloopback_val(target_addr)) {
    return EINVAL;
  }

  idx = netif_get_ip6_addr_match(netif, ip_2_ip6(&target_addr));
  if (idx >= 0) {
    return EEXIST;
  }

#if LWIP_IPV6_DHCP6
  if (netif_dhcp6_data(netif) && netif_dhcp6_data(netif)->state != DHCP6_STATE_OFF) {
    dhcp6_disable(netif);
  }
#endif

  err = netif_add_ip6_address(netif, ip_2_ip6(&target_addr), &idx);
  if ((err != ERR_OK) || (idx == -1)) {
    return ENOBUFS;
  }

  netif_ip6_addr_set_state(netif, idx, IP6_ADDR_PREFERRED);

  return ERR_OK;
}
#endif

static u8_t lwip_ioctl_internal_SIOCSIFADDR(const struct ifreq *ifr)
{
  struct netif *netif = NULL;

  struct netif *loc_netif = NULL;
  ip_addr_t taget_addr;
  u16_t taget_port;
  SOCKADDR_TO_IPADDR_PORT(&ifr->ifr_addr, &taget_addr, taget_port);
  LWIP_UNUSED_ARG(taget_port);

#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCSIFADDR: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif

  /* set netif ipaddr */
  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  }
#if LWIP_HAVE_LOOPIF
  else if (netif->link_layer_type == LOOPBACK_IF) {
    return EPERM;
  }
#endif
  else {
     /* check the address is not multicast/broadcast/0/loopback */
    if (!IP_IS_V4_VAL(taget_addr) || ip_addr_ismulticast_val(taget_addr) ||
        ip_addr_isbroadcast_val(taget_addr, netif) ||
        ip_addr_isany_val(taget_addr) ||
        ip_addr_isloopback_val(taget_addr)) {
      return EINVAL;
    }

    /* reset gateway if new and previous ipaddr not in same net */
    if (!ip_addr_netcmp_val(taget_addr, netif->ip_addr, ip_2_ip4(&netif->netmask))) {
      ip_addr_set_zero_val(netif->gw);
      if (netif == netif_default) {
        (void)netif_set_default(NULL);
      }
    }

    /* lwip disallow two netif sit in same net at the same time */
    loc_netif = netif_list;
    while (loc_netif != NULL) {
      if (loc_netif == netif) {
        loc_netif = loc_netif->next;
        continue;
      }
      if (ip_addr_cmp(&netif->netmask, &loc_netif->netmask) &&
          ip_addr_netcmp_val(loc_netif->ip_addr, taget_addr, ip_2_ip4(&netif->netmask))) {
        return EINVAL;
      }
      loc_netif = loc_netif->next;
    }

#if LWIP_DHCP
    if (netif_dhcp_data(netif) &&
        (netif_dhcp_data(netif)->client.states[LWIP_DHCP_NATIVE_IDX].state != DHCP_STATE_OFF)) {
      (void)netif_dhcp_off(netif);
    }
#endif

#if LWIP_ARP
    /* clear ARP cache when IP address changed */
    if (netif->flags & NETIF_FLAG_ETHARP) {
      etharp_cleanup_netif(netif);
    }
#endif /* LWIP_ARP */

    netif_set_ipaddr(netif, ip_2_ip4(&taget_addr));

    return 0;
  }
}

#if LWIP_IPV6
static u8_t lwip_ioctl_internal_SIOCDIFADDR_6(struct ifreq *ifr)
{
  struct netif *netif = NULL;
  err_t err;
  struct in6_ifreq *ifr6 = NULL;
  ip_addr_t target_addr;
  s8_t idx;

#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCDIFADDR_6: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif

  ifr6 = (struct in6_ifreq *)ifr;

  /* Supports only fixed length LWIP_IPV6_PREFIX_LEN of prefix */
  if ((ifr6->ifr6_prefixlen != LWIP_IPV6_PREFIX_LEN) ||
      (ifr6->ifr6_ifindex > LWIP_NETIF_IFINDEX_MAX)) {
    return EINVAL;
  }

  netif = netif_get_by_index((u8_t)ifr6->ifr6_ifindex);
  if (netif == NULL) {
    return ENODEV;
  }
#if LWIP_HAVE_LOOPIF
  /* Most likely unhittable code.. since loopif is global address.. remove code after confirmation */
  else if (netif->link_layer_type == LOOPBACK_IF) {
    return EPERM;
  }
#endif

  ip_addr_set_zero_ip6_val(target_addr);
  inet6_addr_to_ip6addr(ip_2_ip6(&target_addr), &ifr6->ifr6_addr);

  /* It should not be loopback address */
  if (ip_addr_isloopback_val(target_addr)) {
    return EINVAL;
  }

  /* Allow deletion of existing IPv6 address.... only limitation is, it does not allow removal of link local address */
  idx = netif_get_ip6_addr_match(netif, ip_2_ip6(&target_addr));
  if (idx < 0) {
    return EADDRNOTAVAIL;
  }

  /* Removal of link local address not permitted */
  if (idx == 0) {
    return EPERM;
  }

#if LWIP_IPV6_DHCP6
  if (netif_dhcp6_data(netif) && netif_dhcp6_data(netif)->state != DHCP6_STATE_OFF) {
    dhcp6_disable(netif);
  }
#endif

  err = netif_do_rmv_ipv6_addr(netif, ip_2_ip6(&target_addr));
  if ((err != ERR_OK) || (idx == -1)) {
    return ENOBUFS;
  }

  return ERR_OK;
}
#endif

static u8_t lwip_ioctl_internal_SIOCDIFADDR(const struct ifreq *ifr)
{
  struct netif *netif = NULL;

  ip_addr_t target_addr;
  u16_t target_port;

  SOCKADDR_TO_IPADDR_PORT(&ifr->ifr_addr, &target_addr, target_port);
  LWIP_UNUSED_ARG(target_port);

#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCDIFADDR: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif

  /* set netif ipaddr */
  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  }
#if LWIP_HAVE_LOOPIF
  else if (netif->link_layer_type == LOOPBACK_IF) {
    return EPERM;
  }
#endif

   /* check the address is not loopback */
  if (!IP_IS_V4_VAL(target_addr) || ip_addr_isloopback_val(target_addr)) {
    return EINVAL;
  }

#if LWIP_DHCP
  if (netif_dhcp_data(netif) &&
      (netif_dhcp_data(netif)->client.states[LWIP_DHCP_NATIVE_IDX].state != DHCP_STATE_OFF)) {
    (void)netif_dhcp_off(netif);
  }
#endif

  ip_addr_set_zero_val(netif->gw);
  ip_addr_set_zero_val(netif->ip_addr);
  ip_addr_set_zero_val(netif->netmask);
  if (netif == netif_default) {
    (void)netif_set_default(NULL);
  }

#if LWIP_IPV4 && LWIP_ARP
  if (netif->flags & NETIF_FLAG_ETHARP) {
    etharp_cleanup_netif(netif);
  }
#endif /* LWIP_IPV4 && LWIP_ARP */

  return ERR_OK;
}

static u8_t lwip_ioctl_internal_SIOCGIFBRDADDR(struct ifreq *ifr)
{
  struct netif *netif = NULL;
  struct sockaddr_in *sock_in = NULL;

  /* get netif subnet broadcast addr */
  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  }
  if (ip4_addr_isany_val(*(ip_2_ip4(&netif->netmask)))) {
    return ENXIO;
  }
  sock_in = (struct sockaddr_in *)&ifr->ifr_addr;
  sock_in->sin_family = AF_INET;
  sock_in->sin_addr.s_addr = (ip_2_ip4(&((netif)->ip_addr))->addr | ~(ip_2_ip4(&netif->netmask)->addr));
  return 0;
}

static u8_t lwip_ioctl_internal_SIOCGIFNETMASK(struct ifreq *ifr)
{
  struct netif *netif = NULL;
  struct sockaddr_in *sock_in = NULL;

  /* get netif netmask */
  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  } else {
    sock_in = (struct sockaddr_in *)&ifr->ifr_netmask;
    sock_in->sin_family = AF_INET;
    sock_in->sin_addr.s_addr = ip_2_ip4(&netif->netmask)->addr;
    return 0;
  }
}

static u8_t lwip_ioctl_internal_SIOCSIFNETMASK(const struct ifreq *ifr)
{
  struct netif *netif = NULL;

  struct netif *loc_netif = NULL;
  ip_addr_t taget_addr;
  u16_t taget_port;

#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCSIFNETMASK: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif

  SOCKADDR_TO_IPADDR_PORT(&ifr->ifr_addr, &taget_addr, taget_port);
  LWIP_UNUSED_ARG(taget_port);

  if (!IP_IS_V4_VAL(taget_addr)) {
    return EINVAL;
  }

  /* set netif netmask */
  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  }
#if LWIP_HAVE_LOOPIF
  else if (netif->link_layer_type == LOOPBACK_IF) {
    return EPERM;
  }
#endif
  else {
    if (ip_addr_cmp(&netif->netmask, &taget_addr)) {
      return 0;
    }
    /* check data valid */
    if (!ip_addr_netmask_valid(ip_2_ip4(&taget_addr))) {
      return EINVAL;
    }

    /* lwip disallow two netif sit in same net at the same time */
    loc_netif = netif_list;
    while (loc_netif != NULL) {
      if (loc_netif == netif) {
        loc_netif = loc_netif->next;
        continue;
      }
      if (ip_addr_cmp(&loc_netif->netmask, &taget_addr) &&
          ip_addr_netcmp_val(loc_netif->ip_addr, netif->ip_addr, ip_2_ip4(&loc_netif->netmask))) {
        return EINVAL;
      }
      loc_netif = loc_netif->next;
    }

#if LWIP_DHCP // LWIP_DHCP
    if (netif_dhcp_data(netif) &&
        (netif_dhcp_data(netif)->client.states[LWIP_DHCP_NATIVE_IDX].state != DHCP_STATE_OFF)) {
      (void)netif_dhcp_off(netif);
    }
#endif

    netif_set_netmask(netif, ip_2_ip4(&taget_addr));

    /* check if gateway still reachable */
    if (!ip_addr_netcmp_val(netif->gw, netif->ip_addr, ip_2_ip4(&taget_addr))) {
      ip_addr_set_zero_val(netif->gw);
      if (netif == netif_default) {
        (void)netif_set_default(NULL);
      }
    }
    return 0;
  }
}

static u8_t lwip_ioctl_internal_SIOCSIFHWADDR(const struct ifreq *ifr)
{
  struct netif *netif;
  err_t ret;

#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCSIFHWADDR: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif

  /* set netif hw addr */
  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  }
#if LWIP_HAVE_LOOPIF
  else if (netif->link_layer_type == LOOPBACK_IF) {
    return EPERM;
  }
#endif
  else {

    /* bring netif down to clear all Neighbor Cache Entry */
    (void)netif_set_down(netif);

    ret = netif_set_hwaddr(netif, (const unsigned char *)ifr->ifr_hwaddr.sa_data, netif->hwaddr_len);

    if (ret != ERR_OK) {
      (void)netif_set_up(netif);
      return (u8_t)err_to_errno(ret);
    }

    /* bring netif up to try to send GARP/IGMP/NA/MLD/RS. GARP and NA would
      make the neighboring nodes update their Neighbor Cache immediately. */
    (void)netif_set_up(netif);
    return 0;
  }
}

static u8_t lwip_ioctl_internal_SIOCGIFHWADDR(struct ifreq *ifr)
{
  struct netif *netif = NULL;

  /* get netif hw addr */
  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  }
#if LWIP_HAVE_LOOPIF
  else if (netif->link_layer_type == LOOPBACK_IF) {
    return EPERM;
  }
#endif /* LWIP_HAVE_LOOPIF */
  else {
    if (memcpy_s((void *)ifr->ifr_hwaddr.sa_data, sizeof(ifr->ifr_hwaddr.sa_data),
                 (void *)netif->hwaddr, netif->hwaddr_len) != EOK) {
      return EINVAL;
    }
    return 0;
  }
}

static u8_t lwip_ioctl_internal_SIOCSIFFLAGS(const struct ifreq *ifr)
{
  struct netif *netif = NULL;

#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCSIFFLAGS: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif

  /* set netif hw addr */
  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  }
#if LWIP_HAVE_LOOPIF
  else if (netif->link_layer_type == LOOPBACK_IF) {
    return EPERM;
  }
#endif /* LWIP_HAVE_LOOPIF */
  else {
    if ((ifr->ifr_flags & IFF_UP) && !(netif->flags & NETIF_FLAG_UP)) {
      (void)netif_set_up(netif);
    } else if (!(ifr->ifr_flags & IFF_UP) && (netif->flags & NETIF_FLAG_UP)) {
      (void)netif_set_down(netif);
    }
    if ((ifr->ifr_flags & IFF_RUNNING) && !(netif->flags & NETIF_FLAG_LINK_UP)) {
      (void)netif_set_link_up(netif);
    } else if (!(ifr->ifr_flags & IFF_RUNNING) && (netif->flags & NETIF_FLAG_LINK_UP)) {
      (void)netif_set_link_down(netif);
    }

    if (ifr->ifr_flags & IFF_BROADCAST) {
      netif->flags |= NETIF_FLAG_BROADCAST;
    } else {
      netif->flags = netif->flags & (~NETIF_FLAG_BROADCAST);
    }
    if (ifr->ifr_flags & IFF_NOARP) {
      netif->flags = (netif->flags & (~NETIF_FLAG_ETHARP));
    } else {
      netif->flags |= NETIF_FLAG_ETHARP;
    }

    if (ifr->ifr_flags & IFF_MULTICAST) {
#if LWIP_IGMP
      netif->flags |= NETIF_FLAG_IGMP;
#endif /* LWIP_IGMP */
#if LWIP_IPV6 && LWIP_IPV6_MLD
      netif->flags |= NETIF_FLAG_MLD6;
#endif /* LWIP_IPV6_MLD */
    }
    else {
#if LWIP_IGMP
      netif->flags = (netif->flags &  ~NETIF_FLAG_IGMP);
#endif /* LWIP_IGMP */
#if LWIP_IPV6 && LWIP_IPV6_MLD
      netif->flags = (netif->flags &  ~NETIF_FLAG_MLD6);
#endif /* LWIP_IPV6_MLD */
    }

#if LWIP_DHCP
    if (ifr->ifr_flags & IFF_DYNAMIC) {
      (void)dhcp_start(netif);
    } else {
      dhcp_stop(netif);
#if !LWIP_DHCP_SUBSTITUTE
      dhcp_cleanup(netif);
#endif
    }
#endif

#if LWIP_NETIF_PROMISC
    if ((ifr->ifr_flags & IFF_PROMISC)) {
      netif_update_promiscuous_mode_status(netif, 1);
    } else {
      netif_update_promiscuous_mode_status(netif, 0);
    }
#endif /* LWIP_NETIF_PROMISC */
    return 0;
  }
}

static u8_t lwip_ioctl_internal_SIOCGIFFLAGS(struct ifreq *ifr)
{
  struct netif *netif = NULL;

  /* set netif hw addr */
  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  } else {
    if (netif->flags & NETIF_FLAG_UP) {
      ifr->ifr_flags |= IFF_UP;
    } else {
      ifr->ifr_flags &= ~IFF_UP;
    }
    if (netif->flags & NETIF_FLAG_LINK_UP) {
      ifr->ifr_flags |= IFF_RUNNING;
    } else {
      ifr->ifr_flags &= ~IFF_RUNNING;
    }
    if (netif->flags & NETIF_FLAG_BROADCAST) {
      ifr->ifr_flags |= IFF_BROADCAST;
    } else {
      ifr->ifr_flags &= ~IFF_BROADCAST;
    }
    if (netif->flags & NETIF_FLAG_ETHARP) {
      ifr->ifr_flags &= ~IFF_NOARP;
    } else {
      ifr->ifr_flags |= IFF_NOARP;
    }

#if LWIP_IGMP || LWIP_IPV6_MLD
    if (
#if LWIP_IGMP
      (netif->flags & NETIF_FLAG_IGMP)
#endif /* LWIP_IGMP */
#if LWIP_IGMP && LWIP_IPV6_MLD
      ||
#endif /* LWIP_IGMP && LWIP_IPV6_MLD */
#if LWIP_IPV6_MLD
      (netif->flags & NETIF_FLAG_MLD6)
#endif /* LWIP_IPV6_MLD */
        ) {
      ifr->ifr_flags = (short)((unsigned short)ifr->ifr_flags | IFF_MULTICAST);
    } else {
      ifr->ifr_flags = (short)((unsigned short)ifr->ifr_flags & (~IFF_MULTICAST));
    }
#endif /* LWIP_IGMP || LWIP_IPV6_MLD */

#if LWIP_DHCP
    if (netif->flags & NETIF_FLAG_DHCP) {
      ifr->ifr_flags = (short)((unsigned short)ifr->ifr_flags | IFF_DYNAMIC);
    } else {
      ifr->ifr_flags = (short)((unsigned short)ifr->ifr_flags & (~IFF_DYNAMIC));
    }
#endif

#if LWIP_HAVE_LOOPIF
    if (netif->link_layer_type == LOOPBACK_IF) {
      ifr->ifr_flags |= IFF_LOOPBACK;
    }
#endif

#if LWIP_NETIF_PROMISC
    if (atomic_read(&(netif->flags_ext)) == NETIF_FLAG_PROMISC) {
      ifr->ifr_flags |= IFF_PROMISC;
    } else {
      ifr->ifr_flags &= ~IFF_PROMISC;
    }
#endif /* LWIP_NETIF_PROMISC */

    return 0;
  }
}

static u8_t lwip_ioctl_internal_SIOCGIFNAME(struct ifreq *ifr)
{
  struct netif *netif = NULL;
  int ret;

  for (netif = netif_list; netif != NULL; netif = netif->next) {
    if (ifr->ifr_ifindex == netif->ifindex) {
      break;
    }
  }

  if (netif == NULL) {
    return ENODEV;
  } else {
    if (netif->link_layer_type == LOOPBACK_IF) {
      ret = snprintf_s(ifr->ifr_name, NETIF_NAMESIZE, (NETIF_NAMESIZE - 1), "%s", netif->name);
      if ((ret <= 0) || (ret >= NETIF_NAMESIZE)) {
        return ENOBUFS;
      }
    } else {
      ret = snprintf_s(ifr->ifr_name, NETIF_NAMESIZE, (NETIF_NAMESIZE - 1), "%s%"U8_F, netif->name, netif->num);
      if ((ret <= 0) || (ret >= NETIF_NAMESIZE)) {
        return ENOBUFS;
      }
    }
    return 0;
  }
}

static u8_t lwip_validate_ifname(const char *name, u8_t *let_pos)
{
  unsigned short num_pos = 0;
  unsigned short letter_pos = 0;
  unsigned short pos = 0;
  u8_t have_num = 0;

  /* if the first position of variable name is not letter, such as '6eth2' */
  if (!((*name >= 'a' && *name <= 'z') || (*name >= 'A' && *name <= 'Z'))) {
    return 0;
  }

  /* check if the position of letter is bigger than the the position of digital */
  while (*name != '\0') {
    if (*name >= '0' && *name <= '9') {
      num_pos = pos;
      have_num = 1;
    } else if ((*name >= 'a' && *name <= 'z') || (*name >= 'A' && *name <= 'Z')) {
      letter_pos = pos;
      if (have_num) {
        return 0;
      }
    } else {
      return 0;
    }
    pos++;
    name++;
  }

  /* for the speacil case as all position of variable name is letter, such as 'ethabc' */
  if (num_pos == 0) {
      return 0;
  }

  /* cheak if the digital in the variable name is bigger than 255, such as 'eth266' */
  if (atoi(name - (pos - letter_pos - 1)) > 255) {
    return 0;
  }

  *let_pos = (u8_t)letter_pos;

  return 1;
}

static u8_t lwip_ioctl_internal_SIOCSIFNAME(struct ifreq *ifr)
{
  struct netif *netif = NULL;
  char name[NETIF_NAMESIZE];
  s32_t num = 0;
  u8_t letter_pos = 0;

#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCSIFNAME: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif

  netif = netif_find(ifr->ifr_name);

  if (netif == NULL) {
    return  ENODEV;
  } else if (netif->link_layer_type == LOOPBACK_IF) {
    return EPERM;
  } else if (netif->flags & IFF_UP) {
    return EBUSY;
  } else {
    ifr->ifr_newname[NETIF_NAMESIZE - 1] = '\0';
    if (!lwip_validate_ifname(ifr->ifr_newname, &letter_pos) || strlen(ifr->ifr_newname) > (NETIF_NAMESIZE - 1)) {
      return EINVAL;
    }

    (void)memset_s(name, NETIF_NAMESIZE, 0, NETIF_NAMESIZE);
    (void)strncpy_s(name, NETIF_NAMESIZE, ifr->ifr_newname, strlen(ifr->ifr_newname));

    num = atoi(name + letter_pos + 1);
    if (num > 0xFF || num < 0) {
      return EINVAL;
    }

    name[letter_pos + 1] = '\0';
    if (netif_check_num_isusing(name, (u8_t)num) == 1) {
      return EINVAL;
    }

    if (strncpy_s(netif->name, sizeof(netif->name), name, strlen(name)) != EOK) {
      return EINVAL;
    }
    netif->name[strlen(name)] = '\0';
    netif->num = (u8_t)num;
  }

  return 0;
}

static u8_t lwip_ioctl_internal_SIOCGIFINDEX(struct ifreq *ifr)
{
  struct netif *netif = NULL;

  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  } else {
    ifr->ifr_ifindex = netif->ifindex;
    return 0;
  }
}

static u8_t lwip_ioctl_internal_SIOCSIFMTU(const struct ifreq *ifr)
{
  struct netif *netif = NULL;

#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCSIFMTU: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif

  /* set netif hw addr */
  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  }
#if LWIP_HAVE_LOOPIF
  /* the mtu of loopif is not used. */
  else if (netif->link_layer_type == LOOPBACK_IF) {
    return EPERM;
  }
#endif
  else {
    if (ERR_OK != netif_set_mtu(netif, (u16_t)ifr->ifr_mtu)) {
      return EINVAL;
    }

    return 0;
  }
}

static u8_t lwip_ioctl_internal_SIOCGIFMTU(struct ifreq *ifr)
{
  struct netif *netif = NULL;

  /* get netif hw addr */
  netif = netif_find(ifr->ifr_name);

  if (netif == NULL) {
    return ENODEV;
  } else {
    ifr->ifr_mtu = netif->mtu;
    return 0;
  }
}
#endif /* LWIP_IOCTL_IF */

#if LWIP_NETIF_ETHTOOL
static s32_t lwip_ioctl_internal_SIOCETHTOOL(struct ifreq *ifr)
{
  struct netif *netif = NULL;

#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCETHTOOL: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif

  netif = netif_find(ifr->ifr_name);
  if (netif == NULL) {
    return ENODEV;
  } else {
    return dev_ethtool(netif, ifr);
  }
}
#endif

#if LWIP_SO_RCVBUF || LWIP_FIONREAD_LINUXMODE
static u8_t lwip_ioctl_internal_FIONREAD(struct lwip_sock *sock, void *argp)
{
#if LWIP_SO_RCVBUF
  int recv_avail;
#endif
#if LWIP_FIONREAD_LINUXMODE
  SYS_ARCH_DECL_PROTECT(lev);
#endif

  if (!argp) {
    return EINVAL;
  }

  lwip_sock_lock(sock);
#if LWIP_FIONREAD_LINUXMODE
  if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) != NETCONN_TCP) {
    struct netbuf *nb;
    if (sock->lastdata.netbuf) {
      nb = sock->lastdata.netbuf;
      *((int *)argp) = nb->p->tot_len;
    } else {
      struct netbuf *rxbuf;
      err_t err;

      SYS_ARCH_PROTECT(lev);
      if (sock->rcvevent <= 0) {
        *((int*)argp) = 0;
        SYS_ARCH_UNPROTECT(lev);
      } else {
        SYS_ARCH_UNPROTECT(lev);
        err = netconn_recv_udp_raw_netbuf_flags(sock->conn, &rxbuf, NETCONN_DONTBLOCK);
        if (err != ERR_OK) {
          *((int *)argp) = 0;
        } else {
          sock->lastdata.netbuf = rxbuf;
          *((int *)argp) = rxbuf->p->tot_len;
        }
      }
    }

    lwip_sock_unlock(sock);
    return 0;
  }
#endif /* LWIP_FIONREAD_LINUXMODE */

#if LWIP_SO_RCVBUF
  /* we come here if either LWIP_FIONREAD_LINUXMODE==0 or this is a TCP socket */
  SYS_ARCH_GET(sock->conn->recv_avail, recv_avail);
  if (recv_avail < 0) {
    recv_avail = 0;
  }

  /* Check if there is data left from the last recv operation. /maq 041215 */
  if (sock->lastdata.netbuf) {
    if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) == NETCONN_TCP) {
      recv_avail += sock->lastdata.pbuf->tot_len;
    } else {
      recv_avail += sock->lastdata.netbuf->p->tot_len;
    }
  }
  *((int *)argp) = recv_avail;

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_ioctl(FIONREAD, %p) = %"U16_F"\n", argp, *((u16_t *)argp)));
  lwip_sock_unlock(sock);
  return 0;
#else /* LWIP_SO_RCVBUF */
  lwip_sock_unlock(sock);
  return ENOSYS;
#endif /* LWIP_SO_RCVBUF */
}
#endif /* LWIP_SO_RCVBUF || LWIP_FIONREAD_LINUXMODE */

static u8_t lwip_ioctl_internal_FIONBIO(struct lwip_sock *sock, const void *argp)
{
  u8_t val = 0;
  SYS_ARCH_DECL_PROTECT(lev);
  if (argp == NULL) {
    return EINVAL;
  }
  if (*(int *)argp) {
    val = 1;
  }
  SYS_ARCH_PROTECT(lev);
  netconn_set_nonblocking(sock->conn, val);
  SYS_ARCH_UNPROTECT(lev);
  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_ioctl(FIONBIO, %d)\n", val));
  return 0;
}

#if LWIP_IPV6
#if LWIP_IPV6_DUP_DETECT_ATTEMPTS
static u8_t lwip_ioctl_internal_SIOCSIPV6DAD(const struct ifreq *ifr)
{
#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCSIPV6DAD: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif
  struct netif *tmpnetif = netif_find(ifr->ifr_name);
  if (tmpnetif == NULL) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("Interface not found.\n"));
    return ENODEV;
  }

  if ((ifr->ifr_ifru.ifru_ivalue != 0) && (ifr->ifr_ifru.ifru_ivalue != 1)) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("Invalid ioctl argument(ifru_ivalue 0/1).\n"));
    return EBADRQC;
  }

  if (ifr->ifr_ifru.ifru_ivalue == 1) {
    tmpnetif->ipv6_flags = (tmpnetif->ipv6_flags | LWIP_IPV6_ND6_FLAG_DAD);

    LWIP_DEBUGF(SOCKETS_DEBUG, ("DAD turned on through ioctl for  %s iface index %u \n",
      tmpnetif->name, tmpnetif->num));
  } else {
    tmpnetif->ipv6_flags = (tmpnetif->ipv6_flags & ((~LWIP_IPV6_ND6_FLAG_DAD) & 0xffU));

    LWIP_DEBUGF(SOCKETS_DEBUG, ("DAD turned off through ioctl for  %s iface index %u \n",
      tmpnetif->name, tmpnetif->num));
  }
  return 0;
}

static u8_t lwip_ioctl_internal_SIOCGIPV6DAD(struct ifreq *ifr)
{
  struct netif *tmpnetif = netif_find(ifr->ifr_name);
  if (tmpnetif == NULL) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("Interface not found.\n"));
    return ENODEV;
  }
  ifr->ifr_ifru.ifru_ivalue = (tmpnetif->ipv6_flags & LWIP_IPV6_ND6_FLAG_DAD) ? 1 : 0;
  return 0;
}
#endif

#if LWIP_IOCTL_IPV6DPCTD
static u8_t lwip_ioctl_internal_SIOCSIPV6DPCTD(const struct ifreq *ifr)
{
#if LWIP_ENABLE_NET_CAPABILITY
  LWIP_ERROR("lwip_ioctl_internal_SIOCSIPV6DPCTD: Have no permission CAP_NET_ADMIN",
             IsCapPermit(CAP_NET_ADMIN), return EPERM);
#endif
  struct netif *tmpnetif = netif_find(ifr->ifr_name);
  if (tmpnetif == NULL) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("Interface not found.\n"));
    return ENODEV;
  }
  if ((ifr->ifr_ifru.ifru_ivalue != 0) && (ifr->ifr_ifru.ifru_ivalue != 1)) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("Invalid ioctl argument(ifru_ivalue 0/1).\n"));
    return EBADRQC;
  }
  if (ifr->ifr_ifru.ifru_ivalue == 1) {
    tmpnetif->ipv6_flags = (tmpnetif->ipv6_flags | LWIP_IPV6_ND6_FLAG_DEPRECATED);
    LWIP_DEBUGF(SOCKETS_DEBUG, ("Deprecation turned on through ioctl for  %s iface index %u \n",
      tmpnetif->name, tmpnetif->num));
  } else {
    tmpnetif->ipv6_flags = (tmpnetif->ipv6_flags & ((~LWIP_IPV6_ND6_FLAG_DEPRECATED) & 0xffU));
    LWIP_DEBUGF(SOCKETS_DEBUG, ("Deprecation turned off through ioctl for  %s iface index %u \n",
      tmpnetif->name, tmpnetif->num));
  }
  return 0;
}

static u8_t lwip_ioctl_internal_SIOCGIPV6DPCTD(struct ifreq *ifr)
{
  struct netif *tmpnetif = netif_find(ifr->ifr_name);
  if (tmpnetif == NULL) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("Interface not found.\n"));
    return ENODEV;
  }

  ifr->ifr_ifru.ifru_ivalue = (tmpnetif->ipv6_flags & LWIP_IPV6_ND6_FLAG_DEPRECATED) ? 1 : 0;
  return 0;
}
#endif /* LWIP_IOCTL_IPV6DPCTD */
#endif
static u8_t lwip_ioctl_impl(struct lwip_sock *sock, long cmd, void *argp)
{
  u8_t err = 0;
#if LWIP_NETIF_ETHTOOL
  s32_t ret;
#endif
#if LWIP_IPV6_DUP_DETECT_ATTEMPTS || LWIP_IOCTL_IPV6DPCTD || LWIP_IOCTL_IF || LWIP_NETIF_ETHTOOL
  struct ifreq *ifr = (struct ifreq *)argp;
#endif
#if LWIP_IOCTL_ROUTE
  struct rtentry *rmten = (struct rtentry *)argp;
#endif
#if LWIP_IPV6_DUP_DETECT_ATTEMPTS || LWIP_IOCTL_IPV6DPCTD || LWIP_IOCTL_ROUTE || LWIP_IOCTL_IF
  u8_t is_ipv6 = NETCONNTYPE_ISIPV6(sock->conn->type);
#endif

  LWIP_ASSERT("no socket given", sock != NULL);

  switch (cmd) {
#if LWIP_IPV6
#if LWIP_IPV6_DUP_DETECT_ATTEMPTS
    case SIOCSIPV6DAD:
      /* allow it only on IPv6 sockets... */
      if (!is_ipv6) {
        err = EINVAL;
      } else {
        err = lwip_ioctl_internal_SIOCSIPV6DAD(ifr);
      }
      break;
    case SIOCGIPV6DAD:
      /* allow it only on IPv6 sockets... */
      if (!is_ipv6) {
        err = EINVAL;
      } else {
        err = lwip_ioctl_internal_SIOCGIPV6DAD(ifr);
      }
      break;
#endif /* LWIP_IPV6_DUP_DETECT_ATTEMPTS */
#if LWIP_IOCTL_IPV6DPCTD
    case SIOCSIPV6DPCTD:
      /* allow it only on IPv6 sockets... */
      if (!is_ipv6) {
        err = EINVAL;
      } else {
        err = lwip_ioctl_internal_SIOCSIPV6DPCTD(ifr);
      }
      break;
    case SIOCGIPV6DPCTD:
      /* allow it only on IPv6 sockets... */
      if (!is_ipv6) {
        err = EINVAL;
      } else {
        err = lwip_ioctl_internal_SIOCGIPV6DPCTD(ifr);
      }
      break;
#endif /* LWIP_IOCTL_IPV6DPCTD */
#endif /* LWIP_IPV6 */
#if LWIP_IOCTL_ROUTE
    case SIOCADDRT:
      /* Do not allow if socket is AF_INET6 */
      if (is_ipv6) {
        err = EINVAL;
      } else {
        err = lwip_ioctl_internal_SIOCADDRT(rmten);
      }
      break;
#endif
#if LWIP_IOCTL_IF
    case SIOCGIFCONF:
      /* Do not allow if socket is AF_INET6 */
      if (is_ipv6) {
        err = EINVAL;
      } else {
        err = lwip_ioctl_internal_SIOCGIFCONF(ifr);
      }
      break;
    case SIOCGIFADDR:
      if (is_ipv6) {
        err = EINVAL;
      } else {
        err = lwip_ioctl_internal_SIOCGIFADDR(ifr);
      }
      break;
    case SIOCSIFADDR:
#if LWIP_IPV6
      if (is_ipv6) {
        err = lwip_ioctl_internal_SIOCSIFADDR_6(ifr);
      }
      else
#endif
      {
        err = lwip_ioctl_internal_SIOCSIFADDR(ifr);
      }
      break;
    case SIOCDIFADDR:
      /* Delete interface address */
#if LWIP_IPV6
      if (is_ipv6) {
        err = lwip_ioctl_internal_SIOCDIFADDR_6(ifr);
      }
      else
#endif
      {
        err = lwip_ioctl_internal_SIOCDIFADDR(ifr);
      }
      break;
    case SIOCGIFBRDADDR:
      if (is_ipv6) {
        err = EINVAL;
      } else {
        err = lwip_ioctl_internal_SIOCGIFBRDADDR(ifr);
      }
      break;
    case SIOCGIFNETMASK:
      if (is_ipv6) {
        err = EINVAL;
      } else {
        err = lwip_ioctl_internal_SIOCGIFNETMASK(ifr);
      }
      break;
    case SIOCSIFNETMASK:
      if (is_ipv6) {
        err = EINVAL;
      } else {
        err = lwip_ioctl_internal_SIOCSIFNETMASK(ifr);
      }
      break;
    case SIOCSIFHWADDR:
      err = lwip_ioctl_internal_SIOCSIFHWADDR(ifr);
      break;
    case SIOCGIFHWADDR:
      err = lwip_ioctl_internal_SIOCGIFHWADDR(ifr);
      break;
    case SIOCSIFFLAGS:
      err = lwip_ioctl_internal_SIOCSIFFLAGS(ifr);
      break;
    case SIOCGIFFLAGS:
      err = lwip_ioctl_internal_SIOCGIFFLAGS(ifr);
      break;
    case SIOCGIFNAME:
      err = lwip_ioctl_internal_SIOCGIFNAME(ifr);
      break;
    case SIOCSIFNAME:
      err = lwip_ioctl_internal_SIOCSIFNAME(ifr);
      break;
    /* Need to support the get index through ioctl
     * As of now the options is restricted to PF_PACKET scenario , so removed the compiler flag Begin
     */
    case SIOCGIFINDEX:
      err = lwip_ioctl_internal_SIOCGIFINDEX(ifr);
      break;
    case SIOCGIFMTU:
      err = lwip_ioctl_internal_SIOCGIFMTU(ifr);
      break;
    case SIOCSIFMTU:
      err = lwip_ioctl_internal_SIOCSIFMTU(ifr);
      break;
#endif /* LWIP_IOCTL_IF */

#if LWIP_NETIF_ETHTOOL
    case SIOCETHTOOL:
      ret = lwip_ioctl_internal_SIOCETHTOOL(ifr);
      if (ret != 0) {
        /* an IO error happened */
        err = EIO;
      }
      break;
#endif

#if LWIP_SO_RCVBUF || LWIP_FIONREAD_LINUXMODE
    case FIONREAD:
      err = lwip_ioctl_internal_FIONREAD(sock, argp);
      break;
#endif /* LWIP_SO_RCVBUF || LWIP_FIONREAD_LINUXMODE */

    case FIONBIO:
      err = lwip_ioctl_internal_FIONBIO(sock, argp);
      break;
    /* -1 should return EINVAL */
    case -1:
      err = EINVAL;
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_ioctl_impl(INVALID: 0x%lx)\n", cmd));
      break;
    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_ioctl(UNIMPL: 0x%lx, %p)\n", cmd, argp));
      err = ENOSYS; /* not yet implemented */
      break;
  } /* switch (cmd) */

  return err;
}

int
lwip_ioctl(int s, long cmd, void *argp)
{
  u8_t err;
  struct lwip_sock *sock = get_socket(s);
  if (!sock) {
    /* get_socket has updated errno */
    return -1;
  }
  if (argp == NULL) {
    sock_set_errno(sock, EFAULT);
    done_socket(sock);
    return -1;
  }

  LOCK_TCPIP_CORE();
  err = lwip_ioctl_impl(sock, cmd, argp);
  UNLOCK_TCPIP_CORE();
  if (err != ERR_OK) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_ioctl(%d, cmd: 0x%lx, %p)\n", s, cmd, argp));
    sock_set_errno(sock, err);
  }
  done_socket(sock);
  return (err == 0) ? 0 : -1;
}

/** A minimal implementation of fcntl.
 * Currently only the commands F_GETFL and F_SETFL are implemented.
 * The flag O_NONBLOCK and access modes are supported for F_GETFL, only
 * the flag O_NONBLOCK is implemented for F_SETFL.
 */
int
lwip_fcntl(int s, int cmd, int val)
{
  struct lwip_sock *sock = get_socket(s);
  int ret = -1;
  int op_mode = 0;

  if (!sock) {
    return -1;
  }

  switch (cmd) {
    case F_GETFL:
      ret = netconn_is_nonblocking(sock->conn) ? O_NONBLOCK : 0;
      sock_set_errno(sock, 0);

      if (NETCONNTYPE_GROUP(NETCONN_TYPE(sock->conn)) == NETCONN_TCP) {
#if LWIP_TCPIP_CORE_LOCKING
        LOCK_TCPIP_CORE();
#else
        SYS_ARCH_DECL_PROTECT(lev);
        /* the proper thing to do here would be to get into the tcpip_thread,
           but locking should be OK as well since we only *read* some flags */
        SYS_ARCH_PROTECT(lev);
#endif
#if LWIP_TCP
        if (sock->conn->pcb.tcp) {
          if (!(sock->conn->pcb.tcp->flags & TF_RXCLOSED)) {
            op_mode |= O_RDONLY;
          }
          if (!(sock->conn->pcb.tcp->flags & TF_FIN)) {
            op_mode |= O_WRONLY;
          }
        }
#endif
#if LWIP_TCPIP_CORE_LOCKING
        UNLOCK_TCPIP_CORE();
#else
        SYS_ARCH_UNPROTECT(lev);
#endif
      } else {
        op_mode |= O_RDWR;
      }

      /* ensure O_RDWR for (O_RDONLY|O_WRONLY) != O_RDWR cases */
      ret |= (op_mode == (O_RDONLY | O_WRONLY)) ? O_RDWR : op_mode;

      break;
    case F_SETFL:
      if (val < 0) {
        set_errno(EINVAL);
        done_socket(sock);
        return -1;
      }
      /* Bits corresponding to the file access mode and the file creation flags [..] that are set in arg shall be ignored */
      val &= ~(O_RDONLY | O_WRONLY | O_RDWR);
      if ((val & ~O_NONBLOCK) == 0) {
        /* only O_NONBLOCK, all other bits are zero */
        netconn_set_nonblocking(sock->conn, val & O_NONBLOCK);
        ret = 0;
        sock_set_errno(sock, 0);
      } else {
        sock_set_errno(sock, ENOSYS); /* not yet implemented */
      }
      break;
    default:
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_fcntl(%d, UNIMPL: %d, %d)\n", s, cmd, val));
      sock_set_errno(sock, ENOSYS); /* not yet implemented */
      break;
  }
  done_socket(sock);
  return ret;
}

#if LWIP_COMPAT_SOCKETS == 2 && LWIP_POSIX_SOCKETS_IO_NAMES
int
fcntl(int s, int cmd, ...)
{
  va_list ap;
  int val;

  va_start(ap, cmd);
  val = va_arg(ap, int);
  va_end(ap);
  return lwip_fcntl(s, cmd, val);
}
#endif

const char *
lwip_inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
  const char *ret = NULL;
  int size_int = (int)size;
  if ((src == NULL) || (dst == NULL)) {
    set_errno(EINVAL);
    return NULL;
  }
  if (size_int < 0) {
    set_errno(ENOSPC);
    return NULL;
  }
  switch (af) {
#if LWIP_IPV4
    case AF_INET:
      ret = ip4addr_ntoa_r((const ip4_addr_t *)src, dst, size_int);
      if (ret == NULL) {
        set_errno(ENOSPC);
      }
      break;
#endif
#if LWIP_IPV6
    case AF_INET6:
      ret = ip6addr_ntoa_r((const ip6_addr_t *)src, dst, size_int);
      if (ret == NULL) {
        set_errno(ENOSPC);
      }
      break;
#endif
    default:
      set_errno(EAFNOSUPPORT);
      break;
  }
  return ret;
}

int
lwip_inet_pton(int af, const char *src, void *dst)
{
  int err;
  if ((src == NULL) || (dst == NULL)) {
    set_errno(EINVAL);
    return 0;
  }
  switch (af) {
#if LWIP_IPV4
    case AF_INET:
      err = ip4addr_aton(src, (ip4_addr_t *)dst);
      break;
#endif
#if LWIP_IPV6
    case AF_INET6: {
      /* convert into temporary variable since ip6_addr_t might be larger
         than in6_addr when scopes are enabled */
      ip6_addr_t addr;
      err = ip6addr_aton(src, &addr);
      if (err) {
        memcpy(dst, &addr.addr, sizeof(addr.addr));
      }
      break;
    }
#endif
    default:
      err = -1;
      set_errno(EAFNOSUPPORT);
      break;
  }
  return err;
}

#if LWIP_IGMP
/** Register a new IGMP membership. On socket close, the membership is dropped automatically.
 *
 * ATTENTION: this function is called from tcpip_thread (or under CORE_LOCK).
 *
 * @return 1 on success, 0 on failure
 */
static int
lwip_socket_register_membership(int s, const ip4_addr_t *if_addr, const ip4_addr_t *multi_addr)
{
  struct lwip_sock *sock = get_socket(s);
  u32_t i;
  SYS_ARCH_DECL_PROTECT(lev);

  if (!sock) {
    return 0;
  }

  SYS_ARCH_PROTECT(lev);
#if LWIP_ALLOW_SOCKET_CONFIG
  if (socket_ipv4_multicast_memberships == NULL) {
    socket_ipv4_multicast_memberships =
        mem_malloc(sizeof(struct lwip_socket_multicast_pair) * LWIP_SOCKET_MAX_MEMBERSHIPS);
    if (socket_ipv4_multicast_memberships == NULL) {
      SYS_ARCH_UNPROTECT(lev);
      done_socket(sock);
      return 0;
    }
    (void)memset(socket_ipv4_multicast_memberships,
                 0,
                 sizeof(struct lwip_socket_multicast_pair) * LWIP_SOCKET_MAX_MEMBERSHIPS);
  }
#endif

  for (i = 0; i < LWIP_SOCKET_MAX_MEMBERSHIPS; i++) {
    if (socket_ipv4_multicast_memberships[i].sock == NULL) {
      socket_ipv4_multicast_memberships[i].sock = sock;
      ip4_addr_copy(socket_ipv4_multicast_memberships[i].if_addr, *if_addr);
      ip4_addr_copy(socket_ipv4_multicast_memberships[i].multi_addr, *multi_addr);
      SYS_ARCH_UNPROTECT(lev);
      done_socket(sock);
      return 1;
    }
  }
  SYS_ARCH_UNPROTECT(lev);
  done_socket(sock);
  return 0;
}

/** Unregister a previously registered membership. This prevents dropping the membership
 * on socket close.
 *
 * ATTENTION: this function is called from tcpip_thread (or under CORE_LOCK).
 */
static void
lwip_socket_unregister_membership(int s, const ip4_addr_t *if_addr, const ip4_addr_t *multi_addr)
{
  struct lwip_sock *sock = get_socket(s);
  u32_t i;
  SYS_ARCH_DECL_PROTECT(lev);

  if (!sock) {
    return;
  }

  SYS_ARCH_PROTECT(lev);
#if LWIP_ALLOW_SOCKET_CONFIG
  if (socket_ipv4_multicast_memberships == NULL) {
    SYS_ARCH_UNPROTECT(lev);
    done_socket(sock);
    return;
  }
#endif

  for (i = 0; i < LWIP_SOCKET_MAX_MEMBERSHIPS; i++) {
    if ((socket_ipv4_multicast_memberships[i].sock == sock) &&
        ip4_addr_cmp(&socket_ipv4_multicast_memberships[i].if_addr, if_addr) &&
        ip4_addr_cmp(&socket_ipv4_multicast_memberships[i].multi_addr, multi_addr)) {
      socket_ipv4_multicast_memberships[i].sock = NULL;
      ip4_addr_set_zero(&socket_ipv4_multicast_memberships[i].if_addr);
      ip4_addr_set_zero(&socket_ipv4_multicast_memberships[i].multi_addr);
      break;
    }
  }
  SYS_ARCH_UNPROTECT(lev);
  done_socket(sock);
}

/** Drop all memberships of a socket that were not dropped explicitly via setsockopt.
 *
 * ATTENTION: this function is NOT called from tcpip_thread (or under CORE_LOCK).
 */
static void
lwip_socket_drop_registered_memberships(int s)
{
  struct lwip_sock *sock = get_socket(s);
  u32_t i;
  SYS_ARCH_DECL_PROTECT(lev);

  if (!sock) {
    return;
  }

  SYS_ARCH_PROTECT(lev);
#if LWIP_ALLOW_SOCKET_CONFIG
  if (socket_ipv4_multicast_memberships == NULL) {
    SYS_ARCH_UNPROTECT(lev);
    done_socket(sock);
    return;
  }
#endif
  SYS_ARCH_UNPROTECT(lev);
  for (i = 0; i < LWIP_SOCKET_MAX_MEMBERSHIPS; i++) {
    SYS_ARCH_PROTECT(lev);
    if (socket_ipv4_multicast_memberships[i].sock == sock) {
      ip_addr_t multi_addr, if_addr;
      ip_addr_copy_from_ip4(multi_addr, socket_ipv4_multicast_memberships[i].multi_addr);
      ip_addr_copy_from_ip4(if_addr, socket_ipv4_multicast_memberships[i].if_addr);
      socket_ipv4_multicast_memberships[i].sock = NULL;
      ip4_addr_set_zero(&socket_ipv4_multicast_memberships[i].if_addr);
      ip4_addr_set_zero(&socket_ipv4_multicast_memberships[i].multi_addr);

      SYS_ARCH_UNPROTECT(lev);
      (void)netconn_leave_group(sock->conn, &multi_addr, &if_addr, NETCONN_LEAVE);
    } else {
      SYS_ARCH_UNPROTECT(lev);
    }
  }
  done_socket(sock);
}
#endif /* LWIP_IGMP */

#ifdef LWIP_DEBUG_INFO
void debug_socket_info(int idx, int filter, int expend)
{
  struct lwip_sock *sock = NULL;
  if ((idx >= (int)LWIP_CONFIG_NUM_SOCKETS) || (idx < 0)) {
    LWIP_PLATFORM_PRINT("problem:idx >= LWIP_CONFIG_NUM_SOCKETS || idx < 0\n");
    return;
  }
  sock = get_socket(idx + LWIP_SOCKET_OFFSET);
  if (sock == NULL) {
    return;
  }
  if (filter) {
    LWIP_PLATFORM_PRINT("\n---------------SOCKET\n");
    LWIP_PLATFORM_PRINT("idx      :%d\n", idx);
    LWIP_PLATFORM_PRINT("conn     :%p\n", sockets[idx].conn);
    LWIP_PLATFORM_PRINT("r_event  :%d\n", sockets[idx].rcvevent);
    LWIP_PLATFORM_PRINT("s_event  :%d\n", sockets[idx].sendevent);
    LWIP_PLATFORM_PRINT("e_event  :%d\n", sockets[idx].errevent);
    LWIP_PLATFORM_PRINT("err      :%d\n", atomic_read(&sockets[idx].err));
#if LWIP_NETCONN_FULLDUPLEX
    LWIP_PLATFORM_PRINT("fd_used  :%d\n", sockets[idx].fd_used);
    LWIP_PLATFORM_PRINT("fd_used  :%d\n", sockets[idx].closing);
#endif
    LWIP_PLATFORM_PRINT("select_w :%d\n", sockets[idx].select_waiting);
    LWIP_PLATFORM_PRINT("last_offset :%d\n", sockets[idx].lastoffset);
#if LWIP_EXT_POLL_SUPPORT
#if defined(LWIP_ALIOS_COMPAT) && LWIP_ALIOS_COMPAT
    LWIP_PLATFORM_PRINT("poll_prev:%p\n",   sockets[idx].wq.poll_queue.prev);
    LWIP_PLATFORM_PRINT("poll_next:%p\n",   sockets[idx].wq.poll_queue.next);
#else /* LWIP_ALIOS_COMPAT */
    LWIP_PLATFORM_PRINT("poll_prev:%p\n",   sockets[idx].wq.poll_queue.pstPrev);
    LWIP_PLATFORM_PRINT("poll_next:%p\n",   sockets[idx].wq.poll_queue.pstNext);
#endif /* LWIP_ALIOS_COMPAT */
#endif /* LWIP_EXT_POLL_SUPPORT */
    if (expend) {
      debug_netconn_info((void*)sockets[idx].conn, expend);
    }
  }
  done_socket(sock);
}
#endif /* LWIP_DEBUG_INFO */

#if LWIP_IPV6 && LWIP_IPV6_MLD
/** Register a new MLD6 membership. On socket close, the membership is dropped automatically.
 *
 * ATTENTION: this function is called from tcpip_thread (or under CORE_LOCK).
 *
 * @return 1 on success, 0 on failure
 */
static int
lwip_socket_register_mld6_membership(int s, unsigned int if_idx, const ip6_addr_t *multi_addr)
{
  struct lwip_sock *sock = get_socket(s);
  u32_t i;
  SYS_ARCH_DECL_PROTECT(lev);

  if (!sock) {
    return 0;
  }

  SYS_ARCH_PROTECT(lev);
#if LWIP_ALLOW_SOCKET_CONFIG
  if (socket_ipv6_multicast_memberships == NULL) {
    socket_ipv6_multicast_memberships =
        mem_malloc(sizeof(struct lwip_socket_multicast_mld6_pair) * LWIP_SOCKET_MAX_MEMBERSHIPS);
    if (socket_ipv6_multicast_memberships == NULL) {
      SYS_ARCH_UNPROTECT(lev);
      done_socket(sock);
      return 0;
    }
    (void)memset(socket_ipv6_multicast_memberships, 0,
                 sizeof(struct lwip_socket_multicast_mld6_pair) * LWIP_SOCKET_MAX_MEMBERSHIPS);
  }
#endif

  for (i = 0; i < LWIP_SOCKET_MAX_MEMBERSHIPS; i++) {
    if (socket_ipv6_multicast_memberships[i].sock == NULL) {
      socket_ipv6_multicast_memberships[i].sock   = sock;
      socket_ipv6_multicast_memberships[i].if_idx = (u8_t)if_idx;
      ip6_addr_copy(socket_ipv6_multicast_memberships[i].multi_addr, *multi_addr);
      SYS_ARCH_UNPROTECT(lev);
      done_socket(sock);
      return 1;
    }
  }
  SYS_ARCH_UNPROTECT(lev);
  done_socket(sock);
  return 0;
}

/** Unregister a previously registered MLD6 membership. This prevents dropping the membership
 * on socket close.
 *
 * ATTENTION: this function is called from tcpip_thread (or under CORE_LOCK).
 */
static void
lwip_socket_unregister_mld6_membership(int s, unsigned int if_idx, const ip6_addr_t *multi_addr)
{
  struct lwip_sock *sock = get_socket(s);
  u32_t i;
  SYS_ARCH_DECL_PROTECT(lev);

  if (!sock) {
    return;
  }

  SYS_ARCH_PROTECT(lev);
#if LWIP_ALLOW_SOCKET_CONFIG
  if (socket_ipv6_multicast_memberships == NULL) {
    SYS_ARCH_UNPROTECT(lev);
    done_socket(sock);
    return;
  }
#endif

  for (i = 0; i < LWIP_SOCKET_MAX_MEMBERSHIPS; i++) {
    if ((socket_ipv6_multicast_memberships[i].sock   == sock) &&
        (socket_ipv6_multicast_memberships[i].if_idx == if_idx) &&
        ip6_addr_cmp(&socket_ipv6_multicast_memberships[i].multi_addr, multi_addr)) {
      socket_ipv6_multicast_memberships[i].sock   = NULL;
      socket_ipv6_multicast_memberships[i].if_idx = NETIF_NO_INDEX;
      ip6_addr_set_zero(&socket_ipv6_multicast_memberships[i].multi_addr);
      break;
    }
  }
  SYS_ARCH_UNPROTECT(lev);
  done_socket(sock);
}

/** Drop all MLD6 memberships of a socket that were not dropped explicitly via setsockopt.
 *
 * ATTENTION: this function is NOT called from tcpip_thread (or under CORE_LOCK).
 */
static void
lwip_socket_drop_registered_mld6_memberships(int s)
{
  struct lwip_sock *sock = get_socket(s);
  u32_t i;
  SYS_ARCH_DECL_PROTECT(lev);

  if (!sock) {
    return;
  }

  SYS_ARCH_PROTECT(lev);
#if LWIP_ALLOW_SOCKET_CONFIG
  if (socket_ipv6_multicast_memberships == NULL) {
    SYS_ARCH_UNPROTECT(lev);
    done_socket(sock);
    return;
  }
#endif
  SYS_ARCH_UNPROTECT(lev);

  for (i = 0; i < LWIP_SOCKET_MAX_MEMBERSHIPS; i++) {
    SYS_ARCH_PROTECT(lev);
    if (socket_ipv6_multicast_memberships[i].sock == sock) {
      ip_addr_t multi_addr;
      u8_t if_idx;

      ip_addr_copy_from_ip6(multi_addr, socket_ipv6_multicast_memberships[i].multi_addr);
      if_idx = socket_ipv6_multicast_memberships[i].if_idx;

      socket_ipv6_multicast_memberships[i].sock   = NULL;
      socket_ipv6_multicast_memberships[i].if_idx = NETIF_NO_INDEX;
      ip6_addr_set_zero(&socket_ipv6_multicast_memberships[i].multi_addr);
      SYS_ARCH_UNPROTECT(lev);

      (void)netconn_leave_group_netif(sock->conn, &multi_addr, if_idx, NETCONN_LEAVE);
    } else {
      SYS_ARCH_UNPROTECT(lev);
    }
  }
  done_socket(sock);
}
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

/* Get dst_mac TCP / IP connections, src_ip, dst_ip, ipid, srcport, dstport,
 * Seq, ack, tcpwin, tsval, tsecr information
 *
 * @param s socket descriptor for which the details are required
 * @param conn parameter in which the acquired TCP / IP connection information is to be saved
 *
 * @note If the UDP connection, seq, ack, tcpwin, tsval, tsecr is set to 0
 * @return 0 if successful; failure to return -1
 */

int lwip_get_conn_info(int s, void *conninfo)
{
  struct lwip_sock *isock;
  err_t err;
  int retval = 0;

  isock = get_socket(s);
  if (isock == NULL) {
    set_errno(EBADF);
    return -1;
  }

  err = netconn_getconninfo(isock->conn, conninfo);
  if (err != ERR_OK) {
    retval = -1;
    goto RETURN;
  }

RETURN:
  if (err == ERR_VAL) {
      set_errno(EOPNOTSUPP);
  } else {
    set_errno(err_to_errno(err));
  }
  done_socket(isock);
  return retval;
}

#ifdef LWIP_LITEOS_A_COMPAT
#include "los_vm_map.h"
#include "user_copy.h"
#include "los_strnlen_user.h"
#include "lwip/netifapi.h"

#define DHCP_START 0xFFFFFFFDUL
#define DHCP_STOP  0xFFFFFFFEUL

static int
do_ioctl_SIOCGIFCONF(int sockfd, long cmd, void *argp)
{
  int nbytes;
  struct ifconf ifc;
  char *buf_bak = NULL;
  int ret;

  if (LOS_ArchCopyFromUser(&ifc, argp, sizeof(struct ifconf)) != 0) {
    set_errno(EFAULT);
    return -1;
  }
  if (ifc.ifc_len < 0) {
    set_errno(EINVAL);
    return -1;
  }
  if (ifc.ifc_len > LWIP_NETIF_NUM_MAX * sizeof(struct ifreq)) {
    LWIP_DEBUGF(SOCKETS_DEBUG, ("do_ioctl_SIOCGIFCONF: ifconf.ifc_len is out of limit!"));
    ifc.ifc_len = LWIP_NETIF_NUM_MAX * sizeof(struct ifreq);
  }
  nbytes = ifc.ifc_len;
  buf_bak = ifc.ifc_buf;
  ifc.ifc_buf = malloc(nbytes);
  if (ifc.ifc_buf == NULL) {
    set_errno(ENOMEM);
    return -1;
  }
  (void)memset_s(ifc.ifc_buf, nbytes, 0, nbytes);

  ret = lwip_ioctl(sockfd, cmd, &ifc);
  if (ret == 0) {
    if (LOS_IsUserAddress((VADDR_T)(uintptr_t)buf_bak)) {
      if (LOS_ArchCopyToUser(buf_bak, ifc.ifc_buf, nbytes) != 0) {
        set_errno(EFAULT);
        ret = -1;
      }
    } else {
      set_errno(EFAULT);
      ret = -1;
    }
  }

  free(ifc.ifc_buf);
  ifc.ifc_buf = buf_bak;
  if (LOS_ArchCopyToUser(argp, &ifc, sizeof(struct ifconf)) != 0) {
    set_errno(EFAULT);
    ret = -1;
  }
  return ret;
}

static size_t
get_argp_size(int sockfd, long cmd)
{
  switch (cmd) {
    case FIONREAD:
    case FIONBIO:
      return sizeof(int);
    case SIOCADDRT:
      return sizeof(struct rtentry);
    case SIOCSIFADDR:
    case SIOCDIFADDR:
      size_t nbytes;
#if LWIP_IPV6
      struct lwip_sock *sock = get_socket(sockfd);
      if (!sock) {
        return 0;
      }
      u8_t is_ipv6 = NETCONNTYPE_ISIPV6(sock->conn->type);
      done_socket(sock);
      if (is_ipv6) {
        nbytes = sizeof(struct in6_ifreq);
      } else
#endif
      {
        nbytes = sizeof(struct ifreq);
      }
      return nbytes;
#if LWIP_IPV6
    case SIOCSIPV6DAD:
    case SIOCGIPV6DAD:
    case SIOCSIPV6DPCTD:
    case SIOCGIPV6DPCTD:
#endif
    case SIOCGIFADDR:
    case SIOCGIFBRDADDR:
    case SIOCGIFNETMASK:
    case SIOCSIFNETMASK:
    case SIOCSIFHWADDR:
    case SIOCGIFHWADDR:
    case SIOCSIFFLAGS:
    case SIOCGIFFLAGS:
    case SIOCGIFNAME:
    case SIOCSIFNAME:
    case SIOCGIFINDEX:
    case SIOCGIFMTU:
    case SIOCSIFMTU:
    case SIOCETHTOOL:
      return sizeof(struct ifreq);
    default:
      return 0;
  }
}

static int
do_ioctl_netifapi_dhcp(struct lwip_sock *sock, unsigned long cmd, struct netif *netif)
{
  int ret = -1;
  err_t err;
  switch (cmd) {
    case DHCP_START:
      err = netifapi_dhcp_start(netif);
      if (err != ERR_OK) {
        sock_set_errno(sock, err_to_errno(err));
        break;
      }
      ret = 0;
      break;
    case DHCP_STOP:
      err = netifapi_dhcp_stop(netif);
      if (err != ERR_OK) {
        sock_set_errno(sock, err_to_errno(err));
        break;
      }
      ret = 0;
      break;
    default:
      sock_set_errno(sock, EINVAL);
      break;
  }
  return ret;
}

static int
do_ioctl_dhcp(int sockfd, long cmd, void *argp)
{
  struct netif *netif = NULL;
  char netif_name[NETIF_NAMESIZE];
  u16_t name_len;
  int ret = -1;
  (void)memset_s(netif_name, NETIF_NAMESIZE, 0, NETIF_NAMESIZE);
  struct lwip_sock *sock = get_socket(sockfd);
  if (sock == NULL) {
    set_errno(EFAULT);
    return -1;
  }
  name_len = LOS_StrnlenUser(argp, NETIF_NAMESIZE - 1);
  /* LOS_StrnlenUser didn't follow strnlen's behavior, LOS_StrnlenUser's
   * return value is not reliable.
   */
  if (name_len == 0 || name_len > NETIF_NAMESIZE - 1) {
    sock_set_errno(sock, EINVAL);
    goto FINISH;
  }

  if (LOS_ArchCopyFromUser(netif_name, argp, name_len) != 0) {
    sock_set_errno(sock, EFAULT);
    goto FINISH;
  }
  netif = netifapi_netif_find(netif_name);
  if (netif == NULL) {
    sock_set_errno(sock, EINVAL);
    goto FINISH;
  }
  ret = do_ioctl_netifapi_dhcp(sock, (unsigned long)cmd, netif);

FINISH:
  done_socket(sock);
  return ret;
}

int
socks_ioctl(int sockfd, long cmd, void *argp)
{
  void *argpbak = argp;
  int ret;
  size_t nbytes;
  if (argp == NULL) {
    set_errno(EINVAL);
    return -1;
  }
  if (LOS_IsUserAddress((VADDR_T)(uintptr_t)argp)) {
    if (cmd == SIOCGIFCONF) {
      return do_ioctl_SIOCGIFCONF(sockfd, cmd, argp);
    }
    /* ioctl dhcp only used in user space */
    if ((unsigned long)cmd == DHCP_START || (unsigned long)cmd == DHCP_STOP) {
      return do_ioctl_dhcp(sockfd, cmd, argp);
    }
    nbytes = get_argp_size(sockfd, cmd);
    if (nbytes == 0) {
      set_errno(EINVAL);
      return -1;
    }
    argp = malloc(nbytes);
    if (argp == NULL) {
      set_errno(ENOMEM);
      return -1;
    }
    if (LOS_ArchCopyFromUser(argp, argpbak, nbytes) != 0) {
      free(argp);
      set_errno(EFAULT);
      return -1;
    }
  }
  ret = lwip_ioctl(sockfd, cmd, argp);
  if (ret != 0) {
    if (argp != argpbak) {
      free(argp);
    }
    return -1;
  }
  if (argp != argpbak) {
    if (LOS_ArchCopyToUser(argpbak, argp, nbytes) != 0) {
      set_errno(EFAULT);
      ret = -1;
    }
    free(argp);
  }
  return ret;
}

void
socks_refer(int sockfd)
{
  struct lwip_sock *sock = NULL;
  SYS_ARCH_DECL_PROTECT(lev);

  sock = get_socket(sockfd);
  if (!sock) {
    return;
  }

  SYS_ARCH_PROTECT(lev);

  sock->s_refcount++;

  SYS_ARCH_UNPROTECT(lev);

  done_socket(sock);
}

int
socks_close(int sockfd)
{
  struct lwip_sock *sock = NULL;
  SYS_ARCH_DECL_PROTECT(lev);

  sock = get_socket(sockfd);
  if (!sock) {
    return -1;
  }

  SYS_ARCH_PROTECT(lev);

  if (sock->s_refcount == 0) {
    SYS_ARCH_UNPROTECT(lev);
    done_socket(sock);
    return lwip_close(sockfd);
  }

  sock->s_refcount--;

  SYS_ARCH_UNPROTECT(lev);
  done_socket(sock);
  return 0;
}

#if LWIP_SOCKET_POLL && LWIP_EXT_POLL_SUPPORT
int
socks_poll(int sockfd, poll_table *wait)
{
  if (wait == NULL) {
    return -EFAULT;
  }
  return lwip_poll(sockfd, wait);
}
#endif /* LWIP_SOCKET_POLL && LWIP_EXT_POLL_SUPPORT */
#endif /* LWIP_LITEOS_A_COMPAT */
#endif /* LWIP_SOCKET */
