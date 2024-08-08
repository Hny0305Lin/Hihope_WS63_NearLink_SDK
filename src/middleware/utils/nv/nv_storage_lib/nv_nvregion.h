/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library non-volatile region access interface
 */

#ifndef NV_NVREGION_H
#define NV_NVREGION_H

#include "nv_page.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    uint16_t key_id;                /* Key ID */
    uint16_t key_len    : 13;       /* Key length */
    uint16_t permanent  : 1;        /* 是否是永久Key */
    uint16_t encrypted  : 1;        /* 是否是加密Key */
    uint16_t upgrade    : 1;        /* 是否可以升级 */
    uint16_t key_offset   : 13;     /* Key的位置（相对页首地址的偏移） */
    uint16_t valid        : 1;      /* Key是否有效 */
    uint16_t corrupted    : 1;      /* Key是否被破坏 */
    uint16_t reserved     : 1;      /* Key预留字段 */
} nv_key_index_map_t;

typedef struct {
    kv_page_header_t        *entries;
    nv_page_status_map_t    *page_status_map;
    uint32_t                num_entries;
} kv_nvregion_map_t;

typedef struct {
    uint32_t nv_data_addr;
    uint32_t nv_data_size;
    uint32_t nv_backup_addr;
    uint32_t nv_backup_size;
} kv_nvregion_area_t;

void kv_nvregion_init(uint32_t nv_data_addr, uint32_t nv_data_size, uint32_t nv_backup_addr, uint32_t nv_backup_size);

errcode_t kv_nvregion_scan(void);
errcode_t kv_nvregion_find_page(uint16_t store_id, uint8_t page_index,
                                kv_page_location *location, kv_page_header_t *header);
errcode_t kv_nvregion_find_unused_page(kv_page_location *location);
errcode_t kv_nvregion_erase_page(kv_page_location page_location);
errcode_t kv_nvregion_create_page(uint16_t store_id, uint8_t page_index);
errcode_t kv_nvregion_write_page(kv_page_location page_location, uint16_t store_id, uint8_t page_index);

kv_nvregion_map_t* kv_nvregion_get_map(void);
kv_nvregion_area_t* nv_get_region_area(void);

uint32_t kv_nvregion_get_page_number(kv_page_location page_location);
uint32_t kv_nvregion_get_use_times(kv_page_location page_location);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* NV_NVREGION_H */
