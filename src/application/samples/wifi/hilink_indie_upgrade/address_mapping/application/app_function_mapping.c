 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Records the address mapping table of the function adapted to the app.bin file. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_function_mapping.h"
#include "func_call_list.h"
#include "hilink_kv_adapter.h"
#include "hilink_mbedtls_utils.h"
#include "hilink_mem_adapter.h"
#include "hilink_network_adapter.h"
#include "hilink_open_ota_adapter.h"
#include "hilink_open_ota_mcu_adapter.h"
#include "hilink_sal_aes.h"
#include "hilink_sal_base64.h"
#include "hilink_sal_defines.h"
#include "hilink_sal_drbg.h"
#include "hilink_sal_kdf.h"
#include "hilink_sal_md.h"
#include "hilink_sal_mpi.h"
#include "hilink_sal_rsa.h"
#include "hilink_socket_adapter.h"
#include "hilink_softap_adapter.h"
#include "hilink_stdio_adapter.h"
#include "hilink_str_adapter.h"
#include "hilink_sys_adapter.h"
#include "hilink_thread_adapter.h"
#include "hilink_time_adapter.h"
#include "hilink_tls_client.h"
#include "hilink_device.h"
#include "hichain.h"
#include "ohos_bt_gatt.h"
#include "ohos_bt_def.h"
#include "ohos_bt_gatt_server.h"
#include "hilink_bt_api.h"
#include "hilink_bt_function.h"
#include "cmsis_os2.h"
#include "ohos_bt_gap.h"

