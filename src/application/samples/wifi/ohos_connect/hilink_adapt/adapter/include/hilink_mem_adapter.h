/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 系统适配层内存接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_MEM_ADAPTER_H
#define HILINK_MEM_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 描述: 申请内存空间
 * 参数: size，表示申请内存空间大小
 * 返回: 申请内存空间指针
 */
void *HILINK_Malloc(unsigned int size);

/*
 * 描述: 释放内存空间
 * 参数: pt，表示释放内存空间指针
 */
void HILINK_Free(void *pt);

/*
 * 描述: 内存比较
 * 参数: buf1，指向内存块的指针
 *       buf2，指向内存块的指针
 *       len，要比较的字节数
 * 返回: 0表示buf1和buf2指向的内存中内容相同, 大于0表示buf1大于buf2，小于0表示buf1小于buf2
 */
int HILINK_Memcmp(const void *buf1, const void *buf2, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* HILINK_MEM_ADAPTER_H */
