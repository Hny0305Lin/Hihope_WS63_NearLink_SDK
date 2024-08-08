/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: sample data st
 * This file should be changed only infrequently and with great care.
 */
#ifndef DIAG_SAMPLE_DATA_ST_H
#define DIAG_SAMPLE_DATA_ST_H
#include <stdint.h>

typedef struct {
    uint32_t flag;
    uint32_t transmit_id;
} diag_sample_data_cmd_t;

typedef struct {
    uint32_t ret;
    uint32_t flag;
    uint32_t transmit_id;
} diag_sample_data_ind_t;

typedef struct {
    uint32_t transmit_id;
    uint32_t ret;
    uint32_t offset;
    uint32_t size;
    uint32_t crc;
    uint8_t data[0];
} diag_sample_data_reply_pkt_t;

#endif