/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Malloc function. \n
 *
 * History: \n
 * 2023-2-14， Create file. \n
 */

#include <stddef.h>
#include "securec.h"
#include "debug_print.h"
#include "errcode.h"
#include "malloc.h"

#ifdef _DEBUG_HEAP_MEM_MGR
#include "dbg_heap_memory.h"
#endif

malloc_dfx_t g_st_dfx = { 0 };
malloc_funcs g_malloc_func = { 0 };

/* malloc重新实现 */

typedef enum malloc_type  {
    EXT_NOTMALLOC = 0x7074,   /* empt */
    EXT_HAVEMALLOC = 0x6C6C,  /* full */
} malloc_type_t;

typedef uint16_t malloc_type_t_uint16;

/* 可用空间总大小 */
#define ALIGN4 0x4
#define DOUBLE 2

/* 数据格式: 大小、状态(是否已用)、可用空间 */
typedef struct malloc_state {
    uintptr_t caller;
    uint16_t size;
    malloc_type_t_uint16 is_used;
} malloc_state_t;

uint32_t malloc_register_funcs(const malloc_funcs *funcs)
{
    (void)memcpy_s((void*)(&g_malloc_func), sizeof(malloc_funcs), funcs, sizeof(malloc_funcs));
    return ERRCODE_SUCC;
}

malloc_funcs *malloc_get_funcs(void)
{
    return &g_malloc_func;
}

void malloc_init(uintptr_t heap_start_addr, uintptr_t heap_end_addr)
{
    rom_malloc_init(heap_start_addr, heap_end_addr);
}

void *malloc(size_t size)
{
    uint32_t caller = 0;
#ifdef __GNUC__
    caller = (uint32_t)(uintptr_t)__builtin_return_address(0);
#endif
    return rom_malloc(size, caller);
}

void free(void *addr)
{
    rom_free(addr);
}

/*
* malloc初始化,可用空间以4 字节对齐
* 注意:  本函数内不能增加打印，因其调用位置未进行串口初始化
*/
void rom_malloc_init(uint32_t heap_start_addr, uint32_t heap_end_addr)
{
    uint32_t real_start_addr = 0;
    malloc_dfx_t *st_dfx = NULL;

    malloc_state_t *mem_pos = NULL;

    memset_s((void*)(uintptr_t)heap_start_addr, sizeof(malloc_dfx_t), 0x0, sizeof(malloc_dfx_t));
    st_dfx = &g_st_dfx;
    real_start_addr = (uint32_t)(heap_start_addr + sizeof(malloc_dfx_t));

    /* 创建第一个内存池 */
    st_dfx->heap_start_addr = ((unsigned int)(real_start_addr + (ALIGN4 - 1)) & (~(ALIGN4 - 1)));
    st_dfx->heap_end_addr = (unsigned int)(heap_end_addr - DOUBLE * ALIGN4) & (~(ALIGN4 - 1));
    mem_pos = (malloc_state_t *)st_dfx->heap_start_addr;
    mem_pos->size = (uint16_t)(st_dfx->heap_end_addr - st_dfx->heap_start_addr - sizeof(malloc_state_t));
    st_dfx->heap_size = mem_pos->size;
    mem_pos->is_used = EXT_NOTMALLOC;
    mem_pos->caller = 0x0;
}

/*
* 刷新malloc 可分配空间，合并相邻已free 链表
*/
static void mem_fresh(void)
{
    malloc_state_t *mem_pos = NULL;
    malloc_state_t *next_pos = NULL;
    malloc_dfx_t *st_dfx = &g_st_dfx;

    /* 头结点 */
    mem_pos = (malloc_state_t *)st_dfx->heap_start_addr;
    while ((uintptr_t)((char *)mem_pos + sizeof(malloc_state_t) + mem_pos->size) < st_dfx->heap_end_addr) {
        /* 指向下一个区域 */
        next_pos = (malloc_state_t *)((char *)mem_pos + sizeof(malloc_state_t) +
                                              mem_pos->size);

        if ((mem_pos->is_used == EXT_NOTMALLOC) && (next_pos->is_used == EXT_NOTMALLOC)) {
            mem_pos->size += (uint16_t)(sizeof(malloc_state_t) + next_pos->size);
        } else {
            mem_pos = next_pos;
        }
    }
}

