/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: mini string.h
 */

#ifndef __STDARG_H__
#define __STDARG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define __NEED_va_list
#include <bits/alltypes.h>

#define va_copy(d,s)    __builtin_va_copy(d,s)
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_end(v)       __builtin_va_end(v)

#ifdef __cplusplus
}
#endif

#endif
