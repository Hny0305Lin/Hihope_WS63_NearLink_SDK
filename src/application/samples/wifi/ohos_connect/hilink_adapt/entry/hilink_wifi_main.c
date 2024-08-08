/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink SDK提供的demo示例代码（此文件为DEMO，需集成方适配修改）
 */
#include <stdio.h>
#include <stddef.h>
#include "hilink.h"
#include "hilink_device.h"
#include "cmsis_os2.h"
#include "hilink_socket_adapter.h"

static int GetAcV2Func(unsigned char *acKey, unsigned int acLen)
{
    /* key文件在开发者平台获取 */
    return -1;
}

int hilink_wifi_main(void)
{
    /* 注册ACKey函数 */
    HILINK_RegisterGetAcV2Func(GetAcV2Func);

    /* 设置配网方式 */
    HILINK_SetNetConfigMode(HILINK_NETCONFIG_WIFI);

#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
    if (HILINK_RegisterErrnoCallback(get_os_errno) != 0) {
        printf("reg errno cb err\r\n");
    }
#endif

    /* 修改任务属性 */
    HILINK_SdkAttr *sdkAttr = HILINK_GetSdkAttr();
    if (sdkAttr == NULL) {
        printf("sdkAttr is null");
    }
    sdkAttr->monitorTaskStackSize = 0x600;
    HILINK_SetSdkAttr(*sdkAttr);

    /* 启动Hilink SDK */
    if (HILINK_Main() != 0) {
        printf("HILINK_Main start error");
    }

    return 0;
}
