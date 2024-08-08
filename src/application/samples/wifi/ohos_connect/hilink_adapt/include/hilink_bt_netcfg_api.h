/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 蓝牙SDK配网API头文件（此文件为DEMO，需集成方适配修改）
 */

#ifndef HILINK_BT_NETCFG_API_H
#define HILINK_BT_NETCFG_API_H

#ifdef __cplusplus
extern "C" {
#endif

/* 启动配网 */
int HILINK_BT_StartNetCfg(void);

/* 停止配网 */
int HILINK_BT_StopNetCfg(void);

#ifdef __cplusplus
}
#endif
#endif

