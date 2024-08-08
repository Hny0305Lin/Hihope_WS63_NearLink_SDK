/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 蓝牙SDK 功能函数头文件（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_BT_FUNCTION_H
#define HILINK_BT_FUNCTION_H

#include "hilink_bt_api.h"
#include "ble_cfg_net_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 广播最大长度 */
#define ADV_VALUE_MAX_LEN 31
#define HIBEACON_IV_NONCE_LEN 8
#define HIBEACON_PSK_LEN 16

typedef enum {
    HILINK_BT_SDK_STATUS_SVC_RUNNING = 0, /* 正常运行 */
    HILINK_BT_SDK_STATUS_DEINIT, /* 注销 */
    HILINK_BT_SDK_STATUS_NAME_SET_ABNORM, /* 蓝牙名称设置异常 */
    HILINK_BT_SDK_STATUS_DISCOVER_MODE_SET_ABNORM, /* 蓝牙可发现模式设置异常 */
    HILINK_BT_SDK_STATUS_REG_APP_ABNORM, /* 注册BLE应用异常 */
    HILINK_BT_SDK_STATUS_SVC_CREATE_ABNORM, /* 服务创建异常 */
    HILINK_BT_SDK_STATUS_CHAR_ADD_ABNORM, /* 属性添加异常 */
    HILINK_BT_SDK_STATUS_DESC_ADD_ABNORM, /* 描述添加异常 */
    HILINK_BT_SDK_STATUS_SVC_START_ABNORM, /* 服务启动异常 */
    HILINK_BT_SDK_STATUS_ADV_PARA_SET_ABNORM, /* 广播参数设置异常 */
    HILINK_BT_SDK_STATUS_ADV_DATA_SET_ABNORM, /* 广播数据设置异常 */
    HILINK_BT_SDK_STATUS_ADV_START_ABNORM, /* 广播启动异常 */
} HILINK_BT_SdkStatus;

/* GATTS char属性取值 */
typedef enum {
    HILINK_BT_CHAR_PROP_WRITE_WITHOUT_RESP = 0x04,
    HILINK_BT_CHAR_PROP_WRITE = 0x08,
    HILINK_BT_CHAR_PROP_READ = 0x02,
    HILINK_BT_CHAR_PROP_NOTIFY = 0x10,
    HILINK_BT_CHAR_PROP_INDICATE = 0x20
} HILINK_BT_CharProperty;

/* GATTS char权限取值 */
typedef enum {
    HILINK_BT_CHAR_PERM_READ  = 0x01,
    HILINK_BT_CHAR_PERM_READ_ENCRYPTED = 0x02,
    HILINK_BT_CHAR_PERM_READ_ENCRYPTED_MITM = 0x04,
    HILINK_BT_CHAR_PERM_WRITE = 0x10,
    HILINK_BT_CHAR_PERM_WRITE_ENCRYPTED = 0x20,
    HILINK_BT_CHAR_PERM_WRITE_ENCRYPTED_MITM = 0x40,
    HILINK_BT_CHAR_PERM_WRITE_SIGNED = 0x80,
    HILINK_BT_CHAR_PERM_WRITE_SIGNED_MITM = 0x100,
} HILINK_BT_CharPermission;

/* GATTS desc属性取值 */
typedef enum {
    HILINK_BT_DESC_PERM_WRITE = 0x01,
    HILINK_BT_DESC_PERM_READ = 0x02
} HILINK_BT_DescPermission;

/* 属性值类型: 整型和属性 */
typedef enum {
    HILINK_BT_CMD_DATA_TYPE_INT,
    HILINK_BT_CMD_DATA_TYPE_STR,
} HILINK_BT_CmdDataType;

/* hilink蓝牙应用层数据编码类型 */
typedef enum {
    HILINK_BT_CMD_DATA_MODE_TLV = 0x00, /* TLV格式: 降低报文占用的空间 */
    HILINK_BT_CMD_DATA_MODE_JSON = 0x01 /* JSON格式: 扩展性更好，默认格式 */
} HILINK_BT_CmdDataMode;

