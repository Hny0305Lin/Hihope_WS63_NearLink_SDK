/**
 * @file
 * netif API (to be used from non-TCPIP threads)
 */

/*
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
 */
#ifndef LWIP_HDR_NETIFAPI_H
#define LWIP_HDR_NETIFAPI_H

#include "lwip/opt.h"

#if LWIP_NETIF_API /* don't build if not configured for use in lwipopts.h */

#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#if LWIP_IPV6_DHCP6
#include "lwip/dhcp6.h"
#endif /* LWIP_IPV6_DHCP6 */
#include "lwip/autoip.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/priv/api_msg.h"
#include "lwip/prot/ethernet.h"

#define netifapi_netif_find netifapi_netif_find_by_name

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_IPV6 && LWIP_NETIF_NBR_CACHE_API
struct ipv6_neighbor {
  struct ip6_addr nbripaddr;
  u32_t reachabletime;
  u8_t hwaddr[NETIF_MAX_HWADDR_LEN];
  u8_t hwlen;
  u8_t curstate;
};
#endif

/* User can add neighbors with below states */
#define ND6_STATE_REACHABLE  2
#if LWIP_ND6_STATIC_NBR
#define  ND6_STATE_PERMANENT 6
#endif

/* API for application */
#if LWIP_ARP && LWIP_IPV4
/* Used for netfiapi_arp_* APIs */
enum netifapi_arp_entry {
  NETIFAPI_ARP_PERM /* Permanent entry */
  /* Other entry types can be added here */
};

/** @ingroup netifapi_arp */
err_t netifapi_arp_add(const ip4_addr_t *ipaddr, struct eth_addr *ethaddr, enum netifapi_arp_entry type);
/** @ingroup netifapi_arp */
err_t netifapi_arp_remove(const ip4_addr_t *ipaddr, enum netifapi_arp_entry type);
#endif /* LWIP_ARP && LWIP_IPV4 */

err_t netifapi_netif_add(struct netif *netif
#if LWIP_IPV4
                         , const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw
#endif /* LWIP_IPV4 */
                        );

#if LWIP_IPV4
err_t netifapi_netif_set_addr(struct netif *netif, const ip4_addr_t *ipaddr,
                              const ip4_addr_t *netmask, const ip4_addr_t *gw);
#endif /* LWIP_IPV4*/

/*
Func Name:  netifapi_netif_find_by_name
*/
/**
* @ingroup netifapi_netif
*
* @brief
*  This is a thread safe API, used to get the netif pointer whoes name was the input argument.
*  It is recommended to use this API instead of netif_find_by_name().
*  Call netif_find() in a thread-safe way by running that function inside the tcpip_thread
*  context.
*
* @param[in]    name         Name of the netif.
*
* @returns
*  The netif pointer : On success \n
*  NULL : the netif was NOT exist \n
*
* @par Related Topics
* netif_find_by_name()
*
*/

struct netif *netifapi_netif_find_by_name(const char *name);

/*
Func Name:  netifapi_netif_find_by_ifindex
*/
/**
* @ingroup netifapi_netif
*
* @brief
*  This is a thread safe API, used to get the netif pointer whoes index equals the input argument.
*  It is recommended to use this API instead of netif_find_by_ifindex().
*  Call netif_find_by_ifindex() in a thread-safe way by running that function inside the
*  tcpip_thread context.
*
* @param[in]    ifindex         Index of the netif.
*
* @returns
*  The netif pointer : On success \n
*  NULL : the netif was NOT exist \n
*
* @par Related Topics
* netif_find_by_ifindex()
*/

struct netif *netifapi_netif_find_by_ifindex(unsigned ifindex);

/*
Func Name:  netifapi_netif_find_by_ipaddr
*/
/**
* @ingroup netifapi_netif
*
* @brief
*  This is a thread safe API, used to get the netif pointer whoes address equals the input argument.
*  It is recommended to use this API instead of netif_find_by_ipaddr().
*  Call netif_find_by_ipaddr() in a thread-safe way by running that function inside the tcpip_thread
*  context.
*
* @param[in]    ipaddr         The IP address.
*
* @returns
*  The netif pointer : On success \n
*  NULL : the netif was NOT exist \n
*
* @par Related Topics
* netif_find_by_ipaddr()
*/
struct netif *netifapi_netif_find_by_ipaddr(const ip_addr_t *ipaddr);

#if LWIP_IPV4
/*
Func Name:  netifapi_netif_get_addr
*/
/**
* @ingroup netifapi_netif
*
* @brief
*
*  This is a thread safe API, used to get IP_add configuration for a network interface
 (including netmask and default gateway).
*  It is recommended to use this API instead of netif_get_addr().
*  Call netif_get_addr() in a thread-safe way by running that function inside the
*  tcpip_thread context.
*
* @param[in]    netif          Indicates the network interface to get.
* @param[in]    ipaddr         Indicates the IP address.
* @param[in]    netmask        Indicates the network mask.
* @param[in]    gw             Indicates the default gateway IP address.
*
* @returns
*  0 : On success \n
*  Negative value : On failure \n
*
* @par Related Topics
* netif_get_addr()
*
* @note
*   - netmask and/or gw can be passed NULL, if these details about the netif are not needed
*/
err_t netifapi_netif_get_addr(struct netif *netif,
                              ip4_addr_t *ipaddr,
                              ip4_addr_t *netmask,
                              ip4_addr_t *gw);

#endif /* LWIP_IPV4 */
err_t netifapi_netif_common(struct netif *netif, netifapi_void_fn voidfunc,
                            netifapi_errt_fn errtfunc);
