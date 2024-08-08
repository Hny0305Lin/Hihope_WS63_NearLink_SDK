/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: offline log file saved to the storage
 */

#include <unistd.h>
#include <stdlib.h>
#include "log_file.h"
#include "uapi_crc.h"
#include "fcntl.h"
#include "errcode.h"
#include "securec.h"
#include "stdbool.h"
#include "dirent.h"
#include "soc_osal.h"
#include "common_def.h"
#include "dfx_adapt_layer.h"
#include "sys/stat.h"
#include "debug_print.h"
#include "log_file_common.h"
#include "log_file_file.h"

#if (CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == YES)
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)

#define NOT_SPACE_NUM_MAX         5
STATIC uint8_t g_not_space_nums = 0;
STATIC uint8_t g_full_path[MAX_FILE_PATH_LEN] = {0};

STATIC errcode_t logfile_get_next_record(store_file_info_t *file_info, uint32_t start_pos, uint32_t *next_pos)
{
    store_record_info_t record_info;
    uint32_t record_pos = start_pos;
    uint32_t next_record_pos;
    uint32_t loop_count = 0;

    do {
        int32_t tmp_len = (int32_t)file_info->file_head.file_size - (int32_t)record_pos;

        (void)memset_s(&record_info, sizeof(record_info), 0, sizeof(record_info));

        lseek(file_info->fd, record_pos, SEEK_SET);

        if (tmp_len >= (int32_t)sizeof(store_record_info_t)) {
            (void)read(file_info->fd, &record_info, sizeof(record_info));
            if (tmp_len > record_info.len) {
                next_record_pos = record_pos + record_info.len;
            } else {
                next_record_pos = (uint32_t)(file_info->file_head.offset + record_info.len - tmp_len);
            }
        } else if (tmp_len < (int32_t)sizeof(store_record_info_t)) {
            (void)read(file_info->fd, &record_info, (uint32_t)tmp_len);
            lseek(file_info->fd, file_info->file_head.offset, SEEK_SET);
            (void)read(file_info->fd, ((uint8_t *)&record_info) + tmp_len, sizeof(record_info) - (uint32_t)tmp_len);
            next_record_pos = (uint32_t)(file_info->file_head.offset + record_info.len - tmp_len);
        } else {
            return ERRCODE_DFX_LOGFILE_INTERAL_FAIL;
        }

        if (logfile_check_record_head_valid(&record_info) == true) {
            *next_pos = next_record_pos;
            return ERRCODE_SUCC;
        }

        if (record_pos >= file_info->file_head.file_size) {
            record_pos = file_info->file_head.offset;
        } else {
            record_pos++;
        }
        loop_count++;
    } while (loop_count < file_info->file_head.file_size);

    return ERRCODE_DFX_LOGFILE_RECORD_INVALID;
}

STATIC errcode_t logfile_discard_older_records(store_file_info_t *file_info, uint16_t record_len, uint32_t space_left)
{
    uint32_t next_pos = 0;
    uint32_t space_temp = space_left;
    errcode_t ret;
    while (space_temp < record_len) {
        /* 获取下一个记录的位置 */
        ret = logfile_get_next_record(file_info, file_info->file_head.first_record_pos, &next_pos);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }

        space_temp = space_temp + next_pos - file_info->file_head.first_record_pos;

        file_info->file_head.first_record_pos = next_pos;
        file_info->file_head.records--;
    }

    return ERRCODE_SUCC;
}

STATIC errcode_t logfile_save_data_err(int32_t error_no, int32_t num)
{
    if (error_no != ENOSPC) {
        dfx_log_err("logfile save data failed. errno is %d, num is %d\r\n", error_no, num);
        return ERRCODE_DFX_LOGFILE_WRITE_FAIL;
    }

    if (g_not_space_nums < NOT_SPACE_NUM_MAX) {
        g_not_space_nums++;
        dfx_log_err("logfile save data failed. errno is %d, num is %d\r\n", error_no, num);
    }

    return ERRCODE_DFX_LOGFILE_NOT_ENOUGH_SPACE;
}

