/**
 * @file
 * lwIP network interface abstraction
 *
 * @defgroup netif Network interface (NETIF)
 * @ingroup callbackstyle_api
 *
 * @defgroup netif_ip4 IPv4 address handling
 * @ingroup netif
 *
 * @defgroup netif_ip6 IPv6 address handling
 * @ingroup netif
 *
 * @defgroup netif_cd Client data handling
 * Store data (void*) on a netif for application usage.
 * @see @ref LWIP_NUM_NETIF_CLIENT_DATA
 * @ingroup netif
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
 */

#include "lwip/opt.h"

#include <string.h> /* memset */
#include <stdlib.h> /* atoi */

#include "lwip/def.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/udp.h"
#include "lwip/priv/raw_priv.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/snmp.h"
#include "lwip/igmp.h"
#include "lwip/etharp.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
#include "lwip/ip.h"
#include "lwip/if_api.h"
#if ENABLE_LOOPBACK
#if LWIP_NETIF_LOOPBACK_MULTITHREADING
#include "lwip/tcpip.h"
#endif /* LWIP_NETIF_LOOPBACK_MULTITHREADING */
#endif /* ENABLE_LOOPBACK */

#include "netif/ethernet.h"

#if LWIP_NETIF_PROMISC
#include "netif/ifaddrs.h"
#include "lwip/raw.h"
#endif

#if LWIP_AUTOIP
#include "lwip/autoip.h"
#endif /* LWIP_AUTOIP */
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif /* LWIP_DHCP */
#if LWIP_IPV6_DHCP6
#include "lwip/dhcp6.h"
#endif /* LWIP_IPV6_DHCP6 */
#if LWIP_IPV6_MLD
#include "lwip/mld6.h"
#endif /* LWIP_IPV6_MLD */
#if LWIP_IPV6
#include "lwip/nd6.h"
#include "lwip/ethip6.h"
#endif
#include "netif/driverif.h"

#if LWIP_NAT64
#include "lwip/nat64.h"
#endif

#if LWIP_IP6IN4
#include "lwip/ip6in4.h"
#endif

#if LWIP_MDNS_RESPONDER
#include "lwip/mdns.h"
#endif

#include "lwip/tcpip.h"

#if LWIP_TCP
#include "lwip/priv/tcpip_priv.h"
#endif
#include "lwip/lwip_rpl.h"
#include "lwip/priv/api_msg.h"

/* Interface indexes always start at 1 per RFC 3493, section 4, num starts at 0 .. 8 for padding */
#define IF_NAMEINDEX_ELE_SIZE    (sizeof(struct if_nameindex) + NETIF_NAMESIZE + 4)

#if LWIP_NETIF_STATUS_CALLBACK
#define NETIF_STATUS_CALLBACK(n) do{ if (n->status_callback) { (n->status_callback)(n); }}while(0)
#else
#define NETIF_STATUS_CALLBACK(n)
#endif /* LWIP_NETIF_STATUS_CALLBACK */
#if LWIP_L2_NETDEV_STATUS_CALLBACK
#define L2_NETDEV_STATUS_CALLBACK(n) \
  do { if (n->l2_netdev_status_callback) { (n->l2_netdev_status_callback)(n); }} while (0)
#else
#define L2_NETDEV_STATUS_CALLBACK(n)
#endif /* LWIP_NETIF_STATUS_CALLBACK */

#if LWIP_NETIF_LINK_CALLBACK
#define NETIF_LINK_CALLBACK(n) do{ if (n->link_callback) { (n->link_callback)(n); }}while(0)
#else
#define NETIF_LINK_CALLBACK(n)
#endif /* LWIP_NETIF_LINK_CALLBACK */

#if LWIP_NETIF_EXT_STATUS_CALLBACK
static netif_ext_callback_t *ext_callback;
#endif

#if !LWIP_SINGLE_NETIF
struct netif *netif_list;
#endif /* !LWIP_SINGLE_NETIF */
struct netif *netif_default;

#if PF_PKT_SUPPORT
extern struct raw_pcb *pkt_raw_pcbs;
#endif

#if LWIP_NUM_NETIF_CLIENT_DATA > 0
static u8_t netif_client_id;
#endif

#define NETIF_REPORT_TYPE_IPV4  0x01
#define NETIF_REPORT_TYPE_IPV6  0x02
static void netif_issue_reports(struct netif *netif, u8_t report_type, s8_t addr_idx);

#if LWIP_HAVE_LOOPIF
#if LWIP_IPV4
static err_t netif_loop_output_ipv4(struct netif *netif, struct pbuf *p, const ip4_addr_t *addr);
#endif
#if LWIP_IPV6
static err_t netif_loop_output_ipv6(struct netif *netif, struct pbuf *p, const ip6_addr_t *addr);
#endif


static struct netif loop_netif;

/**
 * open or close netdev by netif without block as not using mutex
 * the api only called by AT command ifconfig up/down
 * @param netif the lwip network interface structure
 * @return ERR_OK callback is ok
 *         ERR_VAL netif is NULL
 */
#if LWIP_L2_NETDEV_STATUS_CALLBACK
u8_t
netif_l2_netdev_status_callback(struct netif *nif)
{
  if (nif == NULL) {
    return ERR_VAL;
  }
  L2_NETDEV_STATUS_CALLBACK(nif);
  return ERR_OK;
}
#endif

/**
 * Initialize a lwip network interface structure for a loopback interface
 *
 * @param netif the lwip network interface structure for this loopif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 */
static err_t
netif_loopif_init(struct netif *netif)
{
  LWIP_ASSERT("netif_loopif_init: invalid netif", netif != NULL);

  /* initialize the snmp variables and counters inside the struct netif
   * ifSpeed: no assumption can be made!
   */
  MIB2_INIT_NETIF(netif, snmp_ifType_softwareLoopback, 0);

  netif->name[0] = 'l';
  netif->name[1] = 'o';
  netif->name[2] = '\0';
  netif->link_layer_type = LOOPBACK_IF;
  netif->mtu = NETIF_LOOPBACK_MTU;
#if LWIP_IPV4
  netif->output = netif_loop_output_ipv4;
#endif
#if LWIP_IPV6
  netif->output_ip6 = netif_loop_output_ipv6;
#endif
#if LWIP_LOOPIF_MULTICAST
  netif_set_flags(netif, NETIF_FLAG_IGMP);
#endif
  NETIF_SET_CHECKSUM_CTRL(netif, NETIF_CHECKSUM_DISABLE_ALL);
  return ERR_OK;
}
#endif /* LWIP_HAVE_LOOPIF */

#if LWIP_DHCP
/*
 * Close DHCP and set static network.
 *
 * @param netif a pre-allocated netif structure
 *
 * @return ERR_OK, or ERR_VAL if failed.
 */
err_t
netif_dhcp_off(struct netif *netif)
{
  ip_addr_t old_ipaddr;
  ip_addr_t old_netmask;
  ip_addr_t old_gateway;

  if (netif == NULL) {
    return ERR_VAL;
  }
  old_ipaddr = netif->ip_addr;
  old_netmask = netif->netmask;
  old_gateway = netif->gw;

  if (netif_dhcp_data(netif)) {
    (void)dhcp_release(netif);
    dhcp_stop(netif);
#if !LWIP_DHCP_SUBSTITUTE
    dhcp_cleanup(netif);
#endif
    LWIP_DEBUGF(NETIF_DEBUG, ("DHCP is close;set static IP\n"));
  }

  ip_addr_set_val(netif->ip_addr, old_ipaddr);
  ip_addr_set_val(netif->netmask, old_netmask);
  ip_addr_set_val(netif->gw, old_gateway);
  (void)netif_set_up(netif);

  return ERR_OK;
}
#endif /* LWIP_DHCP */

#if LWIP_IPV6_DHCP6
u8_t netif_ip6_addr_isdhcp6(struct netif *netif, s8_t i)
{
  struct dhcp6 *dhcp6 = NULL;
  if (netif == NULL) {
    return 0;
  }
  dhcp6 = netif_dhcp6_data(netif);
  if (dhcp6 == NULL) {
    return 0;
  }
  if ((dhcp6_stateful_enabled(dhcp6))
#if LWIP_IPV6_DHCP6_STATEFUL
       && (dhcp6->addr_idx == i)
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
    ) {
    return 1;
  }
#if !LWIP_IPV6_DHCP6_STATEFUL
  LWIP_UNUSED_ARG(i);
#endif
  return 0;
}
#endif /* LWIP_IPV6_DHCP6 */

u8_t netif_ipaddr_isbrdcast(const ip_addr_t *ipaddr)
{
  struct netif *netif = NULL;

  for (netif = netif_list; netif != NULL; netif = netif->next) {
    if (netif_is_up(netif) && ip_addr_isbroadcast(ipaddr, netif)) {
      return 1;
    }
  }

  return 0;
}

void
netif_init(void)
{
#if LWIP_HAVE_LOOPIF
#if LWIP_IPV4
#define LOOPIF_ADDRINIT , &loop_ipaddr, &loop_netmask, &loop_gw
  ip4_addr_t loop_ipaddr, loop_netmask, loop_gw;
  IP4_ADDR(&loop_gw, 127, 0, 0, 1);
  IP4_ADDR(&loop_ipaddr, 127, 0, 0, 1);
  IP4_ADDR(&loop_netmask, 255, 0, 0, 0);
#else /* LWIP_IPV4 */
#define LOOPIF_ADDRINIT
#endif /* LWIP_IPV4 */

  (void)netif_add(&loop_netif LOOPIF_ADDRINIT);

#if LWIP_IPV6
  IP_ADDR6_HOST(loop_netif.ip6_addr, 0, 0, 0, 0x00000001UL);
  loop_netif.ip6_addr_state[0] = IP6_ADDR_VALID;
#endif /* LWIP_IPV6 */

  (void)netif_set_link_up(&loop_netif);
  (void)netif_set_up(&loop_netif);
#if DRIVER_STATUS_CHECK
  loop_netif.flags |= NETIF_FLAG_DRIVER_RDY;
#endif

  loop_netif.flags |= NETIF_FLAG_LOOPBACK;
#endif /* LWIP_HAVE_LOOPIF */
}

/**
 * @ingroup lwip_nosys
 * Forwards a received packet for input processing with
 * ethernet_input() or ip_input() depending on netif flags.
 * Don't call directly, pass to netif_add() and call
 * netif->input().
 * Only works if the netif driver correctly sets
 * NETIF_FLAG_ETHARP and/or NETIF_FLAG_ETHERNET flag!
 */
err_t
netif_input(struct pbuf *p, struct netif *inp)
{
  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("netif_input : invalid arguments", (inp != NULL), return ERR_ARG);
  LWIP_ERROR("netif_input : invalid arguments", (p != NULL), return ERR_ARG);

#if LWIP_ETHERNET
  if (inp->flags & (NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET)) {
    return ethernet_input(p, inp);
  } else
#endif /* LWIP_ETHERNET */
    return ip_input(p, inp);
}

/**
 * @ingroup netif
 * Add a network interface to the list of lwIP netifs.
 *
 * Same as @ref netif_add but without IPv4 addresses
 */
struct netif *
netif_add_noaddr(struct netif *netif)
{
  return netif_add(netif
#if LWIP_IPV4
                   , NULL, NULL, NULL
#endif /* LWIP_IPV4*/
                  );
}

u8_t netif_check_num_isusing(const char *ifname, const u8_t num)
{
  struct netif *netif = NULL;
  LWIP_ERROR("netif_check_num_isusing : invalid arguments", (ifname != NULL), return 0);

  for (netif = netif_list; netif != NULL; netif = netif->next) {
    if ((num == netif->num) && (strncmp(netif->name, ifname, NETIF_NAMESIZE) == 0)) {
      return 1;
    }
  }

  return 0;
}

static u8_t netif_alloc_num(const char *name)
{
  u8_t num;

  for (num = 0; num < LWIP_NETIF_NUM_MAX; num++) {
    if (netif_check_num_isusing(name, num) == 0) {
      break;
    }
  }

  return num;
}

struct netif *
netif_find_by_ifindex(u8_t ifindex)
{
  struct netif *netif = NULL;

  if (ifindex < LWIP_NETIF_IFINDEX_START) {
    LWIP_DEBUGF(NETIF_DEBUG, ("netif_find_by_ifindex: Invalid ifindex(%u) \n", ifindex));
    return netif;
  }

  for (netif = netif_list; netif != NULL; netif = netif->next) {
    if (netif->ifindex == ifindex) {
      LWIP_DEBUGF(NETIF_DEBUG, ("netif_find_by_ifindex: found\n"));
      return netif;
    }
  }

  LWIP_DEBUGF(NETIF_DEBUG, ("netif_find_by_ifindex: didn't find\n"));
  return NULL;
}

/* Need to support the get index for all families
removed PF_PKT_SUPPORT flag , as this flags restricts the usage to only PF_FAMILY Begin */
static u8_t
netif_alloc_ifindex(void)
{
  u8_t tmp_index;

  for (tmp_index = LWIP_NETIF_IFINDEX_START; tmp_index <= LWIP_NETIF_IFINDEX_MAX; tmp_index++) {
    if (netif_get_by_index(tmp_index) == NULL) {
      break;
    }
  }

  return tmp_index;
}

/**
 * @ingroup netif
 * Add a network interface to the list of lwIP netifs.
 *
 * @param netif a pre-allocated netif structure
 * @param ipaddr IP address for the new netif
 * @param netmask network mask for the new netif
 * @param gw default gateway IP address for the new netif
 * @param state opaque data passed to the new netif
 * @param init callback function that initializes the interface
 * @param input callback function that is called to pass
 * ingress packets up in the protocol layer stack.\n
 * It is recommended to use a function that passes the input directly
 * to the stack (netif_input(), NO_SYS=1 mode) or via sending a
 * message to TCPIP thread (tcpip_input(), NO_SYS=0 mode).\n
 * These functions use netif flags NETIF_FLAG_ETHARP and NETIF_FLAG_ETHERNET
 * to decide whether to forward to ethernet_input() or ip_input().
 * In other words, the functions only work when the netif
 * driver is implemented correctly!\n
 * Most members of struct netif should be be initialized by the
 * netif init function = netif driver (init parameter of this function).\n
 * IPv6: Don't forget to call netif_create_ip6_linklocal_address() after
 * setting the MAC address in struct netif.hwaddr
 * (IPv6 requires a link-local address).
 *
 * @return netif, or NULL if failed.
 */
