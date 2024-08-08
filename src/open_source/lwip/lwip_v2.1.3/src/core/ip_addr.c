/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: This is the IPv4 & IPv6 address tools implementation.
 * Author: none
 * Create: 2020
 */

#include "lwip/opt.h"
#include "lwip/inet.h"

#if LWIP_IPV4 || LWIP_IPV6

#include "lwip/ip_addr.h"

#if LWIP_INET_ADDR_FUNC
in_addr_t inet_addr(const char *cp)
{
  LWIP_ERROR("inet_aton:cp is NULL", (cp != NULL), return (INADDR_NONE));
  return ipaddr_addr(cp);
}
#endif

#if LWIP_INET_ATON_FUNC
int inet_aton(const char *cp, struct in_addr *inp)
{
  LWIP_ERROR("inet_aton:cp is NULL", (cp != NULL), return 0);
  return ip4addr_aton(cp, (ip4_addr_t *)inp);
}
#endif

#if LWIP_INET_NTOA_FUNC
char *inet_ntoa(struct in_addr in)
{
  return ip4addr_ntoa((const ip4_addr_t *)&in);
}
#endif
#endif /* LWIP_IPV4 || LWIP_IPV6 */

#if LWIP_IPV4 && LWIP_IPV6
#if LWIP_SMALL_SIZE
int ip_addr_cmp(const ip_addr_t *addr1, const ip_addr_t *addr2)
{
  if (addr1 == NULL || addr2 == NULL) {
    return 0;
  }

  return ((IP_GET_TYPE(addr1) != IP_GET_TYPE(addr2)) ? 0 : (IP_IS_V6_VAL(*(addr1)) ? \
          ip6_addr_cmp(ip_2_ip6(addr1), ip_2_ip6(addr2)) : \
          ip4_addr_cmp(ip_2_ip4(addr1), ip_2_ip4(addr2))));
}

int ip_addr_isany(const ip_addr_t *ipaddr)
{
  return (((ipaddr) == NULL) ? 1 : ((IP_IS_V6(ipaddr)) ? ip6_addr_isany(ip_2_ip6(ipaddr)) :
          ip4_addr_isany(ip_2_ip4(ipaddr))));
}

int ip_addr_isloopback(const ip_addr_t *ipaddr)
{
  if (ipaddr == NULL) {
    return 0;
  }
  return ((IP_IS_V6(ipaddr)) ? ip6_addr_isloopback(ip_2_ip6(ipaddr)) :
          ip4_addr_isloopback(ip_2_ip4(ipaddr)));
}

void ip_addr_copy_fun(ip_addr_t *dest, ip_addr_t *src)
{
  *dest = *src;
  if (!IP_IS_V6_VAL(*src)) {
    ip_clear_no4(dest);
  }
}

#endif /* LWIP_SMALL_SIZE */
#endif /* LWIP_IPV4 && LWIP_IPV6 */