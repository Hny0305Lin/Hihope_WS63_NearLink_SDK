/**
 * @file
 * API functions for name resolving
 *
 * @defgroup netdbapi NETDB API
 * @ingroup socket
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
 * Author: Simon Goldschmidt
 *
 */

#include "lwip/netdb.h"

#if LWIP_DNS && LWIP_SOCKET

#include "lwip/err.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/ip_addr.h"
#include "lwip/api.h"
#include "lwip/dns.h"

#include <string.h> /* memset */
#include <stdlib.h> /* atoi */

#define SOCKADDR6_TO_IP6ADDR_PORT(sin6, ipaddr, port) do { \
  inet6_addr_to_ip6addr(ip_2_ip6(ipaddr), &((sin6)->sin6_addr)); \
  (port) = lwip_ntohs((sin6)->sin6_port); } while (0)
#define SOCKADDR4_TO_IP4ADDR_PORT(sin, ipaddr, port) do { \
  inet_addr_to_ip4addr(ip_2_ip4(ipaddr), &((sin)->sin_addr)); \
  (port) = lwip_ntohs((sin)->sin_port); } while (0)

#if LWIP_DNS_REVERSE
#if LWIP_IPV4 && LWIP_IPV6
static void
sockaddr_to_ipaddr_port(const struct sockaddr *sockaddr, ip_addr_t *ipaddr, u16_t *port)
{
  if ((sockaddr->sa_family) == AF_INET6) {
    SOCKADDR6_TO_IP6ADDR_PORT((const struct sockaddr_in6*)(const void*)(sockaddr), ipaddr, *port);
    ipaddr->type = IPADDR_TYPE_V6;
  } else {
    SOCKADDR4_TO_IP4ADDR_PORT((const struct sockaddr_in*)(const void*)(sockaddr), ipaddr, *port);
    ipaddr->type = IPADDR_TYPE_V4;
  }
}
#endif

#if LWIP_IPV4 && LWIP_IPV6
#define SOCKADDR_TO_IPADDR_PORT(sockaddr, ipaddr, port) sockaddr_to_ipaddr_port(sockaddr, ipaddr, &(port))
#elif LWIP_IPV6
#define SOCKADDR_TO_IPADDR_PORT(sockaddr, ipaddr, port) \
  SOCKADDR6_TO_IP6ADDR_PORT((const struct sockaddr_in6*)(const void*)(sockaddr), ipaddr, port)
#else /* -> LWIP_IPV4: LWIP_IPV4 && LWIP_IPV6 */
#define SOCKADDR_TO_IPADDR_PORT(sockaddr, ipaddr, port) \
  SOCKADDR4_TO_IP4ADDR_PORT((const struct sockaddr_in*)(const void*)(sockaddr), ipaddr, port)
#endif
#endif /* LWIP_DNS_REVERSE */

/** helper struct for gethostbyname_r to access the char* buffer */
struct gethostbyname_r_helper {
  ip_addr_t *addr_list[DNS_MAX_IPADDR+1];
  ip_addr_t addr[DNS_MAX_IPADDR];
  char *aliases;
};

/** h_errno is exported in netdb.h for access by applications. */
#if LWIP_DNS_API_DECLARE_H_ERRNO
int h_errno;

/* Check if err is in range of h_ernno values */
#define LWIP_ERR_IS_HERRNO(err) ((err) >= HOST_NOT_FOUND && (err) <= TRY_AGAIN)
#endif /* LWIP_DNS_API_DECLARE_H_ERRNO */

/** define "hostent" variables storage: 0 if we use a static (but unprotected)
 * set of variables for lwip_gethostbyname, 1 if we use a local storage */
#ifndef LWIP_DNS_API_HOSTENT_STORAGE
#define LWIP_DNS_API_HOSTENT_STORAGE 0
#endif

/** define "hostent" variables storage */
#if LWIP_DNS_API_HOSTENT_STORAGE
#define HOSTENT_STORAGE
#else
#define HOSTENT_STORAGE static
#endif /* LWIP_DNS_API_STATIC_HOSTENT */

