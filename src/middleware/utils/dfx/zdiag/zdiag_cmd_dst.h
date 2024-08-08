/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag cmd process
 * This file should be changed only infrequently and with great care.
 */

#ifndef __ZDIAG_CMD_DST_H__
#define __ZDIAG_CMD_DST_H__

#include "errcode.h"
#include "diag.h"
#include "diag_common.h"
#include "diag_pkt.h"

errcode_t diag_pkt_router_run_cmd(diag_pkt_handle_t *pkt, const diag_option_t *option);
#endif