STATIC errcode_t logfile_save_data(store_file_info_t *file_info, uint8_t *data, uint32_t data_len)
{
    int32_t write_byte;

    /* 判断数据是否需要翻转至文件头存储 */
    if ((file_info->file_head.file_size - file_info->file_head.cur_pos) >= data_len) {
        write_byte = (int32_t)write(file_info->fd, data, data_len);
        if (write_byte != (int32_t)data_len) {
            return logfile_save_data_err(get_errno(), 0);
        }

        g_not_space_nums = 0;
        file_info->file_head.cur_pos += data_len;
    } else {
        uint32_t record_len_written = file_info->file_head.file_size - file_info->file_head.cur_pos;
        uint32_t record_len_remained = data_len - record_len_written;
        write_byte = write(file_info->fd, data, record_len_written);
        if (write_byte != (int32_t)record_len_written) {
            return logfile_save_data_err(get_errno(), 1); /* 1 : 1st log */
        }
        g_not_space_nums = 0;

        lseek(file_info->fd, file_info->file_head.offset, SEEK_SET);
        write_byte = write(file_info->fd, data + record_len_written, record_len_remained);
        if (write_byte != (int32_t)record_len_remained) {
            return logfile_save_data_err(get_errno(), 2); /* 2 : 2nd log */
        }

        file_info->file_head.cur_pos = file_info->file_head.offset + record_len_remained;
    }
    return ERRCODE_SUCC;
}

errcode_t logfile_single_file_write(store_file_info_t *file_info, uint8_t *data, uint32_t data_len)
{
    store_record_info_t record_info;
    record_info.magic = RECORD_HEAD_MAGIC;
    record_info.type = file_info->type;
    record_info.len = (uint16_t)(sizeof(record_info) + data_len);
    record_info.rev = 1;
    record_info.crc = uapi_crc16(0, (uint8_t *)&record_info, sizeof(store_record_info_t) - sizeof(uint16_t));

    uint32_t space_left;

    if (file_info->file_head.cur_pos >= file_info->file_head.first_record_pos) {
        /* 当前记录位置在第一个记录后面 */
        space_left = file_info->file_head.file_size - file_info->file_head.cur_pos +
                     file_info->file_head.first_record_pos - file_info->file_head.offset;
    } else {
        /* 当前记录位置在第一个记录的前面 */
        space_left = file_info->file_head.first_record_pos - file_info->file_head.cur_pos;
    }

    /* 废弃最旧的记录以腾出空间 */
    logfile_discard_older_records(file_info, record_info.len, space_left);

    lseek(file_info->fd, file_info->file_head.cur_pos, SEEK_SET);

    /* 写入记录头 */
    logfile_save_data(file_info, (uint8_t *)&record_info, (uint32_t)sizeof(record_info));

    /* 写入Data */
    logfile_save_data(file_info, data, data_len);

    file_info->file_head.records += 1;

    /* 更新文件头 */
    file_info->file_head.crc =
        uapi_crc16(0, (uint8_t *)&(file_info->file_head), sizeof(store_file_head_t) - sizeof(uint16_t));
    lseek(file_info->fd, 0, SEEK_SET);
    write(file_info->fd, &(file_info->file_head), sizeof(store_file_head_t));

    return ERRCODE_SUCC;
}

STATIC void logfile_load_multi_files_info(store_file_info_t *file_info)
{
    uint32_t path_len = (uint32_t)strlen(file_info->file_cfg.path);

    (void)memset_s((char *)g_full_path, MAX_FILE_PATH_LEN, 0, MAX_FILE_PATH_LEN);
    if (strncpy_s((char *)g_full_path, MAX_FILE_PATH_LEN, file_info->file_cfg.path, path_len) != EOK) {
        return;
    }
    if (strcat_s((char *)g_full_path, MAX_FILE_PATH_LEN, file_info->file_cfg.name) != EOK) {
        return;
    }
    if (strcat_s((char *)g_full_path, MAX_FILE_PATH_LEN, ".idx") != EOK) {
        return;
    }

    file_info->idx_fd = open((const char *)g_full_path, O_RDWR | O_CREAT, 0664);   /* 0664打开方式 */
    if (file_info->idx_fd <= 0) {
        dfx_log_err("logfile open file %s failed\r\n", g_full_path);
        return;
    }

    lseek(file_info->idx_fd, 0, SEEK_SET);
    uint32_t read_byte = (uint32_t)read(file_info->idx_fd, &file_info->muti_file_idx,
                                        (uint32_t)sizeof(store_muti_file_idx_t));
    if (read_byte != sizeof(store_muti_file_idx_t)) {
        file_info->muti_file_idx.file_count = 0;
        file_info->muti_file_idx.cur_file_idx = 0;
        file_info->muti_file_idx.oldest_file_idx = 0;
        lseek(file_info->idx_fd, 0, SEEK_SET);
        write(file_info->idx_fd, &(file_info->muti_file_idx), sizeof(store_muti_file_idx_t));
        fsync(file_info->idx_fd);
    }
}

