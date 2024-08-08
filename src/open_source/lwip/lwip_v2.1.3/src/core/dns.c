/**
 * @file
 * DNS - host name to IP address resolver.
 *
 * @defgroup dns DNS
 * @ingroup callbackstyle_api
 *
 * Implements a DNS host name to IP address resolver.
 *
 * The lwIP DNS resolver functions are used to lookup a host name and
 * map it to a numerical IP address. It maintains a list of resolved
 * hostnames that can be queried with the dns_lookup() function.
 * New hostnames can be resolved using the dns_query() function.
 *
 * The lwIP version of the resolver also adds a non-blocking version of
 * gethostbyname() that will work with a raw API application. This function
 * checks for an IP address string first and converts it if it is valid.
 * gethostbyname() then does a dns_lookup() to see if the name is
 * already in the table. If so, the IP is returned. If not, a query is
 * issued and the function returns with a ERR_INPROGRESS status. The app
 * using the dns client must then go into a waiting state.
 *
 * Once a hostname has been resolved (or found to be non-existent),
 * the resolver code calls a specified callback function (which
 * must be implemented by the module that uses the resolver).
 *
 * Multicast DNS queries are supported for names ending on ".local".
 * However, only "One-Shot Multicast DNS Queries" are supported (RFC 6762
 * chapter 5.1), this is not a fully compliant implementation of continuous
 * mDNS querying!
 *
 * All functions must be called from TCPIP thread.
 *
 * @see DNS_MAX_SERVERS
 * @see LWIP_DHCP_MAX_DNS_SERVERS
 * @see @ref netconn_common for thread-safe access.
 */

/*
 * Port to lwIP from uIP
 * by Jim Pettinato April 2007
 *
 * security fixes and more by Simon Goldschmidt
 *
 * uIP version Copyright (c) 2002-2003, Adam Dunkels.
 * All rights reserved.
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
 */

/*-----------------------------------------------------------------------------
 * RFC 1035 - Domain names - implementation and specification
 * RFC 2181 - Clarifications to the DNS Specification
 *----------------------------------------------------------------------------*/

/** @todo: define good default values (rfc compliance) */
/** @todo: improve answer parsing, more checkings... */
/** @todo: check RFC1035 - 7.3. Processing responses */
/** @todo: one-shot mDNS: dual-stack fallback to another IP version */

/*-----------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#include "lwip/opt.h"

#if LWIP_DNS /* don't build if not configured for use in lwipopts.h */

#include "lwip/def.h"
#include "lwip/udp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dns.h"
#include "lwip/prot/dns.h"
#include "lwip/sys.h"
#include "arch/sys_arch.h"
#include "lwip/tcpip.h"
#include "lwip/netdb.h"

#include <string.h>

#if LWIP_DNS64 && LWIP_NAT64
#include "lwip/nat64.h"
#include "lwip/nat64_addr.h"
#endif

#if LWIP_RIPPLE
#include "lwip/lwip_rpl.h"
#endif

/** Random generator function to create random TXIDs and source ports for queries */
#ifndef DNS_RAND_TXID
#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_XID) != 0) && defined(LWIP_RAND)
#define DNS_RAND_TXID LWIP_RAND
#else
static u16_t dns_txid;
#define DNS_RAND_TXID() (++dns_txid)
#endif
#endif

/** Limits the source port to be >= 1024 by default */
#ifndef DNS_PORT_ALLOWED
#define DNS_PORT_ALLOWED(port) ((port) >= 1024)
#endif

/** DNS resource record max. TTL (one week as default) */
#ifndef DNS_MAX_TTL
#define DNS_MAX_TTL               604800
#elif DNS_MAX_TTL > 0x7FFFFFFF
#error DNS_MAX_TTL must be a positive 32-bit value
#endif

#if DNS_TABLE_SIZE > 255
#error DNS_TABLE_SIZE must fit into an u8_t
#endif
#if DNS_MAX_SERVERS > 255
#error DNS_MAX_SERVERS must fit into an u8_t
#endif

#if LWIP_RIPPLE
#define IS_DNS_SERVER_VALID(server) ((!ip_addr_isany(server)) && (!ip_addr_ismulticast(server)) && \
                                     (!ip_addr_islinklocal(server)) && (!ip_addr_isloopback(server)))
#endif

/* The number of parallel requests (i.e. calls to dns_gethostbyname
 * that cannot be answered from the DNS table.
 * This is set to the table size by default.
 */
#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0)
#ifndef DNS_MAX_REQUESTS
#define DNS_MAX_REQUESTS          DNS_TABLE_SIZE
#else
#if DNS_MAX_REQUESTS > 255
#error DNS_MAX_REQUESTS must fit into an u8_t
#endif
#endif
#else
/* In this configuration, both arrays have to have the same size and are used
 * like one entry (used/free) */
#define DNS_MAX_REQUESTS          DNS_TABLE_SIZE
#endif

/* The number of UDP source ports used in parallel */
#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0)
#ifndef DNS_MAX_SOURCE_PORTS
#define DNS_MAX_SOURCE_PORTS      DNS_MAX_REQUESTS
#else
#if DNS_MAX_SOURCE_PORTS > 255
#error DNS_MAX_SOURCE_PORTS must fit into an u8_t
#endif
#endif
#else
#ifdef DNS_MAX_SOURCE_PORTS
#undef DNS_MAX_SOURCE_PORTS
#endif
#define DNS_MAX_SOURCE_PORTS      1
#endif

/* This entry is allowed to use this DNS server (unmask this server) */
#define DNS_SERVER_SET_AVAILABLE(entry, server_idx) do { \
  (entry)->server_bitmap &= ~(u32_t)(1 << (server_idx)); \
} while (0)

/* This entry is NOT allowed to use this DNS server in this transaction (mask this server) */
#define DNS_SERVER_SET_UNAVAILABLE(entry, server_idx) do { \
  (entry)->server_bitmap |= (1 << (server_idx)); \
} while (0)

/* Check if this entry is allowed to use this DNS server */
#define DNS_SERVER_IS_AVAILABLE(entry, server_idx) \
  (((entry)->server_bitmap & (1 << (server_idx))) == 0 && !ip_addr_isany(&dns_servers[(server_idx)]))

/* Switch to the available DNS server when searching */
#define DNS_SERVER_SEARCHING_FLAG_SWITCH 0x1

#if LWIP_DNS_REVERSE
#define REVERSE_DOMAIN_MAXLEN 256
#endif

#if LWIP_IPV4 && LWIP_IPV6
#define LWIP_DNS_ADDRTYPE_IS_IPV6(t) (((t) == LWIP_DNS_ADDRTYPE_IPV6_IPV4) || ((t) == LWIP_DNS_ADDRTYPE_IPV6))
#define LWIP_DNS_ADDRTYPE_IS_FALLBACK(t) (((t) == LWIP_DNS_ADDRTYPE_IPV4_IPV6) || ((t) == LWIP_DNS_ADDRTYPE_IPV6_IPV4))
#define LWIP_DNS_ADDRTYPE_MATCH_IP(t, ip) (IP_IS_V6_VAL(ip) ? \
                                          (LWIP_DNS_ADDRTYPE_IS_IPV6(t) || LWIP_DNS_ADDRTYPE_IS_FALLBACK(t)) : \
                                          (!LWIP_DNS_ADDRTYPE_IS_IPV6(t)) || LWIP_DNS_ADDRTYPE_IS_FALLBACK(t))
#define LWIP_DNS_ADDRTYPE_ARG(x) , x
#define LWIP_DNS_ADDRTYPE_ARG_OR_ZERO(x) x
#define LWIP_DNS_SET_ADDRTYPE(x, y) do { x = y; } while(0)
#else
#if LWIP_IPV6
#define LWIP_DNS_ADDRTYPE_IS_IPV6(t) 1
#else
#define LWIP_DNS_ADDRTYPE_IS_IPV6(t) 0
#endif /* LWIP_IPV6 */
#define LWIP_DNS_ADDRTYPE_IS_FALLBACK(t) 0
#define LWIP_DNS_ADDRTYPE_MATCH_IP(t, ip) 1
#define LWIP_DNS_ADDRTYPE_ARG(x)
#define LWIP_DNS_ADDRTYPE_ARG_OR_ZERO(x) 0
#define LWIP_DNS_SET_ADDRTYPE(x, y)
#endif /* LWIP_IPV4 && LWIP_IPV6 */

#if LWIP_DNS_SUPPORT_MDNS_QUERIES
#define LWIP_DNS_ISMDNS_ARG(x) , x
#else
#define LWIP_DNS_ISMDNS_ARG(x)
#endif

#define LOCALHOST_STRING  "localhost"

/* DNS table entry states */
typedef enum {
  DNS_STATE_UNUSED           = 0,
  DNS_STATE_NEW              = 1,
  DNS_STATE_ASKING           = 2,
  DNS_STATE_DONE             = 3
} dns_state_enum_t;

/** DNS table entry */
struct dns_table_entry {
  u32_t ttl;
  u32_t ipaddr_count;
  u32_t first_send_time;
  ip_addr_t ipaddr[DNS_MAX_IPADDR];
  u16_t txid;
  u8_t  state;
  u8_t  server_idx;
  u32_t server_bitmap;  /* Bitmap for available DNS servers. 0-unmasked, 1-masked */
  u8_t  tmr;
  u8_t  retries;
  u8_t  seqno;
#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0)
  u8_t pcb_idx;
#endif
  char name[DNS_MAX_NAME_LENGTH];
#if LWIP_IPV4 && LWIP_IPV6
  u8_t reqaddrtype;
#endif /* LWIP_IPV4 && LWIP_IPV6 */
#if LWIP_DNS_SUPPORT_MDNS_QUERIES
  u8_t is_mdns;
#endif
};

#if LWIP_DNS_REVERSE
struct reverse_domain {
  /* Encoded domain name */
  char name[REVERSE_DOMAIN_MAXLEN];
  /* Total length of domain name, including zero */
  size_t length;
};

/* Reverse DNS table entry */
struct reverse_dns_table_entry {
  u32_t ttl;
  u32_t name_count;
  u32_t first_send_time;
  struct reverse_domain encoded_ip;
  char name[NI_MAXHOST + 1];
  u16_t txid;
  u8_t  state;
  u8_t  server_idx;
  u32_t server_bitmap;  /* Bitmap for available DNS servers. 0-unmasked, 1-masked */
  u8_t  tmr;
  u8_t  retries;
  u8_t  seqno;
#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0)
  u8_t pcb_idx;
#endif /* (LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0 */
#if LWIP_DNS_SUPPORT_MDNS_QUERIES
  u8_t is_mdns;
#endif /* LWIP_DNS_SUPPORT_MDNS_QUERIES */
};
#endif

/** DNS request table entry: used when dns_gethostbyname cannot answer the
 * request from the DNS table */
struct dns_req_entry {
  /* pointer to callback on DNS query done */
  dns_found_callback found;
  /* argument passed to the callback function */
  void *arg;
#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0)
  u8_t dns_table_idx;
#endif
#if LWIP_IPV4 && LWIP_IPV6
  u8_t reqaddrtype;
#endif /* LWIP_IPV4 && LWIP_IPV6 */
};

#if LWIP_DNS_REVERSE
/* Reverse DNS request table entry: used when reverse_dns_getnamebyhost cannot answer the
 * request from the Reverse DNS table */
struct reverse_dns_req_entry {
  /* pointer to callback on DNS query done */
  reverse_dns_found_callback found;
  /* argument passed to the callback function */
  void *arg;
#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0)
  u8_t reverse_dns_table_idx;
#endif /* (LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0 */
};
#endif

#if DNS_LOCAL_HOSTLIST

#if DNS_LOCAL_HOSTLIST_IS_DYNAMIC
/** Local host-list. For hostnames in this list, no
 *  external name resolution is performed */
static struct local_hostlist_entry *local_hostlist_dynamic;
#else /* DNS_LOCAL_HOSTLIST_IS_DYNAMIC */

/** Defining this allows the local_hostlist_static to be placed in a different
 * linker section (e.g. FLASH) */
#ifndef DNS_LOCAL_HOSTLIST_STORAGE_PRE
#define DNS_LOCAL_HOSTLIST_STORAGE_PRE static
#endif /* DNS_LOCAL_HOSTLIST_STORAGE_PRE */
/** Defining this allows the local_hostlist_static to be placed in a different
 * linker section (e.g. FLASH) */
#ifndef DNS_LOCAL_HOSTLIST_STORAGE_POST
#define DNS_LOCAL_HOSTLIST_STORAGE_POST
#endif /* DNS_LOCAL_HOSTLIST_STORAGE_POST */
DNS_LOCAL_HOSTLIST_STORAGE_PRE struct local_hostlist_entry local_hostlist_static[]
  DNS_LOCAL_HOSTLIST_STORAGE_POST = DNS_LOCAL_HOSTLIST_INIT;

#endif /* DNS_LOCAL_HOSTLIST_IS_DYNAMIC */

static void dns_init_local(void);
static err_t dns_lookup_local(const char *hostname, ip_addr_t *addr LWIP_DNS_ADDRTYPE_ARG(u8_t dns_addrtype));
#endif /* DNS_LOCAL_HOSTLIST */


