/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library non-volatile region access module implementation
 */

#include "nv_nvregion.h"
#include "nv_page.h"
#include "nv_store.h"
#include "nv_porting.h"
#include "nv_config.h"
#include "common_def.h"
#ifndef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
#include "flash_task_mutex.h"
#include "flash_task_adapt.h"
#endif

static kv_nvregion_map_t g_kv_nvregion_map = {NULL, NULL, 0};
static kv_nvregion_area_t g_kv_nvregion_area = {KV_STORE_START_ADDR, KV_STORE_DATA_SIZE,
    KV_BACKUP_START_ADDR, KV_BACKUP_DATA_SIZE};

kv_nvregion_map_t* kv_nvregion_get_map(void)
{
    return &g_kv_nvregion_map;
}

uint32_t kv_nvregion_get_use_times(kv_page_location page_location)
{
    uint32_t page_number = kv_nvregion_get_page_number(page_location);
    if (page_number == (uint32_t)-1) {
        return (uint32_t)-1;
    }

    return g_kv_nvregion_map.entries[page_number].sequence_number;
}

static void kv_nvregion_clear_map_entry(kv_page_header_t *map_entry)
{
    (void)memset_s(map_entry, sizeof(kv_page_header_t), 0, sizeof(kv_page_header_t));
}

static errcode_t kv_nvregion_is_valid_map_entry(const kv_page_header_t *map_entry)
{
    if ((map_entry->details.store_id != 0) && (map_entry->details.store_id != 0xFFFF) &&
        (~map_entry->inverted_details_word == *(uint32_t *)(uintptr_t)&map_entry->details) &&
        (~map_entry->inverted_sequence_number == map_entry->sequence_number)) {
            return ERRCODE_SUCC;
    }
    return ERRCODE_FAIL;
}

static void kv_nvregion_filter_duplicate_map_entries(kv_page_header_t *new_entry)
{
    kv_page_header_t *existing_entry = g_kv_nvregion_map.entries;
    while (existing_entry < new_entry) {
        if ((existing_entry->details.store_id == new_entry->details.store_id) &&
            (existing_entry->details.page_index == new_entry->details.page_index)) {
            if (existing_entry->sequence_number <= new_entry->sequence_number) {
                /* An existing map entry is older than the new one, use the new one */
                /* Two entries have the same sequence number.  This is unexpected. */
                /* For the moment use the new entry, but it could be better to try and determine based on content */
                /* e.g. amount of free space, number of erased keys, corrupted keys */
                kv_nvregion_clear_map_entry(existing_entry);
            } else {
                /* An existing map entry is more recent than the new one */
                kv_nvregion_clear_map_entry(new_entry);
                break;
            }
        }
        existing_entry++;
    }
}

STATIC void kv_nvregion_free_resource(void)
{
    if (g_kv_nvregion_map.entries != NULL) {
        kv_free(g_kv_nvregion_map.entries);
        g_kv_nvregion_map.entries = NULL;
    }
    if (g_kv_nvregion_map.page_status_map != NULL) {
        kv_free(g_kv_nvregion_map.page_status_map);
        g_kv_nvregion_map.page_status_map = NULL;
    }
}

STATIC errcode_t kv_nvregion_init_map(void)
{
    /* Create blank NV region map */
    uint32_t num_entries = g_kv_nvregion_area.nv_data_size / KV_PAGE_SIZE;
    if (num_entries == 0) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    /* Obtain details of NV memory region */
    if (((g_kv_nvregion_area.nv_data_addr & (KV_PAGE_SIZE - 1)) != 0) ||
        ((g_kv_nvregion_area.nv_data_size & (KV_PAGE_SIZE - 1)) != 0) ||
        ((g_kv_nvregion_area.nv_data_size & ~(KV_PAGE_SIZE - 1)) == 0)) {
        /* NV Data address or size not aligned to a page boundry */
        return ERRCODE_FAIL;
    }

    /* Free up any existing NV region map */
    if (num_entries != g_kv_nvregion_map.num_entries) {
        kv_nvregion_free_resource();
    }

    if (g_kv_nvregion_map.entries == NULL) {
        g_kv_nvregion_map.entries = (kv_page_header_t *)kv_zalloc(num_entries * sizeof(kv_page_header_t));
    }

    if (g_kv_nvregion_map.page_status_map == NULL) {
        g_kv_nvregion_map.page_status_map =
            (nv_page_status_map_t *)kv_zalloc(num_entries * sizeof(nv_page_status_map_t));
    }

    if ((g_kv_nvregion_map.entries == NULL) || (g_kv_nvregion_map.page_status_map == NULL)) {
        kv_nvregion_free_resource();
        return ERRCODE_MALLOC;
    }
    g_kv_nvregion_map.num_entries = num_entries;
    return ERRCODE_SUCC;
}

static errcode_t kv_nvregion_build_map(void)
{
    errcode_t ret;
    unused(ret);
    uint32_t nv_page = g_kv_nvregion_area.nv_data_addr;

    for (uint32_t i = 0; i < g_kv_nvregion_map.num_entries; i++) {
        kv_page_header_t *new_entry = &g_kv_nvregion_map.entries[i];
        ret = kv_key_helper_copy_flash((uint32_t)(uintptr_t)new_entry, (uint32_t)nv_page, sizeof(kv_page_header_t));

        if (kv_nvregion_is_valid_map_entry(new_entry) == ERRCODE_SUCC &&
            new_entry->details.store_id != KV_STORE_ID_BACKUP) {
            if (new_entry->details.ver == 0) {
                g_nv_header_magic = KV_KEY_NO_MAGIC;
            }
            kv_nvregion_filter_duplicate_map_entries(new_entry);
            kv_page_handle_t page;
            page.page_location = (kv_page_location)(uintptr_t)nv_page;
            kv_page_read_status_to_map(&page, &(g_kv_nvregion_map.page_status_map[i]));
        } else {
            kv_nvregion_clear_map_entry(new_entry);
        }

        nv_page += KV_PAGE_SIZE;
    }
    nv_log_debug("[NV] g_nv_header_magic = 0x%x.\r\n", g_nv_header_magic);
    return ERRCODE_SUCC;
}

