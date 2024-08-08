/**
 * @file
 *
 * IPv6 addresses.
 */

/*
 * Copyright (c) 2010 Inico Technologies Ltd.
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
 * Author: Ivan Delamer <delamer@inicotech.com>
 *
 * Functions for handling IPv6 addresses.
 *
 * Please coordinate changes and requests with Ivan Delamer
 * <delamer@inicotech.com>
 */

#include "lwip/opt.h"

#if LWIP_IPV6  /* don't build if not configured for use in lwipopts.h */

#include "lwip/ip_addr.h"
#include "lwip/def.h"
#include "lwip/priv/sockets_priv.h"

#include <string.h>

#if LWIP_IPV4
/* Dual-stack: For ip6addr_aton to handle IPv4-mapped addresses */
#include "lwip/ip4_addr.h"
/* Dual-stack: IPv4 mapped header formatr */
#define IP4MAPPED_HEADER "::FFFF:"
/* Dual-stack: IPv4 compatible header formatr */
#define IPV4COMPATIBLE_HEADER "::"
#endif /* LWIP_IPV4 */

/* used by IP6_ADDR_ANY(6) in ip6_addr.h */
const ip_addr_t ip6_addr_any = IPADDR6_INIT(0ul, 0ul, 0ul, 0ul);

#define lwip_xchar(i)             ((char)((i) < 10 ? '0' + (i) : 'A' + (i) - 10))

#define IP6_ADDR_BLOCK_SIZE_MAX 4

#if LWIP_SMALL_SIZE
#if LWIP_IPV6_SCOPES
const ip6_addr_t ip6_addr_none = {{0xffffffffUL, 0xffffffffUL, 0xffffffffUL, 0xffffffffUL}, 0};
const ip6_addr_t ip6_addr_loopback = {{0, 0, 0, PP_HTONL(0x00000001UL)}, 0};
const ip6_addr_t ip6_addr_allnodes_iflocal = {{PP_HTONL(0xff010000UL), 0, 0, PP_HTONL(0x00000001UL)}, 0};
const ip6_addr_t ip6_addr_allnodes_linklocal = {{PP_HTONL(0xff020000UL), 0, 0, PP_HTONL(0x00000001UL)}, 0};
const ip6_addr_t ip6_addr_allrpl_nodes_linklocal = {{PP_HTONL(0xff020000UL), 0, 0, PP_HTONL(0x0000001aUL)}, 0};
const ip6_addr_t ip6_addr_mpl_ip4_bcast = {{PP_HTONL(0xff130000UL), 0, 0, PP_HTONL(0x000000fcUL)}, 0};
const ip6_addr_t ip6_addr_allrouters_linklocal = {{PP_HTONL(0xff020000UL), 0, 0, PP_HTONL(0x00000002UL)}, 0};
#else
const ip6_addr_t ip6_addr_none = {{0xffffffffUL, 0xffffffffUL, 0xffffffffUL, 0xffffffffUL}};
const ip6_addr_t ip6_addr_loopback = {{0, 0, 0, PP_HTONL(0x00000001UL)}};
const ip6_addr_t ip6_addr_allnodes_iflocal = {{PP_HTONL(0xff010000UL), 0, 0, PP_HTONL(0x00000001UL)}};
const ip6_addr_t ip6_addr_allnodes_linklocal = {{PP_HTONL(0xff020000UL), 0, 0, PP_HTONL(0x00000001UL)}};
const ip6_addr_t ip6_addr_allrpl_nodes_linklocal = {{PP_HTONL(0xff020000UL), 0, 0, PP_HTONL(0x0000001aUL)}};
const ip6_addr_t ip6_addr_mpl_ip4_bcast = {{PP_HTONL(0xff130000UL), 0, 0, PP_HTONL(0x000000fcUL)}};
const ip6_addr_t ip6_addr_allrouters_linklocal = {{PP_HTONL(0xff020000UL), 0, 0, PP_HTONL(0x00000002UL)}};
#endif
void
ip6_addr_set_fun(ip6_addr_t *dest, ip6_addr_t *src)
{
  int i;
  if (dest == NULL) {
    return;
  }
  for (i = 0; i < IP6_ADDR_U32_ARR_SIZE; i++) {
    dest->addr[i] = (src == NULL) ? 0 : src->addr[i];
  }
  ip6_addr_set_zone((dest), (src) == NULL ? IP6_NO_ZONE : ip6_addr_zone(src));
}
#endif

