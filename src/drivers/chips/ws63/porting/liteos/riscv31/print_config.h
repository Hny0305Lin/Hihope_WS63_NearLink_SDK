/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: RISCV31 print config interface for LiteOS
 *
 * Create:  2021-10-20
 */
#ifndef PRINT_CONFIG_H
#define PRINT_CONFIG_H

#include "debug_print.h"

#define print_liteos(fmt, args...)  PRINT(fmt, ##args)

#define oam_trace_print(fmt, args...)  PRINT(fmt, ##args)

#endif