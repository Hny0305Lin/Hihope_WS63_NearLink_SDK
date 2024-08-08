/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag rx
 * This file should be changed only infrequently and with great care.
 */
#ifndef __ZDIAG_RX_API_H__
#define __ZDIAG_RX_API_H__
#include "diag_common.h"
int32_t soc_diag_channel_rx_mux_char_data(diag_channel_id_t id, uint8_t *data, uint16_t size);
#endif
