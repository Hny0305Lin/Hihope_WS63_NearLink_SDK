/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: implementation for ipdebug APIs
 * Author: none
 * Create: 2020
 */
#include "lwip/opt.h"

#ifdef LWIP_DEBUG_INFO
#include "lwip/memp.h"
#include "lwip/pbuf.h"
#include "netif/ifaddrs.h"
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/igmp.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/dhcp.h"

#define IP_STR_LEN 64

void debug_ippcb_info(struct ip_pcb *ippcb)
{
  char local_ip_str[IP_STR_LEN];
  char remote_ip_str[IP_STR_LEN];

  if (ippcb != NULL) {
    (void)memset_s(local_ip_str, sizeof(local_ip_str), 0, sizeof(local_ip_str));
    (void)memset_s(remote_ip_str, sizeof(remote_ip_str), 0, sizeof(remote_ip_str));
    (void)ipaddr_ntoa_r(&ippcb->local_ip, local_ip_str, sizeof(local_ip_str));
    (void)ipaddr_ntoa_r(&ippcb->remote_ip, remote_ip_str, sizeof(remote_ip_str));
    LWIP_PLATFORM_PRINT("l_ip    :%s\n", local_ip_str);
    LWIP_PLATFORM_PRINT("r_ip    :%s\n", remote_ip_str);
    LWIP_PLATFORM_PRINT("so_opt  :%u\n", ippcb->so_options);
    LWIP_PLATFORM_PRINT("tos     :%u\n", ippcb->tos);
    LWIP_PLATFORM_PRINT("ifindex :%u\n", ippcb->netif_idx);
    LWIP_PLATFORM_PRINT("ttl     :%u\n", ippcb->ttl);
  }
}

#if LWIP_UDP || LWIP_UDPLITE
void debug_udppcb_info(struct udp_pcb *udppcb)
{
  if (udppcb != NULL) {
    LWIP_PLATFORM_PRINT("\n--------------UDPPCB\n");
    debug_ippcb_info((struct ip_pcb *)udppcb);
    LWIP_PLATFORM_PRINT("flags   :%u\n", udppcb->flags);
    LWIP_PLATFORM_PRINT("l_port  :%u\n", udppcb->local_port);
    LWIP_PLATFORM_PRINT("r_port  :%u\n", udppcb->remote_port);
    LWIP_PLATFORM_PRINT("last_p  :%u\n", udppcb->last_payload_len);
#if LWIP_MULTICAST_TX_OPTIONS
#if LWIP_IPV4
    char buf[IP4ADDR_STRLEN_MAX];
    (void)ip4addr_ntoa_r(&udppcb->mcast_ip4, buf, IP4ADDR_STRLEN_MAX);
    LWIP_PLATFORM_PRINT("mcast_ip4 :%s\n", buf);
#endif /* LWIP_IPV4 */
    LWIP_PLATFORM_PRINT("mcast_ifindex :%u\n", udppcb->mcast_ifindex);
#endif
#if LWIP_UDPLITE
    LWIP_PLATFORM_PRINT("chksum_r:%u\n", udppcb->chksum_len_rx);
    LWIP_PLATFORM_PRINT("chksum_t:%u\n", udppcb->chksum_len_tx);
#endif /* LWIP_UDPLITE */
  }
}
#endif /* LWIP_UDP || LWIP_UDPLITE */

#if LWIP_RAW
void debug_rawpcb_info(struct raw_pcb *rawpcb)
{
  if (rawpcb != NULL) {
    LWIP_PLATFORM_PRINT("\n--------------RAWPCB\n");
    debug_ippcb_info((struct ip_pcb *)rawpcb);
    LWIP_PLATFORM_PRINT("hdrincl :%u\n", raw_is_flag_set(rawpcb, RAW_FLAGS_HDRINCL));
#if PF_PKT_SUPPORT
    LWIP_PLATFORM_PRINT("netifidx:%u\n", rawpcb->netif_idx);
    LWIP_PLATFORM_PRINT("proto   :%u\n", rawpcb->proto.protocol);
    LWIP_PLATFORM_PRINT("e_proto :%u\n", rawpcb->proto.eth_proto);
#else
    LWIP_PLATFORM_PRINT("proto   :%u\n", rawpcb->protocol);
#endif /* PF_PKT_SUPPORT */
    LWIP_PLATFORM_PRINT("flags   :%u\n", rawpcb->flags);
  }
}

#endif /* LWIIP_RAW */