/* forward declarations */
static err_t dns_send(u8_t idx);
static void dns_recv(void *s, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

static void process_forward_dns(struct pbuf *p, u8_t entry_idx, struct dns_table_entry *entry, struct dns_hdr hdr,
                                const ip_addr_t *addr);
static err_t dns_check_entry(u8_t i);
static void dns_check_entries(void);
static void dns_correct_response(u8_t idx, u32_t ttl);
static void dns_call_found(u8_t idx, const ip_addr_t *addr, u32_t ipaddr_count);
static u16_t dns_create_txid(void);
static void dns_find_first_server(void);

static err_t
dns_enqueue(const char *name, size_t hostnamelen, dns_found_callback found,
            void *callback_arg, u8_t dns_addrtype LWIP_DNS_ISMDNS_ARG(u8_t is_mdns));
static u8_t
dns_backupserver_available(struct dns_table_entry *pentry, u8_t flags);

static u16_t
dns_compare_name(char *query, struct pbuf *p, u16_t start_offset, u16_t allow_compressed_hdr);
static err_t
dns_lookup(const char *name, ip_addr_t *addr, u32_t *count LWIP_DNS_ADDRTYPE_ARG(u8_t dns_addrtype));

#if LWIP_DNS_REVERSE
#ifdef LWIP_TESTBED
err_t reverse_dns_domain_add_label(struct reverse_domain *domain, const char *label, size_t len);
#else
static err_t reverse_dns_domain_add_label(struct reverse_domain *domain, const char *label, size_t len);
#endif

#if LWIP_IPV4
static err_t reverse_dns_build_reverse_v4_domain(struct reverse_domain *domain, const ip4_addr_t *addr);
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
static err_t reverse_dns_build_reverse_v6_domain(struct reverse_domain *domain, const ip6_addr_t *addr);
#endif /* LWIP_IPV6 */

static void process_reverse_dns(struct pbuf *p, u8_t entry_idx, struct reverse_dns_table_entry *reverse_entry,
                                struct dns_hdr hdr, const ip_addr_t *addr);

static void reverse_dns_check_entries(void);
static err_t reverse_dns_lookup(const struct reverse_domain *encoded_ip, char *hostname);
static err_t reverse_dns_send(u8_t idx);
static void reverse_dns_call_found(u8_t idx, const char *hostname, u32_t name_count);
static err_t reverse_dns_check_entry(u8_t i);
static void reverse_dns_correct_response(u8_t idx, u32_t ttl);
static err_t reverse_dns_enqueue(const struct reverse_domain *encoded_ip, reverse_dns_found_callback found,
                                 void *callback_arg);
static u8_t reverse_dns_backupserver_available(struct reverse_dns_table_entry *rentry, u8_t flags);
#endif /* LWIP_DNS_REVERSE */

/*-----------------------------------------------------------------------------
 * Globals
 *----------------------------------------------------------------------------*/

/* DNS variables */
static struct udp_pcb        *dns_pcbs[DNS_MAX_SOURCE_PORTS];
#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0)
static u8_t                   dns_last_pcb_idx;
#endif /* (LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0 */
static u8_t                   dns_seqno = 0;
static struct dns_table_entry dns_table[DNS_TABLE_SIZE];
static struct dns_req_entry   dns_requests[DNS_MAX_REQUESTS];
static ip_addr_t              dns_servers[DNS_MAX_SERVERS];

#if LWIP_DNS_REVERSE
static struct reverse_dns_table_entry reverse_dns_table[DNS_TABLE_SIZE];
static struct reverse_dns_req_entry   reverse_dns_requests[DNS_MAX_REQUESTS];
#endif /* LWIP_DNS_REVERSE */

#if LWIP_IPV4
const ip_addr_t dns_mquery_v4group = DNS_MQUERY_IPV4_GROUP_INIT;
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
const ip_addr_t dns_mquery_v6group = DNS_MQUERY_IPV6_GROUP_INIT;
#endif /* LWIP_IPV6 */

static u8_t dns_first_server_idx = 0;
#if LWIP_DNS_REVERSE
static u8_t rdns_first_server_idx = 0;
#endif /* LWIP_DNS_REVERSE */
struct dns_msg {
  u8_t numdns;
  ip_addr_t *dnsserver;
  sys_sem_t cb_completed;
};

/**
 * Initialize the resolver: set up the UDP pcb and configure the default server
 * (if DNS_SERVER_ADDRESS is set).
 */
void
dns_init(void)
{
#ifdef DNS_SERVER_ADDRESS
  /* initialize default DNS server address */
  ip_addr_t dnsserver;
  DNS_SERVER_ADDRESS(&dnsserver);
  dns_setserver(0, &dnsserver);
#endif /* DNS_SERVER_ADDRESS */

  LWIP_ASSERT("sanity check SIZEOF_DNS_QUERY",
              sizeof(struct dns_query) == SIZEOF_DNS_QUERY);
  LWIP_ASSERT("sanity check SIZEOF_DNS_ANSWER",
              sizeof(struct dns_answer) <= SIZEOF_DNS_ANSWER_ASSERT);

  LWIP_DEBUGF(DNS_DEBUG, ("dns_init: initializing\n"));

  /* if dns client not yet initialized... */
#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) == 0)
  if (dns_pcbs[0] == NULL) {
    dns_pcbs[0] = udp_new_ip_type(IPADDR_TYPE_ANY);
    LWIP_ASSERT("dns_pcbs[0] != NULL", dns_pcbs[0] != NULL);

    /* initialize DNS table not needed (initialized to zero since it is a
     * global variable) */
    LWIP_ASSERT("For implicit initialization to work, DNS_STATE_UNUSED needs to be 0",
                DNS_STATE_UNUSED == 0);

#if LWIP_SO_PRIORITY
    dns_pcbs[0]->priority = LWIP_PKT_PRIORITY_DNS;
#endif /* LWIP_SO_PRIORITY */

    /* initialize DNS client */
    udp_bind(dns_pcbs[0], IP_ANY_TYPE, 0);
    udp_recv(dns_pcbs[0], dns_recv, NULL);
  }
#endif

#if DNS_LOCAL_HOSTLIST
  dns_init_local();
#endif
}

/**
 * @ingroup dns
 * Initialize one of the DNS servers.
 *
 * @param numdns the index of the DNS server to set must be < DNS_MAX_SERVERS
 * @param dnsserver IP address of the DNS server to set
 */
void
dns_setserver(u8_t numdns, const ip_addr_t *dnsserver)
{
  u8_t i;
#if LWIP_RIPPLE
  u8_t changed = 0;
  u8_t cnt;
  ip_addr_t server;
#endif
  if (numdns < DNS_MAX_SERVERS) {
#if LWIP_RIPPLE
    server = dns_servers[numdns];
#endif
    if (dnsserver != NULL) {
      dns_servers[numdns] = (*dnsserver);
    } else {
      dns_servers[numdns] = *IP_ADDR_ANY;
    }

    /* once any change of DNS server happens, simply set it available here,
    and check if it is a valid server when searching for first_server and backup_server. */
    for (i = 0; i < DNS_TABLE_SIZE; i++) {
      dns_table[i].server_bitmap = 0;
#if LWIP_DNS_REVERSE
      reverse_dns_table[i].server_bitmap = 0;
#endif /* LWIP_DNS_REVERSE */
    }

    /* once any changes of DNS servers, refind the first server */
    dns_find_first_server();
#if LWIP_RIPPLE
    if ((dnsserver != NULL) && IS_DNS_SERVER_VALID(dnsserver) &&
        (memcmp(&server, dnsserver, sizeof(ip_addr_t)) != 0)) {
      changed = 1;
    } else if ((dnsserver == NULL) && IS_DNS_SERVER_VALID(&server)) {
      cnt = lwip_dns_get_server_cnt(NULL, NULL);
      if (cnt > 0) {
        changed = 1;
      }
    }

    if (changed == 1) {
#ifdef LWIP_DNS_RPL_REPAIR
      lwip_rpl_trigger_global_repair();
#else
      lwip_rpl_trigger_global_dao();
#endif
    }
#endif
  }
}

void
dns_setserver_internal(void *arg)
{
  struct dns_msg *msg = NULL;

  msg = (struct dns_msg *)arg;

  dns_setserver(msg->numdns, msg->dnsserver);
  sys_sem_signal(&msg->cb_completed);
}

err_t
lwip_dns_setserver(u8_t numdns, ip_addr_t *dnsserver)
{
  struct dns_msg dmsg;
  err_t err;

  (void)memset_s(&dmsg, sizeof(dmsg), 0, sizeof(dmsg));
  if (numdns >= DNS_MAX_SERVERS) {
    LWIP_DEBUGF(DNS_DEBUG, ("lwip_dns_setserver: Invalid param, index %"S32_F", "
                "DNS_MAX_SERVERS is %"S32_F"\n", numdns, DNS_MAX_SERVERS));
    return ERR_VAL;
  }

  dmsg.numdns = numdns;
  dmsg.dnsserver = dnsserver;

  err = sys_sem_new(&dmsg.cb_completed, 0);
  if (err != ERR_OK) {
    return err;
  }

  err = tcpip_callback(dns_setserver_internal, &dmsg);
  if (err != ERR_OK) {
    sys_sem_free(&dmsg.cb_completed);
    return err;
  }

  (void)sys_arch_sem_wait(&dmsg.cb_completed, 0);
  sys_sem_free(&dmsg.cb_completed);

  return ERR_OK;
}

/**
 * @ingroup dns
 * Obtain one of the currently configured DNS server.
 *
 * @param numdns the index of the DNS server
 * @return IP address of the indexed DNS server or "ip_addr_any" if the DNS
 *         server has not been configured.
 */
const ip_addr_t*
dns_getserver(u8_t numdns)
{
  if (numdns < DNS_MAX_SERVERS) {
    return &dns_servers[numdns];
  } else {
    return IP_ADDR_ANY;
  }
}

void
dns_getserver_internal(void *arg)
{
  struct dns_msg *msg = NULL;

  msg = (struct dns_msg *)arg;

  *msg->dnsserver = *dns_getserver(msg->numdns);
  sys_sem_signal(&msg->cb_completed);
}

err_t
lwip_dns_getserver(u8_t numdns, ip_addr_t *dnsserver)
{
  struct dns_msg dmsg;
  err_t err;

  (void)memset_s(&dmsg, sizeof(dmsg), 0, sizeof(dmsg));
  if (dnsserver == NULL) {
    LWIP_DEBUGF(DNS_DEBUG, ("lwip_dns_setserver: Invalid param, dnsserver 0x%p\n", (void *)dnsserver));
    return ERR_VAL;
  }

  dmsg.numdns = numdns;
  dmsg.dnsserver = dnsserver;

  err = sys_sem_new(&dmsg.cb_completed, 0);
  if (err != ERR_OK) {
    return err;
  }

  err = tcpip_callback(dns_getserver_internal, &dmsg);
  if (err != ERR_OK) {
    sys_sem_free(&dmsg.cb_completed);
    return err;
  }

  (void)sys_arch_sem_wait(&dmsg.cb_completed, 0);
  sys_sem_free(&dmsg.cb_completed);

  return ERR_OK;
}

u8_t
lwip_dns_get_server_cnt(u8_t *ip4cnt, u8_t *ip6cnt)
{
  int i;
  u8_t cnt = 0;

  if (ip4cnt != NULL) {
    *ip4cnt = 0;
  }

  if (ip6cnt != NULL) {
    *ip6cnt = 0;
  }

  for (i = 0; i < DNS_MAX_SERVERS; i++) {
    if (ip_addr_isany_val(dns_servers[i])) {
      continue;
    }

    cnt++;
    if (IP_IS_V6_VAL(dns_servers[i])) {
      (ip6cnt != NULL) ? (*ip6cnt)++ : 0;
    } else if (ip4cnt != NULL) {
      (*ip4cnt)++;
    }
  }

  return cnt;
}

#if LWIP_IPV6
err_t
lwip_dns_copy_ip6server_addr(const struct netif *nif, ip6_addr_t *ip6addr, u8_t num)
{
  int i;
  int cnt = 0;

  if ((nif == NULL) || (ip6addr == NULL) || (num == 0)) {
    return ERR_ARG;
  }

  for (i = 0; i < DNS_MAX_SERVERS; i++) {
    if (ip_addr_isany_val(dns_servers[i])) {
      continue;
    }

    if (IP_IS_V6_VAL(dns_servers[i]) && (!ip_addr_isloopback(&dns_servers[i])) &&
        (!ip_addr_ismulticast(&dns_servers[i])) && (!ip_addr_islinklocal(&dns_servers[i]))) {
      ip6_addr_copy(ip6addr[cnt], *ip_2_ip6(&dns_servers[i]));
      cnt++;
      if (cnt >= num) {
        return ERR_OK;
      }
      continue;
    }
#if LWIP_RIPPLE && LWIP_DNS64 && LWIP_NAT64
    if ((lwip_rpl_is_rpl_netif(nif) == lwIP_TRUE) && (lwip_rpl_is_br() == lwIP_TRUE) &&
        (nat64_status_check() == NAT64_RET_OK) && IP_IS_V4_VAL(dns_servers[i]) &&
        (nat64_stateless_addr_4to6(ip_2_ip4(&dns_servers[i]), &ip6addr[cnt]) == ERR_OK)) {
      cnt++;
      if (cnt >= num) {
        return ERR_OK;
      }
    }
#endif
  }

  if (cnt != num) {
    return ERR_VAL;
  }

  return ERR_OK;
}
#endif

#if LWIP_DNS_REVERSE
/*
 * Add a label part to a domain
 * @param domain The domain to add a label to
 * @param label The label to add, like <hostname>, 'local', 'com' or ''
 * @param len The length of the label
 * @return ERR_OK on success, an err_t otherwise if label too long
 */
#ifdef LWIP_TESTBED
err_t
#else
static err_t
#endif
reverse_dns_domain_add_label(struct reverse_domain *domain, const char *label, size_t len)
{
  if (len > DNS_MAX_LABEL_LENGTH) {
    return ERR_VAL;
  }
  if ((len > 0) && (1 + len + domain->length >= REVERSE_DOMAIN_MAXLEN)) {
    return ERR_VAL;
  }
  /* Allow only zero marker on last byte */
  if ((len == 0) && (1 + domain->length > REVERSE_DOMAIN_MAXLEN)) {
    return ERR_VAL;
  }
  if ((domain->length != 0) && (label != NULL)) {
    domain->name[domain->length] = '.';
    domain->length++;
  }
  if ((len != 0) && (label != NULL)) {
    if (memcpy_s(&domain->name[domain->length], (REVERSE_DOMAIN_MAXLEN - domain->length), label, len) != EOK) {
      return ERR_MEM;
    }
    domain->length += len;
  }
  return ERR_OK;
}


#if LWIP_IPV4
/*
 * Build domain for reverse lookup of IPv4 address
 * 192.168.0.12 will be mapped to 12.0.168.192.in-addr.arpa.
 * @param domain Where to write the domain name
 * @param addr Pointer to an IPv4 address to encode
 * @return ERR_OK if domain was written, an err_t otherwise
 */
static err_t
reverse_dns_build_reverse_v4_domain(struct reverse_domain *domain, const ip4_addr_t *addr)
{
  int i;
  err_t res;
  const u8_t *ptr = NULL;
  /* ip4 address have 4 parts, biggges parts is 255, less than 4 */
  char buf[4];
  u8_t val;

  LWIP_ERROR("reverse_dns_build_reverse_v4_domain: domain is NULL", (domain != NULL), return ERR_ARG);
  LWIP_ERROR("reverse_dns_build_reverse_v4_domain: addr is NULL", (addr != NULL), return ERR_ARG);

  (void)memset_s(domain, sizeof(struct reverse_domain), 0, sizeof(struct reverse_domain));

  ptr = (const u8_t *)addr;
  for (i = (int)(sizeof(ip4_addr_t)) - 1; i >= 0; --i) {
    val = ptr[i];
    lwip_itoa(buf, sizeof(buf), val);
    res = reverse_dns_domain_add_label(domain, buf, strlen(buf));
    LWIP_ERROR("reverse_dns_build_reverse_v4_domain: Failed to add label: buf", (res == ERR_OK), return res);
  }
  res = reverse_dns_domain_add_label(domain, REVERSE_PTR_V4_DOMAIN, (sizeof(REVERSE_PTR_V4_DOMAIN)-1));
  LWIP_ERROR("reverse_dns_build_reverse_v4_domain: Failed to add label: REVERSE_PTR_V4_DOMAIN",
             (res == ERR_OK), return res);
  res = reverse_dns_domain_add_label(domain, REVERSE_PTR_TOP_DOMAIN, (sizeof(REVERSE_PTR_TOP_DOMAIN)-1));
  LWIP_ERROR("reverse_dns_build_reverse_v4_domain: Failed to add label: REVERSE_PTR_TOP_DOMAIN",
             (res == ERR_OK), return res);
  res = reverse_dns_domain_add_label(domain, NULL, 0);
  LWIP_ERROR("reverse_dns_build_reverse_v4_domain: Failed to add label: NULL", (res == ERR_OK), return res);

  return ERR_OK;
}
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
/*
 * Build domain for reverse lookup of IP address
 * 2001:db8::567:89ab will be mapped to b.a.9.8.7.6.5.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.8.b.d.0.1.0.0.2.ip6.arpa.
 * @param domain Where to write the domain name
 * @param addr Pointer to an IPv6 address to encode
 * @return ERR_OK if domain was written, an err_t otherwise
 */
static err_t
reverse_dns_build_reverse_v6_domain(struct reverse_domain *domain, const ip6_addr_t *addr)
{
  int i;
  int j;
  err_t res;
  u8_t byte;
  char buf;
  const u8_t *ptr = NULL;

  LWIP_ERROR("reverse_dns_build_reverse_v6_domain: domain is NULL", (domain != NULL), return ERR_ARG);
  LWIP_ERROR("reverse_dns_build_reverse_v6_domain: addr is NULL", (addr != NULL), return ERR_ARG);

  (void)memset_s(domain, sizeof(struct reverse_domain), 0, sizeof(struct reverse_domain));

  ptr = (const u8_t *)addr;
  for (i = (int)(sizeof(ip6_addr_t)) - 1; i >= 0; i--) {
    byte = ptr[i];
    /* each byte have 2 nibbles */
    for (j = 0; j < 2; j++) {
      if ((byte & 0x0F) < 0xA) {
        buf = (char)('0' + (byte & 0x0F));
      } else {
        buf = (char)('a' + (byte & 0x0F) - (0xA));
      }
      res = reverse_dns_domain_add_label(domain, &buf, sizeof(buf));
      LWIP_ERROR("reverse_dns_build_reverse_v6_domain: Failed to add label: buf", (res == ERR_OK), return res);
      /* get next nibble */
      byte >>= 4;
    }
  }
  res = reverse_dns_domain_add_label(domain, REVERSE_PTR_V6_DOMAIN, (sizeof(REVERSE_PTR_V6_DOMAIN)-1));
  LWIP_ERROR("reverse_dns_build_reverse_v6_domain: Failed to add label: REVERSE_PTR_V6_DOMAIN",
             (res == ERR_OK), return res);
  res = reverse_dns_domain_add_label(domain, REVERSE_PTR_TOP_DOMAIN, (sizeof(REVERSE_PTR_TOP_DOMAIN)-1));
  LWIP_ERROR("reverse_dns_build_reverse_v6_domain: Failed to add label: REVERSE_PTR_TOP_DOMAIN",
             (res == ERR_OK), return res);
  res = reverse_dns_domain_add_label(domain, NULL, 0);
  LWIP_ERROR("reverse_dns_build_reverse_v6_domain: Failed to add label: NULL", (res == ERR_OK), return res);

  return ERR_OK;
}
#endif /* LWIP_IPV6 */
#endif /* LWIP_DNS_REVERSE */

/*
 * The DNS resolver client timer - handle retries and timeouts and should
 * be called every DNS_TMR_INTERVAL milliseconds (every second by default).
 */
void
dns_tmr(void)
{
  LWIP_DEBUGF(DNS_DEBUG, ("dns_tmr: dns_check_entries\n"));
  dns_check_entries();
#if LWIP_DNS_REVERSE
  reverse_dns_check_entries();
#endif /* LWIP_DNS_REVERSE */
}

#if LWIP_LOWPOWER
#include "lwip/lowpower.h"
u32_t
dns_tmr_tick(void)
{
  u32_t tick = 0;
  u32_t val;
  s32_t i;

  for (i = 0; i < DNS_TABLE_SIZE; i++) {
    if ((dns_table[i].state == DNS_STATE_NEW) ||
        (dns_table[i].state == DNS_STATE_ASKING)) {
      LOWPOWER_DEBUG(("%s tmr tick: 1\n", __func__));
      return 1;
    }
    if (dns_table[i].state == DNS_STATE_DONE) {
      val = dns_table[i].ttl;
      SET_TMR_TICK(tick, val);
    }
#if LWIP_DNS_REVERSE
    if ((reverse_dns_table[i].state == DNS_STATE_NEW) ||
        (reverse_dns_table[i].state == DNS_STATE_ASKING)) {
      LOWPOWER_DEBUG(("%s tmr tick: 1\n", __func__));
      return 1;
    }
    if (reverse_dns_table[i].state == DNS_STATE_DONE) {
      val = reverse_dns_table[i].ttl;
      SET_TMR_TICK(tick, val);
    }
#endif /* LWIP_DNS_REVERSE */
  }
  LOWPOWER_DEBUG(("%s tmr tick: %u\n", __func__, tick));
  return tick;
}
#endif

#if DNS_LOCAL_HOSTLIST
static void
dns_init_local(void)
{
#if DNS_LOCAL_HOSTLIST_IS_DYNAMIC && defined(DNS_LOCAL_HOSTLIST_INIT)
  size_t i;
  struct local_hostlist_entry *entry;
  /* Dynamic: copy entries from DNS_LOCAL_HOSTLIST_INIT to list */
  struct local_hostlist_entry local_hostlist_init[] = DNS_LOCAL_HOSTLIST_INIT;
  size_t namelen;
  for (i = 0; i < LWIP_ARRAYSIZE(local_hostlist_init); i++) {
    struct local_hostlist_entry *init_entry = &local_hostlist_init[i];
    LWIP_ASSERT("invalid host name (NULL)", init_entry->name != NULL);
    namelen = strlen(init_entry->name);
    LWIP_ASSERT("namelen <= DNS_LOCAL_HOSTLIST_MAX_NAMELEN", namelen <= DNS_LOCAL_HOSTLIST_MAX_NAMELEN);
    entry = (struct local_hostlist_entry *)memp_malloc(MEMP_LOCALHOSTLIST);
    LWIP_ASSERT("mem-error in dns_init_local", entry != NULL);
    if (entry != NULL) {
      entry->name = (char*)entry + sizeof(struct local_hostlist_entry);
      MEMCPY((char*)entry->name, init_entry->name, namelen);
      ((char*)entry->name)[namelen] = 0;
      entry->addr = init_entry->addr;
      entry->next = local_hostlist_dynamic;
      local_hostlist_dynamic = entry;
    }
  }
#endif /* DNS_LOCAL_HOSTLIST_IS_DYNAMIC && defined(DNS_LOCAL_HOSTLIST_INIT) */
}

/**
 * @ingroup dns
 * Iterate the local host-list for a hostname.
 *
 * @param iterator_fn a function that is called for every entry in the local host-list
 * @param iterator_arg 3rd argument passed to iterator_fn
 * @return the number of entries in the local host-list
 */
size_t
dns_local_iterate(dns_found_callback iterator_fn, void *iterator_arg)
{
  size_t i;
#if DNS_LOCAL_HOSTLIST_IS_DYNAMIC
  struct local_hostlist_entry *entry = local_hostlist_dynamic;
  i = 0;
  while (entry != NULL) {
    if (iterator_fn != NULL) {
      iterator_fn(entry->name, &entry->addr, 1, iterator_arg);
    }
    i++;
    entry = entry->next;
  }
#else /* DNS_LOCAL_HOSTLIST_IS_DYNAMIC */
  for (i = 0; i < LWIP_ARRAYSIZE(local_hostlist_static); i++) {
    if (iterator_fn != NULL) {
      iterator_fn(local_hostlist_static[i].name, &local_hostlist_static[i].addr, 1, iterator_arg);
    }
  }
#endif /* DNS_LOCAL_HOSTLIST_IS_DYNAMIC */
  return i;
}

/**
 * @ingroup dns
 * Scans the local host-list for a hostname.
 *
 * @param hostname Hostname to look for in the local host-list
 * @param addr the first IP address for the hostname in the local host-list or
 *         IPADDR_NONE if not found.
 * @param dns_addrtype - LWIP_DNS_ADDRTYPE_IPV4_IPV6: try to resolve IPv4 (ATTENTION: no fallback here!)
 *                     - LWIP_DNS_ADDRTYPE_IPV6_IPV4: try to resolve IPv6 (ATTENTION: no fallback here!)
 *                     - LWIP_DNS_ADDRTYPE_IPV4: try to resolve IPv4 only
 *                     - LWIP_DNS_ADDRTYPE_IPV6: try to resolve IPv6 only
 * @return ERR_OK if found, ERR_ARG if not found
 */
err_t
dns_local_lookup(const char *hostname, ip_addr_t *addr, u8_t dns_addrtype)
{
  LWIP_UNUSED_ARG(dns_addrtype);
  return dns_lookup_local(hostname, addr LWIP_DNS_ADDRTYPE_ARG(dns_addrtype));
}

/* Internal implementation for dns_local_lookup and dns_lookup */
static err_t
dns_lookup_local(const char *hostname, ip_addr_t *addr LWIP_DNS_ADDRTYPE_ARG(u8_t dns_addrtype))
{
#if DNS_LOCAL_HOSTLIST_IS_DYNAMIC
  struct local_hostlist_entry *entry = local_hostlist_dynamic;
  while (entry != NULL) {
    if ((lwip_stricmp(entry->name, hostname) == 0) &&
        LWIP_DNS_ADDRTYPE_MATCH_IP(dns_addrtype, entry->addr)) {
      if (addr) {
        ip_addr_copy(*addr, entry->addr);
      }
      return ERR_OK;
    }
    entry = entry->next;
  }
#else /* DNS_LOCAL_HOSTLIST_IS_DYNAMIC */
  size_t i;
  for (i = 0; i < LWIP_ARRAYSIZE(local_hostlist_static); i++) {
    if ((lwip_stricmp(local_hostlist_static[i].name, hostname) == 0) &&
        LWIP_DNS_ADDRTYPE_MATCH_IP(dns_addrtype, local_hostlist_static[i].addr)) {
      if (addr) {
        ip_addr_copy(*addr, local_hostlist_static[i].addr);
      }
      return ERR_OK;
    }
  }
#endif /* DNS_LOCAL_HOSTLIST_IS_DYNAMIC */
  return ERR_ARG;
}

#if DNS_LOCAL_HOSTLIST_IS_DYNAMIC
/**
 * @ingroup dns
 * Remove all entries from the local host-list for a specific hostname
 * and/or IP address
 *
 * @param hostname hostname for which entries shall be removed from the local
 *                 host-list
 * @param addr address for which entries shall be removed from the local host-list
 * @return the number of removed entries
 */
int
dns_local_removehost(const char *hostname, const ip_addr_t *addr)
{
  int removed = 0;
  struct local_hostlist_entry *entry = local_hostlist_dynamic;
  struct local_hostlist_entry *last_entry = NULL;
  while (entry != NULL) {
    if (((hostname == NULL) || !lwip_stricmp(entry->name, hostname)) &&
        ((addr == NULL) || ip_addr_cmp(&entry->addr, addr))) {
      struct local_hostlist_entry *free_entry;
      if (last_entry != NULL) {
        last_entry->next = entry->next;
      } else {
        local_hostlist_dynamic = entry->next;
      }
      free_entry = entry;
      entry = entry->next;
      memp_free(MEMP_LOCALHOSTLIST, free_entry);
      removed++;
    } else {
      last_entry = entry;
      entry = entry->next;
    }
  }
  return removed;
}

/**
 * @ingroup dns
 * Add a hostname/IP address pair to the local host-list.
 * Duplicates are not checked.
 *
 * @param hostname hostname of the new entry
 * @param addr IP address of the new entry
 * @return ERR_OK if succeeded or ERR_MEM on memory error
 */
err_t
dns_local_addhost(const char *hostname, const ip_addr_t *addr)
{
  struct local_hostlist_entry *entry;
  size_t namelen;
  LWIP_ASSERT("invalid host name (NULL)", hostname != NULL);
  namelen = strlen(hostname);
  LWIP_ASSERT("namelen <= DNS_LOCAL_HOSTLIST_MAX_NAMELEN", namelen <= DNS_LOCAL_HOSTLIST_MAX_NAMELEN);
  entry = (struct local_hostlist_entry *)memp_malloc(MEMP_LOCALHOSTLIST);
  if (entry == NULL) {
    return ERR_MEM;
  }
  entry->name = (char*)entry + sizeof(struct local_hostlist_entry);
  MEMCPY((char*)entry->name, hostname, namelen);
  ((char*)entry->name)[namelen] = 0;

  ip_addr_copy(entry->addr, *addr);
  entry->next = local_hostlist_dynamic;
  local_hostlist_dynamic = entry;
  return ERR_OK;
}
#endif /* DNS_LOCAL_HOSTLIST_IS_DYNAMIC*/
#endif /* DNS_LOCAL_HOSTLIST */

/**
 * @ingroup dns
 * Look up a hostname in the array of known hostnames.
 *
 * @note This function only looks in the internal array of known
 * hostnames, it does not send out a query for the hostname if none
 * was found. The function dns_enqueue() can be used to send a query
 * for a hostname.
 *
 * @param name the hostname to look up
 * @param addr the hostname's IP address, as u32_t (instead of ip_addr_t to
 *         better check for failure: != IPADDR_NONE) or IPADDR_NONE if the hostname
 *         was not found in the cached dns_table.
 * @param count the maximum number of resolved IP Address requested for name
 * @return ERR_OK if found, ERR_ARG if not found
 */
static err_t
dns_lookup(const char *name, ip_addr_t *addr, u32_t *count LWIP_DNS_ADDRTYPE_ARG(u8_t dns_addrtype))
{
  u8_t i;
  u8_t j;
  u8_t matching_response = 0;
#if DNS_LOCAL_HOSTLIST
  if (dns_lookup_local(name, addr LWIP_DNS_ADDRTYPE_ARG(dns_addrtype)) == ERR_OK) {
    *count = 1;
    return ERR_OK;
  }
#endif /* DNS_LOCAL_HOSTLIST */
#ifdef DNS_LOOKUP_LOCAL_EXTERN
  if (DNS_LOOKUP_LOCAL_EXTERN(name, addr, LWIP_DNS_ADDRTYPE_ARG_OR_ZERO(dns_addrtype)) == ERR_OK) {
    *count = 1;
    return ERR_OK;
  }
#endif /* DNS_LOOKUP_LOCAL_EXTERN */

  /* Walk through name list, return entry if found. If not, return NULL. */
  for (i = 0; i < DNS_TABLE_SIZE; ++i) {
    if ((dns_table[i].state == DNS_STATE_DONE) &&
        (lwip_strnicmp(name, dns_table[i].name, sizeof(dns_table[i].name)) == 0)) {

      LWIP_DEBUGF(DNS_DEBUG, ("dns_lookup : found = \"%s\"", name));

      for (j = 0; j < LWIP_MIN(*count, dns_table[i].ipaddr_count); j++) {
        if (LWIP_DNS_ADDRTYPE_MATCH_IP(dns_addrtype, dns_table[i].ipaddr[j])) {
#ifdef  LWIP_DEV_DEBUG
          ip_addr_debug_print_val(DNS_DEBUG, dns_table[i].ipaddr[j]);
          LWIP_DEBUGF(DNS_DEBUG, ("\n"));
#endif  /* LWIP_DEV_DEBUG */
          LWIP_DEBUGF(DNS_DEBUG, ("dns_lookup : found = \"%s\" with matching ADDRTYPE\n", name));
          ip_addr_copy(addr[matching_response], dns_table[i].ipaddr[j]);
          ++matching_response;
        }
      }

      if (matching_response == 0) {
        continue;
      }

      *count = matching_response;
      return ERR_OK;
    }
  }

  return ERR_ARG;
}

#if LWIP_DNS_REVERSE
static err_t
reverse_dns_lookup(const struct reverse_domain *encoded_ip, char *hostname)
{
  u8_t i;

  /* Walk through name list, return entry if found. If not, return NULL. */
  for (i = 0; i < DNS_TABLE_SIZE; ++i) {
    if ((reverse_dns_table[i].state == DNS_STATE_DONE) &&
        (lwip_strnicmp((const char*)(encoded_ip->name), (const char*)(reverse_dns_table[i].encoded_ip.name),
                       sizeof(reverse_dns_table[i].encoded_ip.name)) == 0)) {
      if (strncpy_s(hostname, NI_MAXHOST, reverse_dns_table[i].name, strlen(reverse_dns_table[i].name)) != EOK) {
        LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_lookup : memory error\n"));
        return ERR_MEM;
      }
      LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_lookup : found = \"%s\"\n", hostname));

      return ERR_OK;
    }
  }
  return ERR_ARG;
}
#endif /* LWIP_DNS_REVERSE */

