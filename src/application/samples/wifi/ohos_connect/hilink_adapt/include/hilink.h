/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink主流程框架集成头文件（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_H
#define HILINK_H

#include <stdarg.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 * 功能: HiLink SDK属性结构体，开发者可以通过HILINK_GetSdkAttr查看当前系统的属性值，通过HILINK_SetSdkAttr设置新的属性值；
 * 注意: (1) 普通设备形态和网桥设备形态的主线程任务栈大小，开发者根据产品形态，仅需设置对应产品形态的属性值即可；
 *       (2) 使用HiLink SDK升级架构时，需要设置升级检查任务的栈大小和升级任务的栈大小
 *       (3) 如果开发者未注册软重启接口rebootSoftware和硬重启接口rebootHardware，使用HiLink SDK默认实现接口(硬重启)；
 */
typedef struct {
    unsigned long monitorTaskStackSize;    /* 监控任务栈大小，开发者根据具体情况调整，默认为1024字节 */
    unsigned long deviceMainTaskStackSize; /* 普通设备形态，HiLink SDK运行主任务栈大小，开发者根据具体情况调整 */
    unsigned long bridgeMainTaskStackSize; /* 网桥设备形态，HiLink SDK运行主任务栈大小，开发者根据具体情况调整 */
    unsigned long otaCheckTaskStackSize;   /* HiLink OTA检查升级版本任务栈大小，开发者根据具体情况调整 */
    unsigned long otaUpdateTaskStackSize;  /* HiLink OTA升级任务栈大小，开发者根据具体情况调整 */
    int (*rebootSoftware)(void);           /* 异常场景软重启接口，不影响硬件状态，如果用户注册，首先使用此接口 */
    int (*rebootHardware)(void);           /* 异常场景硬重启接口，影响硬件状态，如果用户没有注册软重启，使用此接口重启 */
} HILINK_SdkAttr;

/* HiLink SDK支持的配网模式 */
enum HILINK_NetConfigMode {
    HILINK_NETCONFIG_NONE, /* 不用配网, 通过网线等手段连接到网络 */
    HILINK_NETCONFIG_WIFI, /* HiLink SDK提供的WiFi自动配网 */
    HILINK_NETCONFIG_OTHER, /* 其他配网模式, APP发送WiFi的信息, 集成方收到WiFi信息数据后, 设置到HiLink SDK */
    HILINK_NETCONFIG_BOTH, /* 其他配网模式和WiFi配网组合 */
    HILINK_NETCONFIG_REGISTER_ONLY, /* HiLink SDK SoftAp配网仅接收注册信息 */
    HILINK_NETCONFIG_NO_SOFTAP_REGISTER_ONLY, /* 不启动SoftAp, PIN码配网仅接收注册信息(通过网线/4G/5G等接入网络) */
    HILINK_NETCONFIG_NAN_SOFTAP, /* WiFi感知超短距配网和SoftAp组合 */
    HILINK_NETCONFIG_BUTT /* 非法配网模式 */
};

typedef enum {
    SETUP_TYPE_NORMAL = 0, /* 后装安装方式 */
    SETUP_TYPE_CENTRAL = 1, /* 前装安装方式 */
    SETUP_TYPE_AILIFE = SETUP_TYPE_NORMAL, /* 智慧生活APP安装方式 */
    SETUP_TYPE_AIINSTALL = SETUP_TYPE_CENTRAL, /* 装维易APP安装方式 */
    SETUP_TYPE_UNKNOW = 2, /* 无效安装方式 */
    SETUP_TYPE_UNREGISTER = 3, /* 未注册回调 */
} SetupType;

