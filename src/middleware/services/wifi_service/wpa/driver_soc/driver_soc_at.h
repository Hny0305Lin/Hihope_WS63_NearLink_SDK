/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: driver_soc_at header
 */

#ifndef DRIVER_EXT_AT_H
#define DRIVER_EXT_AT_H
#include "td_base.h"

#ifdef CONFIG_AT_PRINT
td_s32 uapi_at_printf(TD_CONST td_char *fmt, ...);
#else
#define uapi_at_printf(fmt, arg...)
#endif

#endif /* DRIVER_EXT_AT_H */