/**
 * Compare the "dotted" name "query" with the encoded name "response"
 * to make sure an answer from the DNS server matches the current dns_table
 * entry (otherwise, answers might arrive late for hostname not on the list
 * any more).
 *
 * For now, this function compares case-insensitive to cope with all kinds of
 * servers. This also means that "dns 0x20 bit encoding" must be checked
 * externally, if we want to implement it.
 * Currently, the request is sent exactly as passed in by he user request.
 *
 * @param query hostname (not encoded) from the dns_table
 * @param p pbuf containing the encoded hostname in the DNS response
 * @param start_offset offset into p where the name starts
 * @return 0xFFFF: names differ, other: names equal -> offset behind name
 */
static u16_t
dns_compare_name(char *query, struct pbuf *p, u16_t start_offset, u16_t allow_compressed_hdr)
{
  unsigned char n;
  u16_t response_offset = start_offset;
  u16_t ret;

  do {
    n = (u8_t)pbuf_try_get_at(p, response_offset);
    if (!n) {
      /* error or overflow */
      return 0xFFFF;
    }
    response_offset++;
    /** @see RFC 1035 - 4.1.4. Message compression */
    if (((n & 0xc0) == 0xc0)  && (response_offset == (start_offset + 1)) && (allow_compressed_hdr)) {
      /* Compressed name: cannot be equal since we don't send them */
      n = (u8_t)pbuf_try_get_at(p, response_offset);

      if (!n) {
        /* if the offset of compressed hdr is 0, fail from here */
        return 0xFFFF;
      }

      ret = dns_compare_name(query, p, (u16_t)n, 0);
      if (ret == 0xFFFF) {
        return 0xFFFF;
      } else if (ret == n) {
        return start_offset;
      } else {
        return (u16_t)(response_offset + (u16_t)1);
      }
    } else if (n > DNS_MAX_LABEL_LENGTH) {
      return 0xFFFF;
    } else {
      /* Not compressed name */
      while ((n > 0) && (*query != 0)) {
        u8_t c = pbuf_get_at(p, response_offset);
        if (lwip_tolower((*query)) != lwip_tolower((u8_t)c)) {
          return start_offset;
        }
        response_offset++;
        ++query;
        --n;
      }
      if (*query) {
        ++query;
      }
    }
  } while ((pbuf_get_at(p, response_offset) != 0) && (*query != 0));

  if (response_offset > p->tot_len) {
    return 0xFFFF;
  }
  if ((pbuf_get_at(p, response_offset) == 0) && (*query == 0) && (n == 0)) {
    return (u16_t)(response_offset + (u16_t)1);
  }
  return start_offset;
}

