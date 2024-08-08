/**
 * @file
 * Transmission Control Protocol, incoming traffic
 *
 * The input processing functions of the TCP layer.
 *
 * These functions are generally called in the order (ip_input() ->)
 * tcp_input() -> * tcp_process() -> tcp_receive() (-> application).
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

#if LWIP_TCP /* don't build if not configured for use in lwipopts.h */

#include "lwip/priv/tcp_priv.h"
#if LWIP_SACK
#include "lwip/tcp_sack.h"
#endif
#include "lwip/def.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/inet_chksum.h"
#include "lwip/stats.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#if LWIP_ND6_TCP_REACHABILITY_HINTS
#include "lwip/nd6.h"
#endif /* LWIP_ND6_TCP_REACHABILITY_HINTS */

#include <string.h>

#ifdef LWIP_HOOK_FILENAME
#include LWIP_HOOK_FILENAME
#endif

/* These variables are global to all functions involved in the input
   processing of TCP segments. They are set by the tcp_input()
   function. */
static struct tcp_seg inseg;
static struct tcp_hdr *tcphdr;
static u16_t tcphdr_optlen;
static u16_t tcphdr_opt1len;
static u8_t *tcphdr_opt2;

static u32_t seqno, ackno;
static tcpwnd_size_t recv_acked;
static u16_t tcplen;
static u8_t flags;

static u8_t recv_flags;
static struct pbuf *recv_data;

struct tcp_pcb *tcp_input_pcb;

/* Forward declarations. */
static err_t tcp_process(struct tcp_pcb *pcb);
static void tcp_receive(struct tcp_pcb *pcb);
static void tcp_parseopt(struct tcp_pcb *pcb);

static void tcp_listen_input(struct tcp_pcb_listen *pcb);
static err_t tcp_timewait_input(struct tcp_pcb *pcb);

static int tcp_input_delayed_close(struct tcp_pcb *pcb);

#if LWIP_SACK
void tcp_update_sack_for_received_ooseq_segs(struct tcp_pcb *pcb);

void  tcp_parseopt_sack_permitted(struct tcp_pcb *pcb);

void tcp_pcb_reset_sack_seq(struct tcp_pcb *pcb);
#endif

#if LWIP_SACK && TCP_QUEUE_OOSEQ
static void update_ooseq_order_and_cnt(struct tcp_pcb *pcb, u32_t *order)
{
  struct tcp_seg *next = NULL;
  u32_t min_order = 0xffffffff;
  if (pcb->ooseq == NULL) {
    pcb->ooseq_cnt = 0;
  }

  *order = pcb->ooseq_cnt;
  /* RollOver check */
  if ((pcb->ooseq_cnt + 1) > pcb->ooseq_cnt) {
    pcb->ooseq_cnt++;
  } else {
    /* Find the minimum ordered packet stored in ooseq queue */
    for (next = pcb->ooseq; next != NULL; next = next->next) {
      if (next->order < min_order) {
        min_order = next->order;
      }
    }
    /* Subtract the order of every packet with the minimum order */
    for (next = pcb->ooseq; next != NULL; next = next->next) {
      next->order = next->order - min_order;
    }
    /* Subtract the ooseq_ctn with the minimum order */
    pcb->ooseq_cnt = pcb->ooseq_cnt - min_order;
    pcb->ooseq_cnt++;
  }
}
#endif

/**
 * The initial input processing of TCP. It verifies the TCP header, demultiplexes
 * the segment between the PCBs and passes it on to tcp_process(), which implements
 * the TCP finite state machine. This function is called by the IP layer (in
 * ip_input()).
 *
 * @param p received TCP segment to process (p->payload pointing to the TCP header)
 * @param inp network interface on which this segment was received
 */
void
tcp_input(struct pbuf *p, struct netif *inp)
{
  struct tcp_pcb *pcb, *prev;
  struct tcp_pcb_listen *lpcb;
#if SO_REUSE
  struct tcp_pcb *lpcb_prev = NULL;
  struct tcp_pcb_listen *lpcb_any = NULL;
#endif /* SO_REUSE */
  u8_t hdrlen_bytes;
  err_t err;

  LWIP_UNUSED_ARG(inp);
  LWIP_ASSERT_CORE_LOCKED();
  LWIP_ASSERT("tcp_input: invalid pbuf", p != NULL);

  PERF_START;

  TCP_STATS_INC(tcp.recv);
  MIB2_STATS_INC(mib2.tcpinsegs);

  tcphdr = (struct tcp_hdr *)p->payload;

#if TCP_INPUT_DEBUG
  tcp_debug_print(tcphdr);
#endif

  /* Check that TCP header fits in payload */
  if (p->len < TCP_HLEN) {
    /* drop short packets */
    LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_input: short packet (%"U16_F" bytes) discarded\n", p->tot_len));
    TCP_STATS_INC(tcp.lenerr);
    goto dropped;
  }

  /* Don't even process incoming broadcasts/multicasts. , suspected TCP LAND attacks */
  if (ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif()) ||
      ip_addr_ismulticast(ip_current_dest_addr()) ||
	  (tcphdr->src == tcphdr->dest && ip_addr_cmp(ip_current_dest_addr(), ip_current_src_addr()))) {
    TCP_STATS_INC(tcp.proterr);
    goto dropped;
  }

#if CHECKSUM_CHECK_TCP
  IF__NETIF_CHECKSUM_ENABLED(inp, NETIF_CHECKSUM_CHECK_TCP) {
    /* Verify TCP checksum. */
    u16_t chksum = ip_chksum_pseudo(p, IP_PROTO_TCP, p->tot_len,
                                    ip_current_src_addr(), ip_current_dest_addr());
    if (chksum != 0) {
      LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_input: packet discarded due to failing checksum 0x%04"X16_F"\n",
                                    chksum));
      tcp_debug_print(tcphdr);
      TCP_STATS_INC(tcp.chkerr);
      goto dropped;
    }
  }
#endif /* CHECKSUM_CHECK_TCP */

  /* sanity-check header length */
  hdrlen_bytes = TCPH_HDRLEN_BYTES(tcphdr);
  if ((hdrlen_bytes < TCP_HLEN) || (hdrlen_bytes > p->tot_len)) {
    LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_input: invalid header length (%"U16_F")\n", (u16_t)hdrlen_bytes));
    TCP_STATS_INC(tcp.lenerr);
    goto dropped;
  }

  /* Move the payload pointer in the pbuf so that it points to the
     TCP data instead of the TCP header. */
  tcphdr_optlen = (u16_t)(hdrlen_bytes - TCP_HLEN);
  tcphdr_opt2 = NULL;
  if (p->len >= hdrlen_bytes) {
    /* all options are in the first pbuf */
    tcphdr_opt1len = tcphdr_optlen;
    pbuf_remove_header(p, hdrlen_bytes); /* cannot fail */
  } else {
    u16_t opt2len;
    /* TCP header fits into first pbuf, options don't - data is in the next pbuf */
    /* there must be a next pbuf, due to hdrlen_bytes sanity check above */
    LWIP_ASSERT("p->next != NULL", p->next != NULL);

    /* advance over the TCP header (cannot fail) */
    pbuf_remove_header(p, TCP_HLEN);

    /* determine how long the first and second parts of the options are */
    tcphdr_opt1len = p->len;
    opt2len = (u16_t)(tcphdr_optlen - tcphdr_opt1len);

    /* options continue in the next pbuf: set p to zero length and hide the
        options in the next pbuf (adjusting p->tot_len) */
    pbuf_remove_header(p, tcphdr_opt1len);

    /* check that the options fit in the second pbuf */
    if (opt2len > p->next->len) {
      /* drop short packets */
      LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_input: options overflow second pbuf (%"U16_F" bytes)\n", p->next->len));
      TCP_STATS_INC(tcp.lenerr);
      goto dropped;
    }

    /* remember the pointer to the second part of the options */
    tcphdr_opt2 = (u8_t *)p->next->payload;

    /* advance p->next to point after the options, and manually
        adjust p->tot_len to keep it consistent with the changed p->next */
    pbuf_remove_header(p->next, opt2len);
    p->tot_len = (u16_t)(p->tot_len - opt2len);

    LWIP_ASSERT("p->len == 0", p->len == 0);
    LWIP_ASSERT("p->tot_len == p->next->tot_len", p->tot_len == p->next->tot_len);
  }

  /* Convert fields in TCP header to host byte order. */
  tcphdr->src = lwip_ntohs(tcphdr->src);
  tcphdr->dest = lwip_ntohs(tcphdr->dest);
  seqno = tcphdr->seqno = lwip_ntohl(tcphdr->seqno);
  ackno = tcphdr->ackno = lwip_ntohl(tcphdr->ackno);
  tcphdr->wnd = lwip_ntohs(tcphdr->wnd);

  flags = TCPH_FLAGS(tcphdr);
  tcplen = p->tot_len;
  if (flags & (TCP_FIN | TCP_SYN)) {
    tcplen++;
    if (tcplen < p->tot_len) {
      /* u16_t overflow, cannot handle this */
      LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_input: length u16_t overflow, cannot handle this\n"));
      TCP_STATS_INC(tcp.lenerr);
      goto dropped;
    }
  }

  /* Demultiplex an incoming segment. First, we check if it is destined
     for an active connection. */
  prev = NULL;

  for (pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
    LWIP_ASSERT("tcp_input: active pcb->state != CLOSED", pcb->state != CLOSED);
    LWIP_ASSERT("tcp_input: active pcb->state != TIME-WAIT", pcb->state != TIME_WAIT);
    LWIP_ASSERT("tcp_input: active pcb->state != LISTEN", pcb->state != LISTEN);

    /* check if PCB is bound to specific netif */
    if ((pcb->netif_idx != NETIF_NO_INDEX) &&
        (pcb->netif_idx != netif_get_index(ip_data.current_input_netif))) {
      prev = pcb;
      continue;
    }

    if (pcb->remote_port == tcphdr->src &&
        pcb->local_port == tcphdr->dest &&
        ip_addr_cmp(&pcb->remote_ip, ip_current_src_addr()) &&
        ip_addr_cmp(&pcb->local_ip, ip_current_dest_addr())) {
      /* Move this PCB to the front of the list so that subsequent
         lookups will be faster (we exploit locality in TCP segment
         arrivals). */
      LWIP_ASSERT("tcp_input: pcb->next != pcb (before cache)", pcb->next != pcb);
      if (prev != NULL) {
        prev->next = pcb->next;
        pcb->next = tcp_active_pcbs;
        tcp_active_pcbs = pcb;
      } else {
        TCP_STATS_INC(tcp.cachehit);
      }
      LWIP_ASSERT("tcp_input: pcb->next != pcb (after cache)", pcb->next != pcb);
      break;
    }
    prev = pcb;
  }

  if (pcb == NULL) {
    /* If it did not go to an active connection, we check the connections
       in the TIME-WAIT state. */
    for (pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next) {
      LWIP_ASSERT("tcp_input: TIME-WAIT pcb->state == TIME-WAIT", pcb->state == TIME_WAIT);

      /* check if PCB is bound to specific netif */
      if ((pcb->netif_idx != NETIF_NO_INDEX) &&
          (pcb->netif_idx != netif_get_index(ip_data.current_input_netif))) {
        continue;
      }

      if (pcb->remote_port == tcphdr->src &&
          pcb->local_port == tcphdr->dest &&
          ip_addr_cmp(&pcb->remote_ip, ip_current_src_addr()) &&
          ip_addr_cmp(&pcb->local_ip, ip_current_dest_addr())) {
        /* We don't really care enough to move this PCB to the front
           of the list since we are not very likely to receive that
           many segments for connections in TIME-WAIT. */
        LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_input: packed for TIME_WAITing connection.\n"));
#ifdef LWIP_HOOK_TCP_INPACKET_PCB
        if (LWIP_HOOK_TCP_INPACKET_PCB(pcb, tcphdr, tcphdr_optlen, tcphdr_opt1len,
                                       tcphdr_opt2, p) == ERR_OK)
#endif
        {
          if (tcp_timewait_input(pcb) == ERR_ABRT) {
            /* Peer has sent RST, so we inform the application that the
             * connection is dead before deallocating the PCB. */
            TCP_EVENT_ERR(pcb->state, pcb->errf, pcb->callback_arg, ERR_ABRT);
            tcp_pcb_remove(&tcp_tw_pcbs, pcb);
            tcp_free(pcb);
          }
        }
        pbuf_free(p);
        return;
      }
    }

    /* Finally, if we still did not get a match, we check all PCBs that
       are LISTENing for incoming connections. */
    prev = NULL;
    for (lpcb = tcp_listen_pcbs.listen_pcbs; lpcb != NULL; lpcb = lpcb->next) {
      /* check if PCB is bound to specific netif */
      if ((lpcb->netif_idx != NETIF_NO_INDEX) &&
          (lpcb->netif_idx != netif_get_index(ip_data.current_input_netif))) {
        prev = (struct tcp_pcb *)lpcb;
        continue;
      }

      if (lpcb->local_port == tcphdr->dest) {
        if (IP_IS_ANY_TYPE_VAL(lpcb->local_ip)) {
          /* found an ANY TYPE (IPv4/IPv6) match */
#if SO_REUSE
          lpcb_any = lpcb;
          lpcb_prev = prev;
#else /* SO_REUSE */
          break;
#endif /* SO_REUSE */
        } else if (IP_ADDR_PCB_VERSION_MATCH_EXACT(lpcb, ip_current_dest_addr())) {
          if (ip_addr_cmp(&lpcb->local_ip, ip_current_dest_addr())) {
            /* found an exact match */
            break;
          } else if (ip_addr_isany(&lpcb->local_ip)) {
            /* found an ANY-match */
#if SO_REUSE
            lpcb_any = lpcb;
            lpcb_prev = prev;
#else /* SO_REUSE */
            break;
#endif /* SO_REUSE */
          }
        }
      }
      prev = (struct tcp_pcb *)lpcb;
    }
#if SO_REUSE
    /* first try specific local IP */
    if (lpcb == NULL) {
      /* only pass to ANY if no specific local IP has been found */
      lpcb = lpcb_any;
      prev = lpcb_prev;
    }
#endif /* SO_REUSE */
    if (lpcb != NULL) {
      /* Move this PCB to the front of the list so that subsequent
         lookups will be faster (we exploit locality in TCP segment
         arrivals). */
      if (prev != NULL) {
        ((struct tcp_pcb_listen *)prev)->next = lpcb->next;
        /* our successor is the remainder of the listening list */
        lpcb->next = tcp_listen_pcbs.listen_pcbs;
        /* put this listening pcb at the head of the listening list */
        tcp_listen_pcbs.listen_pcbs = lpcb;
      } else {
        TCP_STATS_INC(tcp.cachehit);
      }

      LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_input: packed for LISTENing connection.\n"));
#ifdef LWIP_HOOK_TCP_INPACKET_PCB
      if (LWIP_HOOK_TCP_INPACKET_PCB((struct tcp_pcb *)lpcb, tcphdr, tcphdr_optlen,
                                     tcphdr_opt1len, tcphdr_opt2, p) == ERR_OK)
#endif
      {
        tcp_listen_input(lpcb);
      }
      pbuf_free(p);
      return;
    }
  }

