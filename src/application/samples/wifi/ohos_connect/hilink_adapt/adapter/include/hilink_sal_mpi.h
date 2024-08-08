/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 多精度整数（Multi-precision integer）配层接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_SAL_MPI_H
#define HILINK_SAL_MPI_H

#include <stdint.h>
#include "hilink_sal_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 标识多精度整数MPI */
typedef void* HiLinkMpi;

/**
 * @brief 大数初始化
 *
 * @return 非NULL大数指针，NULL失败
 * @attention 返回的大数使用完毕调用HILINK_SAL_MpiFree释放
 */
HiLinkMpi HILINK_SAL_MpiInit(void);

/**
 * @brief 释放大数资源
 *
 * @param mpi [IN] 已初始化的MPI
 */
void HILINK_SAL_MpiFree(HiLinkMpi mpi);

/**
 * @brief 模指数运算 x = a^e mod n
 *
 * @param x [OUT] 运算结果，应为已初始化的MPI
 * @param a [IN] 待求幂的数，应为已初始化的MPI
 * @param e [IN] 指数，应为已初始化的MPI
 * @param n [IN] 模数，应为已初始化的MPI
 * @return 0表示成功，非0失败
 */
int HILINK_SAL_MpiExpMod(HiLinkMpi x, HiLinkMpi a, HiLinkMpi e, HiLinkMpi n);

/**
 * @brief MPI与整数比较
 *
 * @param x [IN] 待比较的MPI，应为已初始化的MPI
 * @param z [IN] 待比较的整数
 * @retval 0 x与z相等
 * @retval 1 x大于z
 * @retval -1 x小于z
 * @retval 其他 失败
 */
int HILINK_SAL_MpiCmpInt(HiLinkMpi x, int64_t z);

/**
 * @brief MPI与整数相减 x = a - b
 *
 * @param x [OUT] 运算结果，应为已初始化的MPI
 * @param a [IN] 被减数
 * @param b [IN] 减数
 * @return 0成功，非0失败
 */
int HILINK_SAL_MpiSubInt(HiLinkMpi x, HiLinkMpi a, int64_t b);

/**
 * @brief 两个MPI比较大小
 *
 * @param x [IN] 待比较的MPI，应为已初始化的MPI
 * @param y [IN] 待比较的MPI，应为已初始化的MPI
 * @retval 0 x与Y相等
 * @retval 1 x大于Y
 * @retval -1 x小于Y
 * @retval 其他 失败
 */
int HILINK_SAL_MpiCmpMpi(HiLinkMpi x, HiLinkMpi y);

/**
 * @brief 从字符串读导入MPI
 *
 * @param mpi [OUT] 导入结果，应为已初始化的MPI
 * @param radix [IN] 进制，范围应为[2,16]
 * @param s [IN] 待读取的字符串
 * @return 0成功，非0失败
 */
int HILINK_SAL_MpiReadString(HiLinkMpi mpi, unsigned char radix, const char *s);

/**
 * @brief 从MPI导出字符串
 *
 * @param mpi [IN] 待导出字符串的MPI
 * @param radix [IN] 进制，范围应为[2,16]
 * @param buf [OUT] 输出缓冲区，有效可写长度至少为*bufLen
 * @param bufLen [IN,OUT] 输入为输出缓冲区长度，输出为包括\0的字符串长度
 * @return 0成功，非0失败
 */
int HILINK_SAL_MpiWriteString(HiLinkMpi mpi, unsigned int radix, char *buf, unsigned int *bufLen);

/**
 * @brief 从无符号大端二进制数据导入MPI
 *
 * @param mpi [OUT] 导入结果，应为已初始化的MPI
 * @param buf [IN] 二进制数据缓冲区，有效可读长度至少为bufLen
 * @param bufLen [IN] 二进制数据长度
 * @return 0成功，非0失败
 */
int HILINK_SAL_MpiReadBinary(HiLinkMpi mpi, const unsigned char *buf, unsigned int bufLen);

/**
 * @brief 从MPI导出无符号大端二进制数据
 *
 * @param mpi [IN] 待导出二进制数据的MPI
 * @param buf [OUT] 二进制数据缓冲区，有效可写长度至少为bufLen
 * @param bufLen [IN] 缓冲区长度
 * @return 0成功，非0失败
 */
int HILINK_SAL_MpiWriteBinary(HiLinkMpi mpi, unsigned char *buf, unsigned int bufLen);

#ifdef __cplusplus
}
#endif

#endif /* HILINK_SAL_MPI_H */
