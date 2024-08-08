/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: get cpup function transfer
 */
#ifndef DFX_CPUP_H
#define DFX_CPUP_H

#include <stdint.h>
#include "errcode.h"
#include "dfx_os_st.h"

uint32_t dfx_cpup_get_all_usage(uint16_t max_num, dfx_cpup_item_usage_info_t *cpup_info, uint32_t mode, uint16_t flag);

void dfx_cpup_reset(void);
void dfx_cpup_stop(void);
void dfx_cpup_start(void);
#endif