/*
 * @ingroup netifapi_netif
 * @brief
 * Call the "argfunc" with argument arg in a thread-safe way by running that function inside the tcpip_thread context.
 * @note Use this function only for functions where there is only "netif" parameter.
 */
err_t netifapi_netif_call_argcb(struct netif *netif, netifapi_arg_fn argfunc, void *arg);

/*
 * @ingroup netifapi_netif
 * @brief
 * Call the "argfunc" with argument arg in a thread-safe way by running that function inside the tcpip_thread context.
 */
err_t netifapi_call_argcb(netifapi_arg_fn argfunc, void *arg);

#if LWIP_IPV6
/**
 * @ingroup netifapi_netif
 * @brief
 * This function allows for the addition of a new IPv6 address to an interface.
 * It takes care of finding an empty slot and then sets the address tentative
 * (to make sure that all the subsequent processing happens).
 *
 *
 * @param[in] netif Indicates the netif to add the address on.
 * @param[in] ip6addr Indicates the address to add.
 *
 * @returns
 *  0 : On success. \n
 *  Negative value : On failure.
 *  ERR_BUF : If there is no space to add new IPv6 Address.
 */
err_t netifapi_netif_add_ip6_address(struct netif *netif, ip_addr_t *ipaddr);

/**
 * @ingroup netifapi_netif
 * @brief
 * This function allows for the easy removal of an existing IPv6 address from an interface.
 *
 *
 * @param[in] netif Indicates the netif to remove the address from.
 * @param[in] ip6addr Indicates the address to remove.
 * @note
 * - This interface may force any blocking accept() to return failure with errno ECONNABORTED.
 */
void netifapi_netif_rmv_ip6_address(struct netif *netif, ip_addr_t *ipaddr);

/**
 * @ingroup netifapi_netif
 * @brief
 * This function allows to join a group without sending out MLD6 Report message.
 * This function is not RFC 2710 compliant and is a custom function
 *
 *
 * @param[in] netif Indicates the netif which want to join a group.
 * @param[in] ip6addr Indicates the group address.
 */
err_t netifapi_netif_join_ip6_multicastgroup(struct netif *netif, ip6_addr_t *ip6addr);

/*
 * @ingroup netifapi_netif
 * This function allows for the easy addition of a new IPv6 link local address to an interface.
 * It uses slot fixed for link local address and then sets the address tentative
 * (to make sure that all the subsequent processing happens).
 * This Interface MUST be called to configure link local IP address, after addition of netif.
 *
 * @param netif netif to add the address on
 * @param ip6addr address to add
 * @param from_mac_48bit specifies whether mac address is 48 bit for 64 bit format */
err_t netifapi_netif_add_ip6_linklocal_address(struct netif *netif, u8_t from_mac_48bit);

err_t netif_create_ip6_linklocal_address_wrapper(struct netif *netif, void *arg);

/*
Func Name:  netifapi_netif_get_default
*/
/**
* @ingroup netifapi_netif

* @brief
*  This API is used to get the default network interface.
*  It is used to output all packets for which no specific route is found.
*
* @return
*  NULL: if either the default netif was NOT exist or the default netif was down \n
*  Others: the default netif \n
*/
struct netif *netifapi_netif_get_default(void);

#if LWIP_IPV6_AUTOCONFIG
/**
 * @ingroup netifapi_netif
 * @brief
 *  This function is used to enable autoconfig for IPv6 address in a thread-safe way.
 *  That is, the indicated network interface will send out an RS packet immediately to obtain RA.
 *
 * @param[in]    netif        Indicates the lwIP network interface.
 *
 * @note
 *
 * For netifapi_set_ip6_autoconfig_enabled():
 *  - It enables stateless autoconfiguration.
 *  - It requires the router/default-gateway to understand and implement RFC-4862, but requires no DHCPv6 server.
 *  - No server keeps track of what has or hasn't been assigned, and no server approves the use of the address,
 *    or pass it out.
 *  - Once called: send out an RS packet immediately to obtain RA.
 *  - When receiving an RA, lwIP can generate an IPv6 address (DAD first) and update routing information.
 *
 * For netifapi_dhcp6_enable_stateful():
 *  - It enables stateful DHCPv6 client autoconfiguration (stateless disabled).
 *  - It requires the DHCPv6 server(s) to provide addressing and service information.
 *  - The DHCP server and the client both maintain state information to keep addresses from conflicting,
 *    to handle leases, and to renew addresses over time.
 *  - Once called: disable autoconfig for IPv6 address (clean autoconfig generated addresses),
 *    and start DHCPv6 procedure to create a stateful DHCPv6 client (if no stateful DHCPv6 client).
 *
 * For netifapi_dhcp6_enable_stateless():
 *  - It is used to get "Other configuration" in RFC-4861.
 *  - It requires the DHCPv6 server(s) to provide service information (like DNS servers and NTP servers),
 *    but no addressing information.
 *  - Once called: no effect to current IPv6 addresses,
 *    and start DHCPv6 procedure to create a stateless DHCPv6 client (if no DHCPv6 client).
 *
 * @par Related Topics
 * netifapi_dhcp6_enable_stateful()
 * netifapi_dhcp6_enable_stateless()
 */
err_t netifapi_set_ip6_autoconfig_enabled(struct netif *netif);