/* 发送蓝牙SDK状态回调函数类型 */
typedef void (*HILINK_BT_SdkEventCallBack)(HILINK_BT_SdkStatus event, const void *param);

/* 自定义gatt服务读事件回调 */
typedef int (*HILINK_BT_GattReadCallback)(unsigned char *out, unsigned int *outLen);

/* 自定义gatt服务写事件回调 */
typedef int (*HILINK_BT_GattWriteCallback)(const unsigned char *in, unsigned int inLen);

/* 产品功能命令定义结构体 */
typedef struct {
    unsigned char attrIdx;
    char *attr;
    HILINK_BT_CmdDataType dataType;
    int (*putFunc)(const void *data, unsigned int len);
    int (*getFunc)(void *buf, unsigned int *bufLen, unsigned int len);
} HILINK_BT_AttrInfo;

/* 产品功能定义结构体 */
typedef struct {
    unsigned char svcIdx;
    char *service;
    int (*putFunc)(const void *svc, const unsigned char *in, unsigned int inLen,
        unsigned char **out, unsigned int *outLen);
    int (*getFunc)(const void *svc, const unsigned char *in, unsigned int inLen,
        unsigned char **out, unsigned int *outLen);
    unsigned char attrNum;
    HILINK_BT_AttrInfo *attrInfo;
} HILINK_BT_SvcInfo;

/* 产品Profile定义结构体 */
typedef struct {
    unsigned int svcNum;
    HILINK_BT_SvcInfo *svcInfo;
} HILINK_BT_Profile;

/* 蓝牙gatt character描述 */
typedef struct {
    char *descUuid;
    /* gatt属性描述读写权限：取值由HILINK_BT_DescPermission类型的成员或运算得出 */
    unsigned int descPermission;
} HILINK_BT_GattProfileDesc;

/* 蓝牙gatt character */
typedef struct {
    char *charUuid;
    /* gatt char权限:取值由HILINK_BT_CharPermission类型的成员或运算得出 */
    unsigned int charPermission;
    /* gatt char属性:取值由HILINK_BT_CharProperty类型的成员或运算得出 */
    unsigned int charProperty;
    HILINK_BT_GattReadCallback readFunc;
    HILINK_BT_GattWriteCallback writeFunc;
    HILINK_BT_GattProfileDesc *desc;
    unsigned char descNum;
} HILINK_BT_GattProfileChar;

/* 蓝牙gatt 服务 */
typedef struct {
    char *svcUuid;
    int isPrimary;
    HILINK_BT_GattProfileChar *character;
    unsigned char charNum;
} HILINK_BT_GattProfileSvc;

/* 厂商自定义蓝牙gatt服务列表 */
typedef struct {
    HILINK_BT_GattProfileSvc *service;
    unsigned char serviceNum;
} HILINK_BT_GattServiceList;

/* 配置保存回调结构体 */
typedef struct {
    int (*createItem)(const char *name, unsigned int size);
    int (*readItem)(const char *name, unsigned char *buf, unsigned int len);
    int (*writeItem)(const char *name, const unsigned char *buf, unsigned int len);
    int (*deleteItem)(const char *name);
    int (*destroyConfMgr)(void);
    int (*getHichainFlashAddr)(unsigned int *start, unsigned int *size);
} HILINK_BT_ConfigInterface;

/* 获取广播数据结构体 */
typedef struct {
    unsigned int advSvcDataLen;
    unsigned char advSvcData[ADV_VALUE_MAX_LEN];
    unsigned int advRspDataLen;
    unsigned char advRspData[ADV_VALUE_MAX_LEN];
} HILINK_BT_AdvertiseData;

/* 设置应用层编码模式 */
int HILINK_BT_SetEncodeMode(HILINK_BT_CmdDataMode mode);