typedef struct {
    int (*MlogProcFunc)(const char* format, va_list args);

    /*
     * 功能：日志重定向
     * 参数：logLevel，日志等级，详细信息参考hilink_log_manage.h
     *       func，函数名
     *       line，函数行号
     *       format，格式化字符串
     *       args，格式化字符串可变参数列表
     * 注意：进行日志重定向要保留函数名和行号信息，推荐以{[{tag}]{level}:{func}() {line}, {msg}}进行输出
     *       例如"[HiLink]INFO:HILINK_RegisterBaseCallback() 45, something"
     *       日志重定向依然受当前HiLink日志等级的约束，详细参考hilink_log_manage.h
     *       重定向后原有的串口打印不再生效
     */
    void (*LogVprint)(int logLevel, const char *func, unsigned int line, const char* format, va_list args);

    /**
     * @brief 记录tracelog日志，适配按模块统计计数并流控的能力, 按字符串打印
     *
     * @param srcModule [IN] 记录跟踪日志的源模块
     * @param dstModule [IN] 记录跟踪日志的目的模块
     * @param msgType   [IN] 记录跟踪消息类型
     * @param format    [IN] 格式化字符串
     * @param args      [IN] 格式化字符串可变参数列表
     * @attention 消息打印字符串长度超过单条最大长度时截断处理
    */
    void (*traceMsgPrint)(const char *srcModule, const char *dstModule,
        const char *msgType, const char *format, va_list args);

    /**
     * @brief 记录tracelog日志，适配按模块统计计数并流控的能力, 按字符串打印
     *
     * @param moduleId  [IN] 打印日志模块id
     * @param srcModule [IN] 记录跟踪日志的源模块
     * @param dstModule [IN] 记录跟踪日志的目的模块
     * @param msgType   [IN] 记录跟踪消息类型
     * @param content   [IN] 记录跟踪日志数据内容
    */
    void (*traceMsgPrintEx)(int moduleId, const char *srcModule, const char *dstModule,
        const char *msgType, const char *content);
} HiLinkBaseCallback;

/*
 * 功能：注册基础功能回调函数
 * 参数：cb，回调函数结构体指针
 *       cbSize，回调函数结构体大小sizeof(HiLinkBaseCallback)
 * 返回：0，成功
 *       小于0，失败
 */
int HILINK_RegisterBaseCallback(const HiLinkBaseCallback *cb, unsigned int cbSize);

/*
 * 功能: HiLink SDK入口函数
 * 返回: 0表示成功，返回-1表示失败
 */
int HILINK_Main(void);

/* HiLink SDK 复位接口 */
void HILINK_Reset(void);

/*
 * 功能: 设置HiLink SDK属性
 * 返回: 0表示设置成功，其他设置失败
 */
int HILINK_SetSdkAttr(HILINK_SdkAttr sdkAttr);

/* 查询HiLink SDK属性 */
HILINK_SdkAttr *HILINK_GetSdkAttr(void);

/*
 * 功能: HiLink SDK恢复出厂设置接口
 * 返回: 0表示恢复出厂成功,返回-1表示恢复出厂失败
 * 注意: (1) 设置成功后会清理掉ssid账号信息，并重启模组进入待配网状态
 *       (2) 在设备未注册场景调用此接口恢复出厂， HiLink SDK会调用HILINK_NotifyDevStatus接口通知设备解绑状态
 *       (3) 禁止在HILINK_NotifyDevStatus接口内回调 HiLink SDK对外接口
 */
int HILINK_RestoreFactorySettings(void);

/* 获取当前设备状态，如配网状态、连接云端、在线、离线等，具体见HiLinkStateMachine */
int HILINK_GetDevStatus(void);

/* 获取当前设备集成的HiLink SDK的版本号 */
const char *HILINK_GetSdkVersion(void);

/*
 * 功能：主动上报服务属性状态
 * 参数：(1) svcId 入参，服务ID
 *       (2) payload 入参，json格式服务属性数据
 *       (3) len 入参，payload长度
 * 返回：返回0表示服务状态上报成功，返回-1表示服务状态上报失败
 * 注意：(1) 该接口有同步与异步两种使用方式，对于事件类上报推荐使用同步上报
 *       (2) 同步上报：payload不为NULL且len不为0时，调用该接口时，HiLink SDK会立即上报该payload
 *       (3) 异步上报：payload为NULL或len为0时，调用该接口后，HiLink SDK会记录下该svcId，
 *           并稍后（约200ms）调用HILINK_GetCharState接口获取服务属性并上报
 */
int HILINK_ReportCharState(const char *svcId, const char *payload, unsigned int len);

/*
 * 功能：查询设备是否已被注册
 * 返回：返回非0，已注册；返回0，未注册；
 */
int HILINK_IsRegister(void);

/*
 * 获取当前设备组网状态
 * 返回1表示中枢组网状态，返回2表示云组网状态，返回0表示异常状态
 */
int HILINK_GetNetworkingMode(void);

/*
 * 获取当前设备注册状态
 * 返回1表示中枢注册状态，返回2表示云注册状态，返回0表示未注册状态
 */
int HILINK_GetRegisterStatus(void);