/**
 * Returns an entry containing addresses of address family AF_INET
 * for the host with the name 'name'.
 *
 * @param name the hostname to resolve
 * @return an entry containing addresses of address family AF_INET
 *         for the host with the name 'name'.
 *
 * Note: This function is not thread-safe and should not be used in multiple threads.
 */
struct hostent *
lwip_gethostbyname(const char *name)
{
  err_t err;
  u32_t count = DNS_MAX_IPADDR;
  u32_t  i;
  s32_t addrfamily = 0;

  /* buffer variables for lwip_gethostbyname() */
  HOSTENT_STORAGE struct hostent s_hostent;
  HOSTENT_STORAGE char *s_aliases;
  HOSTENT_STORAGE ip_addr_t s_hostent_addr[DNS_MAX_IPADDR];
  HOSTENT_STORAGE ip_addr_t *s_phostent_addr[DNS_MAX_IPADDR + 1];
  HOSTENT_STORAGE char s_hostname[DNS_MAX_NAME_LENGTH + 1];

  if (name == NULL) {
    LWIP_DEBUGF(DNS_DEBUG, ("lwip_gethostbyname(%s) name = NULL", name));
#if LWIP_DNS_API_DECLARE_H_ERRNO
    h_errno = EFAULT;
#endif /* LWIP_DNS_API_DECLARE_H_ERRNO */
    return NULL;
  }

  /* query host IP address */
  err = netconn_gethostbyname(name, s_hostent_addr, &count);
  if (err != ERR_OK) {
    LWIP_DEBUGF(DNS_DEBUG, ("lwip_gethostbyname(%s) failed, err=%d\n", name, err));

#if LWIP_DNS_API_DECLARE_H_ERRNO
    if (LWIP_ERR_IS_HERRNO(err)) {
      h_errno = err;
    }
#endif /* LWIP_DNS_API_DECLARE_H_ERRNO */
    return NULL;
  }

  /* fill hostent */
  for (i = 0; i < count; i++) {
    s_phostent_addr[i] = &s_hostent_addr[i];
  }

  /* Fix for supporting IPv6 Address Family */
  if (count > 0) {
    /* All addresses in s_hostent_addr list will be of same type
       So, inorder to decide the address family, we need to check the address type of
       just ony entry  */
    if (IP_GET_TYPE(&s_hostent_addr[0]) == IPADDR_TYPE_V4) {
      addrfamily = AF_INET;
    } else {
      addrfamily = AF_INET6;
    }
  } else {
    addrfamily = AF_UNSPEC;
  }

  s_phostent_addr[i] = NULL;
  strncpy(s_hostname, name, DNS_MAX_NAME_LENGTH);
  s_hostname[DNS_MAX_NAME_LENGTH] = 0;
  s_hostent.h_name = s_hostname;
  s_aliases = NULL;
  s_hostent.h_aliases = &s_aliases;
  s_hostent.h_addrtype = addrfamily;
  s_hostent.h_length = sizeof(ip_addr_t);
  s_hostent.h_addr_list = (char**)&(s_phostent_addr[0]);

#if DNS_DEBUG
  /* dump hostent */
  LWIP_DEBUGF(DNS_DEBUG, ("hostent.h_name           == %s\n", s_hostent.h_name));
  LWIP_DEBUGF(DNS_DEBUG, ("hostent.h_aliases        == %p\n", (void *)s_hostent.h_aliases));
  LWIP_DEBUGF(DNS_DEBUG, ("hostent.h_addrtype       == %d\n", s_hostent.h_addrtype));
  LWIP_DEBUGF(DNS_DEBUG, ("hostent.h_length         == %d\n", s_hostent.h_length));
  LWIP_DEBUGF(DNS_DEBUG, ("hostent.h_addr_list      == %p\n", (void *)s_hostent.h_addr_list));
#endif /* DNS_DEBUG */

#if LWIP_DNS_API_HOSTENT_STORAGE
  /* this function should return the "per-thread" hostent after copy from s_hostent */
  return sys_thread_hostent(&s_hostent);
#else
  return &s_hostent;
#endif /* LWIP_DNS_API_HOSTENT_STORAGE */
}