struct netif *
netif_add(struct netif *netif
#if LWIP_IPV4
  , const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw
#endif
)
{
#if LWIP_IPV6
  s8_t i;
#endif

  LWIP_ASSERT_CORE_LOCKED();

#if LWIP_SINGLE_NETIF
  if (netif_default != NULL) {
    LWIP_ASSERT("single netif already set", 0);
    return NULL;
  }
#endif

  LWIP_ERROR("netif_add: invalid netif", netif != NULL, return NULL);

#if LWIP_IPV4
  if (ipaddr == NULL) {
    ipaddr = ip_2_ip4(IP4_ADDR_ANY);
  }
  if (netmask == NULL) {
    netmask = ip_2_ip4(IP4_ADDR_ANY);
  }
  if (gw == NULL) {
    gw = ip_2_ip4(IP4_ADDR_ANY);
  }

  /* reset new interface configuration state */
  ip_addr_set_zero_ip4(&netif->ip_addr);
  ip_addr_set_zero_ip4(&netif->netmask);
  ip_addr_set_zero_ip4(&netif->gw);
#endif /* LWIP_IPV4 */

#if LWIP_NETIF_PROMISC
  (void)atomic_set((&netif->flags_ext), 0);
  netif->flags_ext1 = 0;
#endif /* LWIP_NETIF_PROMISC */

#if LWIP_IPV6
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    ip_addr_set_zero_ip6(&netif->ip6_addr[i]);
    netif->ip6_addr_state[i] = IP6_ADDR_INVALID;
#if LWIP_IPV6_ADDRESS_LIFETIMES
    netif->ip6_addr_valid_life[i] = IP6_ADDR_LIFE_STATIC;
    netif->ip6_addr_pref_life[i] = IP6_ADDR_LIFE_STATIC;
#endif /* LWIP_IPV6_ADDRESS_LIFETIMES */
  }
  netif->output_ip6 = ethip6_output;
#if LWIP_ND6_ROUTER
  netif->forwarding = lwIP_FALSE;
  netif->accept_ra = lwIP_FALSE;
  netif->ra_enable = lwIP_FALSE;
  netif->ra_init_cnt = ND6_RA_INIT_COUNT;
  netif->ra_timer = ND6_RA_INIT_INTERVAL;
#endif
  netif->ipv6_addr_event_cb = NULL;

  /**
  @page RFC-4862 RFC-4862
  @par Compliant Sections
  Section 5.5.4.  Address Lifetime Expiry
  @par Behavior Description
  Test: System management MUST have the ability to disable preferred_lifetime management facility,
  and the facility MUST be disabled by default. User can enable/disable at runtime using ioctl for
  a particular interface.
  */
  /* Default value for enable_deprecated is DISABLE. */
#if LWIP_IPV6_DUP_DETECT_ATTEMPTS
  /* Default value for DAD. */
  netif->ipv6_flags = netif->ipv6_flags | LWIP_IPV6_ND6_FLAG_DAD;
#endif
#endif /* LWIP_IPV6 */
  NETIF_SET_CHECKSUM_CTRL(netif, NETIF_CHECKSUM_ENABLE_ALL);
  netif->mtu = 0;
  netif->flags = 0;
#ifdef netif_get_client_data
  memset(netif->client_data, 0, sizeof(netif->client_data));
#endif /* LWIP_NUM_NETIF_CLIENT_DATA */
#if LWIP_DHCPS
  netif->dhcps = NULL;
#endif /* LWIP_DHCPS */
#if LWIP_IPV6
#if LWIP_IPV6_AUTOCONFIG
  /* IPv6 address autoconfiguration not enabled by default */
  netif->ip6_autoconfig_enabled = 0;
#endif /* LWIP_IPV6_AUTOCONFIG */
#if LWIP_IPV6_SEND_ROUTER_SOLICIT
  nd6_restart_netif(netif);
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */
#if LWIP_L2_NETDEV_STATUS_CALLBACK
  netif->l2_netdev_status_callback = NULL;
#endif
#endif /* LWIP_IPV6 */
#if LWIP_NETIF_STATUS_CALLBACK
  netif->status_callback = NULL;
#endif /* LWIP_NETIF_STATUS_CALLBACK */
#if LWIP_NETIF_LINK_CALLBACK
  netif->link_callback = NULL;
#endif /* LWIP_NETIF_LINK_CALLBACK */
#if LWIP_NETIF_REMOVE_CALLBACK
  netif->remove_callback = NULL;
#endif /* LWIP_NETIF_REMOVE_CALLBACK */
#if LWIP_API_MESH
  netif->linklayer_event = netif_linklayer_event_callback;
#if  LWIP_RIPPLE
  netif->remove_peer = NULL;
  netif->set_beacon_prio = NULL;
  netif->set_unique_id = NULL;
  netif->get_peer_count = NULL;
  netif->set_rank = NULL;
  netif->get_link_metric = NULL;
#endif /* LWIP_RIPPLE */
#endif /* LWIP_API_MESH */
#if LWIP_IGMP && LWIP_LINK_MCAST_FILTER
  netif->igmp_mac_filter = NULL;
#endif /* LWIP_IGMP && LWIP_LINK_MCAST_FILTER */
#if LWIP_IPV6 && LWIP_IPV6_MLD && LWIP_LINK_MCAST_FILTER
  netif->mld_mac_filter = NULL;
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD && LWIP_LINK_MCAST_FILTER */
#if LWIP_ARP && LWIP_ARP_GRATUITOUS_REXMIT
  netif->arp_gratuitous_doing = lwIP_FALSE;
  netif->arp_gratuitous_cnt = 0;
#endif

#if LWIP_HAVE_LOOPIF
  if (netif == &loop_netif) {
    (void)netif_loopif_init(netif);
    netif->num = 0;
  } else
#endif
  {
  /* call user specified initialization function for netif */
    if (driverif_init(netif) != ERR_OK) {
      return NULL;
    }

    netif->num = netif_alloc_num(netif->name);
    if (netif->num == LWIP_NETIF_NUM_MAX) {
      LWIP_DEBUGF(NETIF_DEBUG, ("netif: interface num alloc fail"));
      return NULL;
    }
  }

  netif->input = tcpip_input;

  NETIF_RESET_HINTS(netif);
#if ENABLE_LOOPBACK
  netif->loop_first = NULL;
  netif->loop_last = NULL;
#if LWIP_LOOPBACK_MAX_PBUFS
  netif->loop_cnt_current = 0;
#endif /* LWIP_LOOPBACK_MAX_PBUFS */
#if LWIP_NETIF_LOOPBACK_MULTITHREADING
  netif->reschedule_poll = 0;
#endif /* LWIP_NETIF_LOOPBACK_MULTITHREADING */
#endif /* ENABLE_LOOPBACK */

#if LWIP_IPV4
  (void)netif_set_addr(netif, ipaddr, netmask, gw);
#endif /* LWIP_IPV4 */

#if LWIP_IPV6 && LWIP_ND6_ALLOW_RA_UPDATES
  /* Initialize the MTU for IPv6 to the one set by the netif driver.
     This can be updated later by RA. */
  netif->mtu6 = netif->mtu;
#endif /* LWIP_IPV6 && LWIP_ND6_ALLOW_RA_UPDATES */

  /*
   * Need to support the get index for all families
   * removed PF_PKT_SUPPORT flag , as this flags restricts the usage to only PF_FAMILY
   */
  netif->ifindex = netif_alloc_ifindex();
  LWIP_DEBUGF(NETIF_DEBUG, ("netif: netif->ifindex=%d", netif->ifindex));
  if ((netif->ifindex < LWIP_NETIF_IFINDEX_START) || (netif->ifindex > LWIP_NETIF_IFINDEX_MAX)) {
    LWIP_DEBUGF(NETIF_DEBUG, ("netif: interface index alloc fail"));
    return NULL;
  }

#if !LWIP_SINGLE_NETIF
  /* add this netif to the list */
  netif->next = netif_list;
  netif_list = netif;
#endif /* "LWIP_SINGLE_NETIF */
  mib2_netif_added(netif);

#if LWIP_IGMP
  /* start IGMP processing */
  if (netif->flags & NETIF_FLAG_IGMP) {
    igmp_start(netif);
  }
#endif /* LWIP_IGMP */

  LWIP_DEBUGF(NETIF_DEBUG, ("netif: added interface %s IP",
                            netif->name));
#if LWIP_IPV4
  LWIP_DEBUGF(NETIF_DEBUG, (" addr "));
  ip4_addr_debug_print(NETIF_DEBUG, ipaddr);
  LWIP_DEBUGF(NETIF_DEBUG, (" netmask "));
  ip4_addr_debug_print(NETIF_DEBUG, netmask);
  LWIP_DEBUGF(NETIF_DEBUG, (" gw "));
  ip4_addr_debug_print(NETIF_DEBUG, gw);
#endif /* LWIP_IPV4 */
  LWIP_DEBUGF(NETIF_DEBUG, ("\n"));

  netif_invoke_ext_callback(netif, LWIP_NSC_NETIF_ADDED, NULL);

  return netif;
}

static void
netif_do_ip_addr_changed(const ip_addr_t *old_addr, const ip_addr_t *new_addr)
{
#if LWIP_TCP
  tcp_netif_ip_addr_changed(old_addr, new_addr);
#endif /* LWIP_TCP */
#if LWIP_UDP
  udp_netif_ip_addr_changed(old_addr, new_addr);
#endif /* LWIP_UDP */
#if LWIP_RAW
  raw_netif_ip_addr_changed(old_addr, new_addr);
#endif /* LWIP_RAW */
}

#if LWIP_IPV4
static int
netif_do_set_ipaddr(struct netif *netif, const ip4_addr_t *ipaddr, ip_addr_t *old_addr)
{
  LWIP_ASSERT("invalid pointer", ipaddr != NULL);
  LWIP_ASSERT("invalid pointer", old_addr != NULL);

  /* address is actually being changed? */
  if (ip4_addr_cmp(ipaddr, netif_ip4_addr(netif)) == 0) {
    ip_addr_t new_addr;
    *ip_2_ip4(&new_addr) = *ipaddr;
    IP_SET_TYPE_VAL(new_addr, IPADDR_TYPE_V4);

    ip_addr_copy(*old_addr, *netif_ip_addr4(netif));

    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_STATE, ("netif_set_ipaddr: netif address being changed\n"));
    netif_do_ip_addr_changed(old_addr, &new_addr);

    mib2_remove_ip4(netif);
    mib2_remove_route_ip4(0, netif);
    /* set new IP address to netif */
    ip4_addr_set(ip_2_ip4(&netif->ip_addr), ipaddr);
    IP_SET_TYPE_VAL(netif->ip_addr, IPADDR_TYPE_V4);
    mib2_add_ip4(netif);
    mib2_add_route_ip4(0, netif);

    netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4, (s8_t)LWIP_INVALID_IPV6_IDX);

    NETIF_STATUS_CALLBACK(netif);
    return 1; /* address changed */
  }
  return 0; /* address unchanged */
}

/**
 * @ingroup netif_ip4
 * Change the IP address of a network interface
 *
 * @param netif the network interface to change
 * @param ipaddr the new IP address
 *
 * @note call netif_set_addr() if you also want to change netmask and
 * default gateway
 */
void
netif_set_ipaddr(struct netif *netif, const ip4_addr_t *ipaddr)
{
  ip_addr_t old_addr;

  LWIP_ERROR("netif_set_ipaddr: invalid netif", netif != NULL, return);

  /* Don't propagate NULL pointer (IPv4 ANY) to subsequent functions */
  if (ipaddr == NULL) {
    ipaddr = IP4_ADDR_ANY4;
  }

  LWIP_ASSERT_CORE_LOCKED();

  if (netif_do_set_ipaddr(netif, ipaddr, &old_addr)) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    netif_ext_callback_args_t args;
    args.ipv4_changed.old_address = &old_addr;
    netif_invoke_ext_callback(netif, LWIP_NSC_IPV4_ADDRESS_CHANGED, &args);
#endif
  }
}

static int
netif_do_set_netmask(struct netif *netif, const ip4_addr_t *netmask, ip_addr_t *old_nm)
{
  /* address is actually being changed? */
  if (ip4_addr_cmp(netmask, netif_ip4_netmask(netif)) == 0) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    LWIP_ASSERT("invalid pointer", old_nm != NULL);
    ip_addr_copy(*old_nm, *netif_ip_netmask4(netif));
#else
    LWIP_UNUSED_ARG(old_nm);
#endif
    mib2_remove_route_ip4(0, netif);
    /* set new netmask to netif */
    ip4_addr_set(ip_2_ip4(&netif->netmask), netmask);
    IP_SET_TYPE_VAL(netif->netmask, IPADDR_TYPE_V4);
    mib2_add_route_ip4(0, netif);
    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                ("netif: netmask of interface %s set to %"U16_F".%"U16_F".%"U16_F".%"U16_F"\n",
                netif->name,
                ip4_addr1_16(netif_ip4_netmask(netif)),
                ip4_addr2_16(netif_ip4_netmask(netif)),
                ip4_addr3_16(netif_ip4_netmask(netif)),
                ip4_addr4_16(netif_ip4_netmask(netif))));
    return 1; /* netmask changed */
  }
  return 0; /* netmask unchanged */
}

/**
 * @ingroup netif_ip4
 * Change the netmask of a network interface
 *
 * @param netif the network interface to change
 * @param netmask the new netmask
 *
 * @note call netif_set_addr() if you also want to change ip address and
 * default gateway
 */
void
netif_set_netmask(struct netif *netif, const ip4_addr_t *netmask)
{
#if LWIP_NETIF_EXT_STATUS_CALLBACK
  ip_addr_t old_nm_val;
  ip_addr_t *old_nm = &old_nm_val;
#else
  ip_addr_t *old_nm = NULL;
#endif
  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("netif_set_netmask: invalid netif", netif != NULL, return);

  /* Don't propagate NULL pointer (IPv4 ANY) to subsequent functions */
  if (netmask == NULL) {
    netmask = IP4_ADDR_ANY4;
  }

  if (netif_do_set_netmask(netif, netmask, old_nm)) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    netif_ext_callback_args_t args;
    args.ipv4_changed.old_netmask = old_nm;
    netif_invoke_ext_callback(netif, LWIP_NSC_IPV4_NETMASK_CHANGED, &args);
#endif
  }
}

