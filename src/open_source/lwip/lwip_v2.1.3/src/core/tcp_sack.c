/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: implementation for TCP SACK APIs
 * Author: none
 * Create: 2020
 */

#include "lwip/tcp_sack.h"
#include "lwip/priv/tcp_priv.h"

#include <string.h>

#if LWIP_SACK

#define MAX_NUM_SACK_OPTS    4
#define MAX_TCPOPT_LEN       40
#define MAX_NUM_SACK_BLOCK   4

#define BYTES_FOR_OPTION_BLOCK       8
#define BYTES_FOR_SACK_LENGTH_OPTION 2
#define U32_COUNT_ONE_OPTION_BLOCK   2 // BYTES_FOR_OPTION_BLOCK/sizeof(u32_t)
#define BYTES_FOR_SACK_KIND_AND_TYPE 4

struct tcp_sack_block {
  u32_t   start_seq;
  u32_t   end_seq;
};

/* g_sack_blocks holds the received sack blocks for the processing packet */
struct tcp_sack_block g_sack_blocks[MAX_NUM_SACK_BLOCK];

static int tcp_sack_is_sackblock_valid(const struct tcp_pcb *pcb, u32_t start_seq,
                                       u32_t end_seq, u32_t ackno);
extern err_t
tcp_output_segment(struct tcp_seg *seg, struct tcp_pcb *pcb, struct netif *netif);

/*
 * Called when a new TCP connection is established (i.e when
 * a SYN or SYN-ACK is received)
 *
 * @param pcb the new pcb in which the SYN/ SYN-ACK segment has arrived
 * @return void
 *
 * @note this function is called only when the connection is newly established
 *       and not in the later stages.
 */
void
tcp_pcb_reset_sack_seq(struct tcp_pcb *pcb)
{
  pcb->sack_seq = NULL;
}

#if TCP_QUEUE_OOSEQ
/**
 * Called when a TCP segment is received. This function checks and updates the
 * SACK blocks to be sent if Out Of Seq packets are present. If not, then send
 * just an ACK
 *
 * @param pcb the pcb in which the segment has arrived
 * @return void
 *
 */
void
tcp_update_sack_for_received_ooseq_segs(struct tcp_pcb *pcb)
{
  struct tcp_seg *cur = pcb->ooseq;
  struct _sack_seq *ptr, *ptr1, tmp_sack_seq;

  LWIP_ASSERT("pcb->sack_seq != NULL\n", pcb->sack_seq == NULL);
  if (pcb->flags & TF_SACK) {
    for (ptr = pcb->sack_seq, cur = pcb->ooseq; cur != NULL; cur = cur->next) {
      if (ptr == NULL) {
        ptr = (struct _sack_seq *)mem_malloc(sizeof(struct _sack_seq));
        LWIP_ERROR("mem_malloc failed.\n", ptr != NULL, goto mem_malloc_err1;);
        pcb->sack_seq = ptr;
        ptr->left = cur->tcphdr->seqno;
        ptr->right = cur->tcphdr->seqno + cur->len;
        ptr->order = cur->order;
        ptr->next = NULL;
      } else {
        if (cur->tcphdr->seqno == ptr->right) {
          ptr->right += cur->len;
          ptr->order = (cur->order > ptr->order) ? cur->order : ptr->order;
        } else if (TCP_SEQ_GT(cur->tcphdr->seqno, ptr->right)) {
          pcb->sack_seq = (struct _sack_seq *)mem_malloc(sizeof(struct _sack_seq));
          LWIP_ERROR("mem_malloc failed.\n", pcb->sack_seq != NULL, goto mem_malloc_err1;);
          pcb->sack_seq->next = ptr;
          ptr = pcb->sack_seq;
          ptr->left = cur->tcphdr->seqno;
          ptr->right = cur->tcphdr->seqno + cur->len;
          ptr->order = cur->order;
        }
      }
    }
    for (ptr = pcb->sack_seq; ptr != NULL; ptr = ptr->next) {
      for (ptr1 = ptr->next; ptr1 != NULL; ptr1 = ptr1->next) {
        if (ptr1->order > ptr->order) {
          (void)memcpy_s(&tmp_sack_seq, sizeof(struct _sack_seq), ptr1, sizeof(struct _sack_seq));

          (void)memcpy_s(ptr1, sizeof(struct _sack_seq), ptr, sizeof(struct _sack_seq));
          ptr1->next = tmp_sack_seq.next;

          tmp_sack_seq.next = ptr->next;
          (void)memcpy_s(ptr, sizeof(struct _sack_seq), &tmp_sack_seq, sizeof(struct _sack_seq));
        }
      }
    }
  }
mem_malloc_err1:
  if (pcb->ooseq != NULL) {
    (void)tcp_send_empty_ack(pcb);
  } else {
    /* Acknowledge the segment(s). */
    tcp_ack(pcb);
  }
}
#endif /* TCP_QUEUE_OOSEQ */

/**
 * Called by tcp_create_segment to initialize SACK related fields in the segment
 *
 * @param seg the newly created segment
 * @return void
 *
 */