/**
 * @ingroup netifapi_netif
 * @brief
 *  This function is used to disable autoconfig for IPv6 address in a thread-safe way.
 *  That is, autoconfig generated addresses on the indicated network interface will be invalid.
 *
 * @param[in]    netif        Indicates the lwIP network interface.
 *
 * @note
 * For now, since only prefixes have the autoconfig generated flag, while IPv6 addresses don't have,
 * lwIP can only invalid addresses by prefixes rather than addresses. So, when a network interface has
 * a manually configured IPv6 address, and an autoconfig generated IPv6 address which have the same
 * prefix, calling netifapi_set_ip6_autoconfig_disabled() will invalid both addresses.
 *
 * @par Related Topics
 * netifapi_dhcp6_disable()
 * netifapi_dhcp6_release_stateful()
 */
err_t netifapi_set_ip6_autoconfig_disabled(struct netif *netif);
#endif

#endif /* LWIP_IPV6 */

#if DRIVER_STATUS_CHECK
/**
* @ingroup netifapi_netif
* @brief
*  This API is used to set the netif driver status to the "Driver Not Ready" state. the driver must call this API
*  to intimate the stack that the send buffer in the driver is full.
*
* @param[in]    netif    Indicates the lwIP network interface.
*
* @returns
*  ERR_OK: On success \n
*  ERR_ARG: On passing invalid arguments. \n
*/
err_t netifapi_stop_queue(struct netif *netif);

/**
*  @ingroup netifapi_netif
*  @brief
*  This API is used to set the netif driver status to the "Driver Ready" state. This API is called by the driver to
*  inform the stack that the driver send buffer is available to send after netifapi_stop_queue() was called
*  previously.
*
* @param[in]    netif    Indicates the lwIP network interface.
*
* @returns
*  ERR_OK: On success \n
*  ERR_ARG: On passing invalid arguments. \n
*/
err_t netifapi_wake_queue(struct netif *netif);
#endif

/** @ingroup netifapi_netif */
err_t netifapi_netif_name_to_index(const char *name, u8_t *index);
/** @ingroup netifapi_netif */
err_t netifapi_netif_index_to_name(u8_t index, char *name);

/** @ingroup netifapi_netif
  * @brief
  *  This API is used to remove a network interface from the list of lwIP netifs
  *  in a thread-safe manner.
  *  The Ethernet driver calls this API in a thread-safe manner to remove a network
  *  interface from the list of lwIP netifs.
  *
  * @param[in]    n     Indicates the network interface to be removed.
  *
  * @returns
  *  ERR_OK: On success. \n
  *  ERR_VAL: On failure due to illegal value. \n
  *  ERR_ARG: On passing invalid arguments.
  *
  * @note
   - Any data sent by the application will return success,
   but the data will not be sent if only netifapi_netif_remove() API is called and not the sockets.
   - The adaptor must close all the connections on socket before netifapi_netif_remove()
   API is called, as netifapi_netif_remove() does not close established connections.
  * @see netif_remove()
  */
err_t netifapi_netif_remove(struct netif *netif);

/** @ingroup netifapi_netif
  * @brief
  *  This interface is used to bring an interface up in a thread-safe way.
  *  That is, the interface is available for processing traffic.
  *
  * @param[in]    n     Indicates the network interface.
  *
  * @note
  * Enabling DHCP on a down interface will make it come up once configured.
  *
  * @par Related Topics
  * netifapi_dhcp_start()
  *
  * @see netif_set_up()
  */
err_t netifapi_netif_set_up(struct netif *netif);

/** @ingroup netifapi_netif
  * @brief
  *  This interface is used to bring an interface down in a thread-safe way.
  *  That is, the interface disables any traffic processing.
  *
  * @param[in]    n     Indicates the network interface.
  *
  * @note
  * Enabling DHCP on a down interface will make it come up once configured.
  *
  * @par Related Topics
  * netifapi_dhcp_start()
  *
  * @see netif_set_down()
  */
err_t netifapi_netif_set_down(struct netif *netif);

/** @ingroup netifapi_netif
  *
  * @param[in]    netif     Indicates the network interface to be set as default.
  *
  * @brief
  *  This interface is used to set a network interface as the default network interface.
  *  It is used to output all packets for which no specific route is found.
  *
  * @param[in]    netif    Indicates the lwIP network interface.
  *
  * @see netif_set_default()
  */
err_t netifapi_netif_set_default(struct netif *netif);

/** @ingroup netifapi_netif
  *
  * @brief
  *  This thread-safe interface is called by the driver when its link goes up.
  *
  * @param[in]    netif    Indicates the lwIP network interface.
  *
  * @see netif_set_link_up()
  */
err_t netifapi_netif_set_link_up(struct netif *netif);

/** @ingroup netifapi_netif
  *
  * @brief
  *  This thread-safe interface is called by the driver when its link goes down.
  *
  * @param[in]    netif    Indicates the lwIP network interface.
  *
  * @see netif_set_link_down()
  */
err_t netifapi_netif_set_link_down(struct netif *netif);

/**
 * @defgroup netifapi_dhcp4 DHCPv4
 * @ingroup netifapi
 * To be called from non-TCPIP threads
 */
/** @ingroup netifapi_dhcp4
  *
  * @brief
  *  This interface is used to start DHCP negotiation for a network interface. If no DHCP client instance
  *  is attached to this interface, a new client is created first. If a DHCP client instance is already
  *  present, it restarts negotiation. It is the thread-safe way for calling dhcp_start in the user space.
  *
  * @param[in]    netif    Indicates the lwIP network interface.
  *
  * @see dhcp_start()
  */
err_t netifapi_dhcp_start(struct netif *netif);

/**
  * @ingroup netifapi_dhcp4
  *
  * @brief
  *  This interface is used to remove the DHCP client from the interface. It stops DHCP configuration.
  *  It is the thread-safe way for calling dhcp_stop in the user space.
  *
  * @param[in]    netif    Indicates the lwIP network interface.
  *
  * @deprecated Use netifapi_dhcp_release_and_stop() instead.
  */