void kv_nvregion_init(uint32_t nv_data_addr, uint32_t nv_data_size, uint32_t nv_backup_addr, uint32_t nv_backup_size)
{
    g_kv_nvregion_area.nv_data_addr = nv_data_addr;
    g_kv_nvregion_area.nv_data_size = nv_data_size;
    g_kv_nvregion_area.nv_backup_addr =  nv_backup_addr;
    g_kv_nvregion_area.nv_backup_size = nv_backup_size;
}

kv_nvregion_area_t* nv_get_region_area(void)
{
    return  &g_kv_nvregion_area;
}

errcode_t kv_nvregion_scan(void)
{
    errcode_t res;
    res = kv_nvregion_init_map();
    if (res != ERRCODE_SUCC) {
        return res;
    }
    return kv_nvregion_build_map();
}

errcode_t kv_nvregion_create_page(uint16_t store_id, uint8_t page_index)
{
    errcode_t res;
    kv_page_location page_location;

    if (g_kv_nvregion_map.entries == NULL) {
        return ERRCODE_NV_NOT_INITIALISED;
    }
    if (store_id == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    res = kv_nvregion_find_unused_page(&page_location);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    res = kv_nvregion_erase_page(page_location);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    res = kv_nvregion_write_page(page_location, store_id, page_index);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    return kv_nvregion_scan();
}

errcode_t kv_nvregion_find_page(uint16_t store_id, uint8_t page_index,
                                kv_page_location *location, kv_page_header_t *header)
{
    if (g_kv_nvregion_map.entries == NULL) {
        return ERRCODE_NV_NOT_INITIALISED;
    }
    if (store_id == 0) {
        return ERRCODE_INVALID_PARAM;
    }
    for (uint32_t i = 0; i < g_kv_nvregion_map.num_entries; i++) {
        kv_page_header_t *map_entry = &g_kv_nvregion_map.entries[i];
        if ((map_entry->details.store_id == store_id) && (map_entry->details.page_index == page_index)) {
            if (location != NULL) {
                uint32_t nv_page = g_kv_nvregion_area.nv_data_addr;
                nv_page += KV_PAGE_SIZE * i;
                *location = (kv_page_location)(uintptr_t)nv_page;
            }
            if (header != NULL) {
                (void)memcpy_s(header, sizeof(kv_page_header_t), map_entry, sizeof(kv_page_header_t));
            }
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NV_PAGE_NOT_FOUND;
}

errcode_t kv_nvregion_find_unused_page(kv_page_location *location)
{
    if (g_kv_nvregion_map.entries == NULL) {
        return ERRCODE_NV_NOT_INITIALISED;
    }

    for (uint32_t i = 0; i < g_kv_nvregion_map.num_entries; i++) {
        kv_page_header_t *map_entry = &g_kv_nvregion_map.entries[i];

        if ((map_entry->details.store_id == 0) && (map_entry->details.page_index == 0) &&
            (map_entry->inverted_details_word == 0) &&
            (map_entry->sequence_number == 0) && (map_entry->inverted_sequence_number == 0)) {
            if (location != NULL) {
                uint32_t nv_page = g_kv_nvregion_area.nv_data_addr;
                nv_page += KV_PAGE_SIZE * i;
                *location = (kv_page_location)(uintptr_t)nv_page;
            }
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NV_PAGE_NOT_FOUND;
}

errcode_t kv_nvregion_erase_page(kv_page_location page_location)
{
    uint32_t nv_page_addr = (uint32_t)(uintptr_t)page_location;
    errcode_t ret;

    if ((nv_page_addr < g_kv_nvregion_area.nv_data_addr) ||
        (nv_page_addr >= (g_kv_nvregion_area.nv_data_addr + g_kv_nvregion_area.nv_data_size))) {
        return ERRCODE_INVALID_PARAM;
    }

    ret = kv_key_erase_flash(nv_page_addr, KV_PAGE_SIZE);
    return ret;
}

errcode_t kv_nvregion_write_page(kv_page_location page_location, uint16_t store_id, uint8_t page_index)
{
    uint32_t written;
    kv_page_header_t page_header;
    unused(written);

    /* Build header for KV page */
    page_header.details.store_id = store_id;
    page_header.details.page_index = page_index;
    page_header.details.ver = (g_nv_header_magic == KV_KEY_MAGIC) ? 1 : 0;
    page_header.inverted_details_word = ~*(uint32_t *)(uintptr_t)&page_header.details;
    page_header.sequence_number = 0;
    page_header.inverted_sequence_number = ~page_header.sequence_number;

    return kv_key_write_flash((uint32_t)(uintptr_t)page_location, sizeof(kv_page_header_t),
                              (uint8_t *)&page_header);
}

uint32_t kv_nvregion_get_page_number(kv_page_location page_location)
{
    uint32_t page_number = ((uint32_t)(uintptr_t)page_location - g_kv_nvregion_area.nv_data_addr) / KV_PAGE_SIZE;
    if (page_number >= g_kv_nvregion_map.num_entries) {
        return (uint32_t)-1;
    }
    return page_number;
}