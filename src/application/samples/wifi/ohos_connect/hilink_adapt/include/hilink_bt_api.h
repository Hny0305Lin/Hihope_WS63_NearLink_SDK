/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 蓝牙SDK API头文件（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_BT_API_H
#define HILINK_BT_API_H

#include "ble_cfg_net_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 销毁蓝牙协议栈标志 */
#define BLE_DEINIT_FALG 0x1234ABCD

/* 产品信息定义结构体 */
typedef struct {
    char *manuName;       /* 设备制造商，长度范围（0,4] */
    char *devName;        /* 设备名称 */
    char *productId;      /* 设备HiLink认证号，长度范围（0,5] */
    char *sn;             /* 设备唯一标识，比如sn号，长度范围（0,40] */
    char *mac;            /* 设备MAC地址，固定32字节 */
    char *subProductId;   /* 设备子productId */
    char *model;          /* 设备型号，长度范围（0,32] */
    char *devType;        /* 设备类型，长度范围（0,4] */
    char *hiv;            /* 设备Hilink协议版本，长度范围（0,32] */
    int protType;         /* 设备协议类型，取值范围[1,3] */
} HILINK_BT_DevInfo;

/* 发送蓝牙数据回调函数类型 */
typedef int (*HILINK_BT_SendBtDataCallback)(const unsigned char *buf, unsigned int len);

/* 厂家实现协议栈使用，注册蓝牙数据发送接口 */
int HILINK_BT_RegisterBtDataSendCallback(HILINK_BT_SendBtDataCallback callback);

/* 厂家实现协议栈使用，注册蓝牙OTA数据发送接口 */
int HILINK_BT_RegisterOtaDataSendCallback(HILINK_BT_SendBtDataCallback callback);

/* 厂家实现协议栈使用，处理蓝牙数据 */
int HILINK_BT_ProcessBtData(const unsigned char *buf, unsigned int len);

/* 厂家实现协议栈使用，处理蓝牙OTA的控制数据 */
int HILINK_BT_ProcessOtaCtrl(const unsigned char *buf, unsigned int len);

/* 厂家实现协议栈使用，处理蓝牙OTA的传输数据 */
int HILINK_BT_ProcessOtaData(const unsigned char *buf, unsigned int len);

/* 厂家实现协议栈使用，断开蓝牙连接时，释放通道占用资源 */
void HILINK_BT_DisconnectFreeResource(void);

/* 返回值0代表支持本地控和二次配网，其他值代表不支持 */
int HILINK_BT_SupportLocalControl(void);

/* 设备硬重置时厂商调用的接口 */
int HILINK_BT_HardRevoke(void);

/*
 * 获取设备表面的最强点信号发射功率强度，最强点位置的确定以及功率测试方
 * 法，参照hilink认证蓝牙靠近发现功率设置及测试方法指导文档，power为出参
 * ，单位dbm，返回设备表面的最强信号强度值，如果厂商不想使用蓝牙靠近发现功
 * 能，接口直接返-1，如果需要使用蓝牙靠近发现，则接口返回0，如需及时生效，需
 * 调用HILINK_BT_StartAdvertise()方法启动广播
 */
int HILINK_BT_GetDevSurfacePower(char *power);

/**
 * @brief 注册服务白名单，注册在白名单中的服务支持不加密处理
 *
 * @param services [IN] 字符串列表，成员可读数量至少为size，应为静态或全局变量指针
 * @param size [IN] 字符串列表大小
 * @return 0成功，其他失败
 * @attention 在ble sdk初始化之前调用
 */
int HILINK_BT_RegisterReqSvcTrustList(const char *services[], unsigned int size);

/*
 * 获取设备的子型号，长度固定两个字节
 * subProdId为保存子型号的缓冲区，len为缓冲区的长度
 * 如果产品定义有子型号，则填入两字节子型号，并以'\0'结束, 返回0
 * 没有定义子型号，则返回-1
 * 该接口需设备开发者实现
 * 与hilink sdk相同定义，双模组模式只需一份，已提供给第三方厂家，暂不按编程规范整改
 */
int HILINK_GetSubProdId(char *subProdId, int len);

/*
 * 获取设备SN
 * 与hilink sdk相同定义，双模组模式只需一份，已提供给第三方厂家，暂不按编程规范整改
 */
void HILINK_GetDeviceSn(unsigned int len, char *sn);

/*
 * 获取设备相关版本号
 * 返回0表示版本号获取成功，返回其他表示版本号获取失败
 * 注意，此接口为HiLink内部调用函数
 * 与hilink sdk相同定义，双模组模式只需一份，已提供给第三方厂家，暂不按编程规范整改
 */
int getDeviceVersion(char **firmwareVer, char **softwareVer, char **hardwareVer);

/*
 * 蓝牙处理任务休眠,当前仅harmony版本支持
 */
void HILINK_BT_SuspendScheduleTask(void);

/*
 * 蓝牙处理任务唤醒,当前仅harmony版本支持
 */
void HILINK_BT_ResumeScheduleTask(void);

/*
 * 获取蓝牙mac地址
 * 返回0表示获取成功，返回其他表示获取失败
 */
int HILINK_BT_GetMacAddr(unsigned char *mac, unsigned int len);

/*
 * 获取厂商定制信息
 * 返回0表示获取成功，返回其他表示获取失败
 */
int HILINK_GetCustomInfo(char *customInfo, unsigned int len);

/*
 * 获取厂家ID
 * 返回0表示获取成功，返回其他表示获取失败
 */
int HILINK_GetManuId(char *manuId, unsigned int len);

#ifdef __cplusplus
}
#endif
#endif
