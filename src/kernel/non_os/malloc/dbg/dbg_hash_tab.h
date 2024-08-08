/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: dbg hash tab header file\n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */
#ifndef __DBG_HASH_TAB_H__
#define __DBG_HASH_TAB_H__
#ifdef _DEBUG_HEAP_MEM_MGR

#include <stdint.h>
#include "osal_list.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HEAP_HASH_SIZE 0x40 // 64

typedef struct dbg_hash_data {
    void                  *key;
    uint32_t              length;
#ifdef __GNUC__
    uint32_t              master;
#endif
    struct osal_list_head list;
} dbg_hash_data;

typedef uint32_t (*hash_func)(const void *ptr, size_t tab_len);

typedef struct dbg_hash_tab {
    struct osal_list_head table[HEAP_HASH_SIZE];
    uint32_t              tab_len;
    struct osal_list_head free_head;
    hash_func             hash_key_func;
} dbg_hash_tab;

/*******************   func   *******************/
void hash_mem_add(dbg_hash_tab *hash_tab, void *ptr, uint32_t value, uint32_t master);
uint32_t hash_mem_delete(dbg_hash_tab *hash_tab, void *ptr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
#endif