/**
 * Walk through a compact encoded DNS name and return the end of the name.
 *
 * @param p pbuf containing the name
 * @param query_idx start index into p pointing to encoded DNS name in the DNS server response
 * @return index to end of the name
 */
u16_t
dns_skip_name(const struct pbuf *p, u16_t query_idx)
{
  int n;
  u16_t offset = query_idx;

  do {
    n = pbuf_try_get_at(p, offset++);
    if ((n < 0) || (offset == 0)) {
      return 0xFFFF;
    }
    /** @see RFC 1035 - 4.1.4. Message compression */
    if ((n & 0xc0) == 0xc0) {
      /* Compressed name: since we only want to skip it (not check it), stop here */
      break;
    } else if (n > DNS_MAX_LABEL_LENGTH) {
      return 0xFFFF;
    } else if (offset + n >= p->tot_len) {
      /* Not compressed name */
      return 0xFFFF;
    } else {
      offset = (u16_t)(offset + n);
    }
    n = pbuf_try_get_at(p, offset);
    if (n < 0) {
      return 0xFFFF;
    }
  } while (n != 0);

  if (offset == 0xFFFF) {
    return 0xFFFF;
  }
  return (u16_t)(offset + 1);
}

/**
 * Send a DNS query packet.
 *
 * @param idx the DNS table entry index for which to send a request
 * @return ERR_OK if packet is sent; an err_t indicating the problem otherwise
 */
static err_t
dns_send(u8_t idx)
{
  err_t err;
  struct dns_hdr hdr;
  struct dns_query qry;
  struct pbuf *p;
  u16_t query_idx, copy_len;
  const char *hostname, *hostname_part;
  u8_t n;
  u8_t pcb_idx;
  struct dns_table_entry *entry = &dns_table[idx];
  const ip_addr_t *dst;
  ip_addr_t dns_server_addr;
  u16_t dst_port;

  LWIP_DEBUGF(DNS_DEBUG, ("dns_send: dns_servers[%"U16_F"] \"%s\": request\n",
                          (u16_t)(entry->server_idx), entry->name));
  LWIP_ASSERT("dns server out of array", entry->server_idx < DNS_MAX_SERVERS);

  if (!DNS_SERVER_IS_AVAILABLE(entry, entry->server_idx)
#if LWIP_DNS_SUPPORT_MDNS_QUERIES
      && !entry->is_mdns
#endif
     ) {
    /* DNS server not valid anymore, e.g. PPP netif has been shut down */
    /* call specified callback function if provided */
    dns_call_found(idx, NULL, 0);
    /* flush this entry */
    entry->retries = DNS_MAX_RETRIES - 1;
    return ERR_OK;
  }

  // memory overflow (adding const and variable)
  if (strlen(entry->name) > DNS_MAX_NAME_LENGTH) {
    return ERR_MEM;
  }

  /* if here, we have either a new query or a retry on a previous query to process */
  p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(SIZEOF_DNS_HDR + strlen(entry->name) + 2 +
                                         SIZEOF_DNS_QUERY), PBUF_RAM);
  if (p != NULL) {

    /* fill dns header */
    memset(&hdr, 0, SIZEOF_DNS_HDR);
    hdr.id = lwip_htons(entry->txid);
    hdr.flags1 = DNS_FLAG1_RD;
    hdr.numquestions = PP_HTONS(1);
    pbuf_take(p, &hdr, SIZEOF_DNS_HDR);
    hostname = entry->name;
    --hostname;

    /* convert hostname into suitable query format. */
    query_idx = SIZEOF_DNS_HDR;
    do {
      ++hostname;
      hostname_part = hostname;
      for (n = 0; *hostname != '.' && *hostname != 0; ++hostname, ++n) {
      }
      if (n > DNS_MAX_LABEL_LENGTH) {
        (void)pbuf_free(p);
        entry->state = DNS_STATE_UNUSED;
        return ERR_ARG;
      }
      copy_len = (u16_t)(hostname - hostname_part);
      if (query_idx + n + 1 > 0xFFFF) {
        /* u16_t overflow */
        goto overflow_return;
      }
      pbuf_put_at(p, query_idx, n);
      pbuf_take_at(p, hostname_part, copy_len, (u16_t)(query_idx + 1));
      query_idx = (u16_t)(query_idx + n + 1);
    } while (*hostname != 0);
    pbuf_put_at(p, query_idx, 0);
    query_idx++;

    /* fill dns query */
    if (LWIP_DNS_ADDRTYPE_IS_IPV6(entry->reqaddrtype)) {
      qry.type = PP_HTONS(DNS_RRTYPE_AAAA);
    } else {
      qry.type = PP_HTONS(DNS_RRTYPE_A);
    }
    qry.cls = PP_HTONS(DNS_RRCLASS_IN);
    pbuf_take_at(p, &qry, SIZEOF_DNS_QUERY, query_idx);

#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0)
    pcb_idx = entry->pcb_idx;
#else
    pcb_idx = 0;
#endif
    /* send dns packet */
    LWIP_DEBUGF(DNS_DEBUG, ("sending DNS request ID %d for name \"%s\" to server %d\r\n",
                            entry->txid, entry->name, entry->server_idx));
#if LWIP_DNS_SUPPORT_MDNS_QUERIES
    if (entry->is_mdns) {
      dst_port = DNS_MQUERY_PORT;
#if LWIP_IPV6
      if (LWIP_DNS_ADDRTYPE_IS_IPV6(entry->reqaddrtype)) {
        dst = &dns_mquery_v6group;
      }
#endif
#if LWIP_IPV4 && LWIP_IPV6
      else
#endif
#if LWIP_IPV4
      {
        dst = &dns_mquery_v4group;
      }
#endif
    } else
#endif /* LWIP_DNS_SUPPORT_MDNS_QUERIES */
    {
      dst_port = DNS_SERVER_PORT;
      ip_addr_copy(dns_server_addr, dns_servers[entry->server_idx]);

#if LWIP_IPV4 && LWIP_IPV6
      /* Dual-stack: Unmap IPv4 mapped IPv6 addresses */
      if (IP_IS_V6_VAL(dns_server_addr) && ip6_addr_isipv4mappedipv6(ip_2_ip6(&dns_server_addr))) {
        unmap_ipv4_mapped_ipv6(ip_2_ip4(&dns_server_addr), ip_2_ip6(&dns_server_addr));
        IP_SET_TYPE_VAL(dns_server_addr, IPADDR_TYPE_V4);
      }
#endif /* LWIP_IPV4 && LWIP_IPV6 */
      dst = &dns_server_addr;
    }
    err = udp_sendto(dns_pcbs[pcb_idx], p, dst, dst_port);

    /* free pbuf */
    pbuf_free(p);
  } else {
    err = ERR_MEM;
  }

  return err;
overflow_return:
  pbuf_free(p);
  return ERR_VAL;
}

#if LWIP_DNS_REVERSE
static err_t
reverse_dns_send(u8_t idx)
{
  err_t err;
  struct dns_hdr hdr;
  struct dns_query qry;
  struct pbuf *p = NULL;
  u16_t query_idx;
  u16_t copy_len;
  const char *encoded_domain = NULL;
  const char *encoded_domain_part = NULL;
  u8_t n;
  u8_t pcb_idx;
  struct reverse_dns_table_entry *entry = &reverse_dns_table[idx];
  const ip_addr_t *dst = NULL;
  ip_addr_t dns_server_addr;
  u16_t dst_port;

  LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_send: dns_servers[%"U16_F"] \"%s\": request\n",
              (u16_t)(entry->server_idx), entry->encoded_ip.name));

  LWIP_ASSERT("dns server out of array", entry->server_idx < DNS_MAX_SERVERS);

  if (!DNS_SERVER_IS_AVAILABLE(entry, entry->server_idx)
#if LWIP_DNS_SUPPORT_MDNS_QUERIES
      && !entry->is_mdns
#endif /* LWIP_DNS_SUPPORT_MDNS_QUERIES */
    ) {
    /* DNS server not valid anymore, e.g. PPP netif has been shut down */
    /* call specified callback function if provided */
    reverse_dns_call_found(idx, NULL, 0);
    /* flush this entry */
    entry->retries = DNS_MAX_RETRIES - 1;
    return ERR_OK;
  }

  /* if here, we have either a new query or a retry on a previous query to process */
  p = pbuf_alloc(PBUF_TRANSPORT,
                 (u16_t)(SIZEOF_DNS_HDR + strlen((char *)(entry->encoded_ip.name)) +
                 DNS_TYPE_LENGTH + SIZEOF_DNS_QUERY), PBUF_RAM);
  if (p == NULL) {
    return ERR_MEM;
  }

  /* fill dns header */
  memset(&hdr, 0, SIZEOF_DNS_HDR);
  hdr.id = lwip_htons(entry->txid);
  hdr.flags1 = DNS_FLAG1_RD;
  hdr.numquestions = PP_HTONS(1);
  (void)pbuf_take(p, &hdr, SIZEOF_DNS_HDR);
  encoded_domain = (char *)(entry->encoded_ip.name);
  --encoded_domain;

  /* convert hostname into suitable query format. */
  query_idx = SIZEOF_DNS_HDR;
  do {
    ++encoded_domain;
    encoded_domain_part = encoded_domain;
    for (n = 0; (*encoded_domain != '.') && (*encoded_domain != 0); ++encoded_domain, ++n) {
    }
    copy_len = (u16_t)(encoded_domain - encoded_domain_part);
    pbuf_put_at(p, query_idx, n);
    (void)pbuf_take_at(p, encoded_domain_part, copy_len, (u16_t)(query_idx + 1));
    query_idx = (u16_t)(query_idx + n + 1);
  } while (*encoded_domain != 0);
  pbuf_put_at(p, query_idx, 0);
  query_idx++;

  /* fill dns query */
  qry.type = PP_HTONS(DNS_RRTYPE_PTR);
  qry.cls = PP_HTONS(DNS_RRCLASS_IN);
  (void)pbuf_take_at(p, &qry, SIZEOF_DNS_QUERY, query_idx);

#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0)
  pcb_idx = entry->pcb_idx;
#else
  pcb_idx = 0;
#endif /* (LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0 */

  /* send dns packet */
  LWIP_DEBUGF(DNS_DEBUG, ("sending Reverse DNS request ID %d for name \"%s\" to server %d\r\n",
              entry->txid, entry->encoded_ip.name, entry->server_idx));
#if LWIP_DNS_SUPPORT_MDNS_QUERIES
  if (entry->is_mdns) {
    dst_port = DNS_MQUERY_PORT;
  } else
#endif /* LWIP_DNS_SUPPORT_MDNS_QUERIES */
  {
    dst_port = DNS_SERVER_PORT;
    ip_addr_copy(dns_server_addr, dns_servers[entry->server_idx]);

#if LWIP_IPV4 && LWIP_IPV6
    /* Dual-stack: Unmap IPv4 mapped IPv6 addresses */
    if (IP_IS_V6_VAL(dns_server_addr) && ip6_addr_isipv4mappedipv6(ip_2_ip6(&dns_server_addr))) {
      unmap_ipv4_mapped_ipv6(ip_2_ip4(&dns_server_addr), ip_2_ip6(&dns_server_addr));
      IP_SET_TYPE_VAL(dns_server_addr, IPADDR_TYPE_V4);
    }
#endif /* LWIP_IPV4 && LWIP_IPV6 */
    dst = &dns_server_addr;
  }
  err = udp_sendto(dns_pcbs[pcb_idx], p, dst, dst_port);

  /* free pbuf */
  (void)pbuf_free(p);

  return err;
}
#endif /* LWIP_DNS_REVERSE */

#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0)
static struct udp_pcb *
dns_alloc_random_port(void)
{
  err_t err;
  struct udp_pcb *pcb;

  pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
  if (pcb == NULL) {
    /* out of memory, have to reuse an existing pcb */
    return NULL;
  }
  do {
    u16_t port = (u16_t)DNS_RAND_TXID();
    if (DNS_PORT_ALLOWED(port)) {
      err = udp_bind(pcb, IP_ANY_TYPE, port);
    } else {
      /* this port is not allowed, try again */
      err = ERR_USE;
    }
  } while (err == ERR_USE);
  if ((err != ERR_OK) && (err != ERR_USE)) {
    udp_remove(pcb);
    return NULL;
  }
#if LWIP_SO_PRIORITY
  pcb->priority = LWIP_PKT_PRIORITY_DNS;
#endif /* LWIP_SO_PRIORITY */
  udp_recv(pcb, dns_recv, NULL);
  return pcb;
}

/**
 * dns_alloc_pcb() - allocates a new pcb (or reuses an existing one) to be used
 * for sending a request
 *
 * @return an index into dns_pcbs
 */
