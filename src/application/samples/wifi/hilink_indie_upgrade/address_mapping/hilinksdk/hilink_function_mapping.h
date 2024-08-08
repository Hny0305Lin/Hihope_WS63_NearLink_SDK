/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: hilink function mapping. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#ifndef HILINK_FUNCTION_MAPPING_H
#define HILINK_FUNCTION_MAPPING_H

#ifdef __cplusplus
extern "C" {
#endif

int *get_app_tbl(void);

extern unsigned int __bss_begin__;
extern unsigned int __bss_end__;
extern unsigned int __data_begin__;
extern unsigned int __data_load__;
extern unsigned int __data_size__;

extern unsigned int __sram_text_begin__;
extern unsigned int __sram_text_load__;
extern unsigned int __sram_text_size__;

#ifdef __cplusplus
}
#endif
#endif