void
tcp_update_sack_fields_for_new_seg(struct tcp_seg *seg)
{
  seg->state  = TF_SEG_NONE;
}

/**
 * Enqueues the TCP SACK Options Permitted option for transmission
 *
 * @param pcb the pcb connection which is used for transmission
 * @param optflags the options to be sent while transmission
 * @return void
 *
 */
void
tcp_enqueue_flags_sack(const struct tcp_pcb *pcb, u8_t *optflags)
{
  if ((pcb->state != SYN_RCVD) || (pcb->flags & TF_SACK)) {
    /* In a <SYN,ACK> (sent in state SYN_RCVD), the tcp sack option may only
     * be sent if we received a tcp sack option from the remote host. */
    (*optflags) |= TF_SEG_OPTS_SACK_PERMITTED;
  }
}

/**
 * Builds the SACK Options Permitted Option in SYN/ SYN-ACK message based on
 * TF_SEG_OPTS_SACK_PERMITTED flag.
 *
 * @param opts options filled in the tcp header
 * @return void
 *
 */
void
tcp_build_sack_permitted_option(u32_t *opts)
{
  /* RFC2018 Kind:04 Length:02 NOP:01 NOP:01 */
  opts[0] = htonl(SACK_SYNC_PERMITTED_OPTION);
}

/**
 * Calculates the number of SACK blocks allowed to be sent
 *
 * @param pcb the pcb which is used for transmission
 * @param optlen the length of the TCP Options filled so far
 * @return cnt the number of SACK Blocks allowed to be sent in the options
 *
 */
u8_t
tcp_get_sack_block_count_for_send(const struct tcp_pcb *pcb, u8_t optlen)
{
  u8_t cnt = 0;
  struct _sack_seq *ptr;
  u8_t len;

  len = BYTES_FOR_SACK_KIND_AND_TYPE + optlen;

  ptr = pcb->sack_seq;

  while (ptr != NULL) {
    len = len + BYTES_FOR_OPTION_BLOCK;
    if ((cnt < MAX_NUM_SACK_OPTS) && (len < MAX_TCPOPT_LEN)) {
      cnt++;
      ptr = ptr->next;
    } else {
      break;
    }
  }
  return cnt;
}

/**
 * Fills the TCP Options to be sent, with the SACK blocks received
 * starting from the latest Out Of Sequence segment received
 *
 * @param pcb  the pcb which is used for transmission
 * @param cnt  maximum number of SACK Blocks allowed to be filled in the options
 * @param tcphdr Pointer to the TCP Header where the options are to be filled
 * @return void
 *
 */
void tcp_build_sack_option(struct tcp_pcb *pcb, u8_t cnt, u32_t *options)
{
  struct _sack_seq *ptr;
  int index;
  if (cnt > 0) {
    options[0] = cnt > MAX_NUM_SACK_OPTS ? PP_HTONL(SACK_OPTIONS) :
                 PP_HTONL((SACK_OPTIONS | ((cnt * BYTES_FOR_OPTION_BLOCK) + BYTES_FOR_SACK_LENGTH_OPTION)));

#if LWIP_SACK_DATA_SEG_PIGGYBACK
    for (index = 1, ptr = pcb->sack_seq; (index <= (cnt * U32_COUNT_ONE_OPTION_BLOCK)) && (ptr != NULL);
         ptr = ptr->next)
#else
    for (index = 1, ptr = pcb->sack_seq; (index <= (cnt * U32_COUNT_ONE_OPTION_BLOCK)); ptr = ptr->next)
#endif
    {
      options[index++] = htonl(ptr->left);
      options[index++] = htonl(ptr->right);
    }
#if LWIP_SACK_DATA_SEG_PIGGYBACK
    /* reset the remaining space for sack options */
    while ((index <= (cnt * U32_COUNT_ONE_OPTION_BLOCK))) {
      options[index++] = 0;
      options[index++] = 0;
    }
#else
    if (pcb->sack_seq != NULL) {
      do {
        ptr = pcb->sack_seq->next;
        mem_free(pcb->sack_seq);
        pcb->sack_seq = ptr;
      }
      while (pcb->sack_seq != NULL);
      pcb->sack_seq = NULL;
    }
#endif
  }
}

/**
 * Enables SACK feature in the connection PCB, if SACK permitted option
 * is recieved
 *
 * Called from tcp_parseopt()
 *
 * @param pcb the tcp_pcb for which a segment arrived
 */
void
tcp_parseopt_sack_permitted(struct tcp_pcb *pcb)
{
  pcb->flags |= TF_SACK;
}

/**
 * Initializes sack variables in PCB during connection
 *
 * Called from tcp_connect()
 *
 * @param pcb the tcp_pcb of the connection
 */

