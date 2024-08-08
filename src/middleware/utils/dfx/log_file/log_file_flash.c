/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: offline log file saved to the storage
 */

#include "log_file.h"
#include "errcode.h"
#include "securec.h"
#include "stdbool.h"
#include "soc_osal.h"
#include "common_def.h"
#include "dfx_adapt_layer.h"
#include "debug_print.h"
#include "log_file_common.h"
#include "log_file_flash.h"

#define INVAID_INDEX_ID       0xFFFFFFFF

#if (CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == YES)
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_NO)

STATIC dfx_flash_op_type_t logfile_get_flash_op_type(store_service_t type)
{
    dfx_flash_op_type_t flash_op_type;
    switch (type) {
        case STORE_DIAG:
            flash_op_type = FLASH_OP_TYPE_LOG_FILE;
            break;
        default:
            flash_op_type = FLASH_OP_TYPE_MAX;
            break;
    }
    return flash_op_type;
}

STATIC uint32_t logfile_check_record_head_null(store_record_info_t *record_header, uint32_t start_pos)
{
    uint8_t i;
    uint8_t* record_data = (uint8_t *)(uintptr_t)record_header;
    for (i = 0; i < sizeof(store_record_info_t); i++) {
        if (record_data[i] != 0xFF) {
            break;
        }
    }

    /* 如当前位置数据为全FF, 直接返回记录头长度 */
    if (i == (uint8_t)sizeof(store_record_info_t)) {
        return (uint32_t)sizeof(store_record_info_t);
    }

    uint32_t remain_len = FLASH_SECTOR_SIZE - (start_pos % FLASH_SECTOR_SIZE);

    /* 剩余长度大于记录头长度，即当前位置不在flash页的结尾部分，则直接返回0 */
    if (remain_len > sizeof(store_record_info_t)) {
        return 0;
    }

    /* 当前位置在flash页的结尾部分，则计算从当前位置至flash页的结尾是否全FF，如果是则返回长度，否则返回0 */
    for (i = 0; i < remain_len; i++) {
        if (record_data[i] != 0xFF) {
            break;
        }
    }

    if (i == remain_len) {
        return remain_len;
    }
    return 0;
}

STATIC errcode_t flash_erase_older_records(const store_file_info_t *file_info, uint32_t data_len)
{
    uint32_t space_left = file_info->flash_cur_pos % FLASH_SECTOR_SIZE == 0 ? 0 :
        FLASH_SECTOR_SIZE - file_info->flash_cur_pos % FLASH_SECTOR_SIZE;
    uint32_t next_sector_pos =  file_info->flash_cur_pos + space_left;

    while (space_left < data_len) {
        /* 获取下一个sector的位置 */
        if (next_sector_pos == file_info->file_cfg.file_size) {
            next_sector_pos = 0;
        }
        dfx_flash_erase(logfile_get_flash_op_type(file_info->type), next_sector_pos, FLASH_SECTOR_SIZE);

        next_sector_pos += FLASH_SECTOR_SIZE;
        space_left += FLASH_SECTOR_SIZE;
    }

    return ERRCODE_SUCC;
}

STATIC errcode_t write_cache_to_flash(store_file_info_t *file_info, uint8_t *data, uint32_t data_len)
{
    uint32_t flash_left = file_info->file_cfg.file_size - file_info->flash_cur_pos;
    flash_erase_older_records(file_info, data_len);

    dfx_flash_op_type_t flash_op_type = logfile_get_flash_op_type(file_info->type);
    if (data_len > flash_left) {
        uint32_t data_left_len = data_len - flash_left;
        dfx_flash_write(flash_op_type, file_info->flash_cur_pos, data, flash_left, 0);
        dfx_flash_write(flash_op_type, 0, (uint8_t *)&(file_info->file_head), sizeof(store_file_head_t), 0);
        dfx_flash_write(flash_op_type, sizeof(store_file_head_t), data + flash_left, data_left_len, 0);
        file_info->flash_cur_pos = (uint32_t)sizeof(store_file_head_t) + data_left_len;
    } else {
        dfx_flash_write(flash_op_type, file_info->flash_cur_pos, data, data_len, 0);
        file_info->flash_cur_pos += data_len;
    }

    return ERRCODE_SUCC;
}

