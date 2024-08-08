/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: implementation for DNS64 RFC6147
 * Author: NA
 * Create: 2019
 */
#include "lwip/opt.h"

#if LWIP_DNS64
#include "lwip/pbuf.h"
#include "lwip/prot/dns.h"
#include "lwip/dns.h"
#include "lwip/mem.h"
#define DNS_QUESTION_TYPE_OFFSET 1
#define DNS_QUESTION_TYPE_V4 0
#define DNS_QUESTION_TYPE_V6 1
#define NAT64_PREFIX_LEN 96
#define NAT64_PREFIX_LEN_IN_BYTE ((NAT64_PREFIX_LEN) >> 3)
#define MAX_ANSWERS 32

static const u8_t g_nat64_prefix[NAT64_PREFIX_LEN_IN_BYTE] = {
  0x64, 0xff, 0x9b, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define DNS64_PACKET_OFFSET_COUNT 10
#define DNS64_PACKET_OFFSET_IP4 0
#define DNS64_PACKET_OFFSET_IP6 1
#define DNS64_PACKET_OFFSET_MAX 1

err_t
nat64_dns64_get_prefix(ip6_addr_t *ip6addr, u8_t *len)
{
  if (ip6addr == NULL) {
    return ERR_ARG;
  }

  (void)memset_s(ip6addr, sizeof(ip6_addr_t), 0, sizeof(ip6_addr_t));
  if (memcpy_s(ip6addr, sizeof(ip6_addr_t), g_nat64_prefix, sizeof(g_nat64_prefix)) != EOK) {
    return ERR_VAL;
  }

  if (len != NULL) {
    *len = NAT64_PREFIX_LEN;
  }

  return ERR_OK;
}

static u16_t
nat64_dns64_query_type(u8_t type)
{
  if (type == DNS_QUESTION_TYPE_V6) {
    return PP_HTONS(DNS_RRTYPE_AAAA);
  }

  return PP_HTONS(DNS_RRTYPE_A);
}

static err_t
nat64_dns64_query_handle(struct pbuf *p, const struct dns_hdr *dnshdr, u8_t type, u16_t *len, u8_t handle)
{
  u16_t i;
  u16_t qtype;
  struct dns_query qry;
  u16_t nquestions;
  u8_t n;
  u16_t offset = 0;

  qtype = nat64_dns64_query_type(type);
  nquestions = lwip_ntohs(dnshdr->numquestions);
  /* only support one query */
  if (nquestions != 1) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: nquestions:%d\n", nquestions));
    return ERR_VAL;
  }

  offset += SIZEOF_DNS_HDR;
  /* for compatible of multi query */
  for (i = 0; i < nquestions; i++) {
    offset = dns_skip_name(p, offset);
    if ((offset == 0) || (offset > p->tot_len)) {
      return ERR_VAL;
    }

    if (handle != lwIP_TRUE) {
      offset += SIZEOF_DNS_QUERY;
      if (offset > p->tot_len) {
        return ERR_VAL;
      }
      continue;
    }
    (void)memset_s(&qry, sizeof(struct dns_query), 0, sizeof(struct dns_query));
    if (pbuf_copy_partial(p, &qry, SIZEOF_DNS_QUERY, offset) != SIZEOF_DNS_QUERY) {
      LWIP_DEBUGF(NAT64_DEBUG, ("dns64: get query fail\n"));
      return ERR_VAL;
    }
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: cls:%x type:%x\n", qry.cls, qry.type));
    if ((qry.cls == PP_HTONS(DNS_RRCLASS_IN)) && (qry.type == qtype)) {
      n = (type == DNS_QUESTION_TYPE_V6) ? DNS_RRTYPE_A : DNS_RRTYPE_AAAA;
      pbuf_put_at(p, offset + DNS_QUESTION_TYPE_OFFSET, n);
    } else {
      return ERR_VAL;
    }
    offset += SIZEOF_DNS_QUERY;
    if (offset > p->tot_len) {
      return ERR_VAL;
    }
  }
  if (len != NULL) {
    *len = offset;
  }
  return ERR_OK;
}

