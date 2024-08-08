/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 系统适配层标准输出接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_STDIO_ADAPTER_H
#define HILINK_STDIO_ADAPTER_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 描述: 将数据格式化打印到流
 * 参数: format，可包含格式化符号的待打印字符串
 *       ap, 可变参数，根据格式化符号插入字符串中
 * 返回: 0成功，其他失败
 */
int HILINK_Vprintf(const char *format, va_list ap);

/*
 * 描述: 将数据格式化打印到流
 * 参数: format，可包含格式化符号的待打印字符串
 *       ..., 可变参数列表，根据格式化符号插入字符串中
 * 返回: 0成功，其他失败
 */
int HILINK_Printf(const char *format, ...);

/*
 * 功能: 随机数生成
 * 参数: 1) input，出入参，随机数缓冲区
 *       2) len，随机数字节长度
 * 返回: 0获取成功 -1获取失败
 * 注意: 该接口不可用于安全用途
 */
int HILINK_Rand(unsigned char *input, unsigned int len);

/*
 * 功能: 真随机数生成
 * 参数: 1) input，出入参，随机数缓冲区
 *       2) len，随机数字节长度
 * 返回: 0获取成功 -1获取失败
 * 注意: 对于无安全随机数能力的平台，该接口直接返回-1
 *       该接口仅用作生成安全随机数熵源，HiLink SDK不直接使用
 */
int HILINK_Trng(unsigned char *input, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* HILINK_STDIO_ADAPTER_H */