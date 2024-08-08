/**
 * @file
 * Dynamic pool memory manager
 *
 * lwIP has dedicated pools for many structures (netconn, protocol control blocks,
 * packet buffers, ...). All these pools are managed here.
 *
 * @defgroup mempool Memory pools
 * @ingroup infrastructure
 * Custom memory pools

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

#include "lwip/opt.h"

#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/stats.h"

#include <string.h>

/* Make sure we include everything we need for size calculation required by memp_std.h */
#include "lwip/pbuf.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/ip4_frag.h"
#include "lwip/netbuf.h"
#include "lwip/api.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/priv/api_msg.h"
#include "lwip/priv/sockets_priv.h"
#include "lwip/etharp.h"
#include "lwip/igmp.h"
#include "lwip/timeouts.h"
/* needed by default MEMP_NUM_SYS_TIMEOUT */
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/priv/nd6_priv.h"
#include "lwip/ip6_frag.h"
#include "lwip/mld6.h"

#if LWIP_ALLOW_SOCKET_CONFIG
struct memp_desc *memp_pools[MEMP_MAX] = {0};
#else
#define LWIP_MEMPOOL(name,num,size,desc) LWIP_MEMPOOL_DECLARE(name,num,size,desc)
#include "lwip/priv/memp_std.h"

const struct memp_desc *const memp_pools[MEMP_MAX] = {
#define LWIP_MEMPOOL(name,num,size,desc) &memp_ ## name,
#include "lwip/priv/memp_std.h"
};
#endif

#ifdef LWIP_HOOK_FILENAME
#include LWIP_HOOK_FILENAME
#endif

#if MEMP_MEM_MALLOC && MEMP_OVERFLOW_CHECK >= 2
#undef MEMP_OVERFLOW_CHECK
/* MEMP_OVERFLOW_CHECK >= 2 does not work with MEMP_MEM_MALLOC, use 1 instead */
#define MEMP_OVERFLOW_CHECK 1
#endif

#if MEMP_SANITY_CHECK && !MEMP_MEM_MALLOC
/**
 * Check that memp-lists don't form a circle, using "Floyd's cycle-finding algorithm".
 */
static int
memp_sanity(const struct memp_desc *desc)
{
  struct memp *t, *h;

  t = *desc->tab;
  if (t != NULL) {
    for (h = t->next; (t != NULL) && (h != NULL); t = t->next,
         h = ((h->next != NULL) ? h->next->next : NULL)) {
      if (t == h) {
        return 0;
      }
    }
  }

  return 1;
}
#endif /* MEMP_SANITY_CHECK && !MEMP_MEM_MALLOC */

#if MEMP_OVERFLOW_CHECK
/**
 * Check if a memp element was victim of an overflow or underflow
 * (e.g. the restricted area after/before it has been altered)
 *
 * @param p the memp element to check
 * @param desc the pool p comes from
 */
static void
memp_overflow_check_element(struct memp *p, const struct memp_desc *desc)
{
  mem_overflow_check_raw((u8_t *)p + MEMP_SIZE, desc->size, "pool ", desc->desc);
}

/**
 * Initialize the restricted area of on memp element.
 */
static void
memp_overflow_init_element(struct memp *p, const struct memp_desc *desc)
{
  mem_overflow_init_raw((u8_t *)p + MEMP_SIZE, desc->size);
}

#if MEMP_OVERFLOW_CHECK >= 2
/**
 * Do an overflow check for all elements in every pool.
 *
 * @see memp_overflow_check_element for a description of the check
 */
static void
memp_overflow_check_all(void)
{
  u16_t i, j;
  struct memp *p;
  SYS_ARCH_DECL_PROTECT(old_level);
  SYS_ARCH_PROTECT(old_level);

  for (i = 0; i < MEMP_MAX; ++i) {
    p = (struct memp *)LWIP_MEM_ALIGN(memp_pools[i]->base);
    for (j = 0; j < memp_pools[i]->num; ++j) {
      memp_overflow_check_element(p, memp_pools[i]);
      p = LWIP_ALIGNMENT_CAST(struct memp *, ((u8_t *)p + MEMP_SIZE + memp_pools[i]->size + MEM_SANITY_REGION_AFTER_ALIGNED));
    }
  }
  SYS_ARCH_UNPROTECT(old_level);
}
#endif /* MEMP_OVERFLOW_CHECK >= 2 */
#endif /* MEMP_OVERFLOW_CHECK */

