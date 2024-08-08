/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: string
 *
 * Create: 2021-12-16
 */

#include <linux/kernel.h>
#include "soc_osal.h"
#include "osal_inner.h"
int osal_memncmp(const void *buf1, const void *buf2, unsigned long size)
{
    return memcmp(buf1, buf2, (size_t)size);
}
int osal_strcmp(const char *cs, const char *ct)
{
    return strcmp(cs, ct);
}
int osal_strncmp(const char *s1, const char *s2, unsigned long size)
{
    return strncmp(s1, s2, (size_t)size);
}

int osal_strcasecmp(const char *s1, const char *s2)
{
    return strcasecmp(s1, s2);
}
int osal_strncasecmp(const char *s1, const char *s2, unsigned long size)
{
    return strncasecmp(s1, s2, (size_t)size);
}

char *osal_strchr(const char *s, int c)
{
    return strchr(s, c);
}
char *osal_strnchr(const char *s, int count, int c)
{
    if (s == NULL || count <= 0) {
        return NULL;
    }

    while ((count-- != 0) && (*s != '\0')) {
        if (*s == (char)c) {
            return (char *)s;
        }
        ++s;
    }
    return NULL;
}

char *osal_strrchr(const char *s, int c)
{
    return strrchr(s, c);
}

char *osal_strstr(const char *s1, const char *s2)
{
    return strstr(s1, s2);
}
char *osal_strnstr(const char *s1, const char *s2, int len)
{
    size_t len1, len2, slen_tmp;
    if (s1 == NULL || s2 == NULL) {
        return NULL;
    }
    len2 = strlen(s2);
    if (len2 == 0) {
        return (char *)s1;
    }
    if (len < len2) {
        return NULL;
    }
    len1 = strlen(s1);
    slen_tmp = len > len1 ? len1 : len;

    for (; slen_tmp >= len2; slen_tmp--) {
        if (!strncmp(s1, s2, len2)) {
            return (char *)s1;
        }
        s1++;
    }

    return NULL;
}
unsigned int osal_strlen(const char *s)
{
    return strlen(s);
}
unsigned int osal_strnlen(const char *s, unsigned int count)
{
    return strnlen(s, count);
}
char *osal_strpbrk(const char *cs, const char *ct)
{
    return strpbrk(cs, ct);
}
char *osal_strsep(char **s, const char *ct)
{
    return strsep(s, ct);
}
unsigned int osal_strspn(const char *s, const char *accept)
{
    return strspn(s, accept);
}
unsigned int osal_strcspn(const char *s, const char *reject)
{
    return strcspn(s, reject);
}
void *osal_memscan(void *addr, int c, int size)
{
    return NULL;
}
int osal_memcmp(const void *cs, const void *ct, int count)
{
    return memcmp(cs, ct, (size_t)count);
}
void *osal_memchr(const void *s, int c, int n)
{
    return memchr(s, c, n);
}
void *osal_memchr_inv(const void *start, int c, int bytes)
{
    osal_unused(start, c, bytes);
    return NULL;
}
unsigned long long osal_strtoull(const char *cp, char **endp, unsigned int base)
{
    osal_unused(cp, endp, (int)base);
    return 0;
}
unsigned long osal_strtoul(const char *cp, char **endp, unsigned int base)
{
    return strtoul(cp, endp, (int)base);
}
long osal_strtol(const char *cp, char **endp, unsigned int base)
{
    return simple_strtol(cp, endp, base);
}
long long osal_strtoll(const char *cp, char **endp, unsigned int base)
{
    osal_unused(cp, endp, base);
    return 0;
}