/* just send dns query */
err_t
nat64_dns64_6to4(struct pbuf *p)
{
  struct dns_hdr dnshdr;

  if (p == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: pkt error1\n"));
    return ERR_ARG;
  }
  if (p->tot_len < SIZEOF_DNS_HDR) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: pkt len is too short\n"));
    return ERR_ARG;
  }
  (void)memset_s(&dnshdr, sizeof(struct dns_hdr), 0, sizeof(struct dns_hdr));
  if (pbuf_copy_partial(p, &dnshdr, SIZEOF_DNS_HDR, 0) != SIZEOF_DNS_HDR) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: copy dns hdr failed\n"));
    return ERR_VAL;
  }
  if (nat64_dns64_query_handle(p, &dnshdr, DNS_QUESTION_TYPE_V6, NULL, lwIP_TRUE) != ERR_OK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: DNS query modify failed\n"));
    return ERR_VAL;
  }
  return ERR_OK;
}

static int
nat64_dns64_dname_offset_nochange(const struct pbuf *p4, u16_t query_idx, u16_t qlen,
                                  u16_t *index, u8_t *nat64_dname)
{
  int n;
  int m;
  u16_t offset = query_idx;
  u16_t pointer;

  do {
    n = pbuf_try_get_at(p4, offset++);
    if (n < 0) {
      return -1;
    }

    if ((((u8_t)n) & 0xC0) == 0xC0) {
      m = pbuf_try_get_at(p4, offset);
      /* 2: the total length of the domain name cannot exceed 253 characters. */
      if ((m < 0) || (*index > (DNS_MAX_NAME_LENGTH - 2))) {
        return -1;
      }
      /* the offset include two bytes, and the first byte just include 6 bits. */
      pointer = (u16_t)(((((u8_t)n) & 0x3f) << 8) | ((u8_t)m));
      if (pointer >= qlen) {
        return nat64_dns64_dname_offset_nochange(p4, pointer, qlen, index, nat64_dname);
      } else {
        nat64_dname[(*index)++] = (u8_t)n;
        nat64_dname[(*index)++] = (u8_t)m;
        return 1;
      }
    } else if (n > DNS_MAX_LABEL_LENGTH) {
      return -1;
    } else if ((offset + n) >= p4->tot_len) {
      /* Not compressed name */
      return -1;
    } else {
      if ((*index > (DNS_MAX_NAME_LENGTH - n - 1)) ||
          (pbuf_copy_partial(p4, &nat64_dname[*index], (u16_t)(n + 1), (u16_t)(offset - 1)) != n + 1)) {
        return -1;
      }
      offset = (u16_t)(offset + n);
      *index += (u16_t)(n + 1);
    }
    n = pbuf_try_get_at(p4, offset);
    if (n < 0) {
      return -1;
    }
  } while (n != 0);
  nat64_dname[(*index)++] = (u8_t)n;
  return 1;
}

static err_t
nat64_dns64_answer_dname_copy(const struct pbuf *p4,
                              u16_t nameoffset, u16_t qlen, u16_t *offset6, struct pbuf *p6)
{
  int ret;
  u16_t name_index = 0;
  u8_t *p_nat64_dname = (u8_t *)mem_malloc(sizeof(u8_t) * DNS_MAX_NAME_LENGTH);
  if (p_nat64_dname == NULL) {
    return ERR_MEM;
  }
  (void)memset_s(p_nat64_dname, DNS_MAX_NAME_LENGTH, 0, DNS_MAX_NAME_LENGTH);
  /* find the compression offset, only when all offset is less then */
  ret = nat64_dns64_dname_offset_nochange(p4, nameoffset, qlen, &name_index, p_nat64_dname);
  if (ret < 0) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: nat64_dns64_dname_offset_nochange failed\n"));
    mem_free(p_nat64_dname);
    return ERR_VAL;
  }

  if (pbuf_take_at(p6, p_nat64_dname, name_index, *offset6) != ERR_OK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: pbuf_take_at dname failed\n"));
    mem_free(p_nat64_dname);
    return ERR_VAL;
  }
  *offset6 += name_index;
  mem_free(p_nat64_dname);
  return ERR_OK;
}

static err_t
nat64_dns64_answer_4to6_copy(const struct pbuf *p4,
                             struct dns_answer *ans, u16_t offset4, u16_t *offset6, struct pbuf *p6)
{
  ip4_addr_t ip4addr;
  ans->type = PP_HTONS(DNS_RRTYPE_AAAA);
  ans->len = PP_HTONS((u16_t)sizeof(ip6_addr_t));