#if LWIP_ALLOW_SOCKET_CONFIG
/**
 * Initialize custom memory pool.
 * Related functions: memp_malloc_pool, memp_free_pool
 *
 * @param desc pool to initialize
 */
void
memp_init_pool(const char *szdesc, u32_t num, u16_t size, u8_t type)
{
#if MEMP_MEM_MALLOC
  LWIP_UNUSED_ARG(szdesc);
  LWIP_UNUSED_ARG(num);
  LWIP_UNUSED_ARG(size);
  LWIP_UNUSED_ARG(type);
#else
  u32_t i;
  struct memp *memp = NULL;
  struct memp_desc *desc = NULL;
  u32_t pool_size = ((num) * (MEMP_SIZE + MEMP_ALIGN_SIZE(size)));
  desc = mem_malloc(sizeof(struct memp_desc));
  if (desc == NULL) {
    return;
  }
  (void)memset_s(desc, sizeof(struct memp_desc), 0, sizeof(struct memp_desc));
  desc->base = mem_malloc(pool_size);
  if (desc->base == NULL) {
    mem_free(desc);
    return;
  }
#if MEMP_STATS
  desc->stats = mem_malloc(sizeof(struct stats_mem));
  if (desc->stats == NULL) {
    mem_free(desc->base);
    mem_free(desc);
    return;
  }
  (void)memset(desc->stats, 0, sizeof(struct stats_mem));
#if LWIP_STATS
    lwip_stats.memp[type] = desc->stats;
    desc->stats->max = num;
#endif /* LWIP_STATS */
#endif /* MEMP_STATS */
  (void)memset_s(desc->base, pool_size, 0, pool_size);
  memp_pools[type] = desc;
  desc->desc = szdesc;
  desc->size = (u16_t)LWIP_MEM_ALIGN_SIZE((u32_t)size);
  desc->num  = (u16_t)num;
  desc->tab = &desc->tab_tail;
  *desc->tab = NULL;

  memp = (struct memp*)(desc->base);
  /* create a linked list of memp elements */
  for (i = 0; i < desc->num; ++i) {
    memp->next = *desc->tab;
    *desc->tab = memp;
#if MEMP_OVERFLOW_CHECK
    memp_overflow_init_element(memp, desc);
#endif /* MEMP_OVERFLOW_CHECK */
    /* cast through void* to get rid of alignment warnings */
    memp = (struct memp *)(void *)((u8_t *)memp + MEMP_SIZE + desc->size
#if MEMP_OVERFLOW_CHECK
      + MEM_SANITY_REGION_AFTER_ALIGNED
#endif /* MEMP_OVERFLOW_CHECK */
    );
  }
#if MEMP_STATS
  desc->stats->avail = desc->num;
#endif /* MEMP_STATS */
#endif /* MEMP_MEM_MALLOC */

#if MEMP_STATS && (defined(LWIP_DEBUG) || LWIP_STATS_DISPLAY)
  desc->stats->name  = desc->desc;
#endif /* MEMP_STATS && (defined(LWIP_DEBUG) || LWIP_STATS_DISPLAY) */
}

unsigned int g_lwip_num_sockets = DEFAULT_LWIP_NUM_SOCKETS;
int lwip_set_socket_num(unsigned int socketnum)
{
  extern volatile int tcpip_init_finish;
  SYS_ARCH_DECL_PROTECT(old_level);
  if (tcpip_init_finish) {
    return -1;
  }
  if ((socketnum == 0) || (socketnum > LWIP_NUM_SOCKETS_MAX)) {
    return -1;
  }
  SYS_ARCH_PROTECT(old_level);
  g_lwip_num_sockets = socketnum;
  SYS_ARCH_UNPROTECT(old_level);
  return 0;
}