void tcp_connect_update_sack(struct tcp_pcb *pcb, u32_t iss)
{
  pcb->sack_seq = NULL;
  pcb->flags |= TF_SACK;
  pcb->recovery_point = iss - 1;
  pcb->pipe = 0;
  pcb->high_sacked = iss - 1;
  pcb->high_data = iss - 1;
  pcb->high_rxt = iss - 1;
  pcb->rescue_rxt = iss - 1;

#if LWIP_FACK_THRESHOLD_BASED_FR
  pcb->fack = iss - 1;
#endif /* LWIP_FACK_THRESHOLD_BASED_FR */
#if LWIP_TCP_TLP_SUPPORT
  LWIP_TCP_TLP_CLEAR_VARS(pcb);
#endif /* LWIP_TCP_TLP_SUPPORT */
}

/**
 * Parses SACK option and updates the global variable g_sack_blocks
 * and it will be used in tcp_receive
 *
 * Called from tcp_parseopt()
 *
 * @param pcb the tcp_pcb for which a segment arrived
 */
u32_t
tcp_parseopt_sack(const u8_t *opts, u16_t c)
{
  u8_t l, i;
  u32_t left_wnd_edge, right_wnd_edge;

  for (l = opts[c + 1] - BYTES_FOR_SACK_LENGTH_OPTION,
       c += U32_COUNT_ONE_OPTION_BLOCK, i = 0; l > 0;
       l -= BYTES_FOR_OPTION_BLOCK, i++) {
    if (memcpy_s(&left_wnd_edge, sizeof(left_wnd_edge), opts + c, sizeof(u32_t)) != EOK) {
      LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_parseopt_sack: error in memcpy_s\n"));
      return 0;
    }
    left_wnd_edge = ntohl(left_wnd_edge);
    c += sizeof(u32_t);

    if (memcpy_s(&right_wnd_edge, sizeof(right_wnd_edge), opts + c, sizeof(u32_t)) != EOK) {
      LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_parseopt_sack: error in memcpy_s\n"));
      return 0;
    }

    right_wnd_edge = ntohl(right_wnd_edge);
    c += sizeof(u32_t);

    g_sack_blocks[i].start_seq = left_wnd_edge;
    g_sack_blocks[i].end_seq = right_wnd_edge;
  }
  return i;
}

/**
 * This function checks whether received sack blocks are valid or not
 *
 * Called by tcp_sack_update()
 *
 * @param pcb the tcp_pcb of this connection
 * @param start_seq the left edge of the sack block needs to be valided
 * @param end_seq the right edge of the sack block needs to be valided
 * @param ackno the cumulative ack received in this segment
 *
 * @return 1 if its valid sack block otherwise returns 0
 */
static int
tcp_sack_is_sackblock_valid(const struct tcp_pcb *pcb, u32_t start_seq, u32_t end_seq, u32_t ackno)
{
  /* start greated than end */
  if TCP_SEQ_GEQ(start_seq, end_seq) {
    return 0;
  }

  if (ackno < pcb->lastack) {
    /* Need to consider lastack if ack received now is less than lastack */
    ackno = pcb->lastack;
  }

  /* Check for finding valid sack block */
  if ((TCP_SEQ_GT(start_seq, ackno)) && (TCP_SEQ_LEQ(end_seq, pcb->snd_nxt))) {
    LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_is_sackblock_valid: Valid SACK block received "
        "(%"U32_F", %"U32_F"), ackno %"U32_F"\n", start_seq, end_seq, ackno));
    return 1;
  }

  return 0;
}

/**
 * Update () function implementation as per RFC 6675.
 * This routine checks received SACK and updates its score board.
 * Here state variable in tcp_pcb structure is used as score board.
 *
 * Called by tcp_receive()
 *
 * @param pcb the tcp_pcb of this connection
 * @param ackno the cumulative ack received in this segment
 *
 * @return the number of unack segments SACKed in the new sack block received, otherwise returns 0
 */
u32_t
tcp_sack_update(struct tcp_pcb *pcb, u32_t ackno)
{
  u32_t i;
  struct tcp_seg *seg = NULL;
  u32_t seqnum_lnt;
  u32_t new_sack_blocks = 0;

  for (i = 0; i < pcb->num_sacks; i++) {
    if (tcp_sack_is_sackblock_valid(pcb, g_sack_blocks[i].start_seq, g_sack_blocks[i].end_seq, ackno)) {
      for (seg = pcb->unacked; seg != NULL; seg = seg->next) {
        if (TCP_SEQ_GEQ(ntohl(seg->tcphdr->seqno), g_sack_blocks[i].start_seq) &&
            TCP_SEQ_LEQ(ntohl(seg->tcphdr->seqno) + TCP_TCPLEN(seg), g_sack_blocks[i].end_seq)) {
          if ((i < MAX_NUM_SACK_BLOCK - 1) &&
              (g_sack_blocks[i].start_seq > pcb->lastack) &&
              (g_sack_blocks[i].start_seq > g_sack_blocks[i + 1].start_seq) &&
              (g_sack_blocks[i].end_seq <= g_sack_blocks[i + 1].end_seq)) {
            new_sack_blocks = 1;
          }
          if (!(seg->state & TF_SEG_SACKED)) {
            seg->state |= TF_SEG_SACKED;
            pcb->sacked++;
            new_sack_blocks = 1;
            seqnum_lnt = ntohl(seg->tcphdr->seqno) + (u32_t)(TCP_TCPLEN(seg));
            /* If received SACK block is the highest received sack block then store it */
            if (TCP_SEQ_GT(seqnum_lnt, pcb->high_sacked)) {
              pcb->high_sacked = seqnum_lnt;
            }
#if LWIP_SACK_PERF_OPT
            if (TCP_SEQ_GT(seg->pkt_trans_seq_cntr, pcb->high_sacked_pkt_seq)) {
              /* keep one higher than last sent */
              pcb->high_sacked_pkt_seq = seg->pkt_trans_seq_cntr;
            }
#endif /* LWIP_SACK_PERF_OPT */

#if LWIP_FACK_THRESHOLD_BASED_FR
            if (TCP_SEQ_GT(seqnum_lnt, pcb->fack)) {
              pcb->fack = seqnum_lnt;
            }
#endif /* LWIP_FACK_THRESHOLD_BASED_FR */
            LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_update: New SACK block received "
                "(%"U32_F", %"U32_F")\n", g_sack_blocks[i].start_seq, g_sack_blocks[i].end_seq));
          }
        }
      }
    }
  }

  if (new_sack_blocks) {
    pcb->high_data = pcb->snd_nxt - 1;
  }
  return new_sack_blocks;
}

