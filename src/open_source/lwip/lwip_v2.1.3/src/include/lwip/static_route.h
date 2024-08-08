#ifndef __LWIP_ROUTE_H__
#define __LWIP_ROUTE_H__

#include "lwip/opt.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_IPV4_ROUTE_TABLE_SUPPORT
#define LWIP_HOOK_IP4_ROUTE(dest)                   ip4_static_route_lookup(dest)
#define LWIP_HOOK_ETHARP_GET_GW(netif, dest)        ip4_static_route_get_gw(netif, dest)

/**
 *@ingroup lwip_route
 *@brief add a route entry.
 *
 *@par Description:
 *This API is used to add a route entry into static route table.
 *@attention
 *<ul>
 *<li>The params must be valid. </li>
 *</ul>
 *
 *@param ipaddr      [IN] dest ipaddr.
 *@param netmask   [IN] netmask for ipaddr.
 *@param gateway   [IN] gateway for packets will be sent to ipaddr.
 *@param ifname     [IN] the name of innterface to which packets for this route will be sent.
 *
 *@retval #ERR_ARG      Invalid ipaddr,netmask,gateway. 
 *@retval #ERR_RTE       Invalid ifname.
 *@retval #ERR_OK        The sroute entry is successfully added.
 *@par Dependency:
 *<ul><li>static_route.h: the header file that contains the API declaration.</li></ul>
 *@see lwip_ip4_remove_route_entry
 *@since LiteOS V200R001C00
 */
err_t lwip_ip4_add_route_entry(const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gateway, const char *ifname);

/**
 *@ingroup lwip_ip4_remove_route_entry
 *@brief remove a route entry.
 *
 *@par Description:
 *This API is used to remove a route entry in static route table.
 *@attention
 *<ul>
 *<li>The params must be valid. </li>
 *</ul>
 *
 *@param ipaddr      [IN] dest ipaddr.
 *@param ifname     [IN] the name of innterface to which packets for this route will be sent.
 *
 *@retval #ERR_ARG      Invalid ipaddr. 
 *@retval #ERR_OK        The sroute entry is successfully removed.
 *@par Dependency:
 *<ul><li>static_route.h: the header file that contains the API declaration.</li></ul>
 *@see lwip_ip4_add_route_entry
 *@since LiteOS V200R001C00
 */
err_t lwip_ip4_remove_route_entry(const ip4_addr_t *ipaddr, const char *ifname);

/**
 *@ingroup lwip_ip4_remove_route_entry
 *@brief remove a route entry.
 *
 *@par Description:
 *This API is used to remove a route entry in static route table.
 *@attention
 *<ul>
 *<li>The params must be valid. </li>
 *</ul>
 *
 *@param printf_buf      [IN] buffer for print route table.
 *@param buf_len     [IN] the length of buffer.
 *
 *@retval #ERR_ARG      Invalid printf_buf. 
 *@retval #ERR_OK        The sroute entry is successfully added.
 *@par Dependency:
 *<ul><li>static_route.h: the header file that contains the API declaration.</li></ul>
 *@see lwip_ip4_add_route_entry
 *@since LiteOS V200R001C00
 */
err_t lwip_route_table_show(char *printf_buf, unsigned int buf_len);

struct netif *ip4_static_route_lookup(const ip4_addr_t *dest);
const ip4_addr_t *ip4_static_route_get_gw(struct netif *netif, const ip4_addr_t *dest);

#endif

#ifdef __cplusplus
}
#endif

#endif

