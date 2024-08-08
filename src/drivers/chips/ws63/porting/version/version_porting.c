/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides version port source. \n
 *
 * History: \n
 * 2023-11-08， Create file. \n
 */

#include "version_porting.h"

char *plat_get_sw_version_str(void)
{
    return SDK_VERSION;
}

void print_version(void)
{
    osal_printk("SDK Version:%s\r\n", SDK_VERSION);
}
