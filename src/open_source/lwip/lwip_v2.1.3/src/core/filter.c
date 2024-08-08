/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: implementation for socket filter
 * Author: none
 * Create: 2020
 */

#include "lwip/opt.h"

#if LWIP_SOCK_FILTER

#include <string.h>
#include "lwip/filter.h"
#include "lwip/api.h"

#define get_unaligned_be16(p)   (((((u8_t*)p)[0]) << 8) | (((u8_t*)p)[1]))
#define get_unaligned_be32(p)   ((((u8_t*)p)[0] << 24) | (((u8_t*)p)[1] << 16) |(((u8_t*)p)[2] << 8) | (((u8_t*)p)[3]))

static void *pbuf_header_pointer(struct pbuf *pbuf, u16_t offset, u16_t len, void *buffer)
{
  if (pbuf == NULL) {
    return NULL;
  }
  /* copy from the first pbuf in the chain */
  if (pbuf->len - offset >= len) {
    return (u8_t*)pbuf->payload + offset;
  }

  /* handle the chained pbufs */
  if (pbuf_copy_partial(pbuf, buffer, len, offset)) {
    return buffer;
  }

  return NULL;
}

static inline void *load_pointer(struct pbuf *pbuf, u16_t k, u16_t size, void *buffer)
{
  return pbuf_header_pointer(pbuf, k, size, buffer);
}

/*
 *  sock_run_filter - run a filter on a socket
 *  @pbuf: buffer to run the filter on
 *  @filter: filter to apply
 *  @flen: length of filter
 *
 * Decode and apply filter instructions to the pbuf->payload.
 * Return length to keep, 0 for none. pbuf is the data we are
 * filtering, filter is the array of filter instructions, and
 * len is the number of filter blocks in the array.
 */