static u8_t
dns_alloc_pcb(void)
{
  u8_t i;
  u8_t idx;

  for (i = 0; i < DNS_MAX_SOURCE_PORTS; i++) {
    if (dns_pcbs[i] == NULL) {
      break;
    }
  }
  if (i < DNS_MAX_SOURCE_PORTS) {
    dns_pcbs[i] = dns_alloc_random_port();
    if (dns_pcbs[i] != NULL) {
      /* succeeded */
      dns_last_pcb_idx = i;
      return i;
    }
  }
  /* if we come here, creating a new UDP pcb failed, so we have to use
     an already existing one (so overflow is no issue) */
  idx = (u8_t)(dns_last_pcb_idx + 1);
  for (i = 0;  i < DNS_MAX_SOURCE_PORTS; i++, idx++) {
    if (idx >= DNS_MAX_SOURCE_PORTS) {
      idx = 0;
    }
    if (dns_pcbs[idx] != NULL) {
      dns_last_pcb_idx = idx;
      return idx;
    }
  }
  return DNS_MAX_SOURCE_PORTS;
}
#endif /* ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0) */

/**
 * dns_call_found() - call the found callback and check if there are duplicate
 * entries for the given hostname. If there are any, their found callback will
 * be called and they will be removed.
 *
 * @param idx dns table index of the entry that is resolved or removed
 * @param addr IP address for the hostname (or NULL on error or memory shortage)
 * @param ipaddr_count number of resolved IP address for hostname (or 0 on error or memory shortage)
 *        If addr = NULL, then count will contain the error code which will be set to h_errno
 *        If count contains error code, then it will be reset to zero after it is set to h_errno
 */
static void
dns_call_found(u8_t idx, const ip_addr_t *addr, u32_t ipaddr_count)
{
#if ((LWIP_DNS_SECURE & (LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT)) != 0)
  u32_t i;
#endif /* (LWIP_DNS_SECURE & (LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT)) != 0 */

#if LWIP_IPV4 && LWIP_IPV6
  if (addr != NULL) {
    /* check that address type matches the request and adapt the table entry */
    for (i = 0; i < ipaddr_count; ++i) {
      if (IP_IS_V6_VAL(addr[i])) {
        LWIP_ASSERT("invalid response", LWIP_DNS_ADDRTYPE_IS_IPV6(dns_table[idx].reqaddrtype));
        dns_table[idx].reqaddrtype = LWIP_DNS_ADDRTYPE_IPV6;
      } else {
        LWIP_ASSERT("invalid response", !LWIP_DNS_ADDRTYPE_IS_IPV6(dns_table[idx].reqaddrtype));
        dns_table[idx].reqaddrtype = LWIP_DNS_ADDRTYPE_IPV4;
      }
    }
  }
#endif /* LWIP_IPV4 && LWIP_IPV6 */

#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0)
  for (i = 0; i < DNS_MAX_REQUESTS; i++) {
    if (dns_requests[i].found && (dns_requests[i].dns_table_idx == idx)) {
      (*dns_requests[i].found)(dns_table[idx].name, addr, ipaddr_count, dns_requests[i].arg);
      /* flush this entry */
      dns_requests[i].found = NULL;
    }
  }
#else
  if (dns_requests[idx].found) {
    (*dns_requests[idx].found)(dns_table[idx].name, addr, ipaddr_count, dns_requests[idx].arg);
  }
  dns_requests[idx].found = NULL;
#endif /* (LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0 */
#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0)
  /* close the pcb used unless other request are using it */
  for (i = 0; i < DNS_MAX_REQUESTS; i++) {
    if (i == idx) {
      continue; /* only check other requests */
    }
    if (dns_table[i].state == DNS_STATE_ASKING) {
      if (dns_table[i].pcb_idx == dns_table[idx].pcb_idx) {
        /* another request is still using the same pcb */
        dns_table[idx].pcb_idx = DNS_MAX_SOURCE_PORTS;
        break;
      }
    }
  }
  if (dns_table[idx].pcb_idx < DNS_MAX_SOURCE_PORTS) {
    /* if we come here, the pcb is not used any more and can be removed */
    udp_remove(dns_pcbs[dns_table[idx].pcb_idx]);
    dns_pcbs[dns_table[idx].pcb_idx] = NULL;
    dns_table[idx].pcb_idx = DNS_MAX_SOURCE_PORTS;
  }
#endif /* (LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0 */
}

#if LWIP_DNS_REVERSE
static void
reverse_dns_call_found(u8_t idx, const char *hostname, u32_t name_count)
{
#if ((LWIP_DNS_SECURE & (LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT)) != 0)
  u8_t i;
#endif /* (LWIP_DNS_SECURE & (LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT)) != 0 */

#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0)
  for (i = 0; i < DNS_MAX_REQUESTS; i++) {
    if (reverse_dns_requests[i].found && (reverse_dns_requests[i].reverse_dns_table_idx == idx)) {
      (*reverse_dns_requests[i].found)(hostname, name_count, reverse_dns_requests[i].arg);
      /* flush this entry */
      reverse_dns_requests[i].found = NULL;
    }
  }
#else
  if (reverse_dns_requests[idx].found) {
    (*reverse_dns_requests[idx].found)(hostname, name_count, reverse_dns_requests[idx].arg);
  }
  reverse_dns_requests[idx].found = NULL;
#endif /* (LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0 */

#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0)
  /* close the pcb used unless other request are using it */
  for (i = 0; i < DNS_MAX_REQUESTS; i++) {
    if (i == idx) {
      continue; /* only check other requests */
    }
    if (reverse_dns_table[i].state == DNS_STATE_ASKING) {
      if (reverse_dns_table[i].pcb_idx == reverse_dns_table[idx].pcb_idx) {
        /* another request is still using the same pcb */
        reverse_dns_table[idx].pcb_idx = DNS_MAX_SOURCE_PORTS;
        break;
      }
    }
  }
  if (reverse_dns_table[idx].pcb_idx < DNS_MAX_SOURCE_PORTS) {
    /* if we come here, the pcb is not used any more and can be removed */
    udp_remove(dns_pcbs[reverse_dns_table[idx].pcb_idx]);
    dns_pcbs[reverse_dns_table[idx].pcb_idx] = NULL;
    reverse_dns_table[idx].pcb_idx = DNS_MAX_SOURCE_PORTS;
  }
#endif /* (LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0 */
}
#endif /* LWIP_DNS_REVERSE */

/* Create a query transmission ID that is unique for all outstanding queries */
static u16_t
dns_create_txid(void)
{
  u16_t txid;
  u8_t i;

again:
  txid = (u16_t)DNS_RAND_TXID();

  /* check whether the ID is unique */
  for (i = 0; i < DNS_TABLE_SIZE; i++) {
    if ((dns_table[i].state == DNS_STATE_ASKING) &&
        (dns_table[i].txid == txid)) {
      /* ID already used by another pending query */
      goto again;
    }
  }

  return txid;
}

/* Search for the first DNS server in server list */
static void
dns_find_first_server(void)
{
  u8_t i;

  for (i = 0; i < DNS_MAX_SERVERS; i++) {
    if (!ip_addr_isany(&dns_servers[i])) {
      dns_first_server_idx = i;
#if LWIP_DNS_REVERSE
      rdns_first_server_idx = i;
#endif /* LWIP_DNS_REVERSE */
      return;
    }
  }
  /* if no valid dns server, should be processed in dns_gethostbyname and reverse_dns_getnamebyhost */
  /* server will be checked before send requests, so it's fine to return 0 */
  return;
}

/**
 * dns_check_entry() - see if entry has not yet been queried and, if so, sends out a query.
 * Check an entry in the dns_table:
 * - send out query for new entries
 * - retry old pending entries on timeout (also with different servers)
 * - remove completed entries from the table if their TTL has expired
 *
 * @param i index of the dns_table entry to check
 */
static err_t
dns_check_entry(u8_t i)
{
  err_t err = ERR_OK;
  struct dns_table_entry *entry = NULL;
  LWIP_ASSERT("array index out of bounds", i < DNS_TABLE_SIZE);

  entry = &dns_table[i];

  switch (entry->state) {
    case DNS_STATE_NEW:
      /* initialize new entry */
      entry->txid = dns_create_txid();
      entry->state = DNS_STATE_ASKING;
      entry->server_idx = dns_first_server_idx;
      entry->server_bitmap = 0;
      entry->tmr = 1;
      entry->retries = 0;
      entry->first_send_time = sys_now();

      /* send DNS packet for this entry */
      err = dns_send(i);
      if (err != ERR_OK) {
        LWIP_DEBUGF(DNS_DEBUG | LWIP_DBG_LEVEL_WARNING,
                    ("dns_send returned error: %s\n", lwip_strerr(err)));
      }
      break;
    case DNS_STATE_ASKING:
      /* To ensure that the time gap between the force send routine and periodic send routine is atleast 100ms */
      if (sys_now() - (entry->first_send_time) <= 100) {
        /* If the gap is less than 100ms, then retransmit the DNS Query in the next tick */
        break;
      }
      if (--entry->tmr == 0) {
        if (dns_backupserver_available(entry, DNS_SERVER_SEARCHING_FLAG_SWITCH) == 0) {
          /* reset timer for next retry, linearly increasing as retries in each round */
          entry->tmr = entry->retries + 1;

          /* send DNS packet for this entry */
          err = dns_send(i);
          if (err != ERR_OK) {
            LWIP_DEBUGF(DNS_DEBUG | LWIP_DBG_LEVEL_WARNING,
                        ("dns_send returned error: %s\n", lwip_strerr(err)));
          }
        } else {
          /* no available DNS server found */
          LWIP_DEBUGF(DNS_DEBUG, ("dns_check_entry: \"%s\": timeout\n", entry->name));
          /* call specified callback function if provided */
          dns_call_found(i, NULL, TRY_AGAIN);
          /* flush this entry */
          entry->state = DNS_STATE_UNUSED;
          break;
        }
      }
      break;
    case DNS_STATE_DONE:
      /* if the time to live is nul */
      if ((entry->ttl == 0) || (--entry->ttl == 0)) {
        LWIP_DEBUGF(DNS_DEBUG, ("dns_check_entry: \"%s\": flush\n", entry->name));
        /* flush this entry, there cannot be any related pending entries in this state */
        entry->state = DNS_STATE_UNUSED;
      }
      break;
    case DNS_STATE_UNUSED:
      /* nothing to do */
      break;
    default:
      LWIP_ASSERT("unknown dns_table entry state:", 0);
      break;
  }

  return err;
}

#if LWIP_DNS_REVERSE
static err_t
reverse_dns_check_entry(u8_t i)
{
  err_t err = ERR_OK;
  struct reverse_dns_table_entry *entry = NULL;

  LWIP_ASSERT("reverse_dns_check_entry: array index out of bounds\n", i < DNS_TABLE_SIZE);

  entry = &reverse_dns_table[i];

  switch (entry->state) {
    case DNS_STATE_NEW:
      /* initialize new entry */
      entry->txid = dns_create_txid();
      entry->state = DNS_STATE_ASKING;
      entry->server_idx = rdns_first_server_idx;
      entry->server_bitmap = 0;
      entry->tmr = 1;
      entry->retries = 0;
      entry->first_send_time = sys_now();

      /* send DNS packet for this entry */
      err = reverse_dns_send(i);
      if (err != ERR_OK) {
        LWIP_DEBUGF(DNS_DEBUG | LWIP_DBG_LEVEL_WARNING, ("reverse_dns_send returned error: %s\n", lwip_strerr(err)));
      }
      break;
    case DNS_STATE_ASKING:
      /* To ensure that the time gap between the force send routine and periodic send routine is atleast 100ms */
      if (sys_now() - (entry->first_send_time) <= 100) {
        /* If the gap is less than 100ms, then retransmit the DNS Query in the next tick */
        break;
      }
      if (--entry->tmr == 0) {
        if (reverse_dns_backupserver_available(entry, DNS_SERVER_SEARCHING_FLAG_SWITCH) == 0) {
          /* reset timer for next retry, linearly increasing as retries in each round */
          entry->tmr = entry->retries + 1;

          /* send DNS packet for this entry */
          err = reverse_dns_send(i);
          if (err != ERR_OK) {
            LWIP_DEBUGF(DNS_DEBUG | LWIP_DBG_LEVEL_WARNING,
                        ("reverse_dns_send returned error: %s\n", lwip_strerr(err)));
          }
        } else {
          /* no available DNS server found */
          LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_check_entry: \"%s\": timeout\n", entry->encoded_ip.name));
          /* call specified callback function if provided */
          reverse_dns_call_found(i, NULL, TRY_AGAIN);
          /* flush this entry */
          entry->state = DNS_STATE_UNUSED;
          break;
        }
      }
      break;
    case DNS_STATE_DONE:
      /* if the time to live is null */
      if ((entry->ttl == 0) || (--entry->ttl == 0)) {
        LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_check_entry: \"%s\": flush\n", entry->encoded_ip.name));
        /* flush this entry, there cannot be any related pending entries in this state */
        entry->state = DNS_STATE_UNUSED;
      }
      break;
    case DNS_STATE_UNUSED:
      /* nothing to do */
      break;
    default:
      LWIP_ASSERT("unknown reverse_dns_table entry state:", 0);
      break;
  }

  return err;
}
#endif /* LWIP_DNS_REVERSE */

/**
 * Call dns_check_entry for each entry in dns_table - check all entries.
 */
static void
dns_check_entries(void)
{
  u8_t i;

  for (i = 0; i < DNS_TABLE_SIZE; ++i) {
    (void)dns_check_entry(i);
  }
}

#if LWIP_DNS_REVERSE
/**
 * Call dns_check_entry for each entry in dns_table - check all entries.
 */
static void
reverse_dns_check_entries(void)
{
  u8_t i;

  for (i = 0; i < DNS_TABLE_SIZE; ++i) {
    (void)reverse_dns_check_entry(i);
  }
}
#endif /* LWIP_DNS_REVERSE */

/**
 * Save TTL and call dns_call_found for correct response.
 */
static void
dns_correct_response(u8_t idx, u32_t ttl)
{
  struct dns_table_entry *entry = &dns_table[idx];
  entry->state = DNS_STATE_DONE;

  LWIP_DEBUGF(DNS_DEBUG, ("dns_recv:  response = \"%s\"\n", entry->name));

#ifdef LWIP_DEV_DEBUG
  u32_t j;
  for (j = 0; j < entry->ipaddr_count; j++) {
    ip_addr_debug_print_val(DNS_DEBUG, entry->ipaddr[j]);
    LWIP_DEBUGF(DNS_DEBUG, ("\n"));
  }
#endif /* LWIP_DEV_DEBUG */

  /* read the answer resource record's TTL, and maximize it if needed */
  if (ttl > DNS_MAX_TTL) {
    ttl = DNS_MAX_TTL;
  }
  entry->ttl = ttl;

  /* consider this server as the 'main' fDNS server, for query, try this first */
  if (dns_first_server_idx != entry->server_idx) {
    dns_first_server_idx = entry->server_idx;
  }

  dns_call_found(idx, entry->ipaddr, entry->ipaddr_count);

  if (entry->ttl == 0) {
    /* RFC 883, page 29: "Zero values are
       interpreted to mean that the RR can only be used for the
       transaction in progress, and should not be cached."
       -> flush this entry now */
    /* entry reused during callback? */
    if (entry->state == DNS_STATE_DONE) {
      entry->state = DNS_STATE_UNUSED;
    }
  }
}

#if LWIP_DNS_REVERSE
/**
 * Save TTL and call reverse_dns_call_found for correct response.
 */
