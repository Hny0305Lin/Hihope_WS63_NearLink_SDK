/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink产品适配实现源文件（此文件为DEMO，需集成方适配修改）
 */
#include "hilink_device.h"
#include <stdlib.h>
#include "hilink.h"
#include "securec.h"
#include "hilink_sal_defines.h"

/* 设备产品ID */
static const char *PRODUCT_ID = "2AQO";

/* 设备产品子型号ID */
static const char *SUB_PRODUCT_ID = "00";
/* 设备类型ID */
static const char *DEVICE_TYPE_ID = "046";
/* 设备类型英文名称 */
static const char *DEVICE_TYPE_NAME = "Table Lamp";
/* 设备制造商ID */
static const char *MANUAFACTURER_ID = "17C";
/* 设备制造商英文名称 */
static const char *MANUAFACTURER_NAME = "DALEN";
/* 设备型号 */
static const char *PRODUCT_MODEL = "DL-01W";
/* 设备SN */
static const char *PRODUCT_SN = "123";
/* 设备固件版本号 */
static const char *FIRMWARE_VER = "1.0.0";
/* 设备硬件版本号 */
static const char *HARDWARE_VER = "1.0.0";
/* 设备软件版本号 */
static const char *SOFTWARE_VER = "1.0.0";

/*
 *
 * 服务信息定义
 * 注意：(1)适配格式{svcType， svcId}
 *       (2)与devicepartner平台物模型定义必须保持一致
 */
static const HILINK_SvcInfo SVC_INFO[] = {
    { "switch", "switch" }
};

int HILINK_GetDevInfo(HILINK_DevInfo *devinfo)
{
    if (devinfo == NULL) {
        return -1;
    }
    int err = EOK;
    err |= strcpy_s(devinfo->sn, sizeof(devinfo->sn), PRODUCT_SN);
    HILINK_SAL_DEBUG("HILINK_GetDevInfo sn: [%s]\r\n", devinfo->sn);
    err |= strcpy_s(devinfo->prodId, sizeof(devinfo->prodId), PRODUCT_ID);
    HILINK_SAL_DEBUG("HILINK_GetDevInfo prodId: [%s]\r\n", devinfo->prodId);
    err |= strcpy_s(devinfo->subProdId, sizeof(devinfo->subProdId), SUB_PRODUCT_ID);
    HILINK_SAL_DEBUG("HILINK_GetDevInfo subProdId: [%s]\r\n", devinfo->subProdId);
    err |= strcpy_s(devinfo->model, sizeof(devinfo->model), PRODUCT_MODEL);
    HILINK_SAL_DEBUG("HILINK_GetDevInfo model: [%s]\r\n", devinfo->model);
    err |= strcpy_s(devinfo->devTypeId, sizeof(devinfo->devTypeId), DEVICE_TYPE_ID);
    HILINK_SAL_DEBUG("HILINK_GetDevInfo devTypeId: [%s]\r\n", devinfo->devTypeId);
    err |= strcpy_s(devinfo->devTypeName, sizeof(devinfo->devTypeName), DEVICE_TYPE_NAME);
    HILINK_SAL_DEBUG("HILINK_GetDevInfo devTypeName: [%s]\r\n", devinfo->devTypeName);
    err |= strcpy_s(devinfo->manuId, sizeof(devinfo->manuId), MANUAFACTURER_ID);
    HILINK_SAL_DEBUG("HILINK_GetDevInfo manuId: [%s]\r\n", devinfo->manuId);
    err |= strcpy_s(devinfo->manuName, sizeof(devinfo->manuName), MANUAFACTURER_NAME);
    HILINK_SAL_DEBUG("HILINK_GetDevInfo manuName: [%s]\r\n", devinfo->manuName);
    err |= strcpy_s(devinfo->fwv, sizeof(devinfo->fwv), FIRMWARE_VER);
    HILINK_SAL_DEBUG("HILINK_GetDevInfo fwv: [%s]\r\n", devinfo->fwv);
    err |= strcpy_s(devinfo->hwv, sizeof(devinfo->hwv), HARDWARE_VER);
    HILINK_SAL_DEBUG("HILINK_GetDevInfo hwv: [%s]\r\n", devinfo->hwv);
    err |= strcpy_s(devinfo->swv, sizeof(devinfo->swv), SOFTWARE_VER);
    HILINK_SAL_DEBUG("HILINK_GetDevInfo swv: [%s]\r\n", devinfo->swv);
    if (err != EOK) {
        return -1;
    }
    return 0;
}

