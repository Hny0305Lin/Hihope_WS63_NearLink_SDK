/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink独立升级入口
 */
#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
#include <stdio.h>
#include "hilink.h"
#include "app_function_mapping.h"

int hilink_indie_upgrade_main(void)
{
    hilink_func_map_init();

    /* 修改任务属性 */
    HILINK_SdkAttr *sdkAttr = HILINK_GetSdkAttr();
    if (sdkAttr == NULL) {
        printf("sdkAttr is null");
    }
    sdkAttr->deviceMainTaskStackSize = 0x4000;
    sdkAttr->otaCheckTaskStackSize = 0x3250;
    sdkAttr->otaUpdateTaskStackSize = 0x3250;
    HILINK_SetSdkAttr(*sdkAttr);

    return 0;
}
#endif