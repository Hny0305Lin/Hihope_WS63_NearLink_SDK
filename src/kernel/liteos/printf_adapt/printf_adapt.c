/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: ohos printf adapt file.
 */
#include "los_printf_pri.h"

int printf(const char *restrict fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    UartVprintf(fmt, ap);
    va_end(ap);
    return 0;
}