/**
 * Check whether "cp" is a valid ascii representation
 * of an IPv6 address and convert to a binary address.
 * Returns 1 if the address is valid, 0 if not.
 *
 * @param cp IPv6 address in ascii representation (e.g. "FF01::1")
 * @param addr pointer to which to save the ip address in network order
 * @return 1 if cp could be converted to addr, 0 on failure
 */
int
ip6addr_aton(const char *cp, ip6_addr_t *addr)
{
  u32_t addr_index, zero_blocks, current_block_index, current_block_value;
  const char *s;
#if LWIP_IPV4
  int check_ipv4_mapped = 0;
#endif /* LWIP_IPV4 */

  LWIP_ERROR("inet_ntop:src is NULL", (cp != NULL), return 0);
  u8_t max_colon = 0;
  for (s = cp; *s != 0; s++) {
    if (*s == ':') {
      max_colon = max_colon + 1;
    }
  }
  /* Count the number of colons, to count the number of blocks in a "::" sequence
     zero_blocks may be 1 even if there are no :: sequences */
  zero_blocks = 8;
  u8_t count = 0;
  u8_t current_colon = 1;
  u8_t last_word = 0;
  u8_t last_word_flag = 0;
  for (s = cp; *s != 0; s++) {
    count = count + 1;
    if (last_word_flag > 0) {
      last_word = last_word + 1;
      if (last_word > IP6_ADDR_BLOCK_SIZE_MAX) {
        return 0;
      }
    }
    if (*s == ':') {
      if (current_colon == max_colon) {
        last_word_flag = 1;
      }
      current_colon = current_colon + 1;
      if (count > (IP6_ADDR_BLOCK_SIZE_MAX + 1)) {
        return 0;
      }
      count = 0;
      zero_blocks--;
#if LWIP_IPV4
    } else if (*s == '.') {
      if ((zero_blocks == 6) || (zero_blocks == 5) || (zero_blocks == 2)) {
        check_ipv4_mapped = 1;
        /* last block could be the start of an IPv4 address */
        zero_blocks--;
      } else {
        /* invalid format */
        return 0;
      }
      break;
#endif /* LWIP_IPV4 */
    } else if (!lwip_isxdigit(*s)) {
      return 0;
    }
  }

  /* parse each block */
  addr_index = 0;
  current_block_index = 0;
  current_block_value = 0;
  for (s = cp; *s != 0; s++) {
    if (*s == ':') {
      if (*(s + 1) == 0) {
        return 0;
      }
      if (addr) {
        if (current_block_index & 0x1) {
          addr->addr[addr_index++] |= current_block_value;
        }
        else {
          addr->addr[addr_index] = current_block_value << 16;
        }
      }
      current_block_index++;
#if LWIP_IPV4
      if ((check_ipv4_mapped != 0) && (current_block_index == 6)) {
        ip4_addr_t ip4;
        int ret = ip4addr_aton(s + 1, &ip4);
        if (ret) {
          if (addr) {
            addr->addr[3] = lwip_htonl(ip4.addr);
            current_block_index++;
            goto fix_byte_order_and_return;
          }
          return 1;
        }
      }
#endif /* LWIP_IPV4 */
      current_block_value = 0;
      if (current_block_index > 7) {
        /* address too long! */
        return 0;
      }
      if (s[1] == ':') {
        if (s[2] == ':') {
          /* invalid format: three successive colons */
          return 0;
        }
        s++;
        /* "::" found, set zeros */
        while (zero_blocks > 0) {
          zero_blocks--;
          if (current_block_index & 0x1) {
            addr_index++;
          } else {
            if (addr) {
              addr->addr[addr_index] = 0;
            }
          }
          current_block_index++;
          if (current_block_index > 7) {
            /* address too long! */
            return 0;
          }
        }
#if LWIP_IPV4
        if (check_ipv4_mapped) {
          /* IPv4-Compatible IPv6 address */
          if (current_block_index == 6) {
            ip4_addr_t ip4;
            int ret = ip4addr_aton(s + 1, &ip4);
            if (ret) {
              if (addr != NULL) {
                addr->addr[3] = lwip_htonl(ip4.addr);
                current_block_index++;
                goto fix_byte_order_and_return;
              }
              return 1;
            }
          }
        }
#endif /* LWIP_IPV4 */
      }
    } else if (lwip_isxdigit(*s)) {
      /* add current digit */
      current_block_value = (current_block_value << 4) +
          (lwip_isdigit(*s) ? (u32_t)(*s - '0') :
          (u32_t)(10 + (lwip_islower(*s) ? *s - 'a' : *s - 'A')));
    } else {
      /* unexpected digit, space? CRLF? */
      break;
    }
  }

  if (addr) {
    if (current_block_index & 0x1) {
      addr->addr[addr_index++] |= current_block_value;
    }
    else {
      addr->addr[addr_index] = current_block_value << 16;
    }
#if LWIP_IPV4
fix_byte_order_and_return:
#endif
    /* convert to network byte order. */
    for (addr_index = 0; addr_index < 4; addr_index++) {
      addr->addr[addr_index] = lwip_htonl(addr->addr[addr_index]);
    }

    ip6_addr_clear_zone(addr);
  }

  if (current_block_index != 7) {
    return 0;
  }

  return 1;
}

