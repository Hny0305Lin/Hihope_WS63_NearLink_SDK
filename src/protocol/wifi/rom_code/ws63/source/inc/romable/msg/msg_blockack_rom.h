/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file
 */

#ifndef MSG_BLOCKACK_ROM_H
#define MSG_BLOCKACK_ROM_H
#include "osal_types.h"

typedef struct {
    osal_u16      user_index;
    osal_u8       vap_id;
    osal_u8       cur_protocol;
} mac_ctx_event_stru;

#endif