err_t netifapi_dhcp_stop(struct netif *netif);

/** @ingroup netifapi_dhcp4
  *
  * @brief
  *  This interface is used to make the DHCP client inform the DHCP server regarding its manual(static)
  *  IP configuration.
  *  This is done by sending a DHCP_INFORM message from the client to the server.
  *  It is the thread-safe way for calling dhcp_inform in the user space.
  *
  * @param[in]    netif    Indicates the lwIP network interface.
  *
  * @see dhcp_inform()
  */
err_t netifapi_dhcp_inform(struct netif *netif);

/** @ingroup netifapi_dhcp4
  *
  * @brief
  *  This interface is used to make the DHCP client running on the interface to renew its existing
  *  IP configuration to the DHCP server from whom it has leased the IP Configuration.
  *  This is done by sending a DHCP_REQUEST message from the client to the server after T1-Timer Expiry.
  *  It is the thread-safe way for calling dhcp_renew in the user space.
  *
  * @param[in]    netif    Indicates the lwIP network interface.
  *
  * @see dhcp_renew()
  */
err_t netifapi_dhcp_renew(struct netif *netif);

/**
  * @ingroup netifapi_dhcp4
  *
  * @brief
  *  This interface is used to release the IP Configuration on a network interface that it has leased 
  *  from the DHCP Server and informs the DHCP server that this IP Configuration is no longer needed by the client.
  *  This is done by sending a DHCP_RELEASE message from the client to the server.
  *  It is the thread-safe way for calling dhcp_release in the user space.
  *
  * @param[in]    netif    Indicates the lwIP network interface.
  *
  * @deprecated Use netifapi_dhcp_release_and_stop() instead.
 */
err_t netifapi_dhcp_release(struct netif *netif);

/** @ingroup netifapi_dhcp4
  * @see dhcp_release_and_stop()
  */
err_t netifapi_dhcp_release_and_stop(struct netif *netif);

/** @ingroup netifapi_dhcp4
  *
  * @brief
  * This interface is used to get the DHCP negotiation status for a network interface.
  *  It is the thread-safe way for calling dhcp_is_bound() in the user space.
  *
  * @param[in]    netif    Indicates the lwIP network interface.
  *
  * @see dhcp_is_bound()
  */
err_t netifapi_dhcp_is_bound(struct netif *netif);

/** @ingroup netifapi_dhcp4
  *
  * @brief
  *  This interface is used to free the memory allocated for DHCP during DHCP start.
  *  It is the thread-safe way for calling dhcp_cleanup in the user space.
  *
  * @param[in]    netif    Indicates the lwIP network interface.
  *
  * @see dhcp_cleanup()
  */
err_t netifapi_dhcp_cleanup(struct netif *netif);

/**
 * @defgroup netifapi_autoip AUTOIP
 * @ingroup netifapi
 * To be called from non-TCPIP threads
 */
/** @ingroup netifapi_autoip
  *
  *   Call autoip_start() in a thread-safe way by running that function inside the tcpip_thread context.
  *
  * @param[in]    n    Indicates the lwIP network interface.
  *
  * @returns
  * ERR_OK: On success \n
  * ERR_MEM: On failure due to memory \n
  * ERR_VAL: On failure due to Illegal value.
  *
  * @see autoip_start()
  */
err_t netifapi_autoip_start(struct netif *netif);

/** @ingroup netifapi_autoip
  *
  * @brief
  * Thread-safe API to stop the  AutoIP client.
  *
  * @param[in]    n    Indicates the lwIP network interface.
  *
  * @returns
  *  ERR_OK: On success. \n
  *  ERR_ARG: On passing invalid arguments.
  *
  * @note
  * - Call the autoip_stop API to stop the AutoIP service.
  * - Use only for functions where there is only "netif" parameter.
  *
  * @see autoip_stop()
  */
err_t netifapi_autoip_stop(struct netif *netif);

#if LWIP_NETIF_LINK_CALLBACK
/*
Func Name:  netifapi_netif_set_link_callback
*/
/**
* @ingroup netifapi_netif
* @brief
*  This API is used to set callback to netif. This API is called whenever a link is brought up or down.
*
* @param[in]    netif            Indicates the lwIP network interface.
* @param[in]    link_callback    The callback pointer.
*
* @returns
*  ERR_OK: This API always returns this value. \n
*/
err_t netifapi_netif_set_link_callback(struct netif *netif, netif_status_callback_fn link_callback);
#endif /* LWIP_NETIF_LINK_CALLBACK */

#if LWIP_NETIF_EXT_STATUS_CALLBACK
/*
Func Name:  netifapi_netif_add_ext_callback
*/
/**
* @ingroup netifapi_netif
* @brief
*  This API is used to add an extended netif status callback (NSC).
*
* @param[in]    callback         Indicates the NSC data structure.
* @param[in]    fn               The callback function pointer.
*
* @returns
*  ERR_OK: On success. \n
*  ERR_ARG: On passing invalid arguments.
*/
err_t netifapi_netif_add_ext_callback(netif_ext_callback_t *callback, netif_ext_callback_fn fn);

/*
Func Name:  netifapi_netif_remove_ext_callback
*/
/**
* @ingroup netifapi_netif
* @brief
*  This API is used to remove an extended netif status callback (NSC).
*
* @param[in]    callback         Indicates the NSC data structure.
*
* @returns
*  ERR_OK: On success. \n
*  ERR_VAL: On passing invalid arguments.
*/
err_t netifapi_netif_remove_ext_callback(netif_ext_callback_t *callback);
#endif /* LWIP_NETIF_EXT_STATUS_CALLBACK */

