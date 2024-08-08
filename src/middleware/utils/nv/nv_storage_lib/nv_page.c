/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library page access module implementation
 */

#include "nv_page.h"
#include "nv_key.h"
#include "nv_nvregion.h"
#include "nv_porting.h"
#include "common_def.h"

#define assert_p(x)        ((void)0)

errcode_t kv_page_get_index(kv_page_handle_t *page, uint32_t *page_index)
{
    if ((page->page_location != (kv_page_location)NULL) &&
        (~page->page_header.inverted_details_word == *(uint32_t *)(uintptr_t)&page->page_header.details) &&
        (~page->page_header.inverted_sequence_number == page->page_header.sequence_number)) {
        *page_index = page->page_header.details.page_index;
        return ERRCODE_SUCC;
    }
    return ERRCODE_NV_INVALID_PAGE;
}

errcode_t kv_page_find_first_key(const kv_page_handle_t *page, kv_key_filter_t *search_filter,
                                 kv_key_handle_t *key)
{
    kv_key_location key_location = (kv_key_location)((uintptr_t)page->page_location + sizeof(kv_page_header_t));
    errcode_t res = kv_key_get_handle_from_location(key_location, key);
#if (CONFIG_NV_SUPPORT_SKIP_CORRUPT_KEY == NV_YES)
    if (res == ERRCODE_NV_INVALID_KEY_HEADER) {
        nv_log_debug("[NV] kv_page_find_first_key: key magic err (location = 0x%x)!\r\n", key_location);
        res = kv_key_get_next_magic_position(key_location, key);
    }
#endif
    while (res == ERRCODE_SUCC) {
        res = kv_key_does_filter_match(key, search_filter);
        if (res == ERRCODE_SUCC) {
            return ERRCODE_SUCC;
        }
        res = kv_key_get_next_handle(key, KV_KEY_NOT_VALIDATE);
    }
    return res;
}

errcode_t kv_page_find_next_key(const kv_page_handle_t *page, kv_key_filter_t *search_filter,
                                kv_key_handle_t *key)
{
    errcode_t res = kv_key_get_next_handle(key, KV_KEY_NOT_VALIDATE);
    unused(page);
    while (res == ERRCODE_SUCC) {
        res = kv_key_does_filter_match(key, search_filter);
        if (res == ERRCODE_SUCC) {
            return ERRCODE_SUCC;
        }
        res = kv_key_get_next_handle(key, KV_KEY_NOT_VALIDATE);
    }
    return res;
}

STATIC void kv_page_copy_status_map(kv_page_status_t *page_status, nv_page_status_map_t *page_status_map)
{
    page_status->total_space = KV_PAGE_SIZE - (uint32_t)sizeof(kv_page_header_t);
    page_status->used_space = page_status_map->used_space;
    page_status->reclaimable_space = page_status_map->reclaimable_space;
    page_status->corrupted_space = page_status_map->corrupted_space;
    page_status->max_key_space = page_status->total_space - page_status->used_space + page_status->reclaimable_space;
    page_status->first_writable_location = page_status_map->page_location + page_status_map->first_writable_offset;
}

void kv_page_read_status_to_map(kv_page_handle_t *page, nv_page_status_map_t *status_map)
{
    kv_key_handle_t key;
    uint32_t page_data_loc = (uint32_t)(uintptr_t)page->page_location + (uint32_t)sizeof(kv_page_header_t);

    (void)memset_s(status_map, sizeof(nv_page_status_map_t), 0, sizeof(nv_page_status_map_t));

    status_map->page_location = (uint32_t)(uintptr_t)page->page_location;

    errcode_t key_found = kv_page_find_first_key(page, NULL, &key);
    uint32_t first_key_loc = (uint32_t)(uintptr_t)key.key_location;

    /* 如果第一个KEY不在page起始位置，说明page起始位置处的KEY被破坏 */
    if (first_key_loc > page_data_loc) {
        status_map->corrupted_space += (uint16_t)(first_key_loc - page_data_loc);
        status_map->reclaimable_space += (uint16_t)(first_key_loc - page_data_loc);
        status_map->used_space += (uint16_t)(first_key_loc - page_data_loc);
    }

    while (key_found == ERRCODE_SUCC) {
        uint32_t key_size;
        uint32_t last_key_location;

        key_size = kv_key_flash_size(&key);

        errcode_t ret = kv_key_validate_integrity(&key, true);
        if (ret != ERRCODE_SUCC) {
            /* Key校验失败，则key length不可信，通过获取下一个Key的位置，与当前key的位置差重新计算key_size */
            nv_log_err("[NV] key validate integrity. key id = 0x%x location = 0x%x. ret = 0x%x\r\n",
                key.header.key_id, key.key_location, ret);
            last_key_location = (uint32_t)(uintptr_t)key.key_location;
            key_found = kv_key_get_next_handle(&key, KV_KEY_VALIDATE_WRONG);
            if ((key_found == ERRCODE_SUCC) || (key_found == ERRCODE_NV_KEY_NOT_FOUND)) {
                key_size = (uint32_t)(uintptr_t)key.key_location - last_key_location;
            } else {
                break;
            }

            status_map->corrupted_space += (uint16_t)key_size;
            status_map->reclaimable_space += (uint16_t)key_size;
        } else {
            if (kv_key_is_erased(&key)) {
                status_map->reclaimable_space += (uint16_t)key_size;
            }
            key_found = kv_key_get_next_handle(&key, KV_KEY_VALIDATE_CORRECT);
        }
        status_map->used_space += (uint16_t)key_size;
        assert_p(status_map->used_space <= (KV_PAGE_SIZE - sizeof(kv_page_header_t)));
    }

    status_map->first_writable_offset = (uint16_t)sizeof(kv_page_header_t) + status_map->used_space;
}

void kv_page_get_status(kv_page_handle_t *page, kv_page_status_t *page_status)
{
    nv_page_status_map_t page_status_map;
    kv_page_read_status_to_map(page, &page_status_map);
    kv_page_copy_status_map(page_status, &page_status_map);
}

void kv_page_get_status_from_map(kv_page_handle_t *page, kv_page_status_t *page_status)
{
    kv_nvregion_map_t *nv_map = kv_nvregion_get_map();

    uint32_t page_number = kv_nvregion_get_page_number(page->page_location);
    if (page_number >= nv_map->num_entries) {
        return;
    }

    kv_page_copy_status_map(page_status, &(nv_map->page_status_map[page_number]));
    return;
}