/**
 * Thread-safe variant of lwip_gethostbyname: instead of using a static
 * buffer, this function takes buffer and errno pointers as arguments
 * and uses these for the result.
 *
 * @param name the hostname to resolve
 * @param ret pre-allocated struct where to store the result
 * @param buf pre-allocated buffer where to store additional data
 * @param buflen the size of buf
 * @param result pointer to a hostent pointer that is set to ret on success
 *               and set to zero on error
 * @param h_errnop pointer to an int where to store errors (instead of modifying
 *                 the global h_errno)
 * @return 0 on success, non-zero on error, additional error information
 *         is stored in *h_errnop instead of h_errno to be thread-safe
 */
int
lwip_gethostbyname_r(const char *name, struct hostent *ret, char *buf,
                     size_t buflen, struct hostent **result, int *h_errnop)
{
  err_t err;
  struct gethostbyname_r_helper *h;
  char *hostname;
  size_t namelen;
  int lh_errno;
  u32_t count = DNS_MAX_IPADDR;
  u32_t i;
  s32_t addrfamily = 0;

  if (h_errnop == NULL) {
    /* ensure h_errnop is never NULL */
    h_errnop = &lh_errno;
  }

  if (result == NULL) {
    /* not all arguments given */
    *h_errnop = EINVAL;
    return -1;
  }
  /* first thing to do: set *result to nothing */
  *result = NULL;
  if ((name == NULL) || (ret == NULL) || (buf == NULL)) {
    /* not all arguments given */
    *h_errnop = EINVAL;
    return -1;
  }

  namelen = strlen(name);
  if (buflen < (sizeof(struct gethostbyname_r_helper) + LWIP_MEM_ALIGN_BUFFER(namelen + 1))) {
    /* buf can't hold the data needed + a copy of name */
    *h_errnop = ERANGE;
    return -1;
  }

  h = (struct gethostbyname_r_helper *)LWIP_MEM_ALIGN(buf);
  hostname = ((char *)h) + sizeof(struct gethostbyname_r_helper);

  /* query host IP address */
  err = netconn_gethostbyname(name, h->addr, &count);
  if (err != ERR_OK) {
    LWIP_DEBUGF(DNS_DEBUG, ("lwip_gethostbyname(%s) failed, err=%d\n", name, err));
    /* Fix for supporting proper error information in h_errnop */
    *h_errnop = err;
    return -1;
  }

  /* copy the hostname into buf */
  MEMCPY(hostname, name, namelen);
  hostname[namelen] = 0;

  /* fill hostent */
  for (i = 0; i < count; i++) {
    h->addr_list[i] = &h->addr[i];
  }

  /* Fix for supporting IPv6 Address Family */
  if (count > 0) {
    /* All addresses in s_hostent_addr list will be of same type
       So, inorder to decide the address family, we need to check the address type of
       just ony entry  */
    if (IP_GET_TYPE(h->addr_list[0]) == IPADDR_TYPE_V4) {
      addrfamily = AF_INET;
    } else {
      addrfamily = AF_INET6;
    }
  } else {
    addrfamily = AF_UNSPEC;
  }

  h->addr_list[i] = NULL;
  h->aliases = NULL;
  ret->h_name = hostname;
  ret->h_aliases = &h->aliases;
  ret->h_addrtype = addrfamily;
  ret->h_length = sizeof(ip_addr_t);
  ret->h_addr_list = (char**)&(h->addr_list[0]);

  /* set result != NULL */
  *result = ret;

  /* return success */
  return 0;
}

/**
 * Frees one or more addrinfo structures returned by getaddrinfo(), along with
 * any additional storage associated with those structures. If the ai_next field
 * of the structure is not null, the entire list of structures is freed.
 *
 * @param ai struct addrinfo to free
 */
void
lwip_freeaddrinfo(struct addrinfo *ai)
{
  struct addrinfo *next = NULL;

  while (ai != NULL) {
    next = ai->ai_next;
#if !MEMP_MEM_MALLOC
    if (memp_check(MEMP_NETDB, ai)) {
      memp_free(MEMP_NETDB, ai);
    } else
#endif
    {
      mem_free(ai);
    }
    ai = next;
  }
}