unsigned int lwip_get_socket_num(void)
{
  unsigned int ret;
  SYS_ARCH_DECL_PROTECT(old_level);
  SYS_ARCH_PROTECT(old_level);
  ret = g_lwip_num_sockets;
  SYS_ARCH_UNPROTECT(old_level);
  return ret;
}
#else
/**
 * Initialize custom memory pool.
 * Related functions: memp_malloc_pool, memp_free_pool
 *
 * @param desc pool to initialize
 */
void
memp_init_pool(const struct memp_desc *desc)
{
#if MEMP_MEM_MALLOC
  LWIP_UNUSED_ARG(desc);
#else
  int i;
  struct memp *memp;

  *desc->tab = NULL;
  memp = (struct memp *)LWIP_MEM_ALIGN(desc->base);
#if MEMP_MEM_INIT
  /* force memset on pool memory */
  memset(memp, 0, (size_t)desc->num * (MEMP_SIZE + desc->size
#if MEMP_OVERFLOW_CHECK
                                       + MEM_SANITY_REGION_AFTER_ALIGNED
#endif
                                      ));
#endif
  /* create a linked list of memp elements */
  for (i = 0; i < desc->num; ++i) {
    memp->next = *desc->tab;
    *desc->tab = memp;
#if MEMP_OVERFLOW_CHECK
    memp_overflow_init_element(memp, desc);
#endif /* MEMP_OVERFLOW_CHECK */
    /* cast through void* to get rid of alignment warnings */
    memp = (struct memp *)(void *)((u8_t *)memp + MEMP_SIZE + desc->size
#if MEMP_OVERFLOW_CHECK
                                   + MEM_SANITY_REGION_AFTER_ALIGNED
#endif
                                  );
  }
#if MEMP_STATS
  desc->stats->avail = desc->num;
#endif /* MEMP_STATS */
#endif /* !MEMP_MEM_MALLOC */

#if MEMP_STATS && (defined(LWIP_DEBUG) || LWIP_STATS_DISPLAY)
  desc->stats->name  = desc->desc;
#endif /* MEMP_STATS && (defined(LWIP_DEBUG) || LWIP_STATS_DISPLAY) */
}
#endif

/**
 * Initializes lwIP built-in pools.
 * Related functions: memp_malloc, memp_free
 *
 * Carves out memp_memory into linked lists for each pool-type.
 */
void
memp_init(void)
{
  u16_t i = 0;
#if LWIP_ALLOW_SOCKET_CONFIG
  static u8_t  memp_init_flag = 0;

  if (memp_init_flag == 1) {
    return;
  }

  #define LWIP_MEMPOOL(name, num, size, desc) memp_init_pool(desc, num, (u16_t)size, (u8_t)i++);
  #include "lwip/priv/memp_std.h"

  memp_init_flag = 1;
#else /* LWIP_ALLOW_SOCKET_CONFIG */
  /* for every pool: */
  for (i = 0; i < LWIP_ARRAYSIZE(memp_pools); i++) {
    memp_init_pool(memp_pools[i]);

#if LWIP_STATS && MEMP_STATS
    lwip_stats.memp[i] = memp_pools[i]->stats;
#endif
  }
#endif /* LWIP_ALLOW_SOCKET_CONFIG */

#if MEMP_OVERFLOW_CHECK >= 2
  /* check everything a first time to see if it worked */
  memp_overflow_check_all();
#endif /* MEMP_OVERFLOW_CHECK >= 2 */
}