/**
 * Convert numeric IPv6 address into ASCII representation.
 * returns ptr to static buffer; not reentrant!
 *
 * @param addr ip6 address in network order to convert
 * @return pointer to a global static (!) buffer that holds the ASCII
 *         representation of addr
 */
char *
ip6addr_ntoa(const ip6_addr_t *addr)
{
  static char str[40];
  LWIP_ERROR("inet_ntop:dst is NULL", (addr != NULL), return NULL);
  return ip6addr_ntoa_r(addr, str, 40);
}

/**
 * Same as ipaddr_ntoa, but reentrant since a user-supplied buffer is used.
 *
 * @param addr ip6 address in network order to convert
 * @param buf target buffer where the string is stored
 * @param buflen length of buf
 * @return either pointer to buf which now holds the ASCII
 *         representation of addr or NULL if buf was too small
 */
char *
ip6addr_ntoa_r(const ip6_addr_t *addr, char *buf, int buflen)
{
  u32_t current_block_index, current_block_value, next_block_value;
  s32_t i;
  u8_t zero_flag, empty_block_flag;
#if LWIP_IPV4
    ip4_addr_t addr4;
    char *ret = NULL;
    char *buf_ip4 = NULL;
    int buflen_ip4;
#endif

  if ((addr == NULL) || (buf == NULL)) {
    set_errno(ENOSPC);
    return NULL;
  }

#if LWIP_IPV4
/**
 * @page RFC-6052 RFC-6052
 * @par  RFC-6052 Compliance Information
 * @par Compliant Section
 * Section 2.  IPv4-Embedded IPv6 Address Prefix and Format
 * @par Behavior Description
 * IPv4-converted IPv6 addresses and IPv4-translatable IPv6 addresses
 *  follow the same format, described here as the IPv4-embedded IPv6
 *  address Format. \n
 * @verbatim
 *    +-------------------+--------------+----------------------------+
 *    | Well-Known Prefix | IPv4 address | IPv4-Embedded IPv6 address |
 *    +-------------------+--------------+----------------------------+
 *    | 64:ff9b::/96      |  192.0.2.33  | 64:ff9b::192.0.2.33        |
 *   +-------------------+--------------+----------------------------+
 * @endverbatim
 */
/**
 * @page RFC-4291 RFC-4291
 * @par  RFC-4291 Compliance Information
 * @par Compliant Sections
 * Section 2.5.5. IPv6 Addresses with Embedded IPv4 Addresses
 * @par Behavior Description
 * Two types of IPv6 addresses are defined that carry an IPv4 address in
 *  the low-order 32 bits of the address.  These are the "IPv4-Compatible
 *  IPv6 address" and the "IPv4-mapped IPv6 address".
 */
  if (ip6_addr_isipv4mappedipv6(addr) || ip6_addr_isipv4compatible(addr)) {
    char *header = NULL;
    size_t header_len;

    if (ip6_addr_isipv4mappedipv6(addr)) {
      header = IP4MAPPED_HEADER;
      header_len = sizeof(IP4MAPPED_HEADER);
    } else {
      header = IPV4COMPATIBLE_HEADER;
      header_len = sizeof(IPV4COMPATIBLE_HEADER);
    }

    /* check buffer len before incrementing the buffer */
    if (buflen < (int)header_len) {
      return NULL;
    }

    /* Dual-stack: This is an IPv4 mapped address */
    buf_ip4  = buf + header_len - 1;
    buflen_ip4 = (int)((unsigned int)buflen - header_len + 1);

    if (memcpy_s(buf, (size_t)buflen, header, header_len) != EOK) {
      return NULL;
    }

    addr4.addr = addr->addr[3];
    ret = ip4addr_ntoa_r(&addr4, buf_ip4, buflen_ip4);
    if (ret != buf_ip4) {
      return NULL;
    }
    return buf;
  }
#endif /* LWIP_IPV4 */
  i = 0;
  empty_block_flag = 0; /* used to indicate a zero chain for "::' */

  for (current_block_index = 0; current_block_index < 8; current_block_index++) {
    /* get the current 16-bit block */
    current_block_value = lwip_htonl(addr->addr[current_block_index >> 1]);
    if ((current_block_index & 0x1) == 0) {
      current_block_value = current_block_value >> 16;
    }
    current_block_value &= 0xffff;

    /* Check for empty block. */
    if (current_block_value == 0) {
      if ((current_block_index == 7) && (empty_block_flag != 2)) {
        /* special case, we must render a ':' for the last block. */
        buf[i++] = ':';
        if (i >= buflen) {
          return NULL;
        }
        if (empty_block_flag == 0) {
          /* Check whether the starting of the zero block is the last block(16 bits) in the representation */
          /* In that case, we have to render ":0" to the output string and exit */
          buf[i++] = '0';
          if (i >= buflen) {
              return NULL;
          }
        }
        break;
      }
      if (empty_block_flag == 0) {
        /* generate empty block "::", but only if more than one contiguous zero block,
         * according to current formatting suggestions RFC 5952. */
        next_block_value = lwip_htonl(addr->addr[(current_block_index + 1) >> 1]);
        if ((current_block_index & 0x1) == 0x01) {
            next_block_value = next_block_value >> 16;
        }
        next_block_value &= 0xffff;
        if (next_block_value == 0) {
          empty_block_flag = 1;
          buf[i++] = ':';
          if (i >= buflen) {
            return NULL;
          }
          /* If the current_block_index is the last block and last block empty then, add '0' */
          if (current_block_index == 7) {
            buf[i++] = (char)(lwip_xchar((char)(current_block_value & 0xf)));
          }
          if (i >= buflen) {
            return NULL;
          }
          continue; /* move on to next block. */
        }
      } else if (empty_block_flag == 1) {
        /* move on to next block. */
        continue;
      }
    } else if (empty_block_flag == 1) {
      /* Set this flag value so we don't produce multiple empty blocks. */
      empty_block_flag = 2;
    }

    if (current_block_index > 0) {
      buf[i++] = ':';
      if (i >= buflen) {
        return NULL;
      }
    }

    if ((current_block_value & 0xf000) == 0) {
      zero_flag = 1;
    } else {
      buf[i++] = lwip_xchar(((current_block_value & 0xf000) >> 12));
      zero_flag = 0;
      if (i >= buflen) {
        return NULL;
      }
    }

    if (((current_block_value & 0xf00) == 0) && (zero_flag)) {
      /* do nothing */
    } else {
      buf[i++] = lwip_xchar(((current_block_value & 0xf00) >> 8));
      zero_flag = 0;
      if (i >= buflen) {
        return NULL;
      }
    }

    if (((current_block_value & 0xf0) == 0) && (zero_flag)) {
      /* do nothing */
    }
    else {
      buf[i++] = lwip_xchar(((current_block_value & 0xf0) >> 4));
      zero_flag = 0;
      if (i >= buflen) {
        return NULL;
      }
    }

    buf[i++] = lwip_xchar((current_block_value & 0xf));
    if (i >= buflen) {
      return NULL;
    }
  }

  buf[i] = 0;

  return buf;
}

#endif /* LWIP_IPV6 */
