#include <string.h>
#include <stdint.h>
#include <endian.h>

#ifdef __GNUC__
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define LS >>
#define RS <<
#else
#define LS <<
#define RS >>
#endif

#define MEMCPY_BYTE_BITS 8
#define MEMCPY_ALIGH_UNIT_BYTES_4       4
#define MEMCPY_FAST_COPY_UNIT_NUM_4     4
#define MEMCPY_FAST_COPY_UNIT_BYTES (MEMCPY_ALIGH_UNIT_BYTES_4 * MEMCPY_FAST_COPY_UNIT_NUM_4)

#define MEMCPY_ALIGH_UNIT_BITS (MEMCPY_BYTE_BITS * MEMCPY_ALIGH_UNIT_BYTES_4)

#define MEMCPY_FAST_COPY_OFFSET_UNIT_0 (MEMCPY_ALIGH_UNIT_BYTES_4 * 0)
#define MEMCPY_FAST_COPY_OFFSET_UNIT_1 (MEMCPY_ALIGH_UNIT_BYTES_4 * 1)
#define MEMCPY_FAST_COPY_OFFSET_UNIT_2 (MEMCPY_ALIGH_UNIT_BYTES_4 * 2)
#define MEMCPY_FAST_COPY_OFFSET_UNIT_3 (MEMCPY_ALIGH_UNIT_BYTES_4 * 3)

#define MEMCPY_NOT_ALIGN_FAST_COPY_THRESHOLD (MEMCPY_FAST_COPY_UNIT_BYTES * 2)

#define MEMCPY_OFFSET_BYTES_1 1
#define MEMCPY_ALIGH_OFFSET_BYTES_1 (MEMCPY_ALIGH_UNIT_BYTES_4 - MEMCPY_OFFSET_BYTES_1)
#define MEMCPY_OFFSET_BITS_1 (MEMCPY_BYTE_BITS * MEMCPY_OFFSET_BYTES_1)
#define MEMCPY_OFFSET_ALIGN_BITS_1 (MEMCPY_BYTE_BITS * MEMCPY_ALIGH_OFFSET_BYTES_1)

#define MEMCPY_OFFSET_BYTES_2 2
#define MEMCPY_ALIGH_OFFSET_BYTES_2 (MEMCPY_ALIGH_UNIT_BYTES_4 - MEMCPY_OFFSET_BYTES_2)
#define MEMCPY_OFFSET_BITS_2 (MEMCPY_BYTE_BITS * MEMCPY_OFFSET_BYTES_2)
#define MEMCPY_OFFSET_ALIGN_BITS_2 (MEMCPY_BYTE_BITS * MEMCPY_ALIGH_OFFSET_BYTES_2)

#define MEMCPY_OFFSET_BYTES_3 3
#define MEMCPY_ALIGH_OFFSET_BYTES_3 (MEMCPY_ALIGH_UNIT_BYTES_4 - MEMCPY_OFFSET_BYTES_3)
#define MEMCPY_OFFSET_BITS_3 (MEMCPY_BYTE_BITS * MEMCPY_OFFSET_BYTES_3)
#define MEMCPY_OFFSET_ALIGN_BITS_3 (MEMCPY_BYTE_BITS * MEMCPY_ALIGH_OFFSET_BYTES_3)

#define MEMCPY_BYTE_CHECK_NUM_1     0x01
#define MEMCPY_BYTE_CHECK_NUM_2     0x02
#define MEMCPY_BYTE_CHECK_NUM_4     0x04
#define MEMCPY_BYTE_CHECK_NUM_8     0x08
#define MEMCPY_BYTE_CHECK_NUM_16    0x10
#endif