/**
 * Translates the name of a service location (for example, a host name) and/or
 * a service name and returns a set of socket addresses and associated
 * information to be used in creating a socket with which to address the
 * specified service.
 * Memory for the result is allocated internally and must be freed by calling
 * lwip_freeaddrinfo()!
 *
 * Due to a limitation in dns_gethostbyname, only the first address of a
 * host is returned.
 * Also, service names are not supported (only port numbers)!
 *
 * @param nodename descriptive name or address string of the host
 *                 (may be NULL -> local address)
 * @param servname port number as string of NULL
 * @param hints structure containing input values that set socktype and protocol
 * @param res pointer to a pointer where to store the result (set to NULL on failure)
 * @return 0 on success, non-zero on failure
 *
 * @todo: implement AI_V4MAPPED, AI_ADDRCONFIG
 */
int
lwip_getaddrinfo(const char *nodename, const char *servname,
                 const struct addrinfo *hints, struct addrinfo **res)
{
  s32_t err;
  ip_addr_t addr[DNS_MAX_IPADDR];
  struct addrinfo *ai = NULL;
  struct addrinfo *temp = NULL;
  struct sockaddr_storage *sa = NULL;
  int port_nr = 0;
  size_t total_size;
  size_t namelen = 0;
  int ai_family;
  u32_t count = DNS_MAX_IPADDR;
  u32_t i;
  u8_t type = 0;

#if !LWIP_IPV6
  LWIP_UNUSED_ARG(type);
#endif

  if (res == NULL) {
    return EAI_FAIL;
  }
  *res = NULL;
  if ((nodename == NULL) && (servname == NULL)) {
    return EAI_NONAME;
  }

  if (hints != NULL) {
    ai_family = hints->ai_family;
    if ((ai_family != AF_UNSPEC)
#if LWIP_IPV4
        && (ai_family != AF_INET)
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
        && (ai_family != AF_INET6)
#endif /* LWIP_IPV6 */
       ) {
      return EAI_FAMILY;
    }
  } else {
    ai_family = AF_UNSPEC;
  }

  if (servname != NULL) {
    /* service name specified: convert to port number
     * @todo?: currently, only ASCII integers (port numbers) are supported (AI_NUMERICSERV)! */
    port_nr = atoi(servname);
    if (port_nr == 0 && (strcmp(servname, "0") != 0)) {
      /* atoi failed - service was not numeric */
      return EAI_NONAME;
    }
    if ((port_nr < 0) || (port_nr > 0xffff)) {
      return EAI_NONAME;
    }
  }

  if (nodename != NULL) {
    /* service location specified, try to resolve */
    if ((hints != NULL) && (hints->ai_flags & AI_NUMERICHOST)) {
      /* no DNS lookup, just parse for an address string */
      if (!ipaddr_aton(nodename, &addr[0])) {
        return EAI_NONAME;
      }
#if LWIP_IPV4 && LWIP_IPV6
      if ((IP_IS_V6_VAL(addr[0]) && ai_family == AF_INET) ||
          (IP_IS_V4_VAL(addr[0]) && ai_family == AF_INET6)) {
        return EAI_NONAME;
      }
      count = 1;
#endif /* LWIP_IPV4 && LWIP_IPV6 */
    } else {
      namelen = strlen(nodename);
      if (namelen > DNS_MAX_NAME_LENGTH) {
        /* invalid name length */
        LWIP_DEBUGF(DNS_DEBUG, ("nodename is too large\n"));
        return EAI_FAIL;
      }
      /* AF_UNSPEC: prefer DEFAULT configuration */
      type = NETCONN_DNS_DEFAULT;
#if LWIP_IPV4
      if (ai_family == AF_INET) {
        type = NETCONN_DNS_IPV4;
      }
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
      if (ai_family == AF_INET6) {
        type = NETCONN_DNS_IPV6;
      }
#endif /* LWIP_IPV6 */
      err = netconn_gethostbyname_addrtype(nodename, &addr[0], &count, type);
      if (err == TRY_AGAIN) {
        return EAI_AGAIN;
      } else if (err != ERR_OK) {
        return EAI_FAIL;
      }
    }
  } else {
    /* service location specified, use loopback address */
    if ((hints != NULL) && (hints->ai_flags & AI_PASSIVE)) {
      ip_addr_set_any_val(ai_family == AF_INET6, addr[0]);
    } else {
      ip_addr_set_loopback_val(ai_family == AF_INET6, addr[0]);
    }
    count = 1;
  }

  total_size = sizeof(struct addrinfo) + sizeof(struct sockaddr_storage);
  if (nodename != NULL) {
    namelen = strlen(nodename);
    if (namelen > DNS_MAX_NAME_LENGTH) {
      /* invalid name length */
      return EAI_FAIL;
    }
    LWIP_ASSERT("namelen is too long", total_size + namelen + 1 > total_size);
    total_size += namelen + 1;
  }
  /* If this fails, please report to lwip-devel! :-) */
  LWIP_ERROR("total_size <= NETDB_ELEM_SIZE: please report this!",
    (total_size <= NETDB_ELEM_SIZE), return EAI_FAIL);
  for (i = 0; i < count; i++) {
    ai = (struct addrinfo *)memp_malloc(MEMP_NETDB);
    if (ai == NULL) {
      /* dynamic malloc */
      ai = (struct addrinfo *)mem_malloc(total_size);
      if (ai == NULL) {
        LWIP_DEBUGF(DNS_DEBUG, ("malloc failed which made %d addrinfo not deliver to user\n", count - i));
        if (i == 0) {
          return EAI_MEMORY;
        } else {
          return 0;
        }
      }
    }
    memset(ai, 0, total_size);
    /* cast through void* to get rid of alignment warnings */
    sa = (struct sockaddr_storage *)(void *)((u8_t *)ai + sizeof(struct addrinfo));
    if (IP_IS_V6_VAL(addr[i])) {
#if LWIP_IPV6
      struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)sa;
      /* set up sockaddr */
      inet6_addr_from_ip6addr(&sa6->sin6_addr, ip_2_ip6(&addr[i]));
      sa6->sin6_family = AF_INET6;
  /* SIN6_LEN macro is to differntiate whether stack is using 4.3BSD or 4.4BSD variants of sockaddr_in6 structure */
#if defined(SIN6_LEN)
      sa6->sin6_len = sizeof(struct sockaddr_in6);
#endif
      sa6->sin6_flowinfo = 0;
      sa6->sin6_port = lwip_htons((u16_t)port_nr);
      sa6->sin6_scope_id = ip6_addr_zone(ip_2_ip6(&addr[i]));
      ai->ai_family = AF_INET6;
      ai->ai_addrlen = sizeof(struct sockaddr_in6);
#endif /* LWIP_IPV6 */
    } else {
#if LWIP_IPV4
      struct sockaddr_in *sa4 = (struct sockaddr_in *)sa;
      /* set up sockaddr */
      inet_addr_from_ip4addr(&sa4->sin_addr, ip_2_ip4(&addr[i]));
      sa4->sin_family = AF_INET;
      sa4->sin_port = lwip_htons((u16_t)port_nr);
      ai->ai_family = AF_INET;
      ai->ai_addrlen = sizeof(struct sockaddr_in);
#endif /* LWIP_IPV4 */
    }

    /* set up addrinfo */
    if (hints != NULL) {
      /* copy socktype & protocol from hints if specified */
      ai->ai_socktype = hints->ai_socktype;
      ai->ai_protocol = hints->ai_protocol;
    }
    if (nodename != NULL) {
      /* copy nodename to canonname if specified */
      ai->ai_canonname = ((char *)ai + sizeof(struct addrinfo) + sizeof(struct sockaddr_storage));
      MEMCPY(ai->ai_canonname, nodename, namelen);
      ai->ai_canonname[namelen] = 0;
    }
    ai->ai_addr = (struct sockaddr*)sa;

    if (*res == NULL) {
      *res = ai;
    } else if (temp != NULL) {
      temp->ai_next = ai;
    }
    temp = ai;
  }

  return 0;
}

