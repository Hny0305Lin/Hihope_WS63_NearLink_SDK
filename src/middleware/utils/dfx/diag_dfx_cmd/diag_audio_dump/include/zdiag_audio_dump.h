/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: audio dump by zdiag
 */

#ifndef __ZDIAG_AUDIO_DUMP_H__
#define __ZDIAG_AUDIO_DUMP_H__

#include "errcode.h"

typedef struct {
    uint32_t file_size;
    uint32_t mod_handle;
    uint32_t mod_output;
} audio_dump_cmd_item;

errcode_t zdiag_audio_dump_init(void);

#endif /* __ZDIAG_AUDIO_DUMP_H__ */