static int
netif_do_set_gw(struct netif *netif, const ip4_addr_t *gw, ip_addr_t *old_gw)
{
  /* address is actually being changed? */
  if (ip4_addr_cmp(gw, netif_ip4_gw(netif)) == 0) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    LWIP_ASSERT("invalid pointer", old_gw != NULL);
    ip_addr_copy(*old_gw, *netif_ip_gw4(netif));
#else
    LWIP_UNUSED_ARG(old_gw);
#endif

    ip4_addr_set(ip_2_ip4(&netif->gw), gw);
    IP_SET_TYPE_VAL(netif->gw, IPADDR_TYPE_V4);
    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                ("netif: GW address of interface %s set to %"U16_F".%"U16_F".%"U16_F".%"U16_F"\n",
                netif->name,
                ip4_addr1_16(netif_ip4_gw(netif)),
                ip4_addr2_16(netif_ip4_gw(netif)),
                ip4_addr3_16(netif_ip4_gw(netif)),
                ip4_addr4_16(netif_ip4_gw(netif))));
    return 1; /* gateway changed */
  }
  return 0; /* gateway unchanged */
}

/**
 * @ingroup netif_ip4
 * Change the default gateway for a network interface
 *
 * @param netif the network interface to change
 * @param gw the new default gateway
 *
 * @note call netif_set_addr() if you also want to change ip address and netmask
 */
void
netif_set_gw(struct netif *netif, const ip4_addr_t *gw)
{
#if LWIP_NETIF_EXT_STATUS_CALLBACK
  ip_addr_t old_gw_val;
  ip_addr_t *old_gw = &old_gw_val;
#else
  ip_addr_t *old_gw = NULL;
#endif
  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("netif_set_gw: invalid netif", netif != NULL, return);

  /* Don't propagate NULL pointer (IPv4 ANY) to subsequent functions */
  if (gw == NULL) {
    gw = IP4_ADDR_ANY4;
  }

  if (netif_do_set_gw(netif, gw, old_gw)) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    netif_ext_callback_args_t args;
    args.ipv4_changed.old_gw = old_gw;
    netif_invoke_ext_callback(netif, LWIP_NSC_IPV4_GATEWAY_CHANGED, &args);
#endif
  }
}

/**
 * @ingroup netif_ip4
 * Change IP address configuration for a network interface (including netmask
 * and default gateway).
 *
 * @param netif the network interface to change
 * @param ipaddr the new IP address
 * @param netmask the new netmask
 * @param gw the new default gateway
 */
err_t
netif_set_addr(struct netif *netif, const ip4_addr_t *ipaddr, const ip4_addr_t *netmask,
               const ip4_addr_t *gw)
{
  LWIP_ERROR("netif_set_addr: invalid arguments", (netif != NULL), return ERR_VAL);
#if LWIP_NETIF_EXT_STATUS_CALLBACK
  netif_nsc_reason_t change_reason = LWIP_NSC_NONE;
  netif_ext_callback_args_t cb_args;
  ip_addr_t old_nm_val;
  ip_addr_t old_gw_val;
  ip_addr_t *old_nm = &old_nm_val;
  ip_addr_t *old_gw = &old_gw_val;
#else
  ip_addr_t *old_nm = NULL;
  ip_addr_t *old_gw = NULL;
#endif
  ip_addr_t old_addr;
  int remove;

  LWIP_ASSERT_CORE_LOCKED();

  /* Don't propagate NULL pointer (IPv4 ANY) to subsequent functions */
  if (ipaddr == NULL) {
    ipaddr = IP4_ADDR_ANY4;
  }
  if (netmask == NULL) {
    netmask = IP4_ADDR_ANY4;
  }
  if (gw == NULL) {
    gw = IP4_ADDR_ANY4;
  }

  remove = ip4_addr_isany(ipaddr);
  if (remove) {
    /* when removing an address, we have to remove it *before* changing netmask/gw
       to ensure that tcp RST segment can be sent correctly */
    if (netif_do_set_ipaddr(netif, ipaddr, &old_addr)) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
      change_reason |= LWIP_NSC_IPV4_ADDRESS_CHANGED;
      cb_args.ipv4_changed.old_address = &old_addr;
#endif
    }
  }
  if (netif_do_set_netmask(netif, netmask, old_nm)) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    change_reason |= LWIP_NSC_IPV4_NETMASK_CHANGED;
    cb_args.ipv4_changed.old_netmask = old_nm;
#endif
  }
  if (netif_do_set_gw(netif, gw, old_gw)) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    change_reason |= LWIP_NSC_IPV4_GATEWAY_CHANGED;
    cb_args.ipv4_changed.old_gw = old_gw;
#endif
  }
  if (!remove) {
    /* set ipaddr last to ensure netmask/gw have been set when status callback is called */
    if (netif_do_set_ipaddr(netif, ipaddr, &old_addr)) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
      change_reason |= LWIP_NSC_IPV4_ADDRESS_CHANGED;
      cb_args.ipv4_changed.old_address = &old_addr;
#endif
    }
  }

#if LWIP_NETIF_EXT_STATUS_CALLBACK
  if (change_reason != LWIP_NSC_NONE) {
    change_reason |= LWIP_NSC_IPV4_SETTINGS_CHANGED;
  }
  if (!remove) {
    /* Isssue a callback even if the address hasn't changed, eg. DHCP reboot */
    change_reason |= LWIP_NSC_IPV4_ADDR_VALID;
  }
  if (change_reason != LWIP_NSC_NONE) {
    netif_invoke_ext_callback(netif, change_reason, &cb_args);
  }
#endif

  return ERR_OK;
}

/**
 * @ingroup netif_ip4
 * Get IP_add configuration for a network interface (including netmask
 * and default gateway).
 *
 * @param netif get the network interface
 * @param ipaddr the network interface IP_add
 * @param netmask the network interface netmask
 * @param gw the network interface default gateway
 *
 * @note
 *  - netmask and/or gw can be passed NULL, if these details about the netif are not needed
 */
err_t
netif_get_addr(const struct netif *netif, ip4_addr_t *ipaddr, ip4_addr_t *netmask, ip4_addr_t *gw)
{
  LWIP_ERROR("netif_get_addr: invalid arguments", (netif != NULL), return ERR_VAL);
  LWIP_ERROR("netif_get_addr: invalid IP pointer", (ipaddr != NULL), return ERR_VAL);

  ip4_addr_set(ipaddr, ip_2_ip4(&(netif->ip_addr)));
  if (netmask != NULL) {
    ip4_addr_set(netmask, ip_2_ip4(&(netif->netmask)));
  }

  if (gw != NULL) {
    ip4_addr_set(gw, ip_2_ip4(&(netif->gw)));
  }

  return ERR_OK;
}
#endif /* LWIP_IPV4*/

#ifdef LWIP_TESTBED
#if LWIP_IPV6
#include "lwip/priv/nd6_priv.h"
extern struct nd6_neighbor_cache_entry neighbor_cache[LWIP_ND6_NUM_NEIGHBORS];
extern struct nd6_destination_cache_entry destination_cache[LWIP_ND6_NUM_DESTINATIONS];
extern struct nd6_router_list_entry default_router_list[LWIP_ND6_NUM_ROUTERS];
#endif

/**
 * @ingroup netif
 * reset a network interface from the list of lwIP netifs to a initalize state.
 *
 * @param netif the network interface to remove
 */
err_t
netif_reset(struct netif *netif)
{
#if LWIP_IPV6
  int i;
#endif

  LWIP_ERROR("netif_remove : invalid value.", (netif != NULL), return ERR_VAL);

#if LWIP_IPV4
  if (!ip4_addr_isany_val(*netif_ip4_addr(netif))) {
#if LWIP_TCP
    tcp_netif_ip_addr_changed(netif_ip_addr4(netif), NULL);
#endif /* LWIP_TCP */
#if LWIP_UDP
    udp_netif_ip_addr_changed(netif_ip_addr4(netif), NULL);
#endif /* LWIP_UDP */
#if LWIP_RAW
    raw_netif_ip_addr_changed(netif_ip_addr4(netif), NULL);
#endif /* LWIP_RAW */
  }

#if LWIP_IGMP
  /* stop IGMP processing */
  if (netif->flags & NETIF_FLAG_IGMP) {
    igmp_stop(netif);
    igmp_start(netif);
  }
#endif /* LWIP_IGMP */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i))) {
#if LWIP_TCP
      tcp_netif_ip_addr_changed(netif_ip_addr6(netif, i), NULL);
#endif /* LWIP_TCP */
#if LWIP_UDP
      udp_netif_ip_addr_changed(netif_ip_addr6(netif, i), NULL);
#endif /* LWIP_UDP */
#if LWIP_RAW
      raw_netif_ip_addr_changed(netif_ip_addr6(netif, i), NULL);
#endif /* LWIP_RAW */
    }
    netif_ip6_addr_set_state(netif, i, IP6_ADDR_INVALID);
  }
#if LWIP_IPV6_MLD
  /* stop MLD processing */
  mld6_stop(netif);
#endif /* LWIP_IPV6_MLD */
#endif /* LWIP_IPV6 */
#if LWIP_NETIF_PROMISC
  /* disable promiscuous mode if enabled */
  if (atomic_read(&netif->flags_ext) == NETIF_FLAG_PROMISC) {
    netif_update_promiscuous_mode_status(netif, 0);
  }
#endif

#if LWIP_IPV6
  nd6_remove_netif_neighbor_cache_entries(netif);
  (void)memset_s(neighbor_cache, sizeof(neighbor_cache), 0, sizeof(neighbor_cache));
  (void)memset_s(destination_cache, sizeof(destination_cache), 0, sizeof(destination_cache));
  (void)memset_s(default_router_list, sizeof(default_router_list), 0, sizeof(default_router_list));
#endif

  LWIP_DEBUGF(NETIF_DEBUG, ("netif_reset: reset netif\n"));
  return ERR_OK;
}
#endif

#if LWIP_AUTO_CLEANUP_WHILE_NETIF_REMOVE
static void netif_cleanup_attached_resource(struct netif *netif)
{
#if LWIP_NAT64
  (void)nat64_deinit_netif(netif);
#endif

#if LWIP_IP6IN4
  (void)ip6in4_deinit_netif(netif);
#endif

#if LWIP_RIPPLE
  if (netif->flags & NETIF_IS_RPL_UP) {
    (void)lwip_stop_rpl(netif);
  }
  (void)rpl_context_netif_free(netif);
#endif /* LWIP_RIPPLE */

#if LWIP_MDNS_RESPONDER
  (void)mdns_resp_remove_netif(netif);
#endif

#if LWIP_DHCP
  dhcp_cleanup(netif);
#endif /* LWIP_DHCP */

#if LWIP_DHCPS
  dhcps_stop(netif);
#endif /* LWIP_DHCPS */

#if LWIP_IPV6_DHCP6
  dhcp6_disable(netif);
  dhcp6_cleanup(netif);
#endif /* LWIP_IPV6_DHCP6 */
}
#endif /* LWIP_AUTO_CLEANUP_WHILE_NETIF_REMOVE */

/**
 * @ingroup netif
 * Remove a network interface from the list of lwIP netifs.
 *
 * @param netif the network interface to remove
 */
err_t
netif_remove(struct netif *netif)
{
#if LWIP_IPV6
  int i;
#endif

  LWIP_ASSERT_CORE_LOCKED();
  LWIP_ERROR("netif_remove : invalid value.", (netif != NULL), return ERR_VAL);

  netif_invoke_ext_callback(netif, LWIP_NSC_NETIF_REMOVED, NULL);

#if LWIP_IPV4
  if (!ip4_addr_isany_val(*netif_ip4_addr(netif))) {
    netif_do_ip_addr_changed(netif_ip_addr4(netif), NULL);
  }

#if LWIP_IGMP
  /* stop IGMP processing */
  if (netif->flags & NETIF_FLAG_IGMP) {
    igmp_stop(netif);
  }
#endif /* LWIP_IGMP */
#endif /* LWIP_IPV4*/

#if LWIP_IPV6
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i))) {
      netif_do_ip_addr_changed(netif_ip_addr6(netif, i), NULL);
    }
  }
#if LWIP_IPV6_MLD_QUERIER
  mld6_querier_stop(netif);
#endif /* LWIP_IPV6_MLD_QUERIER */
#if LWIP_IPV6_MLD
  /* stop MLD processing */
  mld6_stop(netif);
#endif /* LWIP_IPV6_MLD */
#endif /* LWIP_IPV6 */

#if LWIP_NETIF_PROMISC
  /* disable promiscuous mode if enabled */
  if (atomic_read(&netif->flags_ext) == NETIF_FLAG_PROMISC) {
    netif_update_promiscuous_mode_status(netif, 0);
  }
#endif

#if LWIP_AUTO_CLEANUP_WHILE_NETIF_REMOVE
  netif_cleanup_attached_resource(netif);
#endif

  if (netif_is_up(netif)) {
    /* set netif down before removing (call callback function) */
    (void)netif_set_down(netif);
  }

  mib2_remove_ip4(netif);

  /* this netif is default? */
  if (netif_default == netif) {
    /* reset default netif */
    (void)netif_set_default(NULL);
  }
#if !LWIP_SINGLE_NETIF
  /*  is it the first netif? */
  if (netif_list == netif) {
    netif_list = netif->next;
  } else {
    /*  look for netif further down the list */
    struct netif *tmp_netif;
    NETIF_FOREACH(tmp_netif) {
      if (tmp_netif->next == netif) {
        tmp_netif->next = netif->next;
        break;
      }
    }
    if (tmp_netif == NULL) {
      return ERR_OK; /* netif is not on the list */
    }
  }
#endif /* !LWIP_SINGLE_NETIF */
  mib2_netif_removed(netif);
#if LWIP_NETIF_REMOVE_CALLBACK
  if (netif->remove_callback) {
    netif->remove_callback(netif);
  }
