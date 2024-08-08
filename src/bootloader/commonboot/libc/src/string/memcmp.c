/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: Boot string function implementation.
 *
 * Create: 2012-12-22
 */

/**
 * memcmp - Compare two areas of memory
 * @cs: One area of memory
 * @ct: Another area of memory
 * @count: The size of the area.
 */
int memcmp(const void *cs, const void *ct, unsigned int count)
{
    unsigned int cnt = count;
    const unsigned char *l = cs;
    const unsigned char *r = ct;
    for (; (cnt != 0) && (*l == *r); cnt--, l++, r++) {
    };
    return (cnt != 0) ? *l - *r : 0;
}
