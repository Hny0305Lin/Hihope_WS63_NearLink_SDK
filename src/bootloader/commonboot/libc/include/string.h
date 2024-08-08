/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: mini string.h
 */

#ifndef	__STRING_H__
#define	__STRING_H__

#include <bits/alltypes.h>
#include "features.h"

#ifdef __cplusplus
extern "C" {
#endif

void *memcpy (void *__restrict, const void *__restrict, size_t);
size_t strlen (const char *);
int memcmp (const void *cs, const void *ct, unsigned int count);
void *memset (void *, int, size_t);
void *memmove (void *, const void *, size_t);
int strcmp (const char *, const char *);
#ifdef __cplusplus
}
#endif

#endif
