/**
 * @file
 * Sequential API Main thread module
 *
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

#if !NO_SYS /* don't build if not configured for use in lwipopts.h */

#include "lwip/priv/tcpip_priv.h"
#include "lwip/sys.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/pbuf.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"

#if defined(LWIP_HI3861_THREAD_SLEEP) && LWIP_HI3861_THREAD_SLEEP
#include <hi_cpu.h>
#include <hi_task.h>
#endif

#if LWIP_L3_EVENT_MSG
static void tcpip_l3_event_msg_clear(enum l3_event_msg_type type, void *event_msg);
#endif

#define TCPIP_MSG_VAR_REF(name)     API_VAR_REF(name)
#define TCPIP_MSG_VAR_DECLARE(name) API_VAR_DECLARE(struct tcpip_msg, name)
#define TCPIP_MSG_VAR_ALLOC(name)   API_VAR_ALLOC(struct tcpip_msg, MEMP_TCPIP_MSG_API, name, ERR_MEM)
#define TCPIP_MSG_VAR_FREE(name)    API_VAR_FREE(MEMP_TCPIP_MSG_API, name)

/* global variables */
static tcpip_init_done_fn tcpip_init_done;
static void *tcpip_init_done_arg;

volatile int tcpip_init_finish = 0;

#if LWIP_TCPIP_CORE_LOCKING
/** The global semaphore to lock the stack. */
sys_mutex_t lock_tcpip_core;
#endif /* LWIP_TCPIP_CORE_LOCKING */

static void tcpip_thread_handle_msg(struct tcpip_msg *msg);

static sys_mbox_t mbox;

#if LWIP_TIMERS
/* wait for a message, timeouts are processed while waiting */
#define TCPIP_MBOX_FETCH(mbox, msg) tcpip_timeouts_mbox_fetch(mbox, msg)
#else /* LWIP_TIMERS */
/* wait for a message with timers disabled (e.g. pass a timer-check trigger into tcpip_thread) */
#define TCPIP_MBOX_FETCH(mbox, msg) do { \
    UNLOCK_TCPIP_CORE(); \
    sys_mbox_fetch(mbox, msg); \
    LOCK_TCPIP_CORE(); \
  } while (0)

#endif /* LWIP_TIMERS */

#define TCPIP_MBOX_POST(mbox, msg)  sys_mbox_post(mbox, msg)
#define TCPIP_MBOX_TRYPOST(mbox, msg) sys_mbox_trypost(mbox, msg)
#define MBOX mbox
#define MBOXPTR &mbox

#if LWIP_TIMERS && (!LWIP_LOWPOWER)
/**
 * Wait (forever) for a message to arrive in an mbox.
 * While waiting, timeouts are processed.
 *
 * @param mbox the mbox to fetch the message from
 * @param msg the place to store the message
 */
static void
tcpip_timeouts_mbox_fetch(sys_mbox_t *tbox, void **msg)
{
  u32_t sleeptime, res;

again:
  LWIP_ASSERT_CORE_LOCKED();

  sleeptime = sys_timeouts_sleeptime();
  if (sleeptime == SYS_TIMEOUTS_SLEEPTIME_INFINITE) {
    UNLOCK_TCPIP_CORE();
    sys_arch_mbox_fetch(tbox, msg, 0);
    LOCK_TCPIP_CORE();
    return;
  } else if (sleeptime == 0) {
    sys_check_timeouts();
    /* We try again to fetch a message from the mbox. */
    goto again;
  }

  UNLOCK_TCPIP_CORE();
  res = sys_arch_mbox_fetch(tbox, msg, sleeptime);
  LOCK_TCPIP_CORE();
  if (res == SYS_ARCH_TIMEOUT) {
    /* If a SYS_ARCH_TIMEOUT value is returned, a timeout occurred
       before a message could be fetched. */
    sys_check_timeouts();
    /* We try again to fetch a message from the mbox. */
    goto again;
  }
}
#endif /* !LWIP_TIMERS */

