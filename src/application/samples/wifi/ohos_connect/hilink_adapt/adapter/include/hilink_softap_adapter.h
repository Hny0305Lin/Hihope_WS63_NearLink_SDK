/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: SoftAP适配层接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_SOFTAP_ADAPTER_H
#define HILINK_SOFTAP_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 描述: 将网卡切为AP模式并开启SoftAP热点
 * 参数: ssid，用于创建SoftAP的ssid
 *       ssidLen，ssid长度，最大为64
 * 返回: 0表示成功，其他表示失败
 */
int HILINK_StartSoftAp(const char *ssid, unsigned int ssidLen);

/*
 * 描述: 关闭SoftAP热点并将网卡切回station模式
 * 返回: 0表示成功，其他表示失败
 */
int HILINK_StopSoftAp(void);

#ifdef __cplusplus
}
#endif
#endif /* HILINK_SOFTAP_ADAPTER_H */