#endif /* LWIP_NETIF_REMOVE_CALLBACK */
  LWIP_DEBUGF( NETIF_DEBUG, ("netif_remove: removed netif\n") );
  return ERR_OK;
}

#if LWIP_IPV4

/*
 * Find a network interface by searching for its ipaddress
 *
 * @param ipaddr IP_add of the netif
 * @return netif if the its found
 *         NULL if there is no netif with ipaddr
 */
struct netif *
netif_find_by_ip4addr(const ip_addr_t *ipaddr)
{
  struct netif *netif = NULL;

  LWIP_ASSERT("Expecting ipaddr to be not NULL ", ipaddr != NULL);

  for (netif = netif_list; netif != NULL; netif = netif->next) {
    if (ip_addr_cmp(&(netif->ip_addr), ipaddr)) {
      LWIP_DEBUGF(NETIF_DEBUG, ("netif_find_using_ipaddr: found\n"));
      return netif;
    }
  }
  LWIP_DEBUGF(NETIF_DEBUG, ("netif_find_using_ip4addr: didn't find\n"));
  return NULL;
}

s8_t netif_find_dst_ipaddr(const ip_addr_t *ipaddr, ip_addr_t **dst_addr)
{
  struct netif *netif = NULL;
  ip_addr_t *pdst_addr = NULL;

  pdst_addr = *dst_addr;
  netif = netif_find_by_ipaddr(ipaddr);
  if (netif == NULL) {
    return -1;
  }
  if ((pdst_addr != NULL) && (!ip_addr_netcmp(pdst_addr, &(netif->ip_addr), ip_2_ip4(&netif->netmask))) &&
    !ip_addr_islinklocal(pdst_addr)) {
    if (!ip_addr_isany(&netif->gw)) {
      *dst_addr  = &(netif->gw);
    }
  }

  return 0;
}
#endif /* LWIP_IPV4 */

/**
 * @ingroup netif
 * Set a network interface as the default network interface
 * (used to output all packets for which no specific route is found)
 * In case of invoking lwip_connect() without prior lwip_bind(), the driver adaptor must call this macro
 * since there should be atleast one default network interface to ensure the functionality while sending out
 * packet to peer with Link Local Address.
 * @param netif the default network interface
 */
err_t
netif_set_default(struct netif *netif)
{
  LWIP_ASSERT_CORE_LOCKED();

  if (netif == NULL) {
    /* remove default route */
    mib2_remove_route_ip4(1, netif);
  } else {
    /* install default route */
    mib2_add_route_ip4(1, netif);
  }
  netif_default = netif;
  LWIP_DEBUGF(NETIF_DEBUG, ("netif: setting default interface %s\n",
                            netif ? netif->name : "\'\'"));
  return ERR_OK;
}

/**
 * @ingroup netif
 * Bring an interface up, available for processing
 * traffic.
 */
err_t
netif_set_up(struct netif *netif)
{
  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("netif_set_up: invalid netif", netif != NULL, return ERR_VAL);

  if (!(netif->flags & NETIF_FLAG_UP)) {
    netif_set_flags(netif, NETIF_FLAG_UP);

    MIB2_COPY_SYSUPTIME_TO(&netif->ts);

    NETIF_STATUS_CALLBACK(netif);

#if LWIP_NETIF_EXT_STATUS_CALLBACK
    {
      netif_ext_callback_args_t args;
      args.status_changed.state = 1;
      netif_invoke_ext_callback(netif, LWIP_NSC_STATUS_CHANGED, &args);
    }
#endif

    netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4 | NETIF_REPORT_TYPE_IPV6, (s8_t)LWIP_INVALID_IPV6_IDX);
#if LWIP_IPV6
    nd6_restart_netif(netif);
#endif /* LWIP_IPV6 */
  }
  return ERR_OK;
}

/** Send ARP/IGMP/MLD/RS events, e.g. on link-up/netif-up or addr-change
 */
static void
netif_issue_reports(struct netif *netif, u8_t report_type, s8_t addr_idx)
{
  LWIP_ERROR("netif_issue_reports : invalid arguments", (netif != NULL), return);

  /* Only send reports when both link and admin states are up */
  if (!(netif->flags & NETIF_FLAG_LINK_UP) ||
      !(netif->flags & NETIF_FLAG_UP)) {
    return;
  }

#if LWIP_IPV4
  if ((report_type & NETIF_REPORT_TYPE_IPV4) &&
      !ip4_addr_isany_val(*netif_ip4_addr(netif))) {
#if LWIP_ARP
    /* For Ethernet network interfaces, we would like to send a "gratuitous ARP" */
    if (netif->flags & (NETIF_FLAG_ETHARP)) {
      etharp_gratuitous(netif);
    }
#endif /* LWIP_ARP */

#if LWIP_IGMP
    /* resend IGMP memberships */
    if (netif->flags & NETIF_FLAG_IGMP) {
      igmp_report_groups(netif);
    }
#endif /* LWIP_IGMP */
#if LWIP_DHCP && LWIP_DHCP_SUBSTITUTE && LWIP_ARP
    dhcp_events_trigger(netif);
#endif /* LWIP_DHCP && LWIP_DHCP_SUBSTITUTE && LWIP_ARP */
  }
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
  if (report_type & NETIF_REPORT_TYPE_IPV6) {
#if LWIP_IPV6_MLD
    /* send mld memberships */
    mld6_report_groups(netif);
#endif /* LWIP_IPV6_MLD */
    /* Send nd6 report messages */
    nd6_report_groups(netif, addr_idx);
  }
#else
  LWIP_UNUSED_ARG(addr_idx);
#endif /* LWIP_IPV6 */
}

/**
 * @ingroup netif
 * Bring an interface down, disabling any traffic processing.
 */
err_t
netif_set_down(struct netif *netif)
{
  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("netif_set_down: invalid netif", netif != NULL, return ERR_VAL);

  if (netif->flags & NETIF_FLAG_UP) {
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    {
      netif_ext_callback_args_t args;
      args.status_changed.state = 0;
      netif_invoke_ext_callback(netif, LWIP_NSC_STATUS_CHANGED, &args);
    }
#endif

    netif_clear_flags(netif, NETIF_FLAG_UP);
    MIB2_COPY_SYSUPTIME_TO(&netif->ts);

#if LWIP_IPV4 && LWIP_ARP
    if (netif->flags & NETIF_FLAG_ETHARP) {
      etharp_cleanup_netif(netif);
    }
#endif /* LWIP_IPV4 && LWIP_ARP */

#if LWIP_IPV6
    nd6_cleanup_netif(netif);
#endif /* LWIP_IPV6 */

    NETIF_STATUS_CALLBACK(netif);
  }

  return ERR_OK;
}

void
netif_get_hwaddr(const struct netif *netif, unsigned char *hw_addr, int hw_len)
{
  int i;
  LWIP_ERROR("netif_get_hwaddr : invalid arguments", (netif != NULL), return);

  LWIP_ERROR("netif_get_hwaddr : invalid arguments", (hw_addr != NULL), return);

  LWIP_ERROR("netif_get_hwaddr: invalid arguments",
             ((unsigned int)hw_len == NETIF_MAX_HWADDR_LEN), return);

  for (i = 0; i < hw_len; i++) {
    hw_addr[i] = netif->hwaddr[i];
  }
}

/*
 * Change the hardware address for a network interface
 *
 * NOTE:Application should call set_link_down before calling
 * the netif_set_hwaddr and then set_link_up, in order
 * to ensure that all IPs are announced after changing
 * hw address
 *
 * @param netif the network interface to change
 * @param hw_addr the new hardware address
 * @param hw_len the length of new hardware address,
 *
 */
err_t
netif_set_hwaddr(struct netif *netif, const unsigned char *hw_addr, int hw_len)
{
  LWIP_ERROR("netif_set_hwaddr : invalid arguments", (netif != NULL), return ERR_VAL);

  LWIP_ERROR("netif_set_hwaddr : invalid arguments", (hw_addr != NULL), return ERR_VAL);

  LWIP_ERROR("netif_set_hwaddr: invalid arguments",
             ((unsigned int)hw_len == NETIF_MAX_HWADDR_LEN), return ERR_VAL);

  if (netif->drv_set_hwaddr == NULL) {
    return ERR_OPNOTSUPP;
  }

  if (netif->drv_set_hwaddr(netif, (u8_t *)hw_addr, hw_len) != ERR_OK) {
    return ERR_VAL;
  }

  if (memcpy_s(netif->hwaddr, NETIF_MAX_HWADDR_LEN, hw_addr, (u32_t)hw_len) != EOK) {
    LWIP_DEBUGF(NETIF_DEBUG, ("netif_set_hwaddr: memcpy_s error\n"));
    return ERR_VAL;
  }

#if LWIP_DHCP && LWIP_DHCP_SUBSTITUTE && LWIP_ARP
  dhcp_events_record(netif, DHCP_EVENT_HW);
#endif /* LWIP_DHCP && LWIP_DHCP_SUBSTITUTE && LWIP_ARP */

  LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
              ("netif: HW address of interface %s%"U16_F" set to %02X:%02X:%02X:%02X:%02X:%02X\n",
              netif->name, netif->num,
              netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2],
              netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]));

  return ERR_OK;
}

#if LWIP_NETIF_STATUS_CALLBACK
/**
 * @ingroup netif
 * Set callback to be called when interface is brought up/down or address is changed while up
 */
void
netif_set_status_callback(struct netif *netif, netif_status_callback_fn status_callback)
{
  LWIP_ASSERT_CORE_LOCKED();

  if (netif) {
    netif->status_callback = status_callback;
  }
}
#endif /* LWIP_NETIF_STATUS_CALLBACK */

#if LWIP_NETIF_REMOVE_CALLBACK
/**
 * @ingroup netif
 * Set callback to be called when the interface has been removed
 */
void
netif_set_remove_callback(struct netif *netif, netif_status_callback_fn remove_callback)
{
  LWIP_ASSERT_CORE_LOCKED();

  if (netif) {
    netif->remove_callback = remove_callback;
  }
}
#endif /* LWIP_NETIF_REMOVE_CALLBACK */

/**
 * @ingroup netif
 * Called by a driver when its link goes up
 */
err_t
netif_set_link_up(struct netif *netif)
{
  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("netif_set_link_up: invalid netif", netif != NULL, return ERR_VAL);

  if (!(netif->flags & NETIF_FLAG_LINK_UP)) {
    netif_set_flags(netif, NETIF_FLAG_LINK_UP);

#if LWIP_DHCP
    dhcp_network_changed(netif);
#endif /* LWIP_DHCP */

#if LWIP_AUTOIP
    autoip_network_changed(netif);
#endif /* LWIP_AUTOIP */

    netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4 | NETIF_REPORT_TYPE_IPV6, (s8_t)LWIP_INVALID_IPV6_IDX);
#if LWIP_IPV6
    nd6_restart_netif(netif);
#endif /* LWIP_IPV6 */

    NETIF_LINK_CALLBACK(netif);
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    {
      netif_ext_callback_args_t args;
      args.link_changed.state = 1;
      netif_invoke_ext_callback(netif, LWIP_NSC_LINK_CHANGED, &args);
    }
#endif
  }
  return ERR_OK;
}

/**
 * @ingroup netif
 * Called by a driver when its link goes down
 */
err_t
netif_set_link_down(struct netif *netif)
{
  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("netif_set_link_down: invalid netif", netif != NULL, return ERR_VAL);

  if (netif->flags & NETIF_FLAG_LINK_UP) {
    netif_clear_flags(netif, NETIF_FLAG_LINK_UP);
#if LWIP_IPV6 && LWIP_ND6_ALLOW_RA_UPDATES
    netif->mtu6 = netif->mtu;
#endif

    NETIF_LINK_CALLBACK(netif);
#if LWIP_NETIF_EXT_STATUS_CALLBACK
    {
      netif_ext_callback_args_t args;
      args.link_changed.state = 0;
      netif_invoke_ext_callback(netif, LWIP_NSC_LINK_CHANGED, &args);
    }
#endif
#if LWIP_NEIGHBOR_CLEANUP_WHILE_NETIF_LINK_DOWN
#if LWIP_IPV4 && LWIP_ARP
    if (netif->flags & NETIF_FLAG_ETHARP) {
      etharp_cleanup_netif(netif);
    }
#endif /* LWIP_IPV4 && LWIP_ARP */
#if LWIP_IPV6
    nd6_cleanup_netif(netif);
#endif /* LWIP_IPV6 */
#endif /* LWIP_NEIGHBOR_CLEANUP_WHILE_NETIF_LINK_DOWN */
  }
  return ERR_OK;
}

/* This is called via tcpip_callback_with_block in wifi driver
 * when wal reports that STA has associated with AP.
 */
void netif_set_link_up_interface(void *arg)
{
  struct netif *netif = (struct netif *)arg;
  if (netif == NULL) {
    return;
  }
  (void)netif_set_link_up(netif);
#if LWIP_IPV6_AUTOCONFIG
  netif_set_ip6_autoconfig_enabled(netif);
#endif
#if LWIP_IPV6
#if LWIP_ND6_ROUTER
  netif->forwarding = lwIP_FALSE;
  netif->ra_enable = lwIP_FALSE;
  netif->accept_ra = lwIP_TRUE;
#endif
  (void)netif_create_ip6_linklocal_address(netif, lwIP_TRUE);
#endif
}

/* This is called via tcpip_callback_with_block in wifi driver
 * when wal reports that STA has disassociated with AP.
 */
