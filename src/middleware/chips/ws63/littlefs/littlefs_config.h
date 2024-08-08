/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: little file system config header.
 */

#ifndef LITTLSFS_CONFIG_H
#define LITTLSFS_CONFIG_H

#if defined(CONFIG_LFS_ADAPT_DEBUG) || defined(CONFIG_LFS_ADAPT_WARN) || defined(CONFIG_LFS_ADAPT_ERROR)
#include "debug_print.h"
#endif

#ifdef CONFIG_LFS_ADAPT_DEBUG
#define lfs_debug_print_info(fmt, arg...) print_str("LFS [I]:" fmt, ##arg)
#else
#define lfs_debug_print_info(fmt, arg...)
#endif

#ifdef CONFIG_LFS_ADAPT_WARN
#define lfs_debug_print_warning(fmt, arg...) print_str("LFS [W]:" fmt, ##arg)
#else
#define lfs_debug_print_warning(fmt, arg...)
#endif

#ifdef CONFIG_LFS_ADAPT_ERROR
#define lfs_debug_print_error(fmt, arg...) print_str("LFS [E]:" fmt, ##arg)
#else
#define lfs_debug_print_error(fmt, arg...)
#endif

#endif