/**
 * The main lwIP thread. This thread has exclusive access to lwIP core functions
 * (unless access to them is not locked). Other threads communicate with this
 * thread using message boxes.
 *
 * It also starts all the timers to make sure they are running in the right
 * thread context.
 *
 * @param arg unused argument
 */
static void
tcpip_thread(void *arg)
{
  struct tcpip_msg *msg;
  LWIP_UNUSED_ARG(arg);

  LWIP_MARK_TCPIP_THREAD();

  LOCK_TCPIP_CORE();
  if (tcpip_init_done != NULL) {
    tcpip_init_done(tcpip_init_done_arg);
  }

  while (1) {                          /* MAIN Loop */
#if defined(LWIP_HI3861_THREAD_SLEEP) && LWIP_HI3861_THREAD_SLEEP
    UNLOCK_TCPIP_CORE();
    hi_cpup_load_check_proc(hi_task_get_current_id(), LOAD_SLEEP_TIME_DEFAULT);
    LOCK_TCPIP_CORE();
#endif
    LWIP_TCPIP_THREAD_ALIVE();
    /* wait for a message, timeouts are processed while waiting */
    TCPIP_MBOX_FETCH(MBOXPTR, (void **)&msg);
    if (msg == NULL) {
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: invalid message: NULL\n"));
      LWIP_ASSERT("tcpip_thread: invalid message", 0);
      continue;
    }
    tcpip_thread_handle_msg(msg);
  }
}

/* Handle a single tcpip_msg
 * This is in its own function for access by tests only.
 */
static void
tcpip_thread_handle_msg(struct tcpip_msg *msg)
{
  switch (msg->type) {
#if !LWIP_TCPIP_CORE_LOCKING
    case TCPIP_MSG_API:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: API message %p\n", (void *)msg));
      msg->msg.api_msg.function(msg->msg.api_msg.msg);
      break;
#endif /* !LWIP_TCPIP_CORE_LOCKING */
#if ((!LWIP_TCPIP_CORE_LOCKING) || LWIP_API_MESH)
#if !LWIP_TCPIP_CORE_LOCKING
    case TCPIP_MSG_API_CALL:
#endif /* !LWIP_TCPIP_CORE_LOCKING */
#if LWIP_API_MESH
    case TCPIP_MSG_LL_EVENT_CALL:
#endif /* LWIP_API_MESH */
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: API CALL message %p\n", (void *)msg));
      msg->msg.api_call.arg->err = msg->msg.api_call.function(msg->msg.api_call.arg);
      sys_sem_signal(msg->msg.api_call.sem);
      break;
#endif /* ((!LWIP_TCPIP_CORE_LOCKING) || LWIP_API_MESH) */

#if !LWIP_TCPIP_CORE_LOCKING_INPUT
    case TCPIP_MSG_INPKT:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: PACKET %p\n", (void *)msg));
      if (msg->msg.inp.input_fn(msg->msg.inp.p, msg->msg.inp.netif) != ERR_OK) {
        pbuf_free(msg->msg.inp.p);
      }
      memp_free(MEMP_TCPIP_MSG_INPKT, msg);
      break;
#endif /* !LWIP_TCPIP_CORE_LOCKING_INPUT */

#if LWIP_TCPIP_TIMEOUT && LWIP_TIMERS
    case TCPIP_MSG_TIMEOUT:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: TIMEOUT %p\n", (void *)msg));
      sys_timeout(msg->msg.tmo.msecs, msg->msg.tmo.h, msg->msg.tmo.arg);
      memp_free(MEMP_TCPIP_MSG_API, msg);
      break;
    case TCPIP_MSG_UNTIMEOUT:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: UNTIMEOUT %p\n", (void *)msg));
      sys_untimeout(msg->msg.tmo.h, msg->msg.tmo.arg);
      memp_free(MEMP_TCPIP_MSG_API, msg);
      break;
#endif /* LWIP_TCPIP_TIMEOUT && LWIP_TIMERS */

    case TCPIP_MSG_CALLBACK:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: CALLBACK %p\n", (void *)msg));
      msg->msg.cb.function(msg->msg.cb.ctx);
      memp_free(MEMP_TCPIP_MSG_API, msg);
      break;

    case TCPIP_MSG_CALLBACK_STATIC:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: CALLBACK_STATIC %p\n", (void *)msg));
      msg->msg.cb.function(msg->msg.cb.ctx);
      break;
#if LWIP_LOWPOWER
    /* just wake up thread do nothing */
    case TCPIP_MSG_NA:
      if (msg->msg.lowpower.type == LOW_BLOCK) {
        LOWPOWER_SIGNAL(msg->msg.lowpower.wait_up);
      } else {
        memp_free(MEMP_TCPIP_MSG_LOWPOWER, msg);
      }
      sys_timeout_set_wake_time(LOW_TMR_DELAY);
      break;
#endif
#if LWIP_L3_EVENT_MSG
    case TCPIP_L3_EVENT_MSG:
      /* here we will invoke callback function directly */
      l3_invoke_msg_callback(msg->msg.l3_event_msg.type, msg->msg.l3_event_msg.msg);
      tcpip_l3_event_msg_clear(msg->msg.l3_event_msg.type, msg->msg.l3_event_msg.msg);
      memp_free(MEMP_L3_EVENT_MSG, msg);
      break;
#endif
    default:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: invalid message: %d\n", msg->type));
      LWIP_ASSERT("tcpip_thread: invalid message", 0);
      break;
  }
}

