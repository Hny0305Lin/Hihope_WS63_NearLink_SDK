#ifndef _STDDEF_H
#define _STDDEF_H

#include <bits/alltypes.h>

#ifdef __LITEOS__
#undef NULL
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0L
#else
#define NULL ((void*)0)
#endif
#endif

#define __NEED_wchar_t
#define __NEED_size_t
#define __NEED_uint8_t
#define __NEED_int8_t
#define __NEED_uint16_t
#define __NEED_uint32_t
#define __NEED_int32_t
#define __NEED_ptrdiff_t
#if __STDC_VERSION__ >= 201112L || __cplusplus >= 201103L
#define __NEED_max_align_t
#endif

#define UINT32_MAX (0xFFFFFFFF)
#define UINT16_MAX (0xFFFF)

#if __GNUC__ > 3
#define offsetof(type, member) __builtin_offsetof(type, member)
#else
#define offsetof(type, member) ((size_t)( (char *)&(((type *)0)->member) - (char *)0 ))
#endif

#endif
