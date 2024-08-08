/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: BLE辅助配网SDK API头文件（此文件为DEMO，需集成方适配修改）
 */

#ifndef BLE_CFG_NET_API_H
#define BLE_CFG_NET_API_H

#ifdef __cplusplus
extern "C" {
#endif

/* WIFI信息长度宏定义 */
#define WIFI_SSID_MAX_LEN 32
#define WIFI_PWD_MAX_LEN 64
#define WIFI_PSK_LEN 32
#define WIFI_BSSID_LEN 6

#define BLE_UUID_LEN 16

/* 用户发送的数据类型 */
typedef enum {
    NETCFG_DATA,
    CUSTOM_DATA,
    CUSTOM_SEC_DATA,
    DATA_TYPE_BUTT
} BLE_DataType;

/* 属性类型定义 */
typedef enum {
    ATTR_TYPE_SERVICE = 0,
    ATTR_TYPE_CHAR,
    ATTR_TYPE_CHAR_VALUE,
    ATTR_TYPE_CHAR_CLIENT_CONFIG,
    ATTR_TYPE_CHAR_USER_DESC,
} BLE_AttrType;

/* UUID长度定义 */
typedef enum {
    UUID_TYPE_NULL = 0,
    UUID_TYPE_16_BIT,
    UUID_TYPE_32_BIT,
    UUID_TYPE_128_BIT,
} BLE_UuidType;

/* BLE辅助配网状态定义 */
typedef enum {
    CFG_NET_PROCESS_SUCCESS = 0x00,
    CFG_NET_BLE_CONNECT,
    CFG_NET_BLE_DIS_CONNECT,
    CFG_NET_SPEKE_SUCCESS,
    CFG_NET_PROCESS_START,
    CFG_NET_RECEIVE_PARA,
    CFG_NET_WIFI_CONNECT,
    CFG_NET_SESSIONKEY_SUCCESS,
    CFG_NET_FAIL_UNKUNOWN = 0x100,
    CFG_NET_FAIL_WIFI_SSID,
    CFG_NET_FAIL_WIFI_PWD,
    CFG_NET_FAIL_SESSIONKEY,
    OTA_WRITE_TASK_START,
    OTA_WRITE_TASK_FINISH,
    CFG_CLEAR_DEV_REG_INFO,
} BLE_CfgNetStatus;

/* BLE广播类型定义 */
typedef enum {
    BLE_ADV_DEFAULT,
    BLE_ADV_NEARBY_V0,
    BLE_ADV_ONEHOP,
    BLE_ADV_LOCAL_NAME,
    BLE_ADV_CUSTOM
} BLE_AdvType;

/*
 * 获取设备PIN码函数类型
 * pincode: 存放pin码的缓冲区
 * size: 缓冲区的长度
 * len: 返回的pin码实际长度
 */
typedef int (*BLE_GetDevPinCode)(unsigned char *pinCode, unsigned int size, unsigned int *len);

/*
 * 获取设备信息函数类型，len即是入参也是出参，入参代表buff缓冲区长度，出参代表获取的设备信息实际长度
 * 格式要求：{"productId":"%s", "sn":"%s", "vendor":"%s"}
 */
typedef int (*BLE_GetDeviceInfo)(unsigned char *devInfo, unsigned int *len);

/*
 * 设置配网信息函数类型
.* 数据格式：{"ssid":"%s","password":"%s","devId":"%s","psk":"%s","code":"%s","random":"%s","vendorData":"%s"}
 */
typedef int (*BLE_SetCfgNetInfo)(const unsigned char *netInfo, unsigned int len);

/* 接收用户数据函数类型 */
typedef int (*BLE_RcvCustomData)(unsigned char *buff, unsigned int len);

/* 配网过程状态处理函数类型 */
typedef int (*BLE_CfgNetProcess)(BLE_CfgNetStatus status);

/* BLE GATT服务读函数类型 */
typedef int (*BLE_GattRead)(unsigned char *buff, unsigned int *len);

/* BLE GATT服务写函数类型 */
typedef int (*BLE_GattWrite)(const unsigned char *buff, unsigned int len);

/* BLE GATT服务指示函数类型 */
typedef int (*BLE_GattIndicate)(unsigned char *buff, unsigned int len);

/* BLE 短距发现开启 */
typedef int (*BLE_NearDiscoveryStart)(void);

/* BLE 短距发现关闭 */
typedef int (*BLE_NearDiscoveryClose)(void);

/* BLE 短距发现回调函数指针 */
typedef struct {
    BLE_NearDiscoveryStart startCb;
    BLE_NearDiscoveryClose closeCb;
} BLE_NearDiscoveryCb;

/* BLE GATT回调函数指针 */
typedef struct {
    BLE_GattRead readCb;
    BLE_GattWrite writeCb;
    BLE_GattIndicate indicateCb;
} BLE_GattOperateFunc;

/* BLE配置参数 */
typedef struct {
    int isBlePair;
    int isDeinitBleStack;
    int data1; /* 为后期配置参数预留，暂不使用 */
    int data2; /* 为后期配置参数预留，暂不使用 */
    int data3; /* 为后期配置参数预留，暂不使用 */
} BLE_ConfPara;

/* BLE GATT服务 */
typedef struct {
    BLE_AttrType attrType;
    unsigned int permission;
    BLE_UuidType uuidType;
    unsigned char uuid[BLE_UUID_LEN];
    unsigned char *value;
    unsigned char valLen;
    unsigned char properties;
    BLE_GattOperateFunc func;
} BLE_GattAttr;

/* GATT服务(单个service及其下挂的全部characteristics和descriptions) */
typedef struct {
    unsigned int attrNum;
    BLE_GattAttr *attrList;
} BLE_GattService;

/* GATT列表(包含多个services和返回的handle) */
typedef struct {
    unsigned int num;
    BLE_GattService *service;
    int *handle;
} BLE_GattList;

/* GATT句柄列表 */
typedef struct {
    unsigned int num;
    int *handle;
} BLE_GattHandleList;

/* BLE的广播数据和扫描应答数据 */
typedef struct {
    unsigned char *advData;
    unsigned int advDataLen;
    unsigned char *rspData;
    unsigned int rspDataLen;
} BLE_AdvData;

/* BLE的广播参数 */
typedef struct {
    unsigned char advType;
    unsigned char discMode;
    unsigned char connMode;
    unsigned int minInterval;
    unsigned int maxInterval;
    unsigned int channelMap;
    unsigned int timeout;
    int txPower;
} BLE_AdvPara;

/* 广播参数和数据 */
typedef struct {
    BLE_AdvPara *advPara;
    BLE_AdvData *advData;
} BLE_AdvInfo;

/* BLE初始化参数 */
typedef struct {
    BLE_ConfPara *confPara;
    BLE_AdvInfo *advInfo;
    BLE_GattList *gattList;
} BLE_InitPara;

/* BLE配网回调函数 */
typedef struct {
    BLE_GetDevPinCode getDevPinCodeCb;
    BLE_GetDeviceInfo getDeviceInfoCb;
    BLE_SetCfgNetInfo setCfgNetInfoCb;
    BLE_RcvCustomData rcvCustomDataCb;
    BLE_CfgNetProcess cfgNetProcessCb;
} BLE_CfgNetCb;

/*
 * BLE配网资源申请：BLE协议栈启动、配网回调函数挂接
 * 如为厂家实现协议栈para传NULL,如需华为实现协议栈则需赋值para结构体内容
 */
int BLE_CfgNetInit(const BLE_InitPara *para, const BLE_CfgNetCb *cb);

/*
 * BLE配网资源注销：配网回调函数清理、BLE协议栈销毁
 * flag为0：只销毁控制和调度线程，flag为1销毁蓝牙协议栈
 */
int BLE_CfgNetDeInit(const BLE_GattHandleList *handleList, unsigned int flag);

/* BLE配网广播控制：参数代表广播时间，0:停止；0xFFFFFFFF:一直广播，其他：广播指定时间后停止，单位秒 */
int BLE_CfgNetAdvCtrl(unsigned int advSecond);

/*
 * 更新广播参数，更新完成后需调用BLE_CfgNetAdvCtrl启动广播
 * 传入空值时可启动hilink构造的广播
 */
int BLE_CfgNetAdvUpdate(const BLE_AdvInfo *advInfo);

/* BLE配网断开连接：防止其他任务长时间占用BLE连接 */
int BLE_CfgNetDisConnect(void);

/* BLE发送用户数据：用户数据发送，与接收回调函数配套使用 */
int BLE_SendCustomData(BLE_DataType dataType, const unsigned char *buff, unsigned int len);

/* BLE获取蓝牙广播类型 */
int BLE_GetAdvType(void);

/*
 * BLE设置蓝牙广播类型：
 * type:BLE_ADV_DEFAULT表示新蓝牙靠近发现类型;
 * type:BLE_ADV_ONEHOP表示蓝牙碰一碰类型；
 * type:BLE_ADV_LOCAL_NAME表示常态广播类型
 */
void BLE_SetAdvType(int type);

/*
 * 设置蓝牙广播名称中的MPP字段，参考“BLE设备接入规范”文档中蓝牙设备名称描述，按需设置
 * 入参含义：mpp
 * m: 1个字节：0x00—0xFF，可选，如果发送该参数，则pp为必传，如果设备侧不携带mpp字段，则网关默认不回连该BLE设备。
 *    bit0: 0心跳广播报文； 1设备主动回连请求广播报文（比如设备有数据上报需要连接网关）
 *    bit1: 心跳时长间隔单位，0单位毫秒，  1 单位为秒。
 * pp: 采用小端格式传输，2个字节 unsigned int 类型参数，16进制。标识设备心跳间隔时长,例如5000ms。
 * len: 长度固定为3个字节 。
 */
int BLE_SetAdvNameMpp(const unsigned char *mpp, unsigned int len);

/*
 * BLE 短距发现注册函数
 * 入参含义：
 *   cb：BLE 短距发现回调函数指针
 *   "BLE_NearDiscoveryStart": 短距功能开启
 *   "BLE_NearDiscoveryClose": 短距功能关闭
 * 返回：
 *   0成功，非0失败
 */
int BLE_NearDiscoveryInit(const BLE_NearDiscoveryCb *cb);

/*
 * BLE 短距发现功能使能
 * 入参含义：
 *   waitTime: 进入短距发现等待时间，单位为秒，设置范围[1, 0XFFFFFFFF)。建议设置600s。
 * 返回：
 *   0成功，非0失败
 * 说明：
 *   蓝牙短距发现功能默认关闭，调用此接口设置等待时间并使能蓝牙短距发现功能；
 *   调用此接口前，先调用“BLE_NearDiscoveryInit”注册“BLE_NearDiscoveryCb”；
 *   使能此功能后，“waitTime”时间内用户没有发起配网，SDK调用“BLE_NearDiscoveryStart”开启短距发现模式，蓝牙
 *   辅助WiFi配网有效发现距离1~2m；
 *   开启短距发现模式后，用户发起配网，SDK调用“BLE_NearDiscoveryClose”关闭短距发现模式，确保配网流程可靠进行；
 *   此功能只影响待配网状态下的蓝牙广播，其它广播形态不受影响；
 */
int BLE_NearDiscoveryEnable(unsigned long waitTime);

/**
 * @brief 根据蓝牙sdk里的任务名获取任务栈大小
 *
 * @param name [IN] 任务名
 * name              | Description
 * "BtScheduleTask"  | 蓝牙sdk调度任务
 * "BtCtrlTask"      | 蓝牙sdk控制任务
 * "BtOtaWriteTask"  | 蓝牙sdk升级写数据任务
 * "BtOtaTimeTask"   | 蓝牙sdk升级时间判断任务
 * "BtAdvCtrlTask"   | 蓝牙sdk广播定时控制任务
 * @param stackSize [OUT] 任务栈大小
 * @return 0成功，非0失败
 */
int HILINK_BT_GetTaskStackSize(const char *name, unsigned long *stackSize);

/**
 * @brief 根据蓝牙sdk的任务名设置任务栈大小
 *
 * @param name [IN] 任务名
 * name              | Description
 * "BtScheduleTask"  | 蓝牙sdk调度任务
 * "BtCtrlTask"      | 蓝牙sdk控制任务
 * "BtOtaWriteTask"  | 蓝牙sdk升级写数据任务
 * "BtOtaTimeTask"   | 蓝牙sdk升级时间判断任务
 * "BtAdvCtrlTask"   | 蓝牙sdk广播定时控制任务
 * @param stackSize [IN] 任务栈大小
 * @return 0成功，非0失败
 * @note 当任务栈不够时，厂商可以先调用HILINK_BT_GetTaskStackSize接口查看hilink
 *       内部的默认值，再根据情况增加栈空间。此函数需要在蓝牙sdk初始化接口
 *       BLE_CfgNetInit之前调用才生效。
 */
int HILINK_BT_SetTaskStackSize(const char *name, unsigned long stackSize);

#ifdef __cplusplus
}
#endif
#endif
