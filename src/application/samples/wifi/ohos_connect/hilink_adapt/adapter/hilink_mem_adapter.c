/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 系统适配层内存接口实现源文件（此文件为DEMO，需集成方适配修改）
 */
#include "hilink_mem_adapter.h"
#include <string.h>
#include <stdlib.h>

void *HILINK_Malloc(unsigned int size)
{
    if (size == 0) {
        return NULL;
    }

    return malloc(size);
}

void HILINK_Free(void *pt)
{
    if (pt == NULL) {
        return;
    }
    free(pt);
}

int HILINK_Memcmp(const void *buf1, const void *buf2, unsigned int len)
{
    if ((buf1 == NULL) && (buf2 == NULL)) {
        return 0;
    }
    if ((buf1 != NULL) && (buf2 == NULL)) {
        return 1;
    }
    if ((buf1 == NULL) && (buf2 != NULL)) {
        return -1;
    }

    return memcmp(buf1, buf2, len);
}