static void *
#if !MEMP_OVERFLOW_CHECK
do_memp_malloc_pool(const struct memp_desc *desc)
#else
do_memp_malloc_pool_fn(const struct memp_desc *desc, const char *file, const int line)
#endif
{
  struct memp *memp;
  SYS_ARCH_DECL_PROTECT(old_level);

#if MEMP_MEM_MALLOC
  memp = (struct memp *)mem_malloc(MEMP_SIZE + MEMP_ALIGN_SIZE(desc->size));
  SYS_ARCH_PROTECT(old_level);
#else /* MEMP_MEM_MALLOC */
  SYS_ARCH_PROTECT(old_level);

  memp = *desc->tab;
#endif /* MEMP_MEM_MALLOC */

  if (memp != NULL) {
#if MEMP_DOUBLE_FREE_CHECK
    memp->is_used = 1;
#endif

#if !MEMP_MEM_MALLOC
#if MEMP_OVERFLOW_CHECK == 1
    memp_overflow_check_element(memp, desc);
#endif /* MEMP_OVERFLOW_CHECK */

    *desc->tab = memp->next;
#if MEMP_OVERFLOW_CHECK
    memp->next = NULL;
#endif /* MEMP_OVERFLOW_CHECK */
#endif /* !MEMP_MEM_MALLOC */
#if MEMP_OVERFLOW_CHECK
    memp->file = file;
    memp->line = line;
#if MEMP_MEM_MALLOC
    memp_overflow_init_element(memp, desc);
#endif /* MEMP_MEM_MALLOC */
#endif /* MEMP_OVERFLOW_CHECK */
    LWIP_ASSERT("memp_malloc: memp properly aligned",
                ((mem_ptr_t)memp % MEM_ALIGNMENT) == 0);
#if MEMP_STATS
    desc->stats->used++;
    if (desc->stats->used > desc->stats->max) {
      desc->stats->max = desc->stats->used;
    }
#endif
    SYS_ARCH_UNPROTECT(old_level);
    /* cast through u8_t* to get rid of alignment warnings */
    return ((u8_t *)memp + MEMP_SIZE);
  } else {
#if MEMP_STATS
    desc->stats->err++;
#endif
    LWIP_DEBUGF(MEMP_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("memp_malloc: out of memory in pool %s\n", desc->desc));
  }

  SYS_ARCH_UNPROTECT(old_level);
  return NULL;
}

/**
 * Get an element from a custom pool.
 *
 * @param desc the pool to get an element from
 *
 * @return a pointer to the allocated memory or a NULL pointer on error
 */
void *
#if !MEMP_OVERFLOW_CHECK
memp_malloc_pool(const struct memp_desc *desc)
#else
memp_malloc_pool_fn(const struct memp_desc *desc, const char *file, const int line)
#endif
{
  LWIP_ASSERT("invalid pool desc", desc != NULL);
  if (desc == NULL) {
    return NULL;
  }

#if !MEMP_OVERFLOW_CHECK
  return do_memp_malloc_pool(desc);
#else
  return do_memp_malloc_pool_fn(desc, file, line);
#endif
}

/**
 * Get an element from a specific pool.
 *
 * @param type the pool to get an element from
 *
 * @return a pointer to the allocated memory or a NULL pointer on error
 */
void *
#if !MEMP_OVERFLOW_CHECK
memp_malloc(memp_t type)
#else
memp_malloc_fn(memp_t type, const char *file, const int line)
#endif
{
  void *memp;
  LWIP_ERROR("memp_malloc: type < MEMP_MAX", (type < MEMP_MAX), return NULL;);

#if MEMP_OVERFLOW_CHECK >= 2
  memp_overflow_check_all();
#endif /* MEMP_OVERFLOW_CHECK >= 2 */

#if !MEMP_OVERFLOW_CHECK
  memp = do_memp_malloc_pool(memp_pools[type]);
#else
  memp = do_memp_malloc_pool_fn(memp_pools[type], file, line);
#endif

  return memp;
}