/* 查询应用层编码模式 */
HILINK_BT_CmdDataMode HILINK_BT_GetEncodeMode(void);

/* 初始化启动HiLink Bluetooth SDK */
int HILINK_BT_Init(const HILINK_BT_Profile *profile);

/* 初始化蓝牙协议栈 */
int HILINK_BT_InitBtStack(void);

/* 启动HiLink BT SDK处理，调用HiLink协议栈 */
int HILINK_BT_Process(void);

/*
 * 结束HiLink Bluetooth SDK
 * flag为0：只销毁控制和调度线程，flag为1销毁蓝牙协议栈，该函数不可重入
 */
int HILINK_BT_DeInit(unsigned int flag);

/* 添加HiLink服务信息service信息 */
int HILINK_BT_AddHiLilnkService(const HILINK_BT_SvcInfo *serviceArray, unsigned int serviceNum);

/* 通知服务状态 */
int HILINK_BT_ReportServiceState(const void *service, const void *buf, unsigned int len);

/* 通知属性状态 */
int HILINK_BT_ReportAttrState(const void *svc, const void *attr, const void *buf, unsigned int len);

/* 查询蓝牙数据发送接口 */
HILINK_BT_SendBtDataCallback HILINK_BT_GetBtDataSendCallback(void);

/* 查询蓝牙OTA数据发送接口 */
HILINK_BT_SendBtDataCallback HILINK_BT_GetOtaDataSendCallback(void);

/* 设置蓝牙SDK事件处理函数 */
int HILINK_BT_SetSdkEventCallback(HILINK_BT_SdkEventCallBack callback);

/*
 * 设置BLE最大连接数量
 * 入参connNum的范围为[1,10]
 * 最大连接数上限为10，超过10个按10个执行
 * 最小连接数为1，小于1按1个执行
 * 若不调用该接口，默认最大连接数为1
 */
void HILINK_BT_SetMaxConnNum(int connNum);

/* 查询蓝牙SDK最大连接数量 */
int HILINK_BT_GetMaxConnNum(void);

/* 添加蓝牙SDK自定义gatt服务 */
int HILINK_BT_SetGattProfile(const HILINK_BT_GattServiceList *gattServiceList);

/* 注册配置保存回调函数到HiLink Bluetooth SDK，若不调用该函数，则默认使用HiLink Bluetooth SDK保存配置实现 */
int HILINK_BT_RegisterConfigInterface(const HILINK_BT_ConfigInterface *interface);

/* 启动广播 */
int HILINK_BT_StartAdvertise(void);

/* 停止广播 */
int HILINK_BT_StopAdvertise(void);

/* 上报蓝牙反馈数据 */
int HILINK_BT_IndicateSvcCharData(const char *svcUuid, const char *charUuid, const char *buf, unsigned int len);

/* 获取蓝牙SDK设备相关信息 */
HILINK_BT_DevInfo *HILINK_BT_GetDevInfo(void);

/* 获取靠近发现中广播数据 */
int HILINK_BT_GetAdvertiseData(HILINK_BT_AdvertiseData *advertiseData);

/* 获取BLE厂商注册的回调函数 */
BLE_CfgNetCb *GetBleCfgNetCallback(void);

/* 设置开启Kitframework认证 */
void HILINK_BT_EnableKitframework(void);

/* 设置关闭Kitframework认证 */
void HILINK_BT_DisableKitframework(void);

typedef struct {
    unsigned char ivNonce[HIBEACON_IV_NONCE_LEN];
    unsigned char psk[HIBEACON_PSK_LEN];
} HiBeaconData;

typedef int (*HiBeaconDataHandler)(const HiBeaconData *data);

/* 注册hibeacon设备配网数据回调 */
void HILINK_BT_RegHiBeaconDataHandler(HiBeaconDataHandler handler);

#ifdef __cplusplus
}
#endif
#endif
