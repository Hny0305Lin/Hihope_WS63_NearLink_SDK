/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: get resource info function transfer
 */
#ifndef DFX_RES_H
#define DFX_RES_H

#include "errcode.h"
#include "dfx_os_st.h"

errcode_t dfx_os_get_resource_status(const osal_os_resource_use_stat_t *os_resource_stat);
#endif