#if LWIP_TCP
void debug_tcppcb_info(struct tcp_pcb *tcppcb)
{
  if (tcppcb != NULL) {
    LWIP_PLATFORM_PRINT("\n--------------TCPPCB\n");
    debug_ippcb_info((struct ip_pcb *)tcppcb);
    LWIP_PLATFORM_PRINT("l_port  :%u\n", tcppcb->local_port);
    LWIP_PLATFORM_PRINT("state   :%u\n", tcppcb->state);
    LWIP_PLATFORM_PRINT("prio    :%u\n", tcppcb->prio);
    LWIP_PLATFORM_PRINT("s_buf_s :%u\n", tcppcb->snd_buf_static);
    LWIP_PLATFORM_PRINT("r_port  :%u\n", tcppcb->remote_port);
    LWIP_PLATFORM_PRINT("flags   :%u\n", tcppcb->flags);
    LWIP_PLATFORM_PRINT("p_tmr   :%u\n", tcppcb->polltmr);
    LWIP_PLATFORM_PRINT("p_intvl :%u\n", tcppcb->pollinterval);
    LWIP_PLATFORM_PRINT("l_timer :%u\n", tcppcb->last_timer);
    LWIP_PLATFORM_PRINT("tmr     :%u\n", tcppcb->tmr);
    LWIP_PLATFORM_PRINT("r_nxt   :%u\n", tcppcb->rcv_nxt);
    LWIP_PLATFORM_PRINT("r_wnd   :%u\n", tcppcb->rcv_wnd);
    LWIP_PLATFORM_PRINT("r_ann_w :%u\n", tcppcb->rcv_ann_wnd);
    LWIP_PLATFORM_PRINT("r_ann_r :%u\n", tcppcb->rcv_ann_right_edge);
    LWIP_PLATFORM_PRINT("rtime   :%d\n", tcppcb->rtime);
    LWIP_PLATFORM_PRINT("mss     :%u\n", tcppcb->mss);
    LWIP_PLATFORM_PRINT("rttest  :%u\n", tcppcb->rttest);
    LWIP_PLATFORM_PRINT("rtseq   :%u\n", tcppcb->rtseq);
    LWIP_PLATFORM_PRINT("sa      :%d\n", tcppcb->sa);
    LWIP_PLATFORM_PRINT("sv      :%d\n", tcppcb->sv);
    LWIP_PLATFORM_PRINT("rto     :%u\n", tcppcb->rto);
    LWIP_PLATFORM_PRINT("nrtx    :%u\n", tcppcb->nrtx);
    LWIP_PLATFORM_PRINT("dupacks :%u\n", tcppcb->dupacks);
    LWIP_PLATFORM_PRINT("lastack :%u\n", tcppcb->lastack);
    LWIP_PLATFORM_PRINT("cwnd    :%u\n", tcppcb->cwnd);
    LWIP_PLATFORM_PRINT("ssth    :%u\n", tcppcb->ssthresh);
    LWIP_PLATFORM_PRINT("s_nxt   :%u\n", tcppcb->snd_nxt);
    LWIP_PLATFORM_PRINT("s_sml   :%u\n", tcppcb->snd_sml);
    LWIP_PLATFORM_PRINT("s_wl1   :%u\n", tcppcb->snd_wl1);
    LWIP_PLATFORM_PRINT("s_w2    :%u\n", tcppcb->snd_wl2);
    LWIP_PLATFORM_PRINT("s_lbb   :%u\n", tcppcb->snd_lbb);
    LWIP_PLATFORM_PRINT("s_wnd   :%u\n", tcppcb->snd_wnd);
    LWIP_PLATFORM_PRINT("s_wnd_m :%u\n", tcppcb->snd_wnd_max);
    LWIP_PLATFORM_PRINT("s_buf   :%u\n", tcppcb->snd_buf);
    LWIP_PLATFORM_PRINT("k_idle  :%u\n", tcppcb->keep_idle);
    LWIP_PLATFORM_PRINT("p_cnt   :%u\n", tcppcb->persist_cnt);
    LWIP_PLATFORM_PRINT("p_boff  :%u\n", tcppcb->persist_backoff);
    LWIP_PLATFORM_PRINT("l_payl_l:%u\n", tcppcb->last_payload_len);
    LWIP_PLATFORM_PRINT("s_quel  :%u\n", tcppcb->snd_queuelen);
    LWIP_PLATFORM_PRINT("s_quel_m:%u\n", tcppcb->snd_queuelen_max);
    LWIP_PLATFORM_PRINT("s_quel_l:%u\n", tcppcb->snd_queuelen_lowat);
    LWIP_PLATFORM_PRINT("s_buf_l :%u\n", tcppcb->snd_buf_lowat);
    LWIP_PLATFORM_PRINT("unsent_o:%u\n", tcppcb->unsent_oversize);
#if LWIP_SACK
    LWIP_PLATFORM_PRINT("pipr    :%u\n", tcppcb->pipe);
    LWIP_PLATFORM_PRINT("r_point :%u\n", tcppcb->recovery_point);
    LWIP_PLATFORM_PRINT("h_data  :%u\n", tcppcb->high_data);
    LWIP_PLATFORM_PRINT("h_rxt   :%u\n", tcppcb->high_rxt);
    LWIP_PLATFORM_PRINT("r_rxt   :%u\n", tcppcb->rescue_rxt);
    LWIP_PLATFORM_PRINT("n_sacks :%u\n", tcppcb->num_sacks);
    LWIP_PLATFORM_PRINT("ooseq_c :%u\n", tcppcb->ooseq_cnt);
#if LWIP_SACK_PERF_OPT
    LWIP_PLATFORM_PRINT("p_seq_n :%u\n", tcppcb->pkt_seq_num);
    LWIP_PLATFORM_PRINT("h_s_seq :%u\n", tcppcb->high_sacked_pkt_seq);
#if LWIP_SACK_CWND_OPT
    LWIP_PLATFORM_PRINT("r_cwnd  :%u\n", tcppcb->recover_cwnd);
    LWIP_PLATFORM_PRINT("r_ssth  :%u\n", tcppcb->recover_ssthresh);
#endif
#endif /* LWIP_SACK_PERF_OPT */
#endif

#if LWIP_WND_SCALE
    LWIP_PLATFORM_PRINT("s_scale :%u\n", tcppcb->snd_scale);
    LWIP_PLATFORM_PRINT("r_scale :%u\n", tcppcb->rcv_scale);
#endif /* LWIP_WND_SCALE */
#if LWIP_TCP_KEEPALIVE
    LWIP_PLATFORM_PRINT("k_intvl :%u\n", tcppcb->keep_intvl);
    LWIP_PLATFORM_PRINT("k_cnt   :%u\n", tcppcb->keep_cnt);
    LWIP_PLATFORM_PRINT("k_cnt_s :%u\n", tcppcb->keep_cnt_sent);
#endif /* LWIP_TCP_KEEPALIVE */
#if DRIVER_STATUS_CHECK
    LWIP_PLATFORM_PRINT("drv_s   :%u\n", tcppcb->drv_status);
#endif /* DRIVER_STATUS_CHECK */

#if LWIP_TCP_TIMESTAMPS
    LWIP_PLATFORM_PRINT("ts_lastacksent   :%u\n", tcppcb->ts_lastacksent);
    LWIP_PLATFORM_PRINT("ts_recent   :%u\n", tcppcb->ts_recent);
#endif /* LWIP_TCP_TIMESTAMPS */
#if LWIP_TCP_TLP_SUPPORT
    LWIP_PLATFORM_PRINT("tlp_pto   :%u\n", tcppcb->tlp_pto_cnt);
    LWIP_PLATFORM_PRINT("tlp_rtx   :%u\n", tcppcb->tlp_rtx_out);
    LWIP_PLATFORM_PRINT("tlp_tm   :%u\n", tcppcb->tlp_time_stamp);
    LWIP_PLATFORM_PRINT("tlp_high   :%u\n", tcppcb->tlp_high_rxt);
#endif /* LWIP_TCP_TLP_SUPPORT */
  }
}
#endif /* LWIP_TCP */