#if LWIP_LOWPOWER
/* send a na msg to wake up tcpip_thread */
void
tcpip_send_msg_na(enum lowpower_msg_type type)
{
  struct tcpip_msg *msg = NULL;
  err_t val;

  /* is not used lowpower mode */
  if ((type != LOW_FORCE_NON_BLOCK) && (get_lowpowper_mod() == LOW_TMR_NORMAL_MOD)) {
    return;
  }
  if (sys_timeout_waiting_long() == 0) {
    return;
  }

  msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_LOWPOWER);
  if (msg == NULL) {
    LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_send_msg_na alloc faild\n"));
    return;
  }
  if (!sys_dual_mbox_valid(MBOXPTR)) {
    LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_send_msg_na mbox invalid\n"));
    memp_free(MEMP_TCPIP_MSG_LOWPOWER, msg);
    return;
  }

  /* just wake up thread if nonblock */
  msg->type = TCPIP_MSG_NA;
  msg->msg.lowpower.type = type;

  if (type == LOW_BLOCK) {
    LOWPOWER_SEM_NEW(msg->msg.lowpower.wait_up, val);
    if (val != ERR_OK) {
      LWIP_DEBUGF(TCPIP_DEBUG, ("alloc sem faild\n"));
      memp_free(MEMP_TCPIP_MSG_LOWPOWER, msg);
      return;
    }
  }

  if (TCPIP_MBOX_TRYPOST(MBOXPTR, msg) != ERR_OK) {
    if (type == LOW_BLOCK) {
      LOWPOWER_SEM_FREE(msg->msg.lowpower.wait_up);
    }
    memp_free(MEMP_TCPIP_MSG_LOWPOWER, msg);
    LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_send_msg_na post faild\n"));
    return;
  }

  if (type == LOW_BLOCK) {
    LOWPOWER_SEM_WAIT(msg->msg.lowpower.wait_up);
    LOWPOWER_SEM_FREE(msg->msg.lowpower.wait_up);
    memp_free(MEMP_TCPIP_MSG_LOWPOWER, msg);
  }
}
#endif /* LWIP_LOWPOWER */

#ifdef TCPIP_THREAD_TEST
/** Work on queued items in single-threaded test mode */
int
tcpip_thread_poll_one(void)
{
  int ret = 0;
  struct tcpip_msg *msg;

  if (sys_arch_mbox_tryfetch(MBOXPTR, (void **)&msg) != SYS_ARCH_TIMEOUT) {
    LOCK_TCPIP_CORE();
    if (msg != NULL) {
      tcpip_thread_handle_msg(msg);
      ret = 1;
    }
    UNLOCK_TCPIP_CORE();
  }
  return ret;
}
#endif

