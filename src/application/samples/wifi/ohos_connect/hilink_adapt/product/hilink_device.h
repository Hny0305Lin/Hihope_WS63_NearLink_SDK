/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink产品适配头文件（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_DEVICE_H
#define HILINK_DEVICE_H

#include "hilink.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    char sn[40];            /* 设备唯一标识，比如sn号，有效字符长度范围[0,40)，为空时使用MAC地址作为SN */
    char prodId[5];         /* 设备产品ID，有效字符长度为4 */
    char subProdId[3];      /* 设备子型号，有效字符长度为0或2 */
    char model[32];         /* 设备型号，长度范围[0,32) */
    char devTypeId[4];      /* 设备类型ID，有效字符长度为3 */
    char devTypeName[32];   /* 设备类型英文名称，长度范围[0,32) */
    char manuId[4];         /* 设备制造商ID，有效字符长度为3 */
    char manuName[32];      /* 设备制造商英文名称，长度范围[0,32) */
    char fwv[64];           /* 设备固件版本，长度范围[0,64) */
    char hwv[64];           /* 设备硬件版本，长度范围[0,64) */
    char swv[64];           /* 设备软件版本，长度范围[0,64) */
} HILINK_DevInfo;

typedef struct {
    char svcType[32];  /* 服务类型，字符长度范围(0, 32) */
    char svcId[64];    /* 服务ID，字符长度范围(0, 64) */
} HILINK_SvcInfo;

enum HILINK_StateMachine {
    /* 设备与云端连接断开(版本向前兼容) */
    HILINK_M2M_CLOUD_OFFLINE = 0,
    /* 设备连接云端成功，处于正常工作态(版本向前兼容) */
    HILINK_M2M_CLOUD_ONLINE,
    /* 设备与云端连接长时间断开(版本向前兼容) */
    HILINK_M2M_LONG_OFFLINE,
    /* 设备与云端连接长时间断开后进行重启(版本向前兼容) */
    HILINK_M2M_LONG_OFFLINE_REBOOT,
    /* HiLink线程未启动 */
    HILINK_UNINITIALIZED,
    /* 设备处于配网模式 */
    HILINK_LINK_UNDER_AUTO_CONFIG,
    /* 设备处于10分钟超时状态 */
    HILINK_LINK_CONFIG_TIMEOUT,
    /* 设备正在连接路由器 */
    HILINK_LINK_CONNECTTING_WIFI,
    /* 设备已经连上路由器 */
    HILINK_LINK_CONNECTED_WIFI,
    /* 设备正在连接云端 */
    HILINK_M2M_CONNECTTING_CLOUD,
    /* 设备与路由器的连接断开 */
    HILINK_LINK_DISCONNECT,
    /* 设备被注册 */
    HILINK_DEVICE_REGISTERED,
    /* 设备被解绑 */
    HILINK_DEVICE_UNREGISTER,
    /* 设备复位标记置位 */
    HILINK_REVOKE_FLAG_SET,
    /* 设备协商注册信息失败 */
    HILINK_NEGO_REG_INFO_FAIL,
    /* 设备与路由器的连接失败 */
    HILINK_LINK_CONNECTED_FAIL,
    /* 打开中枢模式成功 */
    HILINK_OPEN_CENTRAL_MODE_OK,
    /* 打开中枢模式失败 */
    HILINK_OPEN_CENTRAL_MODE_FAILE,
    /* 本地端口创建成功 */
    HILINK_CREATE_CENTRAL_PORT_OK,
    /* 设备恢复交房标记 */
    HILINK_RECOVER_HANDOVER,
    /* 设备交房标记 */
    HILINK_HANDOVER,
    /* 设备已初始化 */
    HILINK_INITIALIZED,
    /* 直连云模式切成中枢模式 */
    HILINK_CLOUD_TO_CENTRAL,
    /* 设备信息完全清除，包括前装跟后装信息 */
    HILINK_DEVICE_CLEAN,
    /* 共部署设备被设置为单品模式 */
    HILINK_DEVICE_MODE,
    /* 设备TLS链路重连事件 */
    HILINK_TLS_LINK_RECONNECTED,
};

typedef int (*HILINK_GetAcKeyFunc)(unsigned char *acKey, unsigned int acLen);

/*
 * 功能：注册获取ACkeyV2函数，key文件在开发者平台获取
 * 参数：HILINK_GetAcKeyFun 入参
 * 备注，SDK优先使用ACkeyV2，HILINK_GetAutoAc逐渐日落。
 */
void HILINK_RegisterGetAcV2Func(HILINK_GetAcKeyFunc func);

