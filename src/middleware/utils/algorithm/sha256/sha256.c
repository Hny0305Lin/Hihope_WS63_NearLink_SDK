/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: sha256 functions
 */

#include "securec.h"
#include "sha256/sha256.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define rotl(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define rotr(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

#define ch(x, y, z)  ((z) ^ ((x) & ((y) ^ (z))))
#define maj(x, y, z) (((x) & ((y) | (z))) | ((y) & (z)))
#define sigma_0(x)    (rotr((x), 2) ^ rotr((x), 13) ^ rotr((x), 22))
#define sigma_1(x)    (rotr((x), 6) ^ rotr((x), 11) ^ rotr((x), 25))
#define sigma0(x)    (rotr((x), 7) ^ rotr((x), 18) ^ ((x) >> 3))
#define sigma1(x)    (rotr((x), 17) ^ rotr((x), 19) ^ ((x) >> 10))

#define K_ARRAY_LEN        64
#define PADDING_ARRAY_LEN  64
#define H_SWAP_L_SHIFT     32
#define SHA256GUTS_BUF_LEN 64
#define BURNSTACK_BUF_LEN  128
#define SHIFT_15_8_WORD    8
#define SHIFT_23_16_WORD   16
#define SHIFT_31_24_WORD   24
#define SHA256_UNROLL_1    1
#define SHA256_UNROLL_2    2
#define SHA256_UNROLL_4    4
#define SHA256_UNROLL_8    8
#define SHA256_UNROLL_16   16
#define SHA256_UNROLL_32   32
#define SHA256_UNROLL_64   64

#define SC_HASH_INDEX2     2
#define SC_HASH_INDEX3     3
#define SC_HASH_INDEX4     4
#define SC_HASH_INDEX5     5
#define SC_HASH_INDEX6     6
#define SC_HASH_INDEX7     7

#define BUF_INDEX1         9
#define BUF_INDEX2         14

#define BURN_PARAMETER_INDEX1    74
#define BURN_PARAMETER_INDEX2    6


#define do_round() do {                                      \
        t1 = h + sigma_1(e) + ch(e, f, g) + *(kp++) + *(w++); \
        t2 = sigma_0(a) + maj(a, b, c);                      \
        h = g, g = f, f = e;                                 \
        e = d + t1;                                          \
        d = c;                                               \
        c = b;                                               \
        b = a;                                               \
        a = t1 + t2;                                         \
    } while (0)

static const uint32_t g_k[K_ARRAY_LEN] = {
    0x428a2f98L, 0x71374491L, 0xb5c0fbcfL, 0xe9b5dba5L,
    0x3956c25bL, 0x59f111f1L, 0x923f82a4L, 0xab1c5ed5L,
    0xd807aa98L, 0x12835b01L, 0x243185beL, 0x550c7dc3L,
    0x72be5d74L, 0x80deb1feL, 0x9bdc06a7L, 0xc19bf174L,
    0xe49b69c1L, 0xefbe4786L, 0x0fc19dc6L, 0x240ca1ccL,
    0x2de92c6fL, 0x4a7484aaL, 0x5cb0a9dcL, 0x76f988daL,
    0x983e5152L, 0xa831c66dL, 0xb00327c8L, 0xbf597fc7L,
    0xc6e00bf3L, 0xd5a79147L, 0x06ca6351L, 0x14292967L,
    0x27b70a85L, 0x2e1b2138L, 0x4d2c6dfcL, 0x53380d13L,
    0x650a7354L, 0x766a0abbL, 0x81c2c92eL, 0x92722c85L,
    0xa2bfe8a1L, 0xa81a664bL, 0xc24b8b70L, 0xc76c51a3L,
    0xd192e819L, 0xd6990624L, 0xf40e3585L, 0x106aa070L,
    0x19a4c116L, 0x1e376c08L, 0x2748774cL, 0x34b0bcb5L,
    0x391c0cb3L, 0x4ed8aa4aL, 0x5b9cca4fL, 0x682e6ff3L,
    0x748f82eeL, 0x78a5636fL, 0x84c87814L, 0x8cc70208L,
    0x90befffaL, 0xa4506cebL, 0xbef9a3f7L, 0xc67178f2L
};

#ifndef RUNTIME_ENDIAN

#ifdef WORDS_BIGENDIAN

#define byte_swap(x)   (x)
#define byte_swap_64(x) (x)

#else /* WORDS_BIGENDIAN */

#define byte_swap(x)   ((rotr((x), 8) & 0xff00ff00L) | (rotl((x), 8) & 0x00ff00ffL))
#define byte_swap_64(x) _byteswap64(x)

static uint64_t _byteswap64(uint64_t x)
{
    uint32_t a = (uint32_t)(x >> H_SWAP_L_SHIFT);
    uint32_t b = (uint32_t)x;

    return ((uint64_t)byte_swap(b) << H_SWAP_L_SHIFT) | (uint64_t)byte_swap(a);
}
#endif /* WORDS_BIGENDIAN */

