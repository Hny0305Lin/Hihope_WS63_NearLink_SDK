/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   PANIC module internal header
 * Author:
 * Create:
 */

#ifndef PANIC_PRIVATE_H
#define PANIC_PRIVATE_H

#include "panic.h"
#if defined USE_CMSIS_OS
#include "cmsis_os2.h"
#endif
#ifdef SUPPORT_DFX_PRESERVE
#include "preserve.h"
#endif
#include "systick.h"

#endif
