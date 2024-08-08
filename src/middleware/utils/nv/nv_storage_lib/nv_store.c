/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library store access module implementation
 */

#include "nv_store.h"
#include "nv_key.h"
#include "nv_page.h"
#include "nv_nvregion.h"
#include "securec.h"
#include "nv_porting.h"
#include "common_def.h"
#include "nv.h"

#define kv_page_base(a)               ((uint8_t *)(((uint32_t)(a)) & ~(KV_PAGE_SIZE - 1)))
#define kv_is_pointer_in_page(x, p)   ((((uint32_t)(x)) >= ((uint32_t) (p))) && \
                                       (((uint32_t)(x)) < (((uint32_t) (p)) + KV_PAGE_SIZE)))
#define kv_is_pointer_word_aligned(x) (((uint32_t)(x) & 0x3) == 0)
#define kv_bytes_to_words(x)          (((x)+3) >> 2) /* minimal number of words needed to store that many bytes */
#define kv_is_mem_in_page(x, y, p)    ((kv_is_pointer_in_page((x), (p))) && \
                                       (kv_is_pointer_in_page(((uint32_t)(x) + ((uint32_t)(y) - 1)), (p))))

/**
 * Array holding the id for each KV store
 */
static const uint16_t g_kv_store_ids[KV_STORE_MAX_NUM] = {
    KV_STORE_ID_ACPU,
};

/**
 *  Array holding the number of pages used by each KV store
 */
/* Consider using NV region contents to determine actual number of pages and stores */
static const uint8_t g_kv_store_num_pages[KV_STORE_MAX_NUM] = {
    KV_STORE_PAGES_ACPU,
};

uint16_t kv_store_get_id(kv_store_t store)
{
    if (store < KV_STORE_MAX_NUM) {
        return g_kv_store_ids[store];
    }
    return 0;
}

uint8_t kv_store_get_page_count(kv_store_t store)
{
    if (store < KV_STORE_MAX_NUM) {
        return g_kv_store_num_pages[store];
    }
    return 0;
}

errcode_t kv_store_get_page_handle(kv_store_t store, uint32_t page_index, kv_page_handle_t *page)
{
    errcode_t res;
    uint16_t store_id;
    kv_page_location page_location;

    (void)memset_s(page, sizeof(kv_page_handle_t), 0, sizeof(*page));

    store_id = kv_store_get_id(store);
    res = kv_nvregion_find_page(store_id, (uint8_t)page_index, &page_location, &page->page_header);
    if (res != ERRCODE_SUCC) {
        return res;
    }
    page->page_location = page_location;
    return ERRCODE_SUCC;
}

errcode_t kv_store_find_valid_key(kv_store_t store, kv_key_id key_id, kv_key_handle_t *key)
{
    uint32_t page_index;
    uint32_t pages_in_store;
    kv_key_filter_t search_filter;

    /* We are looking for the first (and only) valid key in a store */
    search_filter.location = 0;
    search_filter.mask = 0xFFFF;
    search_filter.pattern = key_id;
    search_filter.state = KV_KEY_FILTER_STATE_VALID;
    search_filter.type = KV_KEY_FILTER_TYPE_ANY;
    pages_in_store = kv_store_get_page_count(store);
    for (page_index = 0; page_index < pages_in_store; page_index++) {
        kv_page_handle_t page;
        if ((kv_store_get_page_handle(store, page_index, &page) == ERRCODE_SUCC) &&
            (kv_page_find_first_key(&page, &search_filter, key) ==  ERRCODE_SUCC)) {
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NV_KEY_NOT_FOUND;
}

static errcode_t kv_store_find_backup_key(kv_key_id key_id, kv_key_handle_t *key, kv_page_location page_location)
{
    kv_page_handle_t page;
    kv_key_filter_t search_filter;
    page.page_location = page_location;
    /* We are looking for the first (and only) valid key in a store */
    search_filter.location = 0;
    search_filter.mask = 0xFFFF;
    search_filter.pattern = key_id;
    search_filter.state = KV_KEY_FILTER_STATE_VALID;
    search_filter.type = KV_KEY_FILTER_TYPE_ANY;

    if (kv_page_find_first_key(&page, &search_filter, key) ==  ERRCODE_SUCC) {
        return ERRCODE_SUCC;
    }
    return ERRCODE_NV_KEY_NOT_FOUND;
}

errcode_t kv_store_get_key_attr(kv_store_t store, kv_key_id key_id, uint16_t *len, kv_attributes_t *attributes)
{
    errcode_t res = ERRCODE_FAIL;
    kv_key_handle_t key;

    res = kv_store_find_valid_key(store, key_id, &key);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    /* Extract stored attributes from the key */
    *attributes = kv_key_attributes(&key);
    *len = key.header.length;
    return res;
}

errcode_t kv_store_get_backup_key_attr(kv_key_id key_id, uint16_t *len, kv_attributes_t *attributes,
    kv_key_handle_t *backup_key)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    errcode_t res = ERRCODE_FAIL;
    kv_page_location page_location;
    kv_nvregion_area_t* nvregion_area =  nv_get_region_area();
    if (nvregion_area == NULL) {
        return ERRCODE_FAIL;
    }

    for (uint32_t page_num = 0; page_num < KV_BACKUP_PAGE_NUM; page_num++) {
        page_location = (kv_page_location)(uintptr_t)(nvregion_area->nv_backup_addr + page_num * KV_PAGE_SIZE);
        res = kv_store_find_backup_key(key_id, backup_key, page_location);
        if (res != ERRCODE_SUCC && page_num == KV_BACKUP_PAGE_NUM - 1) {
            return res;
        } else if (res == ERRCODE_SUCC) {
            break;
        }
    }

    if (attributes != NULL && len != NULL) {
        /* Extract stored attributes from the backup_key */
        *attributes = kv_key_attributes(backup_key);
        *len = backup_key->header.length;
    }
    return res;
#else
    unused(key_id);
    unused(len);
    unused(attributes);
    unused(backup_key);
    return ERRCODE_SUCC;
#endif
}

errcode_t kv_store_read_backup_key(kv_key_id key_id, kv_store_key_data_t *key_data,
    kv_attributes_t *attributes)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    errcode_t res;
    kv_page_location page_location;
    kv_nvregion_area_t* nvregion_area =  nv_get_region_area();
    if (nvregion_area == NULL) {
        return ERRCODE_FAIL;
    }

    for (uint32_t page_num = 0; page_num < KV_BACKUP_PAGE_NUM; page_num++) {
        page_location = (kv_page_location)(uintptr_t)(nvregion_area->nv_backup_addr + page_num * KV_PAGE_SIZE);
        res = kv_store_get_backup_key(key_id, key_data, attributes, page_location);
        if (res == ERRCODE_SUCC) {
            return res;
        }
    }
    return ERRCODE_NV_KEY_NOT_FOUND;
#else
    unused(key_id);
    unused(key_data);
    unused(attributes);
    return ERRCODE_NV_KEY_NOT_FOUND;
#endif
}