static void
reverse_dns_correct_response(u8_t idx, u32_t ttl)
{
  struct reverse_dns_table_entry *entry = &reverse_dns_table[idx];
  entry->state = DNS_STATE_DONE;

  LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_correct_response: response = \"%s\"\n", entry->name));

  /* read the answer resource record's TTL, and maximize it if needed */
  if (ttl > DNS_MAX_TTL) {
    ttl = DNS_MAX_TTL;
  }
  entry->ttl = ttl;

  /* consider this server as the 'main' rDNS server, for next query, try this first */
  if (rdns_first_server_idx != entry->server_idx) {
    rdns_first_server_idx = entry->server_idx;
  }

  reverse_dns_call_found(idx, entry->name, entry->name_count);

  if (entry->ttl == 0) {
    /* RFC 883, page 29: "Zero values are
       interpreted to mean that the RR can only be used for the
       transaction in progress, and should not be cached."
       -> flush this entry now */
    /* entry reused during callback? */
    if (entry->state == DNS_STATE_DONE) {
      entry->state = DNS_STATE_UNUSED;
    }
  }
}
#endif /* LWIP_DNS_REVERSE */

/*
 * Check whether there are other backup DNS servers available to try
 * @param
 *  - pentry: the forward dns_table entry to check.
 *  - flags: Only support DNS_SERVER_SEARCHING_FLAG_SWITCH for now.
 *           If DNS_SERVER_SEARCHING_FLAG_SWITCH set, switch to an available DNS server when searching.
 *           Otherwise, only seach for an available DNS server without switching to it.
 * @Return value:
 *  - 0: OK
 *  - 1: NOK
 */
static u8_t
dns_backupserver_available(struct dns_table_entry *pentry, u8_t flags)
{
  u8_t idx, retries;
  u8_t j;

  if (pentry != NULL) {
    idx = pentry->server_idx;
    retries = pentry->retries;

    for (j = 0; j < DNS_MAX_SERVERS; j++) {
      idx = (idx + 1) % DNS_MAX_SERVERS;
      if (idx == dns_first_server_idx) {
        /* for the next round, increase retry times */
        retries++;
      }
      if (retries >= DNS_MAX_RETRIES) {
        /* return NOK if greater than limited retry times */
        return 1;
      }

      if (DNS_SERVER_IS_AVAILABLE(pentry, idx)
#if LWIP_DNS_SUPPORT_MDNS_QUERIES
          && !pentry->is_mdns
#endif /* LWIP_DNS_SUPPORT_MDNS_QUERIES */
        ) {
        if ((flags & DNS_SERVER_SEARCHING_FLAG_SWITCH) != 0) {
          /* if here, we need to update the information for this entry.
          in other failure cases, no need to do this because the request will stop. */
          pentry->server_idx = idx;
          pentry->retries = retries;
        }
        return 0;
      }
    }
  }
  return 1;
}

#if LWIP_DNS_REVERSE
/*
 * Check whether there are other backup reverse DNS servers available to try
 * @param
 *  - rentry: the reverse dns_table entry to check.
 *  - flags: Only support DNS_SERVER_SEARCHING_FLAG_SWITCH for now.
 *           If DNS_SERVER_SEARCHING_FLAG_SWITCH set, switch to an available DNS server when searching.
 *           Otherwise, only seach for an available DNS server without switching to it.
 * @Return value:
 *  - 0: OK
 *  - 1: NOK
 */
static u8_t
reverse_dns_backupserver_available(struct reverse_dns_table_entry *rentry, u8_t flags)
{
  u8_t idx, retries;
  u8_t j;

  if (rentry != NULL) {
    idx = rentry->server_idx;
    retries = rentry->retries;

    for (j = 0; j < DNS_MAX_SERVERS; j++) {
      idx = (idx + 1) % DNS_MAX_SERVERS;
      if (idx == rdns_first_server_idx) {
        /* for the next round, increase retry times */
        retries++;
      }
      if (retries >= DNS_MAX_RETRIES) {
        /* return NOK if greater than limited retry times */
        return 1;
      }

      if (DNS_SERVER_IS_AVAILABLE(rentry, idx)
#if LWIP_DNS_SUPPORT_MDNS_QUERIES
          && !rentry->is_mdns
#endif /* LWIP_DNS_SUPPORT_MDNS_QUERIES */
        ) {
        if ((flags & DNS_SERVER_SEARCHING_FLAG_SWITCH) != 0) {
          /* if here, we need to update the information for this entry.
          in other failure cases, no need to do this because the request will stop. */
          rentry->server_idx = idx;
          rentry->retries = retries;
        }
        return 0;
      }
    }
  }
  return 1;
}
#endif /* LWIP_DNS_REVERSE */

/**
 * Receive input function for DNS response packets arriving for the dns UDP pcb.
 * It multiplexes to process_forward_dns() and process_reverse_dns() based on the response txn id
 */
static void
dns_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  u8_t i;
  u16_t txid;
  struct dns_hdr hdr;
  struct dns_table_entry *forward_entry = NULL;
#if LWIP_DNS_REVERSE
  struct reverse_dns_table_entry *reverse_entry = NULL;
#endif /* LWIP_DNS_REVERSE */

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(pcb);
  LWIP_UNUSED_ARG(port);

  (void)memset_s(&hdr, sizeof(hdr), 0, sizeof(hdr));

  /* is the dns message big enough ? */
  if (p->tot_len < (SIZEOF_DNS_HDR + SIZEOF_DNS_QUERY)) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: pbuf too small\n"));
    /* free pbuf and return */
    goto ignore_packet;
  }

  /* copy dns payload inside static buffer for processing */
  if (pbuf_copy_partial(p, &hdr, SIZEOF_DNS_HDR, 0) == SIZEOF_DNS_HDR) {
    /* Match the ID in the DNS header with the name table. */
    txid = lwip_htons(hdr.id);
    for (i = 0; i < DNS_TABLE_SIZE; i++) {
      forward_entry = &dns_table[i];
#if LWIP_DNS_REVERSE
      reverse_entry = &reverse_dns_table[i];
#endif /* LWIP_DNS_REVERSE */
      if ((forward_entry->state == DNS_STATE_ASKING) && (forward_entry->txid == txid)) {
        process_forward_dns(p, i, forward_entry, hdr, addr);
        break;
      }
#if LWIP_DNS_REVERSE
      else if ((reverse_entry->state == DNS_STATE_ASKING) && (reverse_entry->txid == txid)) {
        process_reverse_dns(p, i, reverse_entry, hdr, addr);
        break;
      }
#endif /* LWIP_DNS_REVERSE */
    }
  }

ignore_packet:
  /* deallocate memory and return */
  pbuf_free(p);
  return;
}

/**
 * Queues a new hostname to resolve and sends out a DNS query for that hostname
 *
 * @param name the hostname that is to be queried
 * @param hostnamelen length of the hostname
 * @param found a callback function to be called on success, failure or timeout
 * @param callback_arg argument to pass to the callback function
 * @return err_t return code.
 */
static err_t
dns_enqueue(const char *name, size_t hostnamelen, dns_found_callback found,
            void *callback_arg, u8_t dns_addrtype LWIP_DNS_ISMDNS_ARG(u8_t is_mdns))
{
  u8_t i;
  u8_t lseq, lseqi;
  struct dns_table_entry *entry = NULL;
  size_t namelen;
  struct dns_req_entry *req;
  err_t err;

#if !LWIP_IPV6
  LWIP_UNUSED_ARG(dns_addrtype);
#endif

#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0)
  u8_t r;
  /* check for duplicate entries */
  for (i = 0; i < DNS_TABLE_SIZE; i++) {
    if ((dns_table[i].state == DNS_STATE_ASKING) &&
        (lwip_strnicmp(name, dns_table[i].name, sizeof(dns_table[i].name)) == 0)) {
#if LWIP_IPV4 && LWIP_IPV6
      if (dns_table[i].reqaddrtype != dns_addrtype) {
        /* requested address types don't match
           this can lead to 2 concurrent requests, but mixing the address types
           for the same host should not be that common */
        continue;
      }
#endif /* LWIP_IPV4 && LWIP_IPV6 */
      /* this is a duplicate entry, find a free request entry */
      for (r = 0; r < DNS_MAX_REQUESTS; r++) {
        if (dns_requests[r].found == 0) {
          dns_requests[r].found = found;
          dns_requests[r].arg = callback_arg;
          dns_requests[r].dns_table_idx = i;
          LWIP_DNS_SET_ADDRTYPE(dns_requests[r].reqaddrtype, dns_addrtype);
          LWIP_DEBUGF(DNS_DEBUG, ("dns_enqueue: \"%s\": duplicate request\n", name));
          return ERR_INPROGRESS;
        }
      }
    }
  }
  /* no duplicate entries found */
#endif

  /* search an unused entry, or the oldest one */
  lseq = 0;
  lseqi = DNS_TABLE_SIZE;
  for (i = 0; i < DNS_TABLE_SIZE; ++i) {
    entry = &dns_table[i];
    /* is it an unused entry ? */
    if (entry->state == DNS_STATE_UNUSED) {
      break;
    }
    /* check if this is the oldest completed entry */
    if (entry->state == DNS_STATE_DONE) {
      u8_t age = (u8_t)(dns_seqno - entry->seqno);
      if (age > lseq) {
        lseq = age;
        lseqi = i;
      }
    }
  }

  /* if we don't have found an unused entry, use the oldest completed one */
  if (i == DNS_TABLE_SIZE) {
    if ((lseqi >= DNS_TABLE_SIZE) || (dns_table[lseqi].state != DNS_STATE_DONE)) {
      /* no entry can be used now, table is full */
      LWIP_DEBUGF(DNS_DEBUG, ("dns_enqueue: \"%s\": DNS entries table is full\n", name));
      return ERR_MEM;
    } else {
      /* use the oldest completed one */
      i = lseqi;
      entry = &dns_table[i];
    }
  }

#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0)
  /* find a free request entry */
  req = NULL;
  for (r = 0; r < DNS_MAX_REQUESTS; r++) {
    if (dns_requests[r].found == NULL) {
      req = &dns_requests[r];
      break;
    }
  }
  if (req == NULL) {
    /* no request entry can be used now, table is full */
    LWIP_DEBUGF(DNS_DEBUG, ("dns_enqueue: \"%s\": DNS request entries table is full\n", name));
    return ERR_MEM;
  }
  req->dns_table_idx = i;
#else
  /* in this configuration, the entry index is the same as the request index */
  req = &dns_requests[i];
#endif

  /* use this entry */
  LWIP_DEBUGF(DNS_DEBUG, ("dns_enqueue: \"%s\": use DNS entry %"U16_F"\n", name, (u16_t)(i)));

  /* fill the entry */
  entry->state = DNS_STATE_NEW;
  entry->seqno = dns_seqno;
  entry->ipaddr_count = 0;
  LWIP_DNS_SET_ADDRTYPE(entry->reqaddrtype, dns_addrtype);
  LWIP_DNS_SET_ADDRTYPE(req->reqaddrtype, dns_addrtype);
  req->found = found;
  req->arg   = callback_arg;
  namelen = LWIP_MIN(hostnamelen, DNS_MAX_NAME_LENGTH - 1);
  MEMCPY(entry->name, name, namelen);
  entry->name[namelen] = 0;

#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0)
  entry->pcb_idx = dns_alloc_pcb();
  if (entry->pcb_idx >= DNS_MAX_SOURCE_PORTS) {
    /* failed to get a UDP pcb */
    LWIP_DEBUGF(DNS_DEBUG, ("dns_enqueue: \"%s\": failed to allocate a pcb\n", name));
    entry->state = DNS_STATE_UNUSED;
    req->found = NULL;
    return ERR_MEM;
  }
  LWIP_DEBUGF(DNS_DEBUG, ("dns_enqueue: \"%s\": use DNS pcb %"U16_F"\n", name, (u16_t)(entry->pcb_idx)));
#endif

#if LWIP_DNS_SUPPORT_MDNS_QUERIES
  entry->is_mdns = is_mdns;
#endif

  dns_seqno++;

  /* force to send query without waiting timer */
  err = dns_check_entry(i);

  /* dns query is enqueued */
  if (err == ERR_OK) {
    return ERR_INPROGRESS;
  } else {
    entry->state = DNS_STATE_UNUSED;
    req->found = NULL;
    return err;
  }
}

#if LWIP_DNS_REVERSE
static err_t
reverse_dns_enqueue(const struct reverse_domain *encoded_ip, reverse_dns_found_callback found, void *callback_arg)
{
  u8_t i;
  u8_t lseq, lseqi;
  struct reverse_dns_table_entry *entry = NULL;
  struct reverse_dns_req_entry *req = NULL;
  err_t err;

#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0)
  u8_t r;
  /* check for duplicate entries */
  for (i = 0; i < DNS_TABLE_SIZE; i++) {
    if ((reverse_dns_table[i].state == DNS_STATE_ASKING) &&
        (lwip_strnicmp((const char*)(encoded_ip->name), (const char*)(reverse_dns_table[i].encoded_ip.name),
                       sizeof(reverse_dns_table[i].encoded_ip.name)) == 0)) {
      /* this is a duplicate entry, find a free request entry */
      for (r = 0; r < DNS_MAX_REQUESTS; r++) {
        if (reverse_dns_requests[r].found == NULL) {
          reverse_dns_requests[r].found = found;
          reverse_dns_requests[r].arg = callback_arg;
          reverse_dns_requests[r].reverse_dns_table_idx = i;
          LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_enqueue: \"%s\": duplicate request\n", encoded_ip->name));
          return ERR_INPROGRESS;
        }
      }
    }
  }
  /* no duplicate entries found */
#endif /* (LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0 */

  /* search an unused entry, or the oldest one */
  lseq = 0;
  lseqi = DNS_TABLE_SIZE;
  for (i = 0; i < DNS_TABLE_SIZE; ++i) {
    entry = &reverse_dns_table[i];
    /* is it an unused entry ? */
    if (entry->state == DNS_STATE_UNUSED) {
      break;
    }
    /* check if this is the oldest completed entry */
    if (entry->state == DNS_STATE_DONE) {
      u8_t age = (u8_t)(dns_seqno - entry->seqno);
      if (age > lseq) {
        lseq = age;
        lseqi = i;
      }
    }
  }

  /* if we don't have found an unused entry, use the oldest completed one */
  if (i == DNS_TABLE_SIZE) {
    if ((lseqi >= DNS_TABLE_SIZE) || (reverse_dns_table[lseqi].state != DNS_STATE_DONE)) {
      /* no entry can be used now, table is full */
      LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_enqueue: \"%s\": Reverse DNS entries table is full\n", encoded_ip->name));
      return ERR_MEM;
    } else {
      /* use the oldest completed one */
      i = lseqi;
      entry = &reverse_dns_table[i];
    }
  }

#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0)
  /* find a free request entry */
  req = NULL;
  for (r = 0; r < DNS_MAX_REQUESTS; r++) {
    if (reverse_dns_requests[r].found == NULL) {
      req = &reverse_dns_requests[r];
      break;
    }
  }
  if (req == NULL) {
    /* no request entry can be used now, table is full */
    LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_enqueue: \"%s\": DNS request entries table is full\n", encoded_ip->name));
    return ERR_MEM;
  }
  req->reverse_dns_table_idx = i;
#else
  /* in this configuration, the entry index is the same as the request index */
  req = &reverse_dns_requests[i];