/**
 * IsLost () function implementation as per RFC 6675.
 * This routine checks whether the particular seg from unacked list is lost or not.
 *
 * Called by tcp_sack_set_pipe() and tcp_sack_get_next_seg().
 *
 * @param pcb the tcp_pcb of this connection
 * @param seg the segment from unacked list which needs to be found whether
 * its lost or not
 *
 * @return 1 on success and 0 on failure
 */
int
tcp_sack_is_lost(const struct tcp_pcb *pcb, struct tcp_seg *seg)
{
  struct tcp_seg *lseg;
  u32_t sacked_seg_len = 0;
  u32_t discontiguous_blocks = 0;

  if ((seg == NULL) || (seg->state & TF_SEG_SACKED)) {
    /* If this seg is already sacked, then its not lost. so return failure */
    return 0;
  }

#if LWIP_FACK_THRESHOLD_BASED_FR
  /* as per Forward ACK, the segment whose distance was 3 * MSS far more than the most forward
     sequence (pcb->fack) should be deemed lost
  */
  if ((pcb->fack - (ntohl(seg->tcphdr->seqno) + (u32_t)(TCP_TCPLEN(seg)) - 1)) > (DUPACK_THRESH * pcb->mss)) {
    return 1;
  }
#endif /* LWIP_FACK_THRESHOLD_BASED_FR */

  /* As per IsLost () definition in RFC 6675 */
  for (lseg = seg->next; lseg != NULL; lseg = lseg->next) {
    if (lseg->state & TF_SEG_SACKED) {
      sacked_seg_len += TCP_TCPLEN(lseg);
      discontiguous_blocks++;
      if ((discontiguous_blocks == DUPACK_THRESH) || (sacked_seg_len > ((DUPACK_THRESH - 1) * pcb->mss))) {
        return 1;
      }
    }
  }

  return 0;
}

/**
 * SetPipe() function implementation as per RFC 6675.
 * This routine traverses the sequence space from HighACK to HighData
 * and MUST set the "pipe" variable to an estimate of the number of
 * octets that are currently in transit between the TCP sender and
 * the TCP receiver.
 *
 * Called by tcp_sack_based_fast_rexmit_alg() and tcp_receive().
 *
 * @param pcb the tcp_pcb of this connection
 */
void
tcp_sack_set_pipe(struct tcp_pcb *pcb)
{
  struct tcp_seg *seg;
  int seg_lost;

  pcb->pipe = 0;

  for (seg = pcb->unacked; seg != NULL; seg = seg->next) {
    /* Pipe needs to be incremented for the retransmitted segments */
    /* As per point b, in SetPipe() definition in Section 4 of RFC 6675 */
    if (seg->state & TF_SEG_SACKED) {
      continue;
    }

    if (seg->state & TF_SEG_RETRANSMITTED) {
      pcb->pipe += TCP_TCPLEN(seg);
    }

    seg_lost = tcp_sack_is_lost(pcb, seg);
    /* Pipe needs to be incremented for the segments which are not lost */
    /* As per point a, in SetPipe() definition in Section 4 of RFC 6675 */
    if (!seg_lost) {
      pcb->pipe += TCP_TCPLEN(seg);
    }
  }

  LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_set_pipe: Pipe is %"U32_F"\n", pcb->pipe));
}

/* Smallest unsacked seg which meets below 3 criteria */
/* 1.a seg is greater than HighRxt */
/* 1.b seg is lesser than highest octet covered by any received SACK */
/* 1.c IsLost(2) returns true */
/* As per point 1, in NextSeg() of RFC 6675 */
static struct tcp_seg *
tcp_sack_next_if_unsacked_and_lost(struct tcp_pcb *pcb, u32_t next_seg_type)
{
  struct tcp_seg *seg = NULL;
  for (seg = pcb->next_seg_for_lr;
      ((seg != NULL) && (seg->tcphdr != NULL) && (TCP_SEQ_LT(ntohl(seg->tcphdr->seqno), pcb->high_sacked)));
      seg = seg->next) {
    LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_get_next_seg: UNSACKED_AND_LOST_SEG "
                                  "check on seg %"U32_F"\n", (u32_t)ntohl(seg->tcphdr->seqno)));
    /* 1.a and 1.c are done here. 1.b is already done in for loop */
    if ((!(seg->state & TF_SEG_SACKED)) &&
        TCP_SEQ_GT(ntohl(seg->tcphdr->seqno), pcb->high_rxt) && /* 1.a */
        (tcp_sack_is_lost(pcb, seg))) { /* 1.c */
#if DRIVER_STATUS_CHECK
      if (pcb->drv_status == DRV_NOT_READY) {
        seg->seg_type = next_seg_type;
      }
#else
      (void)next_seg_type;
#endif
      return seg;
    }
  }
  return NULL;
}

/* Smallest unsacked seg which meets below 2 cirteria */
/* 1.a seg is greater than HighRxt */
/* 1.b seg is lesser than highest octet covered by any received SACK */
/* As per point 3, in NextSeg() of RFC 6675 */
static struct tcp_seg *
tcp_sack_next_if_unsacked(struct tcp_pcb *pcb, u32_t next_seg_type)
{
  struct tcp_seg *seg = NULL;
  for (seg = pcb->next_seg_for_lr;
       ((seg != NULL) && (seg->tcphdr != NULL) && (TCP_SEQ_LT(ntohl(seg->tcphdr->seqno), pcb->high_sacked)));
       seg = seg->next) {
    LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_get_next_seg: UNSACKED_SEG "
                                 "check on seg %"U32_F"\n", (u32_t)ntohl(seg->tcphdr->seqno)));
    /* 1.a is done here. 1.b is already done in for loop */
    if (((seg->state & TF_SEG_SACKED) == 0) &&
        TCP_SEQ_GT(ntohl(seg->tcphdr->seqno), pcb->high_rxt)) {
#if DRIVER_STATUS_CHECK
      if (pcb->drv_status == DRV_NOT_READY) {
        seg->seg_type = next_seg_type;
      }
#else
      (void)next_seg_type;
#endif
      return seg;
    }
  }
  return NULL;
}

/* If HighACK > RescueRxt then return the seg */
/* As per point 4, in NextSeg() of RFC 6675 */
static struct tcp_seg *
tcp_sack_next_if_rescue(struct tcp_pcb *pcb, u32_t next_seg_type)
{
  struct tcp_seg *seg = NULL;
  if (TCP_SEQ_GT(pcb->lastack, pcb->rescue_rxt)) {
    for (seg = pcb->next_seg_for_lr; seg != NULL; seg = seg->next) {
      LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_get_next_seg: RESCUE_RX_SEG "
                                    "check on seg %"U32_F"\n", (u32_t)ntohl(seg->tcphdr->seqno)));
      if (((seg->state & TF_SEG_SACKED) == 0) && ((seg->state & TF_SEG_RETRANSMITTED) == 0)) {
        /* Then set RescueRxt to Recovery point */
        pcb->rescue_rxt = pcb->recovery_point;
#if DRIVER_STATUS_CHECK
        if (pcb->drv_status == DRV_NOT_READY) {
          seg->seg_type = next_seg_type;
        }
#else
        (void)next_seg_type;
#endif
        return seg;
      }
    }
  }
  return NULL;
}
/**
 * NextSeg() function implementation as per RFC 6675.
 * This routine finds the next segment that can be retransmitted
 * during loss recovery phase based on SACK and pipe.
 *
 * Called by tcp_sack_based_loss_recovery_alg().
 *
 * @param pcb the tcp_pcb of this connection
 * @param next_seg_typ the type of next segment which needs to be found
 *
 * @result returns valid segment if its found or it returns NULL
 */
struct tcp_seg *
tcp_sack_get_next_seg(struct tcp_pcb *pcb, u32_t next_seg_type)
{
  switch (next_seg_type) {
    case UNSACKED_AND_LOST_SEG:
      return tcp_sack_next_if_unsacked_and_lost(pcb, next_seg_type);
    case UNSACKED_SEG:
      return tcp_sack_next_if_unsacked(pcb, next_seg_type);
    case RESCUE_RX_SEG:
      return tcp_sack_next_if_rescue(pcb, next_seg_type);
    default:
      return NULL;
  }
}

#if LWIP_SACK_PERF_OPT
void tcp_sack_rexmit_lost_rexmitted(struct tcp_pcb *pcb)
{
  struct tcp_seg *seg = NULL;
  struct tcp_sack_fast_rxmited *fr_seg = NULL;

  for (fr_seg = pcb->fr_segs; fr_seg != NULL; fr_seg = fr_seg->next) {
    seg = fr_seg->seg;
    if ((seg != NULL) && (!(seg->state & TF_SEG_SACKED)) && (seg->state & TF_SEG_RETRANSMITTED) &&
        (TCP_SEQ_GEQ(pcb->high_sacked_pkt_seq, seg->pkt_trans_seq_cntr + 1))) {
      (void)tcp_output_segment(seg, pcb, NULL);
      LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_rexmit_lost_rexmitted: Retransmitted a "
                                   "possibly lost retranmsitted seg %"U32_F" of length %"U32_F" \n",
                                   ntohl(seg->tcphdr->seqno), TCP_TCPLEN(seg)));

      seg->state |= TF_SEG_RETRANSMITTED;

      /* Pipe needs to be incremented for the retransmitted seg
       * As per point C.4 in Section 5 of RFC 6675
       * No need to change pipe, since it was not considered as lost,
       * No change in calucation as it is retransmitted again
       * FRLR state, so no need to increment nrtx
       * Don't take any rtt measurements after retransmitting. */
      pcb->rttest = 0;
    }
  }

  return;
}
#endif

/**
 * Implemntation of loss recovery algorithm as per RFC 6675.
 *
 * Called by tcp_receive().
 *
 * @param pcb the tcp_pcb of this connection
 */
void
tcp_sack_based_loss_recovery_alg(struct tcp_pcb *pcb)
{
  struct tcp_seg *seg;
  u32_t next_seg_type;
#if LWIP_SACK_PERF_OPT
  struct tcp_sack_fast_rxmited *fast_retx = NULL;
#endif

  LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_loss_recovery_alg: Entered SACK "
      "based loss recovery phase\n"));

#if LWIP_SACK_PERF_OPT
  tcp_sack_rexmit_lost_rexmitted(pcb);
#endif

  /* Move the next_seg_for_lr to point first unsacked and  not retransmitted seg */
  for (seg = pcb->next_seg_for_lr; seg != NULL; seg = seg->next) {
    if (!(seg->state & TF_SEG_SACKED)) {
      break;
    }
  }

  pcb->next_seg_for_lr = seg;

  /* If nothing is there, then no need to do Loss recovery */
  if (!pcb->next_seg_for_lr) {
    LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_loss_recovery_alg: No seg "
        "available for loss recovery\n"));
    return;
  }

  LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_loss_recovery_alg: Next seg for "
      "loss recovery is %"U32_F"\n", ntohl(pcb->next_seg_for_lr->tcphdr->seqno)));

  while ((pcb->cwnd >= pcb->pipe) && ((pcb->cwnd - pcb->pipe) >= pcb->mss)) {
    next_seg_type = UNSACKED_AND_LOST_SEG;
    do {
      switch (next_seg_type) {
        case UNSACKED_AND_LOST_SEG:
          /* Try to find unsacked and lost seg */
          seg = tcp_sack_get_next_seg(pcb, UNSACKED_AND_LOST_SEG);
          if (seg) {
#if DRIVER_STATUS_CHECK
            /* Send the segment out only if the driver is ready */
            if (pcb->drv_status == DRV_READY)
#endif
            {
              (void)tcp_output_segment(seg, pcb, NULL);
              LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_loss_recovery_alg: Retransmitted a "
                  "seg %"U32_F" of length %"U32_F" which is UNSACKED_AND_LOST_SEG\n",
                  ntohl(seg->tcphdr->seqno), TCP_TCPLEN(seg)));
            }
#if DRIVER_STATUS_CHECK
            else {
              LWIP_DEBUGF(DRV_STS_DEBUG, ("tcp_sack_based_loss_recovery_alg: Driver busy for "
                                          "seg %"U32_F" of length %"U32_F" which is UNSACKED_AND_LOST_SEG\n",
                                          ntohl(seg->tcphdr->seqno), TCP_TCPLEN(seg)));
            }
#endif
          } else {
            next_seg_type = UNSENT_SEG;
          }
          break;
        case UNSENT_SEG:
          if (pcb->unsent) {
            seg = pcb->unsent;
            /* If unsent data available and receiver's advertise window allows */
            /* then return that segment */
            /* As per point 2, in NextSeg() of RFC 6675 */
            (void)tcp_output(pcb);

            if (TCP_SEQ_GT(pcb->snd_nxt, pcb->high_data + 1)) {
              /* If any of the data octets send are above HighData */
              /* HighData must be updated */
              /* As per point C.3 in Section 5 of RFC 6675 */
              pcb->high_data = pcb->snd_nxt - 1;
              LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_loss_recovery_alg: Transmitted an "
                  "UNSENT_SEG, send next is %"U32_F"\n", pcb->snd_nxt));
            } else {
              /* tcp_output didn't send any new seg */
              seg = NULL;
              next_seg_type = UNSACKED_SEG;
            }
          } else {
            seg = NULL;
            next_seg_type = UNSACKED_SEG;
          }
          break;
        case UNSACKED_SEG:
          seg = tcp_sack_get_next_seg(pcb, UNSACKED_SEG);
          if (seg) {
#if DRIVER_STATUS_CHECK
            /* Send the segment out only if the driver is ready */
            if (pcb->drv_status == DRV_READY)
#endif
            {
              (void)tcp_output_segment(seg, pcb, NULL);
              LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_loss_recovery_alg: Retransmitted a "
                  "seg %"U32_F" of length %"U32_F" which is UNSACKED_SEG\n",
                  ntohl(seg->tcphdr->seqno), TCP_TCPLEN(seg)));
            }