#if LWIP_DNS_REVERSE
static int
lwip_check_nameinfo_args(const struct sockaddr *sa, char *host,
                         socklen_t host_len, int flags, int sa_family)
{
  unsigned int acceptable_flags;
  LWIP_ERROR("lwip_getnameinfo: Parameter sa != NULL", (sa != NULL), return EAI_FAIL);
  if ((sa_family != AF_INET)
#if LWIP_IPV6
    && (sa_family != AF_INET6)
#endif /* LWIP_IPV6 */
    ) {
    LWIP_DEBUGF(DNS_DEBUG, ("lwip_check_nameinfo_flags: sa_family = %d is invalid", sa_family));
    return EAI_FAMILY;
  }

  /* Atleast one among "host" or "serv" should be requested through this API, otherwise return error
   * Currently, we don't support SERVICE NAME Reverse-Lookup */
  if ((host == NULL) || (host_len == 0)) {
    LWIP_DEBUGF(DNS_DEBUG, ("lwip_check_nameinfo_flags: Invalid parameters"));
    return EAI_NONAME;
  }

  /* Since we are not supporting SERVICE NAME Reverse-Lookup NI_DGRAM & NI_NUMERICSERV are not allowed */
  acceptable_flags = NI_NAMEREQD | NI_NUMERICHOST;
  if ((flags <= 0) || ((u32_t)(~acceptable_flags) & (u32_t)flags)) {
    LWIP_DEBUGF(DNS_DEBUG, ("lwip_check_nameinfo_flags: Invalid \"flags\" value"));
    return EAI_BADFLAGS;
  }

  if ((u32_t)flags & NI_NUMERICHOST) {
    LWIP_DEBUGF(DNS_DEBUG,
                ("lwip_check_nameinfo_flags: directly copying given IP to destination buffer"));
#if LWIP_IPV4
    if ((sa_family == AF_INET) && (host_len < IP4ADDR_STRLEN_MAX)) {
      LWIP_DEBUGF(DNS_DEBUG, ("lwip_check_nameinfo_flags: The buffer pointed to by \"host\" is too small"));
      return EAI_OVERFLOW;
    }
#endif /* LWIP_IPV4 */
  
#if LWIP_IPV6
    if ((sa_family == AF_INET6) && (host_len < INET6_ADDRSTRLEN)) {
      LWIP_DEBUGF(DNS_DEBUG, ("lwip_getnameinfo: The buffer pointed to by \"host\" is too small"));
      return EAI_OVERFLOW;
    }
#endif /* LWIP_IPV6 */
  }

  return 0;
}

