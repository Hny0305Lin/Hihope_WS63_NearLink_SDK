/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: implementation for lowpower
 * Author: none
 * Create: 2019
 */

#include "lwip/opt.h"

#include "lwip/priv/tcp_priv.h"

#include "lwip/def.h"
#include "lwip/memp.h"
#include "lwip/priv/tcpip_priv.h"

#include "lwip/ip4_frag.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/igmp.h"
#include "lwip/dns.h"
#include "lwip/nd6.h"
#include "lwip/ip6_frag.h"
#include "lwip/mld6.h"
#include "lwip/dhcp6.h"
#include "lwip/sys.h"
#include "lwip/pbuf.h"
#include "netif/lowpan6.h"
#include "lwip/nat64.h"
#include "lwip/api.h"
#include "lwip/ip6in4.h"

#if LWIP_LOWPOWER
#include "lwip/lowpower.h"

#define TIMEOUT_MAX 120000 /* two min */
#define SYS_TIMEOUT_WAIT_TICKS    30
#define SYS_TIMEOUT_WAIT_TIME_MS  3

static u32_t g_wake_up_time = TIMEOUT_MAX;
static enum lowpower_mod g_lowpower_switch = LOW_TMR_LOWPOWER_MOD;
static struct timer_entry *g_timer_header = NULL;
static struct timer_mng g_timer_mng = { LOW_TMR_TIMER_HANDLING, 0 };
static u32_t g_last_check_timeout = 0;

static const struct timer_handler lowpower_timer_handler[] = {
#if LWIP_TCP
  /*
   * The TCP timer is a special case: it does not have to run always and
   * is triggered to start from TCP using tcp_timer_needed()
   */
  {TCP_FAST_INTERVAL, tcp_fasttmr, tcp_fast_tmr_tick TCP_FASTTMR_NAME},
  {TCP_SLOW_INTERVAL, tcp_slowtmr, tcp_slow_tmr_tick TCP_SLOWTMR_NAME},
#endif /* LWIP_TCP */

#if LWIP_IPV4
#if IP_REASSEMBLY
  {IP_TMR_INTERVAL, ip_reass_tmr, ip_reass_tmr_tick IP_REASSTRM_NAME},
#endif /* IP_REASSEMBLY */

#if LWIP_ARP
  {ARP_TMR_INTERVAL, etharp_tmr, etharp_tmr_tick ETHARPTMR_NAME},
#endif /* LWIP_ARP */

#if LWIP_DHCP
  {DHCP_FINE_TIMER_MSECS, dhcp_fine_tmr, dhcp_fine_tmr_tick DHCP_FINETMR_NAME},
  {DHCP_COARSE_TIMER_MSECS, dhcp_coarse_tmr, dhcp_coarse_tmr_tick DHCP_COARSETMR_NAME},
#endif /* LWIP_DHCP */

#if LWIP_AUTOIP
  {AUTOIP_TMR_INTERVAL, autoip_tmr, autoip_tmr_tick AUTOIPTMR_NAME},
#endif /* LWIP_AUTOIP */

#if LWIP_IGMP
  {IGMP_TMR_INTERVAL, igmp_tmr, igmp_tmr_tick IGMPTMR_NAME},
#endif /* LWIP_IGMP */
#endif /* LWIP_IPV4 */

#if LWIP_DNS
  {DNS_TMR_INTERVAL, dns_tmr, dns_tmr_tick DNSTMR_NAME},
#endif /* LWIP_DNS */

#if LWIP_NAT64
  {NAT64_TMR_INTERVAL, nat64_tmr, nat64_tmr_tick NAT64TMR_NAME},
#endif /* LWIP_NAT64 */

#if LWIP_IP6IN4
  {IP6IN4_TMR_INTERVAL, ip6in4_tmr, ip6in4_tmr_tick IP6IN4TMR_NAME},
#endif /* LWIP_IP6IN4 */

#if LWIP_IPV6
  {ND6_TMR_INTERVAL, nd6_tmr, nd6_tmr_tick ND6TMR_NAME},
#if LWIP_IPV6_REASS
  {IP6_REASS_TMR_INTERVAL, ip6_reass_tmr, ip6_reass_tmr_tick IP6_TREASSTMR_NAME},
#endif /* LWIP_IPV6_REASS */

#if LWIP_IPV6_MLD
  {MLD6_TMR_INTERVAL, mld6_tmr, mld6_tmr_tick MLD6TMR_NAME},
#endif /* LWIP_IPV6_MLD */

#if LWIP_IPV6_DHCP6
  {DHCP6_TIMER_MSECS, dhcp6_tmr, dhcp6_tmr_tick DHCP6TMR_NAME},
#endif /* LWIP_IPV6_DHCP6 */

#if LWIP_6LOWPAN
  {LOWPAN6_TMR_INTERVAL, lowpan6_tmr, lowpan6_tmr_tick LOWPAN6TMR_NAME},
#endif /* LWIP_6LOWPAN */
#endif /* LWIP_IPV6 */
};