void netif_set_link_down_interface(void *arg)
{
  struct netif *netif = (struct netif *)arg;
  if (netif == NULL) {
    return;
  }
  (void)netif_set_link_down(netif);
#if LWIP_IPV4
  (void)netif_set_addr(netif, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
#endif
#if LWIP_IPV6
  int index;
  for (index = 0; index < LWIP_IPV6_NUM_ADDRESSES; index++) {
   (void)netif_do_rmv_ipv6_addr(netif, (void *)(&netif->ip6_addr[index]));
  }
#endif
}

#if LWIP_NETIF_LINK_CALLBACK
/**
 * @ingroup netif
 * Set callback to be called when link is brought up/down
 */
err_t
netif_set_link_callback(struct netif *netif, netif_status_callback_fn link_callback)
{
  LWIP_ASSERT_CORE_LOCKED();
  LWIP_ERROR("netif_set_link_callback: invalid value.", (!((netif == NULL) || (link_callback == NULL))),
               return ERR_VAL);

  netif->link_callback = link_callback;
  return ERR_OK;
}
#endif /* LWIP_NETIF_LINK_CALLBACK */

#if ENABLE_LOOPBACK
/**
 * @ingroup netif
 * Send an IP packet to be received on the same netif (loopif-like).
 * The pbuf is copied and added to an internal queue which is fed to
 * netif->input by netif_poll().
 * In multithreaded mode, the call to netif_poll() is queued to be done on the
 * TCP/IP thread.
 * In callback mode, the user has the responsibility to call netif_poll() in
 * the main loop of their application.
 *
 * @param netif the lwip network interface structure
 * @param p the (IP) packet to 'send'
 * @return ERR_OK if the packet has been sent
 *         ERR_MEM if the pbuf used to copy the packet couldn't be allocated
 */
err_t
netif_loop_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *r;
  err_t err;
  struct pbuf *last;
#if LWIP_LOOPBACK_MAX_PBUFS
  u16_t clen = 0;
#endif /* LWIP_LOOPBACK_MAX_PBUFS */
  /* If we have a loopif, SNMP counters are adjusted for it,
   * if not they are adjusted for 'netif'. */
#if MIB2_STATS
#if LWIP_HAVE_LOOPIF
  struct netif *stats_if = &loop_netif;
#else /* LWIP_HAVE_LOOPIF */
  struct netif *stats_if = netif;
#endif /* LWIP_HAVE_LOOPIF */
#endif /* MIB2_STATS */
#if LWIP_NETIF_LOOPBACK_MULTITHREADING
  u8_t schedule_poll = 0;
#endif /* LWIP_NETIF_LOOPBACK_MULTITHREADING */
  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_ASSERT("netif_loop_output: invalid netif", netif != NULL);
  LWIP_ASSERT("netif_loop_output: invalid pbuf", p != NULL);

  /* Allocate a new pbuf */
  r = pbuf_alloc(PBUF_LINK, p->tot_len, PBUF_RAM);
  if (r == NULL) {
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
    MIB2_STATS_NETIF_INC(stats_if, ifoutdiscards);
    return ERR_MEM;
  }
#if LWIP_LOOPBACK_MAX_PBUFS
  clen = pbuf_clen(r);
  /* check for overflow or too many pbuf on queue */
  if (((netif->loop_cnt_current + clen) < netif->loop_cnt_current) ||
      ((netif->loop_cnt_current + clen) > LWIP_MIN(LWIP_LOOPBACK_MAX_PBUFS, 0xFFFF))) {
    pbuf_free(r);
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
    MIB2_STATS_NETIF_INC(stats_if, ifoutdiscards);
    return ERR_MEM;
  }
  netif->loop_cnt_current = (u16_t)(netif->loop_cnt_current + clen);
#endif /* LWIP_LOOPBACK_MAX_PBUFS */

  /* Copy the whole pbuf queue p into the single pbuf r */
  if ((err = pbuf_copy(r, p)) != ERR_OK) {
    pbuf_free(r);
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
    MIB2_STATS_NETIF_INC(stats_if, ifoutdiscards);
    return err;
  }
  r->flags = p->flags;
  /* Put the packet on a linked list which gets emptied through calling
     netif_poll(). */

  /* let last point to the last pbuf in chain r */
  for (last = r; last->next != NULL; last = last->next) {
    /* nothing to do here, just get to the last pbuf */
  }

  SYS_ARCH_PROTECT(lev);
  if (netif->loop_first != NULL) {
    LWIP_ASSERT("if first != NULL, last must also be != NULL", netif->loop_last != NULL);
    netif->loop_last->next = r;
    netif->loop_last = last;
#if LWIP_NETIF_LOOPBACK_MULTITHREADING
    if (netif->reschedule_poll) {
      schedule_poll = 1;
      netif->reschedule_poll = 0;
    }
#endif /* LWIP_NETIF_LOOPBACK_MULTITHREADING */
  } else {
    netif->loop_first = r;
    netif->loop_last = last;
#if LWIP_NETIF_LOOPBACK_MULTITHREADING
    /* No existing packets queued, schedule poll */
    schedule_poll = 1;
#endif /* LWIP_NETIF_LOOPBACK_MULTITHREADING */
  }
  SYS_ARCH_UNPROTECT(lev);

  LINK_STATS_INC(link.xmit);
  MIB2_STATS_NETIF_ADD(stats_if, ifoutoctets, p->tot_len);
  MIB2_STATS_NETIF_INC(stats_if, ifoutucastpkts);

#if LWIP_NETIF_LOOPBACK_MULTITHREADING
  /* For multithreading environment, schedule a call to netif_poll */
  if (schedule_poll) {
    if (tcpip_try_callback((tcpip_callback_fn)netif_poll, netif) != ERR_OK) {
      SYS_ARCH_PROTECT(lev);
      netif->reschedule_poll = 1;
      SYS_ARCH_UNPROTECT(lev);
    }
  }
#endif /* LWIP_NETIF_LOOPBACK_MULTITHREADING */

  return ERR_OK;
}

#if LWIP_HAVE_LOOPIF
#if LWIP_IPV4
static err_t
netif_loop_output_ipv4(struct netif *netif, struct pbuf *p, const ip4_addr_t *addr)
{
  LWIP_UNUSED_ARG(addr);
  return netif_loop_output(netif, p);
}
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
static err_t
netif_loop_output_ipv6(struct netif *netif, struct pbuf *p, const ip6_addr_t *addr)
{
  LWIP_UNUSED_ARG(addr);
  return netif_loop_output(netif, p);
}
#endif /* LWIP_IPV6 */
#endif /* LWIP_HAVE_LOOPIF */


/**
 * Call netif_poll() in the main loop of your application. This is to prevent
 * reentering non-reentrant functions like tcp_input(). Packets passed to
 * netif_loop_output() are put on a list that is passed to netif->input() by
 * netif_poll().
 */
void
netif_poll(struct netif *netif)
{
  /* If we have a loopif, SNMP counters are adjusted for it,
   * if not they are adjusted for 'netif'. */
#if MIB2_STATS
#if LWIP_HAVE_LOOPIF
  struct netif *stats_if = &loop_netif;
#else /* LWIP_HAVE_LOOPIF */
  struct netif *stats_if = netif;
#endif /* LWIP_HAVE_LOOPIF */
#endif /* MIB2_STATS */
#if PF_PKT_SUPPORT
  struct raw_pcb *pcb;
  int match = 0;
#endif

  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_ASSERT("netif_poll: invalid netif", netif != NULL);

  /* Get a packet from the list. With SYS_LIGHTWEIGHT_PROT=1, this is protected */
  SYS_ARCH_PROTECT(lev);
  while (netif->loop_first != NULL) {
    struct pbuf *in, *in_end;
#if LWIP_LOOPBACK_MAX_PBUFS
    u8_t clen = 1;
#endif /* LWIP_LOOPBACK_MAX_PBUFS */

    in = in_end = netif->loop_first;
    while (in_end->len != in_end->tot_len) {
      LWIP_ASSERT("bogus pbuf: len != tot_len but next == NULL!", in_end->next != NULL);
      in_end = in_end->next;
#if LWIP_LOOPBACK_MAX_PBUFS
      clen++;
#endif /* LWIP_LOOPBACK_MAX_PBUFS */
    }
#if LWIP_LOOPBACK_MAX_PBUFS
    /* adjust the number of pbufs on queue */
    LWIP_ASSERT("netif->loop_cnt_current underflow",
                ((netif->loop_cnt_current - clen) < netif->loop_cnt_current));
    netif->loop_cnt_current = (u16_t)(netif->loop_cnt_current - clen);
#endif /* LWIP_LOOPBACK_MAX_PBUFS */

    /* 'in_end' now points to the last pbuf from 'in' */
    if (in_end == netif->loop_last) {
      /* this was the last pbuf in the list */
      netif->loop_first = netif->loop_last = NULL;
    } else {
      /* pop the pbuf off the list */
      netif->loop_first = in_end->next;
      LWIP_ASSERT("should not be null since first != last!", netif->loop_first != NULL);
    }
    /* De-queue the pbuf from its successors on the 'loop_' list. */
    in_end->next = NULL;
    SYS_ARCH_UNPROTECT(lev);

    in->if_idx = netif_get_index(netif);

    LINK_STATS_INC(link.recv);
    MIB2_STATS_NETIF_ADD(stats_if, ifinoctets, in->tot_len);
    MIB2_STATS_NETIF_INC(stats_if, ifinucastpkts);

#if PF_PKT_SUPPORT
      for (pcb = pkt_raw_pcbs; pcb != NULL; pcb = pcb->next) {
        if (((pcb->proto.eth_proto == htons(ETHTYPE_ALL)) || (pcb->proto.eth_proto == htons(ETHTYPE_IP))) &&
            ((!pcb->netifindex) || (pcb->netifindex == netif->ifindex))) {
          match = 1;
          break;
        }
      }

      /* always succeed because the alloc layer of loopback pbuf was PBUF_LINK */
      if (match && pbuf_header(in, PBUF_LINK_HLEN) == 0) {
        struct eth_hdr *ethhdr;
        /* add ethernet header */
        ethhdr = (struct eth_hdr *)(in->payload);
        /* smac and dmac set to all zeros for loopback IP packet */
        (void)memset_s(ethhdr, sizeof(struct eth_hdr), 0, sizeof(struct eth_hdr));
        ethhdr->type = htons(ETHTYPE_IP); /* eth protocol, should be ETH_P_IP(0x800) */
        in->flags = (u16_t)(in->flags & ~(PBUF_FLAG_LLMCAST | PBUF_FLAG_LLBCAST | PBUF_FLAG_OUTGOING));
        in->flags |= PBUF_FLAG_HOST;
        raw_pkt_input(in, netif, NULL);
        (void)pbuf_header(in, -PBUF_LINK_HLEN);
      }
#endif /* PF_PKT_SUPPORT */

    /* loopback packets are always IP packets! */
    if (ip_input(in, netif) != ERR_OK) {
      pbuf_free(in);
    }
    SYS_ARCH_PROTECT(lev);
  }
  SYS_ARCH_UNPROTECT(lev);
}

#if !LWIP_NETIF_LOOPBACK_MULTITHREADING
/**
 * Calls netif_poll() for every netif on the netif_list.
 */
void
netif_poll_all(void)
{
  struct netif *netif;
  /* loop through netifs */
  NETIF_FOREACH(netif) {
    netif_poll(netif);
  }
}
#endif /* !LWIP_NETIF_LOOPBACK_MULTITHREADING */
#endif /* ENABLE_LOOPBACK */

#if LWIP_NUM_NETIF_CLIENT_DATA > 0
/**
 * @ingroup netif_cd
 * Allocate an index to store data in client_data member of struct netif.
 * Returned value is an index in mentioned array.
 * @see LWIP_NUM_NETIF_CLIENT_DATA
 */
u8_t
netif_alloc_client_data_id(void)
{
  u8_t result = netif_client_id;
  netif_client_id++;

  LWIP_ASSERT_CORE_LOCKED();

#if LWIP_NUM_NETIF_CLIENT_DATA + LWIP_NETIF_CLIENT_DATA_INDEX_MAX > 255
#error LWIP_NUM_NETIF_CLIENT_DATA + LWIP_NETIF_CLIENT_DATA_INDEX_MAX must be <= 255
#endif
  LWIP_ASSERT("Increase LWIP_NUM_NETIF_CLIENT_DATA in lwipopts.h", result < LWIP_NUM_NETIF_CLIENT_DATA);
  return (u8_t)(result + LWIP_NETIF_CLIENT_DATA_INDEX_MAX);
}
#endif

#if LWIP_IPV6
/**
 * @ingroup netif_ip6
 * Change an IPv6 address of a network interface
 *
 * @param netif the network interface to change
 * @param addr_idx index of the IPv6 address
 * @param addr6 the new IPv6 address
 *
 * @note call netif_ip6_addr_set_state() to set the address valid/temptative
 */
void
netif_ip6_addr_set(struct netif *netif, s8_t addr_idx, const ip6_addr_t *addr6)
{
  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ASSERT("netif_ip6_addr_set: invalid netif", netif != NULL);
  LWIP_ASSERT("netif_ip6_addr_set: invalid addr6", addr6 != NULL);

  netif_ip6_addr_set_parts(netif, addr_idx, addr6->addr[0], addr6->addr[1],
                           addr6->addr[2], addr6->addr[3]);
}

err_t netif_do_add_ipv6_addr(struct netif *netif, void *arguments)
{
  s8_t idx = -1;
  ip_addr_t *ipaddr = (ip_addr_t *)arguments;
  err_t err;
  if (IP_IS_V6(ipaddr)) {
    err = netif_add_ip6_address(netif, ip_2_ip6(ipaddr), &idx);
    if ((err != ERR_OK) || (idx == -1)) {
      return ERR_BUF;
    }
    netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV6, idx);
  }
  return ERR_OK;
}

err_t netif_do_rmv_ipv6_addr(struct netif *netif, void *arguments)
{
  ip_addr_t *ipaddr = (ip_addr_t *)arguments;

  if (IP_IS_V6(ipaddr)) {
#if LWIP_TCP
    tcp_unlock_accept(ip_2_ip6(ipaddr));
#endif
    netif_ip6_addr_setinvalid(netif, ip_2_ip6(ipaddr));
  }
  return ERR_OK;
}

/*
 * Change an IPv6 address of a network interface (internal version taking 4 * u32_t)
 *
 * @param netif the network interface to change
 * @param addr_idx index of the IPv6 address
 * @param i0 word0 of the new IPv6 address
 * @param i1 word1 of the new IPv6 address
 * @param i2 word2 of the new IPv6 address
 * @param i3 word3 of the new IPv6 address
 */