static int
lwip_nameinfo_convert_to_numeric(const struct sockaddr *sa, char *host, socklen_t host_len, int sa_family)
{
  switch (sa_family) {
#if LWIP_IPV4
    case AF_INET:
      host = (char *)(inet_ntop(AF_INET, (&(((struct sockaddr_in *)sa)->sin_addr)), host, host_len));
      break;
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
    case AF_INET6:
      host = (char *)(inet_ntop(AF_INET6, (&(((struct sockaddr_in6 *)sa)->sin6_addr)), host, host_len));
      break;
#endif /* LWIP_IPV6 */
    default:
      break;
  }
  if (host == NULL) {
    return EAI_FAIL;
  }
  return 0;
}

/*
* @ingroup netdbapi
* @brief   Converts a socket address to a corresponding host and service, in a
* protocol-independent manner. It is reentrant and allows programs to eliminate
* IPv4-versus-IPv6 dependencies.
* Memory for the result is allocated by the caller.
*
* @param sa  Indicates a pointer to a generic socket address structure
*            (of type sockaddr_in or sockaddr_in6) that holds the
*            input IP address and port number.
* @param salen Indicates the size of the generic socket address structure "sa".
* @param host Indicates a pointer to caller-allocated buffer which will holds the null terminated hostname string.
* @param hostlen Indicates the size of "host" buffer.
* @param serv Indicates a pointer to caller-allocated buffer which will holds the null terminated service-name string.
* @param servlen Indicates the size of "serv" buffer.
* @param flags Used to modify the behaviour of lwip_getnameinfo() and can have the following values:
* - NI_NAMEREQD : If set, then an error is returned if the hostname cannot be determined.
* - NI_DGRAM : If set, then the service is datagram (UDP) based rather than stream (TCP) based and causes
               getservbyport() to be called with a second  argument of "udp" instead of its default of "tcp".
               This is required for the few ports (512-514) that have different services for UDP and TCP.
* - NI_NOFQDN : If set, return only the hostname part of the fully qualified domain name for local hosts.
* - NI_NUMERICHOST : If set, then the numeric form of the hostname is returned.
                     (When not set, this will still happen in case the node's name cannot be determined.)
* - NI_NUMERICSERV : If set, then the numeric form of the service address is returned.
                     (When not set, this will still happen in case the service's name cannot be determined.)
* @return
* 0: On success \n
* Non-zero error number: On failure
* EAI_FAIL   : A nonrecoverable error occurred.
* EAI_FAMILY : The requested address family is not supported.
* EAI_NONAME : The name does not resolve for the supplied arguments.
               NI_NAMEREQD is set and the host's name cannot be located,
               or neither hostname nor service name were requested.
* EAI_OVERFLOW : The buffer pointed to by host or serv was too small.
* EAI_BADFLAGS : The "flags" argument has an invalid value..

* @par Note
* - No support for translation of service names.\n
* - Since there is no support for Service names, the flags - NI_DGRAM, NI_NUMERICSERV is not supported.
* - NI_NOFQDN is not currently implemented
*/