#endif /* (LWIP_DNS_SECURE & LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING) != 0 */

  /* use this entry */
  LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_enqueue: \"%s\": use DNS entry %"U16_F"\n", encoded_ip->name, (u16_t)(i)));

  /* fill the entry */
  entry->state = DNS_STATE_NEW;
  entry->seqno = dns_seqno;
  entry->encoded_ip = *encoded_ip;

  req->found = found;
  req->arg   = callback_arg;

#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0)
  entry->pcb_idx = dns_alloc_pcb();
  if (entry->pcb_idx >= DNS_MAX_SOURCE_PORTS) {
    /* failed to get a UDP pcb */
    LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_enqueue: \"%s\": failed to allocate a pcb\n", encoded_ip->name));
    entry->state = DNS_STATE_UNUSED;
    req->found = NULL;
    return ERR_MEM;
  }
  LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_enqueue: \"%s\": use DNS pcb %"U16_F"\n",
              encoded_ip->name, (u16_t)(entry->pcb_idx)));
#endif /* (LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0 */

#if LWIP_DNS_SUPPORT_MDNS_QUERIES
  entry->is_mdns = 0;
#endif /* LWIP_DNS_SUPPORT_MDNS_QUERIES */

  dns_seqno++;

  /* force to send query without waiting timer */
  err = reverse_dns_check_entry(i);
  /* dns query is enqueued */
  if (err == ERR_OK) {
    return ERR_INPROGRESS;
  } else {
    entry->state = DNS_STATE_UNUSED;
    req->found = NULL;
    return err;
  }
}
#endif /* LWIP_DNS_REVERSE */

/**
 * @ingroup dns
 * Resolve a hostname (string) into an IP address.
 * NON-BLOCKING callback version for use with raw API!!!
 *
 * Returns immediately with one of err_t return codes:
 * - ERR_OK if hostname is a valid IP address string or the host
 *   name is already in the local names table.
 * - ERR_INPROGRESS enqueue a request to be sent to the DNS server
 *   for resolution if no errors are present.
 * - ERR_ARG: dns client not initialized or invalid hostname
 *
 * @param hostname the hostname that is to be queried
 * @param addr pointer to a ip_addr_t where to store the address if it is already
 *             cached in the dns_table (only valid if ERR_OK is returned!)
 * @param count the number of resolved IP Address for hostname
 * @param found a callback function to be called on success, failure or timeout (only if
 *              ERR_INPROGRESS is returned!)
 * @param callback_arg argument to pass to the callback function
 * @return a err_t return code.
 */
err_t
dns_gethostbyname(const char *hostname, ip_addr_t *addr, u32_t *count, dns_found_callback found,
                  void *callback_arg)
{
  return dns_gethostbyname_addrtype(hostname, addr, count, found, callback_arg, LWIP_DNS_ADDRTYPE_DEFAULT);
}

/**
 * @ingroup dns
 * Like dns_gethostbyname, but returned address type can be controlled:
 * @param hostname the hostname that is to be queried
 * @param addr pointer to a ip_addr_t where to store the address if it is already
 *             cached in the dns_table (only valid if ERR_OK is returned!)
 * @param found a callback function to be called on success, failure or timeout (only if
 *              ERR_INPROGRESS is returned!)
 * @param callback_arg argument to pass to the callback function
 * @param dns_addrtype - LWIP_DNS_ADDRTYPE_IPV4_IPV6: try to resolve IPv4 first, try IPv6 if IPv4 fails only
 *                     - LWIP_DNS_ADDRTYPE_IPV6_IPV4: try to resolve IPv6 first, try IPv4 if IPv6 fails only
 *                     - LWIP_DNS_ADDRTYPE_IPV4: try to resolve IPv4 only
 *                     - LWIP_DNS_ADDRTYPE_IPV6: try to resolve IPv6 only
 */
err_t
dns_gethostbyname_addrtype(const char *hostname, ip_addr_t *addr, u32_t *count, dns_found_callback found,
                           void *callback_arg, u8_t dns_addrtype)
{
  size_t hostnamelen;
  int i = 0;
  int valid_dns_server = 0;
#if LWIP_DNS_SUPPORT_MDNS_QUERIES
  u8_t is_mdns;
#endif
  /* not initialized or no valid server yet, or invalid addr pointer
   * or invalid hostname or invalid hostname length */
  if ((addr == NULL) || (hostname == NULL) || (!hostname[0]) || (count == NULL) || (*count == 0)) {
    return ERR_ARG;
  }
#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) == 0)
  if (dns_pcbs[0] == NULL) {
    return ERR_ARG;
  }
#endif
  hostnamelen = strlen(hostname);
  if (hostnamelen >= DNS_MAX_NAME_LENGTH) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_gethostbyname: name too long to resolve"));
    return ERR_ARG;
  }

  /* host name already in octet notation? set ip addr and return ERR_OK */
  if (ipaddr_aton(hostname, &addr[0])) {
#if LWIP_IPV4 && LWIP_IPV6
    if ((IP_IS_V6(addr) && (dns_addrtype != LWIP_DNS_ADDRTYPE_IPV4)) ||
        (IP_IS_V4(addr) && (dns_addrtype != LWIP_DNS_ADDRTYPE_IPV6)))
#endif /* LWIP_IPV4 && LWIP_IPV6 */
    {
     *count = 1;
      return ERR_OK;
    }
  }

#if LWIP_HAVE_LOOPIF
  if (strcmp(hostname, "localhost") == 0) {
    ip_addr_set_loopback(LWIP_DNS_ADDRTYPE_IS_IPV6(dns_addrtype), &addr[0]);
    *count = 1;
    return ERR_OK;
  }
#endif /* LWIP_HAVE_LOOPIF */

  for (i = 0; i < DNS_MAX_SERVERS; i++) {
    if (!ip_addr_isany_val(dns_servers[i])) {
      valid_dns_server = 1;
      break;
    }
  }

  if (!valid_dns_server) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_gethostbyname: There is no valid DNS server"));
    return ERR_CONN;
  }

  /* already have this address cached? */
  if (dns_lookup(hostname, addr, count LWIP_DNS_ADDRTYPE_ARG(dns_addrtype)) == ERR_OK) {
    if (((IP_GET_TYPE(addr) == IPADDR_TYPE_V4) && (dns_addrtype == LWIP_DNS_ADDRTYPE_IPV6)) ||
        ((IP_GET_TYPE(addr) == IPADDR_TYPE_V6) && (dns_addrtype == LWIP_DNS_ADDRTYPE_IPV4))) {
      return ERR_VAL;
    }
    return ERR_OK;
  }
#if LWIP_IPV4 && LWIP_IPV6
  if ((dns_addrtype == LWIP_DNS_ADDRTYPE_IPV4_IPV6) || (dns_addrtype == LWIP_DNS_ADDRTYPE_IPV6_IPV4)) {
    /* fallback to 2nd IP type and try again to lookup */
    u8_t fallback;
    if (dns_addrtype == LWIP_DNS_ADDRTYPE_IPV4_IPV6) {
      fallback = LWIP_DNS_ADDRTYPE_IPV6;
    } else {
      fallback = LWIP_DNS_ADDRTYPE_IPV4;
    }
    if (dns_lookup(hostname, addr, count LWIP_DNS_ADDRTYPE_ARG(fallback)) == ERR_OK) {
      return ERR_OK;
    }
  }
#else /* LWIP_IPV4 && LWIP_IPV6 */
  LWIP_UNUSED_ARG(dns_addrtype);
#endif /* LWIP_IPV4 && LWIP_IPV6 */

#if LWIP_DNS_SUPPORT_MDNS_QUERIES
  if (strstr(hostname, ".local") == &hostname[hostnamelen] - 6) {
    is_mdns = 1;
  } else {
    is_mdns = 0;
  }
#endif /* LWIP_DNS_SUPPORT_MDNS_QUERIES */

  *count = 0;
  /* queue query with specified callback */
  return dns_enqueue(hostname, hostnamelen, found, callback_arg, dns_addrtype LWIP_DNS_ISMDNS_ARG(is_mdns));
}

#if LWIP_DNS_REVERSE
err_t
reverse_dns_getnamebyhost(const ip_addr_t *ipaddr, char *hostname, reverse_dns_found_callback found,
                          void *callback_arg)
{
  int i;
  err_t err = 0;
  int valid_dns_server = 0;
  struct reverse_domain encoded_ip;

  if ((ipaddr == NULL) || (hostname == NULL) || (found == NULL)) {
    LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_getnamebyhost: Invalid parameters\n"));
    return ERR_ARG;
  }

  if (!IP_IS_V4(ipaddr)
#if LWIP_IPV6
   && !IP_IS_V6(ipaddr)
#endif /* LWIP_IPV6 */
    ) {
    LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_getnamebyhost: ipaddr not of a valid type\n"));
    return ERR_ARG;
  }

  if (ip_addr_isloopback(ipaddr)) {
    if (strncpy_s(hostname, sizeof(LOCALHOST_STRING), LOCALHOST_STRING, sizeof(LOCALHOST_STRING)-1) != EOK) {
      return ERR_MEM;
    }
    return ERR_OK;
  }

  for (i = 0; i < DNS_MAX_SERVERS; i++) {
    if (!ip_addr_isany_val(dns_servers[i])) {
      valid_dns_server = 1;
      break;
    }
  }

  if (!valid_dns_server) {
    LWIP_DEBUGF(DNS_DEBUG, ("reverse_dns_getnamebyhost: There is no valid DNS server\n"));
    return ERR_CONN;
  }

  if (IP_IS_V4(ipaddr)) {
    err = reverse_dns_build_reverse_v4_domain(&encoded_ip, (const ip4_addr_t*)ip_2_ip4(ipaddr));
  }

#if LWIP_IPV6
  if (IP_IS_V6(ipaddr)) {
    err = reverse_dns_build_reverse_v6_domain(&encoded_ip, (const ip6_addr_t*)ip_2_ip6(ipaddr));
  }
#endif /* LWIP_IPV6 */

  if (err != ERR_OK) {
    LWIP_DEBUGF(DNS_DEBUG,
                ("reverse_dns_getnamebyhost: Error during conversion from IP-Address to Reverse Domain Name\n"));
    return ERR_VAL;
  }

  /* already have this IP cached? */
  if (reverse_dns_lookup(&encoded_ip, hostname) == ERR_OK) {
    return ERR_OK;
  }

  return reverse_dns_enqueue(&encoded_ip, found, callback_arg);
}
#endif /* LWIP_DNS_REVERSE */

static
void process_forward_dns(struct pbuf *p, u8_t entry_idx, struct dns_table_entry *entry, struct dns_hdr hdr,
                         const ip_addr_t *addr)
{
  struct dns_answer ans = {0};
  struct dns_query qry = {0};
  u8_t err_code = 0;
  u16_t res_idx;
  u16_t ret;
  u16_t nquestions, nanswers;
  ip_addr_t dns_server_addr;

  entry->ipaddr_count = 0;

  /* We only care about the question(s) and the answers. The authrr
      and the extrarr are simply discarded. */
  nquestions = lwip_htons(hdr.numquestions);
  nanswers   = lwip_htons(hdr.numanswers);

  /* Check for correct response. */
  if ((hdr.flags1 & DNS_FLAG1_RESPONSE) == 0) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": not a response\n", entry->name));
    goto forward_dns_responseerr;
  }

  if ((hdr.flags2 & DNS_FLAG2_ERR_MASK) != 0) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": error in flags\n", entry->name));

    if ((hdr.flags2 & DNS_FLAG2_ERR_SERVER_FAILURE) != 0) {
      LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": Some non-recoverable server failure occured\n", entry->name));
      err_code = NO_RECOVERY;
    } else if ((hdr.flags2 & DNS_FLAG2_ERR_NAME) != 0) {
      LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": No such host name\n", entry->name));
      err_code = HOST_NOT_FOUND;
    }

    goto forward_dns_responseerr;
  }

  if (nquestions != 1) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": response not match to query\n", entry->name));
    goto forward_dns_responseerr;
  }

#if LWIP_DNS_SUPPORT_MDNS_QUERIES
  if (!entry->is_mdns)
#endif /* LWIP_DNS_SUPPORT_MDNS_QUERIES */
  {
    /* Check whether response comes from the same network address to which the
       question was sent. (RFC 5452) */
    ip_addr_copy(dns_server_addr, dns_servers[entry->server_idx]);
#if LWIP_IPV4 && LWIP_IPV6
    /* Dual-stack: Unmap IPv4 mapped IPv6 addresses */
    if (IP_IS_V6_VAL(dns_server_addr) && ip6_addr_isipv4mappedipv6(ip_2_ip6(&dns_server_addr))) {
      unmap_ipv4_mapped_ipv6(ip_2_ip4(&dns_server_addr), ip_2_ip6(&dns_server_addr));
      IP_SET_TYPE_VAL(dns_server_addr, IPADDR_TYPE_V4);
    }
#endif /* LWIP_IPV4 && LWIP_IPV6 */
    if (!ip_addr_cmp(addr, &dns_server_addr)) {
      goto forward_dns_responseerr;
    }
  }

  /* Check if the name in the "question" part match with the name in the entry and
      skip it if equal. */
  res_idx = dns_compare_name(entry->name, p, SIZEOF_DNS_HDR, 1);
  if ((res_idx == 0xFFFF) || (res_idx == SIZEOF_DNS_HDR)) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": response not match to query\n", entry->name));
    goto forward_dns_responseerr;
  }

  /* check if "question" part matches the request */
  if (pbuf_copy_partial(p, &qry, SIZEOF_DNS_QUERY, res_idx) != SIZEOF_DNS_QUERY) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": response not match to query\n", entry->name));
    goto forward_dns_responseerr;
  }

  if ((qry.cls != PP_HTONS(DNS_RRCLASS_IN)) ||
      (LWIP_DNS_ADDRTYPE_IS_IPV6(entry->reqaddrtype) && (qry.type != PP_HTONS(DNS_RRTYPE_AAAA))) ||
      (!LWIP_DNS_ADDRTYPE_IS_IPV6(entry->reqaddrtype) && (qry.type != PP_HTONS(DNS_RRTYPE_A)))) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": response not match to query\n", entry->name));
    goto forward_dns_responseerr;
  }
  /* skip the rest of the "question" part */
  res_idx = (u16_t)(res_idx + SIZEOF_DNS_QUERY);

  if (nanswers == 0) {
    /* If there are no answers, then there is a chance to fallback based on entry->reqaddrtype */
    /* In that case, set err_code only if even the fallback response also has zero answers */
    if (!LWIP_DNS_ADDRTYPE_IS_FALLBACK(entry->reqaddrtype)) {
      LWIP_DEBUGF(DNS_DEBUG,
                  ("dns_recv: \"%s\": Requested name is valid but there is no associated A / AAAA records with it\n",
                  entry->name));
      err_code = NO_DATA;
      goto forward_dns_responseerr;
    }
  }

  while ((nanswers > 0) && (res_idx < p->tot_len)) {
      res_idx = dns_skip_name(p, res_idx);
      if (res_idx == 0) {
        LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: Insufficient buffer length for parsing domain name\n"));
        goto forward_dns_responseerr;
      }

    /* Check for IP address type and Internet class. Others are discarded. */
    if (pbuf_copy_partial(p, &ans, SIZEOF_DNS_ANSWER, res_idx) != SIZEOF_DNS_ANSWER) {
      LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: Parsing DNS answer failed\n"));
      goto forward_dns_responseerr;
    }
    if (res_idx + SIZEOF_DNS_ANSWER > 0xFFFF) {
      return;
    }
    res_idx = (u16_t)(res_idx + SIZEOF_DNS_ANSWER);

  /* Only support a answer whose Internet class is 'the Internet' and IP address type is A or AAAA */
    if (ans.cls == PP_HTONS(DNS_RRCLASS_IN)) {
#if LWIP_IPV4
      if ((ans.type == PP_HTONS(DNS_RRTYPE_A)) && (ans.len == PP_HTONS(sizeof(ip4_addr_t)))) {
#if LWIP_IPV4 && LWIP_IPV6
        if (!LWIP_DNS_ADDRTYPE_IS_IPV6(entry->reqaddrtype))
#endif /* LWIP_IPV4 && LWIP_IPV6 */
        {
          /* keep the lowest ttl of multiple IP_add */
          if (lwip_ntohl(ans.ttl) < entry->ttl) {
            entry->ttl = lwip_ntohl(ans.ttl);
          }

          /* read the IP address after answer resource record's header */
          ret = pbuf_copy_partial(p, &(entry->ipaddr[entry->ipaddr_count]), sizeof(ip4_addr_t), res_idx);

          if (ret != sizeof(ip4_addr_t)) {
            LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: Parsing DNS answer failed\n"));
            goto forward_dns_responseerr;
          }
          if (ip4_addr_isany_val(*ip_2_ip4(&entry->ipaddr[entry->ipaddr_count])) ||
             ip4_addr_isnone_val(*ip_2_ip4(&entry->ipaddr[entry->ipaddr_count]))) {
            err_code = NO_DATA;
            goto forward_dns_responseerr;
          }
          IP_SET_TYPE(&(entry->ipaddr[entry->ipaddr_count]), IPADDR_TYPE_V4);
          entry->ipaddr_count++;

          if (entry->ipaddr_count >= DNS_MAX_IPADDR) {
            break;
          }
        }
      }
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
      if ((ans.type == PP_HTONS(DNS_RRTYPE_AAAA)) && (ans.len == PP_HTONS(sizeof(ip6_addr_t)))) {
#if LWIP_IPV4 && LWIP_IPV6
        if (LWIP_DNS_ADDRTYPE_IS_IPV6(entry->reqaddrtype))
#endif /* LWIP_IPV4 && LWIP_IPV6 */
        {
          /* read the IP address after answer resource record's header */
          ret = pbuf_copy_partial(p, &(entry->ipaddr[entry->ipaddr_count]), sizeof(ip6_addr_t), res_idx);

          if (ret != sizeof(ip6_addr_t)) {
            LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: Parsing DNS answer failed\n"));
            goto forward_dns_responseerr;
          }
          if (ip6_addr_isany_val(*ip_2_ip6(&(entry->ipaddr[entry->ipaddr_count]))) ||
              ip6_addr_isnone_val(*ip_2_ip6(&(entry->ipaddr[entry->ipaddr_count])))) {
            err_code = NO_DATA;
            goto forward_dns_responseerr;
          }
          IP_SET_TYPE(&(entry->ipaddr[entry->ipaddr_count]), IPADDR_TYPE_V6);
          entry->ipaddr_count++;

          if (entry->ipaddr_count >= DNS_MAX_IPADDR) {
            break;
          }
        }
      }
#endif /* LWIP_IPV6 */
    }
  /* skip this answer */
    if ((int)(res_idx + lwip_htons(ans.len)) > 0xFFFF) {
      goto forward_dns_responseerr;
    }
    res_idx = (u16_t)(res_idx + lwip_htons(ans.len));
    --nanswers;
  }