/*
Func Name:  netifapi_netif_set_mtu
*/
/**
* @ingroup netifapi_netif
* @brief
*  This API is used to set the MTU of the netif. Call this API whenever the MTU needs to be changed.
* @param[in]    netif    Indicates the lwIP network interface.
* @param[in]    mtu      Indicates the new MTU of the network interface. Valid values are 68 to 1500.
*
* @returns
*  ERR_OK: On success. \n
*  ERR_ARG: On passing invalid arguments.
*
* @note
* - The value of the new mtu which is passed should be in the range 68 to 1500.
* - On modifying the MTU, the MTU change comes into effect immediately.
* - IP packets for existing connections are also sent according to new MTU.
* - Ideally, the application must ensure that connections are terminated before MTU modification or at
* init time to avoid side effects, since peer might be expecting a different MTU.
* - Effective MSS for existing connection will not change, it might remain same. It is not
* suggested to change the MTU at runtime.
* - Only for new connections, effective MSS is used for connection setup. \n
*
*/
err_t netifapi_netif_set_mtu(struct netif *netif, u16_t mtu);

#if LWIP_DHCPS

/*
Func Name:  netifapi_dhcps_start
*/
/**
* @ingroup netifapi_dhcp4
* @brief
*  This API is used to start DHCPv4 Server on the netif. This should be be called only
 once when the DHCPv4 Server needs to be started on the netif.
*
* @param[in]    netif         Indicates the lwIP network interface.
* @param[in]    start_ip      Indicates the starting IP address of the DHCPv4 address pool.
* @param[in]    ip_num        Indicates the number of IP addresses that need to be in the pool.
*
* @par Return values
*  ERR_OK: On success \n
*  ERR_ARG: On passing invalid arguments. \n
*  ERR_MEM: On trying to start a DHCPv4 server on a netif where DHCPv4 server is already running \n
*
* @par Note
* - If the DHCPv4 scope (Address Pool) must have the default configuration, then both
*   start_ip and ip_num must be NULL.
* - If the DHCPv4 scope (Address Pool) must be manually configured, then both start_ip And
*   ip_num must not be NULL.
* - In case Of default DHCPv4 scope (Address Pool) configuration, the total number of addresses
*   available in the pool for clients will be one less
*   than that of total addresses available in the pool, since one of the addresses in the pool
*   will be taken up by the DHCPv4 Server and only others will be available to the DHCPv4 Clients.
* - In case of manual DHCPv4 scope(Address Pool) configuration, start_ip should be in the same subnet of the netif.
* - In case of manual DHCPv4 scope configuration, if the IP address of the DHCPv4 server lies in the range of 
* - [ip_start, ip_start + ip_num], then the total number of addresses available in the pool for clients will be one
* - less than ip_num, since one of the addresses in the pool will be taken up  by the DHCPv4 Server and only others
* - will be available to the DHCPv4 Clients. \n
* - The total number of addresses in the DHCPv4 Scope will be the minimum of non-zero ip_num & LWIP_DHCPS_MAX_LEASE.
* - In case of manual DHCPv4 scope configuration, if ip_start+ip_num goes beyond x.y.z.254, then the total number
*   of addresses in the pool will be from [ip_start, x.y.z.254] only.
*/
err_t netifapi_dhcps_start(struct netif *netif, char *start_ip, u16_t ip_num);


/*
Func Name:  netifapi_dhcps_stop
*/
/**

* @ingroup netifapi_dhcp4
*
* @brief
*  This API is used to stop DHCPv4 Server on the netif where a DHCPv4 server is running.
*

* @param[in]    netif         Indicates the lwIP network interface.
* @par Return values
*  ERR_OK: On success \n

*
* @par Note
*  The DHCPv4 server can be restarted on a netif only after it is stopped on that netif.
*
*/
err_t netifapi_dhcps_stop(struct netif *netif);
err_t netifapi_dhcps_get_client_ip(struct netif *netif, u8_t *mac, u8_t maclen, ip_addr_t *ip);
#endif /* LWIP_DHCPS */

#if LWIP_DHCP
/**  @ingroup netifapi_dhcp4
* @brief
* This API is used to set a static DHCP structure to the netif.
* @note
* If this API is used before netifapi_dhcp_start(), the application needs to use
* netifapi_dhcp_remove_struct() instead of netifapi_dhcp_cleanup() to remove
* the struct dhcp. */
err_t netifapi_dhcp_set_struct(struct netif *netif, struct dhcp *dhcp);

/**  @ingroup netifapi_dhcp4
* @brief
*  This interface is used to remove the static DHCP structure from netif, which was set
*  using the netifapi_dhcp_set_struct() API.
* @note
* The application needs to use this API instead of netifapi_dhcp_cleanup() if the
* DHCP structure is previously set on netif using netifapi_dhcp_set_struct(). */
err_t netifapi_dhcp_remove_struct(struct netif *netif);

#if LWIP_DHCP_SUBSTITUTE
/**  @ingroup netifapi_dhcp4
* @brief
* This API is used to get MAC of all the dhcp clients to the netif.
*
* @param[in]    netif         Indicates the netif structure on which DHCPC start.
* @param[out]   clis_info     To get the dhcp clients information.
*
* @returns
*  ERR_OK : On success. \n
*  Negative value : On failure. \n
*
* @note
* The clis_info got from this API must be freed by netifapi_dhcp_clients_info_free().
* If the last got clis_info not freed, this API will fail with return value ERR_INPROGRESS. */
err_t netifapi_dhcp_clients_info_get(struct netif *netif, struct dhcp_clients_info **clis_info);