#if TCP_INPUT_DEBUG
  LWIP_DEBUGF(TCP_INPUT_DEBUG, ("+-+-+-+-+-+-+-+-+-+-+-+-+-+- tcp_input: flags "));
  tcp_debug_print_flags(TCPH_FLAGS(tcphdr));
  LWIP_DEBUGF(TCP_INPUT_DEBUG, ("-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n"));
#endif /* TCP_INPUT_DEBUG */


#ifdef LWIP_HOOK_TCP_INPACKET_PCB
  if ((pcb != NULL) && LWIP_HOOK_TCP_INPACKET_PCB(pcb, tcphdr, tcphdr_optlen,
      tcphdr_opt1len, tcphdr_opt2, p) != ERR_OK) {
    pbuf_free(p);
    return;
  }
#endif
  if (pcb != NULL) {
    /* The incoming segment belongs to a connection. */
#if TCP_INPUT_DEBUG
    tcp_debug_print_state(pcb->state);
#endif /* TCP_INPUT_DEBUG */

    /* Set up a tcp_seg structure. */
    inseg.next = NULL;
    inseg.len = p->tot_len;
    inseg.p = p;
    inseg.tcphdr = tcphdr;

    recv_data = NULL;
    recv_flags = 0;
    recv_acked = 0;

    if (flags & TCP_PSH) {
      p->flags |= PBUF_FLAG_PUSH;
    }

    /* If there is data which was previously "refused" by upper layer */
    if (pcb->refused_data != NULL) {
      if ((tcp_process_refused_data(pcb) == ERR_ABRT) ||
          ((pcb->refused_data != NULL) && (tcplen > 0))) {
        /* pcb has been aborted or refused data is still refused and the new
           segment contains data */
        if (pcb->rcv_ann_wnd == 0) {
          /* this is a zero-window probe, we respond to it with current RCV.NXT
          and drop the data segment */
          tcp_send_empty_ack(pcb);
        }
        TCP_STATS_INC(tcp.drop);
        MIB2_STATS_INC(mib2.tcpinerrs);
        goto aborted;
      }
    }
    tcp_input_pcb = pcb;
    err = tcp_process(pcb);
    /* A return value of ERR_ABRT means that tcp_abort() was called
       and that the pcb has been freed. If so, we don't do anything. */
    if (err != ERR_ABRT) {
      if (recv_flags & TF_RESET) {
        /* TF_RESET means that the connection was reset by the other
           end. We then call the error callback to inform the
           application that the connection is dead before we
           deallocate the PCB. */
        TCP_EVENT_ERR(pcb->state, pcb->errf, pcb->callback_arg, err);
        tcp_pcb_remove(&tcp_active_pcbs, pcb);
        tcp_free(pcb);
      } else {
        err = ERR_OK;
        /* If the application has registered a "sent" function to be
           called when new send buffer space is available, we call it
           now. */
        if (recv_acked > 0) {
          u16_t acked16;
#if LWIP_WND_SCALE
          /* recv_acked is u32_t but the sent callback only takes a u16_t,
             so we might have to call it multiple times. */
          u32_t acked = recv_acked;
          while (acked > 0) {
            acked16 = (u16_t)LWIP_MIN(acked, 0xffffu);
            acked -= acked16;
#else
          {
            acked16 = recv_acked;
#endif
            TCP_EVENT_SENT(pcb, (u16_t)acked16, err);
            if (err == ERR_ABRT) {
              goto aborted;
            }
          }
          recv_acked = 0;
        }
        if (tcp_input_delayed_close(pcb)) {
          goto aborted;
        }
#if TCP_QUEUE_OOSEQ && LWIP_WND_SCALE
        while (recv_data != NULL) {
          struct pbuf *rest = NULL;
          pbuf_split_64k(recv_data, &rest);
#else /* TCP_QUEUE_OOSEQ && LWIP_WND_SCALE */
        if (recv_data != NULL) {
#endif /* TCP_QUEUE_OOSEQ && LWIP_WND_SCALE */

          LWIP_ASSERT("pcb->refused_data == NULL", pcb->refused_data == NULL);
          if (pcb->flags & TF_RXCLOSED) {
            /* received data although already closed -> abort (send RST) to
               notify the remote host that not all data has been processed */
            (void)pbuf_free(recv_data);
#if TCP_QUEUE_OOSEQ && LWIP_WND_SCALE
            if (rest != NULL) {
              (void)pbuf_free(rest);
            }
#endif /* TCP_QUEUE_OOSEQ && LWIP_WND_SCALE */
            tcp_abort(pcb);
            goto aborted;
          }

          /* Notify application that data has been received. */
          TCP_EVENT_RECV(pcb, recv_data, ERR_OK, err);
          if (err == ERR_ABRT) {
#if TCP_QUEUE_OOSEQ && LWIP_WND_SCALE
            if (rest != NULL) {
              (void)pbuf_free(rest);
            }
#endif /* TCP_QUEUE_OOSEQ && LWIP_WND_SCALE */
            goto aborted;
          }

          /* If the upper layer can't receive this data, store it */
          if (err != ERR_OK) {
#if TCP_QUEUE_OOSEQ && LWIP_WND_SCALE
            if (rest != NULL) {
              pbuf_cat(recv_data, rest);
            }
#endif /* TCP_QUEUE_OOSEQ && LWIP_WND_SCALE */
            pcb->refused_data = recv_data;
            LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_input: keep incoming packet, because pcb is \"full\"\n"));
#if TCP_QUEUE_OOSEQ && LWIP_WND_SCALE
            break;
          } else {
            /* Upper layer received the data, go on with the rest if > 64K */
            recv_data = rest;
#endif /* TCP_QUEUE_OOSEQ && LWIP_WND_SCALE */
          }
        }

        /* If a FIN segment was received, we call the callback
           function with a NULL buffer to indicate EOF. */
        if (recv_flags & TF_GOT_FIN) {
          if (pcb->refused_data != NULL) {
            /* Delay this if we have refused data. */
            pcb->refused_data->flags |= PBUF_FLAG_TCP_FIN;
          } else {
            /* correct rcv_wnd as the application won't call tcp_recved()
               for the FIN's seqno */
            if (pcb->rcv_wnd != TCP_WND_MAX(pcb)) {
              pcb->rcv_wnd++;
            }
            TCP_EVENT_CLOSED(pcb, err);
            if (err == ERR_ABRT) {
              goto aborted;
            }
            /*
             * Scenario: When DATA is already made the recvmbox full and FIN is received (here FIN post has failed)
             * ---- Flag tcp_pcb_flag is marked TCP_PBUF_FLAG_TCP_FIN_RECV_SYSPOST_FAIL which upon
             * tcp_fasttmr() expiry will be retried to post again.
             * Upon user calling recv() will ensure that the refused data will be given to user and
             * subsequent recv() will return 0 to user for FIN
             */
            else if (err != ERR_OK) {
              pcb->tcp_pcb_flag |= TCP_PBUF_FLAG_TCP_FIN_RECV_SYSPOST_FAIL;
            }
          }
        }

        tcp_input_pcb = NULL;
        if (tcp_input_delayed_close(pcb)) {
          goto aborted;
        }
        /* Try to send something out. */
        tcp_output(pcb);
#if TCP_INPUT_DEBUG
#if TCP_DEBUG
        tcp_debug_print_state(pcb->state);
#endif /* TCP_DEBUG */
#endif /* TCP_INPUT_DEBUG */
      }
    }
    /* Jump target if pcb has been aborted in a callback (by calling tcp_abort()).
       Below this line, 'pcb' may not be dereferenced! */
aborted:
    tcp_input_pcb = NULL;
    recv_data = NULL;

    /* give up our reference to inseg.p */
    if (inseg.p != NULL) {
      pbuf_free(inseg.p);
      inseg.p = NULL;
    }
  } else {
    /* If no matching PCB was found, send a TCP RST (reset) to the
       sender. */
    LWIP_DEBUGF(TCP_RST_DEBUG, ("tcp_input: no PCB match found, resetting.\n"));
    if (!(TCPH_FLAGS(tcphdr) & TCP_RST)) {
      TCP_STATS_INC(tcp.proterr);
      TCP_STATS_INC(tcp.drop);
      tcp_rst(NULL, ackno, seqno + tcplen, ip_current_dest_addr(),
              ip_current_src_addr(), tcphdr->dest, tcphdr->src);
    }
    pbuf_free(p);
  }

  LWIP_ASSERT("tcp_input: tcp_pcbs_sane()", tcp_pcbs_sane());
  PERF_STOP("tcp_input");
  return;
dropped:
  TCP_STATS_INC(tcp.drop);
  MIB2_STATS_INC(mib2.tcpinerrs);
  pbuf_free(p);
}

/** Called from tcp_input to check for TF_CLOSED flag. This results in closing
 * and deallocating a pcb at the correct place to ensure noone references it
 * any more.
 * @returns 1 if the pcb has been closed and deallocated, 0 otherwise
 */
static int
tcp_input_delayed_close(struct tcp_pcb *pcb)
{
  LWIP_ASSERT("tcp_input_delayed_close: invalid pcb", pcb != NULL);

  if (recv_flags & TF_CLOSED) {
    /* The connection has been closed and we will deallocate the
        PCB. */
    if (!(pcb->flags & TF_RXCLOSED)) {
      /* Connection closed although the application has only shut down the
          tx side: call the PCB's err callback and indicate the closure to
          ensure the application doesn't continue using the PCB. */
      TCP_EVENT_ERR(pcb->state, pcb->errf, pcb->callback_arg, ERR_CLSD);
    }
    tcp_pcb_remove(&tcp_active_pcbs, pcb);
    tcp_free(pcb);
    return 1;
  }
  return 0;
}

void tcp_sndbuf_init(struct tcp_pcb *pcb)
{
  u32_t sndqueuemax;
#if LWIP_SO_SNDBUF
  u32_t snd_buf = pcb->snd_buf_static;
#else
  u32_t snd_buf = pcb->snd_buf;
#endif
  tcpwnd_size_t mss = pcb->mss;

  if ((snd_buf >> 1) > ((mss << 1) + 1)) {
    if ((snd_buf >> 1) < (snd_buf - 1)) {
      pcb->snd_buf_lowat = snd_buf >> 1;
    } else {
      pcb->snd_buf_lowat = snd_buf - 1;
    }
  } else {
    if (((mss << 1) + 1) < (snd_buf - 1)) {
      pcb->snd_buf_lowat = (mss << 1) + 1;
    } else {
      pcb->snd_buf_lowat = snd_buf - 1;
    }
  }

  sndqueuemax = ((snd_buf / mss) << 3);
  if (sndqueuemax > USHRT_MAX) {
    sndqueuemax = USHRT_MAX;
  }
  pcb->snd_queuelen_max = (u16_t)sndqueuemax;
  pcb->snd_queuelen_lowat = pcb->snd_queuelen_max >> 1;
  if (pcb->snd_queuelen_lowat < TCP_SND_QUEUELEN_LOWAT_THRES) {
    pcb->snd_queuelen_lowat = LWIP_MIN(TCP_SND_QUEUELEN_LOWAT_THRES, pcb->snd_queuelen_max);
  }
}

/**
 * Called by tcp_input() when a segment arrives for a listening
 * connection (from tcp_input()).
 *
 * @param pcb the tcp_pcb_listen for which a segment arrived
 *
 * @note the segment which arrived is saved in global variables, therefore only the pcb
 *       involved is passed as a parameter to this function
 */
