/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink SDK 软件适配层defines（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_SAL_DEFINES_H
#define HILINK_SAL_DEFINES_H

#ifdef HILINK_SDK_BUILD_IN
#include "hilink_log.h"

#define HILINK_SAL_DEBUG(...)               hilink_debug(__VA_ARGS__)
#define HILINK_SAL_INFO(...)                hilink_info(__VA_ARGS__)
#define HILINK_SAL_NOTICE(...)              hilink_notice(__VA_ARGS__)
#define HILINK_SAL_WARN(...)                hilink_warning(__VA_ARGS__)
#define HILINK_SAL_ERROR(...)               hilink_error(__VA_ARGS__)
#define HILINK_SAL_CRIT(...)                hilink_critical(__VA_ARGS__)
#define HILINK_SAL_DEBUG_LIMITED(...)       HILINK_DEBUG_LIMITED(__VA_ARGS__)
#define HILINK_SAL_ERROR_LIMITED(...)       HILINK_ERROR_LIMITED(__VA_ARGS__)

#else

/* hilink TLS适配层使用的mbedtls版本，可设为2或3 */
#define MBEDTLS_MAJOR_VERSION 3
#define HILINK_MBEDTLS_CCM_SUPPORT 0

#if (MBEDTLS_MAJOR_VERSION == 3)
#define MBEDTLS_VERSION_3
#endif

#define HILINK_SAL_LOG_LVL_MIN      0
#define HILINK_SAL_LOG_LVL_DEBUG    1
#define HILINK_SAL_LOG_LVL_INFO     2
#define HILINK_SAL_LOG_LVL_NOTICE   3
#define HILINK_SAL_LOG_LVL_WARN     4
#define HILINK_SAL_LOG_LVL_ERROR    5
#define HILINK_SAL_LOG_LVL_CRIT     6

#ifndef HILINK_SAL_LOG_BUILD_LVL
#define HILINK_SAL_LOG_BUILD_LVL    HILINK_SAL_LOG_LVL_MIN
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int HILINK_Printf(const char *format, ...);

#ifdef __cplusplus
}
#endif

#if HILINK_SAL_LOG_BUILD_LVL <= HILINK_SAL_LOG_LVL_DEBUG
#define HILINK_SAL_DEBUG(...)   \
    do {    \
        HILINK_Printf("[HSD]%s():%u, ", __FUNCTION__, __LINE__);  \
        HILINK_Printf(__VA_ARGS__); \
    } while (0)
#else
#define HILINK_SAL_DEBUG(...)
#endif

#if HILINK_SAL_LOG_BUILD_LVL <= HILINK_SAL_LOG_LVL_INFO
#define HILINK_SAL_INFO(...)   \
    do {    \
        HILINK_Printf("[HSI]%s():%u, ", __FUNCTION__, __LINE__);  \
        HILINK_Printf(__VA_ARGS__); \
    } while (0)
#else
#define HILINK_SAL_INFO(...)
#endif

#if HILINK_SAL_LOG_BUILD_LVL <= HILINK_SAL_LOG_LVL_NOTICE
#define HILINK_SAL_NOTICE(...)   \
    do {    \
        HILINK_Printf("[HSN]%s():%u, ", __FUNCTION__, __LINE__);  \
        HILINK_Printf(__VA_ARGS__); \
    } while (0)
#else
#define HILINK_SAL_NOTICE(...)
#endif

#if HILINK_SAL_LOG_BUILD_LVL <= HILINK_SAL_LOG_LVL_WARN
#define HILINK_SAL_WARN(...)   \
    do {    \
        HILINK_Printf("[HSW]%s():%u, ", __FUNCTION__, __LINE__);  \
        HILINK_Printf(__VA_ARGS__); \
    } while (0)
#else
#define HILINK_SAL_WARN(...)
#endif

#if HILINK_SAL_LOG_BUILD_LVL <= HILINK_SAL_LOG_LVL_ERROR
#define HILINK_SAL_ERROR(...)   \
    do {    \
        HILINK_Printf("[HSE]%s():%u, ", __FUNCTION__, __LINE__);  \
        HILINK_Printf(__VA_ARGS__); \
    } while (0)
#else
#define HILINK_SAL_ERROR(...)
#endif

#if HILINK_SAL_LOG_BUILD_LVL <= HILINK_SAL_LOG_LVL_CRIT
#define HILINK_SAL_CRIT(...)   \
    do {    \
        HILINK_Printf("[HSC]%s():%u, ", __FUNCTION__, __LINE__);  \
        HILINK_Printf(__VA_ARGS__); \
    } while (0)
#endif

#define HILINK_SAL_DEBUG_LIMITED(...)
#define HILINK_SAL_ERROR_LIMITED(...)
#endif /* HILINK_SDK_BUILD_IN */

#ifndef ENUM_INT_REVERSED
#define ENUM_INT_REVERSED   (0x7FFFFFFFU)
#endif

enum HiLinkSalErrorCode {
    HILINK_SAL_KV_INTI_ERR = -600,
    HILINK_SAL_KV_SET_ITEM_ERR,
    HILINK_SAL_KV_GET_ITEM_ERR,
    HILINK_SAL_KV_DELETE_ITEM_ERR,

    HILINK_SAL_GET_IP_ERR = -500,
    HILINK_SAL_GET_MAC_ERR,
    HILINK_SAL_GET_SSID_ERR,
    HILINK_SAL_SET_WIFI_ERR,
    HILINK_SAL_GET_WIFI_INFO_ERR,
    HILINK_SAL_SCAN_WIFI_ERR,
    HILINK_SAL_ADD_WIFI_ERR,
    HILINK_SAL_REMOVE_WIFI_ERR,
    HILINK_SAL_CONENCT_WIFI_ERR,
    HILINK_SAL_SET_SOFTAP_ERR,

    HILINK_SAL_THREAD_ERR = -400,
    HILINK_SAL_MUTEX_ERR,
    HILINK_SAL_SEM_ERR,
    HILINK_SAL_SLEEP_ERR,
    HILINK_SAL_TIME_ERR,
    HILINK_SAL_MALLOC_ERR,

    HILINK_SAL_DNS_ERR = -300,
    HILINK_SAL_FCNTL_ERR,
    HILINK_SAL_SET_SOCK_OPT_ERR,
    HILINK_SAL_GET_SOCK_OPT_ERR,

    HILINK_SAL_MEMCPY_ERR = -200,
    HILINK_SAL_STRCPY_ERR,
    HILINK_SAL_STRNCPY_ERR,
    HILINK_SAL_SPRINTF_ERR,
    HILINK_SAL_MEMSET_ERR,

    HILINK_SAL_PARAM_INVALID = -100,
    HILINK_SAL_TIMEOUT,
    HILINK_SAL_NOT_SUPPORT,
    HILINK_SAL_NOT_INIT,
    HILINK_SAL_FD_INVALID,

    HILINK_SAL_NOK = -1,
    HILINK_SAL_OK = 0,
};

#endif /* HILINK_SAL_DEFINES_H */