errcode_t logfile_write_cache_to_flash(store_file_info_t *file_info)
{
    store_cache_t *cache = file_info->cache;
    uint8_t *read_data;
    int32_t read_len;
    uint32_t tmp_pos = 0;

    /* 读取 cache_write_pos 的瞬时值 */
    tmp_pos = cache->cache_write_pos;

    /* cache中没有新数据，直接退出 */
    if (tmp_pos == cache->cache_read_pos) {
        return ERRCODE_SUCC;
    }

    read_data = (uint8_t *)cache->data + cache->cache_read_pos;
    osal_mutex *file_write_mutex = &(logfile_get_manage()->file_write_mutex);
    osal_mutex_lock(file_write_mutex);
    if (tmp_pos > cache->cache_read_pos) {
        read_len = (int32_t)(tmp_pos - cache->cache_read_pos);
        write_cache_to_flash(file_info, read_data, (uint32_t)read_len);
    } else {
        read_len = (int32_t)(cache->cache_size - cache->cache_read_pos);
        write_cache_to_flash(file_info, read_data, (uint32_t)read_len);
        write_cache_to_flash(file_info, (uint8_t *)cache->data, tmp_pos);
    }

    cache->cache_read_pos = tmp_pos;
    osal_mutex_unlock(file_write_mutex);
    return ERRCODE_SUCC;
}

STATIC bool logfile_flash_without_head(store_file_info_t *file_info)
{
    store_file_head_t *read_flash_head = (store_file_head_t *)dfx_malloc(0, sizeof(store_file_head_t));
    if (read_flash_head == NULL) {
        return false;
    }

    dfx_flash_op_type_t flash_op_type = logfile_get_flash_op_type(file_info->type);
    dfx_flash_read(flash_op_type, 0, (uint8_t *)read_flash_head, sizeof(store_file_head_t));

    logfile_init_file_head(file_info);
    if (read_flash_head->start_flag != FILE_HEAD_START_FLAG) {
        dfx_flash_erase(flash_op_type, 0, FLASH_SECTOR_SIZE);
        dfx_flash_write(flash_op_type, 0, (const uint8_t *)&file_info->file_head, sizeof(store_file_head_t), 0);
        dfx_free(0, read_flash_head);
        file_info->index = 0;
        file_info->flash_cur_pos = (uint32_t)sizeof(store_file_head_t);
        return true;
    }

    dfx_free(0, read_flash_head);
    return false;
}

STATIC void logfile_read_record_from_flash(store_file_info_t *file_info, store_record_info_t *record_info,
                                           uint32_t start_addr)
{
    if (start_addr > file_info->file_cfg.file_size) {
        return;
    }

    memset_s(record_info, sizeof(store_record_info_t), 0, sizeof(store_record_info_t));
    dfx_flash_op_type_t flash_op_type = logfile_get_flash_op_type(file_info->type);
    if (start_addr + (uint32_t)sizeof(store_record_info_t) > file_info->file_cfg.file_size) {
        uint32_t tmp_len = file_info->file_cfg.file_size - start_addr;
        dfx_flash_read(flash_op_type, start_addr, (uint8_t *)record_info, tmp_len);
        dfx_flash_read(flash_op_type, sizeof(store_file_head_t),
            (uint8_t *)record_info + tmp_len, (sizeof(store_record_info_t) - tmp_len));
    } else {
        dfx_flash_read(flash_op_type, start_addr, (uint8_t *)record_info, sizeof(store_record_info_t));
    }
}

STATIC uint32_t get_circled_cur_pos(store_file_info_t *file_info, uint32_t pos_in)
{
    uint32_t pos_out;

    if (pos_in < file_info->file_cfg.file_size) {
        pos_out = pos_in;
    } else {
        pos_out = pos_in - file_info->file_cfg.file_size + (uint32_t)sizeof(store_file_head_t);
    }
    return pos_out;
}