  if (pbuf_take_at(p6, ans, SIZEOF_DNS_ANSWER, *offset6) != ERR_OK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: pbuf_take_at answer failed\n"));
    return ERR_VAL;
  }
  *offset6 += SIZEOF_DNS_ANSWER;

  if (pbuf_take_at(p6, g_nat64_prefix, NAT64_PREFIX_LEN_IN_BYTE, *offset6) != ERR_OK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: pbuf_take_at prefix failed\n"));
    return ERR_VAL;
  }
  *offset6 += NAT64_PREFIX_LEN_IN_BYTE;
  (void)memset_s(&ip4addr, sizeof(ip4_addr_t), 0, sizeof(ip4_addr_t));
  if (pbuf_copy_partial(p4, &ip4addr, sizeof(ip4addr), offset4 + SIZEOF_DNS_ANSWER) != sizeof(ip4addr)) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: pbuf_take_at prefix failed\n"));
    return ERR_VAL;
  }

  if (pbuf_take_at(p6, &ip4addr, sizeof(ip4addr), *offset6) != ERR_OK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: pbuf_take_at prefix failed\n"));
    return ERR_VAL;
  }
  *offset6 += sizeof(ip4addr);

  return  ERR_OK;
}

static err_t
nat64_dns64_answer_4to6(const struct pbuf *p4,
                        const struct dns_hdr *dnshdr, u16_t *qlen, u8_t *an, struct pbuf *p6)
{
  u16_t i;
  u16_t nanswers;
  u16_t name_offset;
  u16_t offset4;
  u16_t offset6;
  struct dns_answer ans;

  nanswers = lwip_ntohs(dnshdr->numanswers);
  if ((nanswers == 0) || (nanswers > MAX_ANSWERS)) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64:answer is zero more than MAX_ANSWERS.\n"));
    return ERR_VAL;
  }

  offset4 = *qlen;
  offset6 = *qlen;
  for (i = 0; i < nanswers; i++) {
    name_offset = offset4;
    offset4 = dns_skip_name(p4, offset4);
    if (offset4 == 0) {
      LWIP_DEBUGF(NAT64_DEBUG, ("dns64: get name offset fail\n"));
      return ERR_VAL;
    }
    (void)memset_s(&ans, sizeof(struct dns_answer), 0, sizeof(struct dns_answer));
    if (pbuf_copy_partial(p4, &ans, SIZEOF_DNS_ANSWER, offset4) != SIZEOF_DNS_ANSWER) {
      LWIP_DEBUGF(NAT64_DEBUG, ("dns64: get answer fail\n"));
      return ERR_VAL;
    }
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64:answer cls:%x type:%x\n", ans.cls, ans.type));
    if ((ans.type != PP_HTONS(DNS_RRTYPE_A)) || (ans.cls != PP_HTONS(DNS_RRCLASS_IN)) ||
        (ans.len != PP_HTONS((u16_t)(sizeof(ip4_addr_t))))) {
      LWIP_DEBUGF(NAT64_DEBUG, ("dns64: not A RR\n"));
      offset4 += SIZEOF_DNS_ANSWER + lwip_ntohs(ans.len);
      continue;
    }

    if ((nat64_dns64_answer_dname_copy(p4, name_offset, *qlen, &offset6, p6) != ERR_OK) ||
        (nat64_dns64_answer_4to6_copy(p4, &ans, offset4, &offset6, p6) != ERR_OK)) {
      LWIP_DEBUGF(NAT64_DEBUG, ("dns64: dname copy fail\n"));
      return ERR_VAL;
    }

    /* answer entry add */
    *an += 1;
    offset4 += SIZEOF_DNS_ANSWER + lwip_ntohs(ans.len);
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64:answer ipv6\n"));
  }
  *qlen = offset6;
  return ERR_OK;
}