static void
tcp_listen_input(struct tcp_pcb_listen *pcb)
{
  struct tcp_pcb *npcb;
  u32_t iss;
  err_t rc;
  struct netif *netif = NULL;

  LWIP_UNUSED_ARG(netif);
  if (flags & TCP_RST) {
    /* An incoming RST should be ignored. Return. */
    return;
  }

  LWIP_ASSERT("tcp_listen_input: invalid pcb", pcb != NULL);

  /* In the LISTEN state, we check for incoming SYN segments,
     creates a new PCB, and responds with a SYN|ACK. */
  if (flags & TCP_ACK) {
    /* For incoming segments with the ACK flag set, respond with a
       RST. */
    LWIP_DEBUGF(TCP_RST_DEBUG, ("tcp_listen_input: ACK in LISTEN, sending reset\n"));
    tcp_rst((struct tcp_pcb *)pcb, ackno, seqno + tcplen, ip_current_dest_addr(),
            ip_current_src_addr(), tcphdr->dest, tcphdr->src);
  } else if (flags & TCP_SYN) {
    LWIP_DEBUGF(TCP_DEBUG, ("TCP connection request %"U16_F" -> %"U16_F".\n", tcphdr->src, tcphdr->dest));
#if TCP_LISTEN_BACKLOG
    if (pcb->accepts_pending >= pcb->backlog) {
      LWIP_DEBUGF(TCP_DEBUG, ("tcp_listen_input: listen backlog exceeded for port %"U16_F"\n", tcphdr->dest));
      return;
    }
#endif /* TCP_LISTEN_BACKLOG */
    npcb = tcp_alloc(pcb->prio);
    /* If a new PCB could not be created (probably due to lack of memory),
       we don't do anything, but rely on the sender will retransmit the
       SYN at a time when we have more memory available. */
    if (npcb == NULL) {
      err_t err;
      LWIP_DEBUGF(TCP_DEBUG, ("tcp_listen_input: could not allocate PCB\n"));
      TCP_STATS_INC(tcp.memerr);
      TCP_EVENT_ACCEPT(pcb, NULL, pcb->callback_arg, ERR_MEM, err);
      LWIP_UNUSED_ARG(err); /* err not useful here */
      return;
    }
#if TCP_LISTEN_BACKLOG
    pcb->accepts_pending++;
    tcp_set_flags(npcb, TF_BACKLOGPEND);
#endif /* TCP_LISTEN_BACKLOG */
    /* Set up the new PCB. */
    ip_addr_copy(npcb->local_ip, *ip_current_dest_addr());
    ip_addr_copy(npcb->remote_ip, *ip_current_src_addr());
    npcb->local_port = pcb->local_port;
    npcb->remote_port = tcphdr->src;
    npcb->state = SYN_RCVD;
    npcb->rcv_nxt = seqno + 1;
    npcb->rcv_ann_right_edge = npcb->rcv_nxt;
    iss = tcp_next_iss(npcb);
    npcb->snd_wl2 = iss;
    npcb->snd_nxt = iss;
    npcb->snd_sml = iss;
    npcb->lastack = iss;
    npcb->snd_lbb = iss;
    npcb->snd_wl1 = seqno - 1;/* initialise to seqno-1 to force window update */

    npcb->rto_end = iss;
    npcb->fast_recovery_point = iss;
#if LWIP_SO_PRIORITY
    /* Inherit the priority from the listening socket */
    npcb->priority = pcb->priority;
#endif /* LWIP_SO_PRIORITY */

#if LWIP_SO_SNDBUF
    npcb->snd_buf = pcb->snd_buf_static;
    npcb->snd_buf_static = pcb->snd_buf_static;
#endif /* LWIP_SO_SNDBUF */
    npcb->callback_arg = pcb->callback_arg;
#if LWIP_CALLBACK_API || TCP_LISTEN_BACKLOG
    npcb->listener = pcb;
#endif /* LWIP_CALLBACK_API || TCP_LISTEN_BACKLOG */
    /* inherit socket options */
    npcb->so_options = pcb->so_options & SOF_INHERITED;
    npcb->netif_idx = pcb->netif_idx;

#if LWIP_SACK
    npcb->sack_seq = NULL;
    npcb->recovery_point = npcb->snd_nxt - 1;
    npcb->pipe = 0;
    npcb->high_sacked = npcb->snd_nxt - 1;
    npcb->high_data = npcb->snd_nxt - 1;
    npcb->high_rxt = npcb->snd_nxt - 1;
    npcb->rescue_rxt = npcb->snd_nxt - 1;
#if LWIP_FACK_THRESHOLD_BASED_FR
    npcb->fack = npcb->snd_nxt - 1;
#endif /* LWIP_FACK_THRESHOLD_BASED_FR */
#if LWIP_TCP_TLP_SUPPORT
    LWIP_TCP_TLP_CLEAR_VARS(npcb);
#endif /* LWIP_TCP_TLP_SUPPORT */
#endif /* LWIP_SACK */
    /* Register the new PCB so that we can begin receiving segments
       for it. */
    TCP_REG_ACTIVE(npcb);

    /* Incoming packet information is saved on npcb, not listening pcb, so for route use new pcb */
    netif = ip_route_pcb(&npcb->remote_ip, (struct ip_pcb*)npcb);
#if DRIVER_STATUS_CHECK
    if (netif != NULL) {
      if (!(netif->flags & NETIF_FLAG_DRIVER_RDY)) {
        npcb->drv_status = DRV_NOT_READY;
      } else {
        npcb->drv_status = DRV_READY;
      }
    } else {
      /* Mark PCB's driver status as active, later  it will identify netif, and then update status for while sending
         packet out */
      npcb->drv_status = DRV_READY;
    }
#endif /* DRIVER_STATUS_CHECK */

    /* Parse any options in the SYN. */
    tcp_parseopt(npcb);
    npcb->snd_wnd = tcphdr->wnd;
    npcb->snd_wnd_max = npcb->snd_wnd;
    npcb->ssthresh = npcb->snd_wnd;

#if LWIP_SACK
    tcp_pcb_reset_sack_seq(npcb);
#endif /* LWIP_SACK */

#if TCP_CALCULATE_EFF_SEND_MSS
#if LWIP_TCP_MAXSEG
    npcb->usr_mss = pcb->usr_mss;
    u16_t mss = ((npcb->usr_mss == 0) ? (TCP_MSS) : (npcb->usr_mss));
#else
    u16_t mss = TCP_MSS;
#endif /* LWIP_TCP_MAXSEG */
    npcb->mss = tcp_eff_send_mss_netif(LWIP_MIN(mss, npcb->rcv_mss), netif, &npcb->remote_ip);
#endif /* TCP_CALCULATE_EFF_SEND_MSS */

    tcp_sndbuf_init(npcb);
    MIB2_STATS_INC(mib2.tcppassiveopens);

#if LWIP_TCP_PCB_NUM_EXT_ARGS
    if (tcp_ext_arg_invoke_callbacks_passive_open(pcb, npcb) != ERR_OK) {
      tcp_abandon(npcb, 0);
      return;
    }
#endif

    /* Send a SYN|ACK together with the MSS option. */
    rc = tcp_enqueue_flags(npcb, TCP_SYN | TCP_ACK);
    if (rc != ERR_OK) {
      tcp_abandon(npcb, 0);
      return;
    }
    tcp_output(npcb);
  }
  return;
}

/**
 * Called by tcp_input() when a segment arrives for a connection in
 * TIME_WAIT.
 *
 * @param pcb the tcp_pcb for which a segment arrived
 *
 * @note the segment which arrived is saved in global variables, therefore only the pcb
 *       involved is passed as a parameter to this function
 */
static err_t
tcp_timewait_input(struct tcp_pcb *pcb)
{
  /* RFC 1337: in TIME_WAIT, ignore RST and ACK FINs + any 'acceptable' segments */
  /* RFC 793 3.9 Event Processing - Segment Arrives:
   * - first check sequence number - we skip that one in TIME_WAIT (always
   *   acceptable since we only send ACKs)
   * - second check the RST bit (... return) */
  if (flags & TCP_RST) {
    /* TWA only if the seqno exactly matches */
    if (seqno == pcb->rcv_nxt) {
      return ERR_ABRT;
    }
  }
  LWIP_ASSERT("tcp_timewait_input: invalid pcb", pcb != NULL);

  /* - fourth, check the SYN bit, */
  if (flags & TCP_SYN) {
    /* If SYN received in TW state, send a challenge ack
     * Right now, lwIP does not support TWA if seq == rcv_nxt,
     * and move the state to established & fallback if any failure.
     * Also does not support TAW by determining age of packet via SYN timestamp,
     * In current behavior, TWA can be achieved by sending ACK, if client is same it can reply back with RST,
     * and on receiving RST, lwIP will assasinate TW pcb, and abort.
     */
    tcp_ack_now(pcb);
    (void)tcp_output(pcb);
    return ERR_OK;
  } else if (flags & TCP_FIN) {
    /* - eighth, check the FIN bit: Remain in the TIME-WAIT state.
         Restart the 2 MSL time-wait timeout.*/
    pcb->tmr = tcp_ticks;
  }

  if ((tcplen > 0)) {
    /* Acknowledge data, FIN or out-of-window SYN */
    tcp_ack_now(pcb);
    tcp_output(pcb);
  }
  return ERR_OK;
}

/**
 * Implements the TCP state machine. Called by tcp_input. In some
 * states tcp_receive() is called to receive data. The tcp_seg
 * argument will be freed by the caller (tcp_input()) unless the
 * recv_data pointer in the pcb is set.
 *
 * @param pcb the tcp_pcb for which a segment arrived
 *
 * @note the segment which arrived is saved in global variables, therefore only the pcb
 *       involved is passed as a parameter to this function
 */
static err_t
tcp_process(struct tcp_pcb *pcb)
{
  struct tcp_seg *rseg;
  u8_t acceptable = 0;
  err_t err;

  err = ERR_OK;

  LWIP_ASSERT("tcp_process: invalid pcb", pcb != NULL);

  /* Process incoming RST segments. */
  if (flags & TCP_RST) {
    /* First, determine if the reset is acceptable. */
    if (pcb->state == SYN_SENT) {
      /* "In the SYN-SENT state (a RST received in response to an initial SYN),
          the RST is acceptable if the ACK field acknowledges the SYN." */
      if (ackno == pcb->snd_nxt) {
        acceptable = 1;
      }
    } else {
      /* "In all states except SYN-SENT, all reset (RST) segments are validated
          by checking their SEQ-fields." */
      if (seqno == pcb->rcv_nxt) {
        acceptable = 1;
      } else  if (TCP_SEQ_BETWEEN(seqno, pcb->rcv_nxt,
                                  pcb->rcv_nxt + pcb->rcv_wnd)) {
        /* If the sequence number is inside the window, we send a challenge ACK
           and wait for a re-send with matching sequence number.
           This follows RFC 5961 section 3.2 and addresses CVE-2004-0230
           (RST spoofing attack), which is present in RFC 793 RST handling. */
        LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_process: RST seq fallin Rx Window, send challenge ACK \n"));
        tcp_ack_now(pcb);
      }
    }

    if (acceptable) {
      LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_process: Connection RESET\n"));
      LWIP_ASSERT("tcp_input: pcb->state != CLOSED", pcb->state != CLOSED);
      recv_flags |= TF_RESET;
      tcp_clear_flags(pcb, TF_ACK_DELAY);

      if (pcb->state == SYN_SENT) {
        return ERR_CONNREFUSED;
      } else {
        return ERR_RST;
      }
    } else {
      LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_process: unacceptable reset seqno %"U32_F" rcv_nxt %"U32_F"\n",
                                    seqno, pcb->rcv_nxt));
      LWIP_DEBUGF(TCP_DEBUG, ("tcp_process: unacceptable reset seqno %"U32_F" rcv_nxt %"U32_F"\n",
                              seqno, pcb->rcv_nxt));
      return ERR_OK;
    }
  }

  if ((flags & TCP_SYN) && (pcb->state != SYN_SENT && pcb->state != SYN_RCVD)) {
    /* Cope with new connection attempt after remote end crashed */
    LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_process: SYN recved in sync state, send challenge ACK \n"));
    tcp_ack_now(pcb);
    return ERR_OK;
  }

  if ((pcb->flags & TF_RXCLOSED) == 0) {
    /* Update the PCB (in)activity timer unless rx is closed (see tcp_shutdown) */
    pcb->tmr = tcp_ticks;
  }
  pcb->keep_cnt_sent = 0;
  pcb->persist_probe = 0;

  tcp_parseopt(pcb);

  /* Do different things depending on the TCP state. */
  switch (pcb->state) {
    case SYN_SENT:
      LWIP_DEBUGF(TCP_INPUT_DEBUG, ("SYN-SENT: ackno %"U32_F" pcb->snd_nxt %"U32_F" unacked %s %"U32_F"\n",
                                    ackno, pcb->snd_nxt, pcb->unacked ? "" : " empty:",
                                    pcb->unacked ? lwip_ntohl(pcb->unacked->tcphdr->seqno) : 0));
      /* received SYN ACK with expected sequence number? */
      if ((flags & TCP_ACK) && (flags & TCP_SYN)
          && (ackno == pcb->lastack + 1)) {
        pcb->rcv_nxt = seqno + 1;
        pcb->rcv_ann_right_edge = pcb->rcv_nxt;
        pcb->lastack = ackno;
      /* Adding for WINDOW SCALE */
      /* SYNACK will have unscaled window value */
        pcb->snd_wnd = tcphdr->wnd;
        pcb->snd_wnd_max = pcb->snd_wnd;
        pcb->snd_wl1 = seqno - 1; /* initialise to seqno - 1 to force window update */
        pcb->state = ESTABLISHED;

#if LWIP_SACK
        tcp_pcb_reset_sack_seq(pcb);
#endif

        /* Base code issue fix.

        LwIP                                             Peer
        SYN             -->
                                                  <--  SYN-ACK
        ACK             -->

        DATA(4380)  // 3 segments of 1460 -->

                                          No data is ACKed

                 <--  1) DUP ACK (using SYNACK's ACK seq number)
                 <--  2) DUP ACK (using SYNACK's ACK seq number)
                 <--  3) DUP ACK (using SYNACK's ACK seq number)

                 Ideally Now lwip should start FR, but it was not triggering.
                 Because in tcp_receive() function 1) DUP ACK was not
                 actually considered DUP ACK (it was changing the advertized
                 window clause 3 of the FR rule). This is fixed below.
       */
        /* Adding for WINDOW SCALE */
        pcb->snd_wnd = SND_WND_SCALE(pcb, tcphdr->wnd);
        /* keep tabs on the biggest window announced by the remote host to calculate
           the maximum segment size */
        if (pcb->snd_wnd_max < SND_WND_SCALE(pcb, tcphdr->wnd)) {
          pcb->snd_wnd_max = SND_WND_SCALE(pcb, tcphdr->wnd);
        }
        pcb->snd_wl2 = ackno;

#if TCP_CALCULATE_EFF_SEND_MSS
#if LWIP_TCP_MAXSEG
        u16_t mss = ((pcb->usr_mss == 0) ? (TCP_MSS) : (pcb->usr_mss));
#else
        u16_t mss = TCP_MSS;
#endif /* LWIP_TCP_MAXSEG */
        pcb->mss = tcp_eff_send_mss(LWIP_MIN(mss, pcb->rcv_mss), (struct ip_pcb *)pcb, &pcb->remote_ip);
#endif /* TCP_CALCULATE_EFF_SEND_MSS */

        TCP_CALC_SSTHRESH(pcb, pcb->ssthresh, INITIAL_SSTHRESH, pcb->mss);

        LWIP_TCP_CALC_INITIAL_CWND(pcb->mss, pcb->iw);
        pcb->cwnd = pcb->iw;

        LWIP_DEBUGF(TCP_CWND_DEBUG, ("tcp_process (SENT): cwnd %"TCPWNDSIZE_F
                                     " ssthresh %"TCPWNDSIZE_F"\n",
                                     pcb->cwnd, pcb->ssthresh));
        LWIP_ASSERT("pcb->snd_queuelen > 0", (pcb->snd_queuelen > 0));
        --pcb->snd_queuelen;
        LWIP_DEBUGF(TCP_QLEN_DEBUG, ("tcp_process: SYN-SENT --queuelen %"TCPWNDSIZE_F"\n", (tcpwnd_size_t)pcb->snd_queuelen));
        rseg = pcb->unacked;
        if (rseg == NULL) {
          /* might happen if tcp_output fails in tcp_rexmit_rto()
             in which case the segment is on the unsent list */
          rseg = pcb->unsent;
          LWIP_ASSERT("no segment to free", rseg != NULL);
          pcb->unsent = rseg->next;
        } else {
          pcb->unacked = rseg->next;
        }
        tcp_seg_free(rseg);

        /* If there's nothing left to acknowledge, stop the retransmit
           timer, otherwise reset it to start again */
        if (pcb->unacked == NULL) {
          pcb->rtime = -1;
        } else {
          pcb->rtime = 0;
        }

        /*
         * RFC6298 section 5.7
         * If the timer expires awaiting the ACK of a SYN segment and the
         * TCP implementation is using an RTO less than 3 seconds, the RTO
         * MUST be re-initialized to 3 seconds when data transmission
         * begins (i.e., after the three-way handshake completes).
         */
        if (pcb->nrtx) {
          pcb->rto = TCP_RTO_TICKS_AFTER_SYN_RTX;
          pcb->nrtx = 0;
          /* clear RTO flags, else FR/ER/FACK/TLP can not be triggered */
          tcp_clear_flags(pcb, TF_RTO);
#if LWIP_SACK
          if (tcp_is_flag_set(pcb, TF_SACK)) {
            tcp_clear_flags(pcb, TF_IN_SACK_RTO);
          }
#endif /* LWIP_SACK */
        }

        /* Call the user specified function to call when successfully
         * connected. */
        TCP_EVENT_CONNECTED(pcb, ERR_OK, err);
        if (err == ERR_ABRT) {
          return ERR_ABRT;
        }
        tcp_ack_now(pcb);
      }
      /* received ACK? possibly a half-open connection */
      else if (flags & TCP_ACK) {
        /* send a RST to bring the other side in a non-synchronized state. */
        tcp_rst(pcb, ackno, seqno + tcplen, ip_current_dest_addr(),
                ip_current_src_addr(), tcphdr->dest, tcphdr->src);
        /* Resend SYN immediately (don't wait for rto timeout) to establish
          connection faster, but do not send more SYNs than we otherwise would
          have, or we might get caught in a loop on loopback interfaces. */
        if (pcb->nrtx < TCP_SYNMAXRTX) {
          pcb->rtime = 0;
          tcp_rexmit_rto(pcb);
        }
      }
      break;
    case SYN_RCVD:
      if (flags & TCP_ACK) {
        /* expected ACK number? */
        if (TCP_SEQ_BETWEEN(ackno, pcb->lastack + 1, pcb->snd_nxt)) {
          pcb->state = ESTABLISHED;
          LWIP_DEBUGF(TCP_DEBUG, ("TCP connection established %"U16_F" -> %"U16_F".\n", inseg.tcphdr->src, inseg.tcphdr->dest));
#if LWIP_CALLBACK_API || TCP_LISTEN_BACKLOG
          if (pcb->listener == NULL) {
            /* listen pcb might be closed by now */
            err = ERR_VAL;
          } else
#endif /* LWIP_CALLBACK_API || TCP_LISTEN_BACKLOG */
          {
#if LWIP_CALLBACK_API
            LWIP_ASSERT("pcb->listener->accept != NULL", pcb->listener->accept != NULL);
#endif
            tcp_backlog_accepted(pcb);
            /* Call the accept function. */
            TCP_EVENT_ACCEPT(pcb->listener, pcb, pcb->callback_arg, ERR_OK, err);
          }
          if (err != ERR_OK) {
            /* If the accept function returns with an error, we abort
             * the connection. */
            /* Already aborted? */
            if (err != ERR_ABRT) {
              tcp_abort(pcb);
            }
            return ERR_ABRT;
          }
          /* If there was any data contained within this ACK,
           * we'd better pass it on to the application as well. */
          tcp_receive(pcb);

          /* Prevent ACK for SYN to generate a sent event */
          if (recv_acked != 0) {
            recv_acked--;
          }

          /* As per RFC6928 larger Intial window has low side effect, it can override value of RFC 2581,
            and chances of retransmission is only 0.7%--> whether to implement this part is question?

            Below changes are from RFC 5681
          **/
          LWIP_TCP_CALC_INITIAL_CWND(pcb->mss, pcb->iw);
          pcb->cwnd = pcb->iw;

          LWIP_DEBUGF(TCP_CWND_DEBUG, ("tcp_process (SYN_RCVD): cwnd %"TCPWNDSIZE_F
                                       " ssthresh %"TCPWNDSIZE_F"\n",
                                       pcb->cwnd, pcb->ssthresh));

          if (recv_flags & TF_GOT_FIN) {
            tcp_ack_now(pcb);
            pcb->state = CLOSE_WAIT;
          }
        } else {
          /* incorrect ACK number, send RST */
          tcp_rst(pcb, ackno, seqno + tcplen, ip_current_dest_addr(),
                  ip_current_src_addr(), tcphdr->dest, tcphdr->src);
        }
      } else if ((flags & TCP_SYN) && (seqno == pcb->rcv_nxt - 1)) {
        /* Looks like another copy of the SYN - retransmit our SYN-ACK */
        tcp_rexmit(pcb);
      }
      break;
    case CLOSE_WAIT:
    /* FALLTHROUGH */
    case ESTABLISHED:
      tcp_receive(pcb);
      if (recv_flags & TF_GOT_FIN) { /* passive close */
        tcp_ack_now(pcb);
        pcb->state = CLOSE_WAIT;
      }
      break;
    case FIN_WAIT_1:
      tcp_receive(pcb);
      if (recv_flags & TF_GOT_FIN) {
        if ((flags & TCP_ACK) && (ackno == pcb->snd_nxt) &&
            pcb->unsent == NULL) {
          LWIP_DEBUGF(TCP_DEBUG,
                      ("TCP connection closed: FIN_WAIT_1 %"U16_F" -> %"U16_F".\n", inseg.tcphdr->src, inseg.tcphdr->dest));
          tcp_ack_now(pcb);
          tcp_pcb_purge(pcb);
          TCP_RMV_ACTIVE(pcb);
          pcb->state = TIME_WAIT;
          TCP_REG(&tcp_tw_pcbs, pcb);
        } else {
          tcp_ack_now(pcb);
          pcb->state = CLOSING;
        }
      } else if ((flags & TCP_ACK) && (ackno == pcb->snd_nxt) &&
                 pcb->unsent == NULL) {
        pcb->state = FIN_WAIT_2;
      }
      break;
    case FIN_WAIT_2:
      tcp_receive(pcb);
      if (recv_flags & TF_GOT_FIN) {
        LWIP_DEBUGF(TCP_DEBUG, ("TCP connection closed: FIN_WAIT_2 %"U16_F" -> %"U16_F".\n", inseg.tcphdr->src, inseg.tcphdr->dest));
        tcp_ack_now(pcb);
        tcp_pcb_purge(pcb);
        TCP_RMV_ACTIVE(pcb);
        pcb->state = TIME_WAIT;
        TCP_REG(&tcp_tw_pcbs, pcb);
      }
      break;
    case CLOSING:
      tcp_receive(pcb);
      if ((flags & TCP_ACK) && ackno == pcb->snd_nxt && pcb->unsent == NULL) {
        LWIP_DEBUGF(TCP_DEBUG, ("TCP connection closed: CLOSING %"U16_F" -> %"U16_F".\n", inseg.tcphdr->src, inseg.tcphdr->dest));
        tcp_pcb_purge(pcb);
        TCP_RMV_ACTIVE(pcb);
        pcb->state = TIME_WAIT;
        TCP_REG(&tcp_tw_pcbs, pcb);
      }
      break;
    case LAST_ACK:
      tcp_receive(pcb);
      if ((flags & TCP_ACK) && ackno == pcb->snd_nxt && pcb->unsent == NULL) {
        LWIP_DEBUGF(TCP_DEBUG, ("TCP connection closed: LAST_ACK %"U16_F" -> %"U16_F".\n", inseg.tcphdr->src, inseg.tcphdr->dest));
        /* bugfix #21699: don't set pcb->state to CLOSED here or we risk leaking segments */
        recv_flags |= TF_CLOSED;
      }
      break;
    default:
      break;
  }
  return ERR_OK;
}

