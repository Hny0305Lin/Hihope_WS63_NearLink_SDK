/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: offline log file saved to the storage
 */

#ifndef LOG_FILE_FLASH_H
#define LOG_FILE_FLASH_H

#include "errcode.h"

#define FLASH_SECTOR_SIZE           4096
#define MAX_INDEX_NUM               65535
#define MAX_UNCOVER_FLAG_INTERVAL   5000

errcode_t logfile_write_cache_to_flash(store_file_info_t *file_info);

errcode_t logfile_flash_prepare(store_file_info_t *file_info);

errcode_t logfile_flash_erase(store_service_t service_type, const store_file_cfg_t *cfg);

#endif /* LOG_FILE_FLASH_H */
