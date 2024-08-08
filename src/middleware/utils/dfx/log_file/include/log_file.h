/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: offline log file saved to the storage
 */

#ifndef LOG_FILE_H
#define LOG_FILE_H

#include <stdint.h>
#include "errcode.h"
#include "dfx_feature_config.h"

/*
 * 文件类型名
 */
#if defined(CONFIG_DFX_STORE_DIAG_ONLY) && (CONFIG_DFX_STORE_DIAG_ONLY == DFX_YES)
typedef enum {
    STORE_DIAG,
    STORE_MAX
} store_service_t;
#else
typedef enum {
    STORE_DIAG,
    STORE_DUMP,
    STORE_DOTTING,
    STORE_MAX
} store_service_t;
#endif

/*
 * 文件头存储信息
 */
typedef struct {
    uint32_t start_flag;        /* 固定为0xaabbccdd */
    uint32_t version;           /* 版本号 */
    uint32_t update_time;       /* 多文件存储时进行判别文件顺序，可以是时间戳等  */
    uint32_t cur_pos;           /* 将要写入的文件位置  */
    uint32_t file_size;         /* service设置的文件最大字节数 */
    uint32_t first_record_pos;  /* 本文件第一个有效的record */
    uint32_t records;           /* 当前文件的记录条数 */
    uint8_t service_type;       /* 业务类型， 对应store_service  */
    uint8_t offset;             /* head size; 即文件中记录区的起始位置 */
    uint16_t crc;               /* 文件头的CRC校验 */
} store_file_head_t;

/*
 * 日志存储格式信息
 */
typedef struct {
    const char *name;           /* 文件名字，多文件存储时，会在name后面增加后缀_0 、_1 ... */
    const char *path;           /* 文件所在目录，以‘/’结尾 */
    uint32_t file_size;         /* 每个文件的最大容量 unit:字节 */
    uint16_t cache_size;        /* 初始创建文件时申请缓存大小； 关闭、删除文件时释放 */
    uint8_t enable_cache;       /* 是否支持缓存 */
    uint8_t mult_files;         /* 支持的存储文件个数， 为1时表示单文件存储 */
} store_file_cfg_t;

/*
 * 离线存储功能初始化
 */
errcode_t uapi_logfile_init(void);

/*
 * 打开日志存放的文件
 * service_type 文件类型
 * cfg 存储格式信息
 */
errcode_t uapi_logfile_open(store_service_t service_type, const store_file_cfg_t *cfg);

/*
 * 写日志到已打开的文件中
 * service_type 文件类型
 * sub_type 存储硬件类型
 * data 日志数据的指针
 * data_len 日志数据的长度
 */
errcode_t uapi_logfile_write(store_service_t service_type, uint8_t sub_type, uint8_t *data, uint32_t data_len);

/*
 * 关闭日志存放的文件，并摧毁线程，关闭定时器，释放使用者分配的缓存
 * service_type 文件类型
 */
errcode_t uapi_logfile_close(store_service_t service_type);

/*
 * 同步离线日志文件数据
 */
errcode_t uapi_logfile_fsync(store_service_t service_type);

/*
 * 暂时停止日志文件的写入（资源不释放，暂时不再接受日志的写入，和对flash的操作）
 * service_type 文件类型
 */
errcode_t uapi_logfile_suspend(store_service_t service_type);

/*
 * 恢复日志文件的写入
 * service_type 文件类型
 */
errcode_t uapi_logfile_resume(store_service_t service_type);

/*
 * 日志文件重置（删除或擦除日志文件）
 * service_type 文件类型
 * cfg 存储格式信息
 */
errcode_t uapi_logfile_reset(store_service_t service_type, store_file_cfg_t *cfg);

#endif /* LOG_FILE_H */
