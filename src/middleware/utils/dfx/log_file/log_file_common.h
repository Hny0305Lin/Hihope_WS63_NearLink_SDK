/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: offline log file saved to the storage
 */

#ifndef LOG_FILE_COMMON_H
#define LOG_FILE_COMMON_H

#include <stdint.h>
#include "errcode.h"
#include "log_file.h"
#include "soc_osal.h"

#define MAX_FILE_PATH_LEN           128
#define MAX_SUFFIX_LEN              8
#define RECORD_HEAD_MAGIC           0x4B3C
#ifndef LOGFILE_SAVE_TASK_SIZE
#define LOGFILE_SAVE_TASK_SIZE      0x800
#endif
#define LOGFILE_SAVE_EVENT_MASK     0x7
#define LOGFILE_TIMER_PERIOD        10000
#define MAX_MUTIFILE_NAME_NUM       9999
#define MAX_MUTIFILE_NUM            100
#define FILE_HEAD_START_FLAG        0xaabbccdd
#define CLOSE_SLEEP_PERIOD          10
#define THREAD_PRIORITY_NUM         24

typedef struct {
    uint16_t magic;
    uint16_t len;           /* 记录头 + 数据的总长度 */
    uint8_t type;           /* 类型 */
    uint8_t rev;            /* 是否有效 */
#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_NO
    uint16_t index;
#endif
    uint16_t crc;           /* CRC16 校验 */
    signed char data[0];
} store_record_info_t;      /* 每条记录的头 */

typedef struct {
    uint32_t cache_write_pos;
    uint32_t cache_read_pos;
    uint32_t cache_size;
    uint32_t threshold_size; /* 门限，cache剩余空间小于门限时，触发保存 */
    signed char data[0];     /* 存储record */
} store_cache_t;

typedef struct {
    uint32_t oldest_file_idx;   /* 多文件存储时，最早的文件序列 */
    uint32_t cur_file_idx;      /* 多文件存储时，当前打开的文件序列 */
    uint32_t file_count;        /* 多文件存储时，当前文件个数 */
} store_muti_file_idx_t;

typedef struct {
    store_file_head_t file_head;
    store_file_cfg_t file_cfg;
    store_service_t type;
    store_muti_file_idx_t muti_file_idx;
    int32_t fd;                     /* 当前打开的文件句柄 */
    int32_t idx_fd;                 /* 索引文件的文件句柄 */
    uint32_t last_save_time;        /* 此文件上次写的时间 */
#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_NO
    uint32_t index;                 /* 当前flash中最新的标识数字 */
    uint32_t flash_cur_pos;         /* 当前文件写入位置 */
#endif
    store_cache_t *cache;
    bool run_flag;
    bool finish_flag;
} store_file_info_t;

typedef struct {
    osal_timer timer_handle;        /* 缓存延后写入文件的定时器 */
    osal_task *task_handle;;        /* 负责文件写入的线程 */
    osal_event event;               /* 线程空闲时等待事件 */
    osal_mutex file_write_mutex;    /* 文件写入的互斥锁 */
    store_file_info_t file_info[STORE_MAX];
} store_file_manage_t;

store_file_manage_t* logfile_get_manage(void);

bool logfile_check_record_head_valid(store_record_info_t *record_header);

void logfile_init_file_head(store_file_info_t *file_info);

#endif /* LOG_FILE_COMMON_H */