#if TCP_QUEUE_OOSEQ
/**
 * Insert segment into the list (segments covered with new one will be deleted)
 *
 * Called from tcp_receive()
 */
static void
tcp_oos_insert_segment(struct tcp_seg *cseg, struct tcp_seg *next)
{
  struct tcp_seg *old_seg;

  LWIP_ASSERT("tcp_oos_insert_segment: invalid cseg", cseg != NULL);

  if (TCPH_FLAGS(cseg->tcphdr) & TCP_FIN) {
    /* received segment overlaps all following segments */
    tcp_segs_free(next);
    next = NULL;
  } else {
    /* delete some following segments
       oos queue may have segments with FIN flag */
    while (next &&
           TCP_SEQ_GEQ((seqno + cseg->len),
                       (next->tcphdr->seqno + next->len))) {
      /* cseg with FIN already processed */
      if (TCPH_FLAGS(next->tcphdr) & TCP_FIN) {
        TCPH_SET_FLAG(cseg->tcphdr, TCP_FIN);
      }
      old_seg = next;
      next = next->next;
      tcp_seg_free(old_seg);
    }
    if (next &&
        TCP_SEQ_GT(seqno + cseg->len, next->tcphdr->seqno)) {
      /* We need to trim the incoming segment. */
      cseg->len = (u16_t)(next->tcphdr->seqno - seqno);
      pbuf_realloc(cseg->p, cseg->len);
    }
  }
  cseg->next = next;
}
#endif /* TCP_QUEUE_OOSEQ */

#if LWIP_SACK
/* Fucntion called when ACK for new data is received in FRLR state
   which is less than recovery_point */
static void tcp_sack_proc_ack_wo_recovery(struct tcp_pcb *pcb)
{
  /* Run SetPipe() */
  /* As per step B in section 5 of RFC 6675 */
  tcp_sack_set_pipe(pcb);

  LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_receive : Pipe is %"U32_F
      " RecoveryPoint is %"U32_F" HighSacked is %"U32_F" HighData is %"U32_F
      " HighRxt is %"U32_F" RescueRxt is %"U32_F"\n", pcb->pipe, pcb->recovery_point,
      pcb->high_sacked, pcb->high_data, pcb->high_rxt, pcb->rescue_rxt));

  /* As per step C of Section 5 of RFC 6675 */
  /* Going to do loss recovery algorithm */
  if (pcb->next_seg_for_lr) {
    tcp_sack_based_loss_recovery_alg(pcb);
  }

#if LWIP_SACK_PERF_OPT
  if (pcb->fr_segs) {
    tcp_sack_rexmit_lost_rexmitted(pcb);
  }
#endif

  return;
}

/* Fucntion called when ACK for new data is received in FRLR state
   which is greater than equal to recovery_point */
static void tcp_sack_proc_ack_with_recovery(struct tcp_pcb *pcb)
{
  pcb->flags = (tcpflags_t)(pcb->flags & (~TF_IN_SACK_FRLR));
  pcb->next_seg_for_lr = NULL;

#if LWIP_SACK_PERF_OPT
  tcp_fr_segs_free(pcb->fr_segs);
  pcb->fr_segs = NULL;
  pcb->last_frseg = NULL;
#endif
#if LWIP_SACK_CWND_OPT
  pcb->cwnd = pcb->recover_cwnd;
  pcb->ssthresh = pcb->recover_ssthresh;
  pcb->recover_cwnd = 0;
  pcb->recover_ssthresh = 0;
  LWIP_DEBUGF(TCP_CWND_DEBUG, ("tcp_receive: Restored cwnd to %"TCPWNDSIZE_F"\n", pcb->cwnd));
#endif
  LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_receive : Disabled SACK loss recovery flag\n"));
}
#endif /* LWIP_SACK */

/** Remove segments from a list if the incoming ACK acknowledges them */
static struct tcp_seg *
tcp_free_acked_segments(struct tcp_pcb *pcb, struct tcp_seg *seg_list, const char *dbg_list_name,
                        struct tcp_seg *dbg_other_seg_list)
{
  struct tcp_seg *next;
  u16_t clen;

  LWIP_UNUSED_ARG(dbg_list_name);
  LWIP_UNUSED_ARG(dbg_other_seg_list);

  while (seg_list != NULL &&
         TCP_SEQ_LEQ(lwip_ntohl(seg_list->tcphdr->seqno) +
                     TCP_TCPLEN(seg_list), ackno)) {
    LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_receive: removing %"U32_F":%"U32_F" from pcb->%s\n",
                                  lwip_ntohl(seg_list->tcphdr->seqno),
                                  lwip_ntohl(seg_list->tcphdr->seqno) + TCP_TCPLEN(seg_list),
                                  dbg_list_name));

    next = seg_list;
    seg_list = seg_list->next;

#if LWIP_SACK
#if LWIP_SACK_PERF_OPT
    if (pcb->flags & TF_SACK) {
      if (pcb->next_seg_for_lr == next) {
        pcb->next_seg_for_lr = pcb->next_seg_for_lr->next;
        LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_receive : Next segment is updated\n"));
      }

      if ((pcb->fr_segs != NULL) && (pcb->fr_segs->seg == next)) {
        struct tcp_sack_fast_rxmited *temp_seg = pcb->fr_segs;
        pcb->fr_segs = pcb->fr_segs->next;
        /* If the acked segment is the latest fast retransmitted segment,
           then update the last_frseg pointer to point to next latest unacked fr segment */
        if (pcb->last_frseg == temp_seg) {
          pcb->last_frseg = pcb->last_frseg->next;
        }
        mem_free(temp_seg);
      }
    }
#else
    if ((pcb->flags & TF_SACK) && (pcb->next_seg_for_lr == next)) {
      pcb->next_seg_for_lr = pcb->next_seg_for_lr->next;
      LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_receive : Next segment is updated\n"));
    }
#endif

    if ((pcb->flags & TF_SACK) && (next->state & TF_SEG_SACKED) && (pcb->sacked)) {
      pcb->sacked--;
    }
#endif

    clen = pbuf_clen(next->p);
    LWIP_DEBUGF(TCP_QLEN_DEBUG, ("tcp_receive: queuelen %"TCPWNDSIZE_F" ... ",
                                 (tcpwnd_size_t)pcb->snd_queuelen));
    LWIP_ASSERT("pcb->snd_queuelen >= pbuf_clen(next->p)", (pcb->snd_queuelen >= clen));

    pcb->snd_queuelen = (u16_t)(pcb->snd_queuelen - clen);
    recv_acked = (tcpwnd_size_t)(recv_acked + next->len);
    tcp_seg_free(next);

    LWIP_DEBUGF(TCP_QLEN_DEBUG, ("%"TCPWNDSIZE_F" (after freeing %s)\n",
                                 (tcpwnd_size_t)pcb->snd_queuelen,
                                 dbg_list_name));
    if (pcb->snd_queuelen != 0) {
      LWIP_ASSERT("tcp_receive: valid queue length",
                  seg_list != NULL || dbg_other_seg_list != NULL);
    }
  }
  return seg_list;
}

/**
 * Called by tcp_process. Checks if the given segment is an ACK for outstanding
 * data, and if so frees the memory of the buffered data. Next, it places the
 * segment on any of the receive queues (pcb->recved or pcb->ooseq). If the segment
 * is buffered, the pbuf is referenced by pbuf_ref so that it will not be freed until
 * it has been removed from the buffer.
 *
 * If the incoming segment constitutes an ACK for a segment that was used for RTT
 * estimation, the RTT is estimated here as well.
 *
 * Called from tcp_process().
 */