#if LWIP_L3_EVENT_MSG
static void
tcpip_l3_event_msg_clear(enum l3_event_msg_type type, void *event_msg)
{
#if LWIP_ROUTE_CHANGE_MSG || LWIP_NAT64_CHANGE_MSG
  switch (type) {
#if LWIP_ROUTE_CHANGE_MSG
    case L3_EVENT_MSG_ROUTE_CHANGE:
    case L3_EVENT_MSG_ROUTE_ADD:
    case L3_EVENT_MSG_ROUTE_UPDATE:
    case L3_EVENT_MSG_ROUTE_DEL:
#endif
#if LWIP_NAT64_CHANGE_MSG
    case L3_EVENT_MSG_NAT64_ADD:
    case L3_EVENT_MSG_NAT64_DEL:
    case L3_EVENT_MSG_NAT64_UPDATE:
#endif
      mem_free(event_msg);
      break;
    default:
      break;
  }
#else
  (void)type;
  (void)event_msg;
#endif
}

void
tcpip_send_msg_l3_event(enum l3_event_msg_type type, void *rinfo)
{
  struct tcpip_msg *msg = NULL;
  msg = (struct tcpip_msg *)memp_malloc(MEMP_L3_EVENT_MSG);
  if (msg == NULL) {
    LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_send_msg_l3_event alloc faild\n"));
    tcpip_l3_event_msg_clear(type, rinfo);
    return;
  }

  msg->type = TCPIP_L3_EVENT_MSG;
  msg->msg.l3_event_msg.type = type;
  msg->msg.l3_event_msg.msg = rinfo;
  /* post the well-structed messge to the mbox */
  if (TCPIP_MBOX_TRYPOST(MBOXPTR, msg) != ERR_OK) {
    tcpip_l3_event_msg_clear(type, rinfo);
    memp_free(MEMP_L3_EVENT_MSG, msg);
    LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_send_msg_l3_event post faild\n"));
    return;
  }
}
#endif /* LWIP_L3_EVENT_MSG */

/**
 * Pass a received packet to tcpip_thread for input processing
 *
 * @param p the received packet
 * @param inp the network interface on which the packet was received
 * @param input_fn input function to call
 */
err_t
tcpip_inpkt(struct pbuf *p, struct netif *inp, netif_input_fn input_fn)
{
#if LWIP_TCPIP_CORE_LOCKING_INPUT
  err_t ret;
#else /* LWIP_TCPIP_CORE_LOCKING_INPUT */
  struct tcpip_msg *msg;
#endif

#if LWIP_TCPIP_CORE_LOCKING_INPUT
  LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_inpkt: PACKET %p/%p\n", (void *)p, (void *)inp));
#if LWIP_LOWPOWER
  tcpip_send_msg_na(LOW_BLOCK);
#endif
  LOCK_TCPIP_CORE();
  ret = input_fn(p, inp);
  UNLOCK_TCPIP_CORE();
  return ret;
#else
  LWIP_ASSERT("Invalid mbox", sys_mbox_valid_val(MBOX));
  if (!sys_mbox_valid_val(MBOX)) {
    return ERR_VAL;
  }

  msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_INPKT);
  if (msg == NULL) {
    return ERR_MEM;
  }

  msg->type = TCPIP_MSG_INPKT;
  msg->msg.inp.p = p;
  msg->msg.inp.netif = inp;
  msg->msg.inp.input_fn = input_fn;
  if (TCPIP_MBOX_TRYPOST(MBOXPTR, msg) != ERR_OK) {
    memp_free(MEMP_TCPIP_MSG_INPKT, msg);
    return ERR_MEM;
  }
  return ERR_OK;
#endif /* LWIP_TCPIP_CORE_LOCKING_INPUT */
}

