/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG alloc memory functions header file
 */

#ifndef UPG_ALLOC_H
#define UPG_ALLOC_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 * 内存申请接口
 * size：申请内存的大小
 */
void *upg_malloc(uint32_t size);

/*
 * 内存释放接口
 * addr：待释放的内存地址
 */
void upg_free(void *addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_ALLOC_H */