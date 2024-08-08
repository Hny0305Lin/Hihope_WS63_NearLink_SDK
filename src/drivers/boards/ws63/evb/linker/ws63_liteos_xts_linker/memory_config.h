/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  Default memory configurations
 *
 * Create: 2021-03-09
 */
#ifndef MEMORY_CONFIG_H
#define MEMORY_CONFIG_H

#include "memory_config_common.h"

/* APP RAM defines
 * APP has base 512K ITCM (Instruction TCM) for code
 *              256K DTCM (Data TCM) for DATA.
 * APP ITCM                 VECTORS TABLE
 *                          RAM TEXT
 *
 * APP DTCM                 STACK
 *                          RAM
 */
/* L2RAM 640K for APP core */
#define L2RAM_OFFSET         0x160000
#define L2RAM_FOR_APP_START  L2RAM_OFFSET
#define L2RAM_FOR_APP_LENGTH 0xA0000    /* 640K */

#endif