#if LWIP_API_MESH
err_t
tcpip_linklayer_event_call(tcpip_api_call_fn fn, struct tcpip_api_call_data *call)
{
  err_t err;

  TCPIP_MSG_VAR_DECLARE(msg);

  if (!sys_mbox_valid_val(MBOX)) {
    return ERR_VAL;
  }
  if (call == NULL) {
    return ERR_VAL;
  }
#if !LWIP_NETCONN_SEM_PER_THREAD
  err = sys_sem_new(&call->sem, 0);
  if (err != ERR_OK) {
    return err;
  }
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

  TCPIP_MSG_VAR_ALLOC(msg);
  TCPIP_MSG_VAR_REF(msg).type = TCPIP_MSG_LL_EVENT_CALL;
  TCPIP_MSG_VAR_REF(msg).msg.api_call.arg = call;
  TCPIP_MSG_VAR_REF(msg).msg.api_call.function = fn;
#if LWIP_NETCONN_SEM_PER_THREAD
  TCPIP_MSG_VAR_REF(msg).msg.api_call.sem = LWIP_NETCONN_THREAD_SEM_GET();
#else /* LWIP_NETCONN_SEM_PER_THREAD */
  TCPIP_MSG_VAR_REF(msg).msg.api_call.sem = &call->sem;
#endif /* LWIP_NETCONN_SEM_PER_THREAD */
  TCPIP_MBOX_POST(MBOXPTR, &TCPIP_MSG_VAR_REF(msg));
  (void)sys_arch_sem_wait(TCPIP_MSG_VAR_REF(msg).msg.api_call.sem, 0);
  TCPIP_MSG_VAR_FREE(msg);

#if !LWIP_NETCONN_SEM_PER_THREAD
  sys_sem_free(&call->sem);
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

  return call->err;
}
#endif /* LWIP_API_MESH */

/**
 * @ingroup lwip_os
 * Pass a received packet to tcpip_thread for input processing with
 * ethernet_input or ip_input. Don't call directly, pass to netif_add()
 * and call netif->input().
 *
 * @param p the received packet, p->payload pointing to the Ethernet header or
 *          to an IP header (if inp doesn't have NETIF_FLAG_ETHARP or
 *          NETIF_FLAG_ETHERNET flags)
 * @param inp the network interface on which the packet was received
 */
err_t
tcpip_input(struct pbuf *p, struct netif *inp)
{
#if LWIP_ETHERNET
  if (inp->flags & (NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET)) {
    return tcpip_inpkt(p, inp, ethernet_input_list);
  } else
#endif /* LWIP_ETHERNET */
    return tcpip_inpkt(p, inp, ip_input);
}

/**
 * @ingroup lwip_os
 * Call a specific function in the thread context of
 * tcpip_thread for easy access synchronization.
 * A function called in that way may access lwIP core code
 * without fearing concurrent access.
 * Blocks until the request is posted.
 * Must not be called from interrupt context!
 *
 * @param function the function to call
 * @param ctx parameter passed to f
 * @return ERR_OK if the function was called, another err_t if not
 *
 * @see tcpip_try_callback
 */
err_t
tcpip_callback(tcpip_callback_fn function, void *ctx)
{
  struct tcpip_msg *msg;
  if (!tcpip_init_finish) {
    return ERR_IF;
  }

  LWIP_ASSERT("Invalid mbox", sys_mbox_valid_val(MBOX));

  msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_API);
  if (msg == NULL) {
    return ERR_MEM;
  }

  msg->type = TCPIP_MSG_CALLBACK;
  msg->msg.cb.function = function;
  msg->msg.cb.ctx = ctx;

  TCPIP_MBOX_POST(MBOXPTR, msg);
  return ERR_OK;
}

/**
 * @ingroup lwip_os
 * Call a specific function in the thread context of
 * tcpip_thread for easy access synchronization.
 * A function called in that way may access lwIP core code
 * without fearing concurrent access.
 * Does NOT block when the request cannot be posted because the
 * MBOX is full, but returns ERR_MEM instead.
 * Can be called from interrupt context.
 *
 * @param function the function to call
 * @param ctx parameter passed to f
 * @return ERR_OK if the function was called, another err_t if not
 *
 * @see tcpip_callback
 */