int HILINK_GetSvcInfo(HILINK_SvcInfo *svcInfo[], unsigned int size)
{
    unsigned int svcNum = sizeof(SVC_INFO) / sizeof(HILINK_SvcInfo);
    if ((svcInfo == NULL) || (size == 0) || (size < svcNum)) {
        return -1;
    }

    for (unsigned int i = 0; i < svcNum; ++i) {
        if (memcpy_s(svcInfo[i], sizeof(HILINK_SvcInfo), &SVC_INFO[i], sizeof(HILINK_SvcInfo)) != EOK) {
            return -1;
        }
    }
    return svcNum;
}

/* AC参数 */
unsigned char A_C[48] = {
    0x49, 0x3F, 0x45, 0x4A, 0x3A, 0x72, 0x38, 0x7B, 0x36, 0x32, 0x50, 0x3C, 0x49, 0x39, 0x62, 0x38,
    0x72, 0xCB, 0x6D, 0xC5, 0xAE, 0xE5, 0x4A, 0x82, 0xD3, 0xE5, 0x6D, 0xF5, 0x36, 0x82, 0x62, 0xEB,
    0x89, 0x30, 0x6C, 0x88, 0x32, 0x56, 0x23, 0xFD, 0xB8, 0x67, 0x90, 0xA7, 0x7B, 0x61, 0x1E, 0xAE
};

/* 获取加密 AC 参数  */
unsigned char *HILINK_GetAutoAc(void)
{
    return A_C;
}

/*
 * 修改服务当前字段值
 * svcId为服务的ID，payload为接收到需要修改的Json格式的字段与其值，len为payload的长度
 * 返回0表示服务状态值修改成功，不需要底层设备主动上报，由Hilink Device SDK上报；
 * 返回-101表示获得报文不符合要求；
 * 返回-111表示服务状态值正在修改中，修改成功后底层设备必须主动上报；
 */
int HILINK_PutCharState(const char *svcId, const char *payload, unsigned int len)
{
    return 0;
}

/*
 * 添加单设备多服务批控服务接口
 * 1.使用场景：单产品需要在一个报文包中包含多个服务同时处理
 * 2.使用方法：
 * (1)厂商需要编写支持批控的H5页面，增加type = 4以及在action中可以包含一个或者以上的服务。
 * (2)厂商需要通过HILINK_EnableBatchControl(bool flag)接口打开或关闭批量功能，当前默认关闭。
 * 3.使用约束：
 * (1)payload为接收到报文的actions内容，为数组格式，len为actions内容的长度；详情请参考《指南》
 * (2)返回值判断：
 *     返回0表示服务状态值修改成功，不需要底层设备主动上报，由HiLink SDK上报；
 *     返回-101表示获得报文不符合要求；
 *     返回-111表示服务状态值正在修改中；
 */
int HILINK_ControlCharState(const char *payload, unsigned int len)
{
    return 0;
}

/*
 * 获取服务字段值
 * svcId表示服务ID。厂商实现该函数时，需要对svcId进行判断；
 * in表示接收到的Json格式的字段与其值；
 * inLen表示接收到的in的长度；
 * out表示保存服务字段值内容的指针,内存由厂商开辟，使用完成后，由Hilink Device SDK释放；
 * outLen表示读取到的payload的长度；
 * 返回0表示服务状态字段值获取成功，返回非0表示获取服务状态字段值不成功。
 */
int HILINK_GetCharState(const char *svcId, const char *in, unsigned int inLen, char **out, unsigned int *outLen)
{
    return 0;
}

/*
 * 功能: 获取SoftAp配网PIN码
 * 返回: 返回10000000到99999999之间的8位数字PIN码, 返回-1表示使用HiLink SDK的默认PIN码
 * 注意: (1)安全认证要求，PIN码不能由sn、mac等设备固定信息生成
 *       (2)该接口实现需要与devicepartner平台匹配
 */
int HILINK_GetPinCode(void)
{
    return -1;
}

