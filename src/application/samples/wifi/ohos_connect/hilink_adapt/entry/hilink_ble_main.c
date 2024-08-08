/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: 蓝牙SDK提供demo示例代码（此文件为DEMO，需集成方适配修改）
 */
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "hilink_bt_api.h"
#include "hilink_bt_function.h"
#include "ble_cfg_net_api.h"
#include "hilink_device.h"

#include "ohos_bt_gatt.h"
#include "ohos_bt_gatt_server.h"
#include "cmsis_os2.h"
#include "hilink_socket_adapter.h"

/*
 * 设备基本信息根据设备实际情况填写
 * 与hilink sdk相同定义，双模组模式只需一份，已提供给第三方厂家，暂不按编程规范整改
 */
#define DEVICE_SN "113GFF01239F0066"
#define PRODUCT_ID "2LWU"
#define DEVICE_MODEL "IH6SC800S3"
#define DEVICE_TYPE "008"
#define MANUAFACTURER "04D"
#define DEVICE_MAC "607D0968B970"
#define DEVICE_HIVERSION "1.0.0"
#define DEVICE_PROT_TYPE 1
#define DEVICE_TYPE_NAME "WiFiCamera"
#define MANUAFACTURER_NAME "HW"

/* 蓝牙sdk单独使用的定义 */
#define SUB_PRODUCT_ID "00"
#define ADV_TX_POWER 0xF8
#define BLE_ADV_TIME 60

/* 厂商自定义蓝牙广播，设备型号信息 */
#define BT_CUSTOM_INFO "12345678"
#define DEVICE_MANU_ID "017"

#define min_len(a, b) (((a) < (b)) ? (a) : (b))

static HILINK_BT_DevInfo g_btDevInfo;

/*
 * 功能: 获取设备sn号
 * 参数[in]: len sn的最大长度, 39字节
 * 参数[out]: sn 设备sn
 * 注意: sn指针的字符串长度为0时将使用设备mac地址作为sn
 */
void HILINK_GetDeviceSn(unsigned int len, char *sn)
{
    if (sn == NULL) {
        return;
    }
    const char *ptr = DEVICE_SN;
    int tmp = min_len(len, sizeof(DEVICE_SN));
    for (int i = 0; i < tmp; i++) {
        sn[i] = ptr[i];
    }
    return;
}

/*
 * 获取设备的子型号，长度固定两个字节
 * subProdId为保存子型号的缓冲区，len为缓冲区的长度
 * 如果产品定义有子型号，则填入两字节子型号，并以'\0'结束, 返回0
 * 没有定义子型号，则返回-1
 * 该接口需设备开发者实现
 */
int HILINK_GetSubProdId(char *subProdId, int len)
{
    if (subProdId == NULL) {
        return -1;
    }
    const char *ptr = SUB_PRODUCT_ID;
    int tmp = min_len((unsigned int)len, sizeof(SUB_PRODUCT_ID));
    for (int i = 0; i < tmp; i++) {
        subProdId[i] = ptr[i];
    }
    return 0;
}

/*
 * 获取设备表面的最强点信号发射功率强度，最强点位置的确定以及功率测试方
 * 法，参照hilink认证蓝牙靠近发现功率设置及测试方法指导文档，power为出参,
 * 单位dbm，下一次发送广播时生效
 */
int HILINK_BT_GetDevSurfacePower(char *power)
{
    if (power == NULL) {
        return -1;
    }
    *power = ADV_TX_POWER;
    return 0;
}

/* 获取蓝牙SDK设备相关信息 */
HILINK_BT_DevInfo *HILINK_BT_GetDevInfo(void)
{
    printf("HILINK_BT_GetDevInfo\n");
    g_btDevInfo.manuName = MANUAFACTURER;
    g_btDevInfo.devName = DEVICE_TYPE_NAME;
    g_btDevInfo.productId = PRODUCT_ID;
    g_btDevInfo.mac = DEVICE_MAC;
    g_btDevInfo.model = DEVICE_MODEL;
    g_btDevInfo.devType = DEVICE_TYPE;
    g_btDevInfo.hiv = DEVICE_HIVERSION;
    g_btDevInfo.protType = DEVICE_PROT_TYPE;
    g_btDevInfo.sn = DEVICE_SN;
    return &g_btDevInfo;
}
/*
 * 若厂商发送广播类型为BLE_ADV_CUSTOM时才需适配此接口
 * 获取厂商定制信息，由厂家实现
 * 返回0表示获取成功，返回其他表示获取失败
 */