err_t
tcpip_try_callback(tcpip_callback_fn function, void *ctx)
{
  struct tcpip_msg *msg;

  LWIP_ASSERT("Invalid mbox", sys_mbox_valid_val(MBOX));

  msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_API);
  if (msg == NULL) {
    return ERR_MEM;
  }

  msg->type = TCPIP_MSG_CALLBACK;
  msg->msg.cb.function = function;
  msg->msg.cb.ctx = ctx;

  if (TCPIP_MBOX_TRYPOST(MBOXPTR, msg) != ERR_OK) {
    memp_free(MEMP_TCPIP_MSG_API, msg);
    return ERR_MEM;
  }
  return ERR_OK;
}

#if LWIP_TCPIP_TIMEOUT && LWIP_TIMERS
/**
 * call sys_timeout in tcpip_thread
 *
 * @param msecs time in milliseconds for timeout
 * @param h function to be called on timeout
 * @param arg argument to pass to timeout function h
 * @return ERR_MEM on memory error, ERR_OK otherwise
 */
err_t
tcpip_timeout(u32_t msecs, sys_timeout_handler h, void *arg)
{
  struct tcpip_msg *msg;

  LWIP_ASSERT("Invalid mbox", sys_mbox_valid_val(MBOX));

  msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_API);
  if (msg == NULL) {
    return ERR_MEM;
  }

  msg->type = TCPIP_MSG_TIMEOUT;
  msg->msg.tmo.msecs = msecs;
  msg->msg.tmo.h = h;
  msg->msg.tmo.arg = arg;
  TCPIP_MBOX_POST(MBOXPTR, msg);
  return ERR_OK;
}

/**
 * call sys_untimeout in tcpip_thread
 *
 * @param h function to be called on timeout
 * @param arg argument to pass to timeout function h
 * @return ERR_MEM on memory error, ERR_OK otherwise
 */
err_t
tcpip_untimeout(sys_timeout_handler h, void *arg)
{
  struct tcpip_msg *msg;

  LWIP_ASSERT("Invalid mbox", sys_mbox_valid_val(MBOX));

  msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_API);
  if (msg == NULL) {
    return ERR_MEM;
  }

  msg->type = TCPIP_MSG_UNTIMEOUT;
  msg->msg.tmo.h = h;
  msg->msg.tmo.arg = arg;
  TCPIP_MBOX_POST(MBOXPTR, msg);
  return ERR_OK;
}
#endif /* LWIP_TCPIP_TIMEOUT && LWIP_TIMERS */


/**
 * Sends a message to TCPIP thread to call a function. Caller thread blocks on
 * on a provided semaphore, which ist NOT automatically signalled by TCPIP thread,
 * this has to be done by the user.
 * It is recommended to use LWIP_TCPIP_CORE_LOCKING since this is the way
 * with least runtime overhead.
 *
 * @param fn function to be called from TCPIP thread
 * @param apimsg argument to API function
 * @param sem semaphore to wait on
 * @return ERR_OK if the function was called, another err_t if not
 */
err_t
tcpip_send_msg_wait_sem(tcpip_callback_fn fn, void *apimsg, sys_sem_t *sem)
{
#if LWIP_TCPIP_CORE_LOCKING
  LWIP_UNUSED_ARG(sem);
#if LWIP_LOWPOWER
  tcpip_send_msg_na(LOW_BLOCK);
#endif
  LOCK_TCPIP_CORE();
  fn(apimsg);
  UNLOCK_TCPIP_CORE();
  return ERR_OK;
#else /* LWIP_TCPIP_CORE_LOCKING */
  TCPIP_MSG_VAR_DECLARE(msg);

  LWIP_ASSERT("semaphore not initialized", sys_sem_valid(sem));
  LWIP_ASSERT("Invalid mbox", sys_mbox_valid_val(MBOX));

  TCPIP_MSG_VAR_ALLOC(msg);
  TCPIP_MSG_VAR_REF(msg).type = TCPIP_MSG_API;
  TCPIP_MSG_VAR_REF(msg).msg.api_msg.function = fn;
  TCPIP_MSG_VAR_REF(msg).msg.api_msg.msg = apimsg;
  TCPIP_MBOX_POST(MBOXPTR, &TCPIP_MSG_VAR_REF(msg));
  sys_arch_sem_wait(sem, 0);
  TCPIP_MSG_VAR_FREE(msg);
  return ERR_OK;
#endif /* LWIP_TCPIP_CORE_LOCKING */
}