static void
tcp_receive(struct tcp_pcb *pcb)
{
  s32_t m;
  u32_t right_wnd_edge;
  int found_dupack = 0;
#if LWIP_SACK
  u32_t new_sacked = 0; /* This variable intrinsically means that new_sack_block has arrived */
#endif
  u32_t rcv_wup;

#if (LWIP_TCP_ER_SUPPORT || LWIP_TCP_TLP_SUPPORT)
  u32_t wnd;
#endif
#if LWIP_TCP_ER_SUPPORT
  struct tcp_seg *unacked_seg = NULL;
  u32_t unacked_cnt = 0;
  u8_t need_er;
#endif
  tcpwnd_size_t outstanding_segments;
  LWIP_ASSERT("tcp_receive: invalid pcb", pcb != NULL);
  LWIP_ASSERT("tcp_receive: wrong state", pcb->state >= ESTABLISHED);

  /* In established state or above, ACK must be present */
  if (!(flags & TCP_ACK)) {
    return;
  }

  /* Handle segment with Invalid sequence number */
  /* Filter out invalid sequence */
  if ((tcplen > 0) && (pcb->state < CLOSE_WAIT)) {
    if (!TCP_SEQ_BETWEEN(pcb->rcv_nxt, seqno + 1, seqno + tcplen - 1)) {
      if (TCP_SEQ_LT(seqno, pcb->rcv_nxt)) {
        /* the whole segment is < rcv_nxt */
        /* must be a duplicate of a packet that has already been correctly handled */
        LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_receive: duplicate seqno %"U32_F"\n", seqno));
        tcp_ack_now(pcb);
      }
    }

    /* Ideally, rcv_wup should be the value of rcv_nxt when an ACK is sent last time. But there are
     * no such fields kept in struct pcb, hence the lower-limit estimate is made by rewinding the
     * current recv window, but no less than 4 times of rcv_mss */
    rcv_wup = pcb->rcv_nxt - LWIP_MAX(pcb->rcv_mss * 4, pcb->rcv_wnd);
    if (TCP_SEQ_LT(seqno + tcplen, rcv_wup) || TCP_SEQ_GT(seqno, pcb->rcv_nxt + pcb->rcv_wnd)) {
      /* seqno is too old or to new to be plausible, send an immediate ACK and drop the segment */
      (void)tcp_send_empty_ack(pcb);
      return;
    }
  } else {
    /* Segments with length 0 is taken care of here. Segments that
       fall out of the window are ACKed. */
    if (!TCP_SEQ_BETWEEN(seqno, pcb->rcv_nxt, pcb->rcv_nxt + pcb->rcv_wnd - 1)) {
      /* Ignore the rest of packets */
      tcp_ack_now(pcb);
      return;
    }
  }

  /* Data from segments acking unsent data is not dropped */
  if (TCP_SEQ_GT(ackno,  pcb->snd_nxt)) {
    /* Check for states */
    if ((pcb->state != CLOSE_WAIT) && (pcb->state != CLOSING) && (pcb->state != LAST_ACK)) {
      (void)tcp_send_empty_ack(pcb);
    }
    return;
  }
  if (flags & TCP_ACK) {
    right_wnd_edge = pcb->snd_wnd + pcb->snd_wl2;

    /* Update window. */
    if (TCP_SEQ_LT(pcb->snd_wl1, seqno) ||
        (pcb->snd_wl1 == seqno && TCP_SEQ_LT(pcb->snd_wl2, ackno)) ||
        (pcb->snd_wl2 == ackno && (u32_t)SND_WND_SCALE(pcb, tcphdr->wnd) > pcb->snd_wnd)) {
      pcb->snd_wnd = SND_WND_SCALE(pcb, tcphdr->wnd);
      /* keep track of the biggest window announced by the remote host to calculate
         the maximum segment size */
      if (pcb->snd_wnd_max < pcb->snd_wnd) {
        pcb->snd_wnd_max = pcb->snd_wnd;
      }
      pcb->snd_wl1 = seqno;
      pcb->snd_wl2 = ackno;
      if (pcb->snd_wnd == 0) {
        if (pcb->persist_backoff == 0) {
          /* start persist timer */
          pcb->persist_cnt = 0;
          pcb->persist_backoff = 1;
          pcb->persist_probe = 0;
        }
      } else if (pcb->persist_backoff > 0) {
        /* stop persist timer */
          pcb->persist_backoff = 0;
      }
      LWIP_DEBUGF(TCP_WND_DEBUG, ("tcp_receive: window update %"TCPWNDSIZE_F"\n", pcb->snd_wnd));
#if TCP_WND_DEBUG
    } else {
      if (pcb->snd_wnd != (tcpwnd_size_t)SND_WND_SCALE(pcb, tcphdr->wnd)) {
        LWIP_DEBUGF(TCP_WND_DEBUG,
                    ("tcp_receive: no window update lastack %"U32_F" ackno %"
                     U32_F" wl1 %"U32_F" seqno %"U32_F" wl2 %"U32_F"\n",
                     pcb->lastack, ackno, pcb->snd_wl1, seqno, pcb->snd_wl2));
      }
#endif /* TCP_WND_DEBUG */
    }

#if LWIP_SACK
    if (pcb->flags & TF_SACK) {
#if !LWIP_SACK_DATA_SEG_PIGGYBACK
      if (tcplen == 0)
#endif
      {
        if (pcb->num_sacks) {
          /* The return value intrinsically means that new_sack_block has arrived */
          new_sacked = tcp_sack_update(pcb, ackno);
          pcb->num_sacks = 0;
        }
      }

#if LWIP_TCP_TLP_SUPPORT
      /* draft-dukkipati-tcpm-tcp-loss-probe-01: 3.2.  TLP Loss Detection: Algorithm Details.

          If the only loss was the last segment, there is the risk that the loss probe itself might repair
          the loss, effectively masking it from congestion control.  To avoid interfering with mandatory
          congestion control [RFC5681] it is imperative that TLP include a mechanism to detect when
          the probe might have masked a loss and to properly reduce the congestion window (cwnd)

          Scenario:

          number of    scoreboard after
          losses          TLP retrans ACKed            mechanism           final outcome
          --------   -----------------  -----------------   -------------
          (1) AAAL       AAAA                        TLP loss detection         all repaired

          As above, with one segment lost, the TLP loss probe itself will repair the loss.  In this case,
          the sender's TLP loss detection algorithm will notice that a segment was lost and repaired,
          and reduce its congestion window in response to the loss.
      */
      /*
       (a) TLPRtxOut > 0
       (b) SEG.ACK == TLPHighRxt
       (c) the segment contains no SACK blocks for sequence ranges above TLPHighRxt
       (d) the ACK does not advance SND.UNA
       (e) the segment contains no data
       (f) the segment is not a window update
      */
      if ((pcb->tlp_rtx_out > 0) && (ackno == pcb->tlp_high_rxt) && (new_sacked == 0) && (tcplen == 0)) {
        pcb->tlp_rtx_out--;
      }

      /* ACK advances: there was a loss, so reduce cwnd. */
      if (pcb->tlp_rtx_out && TCP_SEQ_GEQ(ackno, pcb->tlp_high_rxt)) {
        /* very aggresive, actually we should halve the cwnd as TLP fixes the tail packet loss */
        TCP_WND_DEC(pcb->cwnd, pcb->mss, pcb->mss);
        pcb->tlp_rtx_out = 0;
      }
#endif /* LWIP_TCP_TLP_SUPPORT */
    }
#endif /* LWIP_SACK */

    /* RFC 5827.
        Section: 3.2.  Segment-Based Early Retransmit

        Upon the arrival of an ACK, a sender employing segment-based Early
        Retransmit MUST use the following two conditions to determine when an
        Early Retransmit is sent:
        (3.a) The number of outstanding segments (oseg) -- segments sent but
         not yet acknowledged -- is less than four.

        (3.b) There is either no unsent data ready for transmission at the
         sender, or the advertised receive window does not permit new
         segments to be transmitted.

        ACTION_WITHOUT_SACK:
        When the above two conditions hold and a TCP connection does not
        support SACK, the duplicate ACK threshold used to trigger a
        retransmission MUST be reduced to:
                  ER_thresh = oseg - 1

        ACTION_WITH_SACK:
        When conditions (3.a) and (3.b) hold and a TCP connection does
        support SACK or SCTP is in use, Early Retransmit MUST be used only
        when "oseg - 1" segments have been SACKed.  A segment is considered
        to be SACKed when all of its data bytes (TCP) or data chunks (SCTP)
        have been indicated as arrived by the receiver.
    */
#if LWIP_TCP_ER_SUPPORT
    need_er = 1;

    /* Ensure (3.b) */
    if (pcb->unsent) {
      wnd = LWIP_MIN(pcb->snd_wnd, pcb->cwnd);
      /* Just need to check the first segment */
      if (((ntohl(pcb->unsent->tcphdr->seqno) - pcb->lastack) + pcb->unsent->len) <= wnd) {
        need_er = 0;
      }
    }

    /* Ensure (3.a) */
    if (need_er) {
      for (unacked_seg = pcb->unacked; unacked_seg != NULL; unacked_seg = unacked_seg->next) {
        unacked_cnt++;
        if (unacked_cnt > DUPACK_THRESH) {
          need_er = 0;
          break;
        }
      }
    }
#endif /* LWIP_TCP_ER_SUPPORT */

    /* (From Stevens TCP/IP Illustrated Vol II, p970.) Its only a
     * duplicate ack if:
     * 1) It doesn't ACK new data
     * 2) length of received packet is zero (i.e. no payload)
     * 3) the advertised window hasn't changed
     * 4) There is outstanding unacknowledged data (retransmission timer running)
     * 5) The ACK is == biggest ACK sequence number so far seen (snd_una)
     *
     * If it passes all five, should process as a dupack:
     * a) dupacks < 3: do nothing
     * b) dupacks == 3: fast retransmit
     * c) dupacks > 3: increase cwnd
     *
     * If it only passes 1-3, should reset dupack counter (and add to
     * stats, which we don't do in lwIP)
     *
     * If it only passes 1, should reset dupack counter
     *
     */

    /* Clause 1 */
#if LWIP_SACK
    if ((TCP_SEQ_LEQ(ackno, pcb->lastack)) && (!(pcb->flags & TF_SACK)))
#else
    if (TCP_SEQ_LEQ(ackno, pcb->lastack))
#endif
    {
      /* Clause 2 */
      if (tcplen == 0) {
        /* Clause 3 */
        if (pcb->snd_wl2 + pcb->snd_wnd == right_wnd_edge) {
          /* Clause 4 */
          if (pcb->rtime >= 0) {
            /* Clause 5 */
            if (pcb->lastack == ackno) {
              found_dupack = 1;
              if ((u8_t)(pcb->dupacks + 1) > pcb->dupacks) {
                ++pcb->dupacks;
              }
              if (pcb->dupacks > DUPACK_THRESH) {
                /*
                 4.  For each additional duplicate ACK received (after the third),
                 cwnd MUST be incremented by SMSS.  This artificially inflates the
                 congestion window in order to reflect the additional segment that
                 has left the network.

                 Here, assuming atleast one packet is outstanding, hence limiting wnd increase to dupacks -1
                */
                outstanding_segments = (tcpwnd_size_t)(pcb->snd_nxt - pcb->lastack);
                if (outstanding_segments > (tcpwnd_size_t)((pcb->dupacks - 1) * pcb->mss)) {
                  /* Inflate the congestion window */
                  TCP_WND_INC(pcb->cwnd, pcb->mss);
                }
              } else if ((pcb->dupacks == DUPACK_THRESH)
#if LWIP_TCP_ER_SUPPORT /* ACTION_WITHOUT_SACK for Early Retransmit */
                                || (need_er && unacked_cnt && (pcb->dupacks == (unacked_cnt - 1)))
#endif /* LWIP_TCP_ER_SUPPORT */
                            ) {
                /* Do fast retransmit */
                tcp_rexmit_fast(pcb);
              }
            }
          }
        }
      }
      /* If Clause (1) or more is true, but not a duplicate ack, reset
       * count of consecutive duplicate acks */
      if (!found_dupack) {
        pcb->dupacks = 0;
      }
    }
#if LWIP_SACK
    else if ((TCP_SEQ_LEQ(ackno, pcb->lastack)) && (pcb->flags & TF_SACK)) {
      /* As per RFC 6675 */
      /* Duplicate acknowledgment is a segment that arrives carrying a SACK */
      /* block that identifies previously unacknowledged and un-SACKed octets */
      /* between HighACK and HighData. */
#if !LWIP_SACK_DATA_SEG_PIGGYBACK
      if (tcplen == 0)
#endif
      {
        if (new_sacked) {
          if ((u8_t)(pcb->dupacks + 1) > pcb->dupacks) {
            pcb->dupacks++;
            LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_receive: Incremented dupack "
                "%"U32_F"\n", pcb->dupacks));
          }

          /* As per RFC 6675, Section 5, if the incoming ACK is cumulative */
          /* acknowledgement then need to reset DupAcks to zero. */
          /* Even if app data seg from peer increments the cumulative ACK, */
          /* we are resetting the dupack to 0 in next else if case. */
          if (pcb->unacked != NULL) {
            if ((pcb->dupacks >= DUPACK_THRESH)
#if LWIP_TCP_ER_SUPPORT /* ACTION_WITH_SACK for Early Retransmit */
                /* Basic Early retransmit proposed in RFC 5827 */
                || (need_er && (pcb->sacked == (unacked_cnt - 1)))
#endif /* LWIP_TCP_ER_SUPPORT */
#if (LWIP_TCP_TLP_SUPPORT && LWIP_TCP_ER_SUPPORT)
                /* Enhanced Early retransmit proposed in TLP draft-dukkipati-tcpm-tcp-loss-probe-01 */
                || (need_er && ((unacked_cnt - 1) > pcb->sacked))
#endif /* (LWIP_TCP_TLP_SUPPORT && LWIP_TCP_ER_SUPPORT) */
                  || ((pcb->dupacks < DUPACK_THRESH) && tcp_sack_is_lost(pcb, pcb->unacked))) {
                LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_receive: Going to start SACK based fast "
                            "retransmit and loss recovery phase, dupack is %"U32_F"\n", pcb->dupacks));
                /* Do fast retransmit as per point 1 and 2 in Section 5 of RFC 6675 */
                /* 1. If dupacks >= Threshold then start fast retransmit and loss recovery */
                /* 2. Or if IsLost (HighACK + 1)  returns true then there is loss */
                /* need to enter fast retransmit and loss recovery phase */
                tcp_sack_based_fast_rexmit_alg(pcb);

                if (pcb->flags & TF_IN_SACK_FRLR) {
                  /* Run SetPipe() */
                  /* As per point 4.4 in section 5 of RFC 6675 */
                  tcp_sack_set_pipe(pcb);
                }

                /* Then as per point 4.5, enter step C of Section 5 of RFC 6675 */
                /* Going to do loss recovery algorithm */
#if LWIP_SACK_PERF_OPT
                if ((pcb->flags & TF_IN_SACK_FRLR))
#else
                if ((pcb->flags & TF_IN_SACK_FRLR) && (pcb->next_seg_for_lr))
#endif /* LWIP_SACK_PERF_OPT */
                {
                  tcp_sack_based_loss_recovery_alg(pcb);
                }
#if LWIP_TCP_TLP_SUPPORT
                if (pcb->tlp_time_stamp) {
                  LWIP_TCP_TLP_CLEAR_VARS(pcb);
                  if ((pcb->unacked != NULL) && (pcb->rtime == -1)) {
                    pcb->rtime = 0;
                  }
                }
#endif /* LWIP_TCP_TLP_SUPPORT */
#if LWIP_SACK_PERF_OPT
                if (pcb->dupacks > DUPACK_THRESH) {
                  /* Inflate the congestion window, but not if it means that the value overflows. */
                  /* Adding for WINDOW SCALE */
                  {
                    if (pcb->cwnd < pcb->ssthresh) {
                      /* Adding for WINDOW SCALE */
                      if ((tcpwnd_size_t)(pcb->cwnd + pcb->mss) > pcb->cwnd) {
                        pcb->cwnd += pcb->mss;
                      }
                      /* Adding for WINDOW SCALE */
                      LWIP_DEBUGF(TCP_CWND_DEBUG, ("tcp_receive:dupack slow start cwnd %"TCPWNDSIZE_F"\n", pcb->cwnd));
                    } else {
                      /*  Loss of precision (initialization) (31 bits to 16 bits) */
                      /* Adding for WINDOW SCALE */
                      tcpwnd_size_t new_cwnd = (tcpwnd_size_t)(pcb->cwnd +
                                                               (u32_t)pcb->mss * (u32_t)pcb->mss / pcb->cwnd);
                      if (new_cwnd > pcb->cwnd) {
                        pcb->cwnd = new_cwnd;
                      }
                      /* Adding for WINDOW SCALE */
                      LWIP_DEBUGF(TCP_CWND_DEBUG, ("tcp_receive:dupack congestion avoidance cwnd %"TCPWNDSIZE_F"\n",
                                                   pcb->cwnd));
                    }
                  }
                }
#endif /* LWIP_SACK_PERF_OPT */
              }
            }
          }
      }
    }