void
netif_ip6_addr_set_parts(struct netif *netif, s8_t addr_idx, u32_t i0, u32_t i1, u32_t i2, u32_t i3)
{
  ip_addr_t old_addr;
  ip_addr_t new_ipaddr;
  LWIP_ASSERT_CORE_LOCKED();
  LWIP_ASSERT("netif != NULL", netif != NULL);
  LWIP_ASSERT("invalid index", addr_idx != LWIP_IPV6_NUM_ADDRESSES);

  ip6_addr_copy(*ip_2_ip6(&old_addr), *netif_ip6_addr(netif, addr_idx));
  IP_SET_TYPE_VAL(old_addr, IPADDR_TYPE_V6);

  /* address is actually being changed? */
  if ((ip_2_ip6(&old_addr)->addr[0] != i0) || (ip_2_ip6(&old_addr)->addr[1] != i1) ||
      (ip_2_ip6(&old_addr)->addr[2] != i2) || (ip_2_ip6(&old_addr)->addr[3] != i3)) {
    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_STATE, ("netif_ip6_addr_set: netif address being changed\n"));

    IP_ADDR6(&new_ipaddr, i0, i1, i2, i3);
    ip6_addr_assign_zone(ip_2_ip6(&new_ipaddr), IP6_UNICAST, netif);

    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, addr_idx))) {
      netif_do_ip_addr_changed(netif_ip_addr6(netif, addr_idx), &new_ipaddr);
    }
    /* @todo: remove/readd mib2 ip6 entries? */

    ip_addr_copy(netif->ip6_addr[addr_idx], new_ipaddr);

    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, addr_idx))) {
      netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV6, addr_idx);
      NETIF_STATUS_CALLBACK(netif);
    }

#if LWIP_NETIF_EXT_STATUS_CALLBACK
    {
      netif_ext_callback_args_t args;
      args.ipv6_set.addr_index  = addr_idx;
      args.ipv6_set.old_address = &old_addr;
      netif_invoke_ext_callback(netif, LWIP_NSC_IPV6_SET, &args);
    }
#endif
  }
#ifdef LWIP_DEBUG
  char buf[IP6ADDR_STRLEN_MAX];
  (void)ip6addr_ntoa_r(netif_ip6_addr(netif, addr_idx), buf, IP6ADDR_STRLEN_MAX);
#endif
  LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
              ("netif: IPv6 address %d of interface %s set to %s/0x%"X8_F"\n",
              addr_idx, netif->name, buf,
              netif_ip6_addr_state(netif, addr_idx)));
}

/**
 * @ingroup netif_ip6
 * Change the state of an IPv6 address of a network interface
 * (INVALID, TEMPTATIVE, PREFERRED, DEPRECATED, where TEMPTATIVE
 * includes the number of checks done, see ip6_addr.h)
 *
 * @param netif the network interface to change
 * @param addr_idx index of the IPv6 address
 * @param state the new IPv6 address state
 */
void
netif_ip6_addr_set_state(struct netif *netif, s8_t addr_idx, u8_t state)
{
  u8_t old_state;
  LWIP_ASSERT_CORE_LOCKED();
  LWIP_ERROR("netif != NULL", netif != NULL, return);
  LWIP_ERROR("invalid index", (addr_idx != LWIP_IPV6_NUM_ADDRESSES), return);

  old_state = netif_ip6_addr_state(netif, addr_idx);
  /* state is actually being changed? */
  if (old_state != state) {
    u8_t old_valid = old_state & IP6_ADDR_VALID;
    u8_t new_valid = state & IP6_ADDR_VALID;
    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_STATE, ("netif_ip6_addr_set_state: netif address state being changed\n"));

#if LWIP_IPV6_MLD
    /* Reevaluate solicited-node multicast group membership. */
    if (netif->flags & NETIF_FLAG_MLD6) {
      nd6_adjust_mld_membership(netif, addr_idx, state);
    }
#endif /* LWIP_IPV6_MLD */

    if (old_valid && !new_valid) {
      /* address about to be removed by setting invalid */
      netif_do_ip_addr_changed(netif_ip_addr6(netif, addr_idx), NULL);
      /* @todo: remove mib2 ip6 entries? */
    }
    netif->ip6_addr_state[addr_idx] = state;

    if (!old_valid && new_valid) {
      /* address added by setting valid */
      /* This is a good moment to check that the address is properly zoned. */
      IP6_ADDR_ZONECHECK_NETIF(netif_ip6_addr(netif, addr_idx), netif);
      /* @todo: add mib2 ip6 entries? */
      netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV6, addr_idx);
    }
    if ((old_state & ~IP6_ADDR_TENTATIVE_COUNT_MASK) !=
        (state     & ~IP6_ADDR_TENTATIVE_COUNT_MASK)) {
      /* address state has changed -> call the callback function */
      NETIF_STATUS_CALLBACK(netif);
    }

#if LWIP_NETIF_EXT_STATUS_CALLBACK
    {
      netif_ext_callback_args_t args;
      args.ipv6_addr_state_changed.addr_index = addr_idx;
      args.ipv6_addr_state_changed.old_state  = old_state;
      args.ipv6_addr_state_changed.address    = netif_ip_addr6(netif, addr_idx);
      netif_invoke_ext_callback(netif, LWIP_NSC_IPV6_ADDR_STATE_CHANGED, &args);
    }
#endif
  }
#ifdef LWIP_DEBUG
  char buf[IP6ADDR_STRLEN_MAX];
  (void)ip6addr_ntoa_r(netif_ip6_addr(netif, addr_idx), buf, IP6ADDR_STRLEN_MAX);
#endif
  LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
              ("netif: IPv6 address %d of interface %s set to %s/0x%"X8_F"\n",
              addr_idx, netif->name, buf,
              netif_ip6_addr_state(netif, addr_idx)));
}

/**
 * Checks if a specific local address is present on the netif and returns its
 * index. Depending on its state, it may or may not be assigned to the
 * interface (as per RFC terminology).
 *
 * The given address may or may not be zoned (i.e., have a zone index other
 * than IP6_NO_ZONE). If the address is zoned, it must have the correct zone
 * for the given netif, or no match will be found.
 *
 * @param netif the netif to check
 * @param ip6addr the IPv6 address to find
 * @return >= 0: address found, this is its index
 *         -1: address not found on this netif
 */
s8_t
netif_get_ip6_addr_match(struct netif *netif, const ip6_addr_t *ip6addr)
{
  s8_t i;

  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("netif_get_ip6_addr_match : invalid arguments", (netif != NULL), return -1);
  LWIP_ERROR("netif_get_ip6_addr_match: invalid ip6addr", (ip6addr != NULL), return -1);

#if LWIP_IPV6_SCOPES
  if (ip6_addr_has_zone(ip6addr) && !ip6_addr_test_zone(ip6addr, netif)) {
    return -1; /* wrong zone, no match */
  }
#endif /* LWIP_IPV6_SCOPES */

  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (!ip6_addr_isinvalid(netif_ip6_addr_state(netif, i)) &&
        ip6_addr_cmp_zoneless(netif_ip6_addr(netif, i), ip6addr)) {
      return i;
    }
  }
  return -1;
}

/*
 * Checks if a specific address is assigned to the netif and returns its
 * index.
 *
 * @param netif the netif to check
 * @param ip6addr the IPv6 address to find
 * @return >= 0: address found, this is its index
 *         -1: address not found on this netif
 */
static s8_t
netif_get_ip6_preferred_addr_match(const struct netif *netif, const ip6_addr_t *ip6addr)
{
  s8_t i;
  LWIP_ERROR("netif_get_ip6_preferred_addr_match : invalid arguments", (netif != NULL), return -1);

  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
        ip6_addr_cmp(netif_ip6_addr(netif, i), ip6addr)) {
      return i;
    }
  }
  return -1;
}

err_t
netif_get_ip6_linklocal_address(const struct netif *netif, ip6_addr_t *addr)
{
  LWIP_ERROR("netif_get_ip6_linklocal_address : invalid arguments netif", (netif != NULL), return ERR_VAL);
  LWIP_ERROR("netif_get_ip6_linklocal_address : invalid arguments addr", (addr != NULL), return ERR_VAL);

  if (ip6_addr_isany(netif_ip6_addr(netif, 0))) {
    /* link local address not yet configured */
    return ERR_VAL;
  }

  ip6_addr_copy_ptr(addr, netif_ip6_addr(netif, 0));
  return ERR_OK;
}

#if LWIP_RIPPLE
err_t
netif_create_ip6_linklocal_address_from_mac(const linklayer_addr_t *mac, ip6_addr_t *ip6addr)
{
  if ((mac == NULL) || (ip6addr == NULL) || (mac->addrlen != NETIF_MAX_HWADDR_LEN)) {
    return ERR_ARG;
  }

  ip6addr->addr[0] = PP_HTONL(0xfe800000ul);
  ip6addr->addr[1] = 0;
  ip6addr->addr[2] = lwip_htonl((((u32_t)(mac->addr[0] ^ 0x02)) << 24) |
                                ((u32_t)(mac->addr[1]) << 16) |
                                ((u32_t)(mac->addr[2]) << 8) | (0xff));
  ip6addr->addr[3] = lwip_htonl((u32_t)(0xfeul << 24) |
                                ((u32_t)(mac->addr[3]) << 16) |
                                ((u32_t)(mac->addr[4]) << 8) |
                                ((u32_t)(mac->addr[5])));
  return ERR_OK;
}
#endif /* LWIP_RIPPLE */

/**
 * @ingroup netif_ip6
 * Create a link-local IPv6 address on a netif (stored in slot 0)
 *
 * @param netif the netif to create the address on
 * @param from_mac_48bit if != 0, assume hwadr is a 48-bit MAC address (std conversion)
 *                       if == 0, use hwaddr directly as interface ID
 */
err_t
netif_create_ip6_linklocal_address(struct netif *netif, u8_t from_mac_48bit)
{
  u8_t i, addr_index;

  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("netif_create_ip6_linklocal_address : invalid arguments", (netif != NULL), return ERR_VAL);

  /* Link-local prefix. */
  ip_2_ip6(&netif->ip6_addr[0])->addr[0] = PP_HTONL(0xfe800000ul);
  ip_2_ip6(&netif->ip6_addr[0])->addr[1] = 0;

  /* Generate interface ID. */
  if (from_mac_48bit) {
    /* Assume hwaddr is a 48-bit IEEE 802 MAC. Convert to EUI-64 address. Complement Group bit. */
    ip_2_ip6(&netif->ip6_addr[0])->addr[2] = lwip_htonl((((u32_t)(netif->hwaddr[0] ^ 0x02)) << 24) |
        ((u32_t)(netif->hwaddr[1]) << 16) |
        ((u32_t)(netif->hwaddr[2]) << 8) |
        (0xff));
    ip_2_ip6(&netif->ip6_addr[0])->addr[3] = lwip_htonl((u32_t)(0xfeul << 24) |
        ((u32_t)(netif->hwaddr[3]) << 16) |
        ((u32_t)(netif->hwaddr[4]) << 8) |
        (netif->hwaddr[5]));
  } else {
    /* Use hwaddr directly as interface ID. */
    ip_2_ip6(&netif->ip6_addr[0])->addr[2] = 0;
    ip_2_ip6(&netif->ip6_addr[0])->addr[3] = 0;

    addr_index = 3;
    for (i = 0; (i < 8) && (i < netif->hwaddr_len); i++) {
      if (i == 4) {
        addr_index--;
      }
      ip_2_ip6(&netif->ip6_addr[0])->addr[addr_index] |= lwip_htonl(((u32_t)(netif->hwaddr[netif->hwaddr_len - i - 1])) << (8 * (i & 0x03)));
    }
  }

  /* Set a link-local zone. Even though the zone is implied by the owning
   * netif, setting the zone anyway has two important conceptual advantages:
   * 1) it avoids the need for a ton of exceptions in internal code, allowing
   *    e.g. ip6_addr_cmp() to be used on local addresses;
   * 2) the properly zoned address is visible externally, e.g. when any outside
   *    code enumerates available addresses or uses one to bind a socket.
   * Any external code unaware of address scoping is likely to just ignore the
   * zone field, so this should not create any compatibility problems. */
  ip6_addr_assign_zone(ip_2_ip6(&netif->ip6_addr[0]), IP6_UNICAST, netif);

  /* Set address state. */
#if LWIP_IPV6_DUP_DETECT_ATTEMPTS
  /* Will perform duplicate address detection (DAD). */
  if (LWIP_IS_DAD_ENABLED(netif)) {
    netif_ip6_addr_set_state(netif, 0, IP6_ADDR_TENTATIVE);
  } else {
    netif_ip6_addr_set_state(netif, 0, IP6_ADDR_PREFERRED);
  }
#else
  /* Consider address valid. */
  netif_ip6_addr_set_state(netif, 0, IP6_ADDR_PREFERRED);
#endif /* LWIP_IPV6_AUTOCONFIG */

  return ERR_OK;
}

#if defined(LWIP_RA_PREFIX_DYNAMIC) && LWIP_RA_PREFIX_DYNAMIC
void
netif_create_ip6_address_80bit_prefix(struct netif *netif, const ip6_addr_t *prefix, ip6_addr_t *ip6addr)
{
  if ((netif == NULL) || (prefix == NULL) || (ip6addr == NULL)) {
    return;
  }

  ip6addr->addr[0] = prefix->addr[0];
  ip6addr->addr[1] = prefix->addr[1];
  ip6addr->addr[2] = lwip_htonl((lwip_ntohl(prefix->addr[2]) & 0xffff0000ul) |
                                ((u32_t)(netif->hwaddr[0]) << 8) |
                                ((u32_t)(netif->hwaddr[1])));
  ip6addr->addr[3] = lwip_htonl(((u32_t)(netif->hwaddr[2]) << 24) |
                                ((u32_t)(netif->hwaddr[3]) << 16) |
                                ((u32_t)(netif->hwaddr[4]) << 8) |
                                ((u32_t)(netif->hwaddr[5])));
  ip6_addr_clear_zone(ip6addr);
}
#endif

/**
 * @ingroup netif_ip6
 * This function allows for the easy addition of a new IPv6 address to an interface.
 * It takes care of finding an empty slot and then sets the address tentative
 * (to make sure that all the subsequent processing happens).
 *
 * @param netif netif to add the address on
 * @param ip6addr address to add
 * @param chosen_idx if != NULL, the chosen IPv6 address index will be stored here
 */
