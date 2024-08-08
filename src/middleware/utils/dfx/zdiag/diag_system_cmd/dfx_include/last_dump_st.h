/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: last dump
 * This file should be changed only infrequently and with great care.
 */

#ifndef LAST_DUMP_ST_H
#define LAST_DUMP_ST_H
#include <stdint.h>

#define LAST_DUMP_NAME_LEN 64

typedef struct {
    uint32_t file_num;
} last_dump_start_ind_t;

typedef struct {
    char name[LAST_DUMP_NAME_LEN];
    uint32_t total_size;
    uint32_t offset;
    uint32_t size;
    uint32_t crc;
    uint8_t data[0];
} last_dump_data_ind_t;

typedef struct {
    char name[LAST_DUMP_NAME_LEN];
    uint32_t total_size;
    uint32_t start_address;
} last_dump_data_ind_finish_t;

#endif