/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag mem
 * This file should be changed only infrequently and with great care.
 */

#ifndef __ZDIAG_MEM_H__
#define __ZDIAG_MEM_H__
#include "diag_common.h"
#include "diag_channel.h"
#include "diag_pkt.h"


errcode_t diag_pkt_need_cross_task(diag_pkt_handle_t *pkt);
void diag_pkt_free(diag_pkt_handle_t *pkt);

#endif