/**
 * Synchronously calls function in TCPIP thread and waits for its completion.
 * It is recommended to use LWIP_TCPIP_CORE_LOCKING (preferred) or
 * LWIP_NETCONN_SEM_PER_THREAD.
 * If not, a semaphore is created and destroyed on every call which is usually
 * an expensive/slow operation.
 * @param fn Function to call
 * @param call Call parameters
 * @return Return value from tcpip_api_call_fn
 */
err_t
tcpip_api_call(tcpip_api_call_fn fn, struct tcpip_api_call_data *call)
{
#if LWIP_TCPIP_CORE_LOCKING
  err_t err;
#if LWIP_LOWPOWER
  tcpip_send_msg_na(LOW_BLOCK);
#endif
  LOCK_TCPIP_CORE();
  err = fn(call);
  UNLOCK_TCPIP_CORE();
  return err;
#else /* LWIP_TCPIP_CORE_LOCKING */
  TCPIP_MSG_VAR_DECLARE(msg);

#if !LWIP_NETCONN_SEM_PER_THREAD
  err = sys_sem_new(&call->sem, 0);
  if (err != ERR_OK) {
    return err;
  }
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

  LWIP_ASSERT("Invalid mbox", sys_mbox_valid_val(MBOX));

  TCPIP_MSG_VAR_ALLOC(msg);
  TCPIP_MSG_VAR_REF(msg).type = TCPIP_MSG_API_CALL;
  TCPIP_MSG_VAR_REF(msg).msg.api_call.arg = call;
  TCPIP_MSG_VAR_REF(msg).msg.api_call.function = fn;
#if LWIP_NETCONN_SEM_PER_THREAD
  TCPIP_MSG_VAR_REF(msg).msg.api_call.sem = LWIP_NETCONN_THREAD_SEM_GET();
#else /* LWIP_NETCONN_SEM_PER_THREAD */
  TCPIP_MSG_VAR_REF(msg).msg.api_call.sem = &call->sem;
#endif /* LWIP_NETCONN_SEM_PER_THREAD */
  TCPIP_MBOX_POST(MBOXPTR, &TCPIP_MSG_VAR_REF(msg));
  sys_arch_sem_wait(TCPIP_MSG_VAR_REF(msg).msg.api_call.sem, 0);
  TCPIP_MSG_VAR_FREE(msg);

#if !LWIP_NETCONN_SEM_PER_THREAD
  sys_sem_free(&call->sem);
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

  return call->err;
#endif /* LWIP_TCPIP_CORE_LOCKING */
}

/**
 * @ingroup lwip_os
 * Allocate a structure for a static callback message and initialize it.
 * The message has a special type such that lwIP never frees it.
 * This is intended to be used to send "static" messages from interrupt context,
 * e.g. the message is allocated once and posted several times from an IRQ
 * using tcpip_callbackmsg_trycallback().
 * Example usage: Trigger execution of an ethernet IRQ DPC routine in lwIP thread context.
 * 
 * @param function the function to call
 * @param ctx parameter passed to function
 * @return a struct pointer to pass to tcpip_callbackmsg_trycallback().
 *
 * @see tcpip_callbackmsg_trycallback()
 * @see tcpip_callbackmsg_delete()
 */
struct tcpip_callback_msg *
tcpip_callbackmsg_new(tcpip_callback_fn function, void *ctx)
{
  struct tcpip_msg *msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_API);
  if (msg == NULL) {
    return NULL;
  }
  msg->type = TCPIP_MSG_CALLBACK_STATIC;
  msg->msg.cb.function = function;
  msg->msg.cb.ctx = ctx;
  return (struct tcpip_callback_msg *)msg;
}

