/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink SDK 配网的入口函数
 */

#include "cmsis_os2.h"
#include "nv.h"
#include "hilink_sal_defines.h"
#include "efuse_porting.h"
#include "wifi_device_config.h"
#include "hilink_entry.h"

int hilink_entry(void *param)
{
    param = param;
#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
    hilink_indie_upgrade_main();
#endif

    uint8_t hilink_entry_mode = 0;
    uint16_t hilink_entry_mode_len = 0;
    uint32_t ret = ERRCODE_FAIL;
    ret = uapi_nv_read(NV_ID_HILINK_ENTRY_MODE, sizeof(hilink_entry_mode), &hilink_entry_mode_len,
        &hilink_entry_mode);
    if (ret != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("read hilink entry mode from nv failed\r\n");
        return -1; /* -1:读取配网模式失败 */
    }

    (void)osDelay(500); /* 500: 延时5s, 等待wifi或者ble初始化完毕 */
    if (hilink_entry_mode == 0) { /* 0: wifi配网模式 */
        hilink_wifi_main();
    } else if (hilink_entry_mode == 1) {
        hilink_ble_main();
    }

    return 0;
}