/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2022. All rights reserved.
 * Description: osal liteos cpp build adapt
 * Author:
 * Create:
 */
#include <pthread.h>
#include "stdint.h"
#include "chip_definitions.h"

#if CHIP_BRANDY && defined(SUPPORT_CXX)
int pthread_rwlock_unlock(pthread_rwlock_t *rw)
{
    return 0;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rw)
{
    return 0;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rw)
{
    return 0;
}

locale_t newlocale(int mask, const char *name, locale_t loc)
{
    return 0;
}

void freelocale(locale_t l)
{
    return;
}

locale_t uselocale(locale_t new)
{
    return 0;
}

float wcstof(const wchar_t *restrict s, wchar_t **restrict p)
{
    return 0;
}

double wcstod(const wchar_t *restrict s, wchar_t **restrict p)
{
    return 0;
}

long double wcstold(const wchar_t *restrict s, wchar_t **restrict p)
{
    return 0;
}

struct lconv *localeconv(void)
{
    return 0;
}
#endif