err_t
netif_add_ip6_address(struct netif *netif, const ip6_addr_t *ip6addr, s8_t *chosen_idx)
{
  s8_t i;

  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("netif_add_ip6_address : invalid arguments", (netif != NULL), return ERR_ARG);
  LWIP_ERROR("netif_add_ip6_address : Expecting ip6addr to be not NULL ", (ip6addr != NULL), return ERR_ARG);

  i = netif_get_ip6_addr_match(netif, ip6addr);
  if (i >= 0) {
    /* Address already added */
    if (chosen_idx != NULL) {
      *chosen_idx = i;
    }
    return ERR_OK;
  }

  if (ip6_addr_isinvalid(netif_ip6_addr_state(netif, 0))) {
    (void)netif_create_ip6_linklocal_address(netif, 1);
  }

  /* Find a free slot. The first one is reserved for link-local addresses. */
  for (i = ip6_addr_islinklocal(ip6addr) ? 0 : 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if ((ip6_addr_isinvalid(netif_ip6_addr_state(netif, i)) ||
        ip6_addr_isduplicated(netif_ip6_addr_state(netif, i))) && !netif_ip6_addr_isdhcp6(netif, i)) {
      ip_addr_copy_from_ip6(netif->ip6_addr[i], *ip6addr);
      ip6_addr_assign_zone(ip_2_ip6(&netif->ip6_addr[i]), IP6_UNICAST, netif);
      if (LWIP_IS_DAD_ENABLED(netif)) {
        netif_ip6_addr_set_state(netif, i, IP6_ADDR_TENTATIVE);
      } else {
        netif_ip6_addr_set_state(netif, i, IP6_ADDR_PREFERRED);
      }
      if (chosen_idx != NULL) {
        *chosen_idx = i;
      }
      return ERR_OK;
    }
  }

  if (chosen_idx != NULL) {
    *chosen_idx = -1;
  }
  return ERR_VAL;
}

void
netif_ip6_addr_setinvalid(struct netif *netif, const ip6_addr_t *addr6)
{
  s8_t idx;
  LWIP_ERROR("netif_ip6_addr_set : invalid arguments", (netif != NULL), return);
  LWIP_ERROR("netif_ip6_addr_set : invalid arguments", (addr6 != NULL), return);

  idx = netif_get_ip6_addr_match(netif, addr6);
  if (idx < 0) {
    return;
  }

  netif_ip6_addr_set_state(netif, idx, IP6_ADDR_INVALID);
  return;
}

struct netif *
netif_find_by_ip6addr(const ip6_addr_t *ip6addr)
{
  struct netif *ni = NULL;
  int i;

  LWIP_ERROR("netif_find_by_ip6addr :  Expecting ip6addr to be not NULL ", (ip6addr != NULL), return NULL);

  for (ni = netif_list; ni != NULL; ni = ni->next) {
    i = netif_get_ip6_preferred_addr_match(ni, ip6addr);
    if (i >= 0) {
      return ni;
    }
  }

  return NULL;
}

#if LWIP_IPV6_AUTOCONFIG
void
netif_set_ip6_autoconfig_enabled(struct netif *netif)
{
  LWIP_ERROR("netif_set_ip6_autoconfig_enabled : invalid arguments", (netif != NULL), return);
  netif->ip6_autoconfig_enabled = 1;
#if LWIP_IPV6_SEND_ROUTER_SOLICIT
    /* Send RS packet to obtain RA quickly */
    (void)nd6_send_rs(netif);
#endif
  return;
}

void
netif_set_ip6_autoconfig_disabled(struct netif *netif)
{
  LWIP_ERROR("netif_set_ip6_autoconfig_disabled : invalid arguments", (netif != NULL), return);
  netif->ip6_autoconfig_enabled = 0;

  /* invalid autoconfig generated addresses */
  nd6_clear_netif_autoconf_address(netif);
  return;
}
#endif /* LWIP_IPV6_AUTOCONFIG */

#endif /* LWIP_IPV6 */

/*
 * Change mtu setting for a network interface
 *
 * @param netif the network interface to change
 * @param netif_mtu the new MTU for the interface
 */
#ifndef PPP_SUPPORT
err_t
netif_set_mtu(struct netif *netif, u16_t netif_mtu)
{
  /* As per RFC 791, "Every internet module must be able to forward a datagram of 68
   * octets without further fragmentation.  This is because an internet header
   * may be up to 60 octets, and the minimum fragment is 8 octets." */
  LWIP_ERROR("netif_set_mtu: invalid arguments", (netif != NULL), return ERR_VAL);

#if LWIP_IPV6
  LWIP_ERROR("netif_set_mtu: invalid arguments", (netif_mtu >= NETIF_MTU_MIN) && (netif_mtu <= IP_FRAG_MAX_MTU),
             return ERR_ARG);
#else
  LWIP_ERROR("netif_set_mtu: invalid arguments", (netif_mtu >= IP_FRAG_MIN_MTU) && (netif_mtu <= IP_FRAG_MAX_MTU),
             return ERR_ARG);
#endif

  netif->mtu = netif_mtu;
#if LWIP_IPV6 && LWIP_ND6_ALLOW_RA_UPDATES
  netif->mtu6 = netif_mtu;
#endif /* LWIP_IPV6 && LWIP_ND6_ALLOW_RA_UPDATES */

  LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_STATE, ("netif: MTU of interface %s%"U16_F" is changed to %d\n",
                netif->name, netif->num, netif->mtu));

  return ERR_OK;
}
#endif

#if DRIVER_STATUS_CHECK
err_t
netif_stop_queue(struct netif *netif)
{
  LWIP_ERROR("netif_set_driver_ready: invalid arguments", (netif != NULL), return ERR_ARG);

  if (!(netif->flags & NETIF_FLAG_DRIVER_RDY)) {
    LWIP_DEBUGF(NETIF_DEBUG, ("netif already stopped \n"));
    return ERR_ARG;
  }

  netif->flags = netif->flags & ~NETIF_FLAG_DRIVER_RDY;

  /* Start the waketimer for this netif */
  netif->waketime = 0;

  tcpip_upd_status_to_tcp_pcbs(netif, DRV_NOT_READY);

  return ERR_OK;
}

err_t
netif_wake_queue(struct netif *netif)
{
  LWIP_ERROR("netif_set_driver_ready: invalid arguments", (netif != NULL), return ERR_ARG);

  if (netif->flags & NETIF_FLAG_DRIVER_RDY) {
    LWIP_DEBUGF(NETIF_DEBUG, ("netif already in ready state \n"));
    return ERR_ARG;
  }

  netif->flags |= NETIF_FLAG_DRIVER_RDY;

  tcp_ip_event_sendplus_on_wake_queue(netif);

  /* Stop the waketimer for this netif */
  netif->waketime = -1;

  tcpip_upd_status_to_tcp_pcbs(netif, DRV_READY);

  /* Need to send the buffered TCP segments */
  tcpip_flush_on_wake_queue(netif, DRV_READY);

  return ERR_OK;
}
#endif

#if LWIP_NETIF_PROMISC
/* Enables the promiscuous mode of the netif, if no pkt_raw_pcb are listening over it. */
static void netif_enable_promisc_mode(struct netif *netif)
{
  struct netif *loc_netif = NULL;
  u8_t activ_flag = 0;

  LWIP_ERROR("netif_enable_promisc_mode : invalid arguments", (netif != NULL), return);

  /* Check the status of secondary netif */
  for (loc_netif = netif_list; loc_netif != NULL; loc_netif = loc_netif->next) {
    if (netif == loc_netif) {
      continue;
    }
    if ((strncmp(netif->name, loc_netif->name, NETIF_NAMESIZE) == 0) && (netif->num == loc_netif->num)) {
      if (loc_netif->flags_ext1 & NETIF_FLAG_PROMISC_RUNNING) {
        activ_flag = 1;
        break;
      }
    }
  }

  if (pkt_raw_pcbs_using_netif(netif->ifindex) >= 1) {
    netif->flags_ext1 |= NETIF_FLAG_PROMISC_RUNNING;

    /* enable promiscusous mode */
    if ((activ_flag == 0) && (netif->drv_config != NULL)) {
      netif->drv_config(netif, IFF_PROMISC, 1);
    } else {
      LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_STATE, ("netif: promiscuous mode support not found %s\n",
                  netif->name));
    }
  }

  return;
}

/* disables promiscuous mode of the netif, if no pkt_raw_pcb are listening over it. */
static void netif_disable_promisc_mode(struct netif *netif)
{
  struct netif *loc_netif = NULL;
  u8_t activ_flag = 0;

  LWIP_ERROR("netif_disable_promisc_mode : invalid arguments", (netif != NULL), return);

  /* Check the status of secondary netif */
  for (loc_netif = netif_list; loc_netif != NULL; loc_netif = loc_netif->next) {
    if (netif == loc_netif) {
      continue;
    }
    if ((strncmp(netif->name, loc_netif->name, NETIF_NAMESIZE) == 0) && (netif->num == loc_netif->num)) {
      if (loc_netif->flags_ext1 & NETIF_FLAG_PROMISC_RUNNING) {
        activ_flag = 1;
        break;
      }
    }
  }

  /* disable promiscusous mode */
  if ((activ_flag == 0) && (netif->drv_config != NULL)) {
    netif->drv_config(netif, IFF_PROMISC, 0);
  }

  netif->flags_ext1 &= (u8_t)(~(NETIF_FLAG_PROMISC_RUNNING));

  return;
}

/*
 * Enable promiscuous mode on interface
 *
 * @note: Enabling promiscuous mode on interface will allow receiving all packets.
 * return 0 on success
 *
 */
void netif_update_promiscuous_mode_status(struct netif *netif, u8_t set)
{
  LWIP_ERROR("netif_update_promiscuous_mode_status : invalid arguments", (netif != NULL), return);

  /* return back if promiscuous mode is already enabled on netif */
  if ((set && ((atomic_read(&netif->flags_ext) == NETIF_FLAG_PROMISC))) ||
      (!set && !(atomic_read(&netif->flags_ext) == NETIF_FLAG_PROMISC))) {
    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_STATE, ("netif: promiscuous mode is alredy %s\n",
                (set ? "enabled" : "disabled")));
    return;
  }

  if (netif->drv_config != NULL) {
    if (set) {
      (void)atomic_set((&netif->flags_ext), NETIF_FLAG_PROMISC);
      netif_enable_promisc_mode(netif);
    } else {
      (void)atomic_set((&netif->flags_ext), 0);
      netif_disable_promisc_mode(netif);
    }
    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_STATE, ("netif: promiscuous mode %s on interface %s\n",
                set ? "enabled" : "disabled", netif->name));
  } else {
    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_STATE, ("netif: promiscuous mode support not found %s\n",
                netif->name));
  }
}


void netif_stop_promisc_mode(u8_t ifindex)
{
  struct netif *tmp_if = NULL;

  /* Raw socket without bind and promisc enabled on interface. */
  if (!ifindex) {
    /* loop through netif's and disable promisc mode if this is the only PCB */
    for (tmp_if = netif_list; tmp_if != NULL; tmp_if = tmp_if->next) {
      if ((atomic_read(&tmp_if->flags_ext) == NETIF_FLAG_PROMISC) &&
          (!pkt_raw_pcbs_using_netif(tmp_if->ifindex))) {
        netif_disable_promisc_mode(tmp_if);
      }
    }
  } else { /* RAW socket withbind and promisc enabled on interface */
    tmp_if = netif_find_by_ifindex(ifindex);
    if (((tmp_if != NULL) && (atomic_read(&tmp_if->flags_ext) == NETIF_FLAG_PROMISC)) &&
        (!pkt_raw_pcbs_using_netif(tmp_if->ifindex))) {
      netif_disable_promisc_mode(tmp_if);
    }
  }
}

/* Enables the promiscuous mode of the netif, if no pkt_raw_pcb are listening over it. */
static void netif_start_promisc_if_not_running(struct netif *netif)
{
  LWIP_ERROR("netif_start_promisc_if_not_running : invalid arguments", (netif != NULL), return);

  if ((!(netif->flags_ext1 & NETIF_FLAG_PROMISC_RUNNING))) {
    netif_enable_promisc_mode(netif);
  }

  return;
}

void netif_start_promisc_mode(u8_t ifindex)
{
  struct netif *tmp_if = NULL;

  /* Raw socket without bind and promisc enabled on interface. */
  if (!ifindex) {
    /* loop through netif's and start promisc mode if there is atleast one PCB binded to the netif */
    for (tmp_if = netif_list; tmp_if != NULL; tmp_if = tmp_if->next) {
      if (atomic_read(&tmp_if->flags_ext) == NETIF_FLAG_PROMISC) {
        netif_start_promisc_if_not_running(tmp_if);
      }
    }
  } else { /* RAW socket withbind and promisc enabled on interface */
    tmp_if = netif_find_by_ifindex(ifindex);
    if ((tmp_if != NULL) && (atomic_read(&tmp_if->flags_ext) == NETIF_FLAG_PROMISC)) {
      netif_start_promisc_if_not_running(tmp_if);
    }
  }

  return;
}

#endif /* LWIP_NETIF_PROMISC */

struct netif *
netif_find_by_ipaddr(const ip_addr_t *ipaddr)
{
  LWIP_ERROR("netif_find_by_ipaddr : invalid arguments", (ipaddr != NULL), return NULL);
#if LWIP_IPV4
  if (IP_IS_V4(ipaddr)) {
    return netif_find_by_ip4addr(ipaddr);
  }
#endif

#if LWIP_IPV6
  if (IP_IS_V6(ipaddr)) {
    return netif_find_by_ip6addr(ip_2_ip6(ipaddr));
  }
#endif

  LWIP_DEBUGF(NETIF_DEBUG, ("netif_find_using_ipaddr: didn't find\n"));
  return NULL;
}

/**
* @ingroup netif
* Return the interface index for the netif with name
* or NETIF_NO_INDEX if not found/on error
*
* @param name the name of the netif
*/
u8_t
netif_name_to_index(const char *name)
{
  struct netif *netif = netif_find(name);
  if (netif != NULL) {
    return netif_get_index(netif);
  }
  /* No name found, return invalid index */
  return NETIF_NO_INDEX;
}