#endif
    else if (TCP_SEQ_BETWEEN(ackno, pcb->lastack + 1, pcb->snd_nxt)) {
      /* We come here when the ACK acknowledges new data. */
      tcpwnd_size_t acked;
      u8_t is_infr = 0;

      /* We come here when the ACK acknowledges new data. */
      /* Record how much data this ACK acks */
      acked = (tcpwnd_size_t)(ackno - pcb->lastack);

      /* Reset the "IN Fast Retransmit" flag, since we are no longer
         in fast retransmit. Also reset the congestion window to the
         slow start threshold. */
      if (pcb->flags & TF_INFR) {
        is_infr = 1;
        if (TCP_SEQ_LT(ackno, pcb->fast_recovery_point)) {
          /* Partial ACK recved, retransmit next unacked segment */
          tcp_set_flags(pcb, TF_INFR_PACK);

          /*
             [RFC 6582] Section-3.2
             For the first partial ACK that arrives during fast recovery, also
             reset the retransmit timer.
             If a large number of packets were dropped from a window of
               data, the TCP data sender's retransmit timer will ultimately expire,
               and the TCP data sender will invoke Slow-Start. We call this the Impatient variant of NewReno.

             When N packets have been dropped from a window of data for a large
               value of N, the Slow-but-Steady variant can remain in Fast Recovery
               for N round-trip times, retransmitting one more dropped packet each
               round-trip time; for these scenarios, the Impatient variant gives a
               faster recovery and better performance.

          */
          if (tcp_is_flag_set(pcb, TF_INFR_FPACK)) {
            /* Reset the retransmission timer to prevent immediate rto retransmissions */
            pcb->rtime = 0;
            tcp_clear_flags(pcb, TF_INFR_FPACK);
          }

          if (acked < pcb->mss) {
            TCP_WND_DEC(pcb->cwnd, acked, pcb->mss);
          }
        } else {
          /*
            [RFC 5681] : Section 3.2.  Fast Retransmit/Fast Recovery
           6.  When the next ACK arrives that acknowledges previously
               unacknowledged data, a TCP MUST set cwnd to ssthresh (the value
               set in step 2).  This is termed "deflating" the window.

          Rationale:
          In [RFC3782], the cwnd after Full ACK reception will be set to
           (1) min (ssthresh, FlightSize + SMSS) or (2) ssthresh.  However, the
           first option carries a risk of performance degradation: With the
           first option, if FlightSize is zero, the result will be 1 SMSS.  This
           means TCP can transmit only 1 segment at that moment, which can cause
           a delay in ACK transmission at the receiver due to a delayed ACK
           algorithm.
           The FlightSize on Full ACK reception can be zero in some situations.
           A typical example is where the sending window size during fast
           recovery is small.  In this case, the retransmitted packet and new
           data packets can be transmitted within a short interval.  If all
           these packets successfully arrive, the receiver may generate a Full
           ACK that acknowledges all outstanding data.  Even if the window size
           is not small, loss of ACK packets or a receive buffer shortage during
           fast recovery can also increase the possibility of falling into this
           situation.

          [RFC 6582] Section 3.2
             Full acknowledgments:
             If this ACK acknowledges all of the data up to and including
             recover, then the ACK acknowledges all the intermediate segments
             sent between the original transmission of the lost segment and
             the receipt of the third duplicate ACK.  Set cwnd to either (1)
             min (ssthresh, max(FlightSize, SMSS) + SMSS) or (2) ssthresh,
             where ssthresh is the value set when fast retransmit was entered,
             and where FlightSize in (1) is the amount of data presently
             outstanding.

             (2) ssthresh,-- Not implemented, because lwIP has not implemented mechanism to avoid burst of data
               RFC - Advice,  If the
               second option is selected, the implementation is encouraged to
               take measures to avoid a possible burst of data, in case the
               amount of data outstanding in the network is much less than the
               new congestion window allows.
          */
          tcp_clear_flags(pcb, TF_INFR);
          outstanding_segments = (tcpwnd_size_t)(pcb->snd_nxt - pcb->lastack);
          outstanding_segments = LWIP_MAX(outstanding_segments, pcb->mss);
          TCP_WND_INC(outstanding_segments, pcb->mss);
          pcb->cwnd = LWIP_MIN(pcb->ssthresh, outstanding_segments);

          /* when exiting mark the recovery point */
          pcb->fast_recovery_point = ackno;
          pcb->bytes_acked = 0;
        }
      }

#if !LWIP_SACK_PERF_OPT
#if LWIP_SACK
      if (pcb->flags & TF_IN_SACK_FRLR) {
        if (TCP_SEQ_LT(ackno, pcb->recovery_point)) {
          tcp_sack_proc_ack_wo_recovery(pcb);
        } else {
          tcp_sack_proc_ack_with_recovery(pcb);
        }
      }
      if ((pcb->flags & TF_IN_SACK_RTO != 0) && (TCP_SEQ_GEQ(ackno, pcb->recovery_point))) {
          pcb->flags = (tcpflags_t)(pcb->flags & (~TF_IN_SACK_RTO));
          LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_receive : Disabled SACK retransmit timeout flag\n"));
      }
#endif /* LWIP_SACK */
#endif /* LWIP_SACK_PERF_OPT */

      /*
        Reset RTO backoff when new data was ACKed during RTO phase. As lwIP violates RFC5861
        section 3.1, when RTO happened, lwIP tries to retransmit all unacked segments. If any
        retransmitted segment was ACKed, it means RTO retransmission succeed, and RTO backoff
        could be cleared. A new RTO timeout value was calculated by the stale SRTT and RTTVAR.
        It seems that this way is aggressive, but in order to achieve better throughput under high packet
        lossy environment, we adopt this processing way temporarily.
      */
      if (pcb->nrtx) {
        pcb->nrtx = 0;
        if (pcb->sa != -1) {
          pcb->rto = (s16_t)((((u32_t)pcb->sa >> 3) + pcb->sv) / TCP_SLOW_INTERVAL);
          pcb->rto = (s16_t)(LWIP_MIN(TCP_MAX_RTO_TICKS, LWIP_MAX(TCP_MIN_RTO_TICKS, pcb->rto)));
        } else {
          /* if RTO happened for 1st data segment, just use the initial RTO value. */
          pcb->rto = TCP_INITIAL_RTO_DURATION / TCP_SLOW_INTERVAL;
        }
      }


      /* Record how much data this ACK acks */
      acked = (tcpwnd_size_t)(ackno - pcb->lastack);

      /* Reset the fast retransmit variables. */
      pcb->dupacks = 0;
      pcb->lastack = ackno;

#if LWIP_FACK_THRESHOLD_BASED_FR
      pcb->fack = ackno;
#endif /* LWIP_FACK_THRESHOLD_BASED_FR */

      /* Update the congestion control variables (cwnd and ssthresh). */
#if LWIP_SACK_CWND_OPT
      if ((pcb->flags & TF_IN_SACK_FRLR) && (TCP_SEQ_GEQ(ackno, pcb->recovery_point))) {
        tcp_sack_proc_ack_with_recovery(pcb);
      } /* else case handled below  as it requires the cwnd value updated below */
#endif

      /* RFC 6582 specifies specific window updation algorithm for FR state */
      if ((pcb->state >= ESTABLISHED) && !is_infr) {
        if (pcb->cwnd < pcb->ssthresh) {
          tcpwnd_size_t increase;
          /* limit to 1 SMSS segment during period following RTO */
          u8_t num_seg = (pcb->flags & TF_RTO) ? 1 : 2;

          /*
            RFC 5681 Section 3.1 Slow Start
               During slow start, a TCP increments cwnd by at most SMSS bytes for
               each ACK received that cumulatively acknowledges new data.  Slow
               start ends when cwnd exceeds ssthresh (or, optionally, when it
               reaches it, as noted above) or when congestion is observed.  While
               traditionally TCP implementations have increased cwnd by precisely
               SMSS bytes upon receipt of an ACK covering new data, we RECOMMEND
               that TCP implementations increase cwnd, per:

                  cwnd += min (N, SMSS)                      (2)

               where N is the number of previously unacknowledged bytes acknowledged
               in the incoming ACK.  This adjustment is part of Appropriate Byte
               Counting [RFC3465] and provides robustness against misbehaving
               receivers that may attempt to induce a sender to artificially inflate
               cwnd using a mechanism known as "ACK Division" [SCWA99].  ACK
               Division consists of a receiver sending multiple ACKs for a single
               TCP data segment, each acknowledging only a portion of its data.  A
               TCP that increments cwnd by SMSS for each such ACK will
               inappropriately inflate the amount of data injected into the network.
          */
          /* RFC 3465, section 2.2 Slow Start */
          increase = LWIP_MIN(acked, (tcpwnd_size_t)(num_seg * pcb->mss));
          TCP_WND_INC(pcb->cwnd, increase);
          LWIP_DEBUGF(TCP_CWND_DEBUG, ("tcp_receive: slow start cwnd %"TCPWNDSIZE_F"\n", pcb->cwnd));
        } else {
          /* RFC 3465, section 2.1 Congestion Avoidance */
          TCP_WND_INC(pcb->bytes_acked, acked);
          if (pcb->bytes_acked >= pcb->cwnd) {
            pcb->bytes_acked = (tcpwnd_size_t)(pcb->bytes_acked - pcb->cwnd);
            TCP_WND_INC(pcb->cwnd, pcb->mss);
          }
          LWIP_DEBUGF(TCP_CWND_DEBUG, ("tcp_receive: congestion avoidance cwnd %"TCPWNDSIZE_F"\n", pcb->cwnd));
        }
      }
      LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_receive: ACK for %"U32_F", unacked->seqno %"U32_F":%"U32_F"\n",
                                    ackno,
                                    pcb->unacked != NULL ?
                                    lwip_ntohl(pcb->unacked->tcphdr->seqno) : 0,
                                    pcb->unacked != NULL ?
                                    lwip_ntohl(pcb->unacked->tcphdr->seqno) + TCP_TCPLEN(pcb->unacked) : 0));

      /* Remove segment from the unacknowledged list if the incoming
         ACK acknowledges them. */
      pcb->unacked = tcp_free_acked_segments(pcb, pcb->unacked, "unacked", pcb->unsent);
      /* We go through the ->unsent list to see if any of the segments
         on the list are acknowledged by the ACK. This may seem
         strange since an "unsent" segment shouldn't be acked. The
         rationale is that lwIP puts all outstanding segments on the
         ->unsent list after a retransmission, so these segments may
         in fact have been sent once. */
      pcb->unsent = tcp_free_acked_segments(pcb, pcb->unsent, "unsent", pcb->unacked);

      if (pcb->flags & TF_INFR_PACK) {
        /* Partial ACK recved, fast retransmit next unack segment */
        LWIP_DEBUGF(TCP_FR_DEBUG,
                    ("tcp_receive: dupacks %"U16_F" (%"U32_F"), fast retransmit %"U32_F"\n",
                     (u16_t)pcb->dupacks, (u32_t)pcb->lastack, (u32_t)ntohl(pcb->unacked->tcphdr->seqno)));
        (void)tcp_rexmit(pcb);
        pcb->flags = (tcpflags_t)(pcb->flags & (~TF_INFR_PACK));
      }

#if LWIP_SACK_PERF_OPT
      if (pcb->flags & TF_IN_SACK_FRLR) {
        if (TCP_SEQ_LT(ackno, pcb->recovery_point)) {
          tcp_sack_proc_ack_wo_recovery(pcb);
        }
#if !LWIP_SACK_CWND_OPT
        /* else will not be hit here as the FRLR flags is disabled above in this case */
        else {
          tcp_sack_proc_ack_with_recovery(pcb);
        }
#endif
      }
      if (((pcb->flags & TF_IN_SACK_RTO) != 0) && (TCP_SEQ_GEQ(ackno, pcb->recovery_point))) {
        pcb->flags = (tcpflags_t)(pcb->flags & (~TF_IN_SACK_RTO));
        LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_receive : Disabled SACK retransmit timeout flag\n"));
      }
#endif /* LWIP_SACK_PERF_OPT */

      /* If there's nothing left to acknowledge, stop the retransmit
         timer, otherwise reset it to start again */
      if (pcb->unacked == NULL) {
        pcb->rtime = -1;
      } else {
        pcb->rtime = 0;
      }

      pcb->polltmr = 0;

#if TCP_OVERSIZE
      if (pcb->unsent == NULL) {
        pcb->unsent_oversize = 0;
      }
#endif /* TCP_OVERSIZE */

#if LWIP_IPV6 && LWIP_ND6_TCP_REACHABILITY_HINTS
      if (ip_current_is_v6()) {
        /* Inform neighbor reachability of forward progress. */
        nd6_reachability_hint(ip6_current_src_addr());
      }