/**  @ingroup netifapi_dhcp4
* @brief
*  This API is used to free clis_info to the netif.
*
* @param[in]    netif         Indicates the netif structure on which DHCPC start.
* @param[in]    clis_info     To clis_info got from netifapi_dhcp_clients_info_get().
*
* @returns
*  ERR_OK : On success. \n
*  Negative value : On failure. \n
*
* @note
* The clis_info must be got from netifapi_dhcp_clients_info_get(). */
err_t netifapi_dhcp_clients_info_free(struct netif *netif, struct dhcp_clients_info **clis_info);

/**  @ingroup netifapi_dhcp4
* @brief
*  This API is used to get IPv4 and IPv6 address of a dhcp client to the netif.
*  The client is represented by mac address.
*
* @param[in]      netif         Indicates the netif structure on which DHCPC start.
* @param[in/out]  cli_info      Caller fillup mac and mac_len fields of cli_info to get ip4addr and ip6addr.
*
* @returns
*  ERR_OK : On success. \n
*  Negative value : On failure. \n
*
* @note
* The mac and mac_len information can be got through netifapi_dhcp_clients_info_get(). */
err_t netifapi_dhcp_client_info_find(struct netif *netif, struct dhcp_client_info *cli_info);

#if LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS
/**  @ingroup netifapi_dhcp4
* @brief
*  This API is used to set the maximum of concurrent DHCP clients.
*
* @param[in]    netif                       Indicates the netif structure on which DHCPC start.
* @param[in]    dhcp_max_concurrent_num     The maximum of concurrent DHCP clients.
*
* @returns
*  ERR_OK : On success. \n
*  Negative value : On failure. \n
*
* @note
* If dhcp_max_concurrent_num setting to be 0, it means not set limitation. */
err_t netifapi_dhcp_set_max_concurrent_num(struct netif *netif, u16_t dhcp_max_concurrent_num);
#endif /* LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS */
#endif /* LWIP_DHCP_SUBSTITUTE */
#endif /* LWIP_DHCP */

/**
 * @defgroup netifapi_dhcp6 DHCPv6
 * @ingroup netifapi
 * To be called from non-TCPIP threads
 */
#if LWIP_IPV6_DHCP6
/*
Func Name:  netifapi_dhcp6_enable_stateful
*/
/**
* @ingroup netifapi_dhcp6
*
* @brief
*  This interface is used to enable stateful DHCPv6 client negotiation for a network interface (stateless disabled).
*  If no DHCPv6 client instance is attached to this interface, a new client is created first. If a DHCPv6 client
*  instance is already present and stateful DHCPv6 client has been enabled, it will return without doing anything.
*  It is the thread-safe way for calling dhcp6_enable_stateful in the user space.
*
* @param[in]    netif        Indicates the lwIP network interface.
*
* @return
*  ERR_OK: On success \n
*  ERR_MEM: On failure due to memory \n
*  ERR_VAL: On failure due to Illegal value or linklocal address is not preferred \n
*  ERR_NOADDR: No available address


* @note
* \n
* - Only one addr of IA_NA is supported. Multiple addresses is not supported yet.
* - Not supported options : IA_TA/Relay/Authentication/Rapid Commit/User Class/Vendor Class/Vendor-specific
*   Information/Interface-Id/Reconfigure Message/Reconfigure Accept/DOMAIN_LIST/IA_PD/IAPREFIX
* - First SOLICIT will not be sent until linklocal IPv6 address is REFERRED
*
* @par Related Topics
* netifapi_set_ip6_autoconfig_enabled()
* netifapi_dhcp6_release_stateful()
* netifapi_dhcp6_disable()
*/
err_t netifapi_dhcp6_enable_stateful(struct netif *netif);

/*
Func Name:  netifapi_dhcp6_enable_stateless
*/
/**
* @ingroup netifapi_dhcp6

*
* @brief
*  This interface is used to enable stateless DHCPv6 client negotiation for a network interface (stateful disabled).
*  If no DHCPv6 client instance is attached to this interface, a new client is created first. If a DHCPv6 client
*  instance is already present and stateless DHCPv6 client has been enabled, it will return without doing anything.
*  This interface only enables other config for DHCPv6. Autoconfig for IPv6 address will not be enabled by this
*  interface.
*  It is the thread-safe way for calling dhcp6_enable_stateless in the user space.
*
* @param[in]    netif        Indicates the lwIP network interface.
*
* @return
*  ERR_OK: On success \n
*  ERR_MEM: On failure due to memory \n
*  ERR_VAL: On failure due to Illegal value

* @note
* \n
* - Only RA from one Router will be handled correctly. RAs from multi Routers will cause unexpected problem.
* - Device will send INFORM-REQUEST after every receved RA.
*
* @par Related Topics
* netifapi_dhcp6_disable()
*/
err_t netifapi_dhcp6_enable_stateless(struct netif *netif);

/*
Func Name:  netifapi_dhcp6_disable
*/
/**
* @ingroup netifapi_dhcp6
*
* @brief
*  This interface is used to disable stateful or stateless DHCPv6 on a network interface.
*  If no DHCPv6 client instance is attached to this interface, it will return without doing anything.
*  If stateful DHCPv6 is enabled, it will try to send a RELEASE, but won't handle the REPLY of this RELEASE.
*  It is the thread-safe way for calling dhcp6_disable in the user space.
*
* @param[in]    netif        Indicates the lwIP network interface.
*
* @return
*  ERR_OK: On success \n
*  ERR_VAL: On failure due to Illegal value
*

* @note
* \n
* - To disable stateful DHCPv6, call after netifapi_dhcp6_release_stateful.
*/
err_t netifapi_dhcp6_disable(struct netif *netif);

