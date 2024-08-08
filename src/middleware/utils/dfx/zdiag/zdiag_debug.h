/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag debug
 * This file should be changed only infrequently and with great care.
 */

#ifndef __ZDIAG_DEBUG_H__
#define __ZDIAG_DEBUG_H__

#include "errcode.h"
#include "diag_common.h"
#include "diag_pkt.h"

void zdiag_debug_print_pkt_info(const char *str, uint8_t *pkt);
void zdiag_pkt_printf(const char *str, const diag_pkt_handle_t *pkt);
#endif
