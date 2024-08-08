/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Mem Pool Driver Header. \n
 *
 * History: \n
 * 2018-10-15， Create file. \n
 */
#ifndef CHIP_MEM_H
#define CHIP_MEM_H

#include <stdio.h>
#include "securec.h"
#include "non_os.h"
#include "panic.h"
#include "debug_print.h"

#define CONN_DOG_TAG_SIZE 4
#define CONN_DOG_TAG      0xdeadbeaf

#define conn_irq_disable non_os_enter_critical
#define conn_irq_enable  non_os_exit_critical
#define CONN_IO_PRINT    PRINT

#define conn_likely(_expr)   (_expr)
#define conn_unlikely(_expr) (_expr)
#define conn_bug_on(cond)    \
    do {                                                     \
        if (cond) {                                          \
            panic(PANIC_IRMALLOC_FAIL, (uintptr_t)__LINE__); \
        }                                                    \
    } while (0)
#define CONN_MEM_INFO_SIZE 4  /* sizeof(ptr) */

#define WLAN_MEM_MAX_SUBPOOL_NUM 32  /* Maximum memory sub-pool size supported by each memory pool */

#define conn_get_4byte_align_value(_ul_size) (((_ul_size) + 0x03) & (~0x03))
#define conn_array_size(_ast_array)    (sizeof(_ast_array) / sizeof((_ast_array)[0]))

#define _PRE_MEM_DEBUG_MODE

typedef enum {
    CONN_MEM_STATE_FREE = 0,
    CONN_MEM_STATE_ALLOC,
    CONN_MEM_STATE_BUTT,
} conn_mem_state_enum;

typedef enum {
    CONN_MEM_POOL_ID_LOCAL = 0,
    CONN_MEM_POOL_ID_BUTT,
} conn_mem_pool_id_enum;

typedef struct {
    unsigned int us_len;
    unsigned int us_free_cnt;
    void **ppst_free_stack;
    unsigned int us_total_cnt;
    unsigned char auc_resv[2];    // Reserve 2 bites for four-byte alignment
} conn_mem_subpool_stru;

typedef struct {
    unsigned int us_size;
    unsigned int us_cnt;
} conn_mem_subpool_cfg_stru;

typedef struct {
    unsigned char en_pool_id;
    unsigned char uc_subpool_cnt;
    unsigned char auc_resv[2];     // Reserve 2 bites for four-byte alignment
    conn_mem_subpool_cfg_stru *pst_subpool_cfg_info;
} conn_mem_pool_cfg_stru;

struct conn_mem_stru_tag {
    unsigned char *puc_data;
    unsigned char *puc_origin_data;
    unsigned int us_len;
    unsigned char uc_user_cnt : 4;
    unsigned char en_mem_state_flag : 4;
    unsigned char en_pool_id : 4;
    unsigned char uc_subpool_id : 4;

#ifdef _PRE_MEM_DEBUG_MODE
    unsigned long ul_return_addr;
#endif
};
typedef struct conn_mem_stru_tag conn_mem_stru;

typedef struct {
    unsigned int us_max_byte_len;
    unsigned char uc_subpool_cnt;
    unsigned char uc_resv;
    conn_mem_subpool_stru ast_subpool_table[WLAN_MEM_MAX_SUBPOOL_NUM];
    unsigned int us_mem_used_cnt;
    unsigned int us_mem_total_cnt;
    conn_mem_stru *pst_mem_start_addr;
} conn_mem_pool_stru;

/**
 * @brief  Prototype    : conn_mem_init_pool
 * Description  : mem pool init function, only call once
 * Input        : void* start  the pool start base address, start must aligned to 4 bytes
 *                unsigned int size  pool total size, aligned to 4 bytes
 * Return Value : 0 means succ, others failed
 */
int conn_mem_init_pool(unsigned char *start, unsigned int size);

void conn_mem_print_pool_info(void);

/**
 * @brief  Prototype    : conn_mem_alloc_enhanced
 * Description  : mem alloc function,
 * Input        : unsigned long return_addr call function's address for debug
 *                unsigned int  us_len   request mem length
 * Return Value : void*   NULL means failed, others succ, alloc mem aligned to 4 bytes
 */
void *conn_mem_alloc_enhanced(unsigned long return_addr,
                              unsigned int us_len);

/**
 * @brief  Prototype    : conn_mem_free_enhanced
 * Description  : mem free function, can't free the mem repeat,
 * Input        : void* free_mem , free the alloc mem, if zero, do nothing
 * Return Value : void
 */
void conn_mem_free_enhanced(const void *free_mem);

/**
 * @brief  Prototype    : conn_mem_realloc_enhanced
 * Description  : mem realloc function
 * Input        : unsigned long return_addr call function's address for debug
 *               void* mem    the old mem
 *                unsigned int  us_len   request mem length
 * Return Value : void*   NULL means failed, others succ, alloc mem aligned to 4 bytes
 */
void *conn_mem_realloc_enhanced(unsigned long return_addr,
                                const void *mem,
                                unsigned int us_len);

#endif