/**
 * @ingroup lwip_os
 * Free a callback message allocated by tcpip_callbackmsg_new().
 *
 * @param msg the message to free
 *
 * @see tcpip_callbackmsg_new()
 */
void
tcpip_callbackmsg_delete(struct tcpip_callback_msg *msg)
{
  memp_free(MEMP_TCPIP_MSG_API, msg);
}

/**
 * @ingroup lwip_os
 * Try to post a callback-message to the tcpip_thread MBOX.
 *
 * @param msg pointer to the message to post
 * @return sys_mbox_trypost() return code
 *
 * @see tcpip_callbackmsg_new()
 */
err_t
tcpip_callbackmsg_trycallback(struct tcpip_callback_msg *msg)
{
  LWIP_ASSERT("Invalid mbox", sys_mbox_valid_val(MBOX));
  return TCPIP_MBOX_TRYPOST(MBOXPTR, msg);
}

/**
 * @ingroup lwip_os
 * Try to post a callback-message to the tcpip_thread mbox.
 * Same as @ref tcpip_callbackmsg_trycallback but calls sys_mbox_trypost_fromisr(),
 * mainly to help FreeRTOS, where calls differ between task level and ISR level.
 *
 * @param msg pointer to the message to post
 * @return sys_mbox_trypost_fromisr() return code (without change, so this
 *         knowledge can be used to e.g. propagate "bool needs_scheduling")
 *
 * @see tcpip_callbackmsg_new()
 */
err_t
tcpip_callbackmsg_trycallback_fromisr(struct tcpip_callback_msg *msg)
{
  LWIP_ASSERT("Invalid mbox", sys_mbox_valid_val(MBOX));
  return sys_mbox_trypost_fromisr(MBOXPTR, msg);
}

/**
 * @ingroup lwip_os
 * Initialize this module:
 * - initialize all sub modules
 * - start the tcpip_thread
 *
 * @param initfunc a function to call when tcpip_thread is running and finished initializing
 * @param arg argument to pass to initfunc
 */
void
tcpip_init(tcpip_init_done_fn initfunc, void *arg)
{
  if (tcpip_init_finish) {
    return;
  }

  lwip_init();

  tcpip_init_done = initfunc;
  tcpip_init_done_arg = arg;

#if LWIP_TCPIP_CORE_LOCKING
  if (sys_mutex_new(&lock_tcpip_core) != ERR_OK) {
    LWIP_ASSERT("failed to create lock_tcpip_core", 0);
  }
#endif /* LWIP_TCPIP_CORE_LOCKING */

  if (sys_mbox_new(MBOXPTR, TCPIP_MBOX_SIZE) != ERR_OK) {
    LWIP_ASSERT("failed to create tcpip_thread mbox", 0);
  }

  sys_thread_new(TCPIP_THREAD_NAME, tcpip_thread, NULL, TCPIP_THREAD_STACKSIZE, TCPIP_THREAD_PRIO);

  tcpip_init_finish = 1;
}

/**
 * Simple callback function used with tcpip_callback to free a pbuf
 * (pbuf_free has a wrong signature for tcpip_callback)
 *
 * @param p The pbuf (chain) to be dereferenced.
 */
static void
pbuf_free_int(void *p)
{
  struct pbuf *q = (struct pbuf *)p;
  pbuf_free(q);
}

/**
 * A simple wrapper function that allows you to free a pbuf from interrupt context.
 *
 * @param p The pbuf (chain) to be dereferenced.
 * @return ERR_OK if callback could be enqueued, an err_t if not
 */
err_t
pbuf_free_callback(struct pbuf *p)
{
  return tcpip_try_callback(pbuf_free_int, p);
}

/**
 * A simple wrapper function that allows you to free heap memory from
 * interrupt context.
 *
 * @param m the heap memory to free
 * @return ERR_OK if callback could be enqueued, an err_t if not
 */
err_t
mem_free_callback(void *m)
{
  return tcpip_try_callback(mem_free, m);
}

#endif /* !NO_SYS */
