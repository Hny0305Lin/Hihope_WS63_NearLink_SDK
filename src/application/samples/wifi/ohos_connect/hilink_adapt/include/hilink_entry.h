
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HILINK SDK配网头文件
 */

#ifndef HILINK_ENTRY_H
#define HILINK_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
int hilink_indie_upgrade_main(void);
#endif

int hilink_wifi_main(void);
int hilink_ble_main(void);

#ifdef __cplusplus
}
#endif
#endif