#else /* !RUNTIME_ENDIAN */

#define byte_swap(x)   _byteswap(sc->little_endian, x)
#define byte_swap_64(x) _byteswap64(sc->little_endian, x)

#define _byte_swap(x) ((rotr((x), 8) & 0xff00ff00L) |  \
                      (rotl((x), 8) & 0x00ff00ffL))
#define _byte_swap64(x) __byteswap64(x)

static inline uint64_t __byteswap64(uint64_t x)
{
    uint32_t a = x >> H_SWAP_L_SHIFT;
    uint32_t b = (uint32_t)x;

    return ((uint64_t)_byte_swap(b) << H_SWAP_L_SHIFT) | (uint64_t)_byte_swap(a);
}

static inline uint32_t _byteswap(int little_endian, uint32_t x)
{
    if (!little_endian) {
        return x;
    } else {
        return _byte_swap(x);
    }
}

static inline uint64_t _byte_swap_64(int little_endian, uint64_t x)
{
    if (!little_endian) {
        return x;
    } else {
        return _byte_swap_64(x);
    }
}

static inline void set_endian(int *little_endianp)
{
    const uint8_t endian_bites_len = 4;
    union {
        uint32_t w;
        uint8_t b[endian_bites_len];
    } endian;

    endian.w = 1L;
    *little_endianp = endian.b[0] != 0;
}
#endif /* !RUNTIME_ENDIAN */

static const uint8_t g_padding[PADDING_ARRAY_LEN] = {
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void sha256_init(sha256_context_t *sc)
{
#ifdef RUNTIME_ENDIAN
    set_endian(&sc->little_endian);
#endif /* RUNTIME_ENDIAN */

    sc->total_length = 0LL;
    sc->hash[0] = 0x6a09e667L;
    sc->hash[1] = 0xbb67ae85L;
    sc->hash[SC_HASH_INDEX2] = 0x3c6ef372L;
    sc->hash[SC_HASH_INDEX3] = 0xa54ff53aL;
    sc->hash[SC_HASH_INDEX4] = 0x510e527fL;
    sc->hash[SC_HASH_INDEX5] = 0x9b05688cL;
    sc->hash[SC_HASH_INDEX6] = 0x1f83d9abL;
    sc->hash[SC_HASH_INDEX7] = 0x5be0cd19L;
    sc->buffer_length = 0L;
}

static void burn_stack(int size)
{
    char buf[BURNSTACK_BUF_LEN];

    (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));
    size -= (int)sizeof(buf);
    if (size > 0) {
        burn_stack(size);
    }
}

static void SHA256Guts(sha256_context_t *sc, const uint32_t *cbuf)
{
    uint32_t buf[SHA256GUTS_BUF_LEN] = { 0 };
    uint32_t *w = NULL;
    uint32_t *w2 = NULL;
    uint32_t *w7 = NULL;
    uint32_t *w15 = NULL;
    uint32_t *w16 = NULL;
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t t1, t2;
    const uint32_t *kp = NULL;
    int i;

    w = buf;

    for (i = 0xF; i >= 0; i--) {
        *(w++) = byte_swap(*cbuf);
        cbuf++;
    }

    w16 = &buf[0];
    w15 = &buf[1];
    w7 = &buf[BUF_INDEX1];
    w2 = &buf[BUF_INDEX2];

    for (i = 0x2F; i >= 0; i--) {
        *(w++) = sigma1(*w2) + *(w7++) + sigma0(*w15) + *(w16++);
        w2++;
        w15++;
    }

    a = sc->hash[0];
    b = sc->hash[1];
    c = sc->hash[SC_HASH_INDEX2];
    d = sc->hash[SC_HASH_INDEX3];
    e = sc->hash[SC_HASH_INDEX4];
    f = sc->hash[SC_HASH_INDEX5];
    g = sc->hash[SC_HASH_INDEX6];
    h = sc->hash[SC_HASH_INDEX7];

    kp = g_k;
    w = buf;

#ifndef SHA256_UNROLL
#define SHA256_UNROLL 1
#endif /* !SHA256_UNROLL */

#if SHA256_UNROLL == SHA256_UNROLL_1
    for (i = 0x3F; i >= 0; i--) {
        do_round();
    }

#elif SHA256_UNROLL == SHA256_UNROLL_2
    for (i = 0x1F; i >= 0; i--) {
        do_round();
        do_round();
    }

#elif SHA256_UNROLL == SHA256_UNROLL_4
    for (i = 0xF; i >= 0; i--) {
        do_round();
        do_round();
        do_round();
        do_round();
    }

#elif SHA256_UNROLL == SHA256_UNROLL_8
    for (i = 0x7; i >= 0; i--) {
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
    }

#elif SHA256_UNROLL == SHA256_UNROLL_16
    for (i = 0x3; i >= 0; i--) {
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
        do_round();
    }
#else
#error "SHA256_UNROLL must be 1, 2, 4, 8 or 16!"
#endif

    sc->hash[0] += a;
    sc->hash[1] += b;
    sc->hash[SC_HASH_INDEX2] += c;
    sc->hash[SC_HASH_INDEX3] += d;
    sc->hash[SC_HASH_INDEX4] += e;
    sc->hash[SC_HASH_INDEX5] += f;
    sc->hash[SC_HASH_INDEX6] += g;
    sc->hash[SC_HASH_INDEX7] += h;
}

