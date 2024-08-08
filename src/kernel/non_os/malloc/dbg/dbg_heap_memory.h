/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: dgb heap memory header file\n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */
#ifndef __DBG_HEAP_MEMORY_H__
#define __DBG_HEAP_MEMORY_H__
#ifdef _DEBUG_HEAP_MEM_MGR

#include "dbg_hash_tab.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

uint32_t heap_mem_tab_init(void);
uint32_t heap_mem_dump(void);
void *heap_mem_malloc(uint32_t size, uint32_t master);
void heap_mem_free(void *ptr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
#endif