forward_dns_responseerr:
  if (entry->ipaddr_count) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: response =  \"%s\"\n", entry->name));
#ifdef  LWIP_DEV_DEBUG
    u16_t i;
    for (i = 0; i < entry->ipaddr_count; i++) {
      ip_addr_debug_print(DNS_DEBUG, (&(entry->ipaddr[i])));
      LWIP_DEBUGF(DNS_DEBUG, ("\n"));
    }
#endif /* LWIP_DEV_DEBUG */
    dns_correct_response(entry_idx, lwip_ntohl(ans.ttl));
  } else {
    /* if no IP address extracted from response, mark this server as unavailable */
    DNS_SERVER_SET_UNAVAILABLE(entry, entry->server_idx);

    if (dns_backupserver_available(entry, 0) == 0) {
      /* if there is another backup DNS server to try, then don't stop the DNS request */
      /* retry the next available server immediately */
      entry->tmr     = 1;
      entry->state   = DNS_STATE_ASKING;
      LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": request the other server\n", entry->name));
      /* contact next available server for this entry */
      (void)dns_check_entry(entry_idx);
      return;
    }

#if LWIP_IPV4 && LWIP_IPV6
    if (LWIP_DNS_ADDRTYPE_IS_FALLBACK(entry->reqaddrtype)) {
      /* if no resolution and backup server is found, then check if there is scope for fallback */
      if (entry->reqaddrtype == LWIP_DNS_ADDRTYPE_IPV4_IPV6) {
        /* IPv4 failed, try IPv6 */
        dns_table[entry_idx].reqaddrtype = LWIP_DNS_ADDRTYPE_IPV6;
      } else {
        /* IPv6 failed, try IPv4 */
        dns_table[entry_idx].reqaddrtype = LWIP_DNS_ADDRTYPE_IPV4;
      }
      dns_table[entry_idx].state = DNS_STATE_NEW;
      (void)dns_check_entry(entry_idx);
      return;
    }
#endif

    /* ERROR: call specified callback function with NULL as name to indicate an error */
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": error in response with err_code = %d\n", entry->name, err_code));
    dns_call_found(entry_idx, NULL, err_code);
    /* flush this entry */
    dns_table[entry_idx].state = DNS_STATE_UNUSED;
  }
}

#if LWIP_DNS_REVERSE
static void
process_reverse_dns(struct pbuf *p, u8_t entry_idx, struct reverse_dns_table_entry *reverse_entry, struct dns_hdr hdr,
                    const ip_addr_t *addr)
{
  struct dns_answer ans = {0};
  struct dns_query qry = {0};
  ip_addr_t dns_server_addr;
  u16_t res_idx;
  u16_t ret;
  u16_t nquestions, nanswers;
  u16_t i;
  u8_t err_code = 0;
  u8_t length;

  reverse_entry->name_count = 0;

  /* We only care about the question(s) and the answers. The authrr
     and the extrarr are simply discarded. */
  nquestions = lwip_htons(hdr.numquestions);
  nanswers   = lwip_htons(hdr.numanswers);

  /* Check for correct response. */
  if ((hdr.flags1 & DNS_FLAG1_RESPONSE) == 0) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": not a response\n", reverse_entry->encoded_ip.name));
    goto reverse_dns_responseerr;
  }

  if ((hdr.flags2 & DNS_FLAG2_ERR_MASK) != 0) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": error in flags\n", reverse_entry->encoded_ip.name));

    if ((hdr.flags2 & DNS_FLAG2_ERR_SERVER_FAILURE) != 0) {
      LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": Some non-recoverable server failure occured\n",
                  reverse_entry->encoded_ip.name));
      err_code = NO_RECOVERY;
    }

    goto reverse_dns_responseerr;
  }

  if (nquestions != 1) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": response not match to query\n", reverse_entry->encoded_ip.name));
    goto reverse_dns_responseerr;
  }

  /* Check whether response comes from the same network address to which the
     question was sent. (RFC 5452) */
  ip_addr_copy(dns_server_addr, dns_servers[reverse_entry->server_idx]);
#if LWIP_IPV4 && LWIP_IPV6
  /* Dual-stack: Unmap IPv4 mapped IPv6 addresses */
  if (IP_IS_V6_VAL(dns_server_addr) && ip6_addr_isipv4mappedipv6(ip_2_ip6(&dns_server_addr))) {
    unmap_ipv4_mapped_ipv6(ip_2_ip4(&dns_server_addr), ip_2_ip6(&dns_server_addr));
    IP_SET_TYPE_VAL(dns_server_addr, IPADDR_TYPE_V4);
  }
#endif /* LWIP_IPV4 && LWIP_IPV6 */
  if (!ip_addr_cmp(addr, &dns_server_addr)) {
    goto reverse_dns_responseerr;
  }

  /* Check if the name in the "question" part match with the name in the entry and
     skip it if equal. */
  res_idx = dns_compare_name(reverse_entry->encoded_ip.name, p, SIZEOF_DNS_HDR, 1);
  if ((res_idx == 0xFFFF) || (res_idx == SIZEOF_DNS_HDR)) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": response not match to query\n", reverse_entry->encoded_ip.name));
    goto reverse_dns_responseerr;
  }

  /* check if "question" part matches the request */
  if (pbuf_copy_partial(p, &qry, SIZEOF_DNS_QUERY, res_idx) != SIZEOF_DNS_QUERY) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": response not match to query\n", reverse_entry->encoded_ip.name));
    goto reverse_dns_responseerr;
  }

  if (qry.type != PP_HTONS(DNS_RRTYPE_PTR)) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": response not match to a reverse_dns query\n",
                reverse_entry->encoded_ip.name));
    goto reverse_dns_responseerr;
  }

  /* skip the rest of the "question" part */
  res_idx = (u16_t)(res_idx + SIZEOF_DNS_QUERY);

  while ((nanswers > 0) && (res_idx < p->tot_len)) {
      res_idx = dns_skip_name(p, res_idx);
      if (res_idx == 0) {
        LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: Insufficient buffer length for parsing domain name\n"));
        goto reverse_dns_responseerr;
    }

    /* Check for IP address type and Internet class. Others are discarded. */
    if (pbuf_copy_partial(p, &ans, SIZEOF_DNS_ANSWER, res_idx) != SIZEOF_DNS_ANSWER) {
      LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: Parsing Reverse DNS answer failed\n"));
      goto reverse_dns_responseerr;
    }

    res_idx =  (u16_t)(res_idx + SIZEOF_DNS_ANSWER);

    /* Only need an answer whose Internet class is 'the Internet' and type is 'domain name pointer' */
    if (ans.cls == PP_HTONS(DNS_RRCLASS_IN) && ans.type == PP_HTONS(DNS_RRTYPE_PTR)) {
      ret = pbuf_copy_partial(p, reverse_entry->name, NI_MAXHOST, res_idx);
      if (ret == 0) {
        LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: Parsing Reverse DNS answer failed\n"));
        goto reverse_dns_responseerr;
      }

      /* Hostname received from the network will be of type [3]www[6]google[3]com\0 */
      /* Where [] contains the length of the next part */
      /* This has to be converted to www.google.com\0 */
      i = 0;
      while (reverse_entry->name[i] != '\0') {
        length = (u8_t)(reverse_entry->name[i++]);
        if (length > DNS_MAX_LABEL_LENGTH) {
          LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: Malformed Reverse DNS answer received\n"));
          goto reverse_dns_responseerr;
        }
        while (length && reverse_entry->name[i] != '\0') {
          reverse_entry->name[i - 1] = reverse_entry->name[i];
          ++i;
          --length;
        }
        if (reverse_entry->name[i] != '\0') {
          /* Dont insert period if there arent any more characters in the hostname */
          reverse_entry->name[i - 1] = '.';
        }
      }
      if (i) {
        reverse_entry->name[i - 1] = '\0';
      }
      reverse_entry->name_count = 1;
      break;
    }
    /* skip this answer */
    if ((u32_t)(res_idx + lwip_htons(ans.len)) > 0xFFFFU) {
      goto reverse_dns_responseerr;
    }
    res_idx = (u16_t)(res_idx + lwip_htons(ans.len));
    --nanswers;
  }

reverse_dns_responseerr:
  if (reverse_entry->name_count) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: response = \"%s\"\n", reverse_entry->name));
    reverse_dns_correct_response(entry_idx, lwip_ntohl(ans.ttl));
  } else {
    /* if no IP address extracted from response, mark this server as unavailable */
    DNS_SERVER_SET_UNAVAILABLE(reverse_entry, reverse_entry->server_idx);

    if (reverse_dns_backupserver_available(reverse_entry, 0) == 0) {
      /* if there is another backup DNS server to try, then don't stop the DNS request */
      /* retry the next available server immediately */
      reverse_entry->tmr     = 1;
      reverse_entry->state   = DNS_STATE_ASKING;
      LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": request the other server\n", reverse_entry->encoded_ip.name));
      /* contact next available server for this entry */
      (void)reverse_dns_check_entry(entry_idx);
      return;
    }

    /* ERROR: call specified callback function with NULL as name to indicate an error */
    LWIP_DEBUGF(DNS_DEBUG, ("dns_recv: \"%s\": error in response\n", reverse_entry->encoded_ip.name));
    reverse_dns_call_found(entry_idx, NULL, err_code);

    /* flush this entry */
    reverse_dns_table[entry_idx].state = DNS_STATE_UNUSED;
  }
}
#endif /* LWIP_DNS_REVERSE */

#endif /* LWIP_DNS */

#if defined(LWIP_DENY_DNS_SERVER) && (LWIP_DENY_DNS_SERVER)
static struct udp_pcb *dns_server_pcb = NULL;

static void
dns_server_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  struct pbuf *send_p = NULL;
  struct dns_hdr send_hdr = {0};

  LWIP_UNUSED_ARG(arg);

  LWIP_DEBUGF(DNS_DEBUG, ("dns_server_recv::ip "));
  ip_addr_debug_print_val(DNS_DEBUG, *addr);
  LWIP_DEBUGF(DNS_DEBUG, ("port %d\r\n", port));

  /* is the dns message big enough ? */
  if (p->tot_len < (SIZEOF_DNS_HDR + SIZEOF_DNS_QUERY)) {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_server_recv: pbuf too small\n"));
    /* free pbuf and return */
    goto ignore_packet;
  }

  if (pbuf_copy_partial(p, &send_hdr, SIZEOF_DNS_HDR, 0) == SIZEOF_DNS_HDR) {
    /* set flags */
    send_hdr.flags1 |= DNS_FLAG1_RESPONSE;
    send_hdr.flags2 = DNS_FLAG2_REFUSE; /* Reply code (Query Refused) */
  }

  send_p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(p->tot_len), PBUF_RAM);
  if (send_p != NULL) {
    memcpy_s(send_p->payload, p->tot_len, p->payload, p->tot_len);
    pbuf_take(send_p, &send_hdr, SIZEOF_DNS_HDR);
  } else {
    LWIP_DEBUGF(DNS_DEBUG, ("dns_server_recv: pbuf alloc fail\n"));
    goto ignore_packet;
  }

  (void)udp_sendto(pcb, send_p, addr, port);

ignore_packet:
  /* deallocate memory and return */
  pbuf_free(p);
  return;
}

void dns_server_init(void)
{
  LWIP_DEBUGF(DNS_DEBUG, ("dns_server_init: initializing\n"));

  if (dns_server_pcb == NULL) {
    dns_server_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
#if LWIP_SO_PRIORITY
    dns_server_pcb->priority = LWIP_PKT_PRIORITY_DNS;
#endif /* LWIP_SO_PRIORITY */
    udp_bind(dns_server_pcb, IP_ANY_TYPE, DNS_SERVER_PORT);
    udp_recv(dns_server_pcb, dns_server_recv, NULL);
  }
  LWIP_DEBUGF(DNS_DEBUG, ("dns_server_init\r\n"));
}

void dns_server_deinit(void)
{
  if (dns_server_pcb != NULL) {
    udp_remove(dns_server_pcb);
    dns_server_pcb = NULL;
  }
}
#endif /* defined(LWIP_DENY_DNS_SERVER) && (LWIP_DENY_DNS_SERVER) */