/**
 * @page RFC-2553 RFC-2553
 * @par RFC-2553 Compliance Information
 * @par Compliant Section
 * Section 6.5
 * @par Compliance Information
 * The following flag values are supported.
 * - NI_NAMEREQD: If set, then an error is returned if the hostname cannot be determined. \n
 *   NI_NUMERICHOST: If set, then the numeric form of the hostname is returned.\n
 *   When not set, this will still happen in case the node's name cannot be determined.\n
 * @par Non-Compliance Information
 *   NI_NOFQDN flag is not supported.\n
 *   EAI_AGAIN, EAI_MEMORY, EAI_SYSTEM error codes not supported for the API getnameinfo().\n
 * @par Not Applicable Sections
 * Since there is no service-name lookup, NI_DGRAM & NI_NUMERICSERV flags are not supported.
*/
int
lwip_getnameinfo(const struct sockaddr *sa, socklen_t sa_len, char *host,
                 socklen_t host_len, char *serv, socklen_t serve_len, int flags)
{
  int ret, sa_family;
  ip_addr_t ip_addr_to_resolve;
  u16_t service_port;
  char hostname[NI_MAXHOST] = "";

  LWIP_UNUSED_ARG(sa_len);
  /* Currently, we don't support SERVICE NAME Reverse-Lookup */
  LWIP_UNUSED_ARG(serv);
  LWIP_UNUSED_ARG(serve_len);
  sa_family = sa->sa_family;
  ret = lwip_check_nameinfo_args(sa, host, host_len, flags, sa_family);
  if (ret != 0) {
    return ret;
  }
  
  /* If NI_NUMERICHOST is set, then no need to go for resolution,
    directly copy the given IP to the destination buffer */
  if ((u32_t)flags & NI_NUMERICHOST) {
    return lwip_nameinfo_convert_to_numeric(sa, host, host_len, sa_family);
  }

  SOCKADDR_TO_IPADDR_PORT(sa, &ip_addr_to_resolve, service_port);

  if (netconn_getnameinfo(&ip_addr_to_resolve, hostname) != ERR_OK) {
    if ((u32_t)flags & NI_NAMEREQD) {
      return EAI_NONAME;
    } else {
      return lwip_nameinfo_convert_to_numeric(sa, host, host_len, sa_family);
    }
  }

  if (host_len <= strlen(hostname)) {
    LWIP_DEBUGF(DNS_DEBUG, ("lwip_getnameinfo: The buffer pointed to by \"host\" is too small"));
    return EAI_OVERFLOW;
  }
  (void)strncpy_s(host, host_len, hostname, strlen(hostname));
  return 0;
}
#endif /* LWIP_DNS_REVERSE */

#endif /* LWIP_DNS && LWIP_SOCKET */
