/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of the character string interface at the system adaptation layer. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"

unsigned int HILINK_Strlen(const char *src)
{
    return app_call1(APP_CALL_HILINK_STRLEN, unsigned int, const char *, src);
}

char *HILINK_Strchr(const char *str, int ch)
{
    return app_call2(APP_CALL_HILINK_STRCHR, char *, const char *, str, int, ch);
}

char *HILINK_Strrchr(const char *str, int ch)
{
    return app_call2(APP_CALL_HILINK_STRRCHR, char *, const char *, str, int, ch);
}

int HILINK_Atoi(const char *str)
{
    return app_call1(APP_CALL_HILINK_ATOI, int, const char *, str);
}

char *HILINK_Strstr(const char *str1, const char *str2)
{
    return app_call2(APP_CALL_HILINK_STRSTR, char *, const char *, str1, const char *, str2);
}

int HILINK_Strcmp(const char *str1, const char *str2)
{
    return app_call2(APP_CALL_HILINK_STRCMP, int, const char *, str1, const char *, str2);
}

int HILINK_Strncmp(const char *str1, const char *str2, unsigned int len)
{
    return app_call3(APP_CALL_HILINK_STRNCMP, int, const char *, str1, const char *, str2, unsigned int, len);
}