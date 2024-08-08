#include <string.h>
#include <stdint.h>

#ifdef __GNUC__
typedef __attribute__((__may_alias__)) size_t WT;
#define WS (sizeof(WT))
#endif

void *memmove(void *dest, const void *src, size_t size)
{
    char *d = dest;
    const char *s = src;

    if (d == s) {
        return d;
    }

    if ((uintptr_t)s-(uintptr_t)d-size <= -2*size) {
        return memcpy(d, s, size);
    }

    if (d < s) {
#ifdef __GNUC__
        if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
            while ((uintptr_t)d % WS) {
                if (!size--) {
                    return dest;
                }
                *d++ = *s++;
            }

            for (; size>=WS; size-=WS, d+=WS, s+=WS) {
                *(WT *)d = *(WT *)s;
            }
        }
#endif
        for (; size; size--) {
            *d++ = *s++;
        }
    } else {
#ifdef __GNUC__
        if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
            while ((uintptr_t)(d+size) % WS) {
                if (!size--) {
                    return dest;
                }
                d[size] = s[size];
            }
            while (size>=WS) {
                size-=WS, *(WT *)(d+size) = *(WT *)(s+size);
            }
        }
#endif
        while (size) {
            size--, d[size] = s[size];
        }
    }

    return dest;
}
