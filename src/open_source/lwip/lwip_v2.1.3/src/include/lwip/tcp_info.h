/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: TCP_INFO support
 * Author: none
 * Create: 2020
 */

#ifndef __LWIP_TCP_INFO_H__
#define __LWIP_TCP_INFO_H__


#include "opt.h"
#include "lwip/arch.h"
#include "lwip/priv/tcp_priv.h"

#ifdef LWIP_TCP
#ifdef __cplusplus
extern "C" {
#endif


/*
 * TCP FSM state definitions.
 * Per RFC793, September, 1981.
 */

#define TCP_NSTATES 11

/* for TCP_INFO socket option */
#define TCPI_OPT_TIMESTAMPS 1
#define TCPI_OPT_SACK       2
#define TCPI_OPT_WSCALE     4
#define TCPI_OPT_ECN        8 /* ECN was negociated at TCP session init */ /* Not supported in TCP_INFO */
#define TCPI_OPT_ECN_SEEN   16 /* we received at least one packet with ECT */  /* Not supported in TCP_INFO */
#define TCPI_OPT_SYN_DATA   32 /* SYN-ACK acked data in SYN sent or rcvd */ /* Not supported in TCP_INFO */

#if LWIP_TCP_INFO
#if !LWIP_LITEOS_COMPAT
enum tcpi_state {
  TCP_ESTABLISHED = 1,
  TCP_SYN_SENT,
  TCP_SYN_RECV,
  TCP_FIN_WAIT1,
  TCP_FIN_WAIT2,
  TCP_TIME_WAIT,
  TCP_CLOSE,
  TCP_CLOSE_WAIT,
  TCP_LAST_ACK,
  TCP_LISTEN,
  TCP_CLOSING
};

/** This structure gives information about the TCP connection. */
struct tcp_info {
    u8_t    tcpi_state; /**<  Indicates state of a TCP connection. This is currently supported.*/
    u8_t  tcpi_ca_state; /**<  Indicates the CA state. This is currently set to 0.*/
    u8_t  tcpi_retransmits; /**<  Indicates retransmit. This is currently supported.*/
    u8_t  tcpi_probes; /**<  Indicates probes. This is currently supported.*/
    u8_t  tcpi_backoff; /**<  Indicates backoff. This is currently supported.*/
    u8_t  tcpi_options; /**<  Indicates options . This is currently supported.*/
    u8_t  tcpi_snd_wscale : 4, tcpi_rcv_wscale : 4; /**<  Indicates send and receive of wscale.  This is currently set to 0.*/

    u32_t    tcpi_rto; /**<  Indicates rto. This is currently supported.*/
    u32_t    tcpi_ato; /**<  Indicates ato.  This is currently set to 0.*/
    u32_t    tcpi_snd_mss; /**<  Indicates the send message. This is currently supported.*/
    u32_t    tcpi_rcv_mss; /**<  Indicates the receive message state. This is currently supported.*/

    u32_t    tcpi_unacked; /**<  Indicates unacknowledge. This is currently supported.*/
    u32_t    tcpi_sacked; /**<  Indicates sack.  This is currently set to 0.*/
    u32_t    tcpi_lost;   /**<  Indicates lost packet.  This is currently set to 0.*/
    u32_t    tcpi_retrans;  /**<  Indicates retransmit.  This is currently set to 0.*/
    u32_t    tcpi_fackets;   /**<  Indicates fackets.  This is currently set to 0.*/

    /* Times. */
    u32_t    tcpi_last_data_sent;  /**<  Indicates the last data sent.  This is currently set to 0.*/
    u32_t    tcpi_last_ack_sent;   /**<  Indicates the last acknowledge sent. This is currently set to 0.*/
    u32_t    tcpi_last_data_recv;  /**<  Indicates last data receive.  This is currently set to 0.*/
    u32_t    tcpi_last_ack_recv;   /**<  Indicates the last acknowledge receive.  This is currently set to 0.*/

    /* Metrics. */
    u32_t    tcpi_pmtu;              /**<  Indicates mtu. This is currently set to 0.*/
    u32_t    tcpi_rcv_ssthresh;      /**<  Indicates slow start threshold receive. This is currently set to 0.*/
    u32_t    tcpi_rtt;               /**<  Indicates the rtt. This is currently supported.*/
    u32_t    tcpi_rttvar;            /**<  Indicates rttvar. This is currently supported.*/
    u32_t    tcpi_snd_ssthresh;     /**<  Indicates slow start threshold send. This is currently supported.*/
    u32_t    tcpi_snd_cwnd;        /**<  Indicates send cwnd. This is currently supported.*/
    u32_t    tcpi_advmss;          /**<  Indicates advmss. This is currently set to 0.*/
    u32_t    tcpi_reordering;     /**<  Indicates reordering. This is currently supported.*/

    u32_t    tcpi_rcv_rtt;     /**<  Indicates receive rtt.  This is currently set to 0.*/
    u32_t    tcpi_rcv_space;   /**<  Indicates TCPI receive space. This is currently set to 0.*/

    u32_t    tcpi_total_retrans;   /**<  Indicates total retransmit. This is currently set to 0.*/
};
#endif /* !LWIP_LITEOS_COMPAT */

void tcp_get_info(const struct tcp_pcb *pcb, struct tcp_info *tcpinfo);
#endif /* LWIP_TCP_INFO */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_TCP */
#endif /* __LWIP_TCP_CONFIG_H__ */