void *memcpy(void *restrict dest, const void *restrict src, size_t num)
{
    unsigned char *d = dest;
    const unsigned char *s = src;
    size_t n = num;

#ifdef __GNUC__
    typedef uint32_t __attribute__((__may_alias__)) u32;
    uint32_t w, x;

    for (; (uintptr_t)s % MEMCPY_ALIGH_UNIT_BYTES_4 && n; n--) {
        *d++ = *s++;
    }

    if ((uintptr_t)d % MEMCPY_ALIGH_UNIT_BYTES_4 == 0) {
        for (; n >= MEMCPY_FAST_COPY_UNIT_BYTES; s += MEMCPY_FAST_COPY_UNIT_BYTES,
            d += MEMCPY_FAST_COPY_UNIT_BYTES, n -= MEMCPY_FAST_COPY_UNIT_BYTES) {
            *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_0) = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_0);
            *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_1) = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_1);
            *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_2) = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_2);
            *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_3) = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_3);
        }
        if (n & MEMCPY_FAST_COPY_OFFSET_UNIT_2) {
            *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_0) = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_0);
            *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_1) = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_1);
            d += MEMCPY_FAST_COPY_OFFSET_UNIT_2;
            s += MEMCPY_FAST_COPY_OFFSET_UNIT_2;
        }
        if (n & MEMCPY_FAST_COPY_OFFSET_UNIT_1) {
            *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_0) = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_0);
            d += MEMCPY_FAST_COPY_OFFSET_UNIT_1;
            s += MEMCPY_FAST_COPY_OFFSET_UNIT_1;
        }
        if (n & MEMCPY_BYTE_CHECK_NUM_2) {
            *d++ = *s++; *d++ = *s++;
        }
        if (n & MEMCPY_BYTE_CHECK_NUM_1) {
            *d = *s;
        }
        return dest;
    }

    if (n >= MEMCPY_NOT_ALIGN_FAST_COPY_THRESHOLD) {
        switch ((uintptr_t)d % MEMCPY_ALIGH_UNIT_BYTES_4) {
            case MEMCPY_OFFSET_BYTES_1:
                w = *(u32 *)s;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                n -= MEMCPY_ALIGH_OFFSET_BYTES_1;
                for (; n >= MEMCPY_FAST_COPY_UNIT_BYTES + MEMCPY_OFFSET_BYTES_1; s += MEMCPY_FAST_COPY_UNIT_BYTES,
                    d += MEMCPY_FAST_COPY_UNIT_BYTES, n -= MEMCPY_FAST_COPY_UNIT_BYTES) {
                    x = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_0 + MEMCPY_OFFSET_BYTES_1);
                    *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_0) =
                        (w LS MEMCPY_OFFSET_ALIGN_BITS_1) | (x RS MEMCPY_OFFSET_BITS_1);
                    w = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_1 + MEMCPY_OFFSET_BYTES_1);
                    *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_1) =
                        (x LS MEMCPY_OFFSET_ALIGN_BITS_1) | (w RS MEMCPY_OFFSET_BITS_1);
                    x = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_2 + MEMCPY_OFFSET_BYTES_1);
                    *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_2) =
                        (w LS MEMCPY_OFFSET_ALIGN_BITS_1) | (x RS MEMCPY_OFFSET_BITS_1);
                    w = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_3 + MEMCPY_OFFSET_BYTES_1);
                    *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_3) =
                        (x LS MEMCPY_OFFSET_ALIGN_BITS_1) | (w RS MEMCPY_OFFSET_BITS_1);
                }
                break;
            case MEMCPY_OFFSET_BYTES_2:
                w = *(u32 *)s;
                *d++ = *s++;
                *d++ = *s++;
                n -= MEMCPY_ALIGH_OFFSET_BYTES_2;
                for (; n >= MEMCPY_FAST_COPY_UNIT_BYTES + MEMCPY_OFFSET_BYTES_2; s += MEMCPY_FAST_COPY_UNIT_BYTES,
                    d += MEMCPY_FAST_COPY_UNIT_BYTES, n -= MEMCPY_FAST_COPY_UNIT_BYTES) {
                    x = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_0 + MEMCPY_OFFSET_BYTES_2);
                    *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_0) =
                        (w LS MEMCPY_OFFSET_ALIGN_BITS_2) | (x RS MEMCPY_OFFSET_BITS_2);
                    w = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_1 + MEMCPY_OFFSET_BYTES_2);
                    *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_1) =
                        (x LS MEMCPY_OFFSET_ALIGN_BITS_2) | (w RS MEMCPY_OFFSET_BITS_2);
                    x = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_2 + MEMCPY_OFFSET_BYTES_2);
                    *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_2) =
                        (w LS MEMCPY_OFFSET_ALIGN_BITS_2) | (x RS MEMCPY_OFFSET_BITS_2);
                    w = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_3 + MEMCPY_OFFSET_BYTES_2);
                    *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_3) =
                        (x LS MEMCPY_OFFSET_ALIGN_BITS_2) | (w RS MEMCPY_OFFSET_BITS_2);
                }
                break;
            case MEMCPY_OFFSET_BYTES_3:
                w = *(u32 *)s;
                *d++ = *s++;
                n -= MEMCPY_ALIGH_OFFSET_BYTES_3;
                for (; n >= MEMCPY_FAST_COPY_UNIT_BYTES + MEMCPY_OFFSET_BYTES_3; s += MEMCPY_FAST_COPY_UNIT_BYTES,
                    d += MEMCPY_FAST_COPY_UNIT_BYTES, n -= MEMCPY_FAST_COPY_UNIT_BYTES) {
                    x = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_0 + MEMCPY_OFFSET_BYTES_3);
                    *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_0) =
                        (w LS MEMCPY_OFFSET_ALIGN_BITS_3) | (x RS MEMCPY_OFFSET_BITS_3);
                    w = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_1 + MEMCPY_OFFSET_BYTES_3);
                    *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_1) =
                        (x LS MEMCPY_OFFSET_ALIGN_BITS_3) | (w RS MEMCPY_OFFSET_BITS_3);
                    x = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_2 + MEMCPY_OFFSET_BYTES_3);
                    *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_2) =
                        (w LS MEMCPY_OFFSET_ALIGN_BITS_3) | (x RS MEMCPY_OFFSET_BITS_3);
                    w = *(u32 *)(s + MEMCPY_FAST_COPY_OFFSET_UNIT_3 + MEMCPY_OFFSET_BYTES_3);
                    *(u32 *)(d + MEMCPY_FAST_COPY_OFFSET_UNIT_3) =
                        (x LS MEMCPY_OFFSET_ALIGN_BITS_3) | (w RS MEMCPY_OFFSET_BITS_3);
                }
                break;
        }
    }
    if (n & MEMCPY_BYTE_CHECK_NUM_16) {
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
    }
    if (n & MEMCPY_BYTE_CHECK_NUM_8) {
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
    }
    if (n & MEMCPY_BYTE_CHECK_NUM_4) {
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
    }
    if (n & MEMCPY_BYTE_CHECK_NUM_2) {
        *d++ = *s++; *d++ = *s++;
    }
    if (n & MEMCPY_BYTE_CHECK_NUM_1) {
        *d = *s;
    }
    return dest;
#endif

    for (; n; n--) *d++ = *s++;
    return dest;
}
