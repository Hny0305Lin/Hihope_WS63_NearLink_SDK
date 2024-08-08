/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: hilink function mapping. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "hilink_function_mapping.h"
#include "func_call_list.h"
#include "hilink.h"
#include "hilink_log_manage.h"
#include "hilink_device.h"
#include "ble_cfg_net_api.h"
#include "hilink_bt_function.h"
#include "hilink_network_adapter.h"
#include "hilink_socket_adapter.h"

static int *g_app_tbl;
const void *g_hilink_call_tbl[HILINK_CALL_MAX] __attribute__ ((section(".hilink_tbl"))) = {
    [HILINK_CALL_HILINK_REGISTER_BASE_CALLBACK]         = HILINK_RegisterBaseCallback,
    [HILINK_CALL_HILINK_MAIN]                           = HILINK_Main,
    [HILINK_CALL_HILINK_RESET]                          = HILINK_Reset,
    [HILINK_CALL_HILINK_SET_SDK_ATTR]                   = HILINK_SetSdkAttr,
    [HILINK_CALL_HILINK_GET_SDK_ATTR]                   = HILINK_GetSdkAttr,
    [HILINK_CALL_HILINK_RESTORE_FACTORY_SETTINGS]       = HILINK_RestoreFactorySettings,
    [HILINK_CALL_HILINK_GET_DEV_STATUS]                 = HILINK_GetDevStatus,
    [HILINK_CALL_HILINK_GET_SDK_VERSION]                = HILINK_GetSdkVersion,
    [HILINK_CALL_HILINK_REPORT_CHAR_STATE]              = HILINK_ReportCharState,
    [HILINK_CALL_HILINK_IS_REGISTER]                    = HILINK_IsRegister,
    [HILINK_CALL_HILINK_GET_NETWORKING_MODE]            = HILINK_GetNetworkingMode,
    [HILINK_CALL_HILINK_GET_REGISTER_STATUS]            = HILINK_GetRegisterStatus,
    [HILINK_CALL_HILINK_SET_SCHEDULE_INTERVAL]          = HILINK_SetScheduleInterval,
    [HILINK_CALL_HILINK_SET_MONITOR_SCHEDULE_INTERVAL]  = HILINK_SetMonitorScheduleInterval,
    [HILINK_CALL_HILINK_SET_NET_CONFIG_MODE]            = HILINK_SetNetConfigMode,
    [HILINK_CALL_HILINK_GET_NET_CONFIG_MODE]            = HILINK_GetNetConfigMode,
    [HILINK_CALL_HILINK_SET_NET_CONFIG_TIMEOUT]         = HILINK_SetNetConfigTimeout,
    [HILINK_CALL_HILINK_SET_OTA_BOOT_TIME]              = HILINK_SetOtaBootTime,
    [HILINK_CALL_HILINK_ENABLE_KITFRAMEWORK]            = HILINK_EnableKitframework,
    [HILINK_CALL_HILINK_ENABLE_BATCH_CONTROL]           = HILINK_EnableBatchControl,
    [HILINK_CALL_HILINK_ENABLE_PROCESS_DEL_ERR_CODE]    = HILINK_EnableProcessDelErrCode,
    [HILINK_CALL_HILINK_UNBIND_DEVICE]                  = HILINK_UnbindDevice,
    [HILINK_CALL_HILINK_SET_DEVICE_INSTALL_TYPE]        = HILINK_SetDeviceInstallType,
    [HILINK_CALL_HILINK_GET_DEV_SETUP_TYPE]             = HILINK_GetDevSetupType,
    [HILINK_CALL_HILINK_ENABLE_DEV_ID_INHERIT]          = HILINK_EnableDevIdInherit,
    [HILINK_CALL_HILINK_NOTIFY_NETWORK_AVAILABLE]       = HILINK_NotifyNetworkAvailable,
    [HILINK_CALL_HILINK_SET_LOG_LEVEL]                  = HILINK_SetLogLevel,
    [HILINK_CALL_HILINK_GET_LOG_LEVEL]                  = HILINK_GetLogLevel,
    [HILINK_CALL_HILINK_REGISTER_GET_AC_V2_FUNC]        = HILINK_RegisterGetAcV2Func,
    [HILINK_CALL_BLE_CFG_NET_INIT]                      = BLE_CfgNetInit,
    [HILINK_CALL_BLE_CFG_NET_DE_INIT]                   = BLE_CfgNetDeInit,
    [HILINK_CALL_BLE_CFG_NET_ADV_CTRL]                  = BLE_CfgNetAdvCtrl,
    [HILINK_CALL_BLE_CFG_NET_ADV_UPDATE]                = BLE_CfgNetAdvUpdate,
    [HILINK_CALL_BLE_CFG_NET_DIS_CONNECT]               = BLE_CfgNetDisConnect,
    [HILINK_CALL_BLE_SEND_CUSTOM_DATA]                  = BLE_SendCustomData,
    [HILINK_CALL_BLE_GET_ADV_TYPE]                      = BLE_GetAdvType,
    [HILINK_CALL_BLE_SET_ADV_TYPE]                      = BLE_SetAdvType,
    [HILINK_CALL_BLE_SET_ADV_NAME_MPP]                  = BLE_SetAdvNameMpp,
    [HILINK_CALL_BLE_NEAR_DISCOVERY_INIT]               = BLE_NearDiscoveryInit,
    [HILINK_CALL_BLE_NEAR_DISCOVERY_ENABLE]             = BLE_NearDiscoveryEnable,
    [HILINK_CALL_HILINK_BT_GET_TASK_STACK_SIZE]         = HILINK_BT_GetTaskStackSize,
    [HILINK_CALL_HILINK_BT_SET_TASK_STACK_SIZE]         = HILINK_BT_SetTaskStackSize,
    [HILINK_CALL_HILINK_BT_SET_SDK_EVENT_CALLBACK]      = HILINK_BT_SetSdkEventCallback,
    [HILINK_CALL_HILINK_REG_WIFI_RECOVERY_CALLBACK]     = HILINK_RegWiFiRecoveryCallback,
    [HILINK_CALL_HILINK_REG_ERRNO_CALLBACK]             = HILINK_RegisterErrnoCallback,
};

int *get_app_tbl(void)
{
    return g_app_tbl;
}

/* copy ram */
static void copy_bin_to_ram(unsigned int *start_addr,
    const unsigned int *const load_addr, unsigned int size)
{
    unsigned int i;

    for (i = 0; i < size / sizeof(unsigned int); i++) {
        *(start_addr + i) = *(load_addr + i);
    }
}

/* init ram value */
static void init_mem_value(unsigned int *start_addr,
    const unsigned int *const end_addr, unsigned int init_val)
{
    unsigned int *dest = start_addr;

    while (dest < end_addr) {
        *dest = init_val;
        dest++;
    }
}

static void hilink_info_entry(void **hilink_call_tbl, void *app_call_tbl)
{
#ifndef CHIP_EDA
    /* copy sram_text from flash to SRAM */
    copy_bin_to_ram(&__sram_text_begin__, &__sram_text_load__, (unsigned int)&__sram_text_size__);
    /* copy data from flash to SRAM */
    copy_bin_to_ram(&__data_begin__, &__data_load__, (unsigned int)&__data_size__);
    /* clear bss on SRAM */
    init_mem_value(&__bss_begin__, &__bss_end__, 0);
#endif

    *hilink_call_tbl = g_hilink_call_tbl;
    g_app_tbl = app_call_tbl;
}

__attribute__ ((used, section(".hilink_info"))) struct hilink_info_stru hilink_info = {
    hilink_info_entry,
};
