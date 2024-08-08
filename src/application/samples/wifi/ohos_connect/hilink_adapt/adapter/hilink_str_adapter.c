/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 系统适配层字符串接口实现（此文件为DEMO，需集成方适配修改）
 */
#include "hilink_str_adapter.h"
#include <stdlib.h>
#include <string.h>

unsigned int HILINK_Strlen(const char *src)
{
    if (src == NULL) {
        return 0;
    }
    return (unsigned int)strlen(src);
}

char *HILINK_Strchr(const char *str, int ch)
{
    if (str == NULL) {
        return NULL;
    }

    return strchr(str, ch);
}

char *HILINK_Strrchr(const char *str, int ch)
{
    if (str == NULL) {
        return NULL;
    }

    return strrchr(str, ch);
}

int HILINK_Atoi(const char *str)
{
    if (str == NULL) {
        return 0;
    }
    return atoi(str);
}

char *HILINK_Strstr(const char *str1, const char *str2)
{
    if ((str1 == NULL) || (str2 == NULL)) {
        return NULL;
    }
    return strstr(str1, str2);
}

int HILINK_Strcmp(const char *str1, const char *str2)
{
    if ((str1 == NULL) && (str2 == NULL)) {
        return 0;
    }
    if ((str1 != NULL) && (str2 == NULL)) {
        return 1;
    }
    if ((str1 == NULL) && (str2 != NULL)) {
        return -1;
    }

    return strcmp(str1, str2);
}

int HILINK_Strncmp(const char *str1, const char *str2, unsigned int len)
{
    if ((str1 == NULL) && (str2 == NULL)) {
        return 0;
    }
    if ((str1 != NULL) && (str2 == NULL)) {
        return 1;
    }
    if ((str1 == NULL) && (str2 != NULL)) {
        return -1;
    }

    return strncmp(str1, str2, len);
}