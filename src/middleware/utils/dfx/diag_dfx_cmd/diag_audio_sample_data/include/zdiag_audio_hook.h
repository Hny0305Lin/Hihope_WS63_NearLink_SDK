/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: message for capturing data between core a and the dsp.
 * Author:
 */

#ifndef __ZDIAG_AUDIO_HOOK_H__
#define __ZDIAG_AUDIO_HOOK_H__

#include "td_type.h"

typedef enum {
    HOOK_AUDIO_DATA_START,
    HOOK_AUDIO_DATA_STOP,
} hook_audio_data_status;

typedef struct {
    td_u32 cnt; /* 数据帧计数 */
    td_u32 size;
    td_u32 node_id;
} audio_hook_header;

typedef struct {
    audio_hook_header hdr;
    td_s32 data[];
} audio_hook_frame;

typedef struct {
    hook_audio_data_status status;
    td_u32 id;
} audio_hook_node;

td_void hook_audio_data_start(td_u32 node_id);
td_void hook_audio_data_stop(td_u32 node_id);

#endif /* __ZDIAG_AUDIO_HOOK_H__ */
