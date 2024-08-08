/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of the memory interface at the system adaptation layer. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"

void *HILINK_Malloc(unsigned int size)
{
    return app_call1(APP_CALL_HILINK_MALLOC, void *, unsigned int, size);
}

void HILINK_Free(void *pt)
{
    app_call1(APP_CALL_HILINK_FREE, void, void *, pt);
}

int HILINK_Memcmp(const void *buf1, const void *buf2, unsigned int len)
{
    return app_call3(APP_CALL_HILINK_MEMCMP, int, const void *, buf1, const void *, buf2, unsigned int, len);
}