/*
 * 功能：设置HiLink SDK主任务调度时间间隔，默认为50ms
 * 参数: interval HiLink SDK主任务调度时间间隔，单位为毫秒，取值范围[5,100]
 * 返回：设置失败返回-1，成功返回0
 * 注意：Hi3681L模组(支持低功耗)interval无取值范围限制
 */
int HILINK_SetScheduleInterval(unsigned long interval);

/*
 * 功能：设置HiLink SDK守护任务调度时间间隔，默认1000ms
 * 参数: interval HiLink SDK守护任务调度时间间隔，单位为毫秒
 * 返回：设置失败返回-1，成功返回0
 */
int HILINK_SetMonitorScheduleInterval(unsigned long interval);

/* 设置产品配网模式, 注意: 需要在启动HILINK_Main之前调用本接口设置配网模式 */
int HILINK_SetNetConfigMode(enum HILINK_NetConfigMode netConfigMode);

/* 查询当前产品的配网模式, 返回值为当前产品的配网模式 */
enum HILINK_NetConfigMode HILINK_GetNetConfigMode(void);

/*
 * 功能: 设置HiLink SDK配网超时时间，单位为秒
 * 注意: 户外设备默认超时时间为2分钟，其余设备默认10分钟
 */
void HILINK_SetNetConfigTimeout(unsigned long netConfigTimeout);

/**
 * @brief 设置设备在OTA升级完成重启到重新上线的时间，默认为120s
 *
 * @param bootTime [IN] 设置的时间
 * @return 0表示成功，其他表示失败
 * @attention 允许设置的最大时间bootTime最大为600
 */
int HILINK_SetOtaBootTime(unsigned int bootTime);

/*
 * 功能: 设置HiLink SDK打开Kitframework认证，默认为关闭状态
 * 注意: 该函数由设备开发者或厂商在HILINK_Main函数之前调用一次，不可动态调用
 */
void HILINK_EnableKitframework(void);

/*
 * 功能: 设置HiLink SDK打开组播控制功能，默认为关闭状态
 * 注意: 该函数由设备开发者或厂商在HILINK_Main函数之前调用；当前仅支持灯类WiFi产品使用，安全敏感设备请勿使用
 *      组播控制将通过HILINK_ControlCharState接口传递至产品侧
 */
void HILINK_EnableGroupCtrl(void);

/*
 * @brief 设置单设备多服务批控(type = 4)，默认关闭
 * @attention 批控使能打开后最终调用HILINK_ControlCharState，0表示关闭，1表示开启
 */
void HILINK_EnableBatchControl(bool flag);

/*
 * 设备离线时，如果在App上删除了设备，设备再次上线时云端会给设备下发Errcode=5或Errcode=6错误码。
 * 该接口用于使能SDK处理云端下发的Errcode=5或Errcode=6错误码。
 * enable为0表示SDK不处理云端下发的Errcode=5或Errcode=6错误码，此时SDK不会清除设备端注册信息，
 * 需要用户手动硬件恢复出厂设置，设备才能重新进行配网状态。
 * enable为非0表示SDK处理云端下发的Errcode=5或Errcode=6错误码，此时SDK会清除设备端注册信息，重新进行配网状态
 * 默认enable为1
 */
void HILINK_EnableProcessDelErrCode(int enable);

/*
 * 开发者直接调用该接口完成设备解绑后装信息并恢复交房设置
 * type 0：普通解绑 1:其它场景
 * 返回0表示成功，其他错误码异常
 */
void HILINK_UnbindDevice(int type);

/*
* 设置设备安装方式
* type：0表示后装安装方式，1表示前装安装方式
* 正常返回0，异常返回其他错误码
*/
int HILINK_SetDeviceInstallType(int type);

/*
 * 获取设备是否带前装标识
 * 返回0：不带前装标识，1：带前装标识
*/
SetupType HILINK_GetDevSetupType(void);

/**
 * @brief 使能devId继承功能
 * WiFi单品如需支持故障更换，需要使能，默认不使能。
 * @param[in] <isEnbale> 是否使能
 */
void HILINK_EnableDevIdInherit(bool isEnbale);

/**
 * @brief 网络状态发生变化时系统调用此接口
 *
 * @param status [IN]
 *             true - 网络可用
 *             false - 网络不可用
 * @return void
 */
void HILINK_NotifyNetworkAvailable(bool status);

/* hilink wifi配网模式入口函数 */
int hilink_entry(void *param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HILINK_H */