static void
do_memp_free_pool(const struct memp_desc *desc, void *mem)
{
  struct memp *memp;
  SYS_ARCH_DECL_PROTECT(old_level);

  LWIP_ASSERT("memp_free: mem properly aligned",
              ((mem_ptr_t)mem % MEM_ALIGNMENT) == 0);

  /* cast through void* to get rid of alignment warnings */
  memp = (struct memp *)(void *)((u8_t *)mem - MEMP_SIZE);

  SYS_ARCH_PROTECT(old_level);
#if MEMP_DOUBLE_FREE_CHECK
  if (!memp->is_used) {
    LWIP_DEBUGF(MEMP_DEBUG, ("memp_free: double free memp\n"));
    SYS_ARCH_UNPROTECT(old_level);
    return;
  }
#endif

#if MEMP_OVERFLOW_CHECK == 1
  memp_overflow_check_element(memp, desc);
#endif /* MEMP_OVERFLOW_CHECK */

#if MEMP_STATS
  desc->stats->used--;
#endif

#if MEMP_DOUBLE_FREE_CHECK
  memp->is_used = 0;
#endif
#if MEMP_MEM_MALLOC
  LWIP_UNUSED_ARG(desc);
  SYS_ARCH_UNPROTECT(old_level);
  mem_free(memp);
#else /* MEMP_MEM_MALLOC */
  memp->next = *desc->tab;
  *desc->tab = memp;

#if MEMP_SANITY_CHECK
  LWIP_ASSERT("memp sanity", memp_sanity(desc));
#endif /* MEMP_SANITY_CHECK */

  SYS_ARCH_UNPROTECT(old_level);
#endif /* !MEMP_MEM_MALLOC */
}

/**
 * Put a custom pool element back into its pool.
 *
 * @param desc the pool where to put mem
 * @param mem the memp element to free
 */
void
memp_free_pool(const struct memp_desc *desc, void *mem)
{
  LWIP_ASSERT("invalid pool desc", desc != NULL);
  if ((desc == NULL) || (mem == NULL)) {
    return;
  }

  do_memp_free_pool(desc, mem);
}

#if !MEMP_MEM_MALLOC
u32_t
memp_check(memp_t type, void *mem)
{
  u8_t *memp_start = NULL;
  u8_t *memp_end = NULL;

  memp_start = (u8_t *)LWIP_MEM_ALIGN((void *)memp_pools[type]->base) + MEMP_SIZE;
  memp_end = (u8_t *)memp_start
    + (memp_pools[type]->num - 1U) * (MEMP_SIZE + memp_pools[type]->size
#if MEMP_OVERFLOW_CHECK
    + MEM_SANITY_REGION_AFTER_ALIGNED
#endif
   );

  if ((mem >= (void*)memp_start) && (mem <= (void*)memp_end)) {
    return 1;
  } else {
    return 0;
  }
}
#endif

/**
 * Put an element back into its pool.
 *
 * @param type the pool where to put mem
 * @param mem the memp element to free
 */
void
memp_free(memp_t type, void *mem)
{
#ifdef LWIP_HOOK_MEMP_AVAILABLE
  struct memp *old_first;
#endif

  LWIP_ERROR("memp_free: type < MEMP_MAX", (type < MEMP_MAX), return;);

  if (mem == NULL) {
    return;
  }

#if MEMP_OVERFLOW_CHECK >= 2
  memp_overflow_check_all();
#endif /* MEMP_OVERFLOW_CHECK >= 2 */

#ifdef LWIP_HOOK_MEMP_AVAILABLE
  old_first = *memp_pools[type]->tab;
#endif

  do_memp_free_pool(memp_pools[type], mem);

#ifdef LWIP_HOOK_MEMP_AVAILABLE
  if (old_first == NULL) {
    LWIP_HOOK_MEMP_AVAILABLE(type);
  }
#endif
}
#if !MEMP_MEM_MALLOC
#ifdef LWIP_DEBUG_INFO
void debug_memp_info(void)
{
  memp_t type;
  u32_t memp_used;
  u32_t memp_aval;
  struct memp *memp = NULL;
#if LWIP_ALLOW_SOCKET_CONFIG
  struct memp_desc *desc = NULL;
#else
  const struct memp_desc *desc = NULL;
#endif
  SYS_ARCH_DECL_PROTECT(old_level);

  LWIP_PLATFORM_PRINT("[MEMP]%-4s %-8s %-8s %-8s\n",
                      "TYPE", "NUM", "USED", "AVAILABLE");

  for (type = (memp_t)(0); type < (memp_t)(MEMP_MAX); type++) {
    SYS_ARCH_PROTECT(old_level);
    desc = memp_pools[type];
    memp_aval = 0;
    memp = *desc->tab;
    while (memp != NULL) {
      memp_aval++;
      memp = memp->next;
    }
    memp_used = desc->num - memp_aval;

    SYS_ARCH_UNPROTECT(old_level);
    LWIP_PLATFORM_PRINT("[MEMP]%-4d ", type);
    LWIP_PLATFORM_PRINT("%-8u  ", desc->num);
    LWIP_PLATFORM_PRINT("%-8u  ", memp_used);
    LWIP_PLATFORM_PRINT("%-8u\n", memp_aval);
  }
}

