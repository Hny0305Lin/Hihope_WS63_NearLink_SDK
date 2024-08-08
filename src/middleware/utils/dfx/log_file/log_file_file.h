/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: offline log file saved to the storage
 */

#ifndef LOG_FILE_FILE_H
#define LOG_FILE_FILE_H

#include <stdint.h>
#include "errcode.h"

errcode_t logfile_create_path(store_file_cfg_t *cfg);
errcode_t logfile_prepare_file_fd(store_file_info_t *file_info, store_file_cfg_t *cfg);
errcode_t logfile_remove_files(store_service_t service_type, store_file_cfg_t *cfg);

errcode_t logfile_single_file_write(store_file_info_t *file_info, uint8_t *data, uint32_t data_len);
errcode_t logfile_multi_file_write(store_file_info_t *file_info, uint8_t *data, uint32_t data_len);

errcode_t logfile_write_cache_to_file(store_file_info_t *file_info);

#endif /* LOG_FILE_FILE_H */
