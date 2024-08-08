/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: adapt PSD
 * This file should be changed only infrequently and with great care.
 */

#ifndef ZDIAG_ADAPT_PSD_H
#define ZDIAG_ADAPT_PSD_H

#include "diag.h"

errcode_t diag_cmd_psd_enable(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
typedef void (*psd_enable_callback)(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
void uapi_zdiag_psd_enable_unregister(void);
void uapi_zdiag_psd_enable_register(psd_enable_callback cb);
psd_enable_callback uapi_zdiag_psd_enable_get_cb(void);

#endif