void SHA256Update(sha256_context_t *sc, const void *vdata, uint32_t len)
{
    const uint8_t *data = vdata;
    uint32_t buffer_bytes_left, bytes_to_copy;
    int need_burn = 0;

#ifdef SHA256_FAST_COPY
    if (sc->buffer_length) {
        buffer_bytes_left = 64L - sc->buffer_length;

        bytes_to_copy = buffer_bytes_left;
        if (bytes_to_copy > len) {
            bytes_to_copy = len;
        }

        if (memcpy_s(&sc->buffer.bytes[sc->buffer_length], (64L - sc->buffer_length), data, bytes_to_copy) != EOK) {
            return;
        }
        sc->total_length += bytes_to_copy * 8L;

        sc->buffer_length += bytes_to_copy;
        data += bytes_to_copy;
        len -= bytes_to_copy;

        if (sc->buffer_length == 64L) {
            SHA256Guts(sc, sc->buffer.words);
            need_burn = 1;
            sc->buffer_length = 0L;
        }
    }

    while (len > 63L) {
        sc->total_length += 512L;

        SHA256Guts(sc, data);
        need_burn = 1;

        data += 64L;
        len -= 64L;
    }

    if (len) {
        if (memcpy_s(&sc->buffer.bytes[sc->buffer_length], (64L - sc->buffer_length), data, len) != EOK) {
            return;
        }

        sc->total_length += len * 8L;

        sc->buffer_length += len;
    }

#else  /* SHA256_FAST_COPY */
    while (len != 0) {
        buffer_bytes_left = 64L - sc->buffer_length;

        bytes_to_copy = buffer_bytes_left;
        if (bytes_to_copy > len) {
            bytes_to_copy = len;
        }

        if (memcpy_s(&sc->buffer.bytes[sc->buffer_length], (64L - sc->buffer_length), data, bytes_to_copy) != EOK) {
            return;
        }
        sc->total_length += (uint64_t)(unsigned)(bytes_to_copy * 8L);

        sc->buffer_length += bytes_to_copy;
        data += bytes_to_copy;
        len -= bytes_to_copy;

        if (sc->buffer_length == 64L) {
            SHA256Guts(sc, sc->buffer.words);
            need_burn = 1;
            sc->buffer_length = 0L;
        }
    }
#endif /* SHA256_FAST_COPY */

    if (need_burn != 0) {
        burn_stack(sizeof(uint32_t[BURN_PARAMETER_INDEX1]) +
                   sizeof(uint32_t *[BURN_PARAMETER_INDEX2]) + sizeof(int));
    }
}

void sha256_final(sha256_context_t *sc, uint8_t hash[SHA256_HASH_SIZE], uint32_t hash_len)
{
    uint32_t bytes_to_pad;
    uint64_t length_pad;
    int i;
    if (hash_len == 0) {
        return;
    }
    bytes_to_pad = 120L - sc->buffer_length;
    if (bytes_to_pad > 64L) {
        bytes_to_pad -= 64L;
    }

    length_pad = byte_swap_64(sc->total_length);

    SHA256Update(sc, g_padding, bytes_to_pad);
    SHA256Update(sc, &length_pad, 8L);

    if (hash) {
        for (i = 0; i < SHA256_HASH_WORDS; i++) {
#ifdef SHA256_FAST_COPY
            *((uint32_t *)hash) = byte_swap(sc->hash[i]);
#else  /* SHA256_FAST_COPY */
            hash[0] = (uint8_t)(sc->hash[i] >> SHIFT_31_24_WORD);
            hash[1] = (uint8_t)(sc->hash[i] >> SHIFT_23_16_WORD);
            hash[SC_HASH_INDEX2] = (uint8_t)(sc->hash[i] >> SHIFT_15_8_WORD);
            hash[SC_HASH_INDEX3] = (uint8_t)sc->hash[i];
#endif /* SHA256_FAST_COPY */
            hash += 4;   // Pointer offset 4 bites per cycle
        }
    }
}

void sha256_hash(const uint8_t *in_buff, uint32_t in_buff_len, uint8_t *out_buff, uint32_t out_buff_len)
{
    sha256_context_t foo;

    sha256_init(&foo);
    SHA256Update(&foo, in_buff, in_buff_len);
    sha256_final(&foo, out_buff, out_buff_len);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