errcode_t kv_store_get_key(kv_store_t store, kv_key_id key_id, kv_store_key_data_t *key_data,
                           kv_attributes_t *attributes)
{
    errcode_t res;
    kv_key_handle_t key;

    res = kv_store_find_valid_key(store, key_id, &key);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    if (attributes != NULL) {
        *attributes = kv_key_attributes(&key);
    }

    /* Attempt to obtain key data from store */
    key_data->kvalue_actual_length = key.header.length;
    if (key_data->kvalue_max_length < key_data->kvalue_actual_length) {
        return ERRCODE_NV_GET_BUFFER_TOO_SMALL;
    }

    res = kv_key_read_data(&key, key_data->kvalue);
    return res;
}

errcode_t kv_store_get_backup_key(kv_key_id key_id, kv_store_key_data_t *key_data, kv_attributes_t *attributes,
    kv_page_location page_location)
{
    errcode_t res_b;
    kv_key_handle_t key;

    res_b = kv_store_find_backup_key(key_id, &key, page_location);
    if (res_b != ERRCODE_SUCC) {
        return res_b;
    }

    if (attributes != NULL) {
        *attributes = kv_key_attributes(&key);
    }

    /* Attempt to obtain key data from store */
    key_data->kvalue_actual_length = key.header.length;
    if (key_data->kvalue_max_length < key_data->kvalue_actual_length) {
        return ERRCODE_NV_GET_BUFFER_TOO_SMALL;
    }

    res_b = kv_key_read_data(&key, key_data->kvalue);
    return res_b;
}

errcode_t kv_store_get_status(kv_store_t store, nv_store_status_t *store_status)
{
    uint32_t pages_in_store;
    uint32_t page_index;

    if ((store >= KV_STORE_MAX_NUM) || (store_status == NULL)) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    (void)memset_s(store_status, sizeof(nv_store_status_t), 0, sizeof(nv_store_status_t));

    pages_in_store = kv_store_get_page_count(store);
    for (page_index = 0; page_index < pages_in_store; page_index++) {
        kv_page_handle_t page;
        kv_page_status_t page_status;
        if (kv_store_get_page_handle(store, page_index, &page) == ERRCODE_SUCC) {
            kv_page_get_status(&page, &page_status);
            store_status->total_space += page_status.total_space;
            store_status->used_space += page_status.used_space;
            store_status->reclaimable_space += page_status.reclaimable_space;
            store_status->corrupted_space += page_status.corrupted_space;
            if (page_status.max_key_space > store_status->max_key_space) {
                store_status->max_key_space = page_status.max_key_space;
            }
        }
    }
    return ERRCODE_SUCC;
}

errcode_t kv_store_find_write_page(kv_store_t store, uint32_t required_space, kv_page_handle_t *page,
                                   kv_page_status_t *page_status)
{
    uint32_t pages_in_store;
    uint32_t page_index;
    kv_page_handle_t page_tmp;
    kv_page_status_t page_status_tmp;
    uint32_t mininal_used_times = 0;
    uint32_t need_defrag_page = (uint32_t)-1;

    pages_in_store = kv_store_get_page_count(store);

    for (page_index = 0; page_index < pages_in_store; page_index++) {
        errcode_t res = kv_store_get_page_handle(store, page_index, page);
        if (res != ERRCODE_SUCC) {
            return res;
        }
        kv_page_get_status_from_map(page, page_status);

        /* 如果有未使用的空间，直接返回该页，空间计算使用加法判断更保险 */
        if (page_status->total_space >= (page_status->used_space + required_space)) {
            return ERRCODE_SUCC;
        }

        /* 如果未找到有未使用空间够用的页，考虑到flash擦写平衡，在可换页的页中找到一个擦写次数最少的 */
        if (page_status->max_key_space >= required_space) {
            uint32_t used_times = kv_nvregion_get_use_times(page->page_location);
            if ((need_defrag_page == (uint32_t)-1) || (used_times < mininal_used_times)) {
                need_defrag_page = page_index;
                page_tmp = *page;
                page_status_tmp = *page_status;
                mininal_used_times = used_times;
            }
        }
    }

    if (need_defrag_page != (uint32_t)-1) {
        *page = page_tmp;
        *page_status = page_status_tmp;
        return ERRCODE_NV_DEFRAGMENTATION_NEEDED;
    }

    return ERRCODE_NV_NO_ENOUGH_SPACE;
}