u32_t sock_run_filter(struct pbuf *pbuf, struct sock_filter *filter, u16_t len)
{
  void *ptr = NULL;
  u32_t X = 0;
  u32_t A = 0;
  u32_t mem[LSF_MEMWORDS] = {0};
  u32_t tmp;
  u32_t pc;
  u32_t k;
  struct sock_filter *entry = NULL;
  if (pbuf == NULL) {
    return 0;
  }
  for (pc = 0; pc < len; pc++) {
    entry = &filter[pc];

    switch (entry->code) {
      case LSF_JMP | LSF_JA:
        pc += entry->k;
        continue;
      case LSF_JMP | LSF_JGT | LSF_K:
        pc += (u32_t)((A > entry->k) ? entry->jt : entry->jf);
        continue;
      case LSF_JMP | LSF_JGE | LSF_K:
        pc += (u32_t)((A >= entry->k) ? entry->jt : entry->jf);
        continue;
      case LSF_JMP | LSF_JEQ | LSF_K:
        pc += (u32_t)((A == entry->k) ? entry->jt : entry->jf);
        continue;
      case LSF_JMP | LSF_JSET | LSF_K:
        pc += (u32_t)((A & entry->k) ? entry->jt : entry->jf);
        continue;
      case LSF_JMP | LSF_JGT | LSF_X:
        pc += (u32_t)((A > X) ? entry->jt : entry->jf);
        continue;
      case LSF_JMP | LSF_JGE | LSF_X:
        pc += (u32_t)((A >= X) ? entry->jt : entry->jf);
        continue;
      case LSF_JMP | LSF_JEQ | LSF_X:
        pc += (u32_t)((A == X) ? entry->jt : entry->jf);
        continue;
      case LSF_JMP | LSF_JSET | LSF_X:
        pc += (u32_t)((A & X) ? entry->jt : entry->jf);
        continue;
      case LSF_ALU | LSF_ADD | LSF_X:
        A += X;
        continue;
      case LSF_ALU | LSF_ADD | LSF_K:
        A += entry->k;
        continue;
      case LSF_ALU | LSF_SUB | LSF_X:
        A -= X;
        continue;
      case LSF_ALU | LSF_SUB | LSF_K:
        A -= entry->k;
        continue;
      case LSF_ALU | LSF_MUL | LSF_X:
        A *= X;
        continue;
      case LSF_ALU | LSF_MUL | LSF_K:
        A *= entry->k;
        continue;
      case LSF_ALU | LSF_DIV | LSF_X:
        if (X == 0) {
          return 0;
        }
        A /= X;
        continue;
      case LSF_ALU | LSF_DIV | LSF_K:
        A /= entry->k;
        continue;
      case LSF_ALU | LSF_AND | LSF_X:
        A &= X;
        continue;
      case LSF_ALU | LSF_AND | LSF_K:
        A &= entry->k;
        continue;
      case LSF_ALU | LSF_OR | LSF_X:
        A |= X;
        continue;
      case LSF_ALU | LSF_OR | LSF_K:
        A |= entry->k;
        continue;
      case LSF_ALU | LSF_LSH | LSF_X:
        A <<= X;
        continue;
      case LSF_ALU | LSF_LSH | LSF_K:
        A <<= entry->k;
        continue;
      case LSF_ALU | LSF_RSH | LSF_X:
        A >>= X;
        continue;
      case LSF_ALU | LSF_RSH | LSF_K:
        A >>= entry->k;
        continue;
      case LSF_ALU | LSF_NEG:
        A = (u32_t)(-(int32_t)A);
        continue;
      case LSF_LD | LSF_W | LSF_ABS:
        k = entry->k;
load_w:
        ptr = load_pointer(pbuf, (u16_t)k, 4, &tmp); // read 4 bytes from pbuf to tmp by offset k
        if (ptr != NULL) {
          A = (u32_t)get_unaligned_be32(ptr);
          continue;
        }
        break;
      case LSF_LD | LSF_H | LSF_ABS:
        k = entry->k;
load_h:
        ptr = load_pointer(pbuf, (u16_t)k, 2, &tmp); // read 2 bytes from pbuf to tmp by offset k
        if (ptr != NULL) {
          A = (u32_t)get_unaligned_be16(ptr);
          continue;
        }
        break;
      case LSF_LD | LSF_B | LSF_ABS:
        k = entry->k;
load_b:
        ptr = load_pointer(pbuf, (u16_t)k, 1, &tmp); // read 1 bytes from pbuf to tmp by offset k
        if (ptr != NULL) {
          A = *(u8_t *)ptr;
          continue;
        }
        break;
      case LSF_LD | LSF_W | LSF_LEN:
        A = pbuf->tot_len;
        continue;
      case LSF_LDX | LSF_W | LSF_LEN:
        X = pbuf->tot_len;
        continue;
      case LSF_LD | LSF_W | LSF_IND:
        k = X + entry->k;
        goto load_w;
      case LSF_LD | LSF_H | LSF_IND:
        k = X + entry->k;
        goto load_h;
      case LSF_LD | LSF_B | LSF_IND:
        k = X + entry->k;
        goto load_b;
      case LSF_LD | LSF_IMM:
        A = entry->k;
        continue;
      case LSF_LDX | LSF_IMM:
        X = entry->k;
        continue;
      case LSF_LD | LSF_MEM:
        A = mem[entry->k];
        continue;
      case LSF_LDX | LSF_MEM:
        X = mem[entry->k];
        continue;
      case LSF_MISC | LSF_TAX:
        X = A;
        continue;
      case LSF_MISC | LSF_TXA:
        A = X;
        continue;
      case LSF_ST:
        mem[entry->k] = A;
        continue;
      case LSF_STX:
        mem[entry->k] = X;
        continue;
      case LSF_RET | LSF_K:
        return entry->k;
      case LSF_RET | LSF_A:
        return A;
      default:
        return 0;
    }
  }

  return 0;
}

/*
 *  sock_filter - run a packet through a socket filter
 *
 * Run the filter code and then cut pbuf->payload to correct size returned by
 * sk_run_filter. If pkt_len is 0 we toss packet. If pbuf->tot_len is smaller
 * than pkt_len we keep whole pbuf->payload. This is the socket level
 * wrapper to sk_run_filter. It returns 0 if the packet should
 * be accepted or EPERM if the packet should be tossed.
 *
 */
s32_t sock_filter(struct netconn *conn, struct pbuf *pbuf)
{
  s32_t err = ERR_OK;
  u32_t pkt_len;

  if (conn->sk_filter.filter != NULL) {
    pkt_len = sock_run_filter(pbuf, conn->sk_filter.filter, conn->sk_filter.len);
    if (pkt_len == 0) {
      err = EPERM;
    }
  }

  return err;
}

/*
 *  sock_check_filter - verify socket filter code
 *  @filter: filter to be verified
 *  @flen: filter length
 *
 *  make sure the user filter code was legal, checking include
 *  1) the filter must contain no illegal instructions,
 *  2) no references or jumps that are out of range,
 *  3) and must end with a RET instruction.
 *
 *  All jumps are forward because they are not signed.
 *
 *  return 0 if the filter is legal, EINVAL if not.
 */
