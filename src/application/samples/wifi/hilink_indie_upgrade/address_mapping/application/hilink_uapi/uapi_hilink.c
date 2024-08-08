  /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: HiLink function adaption \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */
#include "hilink_call.h"
#include "hilink.h"

int HILINK_RegisterBaseCallback(const HiLinkBaseCallback *cb, unsigned int cbSize)
{
    return hilink_call2(HILINK_CALL_HILINK_REGISTER_BASE_CALLBACK, int,
        const HiLinkBaseCallback *, cb, unsigned int, cbSize);
}

int HILINK_Main(void)
{
    return hilink_call0(HILINK_CALL_HILINK_MAIN, int);
}

void HILINK_Reset(void)
{
    hilink_call0(HILINK_CALL_HILINK_RESET, int);
}

int HILINK_SetSdkAttr(HILINK_SdkAttr sdkAttr)
{
    return hilink_call1(HILINK_CALL_HILINK_SET_SDK_ATTR, int, HILINK_SdkAttr, sdkAttr);
}

HILINK_SdkAttr *HILINK_GetSdkAttr(void)
{
    return hilink_call0(HILINK_CALL_HILINK_GET_SDK_ATTR, HILINK_SdkAttr *);
}

int HILINK_RestoreFactorySettings(void)
{
    return hilink_call0(HILINK_CALL_HILINK_RESTORE_FACTORY_SETTINGS, int);
}

int HILINK_GetDevStatus(void)
{
    return hilink_call0(HILINK_CALL_HILINK_GET_DEV_STATUS, int);
}

const char *HILINK_GetSdkVersion(void)
{
    return hilink_call0(HILINK_CALL_HILINK_GET_SDK_VERSION, const char *);
}

int HILINK_ReportCharState(const char *svcId, const char *payload, unsigned int len)
{
    return hilink_call3(HILINK_CALL_HILINK_REPORT_CHAR_STATE, int,
        const char *, svcId, const char *, payload, unsigned int, len);
}

int HILINK_IsRegister(void)
{
    return hilink_call0(HILINK_CALL_HILINK_IS_REGISTER, int);
}

int HILINK_GetNetworkingMode(void)
{
    return hilink_call0(HILINK_CALL_HILINK_GET_NETWORKING_MODE, int);
}

int HILINK_GetRegisterStatus(void)
{
    return hilink_call0(HILINK_CALL_HILINK_GET_REGISTER_STATUS, int);
}

int HILINK_SetScheduleInterval(unsigned long interval)
{
    return hilink_call1(HILINK_CALL_HILINK_SET_SCHEDULE_INTERVAL, int, unsigned long, interval);
}

int HILINK_SetMonitorScheduleInterval(unsigned long interval)
{
    return hilink_call1(HILINK_CALL_HILINK_SET_MONITOR_SCHEDULE_INTERVAL, int, unsigned long, interval);
}

int HILINK_SetNetConfigMode(enum HILINK_NetConfigMode netConfigMode)
{
    return hilink_call1(HILINK_CALL_HILINK_SET_NET_CONFIG_MODE, int, enum HILINK_NetConfigMode, netConfigMode);
}

enum HILINK_NetConfigMode HILINK_GetNetConfigMode(void)
{
    return hilink_call0(HILINK_CALL_HILINK_GET_NET_CONFIG_MODE, enum HILINK_NetConfigMode);
}

void HILINK_SetNetConfigTimeout(unsigned long netConfigTimeout)
{
    hilink_call1(HILINK_CALL_HILINK_SET_NET_CONFIG_TIMEOUT, void, unsigned long, netConfigTimeout);
}

int HILINK_SetOtaBootTime(unsigned int bootTime)
{
    return hilink_call1(HILINK_CALL_HILINK_SET_OTA_BOOT_TIME, int, unsigned int, bootTime);
}

void HILINK_EnableKitframework(void)
{
    hilink_call0(HILINK_CALL_HILINK_ENABLE_KITFRAMEWORK, void);
}

void HILINK_EnableBatchControl(bool flag)
{
    hilink_call1(HILINK_CALL_HILINK_ENABLE_BATCH_CONTROL, void, bool, flag);
}

void HILINK_EnableProcessDelErrCode(int enable)
{
    hilink_call1(HILINK_CALL_HILINK_ENABLE_PROCESS_DEL_ERR_CODE, void, int, enable);
}

void HILINK_UnbindDevice(int type)
{
    hilink_call1(HILINK_CALL_HILINK_UNBIND_DEVICE, void, int, type);
}

int HILINK_SetDeviceInstallType(int type)
{
    return hilink_call1(HILINK_CALL_HILINK_SET_DEVICE_INSTALL_TYPE, int, int, type);
}

SetupType HILINK_GetDevSetupType(void)
{
    return hilink_call0(HILINK_CALL_HILINK_GET_DEV_SETUP_TYPE, SetupType);
}

void HILINK_EnableDevIdInherit(bool isEnbale)
{
    hilink_call1(HILINK_CALL_HILINK_ENABLE_DEV_ID_INHERIT, void, bool, isEnbale);
}

void HILINK_NotifyNetworkAvailable(bool status)
{
    hilink_call1(HILINK_CALL_HILINK_NOTIFY_NETWORK_AVAILABLE, void, bool, status);
}