#endif /* LWIP_IPV6 && LWIP_ND6_TCP_REACHABILITY_HINTS*/

      pcb->snd_buf += recv_acked;
      /* check if this ACK ends our retransmission of in-flight data */
      if (pcb->flags & TF_RTO) {
        /* RTO is done if
            1) both queues are empty or
            2) unacked is empty and unsent head contains data not part of RTO or
            3) unacked head contains data not part of RTO */
        if (pcb->unacked == NULL) {
          if ((pcb->unsent == NULL) ||
              (TCP_SEQ_LEQ(pcb->rto_end, lwip_ntohl(pcb->unsent->tcphdr->seqno)))) {
            tcp_clear_flags(pcb, TF_RTO);
          }
        } else if (TCP_SEQ_LEQ(pcb->rto_end, lwip_ntohl(pcb->unacked->tcphdr->seqno))) {
          tcp_clear_flags(pcb, TF_RTO);
        }
      }
      /* End of ACK for new data processing. */
    } else {
      /* Out of sequence ACK, didn't really ack anything */
      tcp_send_empty_ack(pcb);
    }

    LWIP_DEBUGF(TCP_RTO_DEBUG, ("tcp_receive: pcb->rttest %"U32_F" rtseq %"U32_F" ackno %"U32_F"\n",
                                pcb->rttest, pcb->rtseq, ackno));

    /* RTT estimation calculations. This is done by checking if the
       incoming segment acknowledges the segment we use to take a
       round-trip time measurement. */
    if (pcb->rttest && TCP_SEQ_LT(pcb->rtseq, ackno)) {
      m = (s32_t)(sys_now() - pcb->rttest);

      if (pcb->sa != -1) {
        /*
          RFC 6298 section 2.3
          When a subsequent RTT measurement R is made, a host MUST set:
              RTTVAR <- (1 - beta) * RTTVAR + beta * |SRTT - R|;
              SRTT <- (1 - alpha) * SRTT + alpha * R;
              RTO <- SRTT + max (G, K*RTTVAR);
              where alpha = 1/8, beta = 1/4;
          The value of SRTT used in the update to RTTVAR is its value
          before updating SRTT itself using the second assignment. That
          is, updating RTTVAR and SRTT MUST be computed in the above order.
        */
        m = (s32_t)((u32_t)m - ((u32_t)pcb->sa >> 3));
        pcb->sa = (s32_t)(pcb->sa + m);
        if (m < 0) {
          m = (s32_t)-m;
        }
        m = (s32_t)(m - ((u32_t)pcb->sv >> 2));
        pcb->sv = (s32_t)(pcb->sv + m);
      } else {
        /*
          RFC 6298 section 2.2
          When the first RTT measurement R is made, the host MUST set:
              SRTT <- R
              RTTVAR <- R/2
              RTO <- SRTT + max (G, K*RTTVAR), where K = 4.
        */
        pcb->sa = m << 3;
        pcb->sv = m << 1;
      }

      pcb->rto = (s16_t)((((u16_t)pcb->sa >> 3) + pcb->sv) / TCP_SLOW_INTERVAL);
      /*
         RFC 6298 section 2.5
         A maximum value MAY be placed on RTO provided it is at least 60 seconds.
      */
      pcb->rto = (s16_t)(LWIP_MIN(TCP_MAX_RTO_TICKS, LWIP_MAX(TCP_MIN_RTO_TICKS, pcb->rto)));

      LWIP_DEBUGF(TCP_RTO_DEBUG, ("tcp_receive: RTO %"U16_F" (%"U16_F" milliseconds)\n",
                                  pcb->rto, (u16_t)(pcb->rto * TCP_SLOW_INTERVAL)));

      pcb->rttest = 0;
    }
#if LWIP_TCP_TLP_SUPPORT
    /* draft-dukkipati-tcpm-tcp-loss-probe-01: During ACK processing */
    /* a) Cancel any existing PTO. */
    if (pcb->tlp_time_stamp) {
      LWIP_TCP_TLP_CLEAR_VARS(pcb);
      if ((pcb->unacked != NULL) && (pcb->rtime == -1)) {
        pcb->rtime = 0;
      }
    }
    /*
      b) If conditions for scheduling PTO allow:
      -> Reschedule PTO relative to the ACK receipt time.
    */
    wnd = LWIP_MIN(pcb->snd_wnd, pcb->cwnd);
    tcp_tlp_schedule_probe(pcb, wnd);
#endif /* LWIP_TCP_TLP_SUPPORT */
  }

  /* If the incoming segment contains data, we must process it
     further unless the pcb already received a FIN.
     (RFC 793, chapter 3.9, "SEGMENT ARRIVES" in states CLOSE-WAIT, CLOSING,
     LAST-ACK and TIME-WAIT: "Ignore the segment text.") */
  if ((tcplen > 0) && (pcb->state < CLOSE_WAIT)) {
/* For sending SACK options along with data */
#if LWIP_SACK_DATA_SEG_PIGGYBACK
    if (pcb->sack_seq != NULL) {
      do {
        struct _sack_seq *ptr = NULL;
        ptr = pcb->sack_seq->next;
        mem_free(pcb->sack_seq);
        pcb->sack_seq = ptr;
      } while (pcb->sack_seq != NULL);
      pcb->sack_seq = NULL;
    }
#endif

    /* This code basically does three things:

    +) If the incoming segment contains data that is the next
    in-sequence data, this data is passed to the application. This
    might involve trimming the first edge of the data. The rcv_nxt
    variable and the advertised window are adjusted.

    +) If the incoming segment has data that is above the next
    sequence number expected (->rcv_nxt), the segment is placed on
    the ->ooseq queue. This is done by finding the appropriate
    place in the ->ooseq queue (which is ordered by sequence
    number) and trim the segment in both ends if needed. An
    immediate ACK is sent to indicate that we received an
    out-of-sequence segment.

    +) Finally, we check if the first segment on the ->ooseq queue
    now is in sequence (i.e., if rcv_nxt >= ooseq->seqno). If
    rcv_nxt > ooseq->seqno, we must trim the first edge of the
    segment on ->ooseq before we adjust rcv_nxt. The data in the
    segments that are now on sequence are chained onto the
    incoming segment so that we only need to call the application
    once.
    */

    /* First, we check if we must trim the first edge. We have to do
       this if the sequence number of the incoming segment is less
       than rcv_nxt, and the sequence number plus the length of the
       segment is larger than rcv_nxt. */
    /*    if (TCP_SEQ_LT(seqno, pcb->rcv_nxt)) {
          if (TCP_SEQ_LT(pcb->rcv_nxt, seqno + tcplen)) {*/
    if (TCP_SEQ_BETWEEN(pcb->rcv_nxt, seqno + 1, seqno + tcplen - 1)) {
      /* Trimming the first edge is done by pushing the payload
         pointer in the pbuf downwards. This is somewhat tricky since
         we do not want to discard the full contents of the pbuf up to
         the new starting point of the data since we have to keep the
         TCP header which is present in the first pbuf in the chain.

         What is done is really quite a nasty hack: the first pbuf in
         the pbuf chain is pointed to by inseg.p. Since we need to be
         able to deallocate the whole pbuf, we cannot change this
         inseg.p pointer to point to any of the later pbufs in the
         chain. Instead, we point the ->payload pointer in the first
         pbuf to data in one of the later pbufs. We also set the
         inseg.data pointer to point to the right place. This way, the
         ->p pointer will still point to the first pbuf, but the
         ->p->payload pointer will point to data in another pbuf.

         After we are done with adjusting the pbuf pointers we must
         adjust the ->data pointer in the seg and the segment
         length.*/

      struct pbuf *p = inseg.p;
      u32_t off32 = pcb->rcv_nxt - seqno;
      u16_t new_tot_len, off;
      LWIP_ASSERT("inseg.p != NULL", inseg.p);
      LWIP_ASSERT("insane offset!", (off32 < 0xffff));
      off = (u16_t)off32;
      LWIP_ASSERT("pbuf too short!", (((s32_t)inseg.p->tot_len) >= off));
      inseg.len -= off;
      new_tot_len = (u16_t)(inseg.p->tot_len - off);
      while (p->len < off) {
        off -= p->len;
        /* all pbufs up to and including this one have len==0, so tot_len is equal */
        p->tot_len = new_tot_len;
        p->len = 0;
        p = p->next;
      }
      /* cannot fail... */
      pbuf_remove_header(p, off);
      inseg.tcphdr->seqno = seqno = pcb->rcv_nxt;
    } else {
      if (TCP_SEQ_LT(seqno, pcb->rcv_nxt)) {
        /* the whole segment is < rcv_nxt */
        /* must be a duplicate of a packet that has already been correctly handled */

        LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_receive: duplicate seqno %"U32_F"\n", seqno));
        tcp_ack_now(pcb);
      }
    }

    /* The sequence number must be within the window (above rcv_nxt
       and below rcv_nxt + rcv_wnd) in order to be further
       processed. */
    if (TCP_SEQ_BETWEEN(seqno, pcb->rcv_nxt,
                        pcb->rcv_nxt + pcb->rcv_wnd - 1)) {
      if (pcb->rcv_nxt == seqno) {
        /* The incoming segment is the next in sequence. We check if
           we have to trim the end of the segment and update rcv_nxt
           and pass the data to the application. */
        tcplen = TCP_TCPLEN(&inseg);

        if (tcplen > pcb->rcv_wnd) {
          LWIP_DEBUGF(TCP_INPUT_DEBUG,
                      ("tcp_receive: other end overran receive window"
                       "seqno %"U32_F" len %"U16_F" right edge %"U32_F"\n",
                       seqno, tcplen, pcb->rcv_nxt + pcb->rcv_wnd));
          if (TCPH_FLAGS(inseg.tcphdr) & TCP_FIN) {
            /* Must remove the FIN from the header as we're trimming
             * that byte of sequence-space from the packet */
            TCPH_FLAGS_SET(inseg.tcphdr, TCPH_FLAGS(inseg.tcphdr) & ~(unsigned int)TCP_FIN);
          }
          /* Adjust length of segment to fit in the window. */
          TCPWND_CHECK16(pcb->rcv_wnd);
          inseg.len = (u16_t)pcb->rcv_wnd;
          if (TCPH_FLAGS(inseg.tcphdr) & TCP_SYN) {
            inseg.len -= 1;
          }
          pbuf_realloc(inseg.p, inseg.len);
          tcplen = TCP_TCPLEN(&inseg);
          LWIP_ASSERT("tcp_receive: segment not trimmed correctly to rcv_wnd\n",
                      (seqno + tcplen) == (pcb->rcv_nxt + pcb->rcv_wnd));
        }
#if TCP_QUEUE_OOSEQ
        /* Received in-sequence data, adjust ooseq data if:
           - FIN has been received or
           - inseq overlaps with ooseq */
        if (pcb->ooseq != NULL) {
          if (TCPH_FLAGS(inseg.tcphdr) & TCP_FIN) {
            LWIP_DEBUGF(TCP_INPUT_DEBUG,
                        ("tcp_receive: received in-order FIN, binning ooseq queue\n"));
            /* Received in-order FIN means anything that was received
             * out of order must now have been received in-order, so
             * bin the ooseq queue */
            while (pcb->ooseq != NULL) {
              struct tcp_seg *old_ooseq = pcb->ooseq;
              pcb->ooseq = pcb->ooseq->next;
              tcp_seg_free(old_ooseq);
            }
          } else {
            struct tcp_seg *next = pcb->ooseq;
            /* Remove all segments on ooseq that are covered by inseg already.
             * FIN is copied from ooseq to inseg if present. */
            while (next &&
                   TCP_SEQ_GEQ(seqno + tcplen,
                               next->tcphdr->seqno + next->len)) {
              struct tcp_seg *tmp;
              /* inseg cannot have FIN here (already processed above) */
              if ((TCPH_FLAGS(next->tcphdr) & TCP_FIN) != 0 &&
                  (TCPH_FLAGS(inseg.tcphdr) & TCP_SYN) == 0) {
                TCPH_SET_FLAG(inseg.tcphdr, TCP_FIN);
                tcplen = TCP_TCPLEN(&inseg);
              }
              tmp = next;
              next = next->next;
              tcp_seg_free(tmp);
            }
            /* Now trim right side of inseg if it overlaps with the first
             * segment on ooseq */
            if (next &&
                TCP_SEQ_GT(seqno + tcplen,
                           next->tcphdr->seqno)) {
              /* inseg cannot have FIN here (already processed above) */
              inseg.len = (u16_t)(next->tcphdr->seqno - seqno);
              if (TCPH_FLAGS(inseg.tcphdr) & TCP_SYN) {
                inseg.len -= 1;
              }
              pbuf_realloc(inseg.p, inseg.len);
              tcplen = TCP_TCPLEN(&inseg);
              LWIP_ASSERT("tcp_receive: segment not trimmed correctly to ooseq queue\n",
                          (seqno + tcplen) == next->tcphdr->seqno);
            }
            pcb->ooseq = next;
          }
        }
#endif /* TCP_QUEUE_OOSEQ */

        pcb->rcv_nxt = seqno + tcplen;

        /* Update the receiver's (our) window. */
        LWIP_ASSERT("tcp_receive: tcplen > rcv_wnd\n", pcb->rcv_wnd >= tcplen);
        pcb->rcv_wnd -= tcplen;

        tcp_update_rcv_ann_wnd(pcb);

        /* If there is data in the segment, we make preparations to
           pass this up to the application. The ->recv_data variable
           is used for holding the pbuf that goes to the
           application. The code for reassembling out-of-sequence data
           chains its data on this pbuf as well.

           If the segment was a FIN, we set the TF_GOT_FIN flag that will
           be used to indicate to the application that the remote side has
           closed its end of the connection. */
        if (inseg.p->tot_len > 0) {
          recv_data = inseg.p;
          /* Since this pbuf now is the responsibility of the
             application, we delete our reference to it so that we won't
             (mistakingly) deallocate it. */
          inseg.p = NULL;
        }
        if (TCPH_FLAGS(inseg.tcphdr) & TCP_FIN) {
          LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_receive: received FIN.\n"));
          recv_flags |= TF_GOT_FIN;
        }

#if TCP_QUEUE_OOSEQ
        /* We now check if we have segments on the ->ooseq queue that
           are now in sequence. */
        while (pcb->ooseq != NULL &&
               pcb->ooseq->tcphdr->seqno == pcb->rcv_nxt) {

          struct tcp_seg *cseg = pcb->ooseq;
          seqno = pcb->ooseq->tcphdr->seqno;

          pcb->rcv_nxt += TCP_TCPLEN(cseg);
          LWIP_ASSERT("tcp_receive: ooseq tcplen > rcv_wnd\n",
                      pcb->rcv_wnd >= TCP_TCPLEN(cseg));
          pcb->rcv_wnd -= TCP_TCPLEN(cseg);

          tcp_update_rcv_ann_wnd(pcb);

          if (cseg->p->tot_len > 0) {
            /* Chain this pbuf onto the pbuf that we will pass to
               the application. */
            /* With window scaling, this can overflow recv_data->tot_len, but
               that's not a problem since we explicitly fix that before passing
               recv_data to the application. */
            if (recv_data) {
              pbuf_cat(recv_data, cseg->p);
            } else {
              recv_data = cseg->p;
            }
            cseg->p = NULL;
          }
          if (TCPH_FLAGS(cseg->tcphdr) & TCP_FIN) {
            LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_receive: dequeued FIN.\n"));
            recv_flags |= TF_GOT_FIN;
            if (pcb->state == ESTABLISHED) { /* force passive close or we can move to active close */
              pcb->state = CLOSE_WAIT;
            }
          }

          pcb->ooseq = cseg->next;
          tcp_seg_free(cseg);
        }
#endif /* TCP_QUEUE_OOSEQ */

#if LWIP_SACK
        tcp_update_sack_for_received_ooseq_segs(pcb);
#else
        /* Acknowledge the segment(s). */
        tcp_ack(pcb);
#endif

#if LWIP_IPV6 && LWIP_ND6_TCP_REACHABILITY_HINTS
        if (ip_current_is_v6()) {
          /* Inform neighbor reachability of forward progress. */
          nd6_reachability_hint(ip6_current_src_addr());
        }
#endif /* LWIP_IPV6 && LWIP_ND6_TCP_REACHABILITY_HINTS*/

      } else {
        /* We get here if the incoming segment is out-of-sequence. */

#if LWIP_SACK && TCP_QUEUE_OOSEQ
        if (pcb->flags & TF_SACK) {
          update_ooseq_order_and_cnt(pcb, &inseg.order);
        }
#endif

#if TCP_QUEUE_OOSEQ
        /* We queue the segment on the ->ooseq queue. */
        if (pcb->ooseq == NULL) {
          pcb->ooseq = tcp_seg_copy(&inseg);
        } else {
          /* If the queue is not empty, we walk through the queue and
             try to find a place where the sequence number of the
             incoming segment is between the sequence numbers of the
             previous and the next segment on the ->ooseq queue. That is
             the place where we put the incoming segment. If needed, we
             trim the second edges of the previous and the incoming
             segment so that it will fit into the sequence.

             If the incoming segment has the same sequence number as a
             segment on the ->ooseq queue, we discard the segment that
             contains less data. */

          struct tcp_seg *next, *prev = NULL;
          for (next = pcb->ooseq; next != NULL; next = next->next) {
            if (seqno == next->tcphdr->seqno) {
              /* The sequence number of the incoming segment is the
                 same as the sequence number of the segment on
                 ->ooseq. We check the lengths to see which one to
                 discard. */
              if (inseg.len > next->len) {
                /* The incoming segment is larger than the old
                   segment. We replace some segments with the new
                   one. */
                struct tcp_seg *cseg = tcp_seg_copy(&inseg);
                if (cseg != NULL) {
                  if (prev != NULL) {
                    prev->next = cseg;
                  } else {
                    pcb->ooseq = cseg;
                  }
                  tcp_oos_insert_segment(cseg, next);
                }
                break;
              } else {
                /* Either the lengths are the same or the incoming
                   segment was smaller than the old one; in either
                   case, we ditch the incoming segment. */
                break;
              }
            } else {
              if (prev == NULL) {
                if (TCP_SEQ_LT(seqno, next->tcphdr->seqno)) {
                  /* The sequence number of the incoming segment is lower
                     than the sequence number of the first segment on the
                     queue. We put the incoming segment first on the
                     queue. */
                  struct tcp_seg *cseg = tcp_seg_copy(&inseg);
                  if (cseg != NULL) {
                    pcb->ooseq = cseg;
                    tcp_oos_insert_segment(cseg, next);
                  }
                  break;
                }
              } else {
                /*if (TCP_SEQ_LT(prev->tcphdr->seqno, seqno) &&
                  TCP_SEQ_LT(seqno, next->tcphdr->seqno)) {*/
                if (TCP_SEQ_BETWEEN(seqno, prev->tcphdr->seqno + 1, next->tcphdr->seqno - 1)) {
                  /* The sequence number of the incoming segment is in
                     between the sequence numbers of the previous and
                     the next segment on ->ooseq. We trim trim the previous
                     segment, delete next segments that included in received segment
                     and trim received, if needed. */
                  struct tcp_seg *cseg = tcp_seg_copy(&inseg);
                  if (cseg != NULL) {
                    if (TCP_SEQ_GT(prev->tcphdr->seqno + prev->len, seqno)) {
                      /* We need to trim the prev segment. */
                      prev->len = (u16_t)(seqno - prev->tcphdr->seqno);
                      pbuf_realloc(prev->p, prev->len);
                    }
                    prev->next = cseg;
                    tcp_oos_insert_segment(cseg, next);
                  }
                  break;
                }
              }

              /* We don't use 'prev' below, so let's set it to current 'next'.
                 This way even if we break the loop below, 'prev' will be pointing
                 at the segment right in front of the newly added one. */
              prev = next;

              /* If the "next" segment is the last segment on the
                 ooseq queue, we add the incoming segment to the end
                 of the list. */
              if (next->next == NULL &&
                  TCP_SEQ_GT(seqno, next->tcphdr->seqno)) {
                if (TCPH_FLAGS(next->tcphdr) & TCP_FIN) {
                  /* segment "next" already contains all data */
                  break;
                }
                next->next = tcp_seg_copy(&inseg);
                if (next->next != NULL) {
                  if (TCP_SEQ_GT(next->tcphdr->seqno + next->len, seqno)) {
                    /* We need to trim the last segment. */
                    next->len = (u16_t)(seqno - next->tcphdr->seqno);
                    pbuf_realloc(next->p, next->len);
                  }
                  /* check if the remote side overruns our receive window */
                  if (TCP_SEQ_GT((u32_t)tcplen + seqno, pcb->rcv_nxt + (u32_t)pcb->rcv_wnd)) {
                    LWIP_DEBUGF(TCP_INPUT_DEBUG,
                                ("tcp_receive: other end overran receive window"
                                 "seqno %"U32_F" len %"U16_F" right edge %"U32_F"\n",
                                 seqno, tcplen, pcb->rcv_nxt + pcb->rcv_wnd));
                    if (TCPH_FLAGS(next->next->tcphdr) & TCP_FIN) {
                      /* Must remove the FIN from the header as we're trimming
                       * that byte of sequence-space from the packet */
                      TCPH_FLAGS_SET(next->next->tcphdr, TCPH_FLAGS(next->next->tcphdr) & ~TCP_FIN);
                    }
                    /* Adjust length of segment to fit in the window. */
                    next->next->len = (u16_t)(pcb->rcv_nxt + pcb->rcv_wnd - seqno);
                    pbuf_realloc(next->next->p, next->next->len);
                    tcplen = TCP_TCPLEN(next->next);
                    LWIP_ASSERT("tcp_receive: segment not trimmed correctly to rcv_wnd\n",
                                (seqno + tcplen) == (pcb->rcv_nxt + pcb->rcv_wnd));
                  }
                }
                break;
              }
            }
          }
        }
#if defined(TCP_OOSEQ_BYTES_LIMIT) || defined(TCP_OOSEQ_PBUFS_LIMIT)
        {
          /* Check that the data on ooseq doesn't exceed one of the limits
             and throw away everything above that limit. */
#ifdef TCP_OOSEQ_BYTES_LIMIT
          const u32_t ooseq_max_blen = TCP_OOSEQ_BYTES_LIMIT(pcb);
          u32_t ooseq_blen = 0;
#endif
#ifdef TCP_OOSEQ_PBUFS_LIMIT
          const u16_t ooseq_max_qlen = TCP_OOSEQ_PBUFS_LIMIT(pcb);
          u16_t ooseq_qlen = 0;
#endif
          struct tcp_seg *next, *prev = NULL;
          for (next = pcb->ooseq; next != NULL; prev = next, next = next->next) {
            struct pbuf *p = next->p;
            int stop_here = 0;
#ifdef TCP_OOSEQ_BYTES_LIMIT
            ooseq_blen += p->tot_len;
            if (ooseq_blen > ooseq_max_blen) {
              stop_here = 1;
            }
#endif
#ifdef TCP_OOSEQ_PBUFS_LIMIT
            ooseq_qlen += pbuf_clen(p);
            if (ooseq_qlen > ooseq_max_qlen) {
              stop_here = 1;
            }
#endif
            if (stop_here) {
              /* too much ooseq data, dump this and everything after it */
              tcp_segs_free(next);
              if (prev == NULL) {
                /* first ooseq segment is too much, dump the whole queue */
                pcb->ooseq = NULL;
              } else {
                /* just dump 'next' and everything after it */
                prev->next = NULL;
              }
              break;
            }
          }
        }
#endif /* TCP_OOSEQ_BYTES_LIMIT || TCP_OOSEQ_PBUFS_LIMIT */
#endif /* TCP_QUEUE_OOSEQ */

#if LWIP_SACK
        tcp_update_sack_for_received_ooseq_segs(pcb);
#else
        /* We send the ACK packet after we've (potentially) dealt with SACKs,
           so they can be included in the acknowledgment. */
        tcp_send_empty_ack(pcb);
#endif
      }
    } else {
      /* The incoming segment is not within the window. */
      tcp_send_empty_ack(pcb);
    }
  } else {
    /* Segments with length 0 is taken care of here. Segments that
       fall out of the window are ACKed. */
    if (!TCP_SEQ_BETWEEN(seqno, pcb->rcv_nxt, pcb->rcv_nxt + pcb->rcv_wnd - 1)) {
      tcp_ack_now(pcb);
    }
  }
}