/**
* @ingroup netif
* Return the interface name for the netif matching index
* or NULL if not found/on error
*
* @param idx the interface index of the netif
* @param name char buffer of at least NETIF_NAMESIZE bytes
*/
char *
netif_index_to_name(u8_t index, char *name)
{
  int ret;
  struct netif *curif = netif_list;
  if (index == 0) {
    return NULL; /* indexes start at 1 */
  }

  /* find netif from num */
  while (curif != NULL) {
    if (curif->ifindex != index) {
      curif = curif->next;
      continue;
    }
    if (curif->link_layer_type == LOOPBACK_IF) {
      ret = snprintf_s(name, NETIF_NAMESIZE, (NETIF_NAMESIZE - 1), "%s", curif->name);
      if ((ret <= 0) || (ret >= NETIF_NAMESIZE)) {
          return NULL;
      }
    } else {
      ret = snprintf_s(name, NETIF_NAMESIZE, (NETIF_NAMESIZE - 1), "%s%"U8_F, curif->name, curif->num);
    }
    if ((ret <= 0) || (ret >= NETIF_NAMESIZE)) {
      return NULL;
    }
    return name;
  }
  return NULL;
}

/*
 * @ingroup netif_if
 * Return all the array of interfaces in the netif name and index pair .
 * or NULL if not found on error
 *
 * @param output : Array of all the interfaces available
 */
err_t netif_get_nameindex_all(void *arg)
{
  struct netif *curif = NULL;
  struct if_nameindex *ifname_idx_list = NULL;
  struct if_nameindex *tmp_if = NULL;
  char *ifname = NULL;
  u16_t if_count = 1; /* One extra count for the last array which shows the end of the list */
  u16_t total_size;
  int ret;
  struct if_nameindex **if_list = (struct if_nameindex **)arg;
  for (curif = netif_list; curif != NULL; curif = curif->next) {
    if_count++;
  }

  /* This API allows to handle only 0xfe indexes as of now */
  LWIP_ERROR("Max number of interfaces reached.", (if_count <= LWIP_NETIF_IFINDEX_MAX_EX),
             return ERR_MEM);

  /* No overflow check required here */
  total_size = (u16_t)(if_count * IF_NAMEINDEX_ELE_SIZE);
  ifname_idx_list = (struct if_nameindex *)mem_malloc(total_size);
  if (ifname_idx_list == NULL) {
    *if_list = NULL;
    return ERR_MEM;
  }

  (void)memset_s((void *)ifname_idx_list, total_size, 0, total_size);

  ifname = ((char*)(ifname_idx_list) + (sizeof(struct if_nameindex) * if_count));
  tmp_if = ifname_idx_list;

  for (curif = netif_list; curif != NULL; curif = curif->next) {
    tmp_if->if_index = curif->ifindex;
    tmp_if->if_name = ifname;
    if (curif->link_layer_type == LOOPBACK_IF) {
      ret = snprintf_s(ifname, NETIF_NAMESIZE, (NETIF_NAMESIZE - 1), "%s", curif->name);
      if ((ret <= 0) || (ret >= NETIF_NAMESIZE)) {
        LWIP_DEBUGF(NETIF_DEBUG, ("netif_get_nameindex_all: name is too long \n"));
        return ERR_BUF;
      }
    } else {
      ret = snprintf_s(ifname, NETIF_NAMESIZE, (NETIF_NAMESIZE - 1), "%s%"U8_F, curif->name, curif->num);
      if ((ret <= 0) || (ret >= NETIF_NAMESIZE)) {
        LWIP_DEBUGF(NETIF_DEBUG, ("netif_get_nameindex_all: name is too long \n"));
        return ERR_BUF;
      }
    }
    tmp_if++;
    ifname = (char*)(ifname + NETIF_NAMESIZE + 4); // keep buffer of 4 bytes
  }
  tmp_if->if_index = 0;
  tmp_if->if_name = NULL;

  *if_list = ifname_idx_list;

  return ERR_OK;
}

/**
* @ingroup netif
* Return the interface for the netif index
*
* @param idx index of netif to find
*/
struct netif *
netif_get_by_index(u8_t idx)
{
  struct netif *netif;

  LWIP_ASSERT_CORE_LOCKED();

  if (idx != NETIF_NO_INDEX) {
    NETIF_FOREACH(netif) {
      if (idx == netif_get_index(netif)) {
        return netif; /* found! */
      }
    }
  }

  return NULL;
}

#if LWIP_RIPPLE
u8_t
netif_count(void)
{
  struct netif *netif = NULL;
  u8_t cnt = 0;

  for (netif = netif_list; netif != NULL; netif = netif->next) {
    cnt++;
  }

  return cnt;
}
#endif /* LWIP_RIPPLE */

#if LWIP_API_MESH
#if LWIP_RIPPLE
err_t
netif_remove_peer(struct netif *netif, struct linklayer_addr *peeraddr)
{
  LWIP_ERROR("netif_remove_peer: invalid arguments", (netif != NULL), return ERR_ARG);

  LWIP_ERROR("netif_remove_peer: invalid arguments", (peeraddr != NULL), return ERR_ARG);

  if (netif->remove_peer != NULL) {
    return netif->remove_peer(netif, peeraddr);
  }

  LWIP_DEBUGF(NETIF_DEBUG, ("netif_remove_peer: not support\n"));

  return ERR_OPNOTSUPP;
}

err_t
netif_set_beacon_prio(struct netif *netif, u8_t prio)
{
  LWIP_ERROR("netif_set_beacon_prio: invalid arguments", (netif != NULL), return ERR_ARG);

  if (netif->set_beacon_prio != NULL) {
    return netif->set_beacon_prio(netif, prio);
  }

  LWIP_DEBUGF(NETIF_DEBUG, ("netif_set_beacon_prio: not support\n"));

  return ERR_OPNOTSUPP;
}

err_t
netif_set_unique_id(struct netif *netif, uniqid_t id)
{
  LWIP_ERROR("netif_set_unique_id: invalid arguments", (netif != NULL), return ERR_ARG);

  if (netif->set_unique_id != NULL) {
    return netif->set_unique_id(netif, id);
  }

  LWIP_DEBUGF(NETIF_DEBUG, ("netif_set_unique_id: not support\n"));

  return ERR_OPNOTSUPP;
}

err_t
netif_get_peer_count(struct netif *netif, u16_t *count)
{
  LWIP_ERROR("netif_get_peer_count: invalid arguments", ((netif != NULL) && (count != NULL)), return ERR_ARG);

  if (netif->get_peer_count != NULL) {
    return netif->get_peer_count(netif, count);
  }

  LWIP_DEBUGF(NETIF_DEBUG, ("netif_get_peer_count: not support\n"));

  return ERR_OPNOTSUPP;
}

err_t
netif_set_rank(struct netif *netif, u16_t rank)
{
  LWIP_ERROR("netif_set_rank: invalid arguments", (netif != NULL), return ERR_ARG);

  if (netif->set_rank != NULL) {
    return netif->set_rank(netif, rank);
  }

  LWIP_DEBUGF(NETIF_DEBUG, ("netif_set_rank: not support\n"));

  return ERR_OPNOTSUPP;
}

err_t
netif_get_link_metric(struct netif *netif, u16_t *metric)
{
  LWIP_ERROR("netif_get_link_metric: invalid arguments", (netif != NULL), return ERR_ARG);

  if (netif->get_link_metric != NULL) {
    return netif->get_link_metric(netif, metric);
  }

  LWIP_DEBUGF(NETIF_DEBUG, ("netif_get_link_metric: not support\n"));

  return ERR_OPNOTSUPP;
}

#endif /* LWIP_RIPPLE */

err_t
netif_linklayer_event_internal(struct tcpip_api_call_data *m)
{
  struct netifapi_msg *msg = (struct netifapi_msg *)(void *)m;
  const struct linklayer_event_info *evt = msg->msg.linklayer_event.evt;

  switch (evt->type) {
    case LL_EVENT_DEL_PEER:
    case LL_EVENT_STA_DISCONN:
#if LWIP_DHCPS
      dhcps_client_disconnect(msg->netif, (const u8_t *)evt->info.sta_conn.addr.addr, evt->info.sta_conn.addr.addrlen);
#endif
      break;
    default:
      break;
  }
  return ERR_OK;
}

err_t
netif_linklayer_event(struct netif *netif, const struct linklayer_event_info *evt)
{
  err_t err;
  LWIP_API_VAR_DECLARE(msg);

  LWIP_API_VAR_ALLOC(msg);

  LWIP_API_VAR_REF(msg).netif = netif;
  LWIP_API_VAR_REF(msg).msg.linklayer_event.evt = evt;

  err = tcpip_linklayer_event_call(netif_linklayer_event_internal, &API_VAR_REF(msg).call);

  LWIP_API_VAR_FREE(msg);
  return err;
}

struct linklayer_event_handle {
  void *dst;
  int copy_len;
};

err_t
netif_linklayer_event_callback(struct netif *netif, u8_t evt_type, const void *evt_info)
{
  struct linklayer_event_info evt;

  LWIP_ERROR("netif_linklayer_event_callback: invalid arguments",
             ((netif != NULL) && (evt_type < LL_EVENT_MAX)), return ERR_ARG);

  struct linklayer_event_handle handle[LL_EVENT_MAX] = {
#if LWIP_RIPPLE
    {
      .dst = &evt.info.tx_info,
      .copy_len = sizeof(linklayer_event_tx_info_t),
    },
    {
      .dst = &evt.info.new_peer,
      .copy_len = sizeof(linklayer_event_new_peer_t),
    },
    {
      .dst = &evt.info.del_peer,
      .copy_len = sizeof(linklayer_event_del_peer_t),
    },
#else
    {
      .dst = NULL,
      .copy_len = 0,
    },
    {
      .dst = NULL,
      .copy_len = 0,
    },
    {
      .dst = NULL,
      .copy_len = 0,
    },
#endif /* LWIP_RIPPLE */
    {
      .dst = &evt.info.ap_conn,
      .copy_len = sizeof(linklayer_event_ap_conn_t),
    },
    {
      .dst = NULL,
      .copy_len = 0,
    },
    {
      .dst = &evt.info.sta_conn,
      .copy_len = sizeof(linklayer_event_sta_conn_t),
    },
    {
      .dst = &evt.info.sta_conn,
      .copy_len = sizeof(linklayer_event_sta_conn_t),
    },
  };

  evt.type = evt_type;
  LWIP_DEBUGF(NETIF_DEBUG, ("netif_linklayer_event_callback: %u\n", evt_type));

  if (handle[evt_type].dst != NULL) {
    LWIP_ERROR("netif_linklayer_event_callback: invalid arguments", (evt_info != NULL), return ERR_ARG);
    (void)memcpy_s(handle[evt_type].dst, handle[evt_type].copy_len, evt_info, handle[evt_type].copy_len);
  }

  err_t ret = netif_linklayer_event(netif, &evt);

#if LWIP_RIPPLE
  ret = netif_linklayer_event_handler(netif, &evt);
#endif /* LWIP_RIPPLE */

  return ret;
}
#endif /* LWIP_API_MESH */

/**
 * @ingroup netif
 * Find a network interface by searching for its name
 *
 * @param name the name of the netif (like netif->name) plus concatenated number
 * in ascii representation (e.g. 'en0')
 */
struct netif *
netif_find(const char *name)
{
  struct netif *netif = NULL;
  char candidate_name[NETIF_NAMESIZE];
  int ret;

  LWIP_ERROR("netif_find : invalid value.", (name != NULL), return NULL);

  NETIF_FOREACH(netif) {
    if (strncmp(name, netif->name, NETIF_NAMESIZE) == 0) {
      LWIP_DEBUGF(NETIF_DEBUG, ("netif_find: found %s\n", name));
      return netif;
    }

    if (netif->link_layer_type != LOOPBACK_IF) {
      ret = snprintf_s(candidate_name, NETIF_NAMESIZE, (NETIF_NAMESIZE - 1), "%s%"U8_F, netif->name, netif->num);
      if ((ret <= 0) || (ret >= NETIF_NAMESIZE)) {
        LWIP_DEBUGF(NETIF_DEBUG, ("netif_find: name '%s'is too long\n", netif->name));
        continue;
      }
      if (strncmp(name, candidate_name, NETIF_NAMESIZE) == 0) {
        LWIP_DEBUGF(NETIF_DEBUG, ("netif_find: found %s\n", name));
        return netif;
      }
    }
  }
  LWIP_DEBUGF(NETIF_DEBUG, ("netif_find: didn't find %s\n", name));
  return NULL;
}

#if LWIP_NETIF_EXT_STATUS_CALLBACK
/**
 * @ingroup netif
 * Add extended netif events listener
 * @param callback pointer to listener structure
 * @param fn callback function
 */
void
netif_add_ext_callback(netif_ext_callback_t *callback, netif_ext_callback_fn fn)
{
  LWIP_ASSERT_CORE_LOCKED();
  LWIP_ASSERT("callback must be != NULL", callback != NULL);
  LWIP_ASSERT("fn must be != NULL", fn != NULL);

  callback->callback_fn = fn;
  callback->next        = ext_callback;
  ext_callback          = callback;
}

/**
 * @ingroup netif
 * Remove extended netif events listener
 * @param callback pointer to listener structure
 */
void
netif_remove_ext_callback(netif_ext_callback_t* callback)
{
  netif_ext_callback_t *last, *iter;

  LWIP_ASSERT_CORE_LOCKED();
  LWIP_ASSERT("callback must be != NULL", callback != NULL);

  if (ext_callback == NULL) {
    return;
  }

  if (callback == ext_callback) {
    ext_callback = ext_callback->next;
  } else {
    last = ext_callback;
    for (iter = ext_callback->next; iter != NULL; last = iter, iter = iter->next) {
      if (iter == callback) {
        LWIP_ASSERT("last != NULL", last != NULL);
        last->next = callback->next;
        callback->next = NULL;
        return;
      }
    }
  }
}

/**
 * Invoke extended netif status event
 * @param netif netif that is affected by change
 * @param reason change reason
 * @param args depends on reason, see reason description
 */
void
netif_invoke_ext_callback(struct netif *netif, netif_nsc_reason_t reason, const netif_ext_callback_args_t *args)
{
  netif_ext_callback_t *callback = ext_callback;

  LWIP_ASSERT("netif must be != NULL", netif != NULL);

  while (callback != NULL) {
    callback->callback_fn(netif, reason, args);
    callback = callback->next;
  }
}
#endif /* LWIP_NETIF_EXT_STATUS_CALLBACK */
