/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: software nv by zdiag
 */
#ifndef ZDIAG_NV_H
#define ZDIAG_NV_H

#include "nv.h"
#include "errcode.h"

#define KVALUE_MAX_LENGTH 4060

typedef struct {
    uint16_t key;
    uint16_t crc;
} nv_read_input_t;

typedef struct {
    uint16_t ret;
    uint16_t key;
    uint32_t length;
    uint8_t data[0];
} nv_read_output_t;

typedef struct {
    bool permanent;
    bool encrypted;
    bool non_upgrade;
    uint8_t reserve;
} nv_attr;

typedef struct {
    uint16_t key;
    uint16_t crc;
    uint32_t length;
    uint8_t value[0];
} nv_write_input_t;

typedef struct {
    uint32_t ret;
    uint32_t key;
} nv_write_output_t;

typedef struct {
    uint16_t key;
    uint16_t length;
    bool permanent;
    bool encrypted;
    bool non_upgrade;
    uint8_t value[0];
} nv_write_with_attr_input_t;

typedef struct {
    uint32_t ret;
    uint32_t key;
} nv_write_with_attr_output_t;

typedef struct {
    uint32_t ret;
    uint16_t key;
    uint16_t length;
    nv_attr attr;
    uint8_t value[0];
} nv_read_with_attr_output_t;

typedef struct {
    uint8_t mode;
    uint8_t region_flag[KEY_ID_REGION_MAX_NUM];
    uint8_t resverd;
    uint32_t ret;
} nv_reset_mode_out_t;

typedef struct {
    uint16_t store_id;     /* Identifies the page as belonging to a certain store */
    uint8_t  ver;
    uint8_t  page_index;   /* Uniquely identifies a page within a store */
    uint32_t inverted_details_word;
    uint32_t sequence_number;
    uint32_t inverted_sequence_number;
    uint32_t total_space;               /* 本页最大空间（不含页头） */
    uint32_t used_space;                /* 本页已使用空间 */
    uint32_t reclaimable_space;         /* 本页可回收空间(包含已损坏空间) */
    uint32_t corrupted_space;           /* 本页已损坏空间 */
    uint32_t max_key_space;             /* 本页可写的最大空间（未使用+可回收） */
    uint32_t first_writable_location;   /* 本页第一个可写入位置 */
} nv_page_usage_out;

errcode_t zdiag_nv_init(void);

#endif /* ZDIAG_NV_H */