STATIC bool logfile_is_index_continuous(uint32_t last_index, uint32_t cur_index)
{
    /*
     * 比较当前记录与上一条记录的index，下列三种情况说明index是连续的：
     * 1、当前记录是遍历的第一条记录
     * 2、当前记录index上一条记录大1
     * 3、当前记录的index发生翻转（当前是0，上一条是65535）
    */
    if ((last_index == 0) ||
        (cur_index == last_index + 1) ||
        (cur_index == 0 && last_index == MAX_INDEX_NUM)) {
        return true;
    }
    return false;
}

errcode_t logfile_flash_prepare(store_file_info_t *file_info)
{
    store_record_info_t record_info = { 0 };
    uint32_t first_jump_pos = 0;
    uint32_t first_jump_index = INVAID_INDEX_ID;
    uint32_t last_index = 0;
    uint32_t i;

    /* 如果flash中没有有效的文件头，即第一次打开logfile，直接返回 */
    if (logfile_flash_without_head(file_info)) {
        return ERRCODE_SUCC;
    }

    file_info->index = INVAID_INDEX_ID;

    /* 遍历flash区域，找到最新的写入地址 */
    for (i = (uint32_t)sizeof(store_file_head_t); i < file_info->file_cfg.file_size;) {
        /* 从头开始遍历，从flash读取数据 */
        logfile_read_record_from_flash(file_info, &record_info, i);

        /* 检查当前位置是否为全FF */
        uint32_t null_char_num = logfile_check_record_head_null(&record_info, i);
        if (null_char_num == sizeof(store_record_info_t)) {
            /* 找到记录头全部为FF的数据，结束遍历，此处即为最新记录的位置 */
            file_info->index = (last_index == 0) ? 0 : (last_index + 1);
            file_info->flash_cur_pos = i;
            break;
        } else if (null_char_num > 0 && null_char_num < sizeof(store_record_info_t)) {
            /* 找到页的结尾处有FF，但长度不够记录头的长度 */
            file_info->index = (last_index == 0) ? 0 : (last_index + 1);
            file_info->flash_cur_pos = i;
            i += null_char_num;
            continue;
        }

        /* 检查当前位置是否是一个有效的数据头 */
        if (logfile_check_record_head_valid(&record_info) != true) {
            i++; /* 如果不是有效记录头，移至下一个字节 */
            continue;
        }

        if (!logfile_is_index_continuous(last_index, record_info.index)) {
            if (file_info->index != INVAID_INDEX_ID) {
                /* 在flash页结尾找到FF的情况下，找到index不连续的情况，结束遍历，此处即为最新记录的位置 */
                break;
            } else if (first_jump_index == INVAID_INDEX_ID) {
                /* 在未找到全FF数据的情况下，记录第一个index不连续的位置 */
                first_jump_index = last_index;
                first_jump_pos = i;
            }
        }

        last_index = record_info.index;
        i += record_info.len;
    }

    if ((file_info->index == INVAID_INDEX_ID) && (first_jump_index != INVAID_INDEX_ID)) {
        file_info->index = (first_jump_index == 0) ? 0 : (first_jump_index + 1);
        file_info->flash_cur_pos = first_jump_pos;
    } else if ((file_info->index == INVAID_INDEX_ID) && (first_jump_index == INVAID_INDEX_ID)) {
        file_info->index = (last_index == 0) ? 0 : (last_index + 1);
        file_info->flash_cur_pos = get_circled_cur_pos(file_info, i);
    }

    dfx_log_info("Found flash_cur_pos = 0x%x last index = 0x%x\r\n", file_info->flash_cur_pos, file_info->index);
    return ERRCODE_SUCC;
}

errcode_t logfile_flash_erase(store_service_t service_type, const store_file_cfg_t *cfg)
{
    return dfx_flash_erase(logfile_get_flash_op_type(service_type), 0, cfg->file_size);
}

#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */
#endif /* CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE */