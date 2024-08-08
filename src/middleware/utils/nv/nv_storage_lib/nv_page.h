/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library page access interface
 */

#ifndef NV_PAGE_H
#define NV_PAGE_H

#include "nv_key.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/** Represents the location in flash of a KV page. Provided in such a way that it can't be dereferenced,
 * as flash is not directly addressable */
typedef const void *kv_page_location;

typedef struct {
    uint16_t store_id;     /* Identifies the page as belonging to a certain store */
    uint8_t  ver;
    uint8_t  page_index;   /* Uniquely identifies a page within a store */
} kv_page_details_t;

typedef struct {
    kv_page_details_t  details;
    uint32_t           inverted_details_word;
    uint32_t           sequence_number;
    uint32_t           inverted_sequence_number;
} kv_page_header_t;

/* A KV page handle, which provides both the page header information and the location in flash */
typedef struct {
    kv_page_header_t    page_header;
    kv_page_location    page_location;   /* Location in flash of the KV page */
} kv_page_handle_t;

typedef struct {
    uint32_t total_space;               /* 本页最大空间（不含页头） */
    uint32_t used_space;                /* 本页已使用空间 */
    uint32_t reclaimable_space;         /* 本页可回收空间(包含已损坏空间) */
    uint32_t corrupted_space;           /* 本页已损坏空间 */
    uint32_t max_key_space;             /* 本页可写的最大空间（未使用+可回收） */
    uint32_t first_writable_location;   /* 本页第一个可写入位置 */
} kv_page_status_t;

typedef struct {
    uint32_t page_location;             /* 本页首地址（包含页头） */
    uint16_t used_space;                /* 本页已使用空间 */
    uint16_t reclaimable_space;         /* 本页可回收空间(包含已损坏空间) */
    uint16_t corrupted_space;           /* 本页已损坏空间 */
    uint16_t first_writable_offset;     /* 本页第一个可写入位置（相对页首地址的偏移） */
} nv_page_status_map_t;

errcode_t kv_page_get_index(kv_page_handle_t *page, uint32_t *page_index);
errcode_t kv_page_find_first_key(const kv_page_handle_t *page, kv_key_filter_t *search_filter,
    kv_key_handle_t *key);
errcode_t kv_page_find_next_key(const kv_page_handle_t *page, kv_key_filter_t *search_filter,
    kv_key_handle_t *key);
void kv_page_get_status(kv_page_handle_t *page, kv_page_status_t *page_status);
void kv_page_get_status_from_map(kv_page_handle_t *page, kv_page_status_t *page_status);
void kv_page_read_status_to_map(kv_page_handle_t *page, nv_page_status_map_t *status_map);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* NV_PAGE_H_ */