int HILINK_GetCustomInfo(char *customInfo, unsigned int len)
{
    if (customInfo == NULL || len == 0) {
        return -1;
    }

    const char *ptr = BT_CUSTOM_INFO;
    int tmp = min_len(len, strlen(BT_CUSTOM_INFO));
    for (int i = 0; i < tmp; i++) {
        customInfo[i] = ptr[i];
    }

    return 0;
}

/*
 * 若厂商发送广播类型为BLE_ADV_CUSTOM时才需适配此接口
 * 获取厂家ID，由厂家实现
 * 返回0表示获取成功，返回其他表示获取失败
 */
int HILINK_GetManuId(char *manuId, unsigned int len)
{
    if (manuId == NULL || len == 0) {
        return -1;
    }

    const char *ptr = DEVICE_MANU_ID;
    int tmp = min_len(len, strlen(DEVICE_MANU_ID));
    for (int i = 0; i < tmp; i++) {
        manuId[i] = ptr[i];
    }

    return 0;
}

/*
 * 获取蓝牙mac地址，由厂家实现
 * 返回0表示获取成功，返回其他表示获取失败
 */
int HILINK_BT_GetMacAddr(unsigned char *mac, unsigned int len)
{
    (void)mac;
    (void)len;
    return 0;
}

/* 填写固件、软件和硬件版本号，APP显示版本号以及OTA版本检查与此相关 */
int getDeviceVersion(char* *firmwareVer, char* *softwareVer, char* *hardwareVer)
{
    *firmwareVer = "1.0.0";
    *softwareVer = "1.1.0";
    *hardwareVer = "1.1.1";
    return 0;
}

static void HILINK_BT_StateChangeHandler(HILINK_BT_SdkStatus event, const void *param)
{
    (void)param;
    /* ble sdk初始化完成后，发送广播让设备被手机发现 */
    if (event == HILINK_BT_SDK_STATUS_SVC_RUNNING) {
        /* 设置蓝牙广播格式，包括靠近发现、碰一碰等，下一次发送广播生效 */
        BLE_SetAdvType(BLE_ADV_DEFAULT);

        /* BLE配网广播控制：参数代表广播时间，0:停止；0xFFFFFFFF:一直广播，其他：广播指定时间后停止，单位秒 */
        (void)BLE_CfgNetAdvCtrl(BLE_ADV_TIME);
    }
}

static BLE_ConfPara g_isBlePair = {
    .isBlePair = 0,
};

static BLE_InitPara g_bleInitParam = {
    .confPara = &g_isBlePair,
    /* advInfo为空表示使用ble sdk默认广播参数及数据 */
    .advInfo  = NULL,
    .gattList = NULL,
};

/* APP下发自定义指令时调用此函数，需处理自定义数据，返回0表示处理成功 */
static int BleRcvCustomData(unsigned char *buff, unsigned int len)
{
    printf("custom data, len: %u, data: %s\r\n", len, buff);
    /* 处理自定义数据 */
    return 0;
}

static BLE_CfgNetCb g_bleCfgNetCb = {
    .rcvCustomDataCb = BleRcvCustomData,
};

int hilink_ble_main(void)
{
    /* 设备按需设置，例如接入蓝牙网关时，设置广播类型标志及心跳间隔 */
    unsigned char mpp[] = {0x02, 0x3c, 0x00};
    int ret = BLE_SetAdvNameMpp(mpp, sizeof(mpp));
    if (ret != 0) {
        printf("set adv name mpp failed\r\n");
        return -1;
    }

    ret = HILINK_SetNetConfigMode(HILINK_NETCONFIG_OTHER);
    if (ret != 0) {
        printf("SetNetConfigMode failed\r\n");
        return -1;
    }

    /* 注册SDK状态接收函数，可在初始化完成后发送广播 */
    ret = HILINK_BT_SetSdkEventCallback(HILINK_BT_StateChangeHandler);
    if (ret != 0) {
        printf("set event callback failed\r\n");
        return -1;
    }

    /* 初始化ble sdk */
    ret = BLE_CfgNetInit(&g_bleInitParam, &g_bleCfgNetCb);
    if (ret != 0) {
        printf("ble sdk init fail\r\n");
        return -1;
    }

    /* 修改任务属性 */
    HILINK_SdkAttr *sdkAttr = HILINK_GetSdkAttr();
    if (sdkAttr == NULL) {
        printf("sdkAttr is null");
        return -1;
    }
    sdkAttr->monitorTaskStackSize = 0x400;  /* 示例代码 推荐栈大小为0x400 */
    HILINK_SetSdkAttr(*sdkAttr);

#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
    if (HILINK_RegisterErrnoCallback(get_os_errno) != 0) {
        printf("reg errno cb err\r\n");
    }
#endif

    /* 启动Hilink SDK */
    if (HILINK_Main() != 0) {
        printf("HILINK_Main start error");
    }

    return 0;
}