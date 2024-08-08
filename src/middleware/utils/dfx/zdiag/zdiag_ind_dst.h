/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag ind process
 * This file should be changed only infrequently and with great care.
 */

#ifndef __ZDIAG_IND_DST_H__
#define __ZDIAG_IND_DST_H__
#include "diag_common.h"
#include "diag.h"
#include "diag_pkt.h"
errcode_t diag_pkt_router_run_ind(diag_pkt_handle_t *pkt, const diag_option_t *option);
#endif