void
set_timer_state(enum timer_state state, u32_t waiting_time)
{
  g_timer_mng.waiting_time = waiting_time;
  g_timer_mng.state = state;
}

/* should not call by tcpip_thread */
u8_t
sys_timeout_waiting_long(void)
{
  u8_t i = 0;
  u8_t j = 0;

  while (g_timer_mng.state == LOW_TMR_GETING_TICKS) {
    i++;
    if (i == SYS_TIMEOUT_WAIT_TICKS) {
      i = 0;
      j++;
      if (j == SYS_TIMEOUT_WAIT_TIME_MS) {
        break;
      }
      sys_msleep(1);
    }
  }

  if (g_timer_mng.state == LOW_TMR_TIMER_WAITING) {
    return ((g_timer_mng.waiting_time > TIMEOUT_TICK) ? 1 : 0);
  }

  return 0;
}

err_t
sys_timeout_reg(
  u32_t msec,
  sys_timeout_handler handler,
  void *arg,
#if LOWPOWER_TIMER_DEBUG
  char *name,
#endif
  get_next_timeout next_tick)
{
  struct timer_entry *timeout = NULL;
  struct timer_entry *temp = NULL;

  if (handler == NULL) {
    return -1;
  }

  timeout = (struct timer_entry *)memp_malloc(MEMP_SYS_TIMEOUT);
  if (timeout == NULL) {
    LOWPOWER_DEBUG(("sys_timeout_reg: timeout != NULL, pool MEMP_SYS_TIMEOUT is empty"));
    return -1;
  }

  timeout->handler = handler;
  timeout->clock_max = msec / TIMEOUT_TICK; /* time interval */
  timeout->next_tick = next_tick;
  timeout->timeout = sys_now();
  timeout->args = arg;
  timeout->enable = 0;

#if LOWPOWER_TIMER_DEBUG
  timeout->name = name;
#endif

  /* add list tail */
  timeout->next = NULL;
  if (g_timer_header == NULL) {
    g_timer_header = timeout;
  } else {
    temp = g_timer_header;
    while (temp->next != NULL) {
      temp = temp->next;
    }
    temp->next = timeout;
  }

  return 0;
}

/* deal timeout and return next timer prev */
static void
timeout_handler(struct timer_entry *t, u32_t now)
{
#if LOWPOWER_TIMER_DEBUG
  if (t->name != NULL) {
    LOWPOWER_DEBUG(("%s timeout: now:%u\n", t->name, now));
  }
#endif

  t->handler(t->args);
  LWIP_UNUSED_ARG(now);
  t->timeout += t->clock_max * TIMEOUT_TICK;
}

static u32_t
get_timer_tick(struct timer_entry *t)
{
  u32_t tick;

  /* disable lowpower, need to timeout once a tick */
  if (g_lowpower_switch == LOW_TMR_NORMAL_MOD) {
    return t->clock_max;
  }

  if (t->next_tick != NULL) {
    tick = t->next_tick();
  } else {
    tick = 1;
    LOWPOWER_DEBUG(("next->tick is NULL\n"));
  }
  tick *= t->clock_max;
  return tick;
}

static void
handle_timer_and_free(struct timer_entry **pt, struct timer_entry **pn, struct timer_entry *p)
{
  struct timer_entry *t = *pt;
  struct timer_entry *n = *pn;
  /* insert after previous node or as the header */
  if (p == NULL) {
    g_timer_header = n;
  } else {
    p->next = n;
  }

  t->next = NULL;
  sys_timeout_handler handler = t->handler;
  void *args = t->args;
  memp_free(MEMP_SYS_TIMEOUT, t);
  *pt = NULL;
  handler(args);

  /* the last entry */
  if ((n == NULL) && (p != NULL)) {
    *pn = p->next;
  }
}

static u32_t
get_sleep_time(u32_t now)
{
  struct timer_entry *t = NULL;
  struct timer_entry *n = NULL;
  struct timer_entry *p = NULL;
  u32_t msec = TIMEOUT_MAX;
  u32_t tick;
  u32_t temp;

  LOWPOWER_DEBUG(("\n*******get_sleep_time*****************\n"));

  for (t = g_timer_header, p = NULL; t != NULL; t = n) {
    n = t->next;
again:
    tick = get_timer_tick(t);
    if (tick == 0) {
      t->enable = 0;
      p = t;
      continue;
    }

    if (t->enable == 0) {
      t->timeout = now;
    }
    t->enable = 1;
    temp = tick * TIMEOUT_TICK;

    if (temp <= now - t->timeout) {
      if (t->next_tick == NULL) {
        handle_timer_and_free(&t, &n, p);
        /* t is free p=p */
        continue;
      }
      timeout_handler(t, now);
      goto again;
    }

    temp = temp - (now - t->timeout);
    msec = msec > temp ? temp : msec;
    p = t;
  }

  LOWPOWER_DEBUG(("msec = %u now = %u\n", msec, now));
  return msec;
}