#if DRIVER_STATUS_CHECK
            else {
              LWIP_DEBUGF(DRV_STS_DEBUG, ("tcp_sack_based_loss_recovery_alg: Driver busy for "
                                          "seg %"U32_F" of length %"U32_F" which is UNSACKED_SEG\n",
                                          ntohl(seg->tcphdr->seqno), TCP_TCPLEN(seg)));
            }
#endif
          } else {
            next_seg_type = RESCUE_RX_SEG;
          }
          break;
        case RESCUE_RX_SEG:
          seg = tcp_sack_get_next_seg(pcb, RESCUE_RX_SEG);
          if (seg) {
#if DRIVER_STATUS_CHECK
            /* Send the segment out only if the driver is ready */
            if (pcb->drv_status == DRV_READY)
#endif
            {
              (void)tcp_output_segment(seg, pcb, NULL);
              LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_loss_recovery_alg: Retransmitted a "
                  "seg %"U32_F" of length %"U32_F" which is RESCUE_RX_SEG\n",
                  ntohl(seg->tcphdr->seqno), TCP_TCPLEN(seg)));
            }
#if DRIVER_STATUS_CHECK
            else {
              LWIP_DEBUGF(DRV_STS_DEBUG, ("tcp_sack_based_loss_recovery_alg: Driver busy for "
                                          "seg %"U32_F" of length %"U32_F" which is RESCUE_RX_SEG\n",
                                          ntohl(seg->tcphdr->seqno), TCP_TCPLEN(seg)));
            }
#endif
          } else {
            next_seg_type = 0;
          }
          break;
        default:
          break;
      }
    } while ((seg == NULL) && (next_seg_type != 0));

    /* If no segment has been retransmitted, then return from here */
    if (!seg) {
      break;
    }

    /* If any of the data octets send are below HighData */
    /* HighRxt needs to be set to the highest seq no of the retransmitted seg */
    /* As per point C.2 in Section 5 of RFC 6675 */
    if ((next_seg_type == UNSACKED_AND_LOST_SEG)
        || (next_seg_type == UNSACKED_SEG)
        || (next_seg_type == RESCUE_RX_SEG)) {
      pcb->high_rxt = ntohl(seg->tcphdr->seqno) + TCP_TCPLEN(seg) - 1;
      pcb->next_seg_for_lr = seg->next;
      /* Enabling TF_SEG_RETRANSMITTED */
      /* This will be required to count pipe */
      seg->state |= TF_SEG_RETRANSMITTED;

#if LWIP_SACK_PERF_OPT
      fast_retx = (struct tcp_sack_fast_rxmited *)mem_malloc(sizeof(struct tcp_sack_fast_rxmited));
      if (fast_retx != NULL) {
        fast_retx->seg = seg;
        fast_retx->next = NULL;
        /* If no segments were fast retransmitted previously, then last_frseg will be NULL */
        if (pcb->last_frseg == NULL) {
          pcb->fr_segs = fast_retx;
          /* Update last_frseg to this segment */
          pcb->last_frseg = pcb->fr_segs;
        } else {
          /* Append the segment to the latest fast retransmitted segment */
          pcb->last_frseg->next = fast_retx;
          pcb->last_frseg = pcb->last_frseg->next;
        }
      } else {
        LWIP_DEBUGF(TCP_SACK_DEBUG, ("Memory allocation failure for fr_segs\n"));
      }
#endif

      /* Pipe needs to be incremented for the retransmitted seg */
      /* As per point C.4 in Section 5 of RFC 6675 */
      pcb->pipe += (u32_t)(TCP_TCPLEN(seg));

      /* Don't take any rtt measurements after retransmitting. */
      pcb->rttest = 0;
    }

    LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_loss_recovery_alg: Pipe is %"U32_F
        " RecoveryPoint is %"U32_F" HighSacked is %"U32_F" HighData is %"U32_F
        " HighRxt is %"U32_F" RescueRxt is %"U32_F"\n", pcb->pipe, pcb->recovery_point,
        pcb->high_sacked, pcb->high_data, pcb->high_rxt, pcb->rescue_rxt));
  }

  LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_loss_recovery_alg: Leaving SACK "
      "based loss recovery phase\n"));
}

/**
 * Initiates fast retransmit as per point 4 in Section 5 of RFC 6675
 * It initialize recovery_point  and enters fast retransmit.
 *
 * Called by tcp_receive() for fast retramsmit in case of SACK.
 *
 * @param pcb the tcp_pcb for which to retransmit the first unacked segment
 */