STATIC errcode_t logfile_delete_oldest_file(store_file_info_t *file_info, uint8_t *suffix, uint32_t suffix_length)
{
    if (strcpy_s((char *)g_full_path, MAX_FILE_PATH_LEN, file_info->file_cfg.path) != EOK) {
        return ERRCODE_FAIL;
    }
    if (strcat_s((char *)g_full_path, MAX_FILE_PATH_LEN, file_info->file_cfg.name) != EOK) {
        return ERRCODE_FAIL;
    }

    (void)sprintf_s((char *)suffix, suffix_length, ".%04u", file_info->muti_file_idx.oldest_file_idx);
    if (strcat_s((char *)g_full_path, MAX_FILE_PATH_LEN, (char *)suffix) != EOK) {
        return ERRCODE_FAIL;
    }

    if (remove((char *)g_full_path) != 0) {
        dfx_log_debug("logfile remove file [%d] failed. errno is %d \r\n",
            file_info->muti_file_idx.oldest_file_idx, get_errno());
    }

    if (file_info->muti_file_idx.oldest_file_idx == MAX_MUTIFILE_NAME_NUM) {
        file_info->muti_file_idx.oldest_file_idx = 1;
    } else {
        file_info->muti_file_idx.oldest_file_idx++;
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t logfile_open_next_file(store_file_info_t *file_info)
{
    uint8_t suffix[MAX_SUFFIX_LEN] = {0};

    (void)memset_s((char *)g_full_path, MAX_FILE_PATH_LEN, 0, MAX_FILE_PATH_LEN);
    /* 关闭当前文件 */
    close(file_info->fd);

    if (file_info->muti_file_idx.file_count >= file_info->file_cfg.mult_files) {
        /* 如果文件已满，删除最旧的文件 */
        (void)logfile_delete_oldest_file(file_info, suffix, sizeof(suffix));
    }

    /* 打开下一个文件 */
    if (file_info->muti_file_idx.cur_file_idx == MAX_MUTIFILE_NAME_NUM) {
        file_info->muti_file_idx.cur_file_idx = 1;
    } else {
        file_info->muti_file_idx.cur_file_idx++;
    }

    (void)memset_s(g_full_path, MAX_FILE_PATH_LEN, 0, MAX_FILE_PATH_LEN);
    if (strcpy_s((char *)g_full_path, MAX_FILE_PATH_LEN, file_info->file_cfg.path) != EOK) {
        return ERRCODE_FAIL;
    }
    if (strcat_s((char *)g_full_path, MAX_FILE_PATH_LEN, file_info->file_cfg.name) != EOK) {
        return ERRCODE_FAIL;
    }

    (void)memset_s(suffix, MAX_SUFFIX_LEN, 0, MAX_SUFFIX_LEN);
    (void)sprintf_s((char *)suffix, MAX_SUFFIX_LEN, ".%04u", file_info->muti_file_idx.cur_file_idx);
    if (strcat_s((char *)g_full_path, MAX_FILE_PATH_LEN, (char *)suffix) != EOK) {
        return ERRCODE_FAIL;
    }

    file_info->fd = open((const char *)g_full_path, O_RDWR | O_CREAT, 0664);   /* 0664 是以写的方式打开 */
    if (file_info->fd == 0) {
        dfx_log_err("logfile open next file %s failed\r\n", g_full_path);
        return ERRCODE_DFX_LOGFILE_OPEN_FAIL;
    }

    if (file_info->muti_file_idx.file_count < file_info->file_cfg.mult_files) {
        file_info->muti_file_idx.file_count++;
    }

    /* 重置idx文件信息 */
    lseek(file_info->idx_fd, 0, SEEK_SET);
    write(file_info->idx_fd, &(file_info->muti_file_idx), sizeof(store_muti_file_idx_t));
    fsync(file_info->idx_fd);

    /* 重置文件头信息 */
    logfile_init_file_head(file_info);
    return ERRCODE_SUCC;
}

errcode_t logfile_multi_file_write(store_file_info_t *file_info, uint8_t *data, uint32_t data_len)
{
    store_record_info_t record_info;
    errcode_t ret_val;
    record_info.magic = RECORD_HEAD_MAGIC;
    record_info.type = file_info->file_head.service_type;
    record_info.len = (uint16_t)(sizeof(record_info) + data_len);
    record_info.rev = 1;
    record_info.crc = uapi_crc16(0, (uint8_t *)&record_info, sizeof(store_record_info_t) - sizeof(uint16_t));

    int32_t tmp_len = (int32_t)file_info->file_head.file_size - (int32_t)file_info->file_head.cur_pos;

    if (tmp_len < record_info.len) {
        /* 如果本文件空间不足， 打开下一个文件 */
        logfile_open_next_file(file_info);
    }

    lseek(file_info->fd, file_info->file_head.cur_pos, SEEK_SET);

    /* 写入记录头 */
    ret_val = logfile_save_data(file_info, (uint8_t *)&record_info, (uint32_t)sizeof(record_info));
    if (ret_val != ERRCODE_SUCC) {
        if (ret_val != ERRCODE_DFX_LOGFILE_NOT_ENOUGH_SPACE) {
            dfx_log_err("logfile save record head failed, ret = 0x%x\r\n", ret_val);
        }
        return ret_val;
    }

    /* 写入Data */
    ret_val = logfile_save_data(file_info, data, data_len);
    if (ret_val != ERRCODE_SUCC) {
        if (ret_val != ERRCODE_DFX_LOGFILE_NOT_ENOUGH_SPACE) {
            dfx_log_err("logfile save record data failed, ret = 0x%x\r\n", ret_val);
        }
        return ret_val;
    }

    file_info->file_head.records += 1;

    /* 更新文件头 */
    file_info->file_head.crc =
        uapi_crc16(0, (uint8_t *)&(file_info->file_head), sizeof(store_file_head_t) - sizeof(uint16_t));

    lseek(file_info->fd, 0, SEEK_SET);
    write(file_info->fd, &(file_info->file_head), sizeof(store_file_head_t));
    return ERRCODE_SUCC;
}

STATIC errcode_t write_cache_to_single_file(store_file_info_t *file_info, uint8_t *data, uint32_t data_len)
{
    uint32_t space_left;

    if (file_info->file_head.cur_pos >= file_info->file_head.first_record_pos) {
        space_left = file_info->file_head.file_size - file_info->file_head.cur_pos +
                     file_info->file_head.first_record_pos - file_info->file_head.offset;
    } else {
        space_left = file_info->file_head.first_record_pos - file_info->file_head.cur_pos;
    }

    logfile_discard_older_records(file_info, (uint16_t)data_len, space_left);

    lseek(file_info->fd, file_info->file_head.cur_pos, SEEK_SET);

    logfile_save_data(file_info, data, data_len);

    file_info->file_head.crc =
        uapi_crc16(0, (uint8_t *)&(file_info->file_head), sizeof(store_file_head_t) - sizeof(uint16_t));
    lseek(file_info->fd, 0, SEEK_SET);
    write(file_info->fd, &(file_info->file_head), sizeof(store_file_head_t));

    return ERRCODE_SUCC;
}

STATIC errcode_t write_cache_to_multi_file(store_file_info_t *file_info, uint8_t *data, uint32_t data_len)
{
    int32_t tmp_len = (int32_t)file_info->file_head.file_size - (int32_t)file_info->file_head.cur_pos;

    if (tmp_len < (int32_t)data_len) {
        logfile_open_next_file(file_info);
    }

    lseek(file_info->fd, file_info->file_head.cur_pos, SEEK_SET);

    logfile_save_data(file_info, data, data_len);

    file_info->file_head.crc =
        uapi_crc16(0, (uint8_t *)&(file_info->file_head), sizeof(store_file_head_t) - sizeof(uint16_t));

    lseek(file_info->fd, 0, SEEK_SET);
    write(file_info->fd, &(file_info->file_head), sizeof(store_file_head_t));

    return ERRCODE_SUCC;
}

errcode_t logfile_write_cache_to_file(store_file_info_t *file_info)
{
    store_cache_t *cache = file_info->cache;
    uint8_t *read_data;
    int32_t read_len;
    int32_t tmp_pos = 0;

    /* 读取 cache_write_pos 的瞬时值 */
    tmp_pos = (int32_t)cache->cache_write_pos;

    read_data = (uint8_t *)cache->data + cache->cache_read_pos;
    osal_mutex_lock(&(logfile_get_manage()->file_write_mutex));
    if (tmp_pos > (int32_t)cache->cache_read_pos) {
        read_len = tmp_pos - (int32_t)cache->cache_read_pos;
        if (file_info->file_cfg.mult_files == 1) {
            write_cache_to_single_file(file_info, read_data, (uint32_t)read_len);
        } else {
            write_cache_to_multi_file(file_info, read_data, (uint32_t)read_len);
        }
    } else {
        read_len = (int32_t)cache->cache_size - (int32_t)cache->cache_read_pos;
        if (file_info->file_cfg.mult_files == 1) {
            write_cache_to_single_file(file_info, read_data, (uint32_t)read_len);
            write_cache_to_single_file(file_info, (uint8_t *)cache->data, (uint32_t)tmp_pos);
        } else {
            write_cache_to_multi_file(file_info, read_data, (uint32_t)read_len);
            write_cache_to_multi_file(file_info, (uint8_t *)cache->data, (uint32_t)tmp_pos);
        }
    }

    cache->cache_read_pos = (uint32_t)tmp_pos;
    osal_mutex_unlock(&(logfile_get_manage()->file_write_mutex));

    return ERRCODE_SUCC;
}

errcode_t logfile_create_path(store_file_cfg_t *cfg)
{
    int32_t ret;
    if (access(cfg->path, 0) != 0) {
        ret = mkdir(cfg->path, S_IREAD | S_IWRITE);
        if (ret == 0) {
            return ERRCODE_SUCC;
        } else {
            dfx_log_err("mkdir %s failed, ret \r\n", cfg->path);
            return ERRCODE_DFX_LOGFILE_MKDIR_FATAL;
        }
    } else {
        return ERRCODE_SUCC;
    }
}

errcode_t logfile_remove_files(store_service_t service_type, store_file_cfg_t *cfg)
{
    unused(service_type);
    uint8_t full_path_tmp[MAX_FILE_PATH_LEN] = {0};
    struct dirent *pdirent = NULL;
    DIR *d = NULL;
    uint32_t ret = ERRCODE_SUCC;

    d = opendir(cfg->path);
    /* 目录不存在直接返回 */
    if (d == NULL) {
        return ERRCODE_SUCC;
    }

    do {
        pdirent = readdir(d);
        if (pdirent == NULL) {
            break;
        }

        if (strncpy_s((char *)full_path_tmp, MAX_FILE_PATH_LEN, cfg->path, strlen(cfg->path)) != EOK) {
            ret = ERRCODE_FAIL;
            break;
        }
        if (strcat_s((char *)full_path_tmp, MAX_FILE_PATH_LEN, pdirent->d_name) != EOK) {
            ret = ERRCODE_FAIL;
            break;
        }
        if (remove((char *)full_path_tmp) != 0) {
            dfx_log_err("remove %s failed!\r\n", full_path_tmp);
            ret = ERRCODE_FAIL;
            break;
        }
    } while (pdirent != NULL);

    (void)closedir(d);
    return ret;
}

errcode_t logfile_prepare_file_fd(store_file_info_t *file_info, store_file_cfg_t *cfg)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t suffix[MAX_SUFFIX_LEN];
    uint32_t cur_file_idx = 0;

    if (cfg->mult_files > 1) {
        logfile_load_multi_files_info(file_info);
        cur_file_idx = (file_info->muti_file_idx.file_count == 0) ? 1 : file_info->muti_file_idx.cur_file_idx;
    }

    (void)memset_s((char *)g_full_path, MAX_FILE_PATH_LEN, 0, MAX_FILE_PATH_LEN);
    ret = (errcode_t)strcpy_s((char *)g_full_path, MAX_FILE_PATH_LEN, cfg->path);
    ret = (errcode_t)strcat_s((char *)g_full_path, MAX_FILE_PATH_LEN, cfg->name);
    if (cfg->mult_files > 1) {
        (void)sprintf_s((char *)suffix, MAX_SUFFIX_LEN, ".%04u", cur_file_idx);
        ret = (errcode_t)strcat_s((char *)g_full_path, MAX_FILE_PATH_LEN, (char *)suffix);
    }

    file_info->fd = open((const char *)g_full_path, O_RDWR | O_CREAT, 0664);   /* 0664打开方式 */
    if (file_info->fd <= 0) {
        dfx_log_err("logfile open file failed\r\n");
        ret = ERRCODE_DFX_LOGFILE_OPEN_FAIL;
    }

    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (cfg->mult_files > 1 && file_info->muti_file_idx.file_count == 0) {
        file_info->muti_file_idx.cur_file_idx = 1;
        file_info->muti_file_idx.oldest_file_idx = 1;
        file_info->muti_file_idx.file_count = 1;
        lseek(file_info->idx_fd, 0, SEEK_SET);
        write(file_info->idx_fd, &(file_info->muti_file_idx), sizeof(store_muti_file_idx_t));
        fsync(file_info->idx_fd);
    }

    int32_t read_byte = read(file_info->fd, &file_info->file_head, sizeof(store_file_head_t));
    /* 先把文件头读到file_info->file_head */
    /* 判断是否是新文件 */
    if ((read_byte != sizeof(store_file_head_t) || file_info->file_head.start_flag != FILE_HEAD_START_FLAG)) {
        /* 初始化文件头信息 */
        logfile_init_file_head(file_info);
    }
    return ret;
}

#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES */
#endif /* CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE */