/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2022. All rights reserved.
 * Description: osal adapt string source file.
 */
#include "osal_adapt.h"

int osal_adapt_strncmp(const char *str1, const char *str2, unsigned long size)
{
    return osal_strncmp(str1, str2, size);
}