static int *g_hilink_tbl;
static struct hilink_info_stru *g_hilink_info = (struct hilink_info_stru *)&hilink_info_addr;
static const void *g_app_call_tbl[APP_CALL_MAX] = {
    [APP_CALL_HILINK_KV_STORE_INIT]                     = HILINK_KVStoreInit,
    [APP_CALL_HILINK_SET_VALUE]                         = HILINK_SetValue,
    [APP_CALL_HILINK_GET_VALUE]                         = HILINK_GetValue,
    [APP_CALL_HILINK_DELETE_VALUE]                      = HILINK_DeleteValue,
    [APP_CALL_HILINK_GET_FILE_NAME]                     = HILINK_GetFileName,
    [APP_CALL_HILINK_MALLOC]                            = HILINK_Malloc,
    [APP_CALL_HILINK_FREE]                              = HILINK_Free,
    [APP_CALL_HILINK_MEMCMP]                            = HILINK_Memcmp,
    [APP_CALL_HILINK_GET_ADDR_INFO]                     = HILINK_GetAddrInfo,
    [APP_CALL_HILINK_FREE_ADDR_INFO]                    = HILINK_FreeAddrInfo,
    [APP_CALL_HILINK_SOCKET]                            = HILINK_Socket,
    [APP_CALL_HILINK_CLOSE]                             = HILINK_Close,
    [APP_CALL_HILINK_SET_SOCKET_OPT]                    = HILINK_SetSocketOpt,
    [APP_CALL_HILINK_BIND]                              = HILINK_Bind,
    [APP_CALL_HILINK_CONNECT]                           = HILINK_Connect,
    [APP_CALL_HILINK_RECV]                              = HILINK_Recv,
    [APP_CALL_HILINK_SEND]                              = HILINK_Send,
    [APP_CALL_HILINK_RECV_FROM]                         = HILINK_RecvFrom,
    [APP_CALL_HILINK_SEND_TO]                           = HILINK_SendTo,
    [APP_CALL_HILINK_SELECT]                            = HILINK_Select,
    [APP_CALL_HILINK_GET_SOCKET_ERRNO]                  = HILINK_GetSocketErrno,
    [APP_CALL_HILINK_HTONL]                             = HILINK_Htonl,
    [APP_CALL_HILINK_NTOHL]                             = HILINK_Ntohl,
    [APP_CALL_HILINK_HTONS]                             = HILINK_Htons,
    [APP_CALL_HILINK_NTOHS]                             = HILINK_Ntohs,
    [APP_CALL_HILINK_INET_ATON]                         = HILINK_InetAton,
    [APP_CALL_HILINK_INET_ADDR]                         = HILINK_InetAddr,
    [APP_CALL_HILINK_INET_NTOA]                         = HILINK_InetNtoa,
    [APP_CALL_HILINK_VPRINTF]                           = HILINK_Vprintf,
    [APP_CALL_HILINK_PRINTF]                            = HILINK_Printf,
    [APP_CALL_HILINK_RAND]                              = HILINK_Rand,
    [APP_CALL_HILINK_TRNG]                              = HILINK_Trng,
    [APP_CALL_HILINK_STRLEN]                            = HILINK_Strlen,
    [APP_CALL_HILINK_STRCHR]                            = HILINK_Strchr,
    [APP_CALL_HILINK_STRRCHR]                           = HILINK_Strrchr,
    [APP_CALL_HILINK_ATOI]                              = HILINK_Atoi,
    [APP_CALL_HILINK_STRSTR]                            = HILINK_Strstr,
    [APP_CALL_HILINK_STRCMP]                            = HILINK_Strcmp,
    [APP_CALL_HILINK_STRNCMP]                           = HILINK_Strncmp,
    [APP_CALL_HILINK_CREATE_TASK]                       = HILINK_CreateTask,
    [APP_CALL_HILINK_THREAD_SUSPEND]                    = HILINK_ThreadSuspend,
    [APP_CALL_HILINK_THREAD_RESUME]                     = HILINK_ThreadResume,
    [APP_CALL_HILINK_DELETE_TASK]                       = HILINK_DeleteTask,
    [APP_CALL_HILINK_GET_CURRENT_TASK_ID]               = HILINK_GetCurrentTaskId,
    [APP_CALL_HILINK_MUTEX_CREATE]                      = HILINK_MutexCreate,
    [APP_CALL_HILINK_MUTEX_LOCK]                        = HILINK_MutexLock,
    [APP_CALL_HILINK_MUTEX_UNLOCK]                      = HILINK_MutexUnlock,
    [APP_CALL_HILINK_MUTEX_DESTROY]                     = HILINK_MutexDestroy,
    [APP_CALL_HILINK_SEM_CREATE]                        = HILINK_SemCreate,
    [APP_CALL_HILINK_SEM_WAIT]                          = HILINK_SemWait,
    [APP_CALL_HILINK_SEM_POST]                          = HILINK_SemPost,
    [APP_CALL_HILINK_SEM_DESTROY]                       = HILINK_SemDestroy,
    [APP_CALL_HILINK_MILLI_SLEEP]                       = HILINK_MilliSleep,
    [APP_CALL_HILINK_SCHED_YIELD]                       = HILINK_SchedYield,
    [APP_CALL_HILINK_GET_OS_TIME]                       = HILINK_GetOsTime,
    [APP_CALL_HILINK_GET_UTC_TIME]                      = HILINK_GetUtcTime,
    [APP_CALL_HILINK_OTA_ADAPTER_FLASH_INIT]            = HILINK_OtaAdapterFlashInit,
    [APP_CALL_HILINK_OTA_ADAPTER_GET_UPDATE_INDEX]      = HILINK_OtaAdapterGetUpdateIndex,
    [APP_CALL_HILINK_OTA_ADAPTER_FLASH_ERASE]           = HILINK_OtaAdapterFlashErase,
    [APP_CALL_HILINK_OTA_ADAPTER_FLASH_WRITE]           = HILINK_OtaAdapterFlashWrite,
    [APP_CALL_HILINK_OTA_ADAPTER_FLASH_READ]            = HILINK_OtaAdapterFlashRead,
    [APP_CALL_HILINK_OTA_ADAPTER_FLASH_FINISH]          = HILINK_OtaAdapterFlashFinish,
    [APP_CALL_HILINK_OTA_ADAPTER_FLASH_MAX_SIZE]        = HILINK_OtaAdapterFlashMaxSize,
    [APP_CALL_HILINK_OTA_ADAPTER_RESTART]               = HILINK_OtaAdapterRestart,
    [APP_CALL_HILINK_OTA_START_PROCESS]                 = HILINK_OtaStartProcess,
    [APP_CALL_HILINK_OTA_END_PROCESS]                   = HILINK_OtaEndProcess,
    [APP_CALL_HILINK_GET_REBOOT_FLAG]                   = HILINK_GetRebootFlag,
    [APP_CALL_HILINK_GET_MCU_VERSION]                   = HILINK_GetMcuVersion,
    [APP_CALL_HILINK_NOTIFY_OTA_STATUS]                 = HILINK_NotifyOtaStatus,
    [APP_CALL_HILINK_NOTIFY_OTA_DATA]                   = HILINK_NotifyOtaData,
    [APP_CALL_HILINK_RESTART]                           = HILINK_Restart,
    [APP_CALL_HILINK_GET_SYSTEM_BOOT_REASON]            = HILINK_GetSystemBootReason,
    [APP_CALL_HILINK_SAL_RSA_INIT]                      = HILINK_SAL_RsaInit,
    [APP_CALL_HILINK_SAL_RSA_FREE]                      = HILINK_SAL_RsaFree,
    [APP_CALL_HILINK_SAL_RSA_PARAM_IMPORT]              = HILINK_SAL_RsaParamImport,
    [APP_CALL_HILINK_RSA_PKCS1_VERIFY]                  = HILINK_RsaPkcs1Verify,
    [APP_CALL_HILINK_RSA_PKCS1_DECRYPT]                 = HILINK_RsaPkcs1Decrypt,
    [APP_CALL_HILINK_RSA_PKCS1_ENCRYPT]                 = HILINK_RsaPkcs1Encrypt,
    [APP_CALL_HILINK_TLS_CLIENT_CREATE]                 = HILINK_TlsClientCreate,
    [APP_CALL_HILINK_SET_TLS_CLIENT_OPTION]             = HILINK_SetTlsClientOption,
    [APP_CALL_HILINK_TLS_CLIENT_CONNECT]                = HILINK_TlsClientConnect,
    [APP_CALL_HILINK_TLS_CLIENT_GET_CONTEXT_FD]         = HILINK_TlsClientGetContextFd,
    [APP_CALL_HILINK_TLS_CLIENT_READ]                   = HILINK_TlsClientRead,
    [APP_CALL_HILINK_TLS_CLIENT_WRITE]                  = HILINK_TlsClientWrite,
    [APP_CALL_HILINK_TLS_CLIENT_IS_VALID_CERT]          = HILINK_TlsClientIsValidCert,
    [APP_CALL_HILINK_TLS_CLIENT_FREE_RESOURCE]          = HILINK_TlsClientFreeResource,
    [APP_CALL_HILINK_SAL_AES_GCM_ENCRYPT]               = HILINK_SAL_AesGcmEncrypt,
    [APP_CALL_HILINK_SAL_AES_GCM_DECRYPT]               = HILINK_SAL_AesGcmDecrypt,
    [APP_CALL_HILINK_SAL_ADD_PADDING]                   = HILINK_SAL_AddPadding,
    [APP_CALL_HILINK_SAL_GET_PADDING]                   = HILINK_SAL_GetPadding,
    [APP_CALL_HILINK_SAL_AES_CBC_ENCRYPT]               = HILINK_SAL_AesCbcEncrypt,
    [APP_CALL_HILINK_SAL_AES_CBC_DECRYPT]               = HILINK_SAL_AesCbcDecrypt,
    [APP_CALL_HILINK_SAL_AES_CCM_ENCRYPT]               = HILINK_SAL_AesCcmEncrypt,
    [APP_CALL_HILINK_SAL_AES_CCM_DECRYPT]               = HILINK_SAL_AesCcmDecrypt,
    [APP_CALL_HILINK_SAL_BASE64_ENCODE]                 = HILINK_SAL_Base64Encode,
    [APP_CALL_HILINK_SAL_BASE64_DECODE]                 = HILINK_SAL_Base64Decode,
    [APP_CALL_HILINK_SAL_DRBG_INIT]                     = HILINK_SAL_DrbgInit,
    [APP_CALL_HILINK_SAL_DRBG_DEINIT]                   = HILINK_SAL_DrbgDeinit,
    [APP_CALL_HILINK_SAL_DRBG_RANDOM]                   = HILINK_SAL_DrbgRandom,
    [APP_CALL_HILINK_SAL_HKDF]                          = HILINK_SAL_Hkdf,
    [APP_CALL_HILINK_SAL_PKCS5_PBKDF2_HMAC]             = HILINK_SAL_Pkcs5Pbkdf2Hmac,
    [APP_CALL_HILINK_SAL_MD_CALC]                       = HILINK_SAL_MdCalc,
    [APP_CALL_HILINK_SAL_HMAC_CALC]                     = HILINK_SAL_HmacCalc,
    [APP_CALL_HILINK_SAL_MD_INIT]                       = HILINK_SAL_MdInit,
    [APP_CALL_HILINK_SAL_MD_UPDATE]                     = HILINK_SAL_MdUpdate,
    [APP_CALL_HILINK_SAL_MD_FINISH]                     = HILINK_SAL_MdFinish,
    [APP_CALL_HILINK_SAL_MD_FREE]                       = HILINK_SAL_MdFree,
    [APP_CALL_HILINK_SAL_MPI_INIT]                      = HILINK_SAL_MpiInit,
    [APP_CALL_HILINK_SAL_MPI_FREE]                      = HILINK_SAL_MpiFree,
    [APP_CALL_HILINK_SAL_MPI_EXP_MOD]                   = HILINK_SAL_MpiExpMod,
    [APP_CALL_HILINK_SAL_MPI_CMP_INT]                   = HILINK_SAL_MpiCmpInt,
    [APP_CALL_HILINK_SAL_MPI_SUB_INT]                   = HILINK_SAL_MpiSubInt,
    [APP_CALL_HILINK_SAL_MPI_CMP_MPI]                   = HILINK_SAL_MpiCmpMpi,
    [APP_CALL_HILINK_SAL_MPI_READ_STRING]               = HILINK_SAL_MpiReadString,
    [APP_CALL_HILINK_SAL_MPI_WRITE_STRING]              = HILINK_SAL_MpiWriteString,
    [APP_CALL_HILINK_SAL_MPI_READ_BINARY]               = HILINK_SAL_MpiReadBinary,
    [APP_CALL_HILINK_SAL_MPI_WRITE_BINARY]              = HILINK_SAL_MpiWriteBinary,
    [APP_CALL_HILINK_GET_LOCAL_IP]                      = HILINK_GetLocalIp,
    [APP_CALL_HILINK_GET_MAC_ADDR]                      = HILINK_GetMacAddr,
    [APP_CALL_HILINK_GET_WIFI_SSID]                     = HILINK_GetWiFiSsid,
    [APP_CALL_HILINK_SET_WIFI_INFO]                     = HILINK_SetWiFiInfo,
    [APP_CALL_HILINK_RECONNECT_WIFI]                    = HILINK_ReconnectWiFi,
    [APP_CALL_HILINK_CONNECT_WIFI]                      = HILINK_ConnectWiFi,
    [APP_CALL_HILINK_GET_NETWORK_STATE]                 = HILINK_GetNetworkState,
    [APP_CALL_HILINK_GET_WIFI_BSSID]                    = HILINK_GetWiFiBssid,
    [APP_CALL_HILINK_GET_WIFI_RSSI]                     = HILINK_GetWiFiRssi,
    [APP_CALL_HILINK_START_SOFT_AP]                     = HILINK_StartSoftAp,
    [APP_CALL_HILINK_STOP_SOFT_AP]                      = HILINK_StopSoftAp,
    [APP_CALL_HILINK_GET_DEV_INFO]                      = HILINK_GetDevInfo,
    [APP_CALL_HILINK_GET_SVC_INFO]                      = HILINK_GetSvcInfo,
    [APP_CALL_HILINK_GET_AUTO_AC]                       = HILINK_GetAutoAc,
    [APP_CALL_HILINK_PUT_CHAR_STATE]                    = HILINK_PutCharState,
    [APP_CALL_HILINK_CONTROL_CHAR_STATE]                = HILINK_ControlCharState,
    [APP_CALL_HILINK_GET_CHAR_STATE]                    = HILINK_GetCharState,
    [APP_CALL_HILINK_GET_PIN_CODE]                      = HILINK_GetPinCode,
    [APP_CALL_HILINK_NOTIFY_DEV_STATUS]                 = HILINK_NotifyDevStatus,
    [APP_CALL_HILINK_PROCESS_BEFORE_RESTART]            = HILINK_ProcessBeforeRestart,
    [APP_CALL_REGISTE_LOG]                              = registe_log,
    [APP_CALL_GET_INSTANCE]                             = get_instance,
    [APP_CALL_DESTROY]                                  = destroy,
    [APP_CALL_SET_CONTEXT]                              = set_context,
    [APP_CALL_RECEIVE_DATA]                             = receive_data,
    [APP_CALL_RECEIVE_DATA_WITH_JSON_OBJECT]            = receive_data_with_json_object,
    [APP_CALL_INIT_CENTER]                              = init_center,
    [APP_CALL_START_PAKE]                               = start_pake,
    [APP_CALL_AUTHENTICATE_PEER]                        = authenticate_peer,
    [APP_CALL_DELETE_LOCAL_AUTH_INFO]                   = delete_local_auth_info,
    [APP_CALL_IMPORT_AUTH_INFO]                         = import_auth_info,
    [APP_CALL_ADD_AUTH_INFO]                            = add_auth_info,
    [APP_CALL_REMOVE_AUTH_INFO]                         = remove_auth_info,
    [APP_CALL_IS_TRUST_PEER]                            = is_trust_peer,
    [APP_CALL_LIST_TRUST_PEERS]                         = list_trust_peers,
    [APP_CALL_SET_SELF_AUTH_ID]                         = set_self_auth_id,
    [APP_CALL_GET_LOCAL_ADDRESS]                        = GetLocalAddress,
    [APP_CALL_GET_LOCAL_NAME]                           = GetLocalName,
    [APP_CALL_SET_LOCAL_NAME]                           = SetLocalName,
    [APP_CALL_BLUETOOTH_FACTORY_RESET]                  = BluetoothFactoryReset,
    [APP_CALL_GET_BT_SCAN_MODE]                         = GetBtScanMode,
    [APP_CALL_SET_BT_SCAN_MODE]                         = SetBtScanMode,
    [APP_CALL_READ_BT_MAC_ADDR]                         = ReadBtMacAddr,
    [APP_CALL_GET_PARIED_DEVICES_NUM]                   = GetPariedDevicesNum,
    [APP_CALL_GET_PAIR_STATE]                           = GetPairState,
    [APP_CALL_REMOVE_PAIR]                              = RemovePair,
    [APP_CALL_REMOVE_ALL_PAIRS]                         = RemoveAllPairs,
    [APP_CALL_READ_REMOTE_RSSI_VALUE]                   = ReadRemoteRssiValue,
    [APP_CALL_IS_ACL_CONNECTED]                         = IsAclConnected,
    [APP_CALL_DISCONNECT_REMOTE_DEVICE]                 = DisconnectRemoteDevice,
    [APP_CALL_CONNECT_REMOTE_DEVICE]                    = ConnectRemoteDevice,
    [APP_CALL_INIT_BT_STACK]                            = InitBtStack,
    [APP_CALL_ENABLE_BT_STACK]                          = EnableBtStack,
    [APP_CALL_DISABLE_BT_STACK]                         = DisableBtStack,
    [APP_CALL_SET_DEVICE_NAME]                          = SetDeviceName,
    [APP_CALL_BLE_SET_ADV_DATA]                         = BleSetAdvData,
    [APP_CALL_BLE_START_ADV]                            = BleStartAdv,
    [APP_CALL_BLE_STOP_ADV]                             = BleStopAdv,
    [APP_CALL_BLE_UPDATE_ADV]                           = BleUpdateAdv,
    [APP_CALL_BLE_SET_SECURITY_IO_CAP]                  = BleSetSecurityIoCap,
    [APP_CALL_BLE_SET_SECURITY_AUTH_REQ]                = BleSetSecurityAuthReq,
    [APP_CALL_BLE_GATT_SECURITY_RSP]                    = BleGattSecurityRsp,
    [APP_CALL_BLE_SCAN_FILTER_PARAM_SETUP]              = BleScanFilterParamSetup,
    [APP_CALL_BLE_SCAN_FILTER_ADD_REMOVE]               = BleScanFilterAddRemove,
    [APP_CALL_BLE_SCAN_FILTER_CLEAR]                    = BleScanFilterClear,
    [APP_CALL_BLE_SCAN_FILTER_ENABLE]                   = BleScanFilterEnable,
    [APP_CALL_BLE_SET_SCAN_PARAMETERS]                  = BleSetScanParameters,
    [APP_CALL_BLE_START_SCAN]                           = BleStartScan,
    [APP_CALL_BLE_STOP_SCAN]                            = BleStopScan,
    [APP_CALL_BLE_GATT_REGISTER_CALLBACKS]              = BleGattRegisterCallbacks,
    [APP_CALL_BLE_START_ADV_EX]                         = BleStartAdvEx,
    [APP_CALL_BLE_GATTS_REGISTER]                       = BleGattsRegister,
    [APP_CALL_BLE_GATTS_UN_REGISTER]                    = BleGattsUnRegister,
    [APP_CALL_BLE_GATTS_DISCONNECT]                     = BleGattsDisconnect,
    [APP_CALL_BLE_GATTS_ADD_SERVICE]                    = BleGattsAddService,
    [APP_CALL_BLE_GATTS_ADD_CHARACTERISTIC]             = BleGattsAddCharacteristic,
    [APP_CALL_BLE_GATTS_ADD_DESCRIPTOR]                 = BleGattsAddDescriptor,
    [APP_CALL_BLE_GATTS_START_SERVICE]                  = BleGattsStartService,
    [APP_CALL_BLE_GATTS_STOP_SERVICE]                   = BleGattsStopService,
    [APP_CALL_BLE_GATTS_DELETE_SERVICE]                 = BleGattsDeleteService,
    [APP_CALL_BLE_GATTS_CLEAR_SERVICES]                 = BleGattsClearServices,
    [APP_CALL_BLE_GATTS_SEND_RESPONSE]                  = BleGattsSendResponse,
    [APP_CALL_BLE_GATTS_SEND_INDICATION]                = BleGattsSendIndication,
    [APP_CALL_BLE_GATTS_SET_ENCRYPTION]                 = BleGattsSetEncryption,
    [APP_CALL_BLE_GATTS_REGISTER_CALLBACKS]             = BleGattsRegisterCallbacks,
    [APP_CALL_BLE_GATTS_START_SERVICE_EX]               = BleGattsStartServiceEx,
    [APP_CALL_BLE_GATTS_STOP_SERVICE_EX]                = BleGattsStopServiceEx,
    [APP_CALL_HILINK_GET_DEVICE_SN]                     = HILINK_GetDeviceSn,
    [APP_CALL_HILINK_GET_SUB_PROD_ID]                   = HILINK_GetSubProdId,
    [APP_CALL_HILINK_BT_GET_DEV_SURFACE_POWER]          = HILINK_BT_GetDevSurfacePower,
    [APP_CALL_HILINK_BT_GET_DEV_INFO]                   = HILINK_BT_GetDevInfo,
    [APP_CALL_HILINK_GET_CUSTOM_INFO]                   = HILINK_GetCustomInfo,
    [APP_CALL_HILINK_GET_MANU_ID]                       = HILINK_GetManuId,
    [APP_CALL_HILINK_BT_GET_MAC_ADDR]                   = HILINK_BT_GetMacAddr,
    [APP_CALL_GET_DEVICE_VERSION]                       = getDeviceVersion,
    [APP_CALL_OS_KERNEL_GET_TICK_COUNT]                 = osKernelGetTickCount,
    [APP_CALL_OS_KERNEL_GET_TICK_FREQ]                  = osKernelGetTickFreq,
    [APP_CALL_OS_DELAY]                                 = osDelay,
    [APP_CALL_OS_THREAD_NEW]                            = osThreadNew,
    [APP_CALL_OS_THREAD_TERMINATE]                      = osThreadTerminate,
    [APP_CALL_OS_THREAD_GET_ID]                         = osThreadGetId,
    [APP_CALL_OS_MUTEX_NEW]                             = osMutexNew,
    [APP_CALL_OS_MUTEX_DELETE]                          = osMutexDelete,
    [APP_CALL_OS_MUTEX_ACQUIRE]                         = osMutexAcquire,
    [APP_CALL_OS_MUTEX_RELEASE]                         = osMutexRelease,
    [APP_CALL_OS_SEMAPHORE_NEW]                         = osSemaphoreNew,
    [APP_CALL_OS_SEMAPHORE_ACQUIRE]                     = osSemaphoreAcquire,
    [APP_CALL_OS_SEMAPHORE_RELEASE]                     = osSemaphoreRelease,
    [APP_CALL_OS_SEMAPHORE_DELETE]                      = osSemaphoreDelete,
    [APP_CALL_OS_THREAD_SUSPEND]                        = osThreadSuspend,
    [APP_CALL_OS_THREAD_RESUME]                         = osThreadResume,
};

int *get_hilink_tbl(void)
{
    return g_hilink_tbl;
}

void hilink_func_map_init(void)
{
    printf("%s %d, 0x%x\r\n", __FUNCTION__, __LINE__, g_hilink_info);

    if (g_hilink_info->entry != NULL) {
        printf("%s %d, 0x%x\r\n", __FUNCTION__, __LINE__, g_hilink_info->entry);
        g_hilink_info->entry((void **)&g_hilink_tbl, g_app_call_tbl);
    }
}