/*
* malloc 接口
*/
void *rom_malloc(uint32_t size, uint32_t caller)
{
    /* 首地址查询大于等于size且未用空间 */
    malloc_state_t *mem_pos = NULL;
    malloc_state_t *next_pos = NULL;
    malloc_dfx_t *st_dfx = &g_st_dfx;
    uint16_t align_size;
    uint32_t count = 0;

    if ((st_dfx->heap_start_addr == 0) || (size == 0)) {
        return NULL;
    }
    /* 头结点 */
    mem_pos = (malloc_state_t *)st_dfx->heap_start_addr;
    if ((mem_pos->is_used != EXT_NOTMALLOC) && (mem_pos->is_used != EXT_HAVEMALLOC)) {
        return NULL;
    }
    /* 分配空间4 字节对齐 */
    align_size = (size + (ALIGN4 - 1)) & (~(ALIGN4 - 1));

    for (;;) {
        if (mem_pos->is_used == EXT_NOTMALLOC) {
            /* 分配可用块的部分可用空间 */
            if ((sizeof(malloc_state_t) + align_size) < (mem_pos->size)) {
                /* 剩余空间自动成为未用块,须大于sizeof(malloc_state_t) */
                next_pos = (malloc_state_t *)((char *)mem_pos + align_size +
                                                      sizeof(malloc_state_t));

                next_pos->size = (uint16_t)(mem_pos->size - align_size - sizeof(malloc_state_t));
                next_pos->is_used = EXT_NOTMALLOC;

                mem_pos->size = align_size;
                mem_pos->is_used = EXT_HAVEMALLOC;
                mem_pos->caller = caller;

                st_dfx->heap_malloc_size += mem_pos->size;
                return (void *)((char *) mem_pos + sizeof(malloc_state_t));
            } else if ((align_size <= mem_pos->size) &&
                     ((sizeof(malloc_state_t) + align_size) >= (mem_pos->size))) {
                /* 分配整块可用块 */
                mem_pos->is_used = EXT_HAVEMALLOC;
                mem_pos->caller = caller;
                st_dfx->heap_malloc_size += mem_pos->size;
                return (void *)((char *) mem_pos + sizeof(malloc_state_t));
            }
        }

        /* 当前块不可用，查找下一块 */
        next_pos = (malloc_state_t *)((char *)mem_pos + sizeof(malloc_state_t) + mem_pos->size);
        mem_pos = next_pos;

        /* 查询至总空间结尾，失败则合并已free空间再次查询 */
        if ((uintptr_t)mem_pos >= st_dfx->heap_end_addr) {
            if (count >= 1) {
                return NULL;
            }

            mem_fresh();
            mem_pos = (malloc_state_t *)st_dfx->heap_start_addr;
            count++;
        }
        if ((mem_pos->is_used != EXT_HAVEMALLOC) && (mem_pos->is_used != EXT_NOTMALLOC)) {
            return NULL;
        }
    }
}

/*
* free 接口
*/
uint32_t rom_free(void *addr)
{
    /* 置链表状态为未用 */
    malloc_state_t *mem_pos = NULL;
    malloc_dfx_t *st_dfx = &g_st_dfx;

    if (addr == NULL) {
        return ERRCODE_SUCC;
    }

    mem_pos = (malloc_state_t *)((char *)addr - sizeof(malloc_state_t));
    if (mem_pos->is_used != EXT_HAVEMALLOC) {
        return ERRCODE_SUCC;
    }
    mem_pos->is_used = EXT_NOTMALLOC;
    st_dfx->heap_free_size += mem_pos->size;

    return ERRCODE_SUCC;
}

/*
* 测试代码
*/
void print_mem_info(void)
{
    malloc_dfx_t *st_dfx = &g_st_dfx;
    malloc_state_t *pos = (malloc_state_t *)st_dfx->heap_start_addr;

    PRINT("malloc start addr is 0x%x, end sddr is %d\r\n", st_dfx->heap_start_addr, st_dfx->heap_end_addr);
    if ((void *)st_dfx->heap_start_addr == NULL) {
        return;
    }
    PRINT("info:\r\n");
    while ((void *)pos < (void *)st_dfx->heap_end_addr) {
        /* 输出所有信息 */
        PRINT("addr:0x%x    ", pos);
#if ATE_DEBUG
        print_str("size:%d    ", pos->size);
        print_str("is_used:%x    ", pos->is_used);
        print_str("caller:0x%x\r\n", pos->caller);
#endif
        pos = (malloc_state_t *)((char *)pos + pos->size + sizeof(malloc_state_t));
    }
    return;
}