void
tcp_sack_based_fast_rexmit_alg(struct tcp_pcb *pcb)
{
  tcpwnd_size_t ssthresh;

  /* If it entered to retrasnmit timeout from loss recovery, then it should not enter again to loss recovery phase */
  /* untill we receive a cumulative ack for recovery_point */
  if ((!(pcb->flags & TF_IN_SACK_FRLR)) && (!(pcb->flags & TF_IN_SACK_RTO))) {
    LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_fast_rexmit_alg: Started SACK based "
        "fast retransmit\n"));
    /* Assigning HighData to recovery_point, If cumulative ACK is received */
    /* for this recovery_point, then that shoud be the end of loss recovery phase */
    /* As per point 4.1 in section 5 of RFC 6675 */
    pcb->recovery_point = pcb->high_data;

    /* As per point 4.2 in section 5 of RFC 6675 */
    /* Set ssthresh to half of the minimum of the current
     * cwnd and the advertised window */

#if LWIP_SACK_CWND_OPT
    pcb->recover_cwnd = pcb->cwnd;
    pcb->recover_ssthresh = pcb->ssthresh;
    LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_fast_rexmit_alg: recover_cwnd is %"TCPWNDSIZE_F""
                                 "and ssthresh is %"TCPWNDSIZE_F"\n", pcb->cwnd, pcb->ssthresh));
#endif
    if (pcb->cwnd > pcb->snd_wnd) {
      pcb->ssthresh = (tcpwnd_size_t)(((u64_t)pcb->snd_wnd * 7) >> 3);
    } else {
      pcb->ssthresh = (tcpwnd_size_t)(((u64_t)pcb->cwnd * 7) >> 3);
    }

    /* The minimum value for ssthresh should be 2 MSS */
    if (pcb->ssthresh < 2 * pcb->mss) {
      LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_fast_rexmit_alg: The minimum "
           "value for ssthresh %"TCPWNDSIZE_F" should be min 2 mss %"TCPWNDSIZE_F"\n",
           pcb->ssthresh, 2 * pcb->mss));
      pcb->ssthresh = 2 * pcb->mss;
    }

    pcb->cwnd = pcb->ssthresh;
    if (pcb->cwnd < pcb->iw) {
      pcb->cwnd = pcb->iw;
    }

    TCP_CALC_SSTHRESH(pcb, ssthresh, INITIAL_SSTHRESH, pcb->mss);
    if (pcb->ssthresh < ssthresh) {
      pcb->ssthresh = ssthresh;
    }

    LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_fast_rexmit_alg: cwnd is %"TCPWNDSIZE_F
        " ssthresh is %"TCPWNDSIZE_F"\n", pcb->cwnd, pcb->ssthresh));

    /* Retransmit the first data segment, starting with sequence number HighACK + 1 */
    /* As per point 4.3 in section 5 of RFC 6675 */
#if DRIVER_STATUS_CHECK
    /* Send the segment out only if the driver is ready */
    if (pcb->drv_status == DRV_READY) {
      (void)tcp_output_segment(pcb->unacked, pcb, NULL);
    } else {
      pcb->unacked->seg_type = FAST_RETX_SEG;
      LWIP_DEBUGF(DRV_STS_DEBUG, ("tcp_sack_based_fast_rexmit_alg: Driver busy so delaying.\n"));
    }
#else
    (void)tcp_output_segment(pcb->unacked, pcb, NULL);
#endif

    /* Set both HighRxt and RescueRxt to the highest sequence number in the retransmitted segment. */
    /* As per point 4.3 in Section 5 of RFC 6675.  */
    pcb->high_rxt = ntohl(pcb->unacked->tcphdr->seqno) + TCP_TCPLEN(pcb->unacked) - 1;
    pcb->rescue_rxt = pcb->high_rxt;
    pcb->next_seg_for_lr = pcb->unacked->next;

    /* Enabling TF_SEG_RETRANSMITTED */
    /* This will be required to count pipe */
    pcb->unacked->state |= TF_SEG_RETRANSMITTED;

#if LWIP_SACK_PERF_OPT
    if (pcb->fr_segs != NULL) {
      /* Free if an segments are stored in the fr_segs. Ideally there
         should be no segments at this instance */
      tcp_fr_segs_free(pcb->fr_segs);
    }

    pcb->fr_segs = (struct tcp_sack_fast_rxmited *)mem_malloc(sizeof(struct tcp_sack_fast_rxmited));
    if (pcb->fr_segs != NULL) {
      pcb->fr_segs->seg = pcb->unacked;
      pcb->fr_segs->next = NULL;
      pcb->last_frseg = pcb->fr_segs;
    } else {
      LWIP_DEBUGF(TCP_SACK_DEBUG, ("Memory allocation failure for fr_segs\n"));
    }
#endif

    /* Similar to tcp_rexmit() function */
    ++pcb->nrtx;

    /* Don't take any rtt measurements after retransmitting. */
    pcb->rttest = 0;

    LWIP_DEBUGF(TCP_SACK_DEBUG, ("tcp_sack_based_fast_rexmit_alg: Pipe is %"U32_F
        " RecoveryPoint is %"U32_F" HighSacked is %"U32_F" HighData is %"U32_F
        " HighRxt is %"U32_F" RescueRxt is %"U32_F"\n", pcb->pipe, pcb->recovery_point,
        pcb->high_sacked, pcb->high_data, pcb->high_rxt, pcb->rescue_rxt));

    pcb->flags |= TF_IN_SACK_FRLR;
  }
}

#endif
