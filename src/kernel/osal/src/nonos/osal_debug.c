/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: OS Abstract Layer.
 */

static unsigned char g_bug_on_flag = 0;
void osal_bug_on(unsigned char condition)
{
    if (condition != 0 && g_bug_on_flag == 0) {
        g_bug_on_flag = 1;
        *(volatile unsigned long*)0 = 0; // Invoke a system instruction to trigger an exception.
    }
}