/*
 * 功能：获取设备信息
 * 参数：devinfo 出入参，待填充的设备信息
 * 返回：获取成功返回 0，否则返-1
 * 注意：(1) sn不填充时将使用设备mac地址作为sn。
 *       (2) 如果产品定义有子型号则需要填充subProdId。
 *       (3) 所有需要填充的字段都需要以'\0'结束。
 *       (4) softap配网时devTypeName与manuName用来拼接ssid，两个字段长度和尽量不超过10字节，否则会截断。
 */
int HILINK_GetDevInfo(HILINK_DevInfo *devinfo);

/*
 * 功能：获取设备服务信息
 * 参数：(1) svcInfo 出入参，待填充的设备服务信息结构体指针数组。
 *       (2) size 入参，svcInfo结构体指针数组大小。
 * 返回：获取成功返回服务数量，否则返-1。
 * 注意：所有需要填充的字段都需要以'\0'结束。
 */
int HILINK_GetSvcInfo(HILINK_SvcInfo *svcInfo[], unsigned int size);

/* 获取AC 参数接口函数 */
unsigned char *HILINK_GetAutoAc(void);

/*
 * 修改服务当前字段值
 * svcId为服务的ID，payload为接收到需要修改的Json格式的字段与其值，len为payload的长度。
 * 返回0表示该次控制为同步上报，由HILINK SDK上报。
 * 返回-111表示该次控制为异步上报，修改成功后设备必须调用HILINK_ReportCharState主动上报。
 * 其余返回值均为错误值，HILINK SDK不会上报任何信息。
 */
int HILINK_PutCharState(const char *svcId, const char *payload, unsigned int len);

/*
 * 功能：单设备多服务批控接口
 * 参数：payload为Json格式的报文内容(数组格式)，len为payload的长度，例如：
 * [
 *   {"sid": "switch", "data":{"on": 1}},
 *   {"sid": "brightness","data": {"brightness": 20}}
 * ]
 * 返回值：0表示服务状态值修改成功，不需要底层设备主动上报，由HiLink SDK上报。
 *         -101表示获得报文不符合要求。
 *         -111表示服务状态值正在修改中。
 * 注意：(1) 此函数由设备厂商实现。
 *       (2) 厂商需通过HILINK_EnableBatchControl(bool flag)接口打开或关闭批量控制功能，默认关闭。
 */
int HILINK_ControlCharState(const char *payload, unsigned int len);

/*
 * 获取服务字段值
 * svcId表示服务ID。厂商实现该函数时，需要对svcId进行判断。
 * in表示接收到的Json格式的字段与其值。
 * inLen表示接收到的in的长度。
 * out表示保存服务字段值内容的指针,内存由厂商开辟，使用完成后，由Hilink Device SDK释放，需要以'\0'结尾,并保证内容符合json格式。
 * outLen表示读取到的payload的长度。
 * 返回0表示服务状态字段值获取成功，返回非0表示获取服务状态字段值不成功。
 */
int HILINK_GetCharState(const char *svcId, const char *in, unsigned int inLen, char **out, unsigned int *outLen);

/*
 * 获取SoftAp配网PIN码
 * 返回值为8位数字PIN码, 返回-1表示使用HiLink SDK的默认PIN码。
 * 该接口需设备开发者实现。
 * 安全认证要求，PIN码不能由sn、mac等设备固定信息生成。
 */
int HILINK_GetPinCode(void);

/*
 * 通知设备的状态
 * status表示设备当前的状态
 * 注意: (1) 此函数由设备厂商根据产品业务选择性实现。
 *      (2) 禁止在HILINK_NotifyDevStatus接口内回调HiLink SDK的对外接口。
 */
void HILINK_NotifyDevStatus(int status);

 /*
 * 功能：实现模组重启前的设备操作
 * 参数：flag 入参，触发重启的类型
 *          0表示HiLink SDK 线程看门狗触发模组重启。
 *          1表示APP删除设备触发模组重启。
 *          2表示设备长时间离线无法恢复而重启。
            3表示网关定时重启。
 * 返回值：0表示处理成功, 系统可以重启，使用硬重启。
 *         1表示处理成功, 系统可以重启，如果通过HILINK_SetSdkAttr()注册了软重启(sdkAttr.rebootSoftware），使用软重启。
 *         负值表示处理失败，系统不能重启。
 * 注意：(1) 此函数由设备厂商实现。
 *       (2) 若APP删除设备触发模组重启时，设备操作完务必返回0，否则会导致删除设备异常。
 *       (3) 设备长时间离线无法恢复而重启，应对用户无感，不可影响用户体验，否则不可以重启。
 */
int HILINK_ProcessBeforeRestart(int flag);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HILINK_DEVICE_H */