/*
Func Name:  netifapi_dhcp6_release_stateful
*/
/**
* @ingroup netifapi_dhcp6

*
* @brief
*  This interface is used to try to send a RELEASE when stateful DHCPv6 is enabled on a network interface.
*  If stateful DHCPv6 client is doing RELEASE, it will return without doing anything.
*  If stateless DHCPv6 is enabled on a network interface, it will return ERR_VAL and do nothing else.
*  If no DHCPv6 client instance is attached to this interface, it will return ERR_VAL and do nothing else.
*  If stateful DHCPv6 is enabled and a valid IPv6 address is using, it will try to send a RELEASE and will
*  handle the REPLY of this RELEASE. It will stop using the IPv6 address to be released as soon as the client
*  begins the Release message exchange process.
*  It is the thread-safe way for calling dhcp6_release_stateful in the user space.
*
* @param[in]    netif        Indicates the lwIP network interface.
*
* @return
*  ERR_OK: On success \n
*  ERR_VAL: On failure due to Illegal value

*/
err_t netifapi_dhcp6_release_stateful(struct netif *netif);

/*
Func Name:  netifapi_dhcp6_cleanup
*/
/**
* @ingroup netifapi_dhcp6
*
* @brief
*  This interface is used to free the memory allocated for DHCPv6 during DHCPv6 enable.
*  It is the thread-safe way for calling dhcp_cleanup in the user space.
*
* @param[in]    netif        Indicates the lwIP network interface.
*
* @return
*  ERR_OK: On success \n
*  ERR_VAL: On failure due to Illegal value
*

* @note
* \n
* - Call after DHCPv6 client is disabled.
* - Call netifapi_dhcp6_disable() before calling netifapi_dhcp6_cleanup.
*/
err_t netifapi_dhcp6_cleanup(struct netif *netif);
#endif /* LWIP_IPV6_DHCP6 */

/* @ingroup netifapi_netif */
err_t netifapi_netif_nameindex_all(void *arg);

#if LWIP_NETIF_HOSTNAME
  /*
  Func Name: netifapi_set_hostname
  */
  /**
  * @ingroup netifapi_dhcp4

  *
  * @brief
  *  This API is used to set the hostname of the netif, which is using in DHCP
  *  message. The hostname string lenght should be less than NETIF_HOSTNAME_MAX_LEN,
  *  otherwise the hostname will truncate to (NETIF_HOSTNAME_MAX_LEN-1).
  *
  * @param[in]    netif    Indicates the lwIP network interface.
  * @param[in]    hostname The new hostname to use.
  * @param[in]    namelen  The hostname string length, should be within the scope of 0~NETIF_HOSTNAME_MAX_LEN-1.
  *
  * @return
  *  ERR_OK: On success \n
  *  ERR_ARG: On passing invalid arguments. \n
  *
  */
  err_t netifapi_set_hostname(struct netif *netif, char *hostname, u8_t namelen);

  /*
  Func Name: netifapi_get_hostname
  */
  /**
  * @ingroup netifapi_dhcp4

  *
  * @brief
  *  This API is used to get the hostname of the netif, which is using in DHCP
  *  message. the hostname buffer length shoud not smaller than NETIF_HOSTNAME_MAX_LEN,
  *  otherwise it will get a truncated hostname.
  *
  * @param[in]    netif    Indicates the lwIP network interface.
  * @param[out]   hostname The buffer to stroe hostname string of the netif.
  * @param[in]    namelen  The hostname string buffer length.
  *
  * @return
  *  ERR_OK: On success \n
  *  ERR_ARG: On passing invalid arguments. \n
  *
  */
  err_t netifapi_get_hostname(struct netif *netif, char *hostname, u8_t namelen);

#endif /* LWIP_NETIF_HOSTNAME */

#if LWIP_IP_FILTER
/*
Func Name:  netifapi_set_ip_filter
*/
/**
* @ingroup netifapi_netif

*

*
* @brief
*  This API is used to set an ip filter for ip packets. The filter function will be called when
*  a packet pass to ip_input. User can define rules to accept or drop a packet in filter_fn,
*  returning 0 means accept, -1 means drop.
*
* @param[in]    filter_fn        The filter function which user implement. Prototype for filter_fn
*                                is err_t (*ip_filter_fn)(const struct pbuf *p, struct netif *inp);
*                                NULL indicate to disable the filter.
* @param[in]    type             IP address type as IPv4 or IPv6. IPADDR_TYPE_V4 is for IPv4 and IPADDR_TYPE_V6
*                                is for IPv6.
*
* @return
*  ERR_OK: On success \n
*  ERR_VAL: On failure due to Illegal value
*

*
* @note
* 1. User can access pbuf in filter_fn, modify pbuf data and access pbuf after filter_fn return are
*    not allowed. \n
* 2. User can define rules to accept or drop a packet in filter_fn, return 0 means accept the
*    packet, -1 means drop the packet and user no need to care about free pbuf. \n
* 3. Rules in filter_fn which user implement should be light and not complex, sleep and other block
*    function must not be called, or it will have a negative influence to TCP/IP stack. \n
* 4. Filter only effect to IP packet.
*
*
*/
err_t netifapi_set_ip_filter(ip_filter_fn filter_fn, int type);
#endif /* LWIP_IP_FILTER */