/*
 * 通知设备的状态
 * status表示设备当前的状态
 * 注意，此函数由设备厂商根据产品业务选择性实现
 * 注意: (1) 此函数由设备厂商根据产品业务选择性实现
 *      (2) 禁止在HILINK_NotifyDevStatus接口内回调HiLink SDK的对外接口
 */
void HILINK_NotifyDevStatus(int status)
{
    HILINK_SAL_DEBUG("HILINK_NotifyDevStatus status: %d\r\n", status);
    switch (status) {
        case HILINK_M2M_CLOUD_OFFLINE:
            /* 设备与云端连接断开，请在此处添加实现 */
            break;
        case HILINK_M2M_CLOUD_ONLINE:
            /* 设备连接云端成功，请在此处添加实现 */
            break;
        case HILINK_M2M_LONG_OFFLINE:
            /* 设备与云端连接长时间断开，请在此处添加实现 */
            break;
        case HILINK_M2M_LONG_OFFLINE_REBOOT:
            /* 设备与云端连接长时间断开后进行重启，请在此处添加实现 */
            break;
        case HILINK_UNINITIALIZED:
            /* HiLink线程未启动，请在此处添加实现 */
            break;
        case HILINK_LINK_UNDER_AUTO_CONFIG:
            /* 设备处于配网模式，请在此处添加实现 */
            break;
        case HILINK_LINK_CONFIG_TIMEOUT:
            /* 设备处于10分钟超时状态，请在此处添加实现 */
            break;
        case HILINK_LINK_CONNECTTING_WIFI:
            /* 设备正在连接路由器，请在此处添加实现 */
            break;
        case HILINK_LINK_CONNECTED_WIFI:
            /* 设备已经连上路由器，请在此处添加实现 */
            break;
        case HILINK_M2M_CONNECTTING_CLOUD:
            /* 设备正在连接云端，请在此处添加实现 */
            break;
        case HILINK_LINK_DISCONNECT:
            /* 设备与路由器的连接断开，请在此处添加实现 */
            break;
        case HILINK_DEVICE_REGISTERED:
            /* 设备被注册，请在此处添加实现 */
            break;
        case HILINK_DEVICE_UNREGISTER:
            /* 设备被解绑，请在此处添加实现 */
            break;
        case HILINK_REVOKE_FLAG_SET:
            /* 设备被复位标记置位，请在此处添加实现 */
            break;
        case HILINK_NEGO_REG_INFO_FAIL:
            /* 设备协商配网信息失败 */
            break;
        case HILINK_LINK_CONNECTED_FAIL:
            /* 设备与路由器的连接失败 */
            break;
        case HILINK_CLOUD_TO_CENTRAL:
            /* 直连云模式切成中枢模式 */
            break;
        case HILINK_DEVICE_CLEAN:
            /* 设备完全清除，包括前装跟后装信息 */
            break;
        default:
            break;
    }

    return;
}

 /*
 * 功能：实现模组重启前的设备操作
 * 参数：flag 入参，触发重启的类型
 *          0表示HiLink SDK 线程看门狗触发模组重启;
 *          1表示APP删除设备触发模组重启;
 *          2表示设备长时间离线无法恢复而重启;
 * 返回值：0表示处理成功, 系统可以重启，使用硬重启;
 *         1表示处理成功, 系统可以重启，如果通过HILINK_SetSdkAttr()注册了软重启(sdkAttr.rebootSoftware），使用软重启;
 *         负值表示处理失败，系统不能重启
 * 注意：(1) 此函数由设备厂商实现；
 *       (2) 若APP删除设备触发模组重启时，设备操作完务必返回0，否则会导致删除设备异常；
 *       (3) 设备长时间离线无法恢复而重启，应对用户无感，不可影响用户体验，否则不可以重启；
 */
int HILINK_ProcessBeforeRestart(int flag)
{
    /* HiLink SDK线程看门狗超时触发模组重启 */
    if (flag == 0) {
        /* 实现模组重启前的操作(如:保存系统状态等) */
        return -1;
    }

    /* APP删除设备触发模组重启 */
    if (flag == 1) {
        /* 实现模组重启前的操作(如:保存系统状态等) */
        return 1;
    }

    /* 设备长时间离线触发模组重启，尝试恢复网络 */
    if (flag == 2) {
        /* 实现模组重启前的操作(如:保存系统状态等) */
        return -1;
    }

    return -1;
}