err_t
nat64_dns64_4to6(const struct pbuf *p4, struct pbuf *p6)
{
  struct dns_hdr dnshdr;
  u16_t qlen = 0;
  u8_t an = 0;

  if ((p4 == NULL) || (p6 == NULL)) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: pkt error\n"));
    return ERR_ARG;
  }
  if (p6->tot_len < SIZEOF_DNS_HDR) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: pkt len is too short\n"));
    return ERR_ARG;
  }
  (void)memset_s(&dnshdr, sizeof(struct dns_hdr), 0, sizeof(struct dns_hdr));
  if (pbuf_copy_partial(p6, &dnshdr, SIZEOF_DNS_HDR, 0) != SIZEOF_DNS_HDR) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: copy dns hdr failed\n"));
    return ERR_VAL;
  }
  if (nat64_dns64_query_handle(p6, &dnshdr, DNS_QUESTION_TYPE_V4, &qlen, lwIP_TRUE) != ERR_OK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: DNS query modify failed\n"));
    return ERR_VAL;
  }

  if (nat64_dns64_answer_4to6(p4, &dnshdr, &qlen, &an, p6) != ERR_OK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: DNS answer modify failed\n"));
    return ERR_VAL;
  }

  dnshdr.numanswers = lwip_htons(an);
  dnshdr.numauthrr = 0;
  dnshdr.numextrarr = 0;
  dnshdr.flags1 &= ~DNS_FLAG1_AUTHORATIVE;
  if (pbuf_take_at(p6, &dnshdr, SIZEOF_DNS_HDR, 0) != ERR_OK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: update the dns header failed\n"));
    return ERR_VAL;
  }
  /* for we just allocate one pbuf for the ipv6 pkt */
  p6->tot_len = qlen;
  p6->len = p6->tot_len;
  return ERR_OK;
}

static err_t
nat64_dns64_answer_acount(const struct pbuf *p4, const struct dns_hdr *dnshdr, const u16_t *qlen, u8_t *an)
{
  u16_t i;
  u16_t nanswers;
  u16_t offset4;
  struct dns_answer ans;

  nanswers = lwip_ntohs(dnshdr->numanswers);
  if ((nanswers == 0) || (nanswers > MAX_ANSWERS)) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64:answer is zero or more than MAX_ANSWERS.\n"));
    return ERR_VAL;
  }

  offset4 = *qlen;
  for (i = 0; i < nanswers; i++) {
    offset4 = dns_skip_name(p4, offset4);
    if (offset4 == 0) {
      LWIP_DEBUGF(NAT64_DEBUG, ("dns64: get name offset fail\n"));
      return ERR_VAL;
    }
    (void)memset_s(&ans, sizeof(struct dns_answer), 0, sizeof(struct dns_answer));
    if (pbuf_copy_partial(p4, &ans, SIZEOF_DNS_ANSWER, offset4) != SIZEOF_DNS_ANSWER) {
      LWIP_DEBUGF(NAT64_DEBUG, ("dns64: get answer fail\n"));
      return ERR_VAL;
    }
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64:answer cls:%x type:%x\n", ans.cls, ans.type));
    if ((ans.type != PP_HTONS(DNS_RRTYPE_A)) || (ans.cls != PP_HTONS(DNS_RRCLASS_IN)) ||
        (ans.len != PP_HTONS((u16_t)(sizeof(ip4_addr_t))))) {
      LWIP_DEBUGF(NAT64_DEBUG, ("dns64: not A RR\n"));
      offset4 += SIZEOF_DNS_ANSWER + lwip_ntohs(ans.len);
      continue;
    }

    /* answer entry add */
    *an += 1;
    offset4 += SIZEOF_DNS_ANSWER + lwip_ntohs(ans.len);
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64:answer ipv6\n"));
  }

  return ERR_OK;
}

err_t
nat64_dns64_extra_count(struct pbuf *p4, u16_t *count)
{
  struct dns_hdr dnshdr;
  u16_t qlen = 0;
  u8_t an = 0;

  if (p4 == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: pkt error\n"));
    return ERR_ARG;
  }
  if (p4->tot_len < SIZEOF_DNS_HDR) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: pkt len is too short\n"));
    return ERR_ARG;
  }
  (void)memset_s(&dnshdr, sizeof(struct dns_hdr), 0, sizeof(struct dns_hdr));
  if (pbuf_copy_partial(p4, &dnshdr, SIZEOF_DNS_HDR, 0) != SIZEOF_DNS_HDR) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: copy dns hdr failed\n"));
    return ERR_VAL;
  }
  if (nat64_dns64_query_handle(p4, &dnshdr, DNS_QUESTION_TYPE_V4, &qlen, lwIP_FALSE) != ERR_OK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: DNS query modify failed\n"));
    return ERR_VAL;
  }

  if (nat64_dns64_answer_acount(p4, &dnshdr, &qlen, &an) != ERR_OK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("dns64: DNS query modify failed\n"));
    return ERR_VAL;
  }

  if (count != NULL) {
    *count = an;
  }

  return ERR_OK;
}
#endif
