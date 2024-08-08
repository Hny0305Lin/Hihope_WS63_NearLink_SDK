/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: get init info function
 */
#ifndef DIAG_CMD_LOGFLASH_TEST_H
#define DIAG_CMD_LOGFLASH_TEST_H

#include "diag.h"

typedef struct {
    uint8_t multi_num;      // 文件的数量
    uint32_t file_size;     // 文件的大小
    uint8_t enable_cache;   // 是否支持缓存
    uint16_t cache_size;    // 缓存的大小
} dfx_test_logfile_cfg_t;

typedef struct {
    uint32_t ctrl_type;
    uint32_t service_type;
} dfx_test_logfile_ctrl_t;

typedef struct {
    uint32_t service_type;
    uint32_t data_type;
    uint32_t data_len;
    uint32_t test_timer;
} dfx_test_logfile_write_t;

errcode_t diag_logfile_init(void);
errcode_t diag_cmd_logflash_test(uint16_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size, diag_option_t *option);

#endif /* DIAG_CMD_LOGFLASH_TEST_H */