#if LWIP_IPV6
#if LWIP_NETIF_NBR_CACHE_API

/*
Func Name:  netifapi_add_ipv6_neighbor
*/
/**
* @ingroup netifapi_netif

*
* @brief
*  This API is used to add a neighbor entry to the neighbor cache table. User
*  can use this to add both static and non-sttaic neighbor. The neighbor state
*  passed to the API will determine if the neighbor to be added is static or
*  dynamic. To add static neighbor using this API user must set the confiburation
*  LWIP_ND6_STATIC_NBR to 1

*
* @param[in]    netif     Indicates the network interface object through which this neighbor can be
                                 reached.
* @param[in]    nbr      Provides information about the neighbor to be added. It includes
*                              IPv6 address of the neighbor, Its hardware
*                              address and its length, current state of the
                                neighbor, and Initial reachable duration of the neighbor.
*
* @return
*  ERR_OK: On success \n
*  ERR_ARG: If any parameter is invalid \n
*  ERR_VAL: On failure due to illegal value \n
*  ERR_MEM: If no more space available  \n
*  ERR_NBR_OVERWRITEFORBIDDEN: If try to add a neighbor that already exists
*

*
* @note
* 1. The allowed states for the neighbor are ND6_STATE_REACHABLE(2) for non
*     static neighbor and ND6_STATE_PERMANENT(6) for static neighbor \n
* 2. For static neighbor user must set the reachable time to 0. \n
* 3. For non static neighbors the initial reachable time cannot be greater than
*    the LWIP_ND6_REACHABLE_TIME configured. Once the rechable time set by the
*    user expires, neighbor will move to stale state. When it again moves to the
*    reachable state, the rechable time will be set to LWIP_ND6_REACHABLE_TIME. \n
* 4. If you try to add a neighbor that already exists and it is not a static
*    neighbor we do not allow it to be overwritten. For static neighbor we will
*    update the new hw address. \n
* 5. To use this API user MUST set LWIP_NETIF_NBR_CACHE_API configuration to 1.
* \n

*/
err_t netifapi_add_ipv6_neighbor(struct netif *netif, struct ipv6_neighbor *nbr);

/*
Func Name:  netifapi_del_ipv6_neighbor
*/
/**
* @ingroup netifapi_netif

* @brief
*  This API is used to delete a neighbor entry to the neighbor cache table. User
*  can use this to delete both static and non-sttaic neighbor.
*
* @param[in]    netif     Indicates the network interface object which the neighbor is part of.
* @param[in]    ipaddr   Indicates the IP address of the neighbor to remove.
*
* @return
*  ERR_OK: On success \n
*  ERR_ARG: If any parameter is invalid \n
*  ERR_NBR_NOTFOUND: If the neighbor does not exist

* @note
* 1. If the neighbor to be deleted has an associated default router entry we
*    will remove the default route also. \n
*
* 2. To use this API user MUST set LWIP_NETIF_NBR_CACHE_API configuration to 1
* \n
*/
err_t netifapi_del_ipv6_neighbor(struct netif *netif, struct ip6_addr *ipaddr);

/*
Func Name:  netifapi_query_ipv6_neighbor
*/
/**
* @ingroup netifapi_netif

* @brief
*  This API is used to query a neighbor entry from the neighbor cache table. User
*  can use this to query both static and non-static neighbor.
*
* @param[in]    netif     Indicates the network interface object which the neighbor is part of.
* @param[in]    ipaddr   Indicates the IP address of the neighbor to be queried.
* @param[out]  nbrinfo  Indicates information about the neighbor.
*
* @return
*  ERR_OK: On success \n
*  ERR_ARG: If any parameter is invalid \n
*  ERR_NBR_NOTFOUND: If the neighbor does not exist

*
* @note
* 1. The nbrinfo parameter must be a valid memory of type struct ipv6_neighbor. \n
*
* 2. To use this API user must set LWIP_NETIF_NBR_CACHE_API configuration to 1
* \n
*/
err_t netifapi_query_ipv6_neighbor(struct netif *netif, struct ip6_addr *ipaddr,
                                   struct ipv6_neighbor *nbrinfo);
#endif /* LWIP_NETIF_NBR_CACHE_API */

#if LWIP_ND6_ROUTER
err_t netifapi_set_ra_enable(struct netif *netif, u8_t ra_enable);
err_t netifapi_set_ipv6_forwarding(struct netif *netif, u8_t forwarding);
err_t netifapi_set_accept_ra(struct netif *netif, u8_t accept_ra);
#endif /* LWIP_ND6_ROUTER */
#endif /* LWIP_IPV6 */


#if LWIP_LOWPOWER
err_t netifapi_enable_lowpower(void);
err_t netifapi_disable_lowpower(void);
#endif

#if LWIP_IPV4 && LWIP_ARP
/*
Func Name:  netifapi_ip_to_mac
*/
/**  @ingroup netifapi_netif
* @brief
* This API is used to get MAC address through IPv4 address.
*
* @param[in]       ip         Indicates the IPv4 address to find related MAC address.
* @param[out]      mac        The buffer to record the related MAC address.
* @param[in/out]   maclen     As in parameter, indicates the length of mac buffer.
*                             As out parameter, indicates the length of related MAC address.
*
* @returns
*  ERR_OK : On success. \n
*  Negative value : On failure. \n
*/
err_t netifapi_ip_to_mac(const ip4_addr_t *ip, u8_t *mac, u8_t *maclen);
#endif

#ifdef __cplusplus
}
#endif

#endif /* LWIP_NETIF_API */

#endif /* LWIP_HDR_NETIFAPI_H */