void debug_netconn_info(struct netconn *conn, int expend)
{
  if (conn != NULL) {
    LWIP_PLATFORM_PRINT("\n--------------NETCONN\n");
    LWIP_PLATFORM_PRINT("socket  :%d\n", conn->socket);
    LWIP_PLATFORM_PRINT("s_to    :%d\n", conn->send_timeout);
    LWIP_PLATFORM_PRINT("r_to    :%d\n", conn->recv_timeout);
#if LWIP_SO_RCVBUF
    LWIP_PLATFORM_PRINT("r_buf_s :%d\n", conn->recv_bufsize);
    LWIP_PLATFORM_PRINT("r_avail :%d\n", conn->recv_avail);
#endif
    LWIP_PLATFORM_PRINT("l_err   :%d\n", conn->last_err);
    LWIP_PLATFORM_PRINT("flags   :%u\n", conn->flags);
    LWIP_PLATFORM_PRINT("type    :%u\n", conn->type);
    LWIP_PLATFORM_PRINT("state   :%d\n", conn->state);
    LWIP_PLATFORM_PRINT("shutdown:%d\n", conn->shutdown);
#if LWIP_TCP
    LWIP_PLATFORM_PRINT("pending_error:%d\n", conn->pending_err);
#endif
    LWIP_PLATFORM_PRINT("lrcv_left:%u\n", conn->lrcv_left);
    LWIP_PLATFORM_PRINT("last_err:%d\n", conn->last_err);

    if (expend) {
      if (conn->type == NETCONN_TCP) {
#if LWIP_TCP
        debug_tcppcb_info((void *)conn->pcb.tcp);
#endif /* LWIP_TCP */
      } else if (conn->type == NETCONN_UDP || conn->type == NETCONN_UDPLITE || conn->type == NETCONN_UDPNOCHKSUM) {
#if LWIP_UDP || LWIP_UDPLITE
        debug_udppcb_info((void *)conn->pcb.udp);
#endif /* LWIP_UDP || LWIP_UDPLITE */
      } else if (conn->type == NETCONN_RAW) {
#if LWIP_RAW
        debug_rawpcb_info((void *)conn->pcb.raw);
#endif
      } else {
        return;
      }
    }
  }
}

#endif /* LWIP_DEBUG_INFO */