static void
check_timeout(u32_t now)
{
  struct timer_entry *t = NULL;
  struct timer_entry *n = NULL;
  struct timer_entry *p = NULL;
  u32_t msec;
  u32_t ticks;
  u32_t i;

  LOWPOWER_DEBUG(("\n**********timeout**************\n"));
  LOWPOWER_DEBUG(("now = %u\n", now));

  for (t = g_timer_header, p = NULL; t != NULL; t = n) {
    n = t->next;
    if (t->enable == 0) {
      p = t;
      continue;
    }

    msec = now - t->timeout;
    ticks = msec / TIMEOUT_TICK;
    ticks = ticks / t->clock_max;

    for (i = 0; i < ticks; i++) {
      /* remove timer_entry form list */
      if (t->next_tick == NULL) {
        handle_timer_and_free(&t, &n, p);
        break;
      }
      timeout_handler(t, now);
      PBUF_CHECK_FREE_OOSEQ();
    }
    if (t != NULL) {
      p = t;
    }
  }
}

void
tcpip_timeouts_mbox_fetch(sys_mbox_t *mbox, void **msg)
{
  u32_t sleeptime;
  u32_t ret;
  u32_t now;

again:
  if (g_timer_header == NULL) {
    UNLOCK_TCPIP_CORE();
    (void)sys_arch_mbox_fetch_ext(mbox, msg, 0, 0);
    LOCK_TCPIP_CORE();
    return;
  }

  set_timer_state(LOW_TMR_GETING_TICKS, 0);
  sleeptime = get_sleep_time(sys_now());
  sleeptime = sleeptime > sys_timeout_get_wake_time() ? sys_timeout_get_wake_time() : sleeptime;
  set_timer_state(LOW_TMR_TIMER_WAITING, sleeptime);

  sys_timeout_set_wake_time(TIMEOUT_MAX);
  UNLOCK_TCPIP_CORE();
  ret = sys_arch_mbox_fetch_ext(mbox, msg, sleeptime, 0);
  LOCK_TCPIP_CORE();
  set_timer_state(LOW_TMR_TIMER_HANDLING, 0);
  now = sys_now();
  if ((now - g_last_check_timeout) >= TIMEOUT_CHECK) {
    check_timeout(sys_now());
    g_last_check_timeout = sys_now();
  }

  if (ret == SYS_ARCH_TIMEOUT) {
    goto again;
  }
}

void
lowpower_cycle_tmr(void *args)
{
  struct timer_handler *handler = (struct timer_handler *)args;

  handler->handler();
}

err_t
set_timer_interval(u8_t i, u32_t interval)
{
  if (i >= LWIP_ARRAYSIZE(lowpower_timer_handler)) {
    return -1;
  }
  return sys_timeout_reg(interval, lowpower_cycle_tmr,
                         (void *)(&lowpower_timer_handler[i]),
#if LOWPOWER_TIMER_DEBUG
                         lowpower_timer_handler[i].name,
#endif
                         lowpower_timer_handler[i].next_tick);
}

/* registed when init */
void
tcp_timer_needed(void)
{}

void
sys_timeouts_init(void)
{
  u8_t i;

  for (i = 0; i < LWIP_ARRAYSIZE(lowpower_timer_handler); i++) {
    if (set_timer_interval(i, lowpower_timer_handler[i].interval) != 0) {
      LOWPOWER_DEBUG(("ERROR:regist timer faild! i = %u\n", i));
    }
  }
}

void
sys_untimeout(sys_timeout_handler handler, void *arg)
{
  struct timer_entry *t = NULL;
  struct timer_entry *p = NULL;
  struct timer_entry *n = NULL;

  for (t = g_timer_header, p = NULL; t != NULL; t = n) {
    n = t->next;
    if ((t->handler == handler) && (t->args == arg)) {
      if (p == NULL) {
        g_timer_header = t->next;
      } else {
        p->next = t->next;
      }
      t->next = NULL;
      memp_free(MEMP_SYS_TIMEOUT, t);
      t = NULL;
    }
    if (t != NULL) {
      p = t;
    }
  }
}
void
sys_restart_timeouts(void)
{}

err_t
sys_timeout(u32_t msecs, sys_timeout_handler handler, void *arg)
{
  return sys_timeout_reg(msecs, handler, arg,
#if LOWPOWER_TIMER_DEBUG
                         NULL,
#endif
                         NULL);
}

void
sys_timeout_set_wake_time(u32_t val)
{
  g_wake_up_time = val;
}

u32_t
sys_timeout_get_wake_time(void)
{
  return g_wake_up_time;
}

void
set_lowpower_mod(enum lowpower_mod sw)
{
  g_lowpower_switch = sw;
}

enum lowpower_mod
get_lowpowper_mod(void)
{
  return g_lowpower_switch;
}
#endif /* LWIP_LOWPOWER */
