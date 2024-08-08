/**
 * @file cstl_public_inner.h
 * @copyright Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief cstl_public_cstl公共模块定义
 * @details cstl公共定义实现源码
 * @date 2021-05-14
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version   <th>Author    <th>Description
 * </table>
 * *******************************************************************************************
 */

#ifndef CSTL_PUBLIC_INNER_H
#define CSTL_PUBLIC_INNER_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CSTL_DEBUG
    #define CSTL_STATIC
    #ifndef CSTL_PRINTF
        #define CSTL_PRINTF printf
    #endif
#else
    #define CSTL_STATIC static
    #ifndef CSTL_PRINTF
        #define CSTL_PRINTF(format, ...)
    #endif
#endif

/* 后根遍历删除avl和rawavl时，栈容量的极值 */
#define CSTL_AVL_STACK_SIZE 64U

/* -0xC 内存不足。 */
#define CSTL_ERRNO_NOMEM (-12)

/* -0x16 入参无效。 */
#define CSTL_ERRNO_INVAL (-22)

typedef struct {
    uintptr_t inputData;   /* 用户实际输入的数据 */
    size_t dataSize;       /* 实际输入的大小 */
} CstlUserData;

/* 判断两数相乘在当前系统下会否溢出 */
bool IsMultiOverflow(size_t x, size_t y);

/* 判断两数相加在当前系统下会否溢出 */
bool IsAddOverflow(size_t x, size_t y);

/**
 * @details 默认比较函数
 */
int32_t CstlIntCmpFunc(uintptr_t data1, uintptr_t data2);

#ifndef SIZE_MAX
#define SIZE_MAX                       __SIZE_MAX__
#endif

#ifdef __cplusplus
}
#endif

#endif  /* CSTL_PUBLIC_INNER_H */