void debug_memp_detail(int type)
{
  u32_t idx;
#if LWIP_ALLOW_SOCKET_CONFIG
  struct memp_desc *desc = NULL;
#else
  const struct memp_desc *desc = NULL;
#endif

  if ((type >= MEMP_MAX) || (type < 0)) {
    LWIP_PLATFORM_PRINT("[MEMP]memp type  %u is more than MEMP_MAX or < 0\n", type);
    return;
  }

  desc = memp_pools[type];

  LWIP_PLATFORM_PRINT("[MEMP]%-4s %-8s\n", "TYPE", "INDEX");
  for (idx = 0; idx < desc->num; idx++) {
    LWIP_PLATFORM_PRINT("[MEMP]%-4d ", type);
    LWIP_PLATFORM_PRINT("%-8u\n",   idx);
  }
}

void debug_memp_type_info(int type)
{
#if LWIP_ALLOW_SOCKET_CONFIG
  struct memp_desc *desc = NULL;
#else
  const struct memp_desc *desc = NULL;
#endif
  struct memp *memp = NULL;
  struct memp *memp_free_list = NULL;
  u32_t idx;
  u8_t found_in_free_list;
  SYS_ARCH_DECL_PROTECT(old_level);

  if ((type != MEMP_TCP_PCB) &&
#if LWIP_RAW
    (type != MEMP_RAW_PCB) &&
#endif /* LWIP_RAW */
    (type != MEMP_UDP_PCB) && (type != MEMP_NETCONN)) {
    LWIP_PLATFORM_PRINT("memp type error!\n");
    return;
  }

  desc = memp_pools[type];

  for (idx = 0; idx < desc->num; idx++) {
    SYS_ARCH_PROTECT(old_level);
    memp = (struct memp *)(void *)((u8_t *)(LWIP_MEM_ALIGN(desc->base)) + idx * (MEMP_SIZE + desc->size
#if MEMP_OVERFLOW_CHECK
      + MEM_SANITY_REGION_AFTER_ALIGNED
#endif
      ));

    /* Check whether the current memp element is present in the free list */
    memp_free_list = *desc->tab;

    found_in_free_list = 0;
    while (memp_free_list != NULL) {
      if (memp_free_list == memp) {
        found_in_free_list = 1;
        break;
      }
      memp_free_list = memp_free_list->next;
    }
    SYS_ARCH_UNPROTECT(old_level);
    if (found_in_free_list != 0) {
      /* If the memp element is found in the free list, then dont print its debug info */
      continue;
    }

    switch (type) {
      case MEMP_TCP_PCB:
        (void)debug_tcppcb_info((struct tcp_pcb *)((u8_t*)memp + MEMP_SIZE));
        break;
      case MEMP_UDP_PCB:
        (void)debug_udppcb_info((struct udp_pcb *)((u8_t*)memp + MEMP_SIZE));
        break;
#if LWIP_RAW
      case MEMP_RAW_PCB:
        (void)debug_rawpcb_info((struct raw_pcb *)((u8_t*)memp + MEMP_SIZE));
        break;
#endif
      case MEMP_NETCONN:
        (void)debug_netconn_info((struct netconn *)((u8_t*)memp + MEMP_SIZE), 0);
        break;
      default:
        break;
    }
  }
}
#endif /* LWIP_DEBUG_INFO */
#endif /* !MEMP_MEM_MALLOC */
