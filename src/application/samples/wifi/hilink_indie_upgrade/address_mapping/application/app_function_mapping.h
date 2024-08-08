/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: hilink function mapping. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#ifndef APP_FUNCTION_MAPPING_H
#define APP_FUNCTION_MAPPING_H

#ifdef __cplusplus
extern "C" {
#endif

int *get_hilink_tbl(void);
void hilink_func_map_init(void);

extern char hilink_info_addr;

#ifdef __cplusplus
}
#endif
#endif
