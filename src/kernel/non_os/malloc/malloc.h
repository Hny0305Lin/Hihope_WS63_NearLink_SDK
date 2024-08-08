/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Malloc function Header. \n
 *
 * History: \n
 * 2023-2-14， Create file. \n
 */

#ifndef __MALLOC_ROM_H__
#define __MALLOC_ROM_H__

#include <stdint.h>
#include <stddef.h>
#include "common_def.h"

typedef struct malloc_dfx {
    uint32_t heap_malloc_size;    /* 已经申请的堆空间总大小 */
    uint32_t heap_free_size;      /* 剩余堆空间总大小 */
    uint32_t heap_size;           /* 堆空间总大小 */
    uintptr_t heap_start_addr;  /* 堆空间起始地址 */
    uintptr_t heap_end_addr;    /* 堆空间结束地址 */
    uint32_t start_status;        /* 启动状态维测，结合set_start_status函数使用 */
} malloc_dfx_t;

typedef void (*free_func)(void *addr);
typedef void *(*malloc_func)(uint32_t size, uint32_t caller);
typedef void (*malloc_init_func)(uint32_t heap_start_addr, uint32_t heap_end_addr);

typedef struct {
    malloc_init_func init;   /* 初始化堆内存管理接口 */
    malloc_func malloc; /* 申请堆内存接口 */
    free_func free;     /* 释放堆内存接口 */
} malloc_funcs;

malloc_funcs *malloc_get_funcs(void);
uint32_t malloc_register_funcs(const malloc_funcs *funcs);
/**
* @ingroup
* @brief  动态内存管理模块初始化接口,需要调用uapi_register_malloc接口先注册后才能使用。
*
* @par 描述:
*          使用动态内存前，需要调用1次本接口进行初始化
* @attention   无。
*
* @param  heap_start_addr [IN] 类型 #uintptr_t  堆管理起始地址。
* @param  heap_end_addr [IN] 类型 #uintptr_t  堆管理结束地址。
* @param  check_sum [IN] 类型 #uint32_t 校验值，前两个参数的异或结果。
*
* @retval 无
*
* <ul><li>malloc.h: 该接口声明所在的头文件.</li></ul>
* @see uapi_register_malloc
 */
void malloc_init(uintptr_t heap_start_addr, uintptr_t heap_end_addr);

/**
* @ingroup
* @brief  申请动态内存，需要调用uapi_register_malloc接口先注册后才能使用。
*
* @par 描述:
*        申请动态内存
* @attention   无。
*
* @param  size [IN] 类型 #uint32_t 需要申请的内存大小
*
* @retval void*  申请到的内存指针，如果申请失败则返回HI_NULL
*
* <ul><li>malloc.h: 该接口声明所在的头文件.</li></ul>
* @see uapi_register_malloc
 */
void *malloc(size_t size);

/**
* @ingroup
* @brief  释放动态内存，需要调用uapi_register_malloc接口先注册后才能使用。
*
* @par 描述:
*       释放动态内存
* @attention   无。
*
* @param  addr [IN] 类型 #void*  需要释放的内存地址指针。注意不要多次释放同一个内存地址。
*
* @retval EXT_ERR_SUCCESS      释放内存成功。
* @retval EXT_ERR_FAILURE     释放内存错误。
*
* <ul><li>malloc.h: 该接口声明所在的头文件.</li></ul>
* @see uapi_register_malloc
 */
void free(void *addr);

void rom_malloc_init(uint32_t heap_start_addr, uint32_t heap_end_addr);
void *rom_malloc(uint32_t size, uint32_t caller);
uint32_t  rom_free(void *addr);
void print_mem_info(void);
#endif