/**
 * Parses the options contained in the incoming segment.
 *
 * Called from tcp_listen_input() and tcp_process().
 * Currently, only the MSS option is supported!
 *
 * @param pcb the tcp_pcb for which a segment arrived
 */
static void
tcp_parseopt(struct tcp_pcb *pcb)
{
  u16_t c, max_c;
  u16_t mss;
  u8_t *opts = NULL, opt;
#if LWIP_TCP_TIMESTAMPS
  u32_t tsval;
#endif

#if LWIP_SACK
  u8_t sack_perm_rx = 0;
#endif

  LWIP_ASSERT("tcp_parseopt: invalid pcb", pcb != NULL);

  /* Parse the TCP MSS option, if present. */
  if (TCPH_HDRLEN_BYTES(tcphdr) > TCP_HLEN) {
    opts = (u8_t *)tcphdr + TCP_HLEN;

    /* tcp headerlen check for > 5 is done, so value wont be negative */
    max_c = (u16_t)(TCPH_HDRLEN_BYTES(tcphdr) - TCP_HLEN);
    for (c = 0; c < max_c;) {
      opt = opts[c];
      switch (opt) {
      case LWIP_TCP_OPT_EOL:
        /* End of options. */
        LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: EOL\n"));
        goto RETURN;
      case LWIP_TCP_OPT_NOP:
        /* NOP option. */
        ++c;
        LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: NOP\n"));
        break;
      case LWIP_TCP_OPT_MSS:
        LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: MSS\n"));
        if (c + LWIP_TCP_OPT_LEN_MSS > max_c || opts[c + 1] != LWIP_TCP_OPT_LEN_MSS) {
          /* Bad length */
          LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: bad length\n"));
          goto RETURN;
        }
        /* An MSS option with the right option length. */
        mss = (u16_t)((opts[c + 2] << 8) | opts[c + 3]);

        /* Check for mss only in SYN packets, else ignore it */
        if (flags & TCP_SYN) {
        /* Limit the mss to the 536 and prevent division by zero */
          pcb->rcv_mss = (u16_t)((mss == 0) ? TCP_DEFAULT_MSS : mss);
        }

        /* Advance to next option */
        c = (u16_t)(c + LWIP_TCP_OPT_LEN_MSS);
        break;

#if LWIP_WND_SCALE
      case LWIP_TCP_OPT_WS:
        LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: WND_SCALE\n"));
        if (c + LWIP_TCP_OPT_LEN_WS > max_c || opts[c + 1] != LWIP_TCP_OPT_LEN_WS) {
          /* Bad length */
          LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: bad length\n"));
          goto RETURN;
        }
        /* An WND_SCALE option with the right option length. */
        /* If syn was received with wnd scale option,
           activate wnd scale opt, but only if this is not a retransmission */
        if ((flags & TCP_SYN) && !(pcb->flags & TF_WND_SCALE)) {
          pcb->snd_scale = opts[c + 2];
          /* the largest scale is 14 */
          if (pcb->snd_scale > 14U) {
            pcb->snd_scale = 14U;
          }
          pcb->rcv_scale = TCP_RCV_SCALE;
          pcb->flags |= TF_WND_SCALE;
          /* window scaling is enabled, we can use the full receive window */
          LWIP_ASSERT("window not at default value", pcb->rcv_wnd == TCPWND_MIN16(TCP_WND));
          LWIP_ASSERT("window not at default value", pcb->rcv_ann_wnd == TCPWND_MIN16(TCP_WND));
          pcb->rcv_wnd = pcb->rcv_ann_wnd = TCP_WND;
        }
        /* Advance to next option */
        c = (u16_t)(c + LWIP_TCP_OPT_LEN_WS);
        break;
#endif /* LWIP_WND_SCALE */

      /* Adding for SACK */
#if LWIP_SACK
      case LWIP_TCP_OPT_SACK_PERMITTED:
        LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: TCP SACK permitted\n"));
        if (c + LWIP_TCP_OPT_LEN_SACK_PERMITTED > max_c || opts[c + 1] != LWIP_TCP_OPT_LEN_SACK_PERMITTED) {
          /* Bad length */
          LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: bad length\n"));
          goto RETURN;
        }
        /*
         * If syn was received with tcp sack option,
         * activate sack opt
         */
        if (flags & TCP_SYN) {
          tcp_parseopt_sack_permitted(pcb);
          sack_perm_rx = 1;
        }

        c = (u16_t)(c + LWIP_TCP_OPT_LEN_SACK_PERMITTED);
        break;
      case LWIP_TCP_OPT_SACK:
        LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: TCP SACK\n"));

        /* Length check: (a) Non Zero (b)less than the max allowed options length (c) length should be 8*n+2 */
        if (((c + LWIP_TCP_OPT_SACK_MIN_LEN) > max_c) ||
            (!opts[c + 1]) || ((c + opts[c + 1]) > max_c) || ((opts[c + 1] - 2) % 8 != 0)) {
          /* Bad length */
          LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: bad length\n"));
          goto RETURN;
        }

        if (pcb->flags & TF_SACK) {
          pcb->num_sacks = tcp_parseopt_sack(opts, c);
        }
        /* Advance to next option */
        c = (u16_t)(c + opts[c + 1]);
        break;
#endif

#if LWIP_TCP_TIMESTAMPS
      case LWIP_TCP_OPT_TS:
        LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: TS\n"));
        if ((c + LWIP_TCP_OPT_LEN_TS > max_c) || (opts[c + 1] != LWIP_TCP_OPT_LEN_TS)) {
          /* Bad length */
          LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: bad length\n"));
          goto RETURN;
        }
        /* TCP timestamp option with valid length */
        tsval = (opts[c + 2]) | (opts[c + 3] << 8) | (opts[c + 4] << 16) | (u32_t)(opts[c + 5] << 24);
        if (flags & TCP_SYN) {
          pcb->ts_recent = lwip_ntohl(tsval);
          /* Enable sending timestamps in every segment now that we know
             the remote host supports it. */
          pcb->flags |= TF_TIMESTAMP;
        } else if (TCP_SEQ_BETWEEN(pcb->ts_lastacksent, seqno, seqno + tcplen)) {
          pcb->ts_recent = lwip_ntohl(tsval);
        }
        /* Advance to next option */
        c = (u16_t)(c + LWIP_TCP_OPT_LEN_TS);
        break;
#endif /* LWIP_TCP_TIMESTAMPS */
      default:
        LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: other\n"));
        if ((max_c <= c + 1) || opts[c + 1] == 0) {
          LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_parseopt: bad length\n"));
          /* If the length field is zero, the options are malformed
             and we don't process them further. */
          goto RETURN;
        }
        /* All other options have a length field, so that we easily
           can skip past them. */
        c = (u16_t)(c + opts[c + 1]);
        break;
      }
    }
  }

  RETURN:

#if LWIP_SACK
  if (flags & TCP_SYN) {
    /* If flags field is set with TF_SACK but, the peer does not support SACK,
       then disable SACK for the connection */
    if ((pcb->flags & TF_SACK) && (sack_perm_rx == 0)) {
      pcb->flags  = (tcpflags_t)(pcb->flags & (~TF_SACK));
    }
  }
#endif
  return;
}

void
tcp_trigger_input_pcb_close(void)
{
  recv_flags |= TF_CLOSED;
}

#endif /* LWIP_TCP */
