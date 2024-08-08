#include <string.h>
#include <stdint.h>
#if defined(__LITEOS__) && defined(LOSCFG_BASE_MEM_NODE_SIZE_CHECK)
#include "los_memory.h"
static int __memset_check(void *dest, unsigned int nodeLength)
{
    unsigned int ret;
    unsigned int totalSize = 0;
    unsigned int availSize = 0;
    unsigned char *pool = m_aucSysMem1;

    ret = LOS_MemNodeSizeCheck(pool, dest, &totalSize, &availSize);
    if ((ret == 0) && (nodeLength > availSize)) {
        return -1;
    }

    return 0;
}
#endif

void *memset(void *dest, int c, size_t n)
{
    typedef uint32_t __attribute__((__may_alias__)) u32;
    typedef uint64_t __attribute__((__may_alias__)) u64;

    unsigned char *s = dest;

    if (!n) {
        return dest;
    }

    s[0] = c;
    s[n-1] = c;

    if (n <= 2) { // 2 bytes mem
        return dest;
    }

    s[1] = c;
    s[2] = c;
    s[n-2] = c;
    s[n-3] = c;

    if (n <= 6) { // 6 bytes mem
        return dest;
    }

    s[3] = c;
    s[n-4] = c;

    if (n <= 8) { // 8 bytes mem
        return dest;
    }

    size_t k = -(uintptr_t)s & 3; // 3 : 4-byte boundary process
    s += k;
    n -= k;
    n &= -4;

    u32 c32 = ((u32)-1) / 255 * (unsigned char)c;

    *(u32 *)(s + 0) = c32;
    *(u32 *)(s + n - 4) = c32;
    if (n <= 8) {
        return dest;
    }

    *(u32 *)(s + 4) = c32;
    *(u32 *)(s + 8) = c32;
    *(u32 *)(s + n - 12) = c32;
    *(u32 *)(s + n - 8) = c32;

    if (n <= 24) { // 24 bytes mem
        return dest;
    }

    *(u32 *)(s + 12) = c32;
    *(u32 *)(s + 16) = c32;
    *(u32 *)(s + 20) = c32;
    *(u32 *)(s + 24) = c32;
    *(u32 *)(s + n - 28) = c32;
    *(u32 *)(s + n - 24) = c32;
    *(u32 *)(s + n - 20) = c32;
    *(u32 *)(s + n - 16) = c32;

    k = 24 + ((uintptr_t)s & 4);
    s += k;
    n -= k;

    u64 c64 = c32 | ((u64)c32 << 32);
    for (; n >= 32; n -= 32, s += 32) {
        *(u64 *)(s + 0) = c64;
        *(u64 *)(s + 8) = c64;
        *(u64 *)(s + 16) = c64;
        *(u64 *)(s + 24) = c64;
    }

    return dest;
}