s32_t sock_check_filter(struct sock_filter *filter, s32_t len)
{
  struct sock_filter *entry = NULL;
  s32_t pc;

  if (len < 1 || len > LSF_MAXINSNS) {
    return EINVAL;
  }

  for (pc = 0; pc < len; pc++) {
    entry = &filter[pc];

    switch (entry->code) {
      case LSF_LD | LSF_W | LSF_ABS:
      case LSF_LD | LSF_H | LSF_ABS:
      case LSF_LD | LSF_B | LSF_ABS:
      case LSF_LD | LSF_W | LSF_LEN:
      case LSF_LD | LSF_W | LSF_IND:
      case LSF_LD | LSF_H | LSF_IND:
      case LSF_LD | LSF_B | LSF_IND:
      case LSF_LD | LSF_IMM:
      case LSF_LDX | LSF_W | LSF_LEN:
      case LSF_LDX | LSF_IMM:
      case LSF_ALU | LSF_ADD | LSF_K:
      case LSF_ALU | LSF_ADD | LSF_X:
      case LSF_ALU | LSF_SUB | LSF_K:
      case LSF_ALU | LSF_SUB | LSF_X:
      case LSF_ALU | LSF_MUL | LSF_K:
      case LSF_ALU | LSF_MUL | LSF_X:
      case LSF_ALU | LSF_DIV | LSF_X:
      case LSF_ALU | LSF_AND | LSF_K:
      case LSF_ALU | LSF_AND | LSF_X:
      case LSF_ALU | LSF_OR | LSF_K:
      case LSF_ALU | LSF_OR | LSF_X:
      case LSF_ALU | LSF_LSH | LSF_K:
      case LSF_ALU | LSF_LSH | LSF_X:
      case LSF_ALU | LSF_RSH | LSF_K:
      case LSF_ALU | LSF_RSH | LSF_X:
      case LSF_ALU | LSF_NEG:
      case LSF_RET | LSF_K:
      case LSF_RET | LSF_A:
      case LSF_MISC | LSF_TAX:
      case LSF_MISC | LSF_TXA:
        break;

      /* special checks needed for following instructions */
      case LSF_ALU | LSF_DIV | LSF_K:
        if (entry->k == 0) {
          return EINVAL;
        }
        break;

      case LSF_LD | LSF_MEM:
      case LSF_LDX | LSF_MEM:
      case LSF_ST:
      case LSF_STX:
        /* invalid memory addresses */
        if (entry->k >= LSF_MEMWORDS) {
          return EINVAL;
        }
        break;
      case LSF_JMP | LSF_JA:
        if (entry->k >= (unsigned)(len - pc - 1)) {
          return EINVAL;
        }
        break;

      case LSF_JMP | LSF_JEQ | LSF_K:
      case LSF_JMP | LSF_JEQ | LSF_X:
      case LSF_JMP | LSF_JGE | LSF_K:
      case LSF_JMP | LSF_JGE | LSF_X:
      case LSF_JMP | LSF_JGT | LSF_K:
      case LSF_JMP | LSF_JGT | LSF_X:
      case LSF_JMP | LSF_JSET | LSF_K:
      case LSF_JMP | LSF_JSET | LSF_X:
        if (pc + entry->jt + 1 >= len || pc + entry->jf + 1 >= len) {
          return EINVAL;
        }
        break;

      default:
        return EINVAL;
    }
  }

  return (LSF_CLASS(filter[len - 1].code) == LSF_RET) ? 0 : EINVAL;
}

/*
 *  sock_attach_filter - attach a user filter
 *  If an error occurs or there is insufficient memory for the filter a posix
 *  errno code is returned. On success the return is zero.
 */
s32_t sock_attach_filter(struct sock_fprog *fprog, struct netconn *conn)
{
  u32_t fsize = sizeof(struct sock_filter) * fprog->len;
  struct sock_filter *fp = NULL;
  s32_t err;

  if (fprog->filter == NULL || fprog->len == 0) {
    return EINVAL;
  }

  err = sock_check_filter(fprog->filter, fprog->len);
  if (err) {
    return err;
  }

  fp = mem_malloc(fsize);
  if (fp == NULL) {
    return ENOMEM;
  }

  (void)memcpy_s(fp, fsize, fprog->filter, fsize);

  if (conn->sk_filter.filter != NULL) {
    mem_free(conn->sk_filter.filter);
    conn->sk_filter.filter = NULL;
    conn->sk_filter.len = 0;
  }

  conn->sk_filter.len = fprog->len;
  conn->sk_filter.filter = fp;
  return 0;
}

s32_t sock_detach_filter(struct netconn *conn)
{
  s32_t ret = ENOENT;

  if (conn->sk_filter.filter) {
    mem_free(conn->sk_filter.filter);
    conn->sk_filter.filter = NULL;
    conn->sk_filter.len = 0;
    ret = 0;
  }
  return ret;
}

#endif  /* LWIP_SOCK_FILTER */
