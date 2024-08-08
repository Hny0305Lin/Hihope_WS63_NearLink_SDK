/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: service_wifi_api.h 的头文件
 */

#ifndef __SERVICE_WIFI_API_H__
#define __SERVICE_WIFI_API_H__

#include "wifi_device_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
* @ingroup wifi_service_api
*
* Type of WiFi device. \n
* CNcomment:WiFi设备的类型. CNend
*/
typedef enum {
    WIFI_TYPE_STA,           /**< STAION. CNcomment:STAION.CNend */
    WIFI_TYPE_HOTSPOT,       /**< HOTSPOT. CNcomment:HOTSPOT.CNend */
    WIFI_TYPE_P2P,           /**< P2P. CNcomment:P2P.CNend */
    WIFI_TYPE_BUTT
} wifi_device_type_enum;

/**
* @ingroup wifi_service_api
*
* Connection mode of WPS. \n
* CNcomment:WPS的连接方式.CNend
*/
typedef enum wifi_wps_method {
    WIFI_WPS_PBC,            /**< Connection mode of WPS PBC. CNcomment:WPS PBC连接方式.CNend */
    WIFI_WPS_PIN,            /**< Connection mode of WPS PIN. CNcomment:WPS PIN连接方式.CNend */
    WIFI_WPS_BUTT,
} wifi_wps_method;

/**
* @ingroup wifi_service_api
*
* Config of wps. \n
* CNcomment:wps的配置.CNend
*/
typedef struct wifi_wps_config {
    wifi_wps_method  wps_method;                   /**< Connection mode of WPS.CNcomment:WPS的连接方式.CNend */
    unsigned char  bssid[WIFI_MAC_LEN];         /**< Basic service set ID (BSSID).CNcomment:BSSID.CNend */
    char   wps_pin[WIFI_WPS_PIN_MAX_LEN_NUM];    /**< Pin Code of WPS.CNcomment:WPS的Pin码.CNend */
} wifi_wps_config;

/**
* @ingroup wifi_service_api
*
* Type of wow pattern. \n
* CNcomment:wow pattern类型.CNend
*/
typedef enum {
    WOW_PATTERN_ADD,    /**< Add Pattern. CNcomment:添加模式.CNend */
    WOW_PATTERN_DEL,    /**< DEL Pattern. CNcomment:删除模式.CNend */
    WOW_PATTERN_CLR,    /**< CLR Pattern. CNcomment:清除模式.CNend */
    WOW_PATTERN_BUTT,
} wow_pattern_type;

/**
* @ingroup wifi_service_api
*
* Enumeration of return codes.\n
* CNcomment:返回码的枚举. CNend
*/
typedef enum {
    WIFI_SUCCESS = 0, /**< No errors. CNcomment:没有错误.CNend */
    ERROR_WIFI_INVALID_ARGS = -1, /**< Invalid parameters.CNcomment:无效参数.CNend */
    ERROR_WIFI_CHIP_INVALID = -2, /**< Invalid chip.CNcomment:无效芯片.CNend */
    ERROR_WIFI_IFACE_INVALID = -3, /**< Invalid WiFi interface.CNcomment:无效的WiFi接口.CNend */
    ERROR_WIFI_RTT_CONTROLLER_INVALID = -4, /**< Invalid RTT controller.CNcomment:无效的RTT控制器.CNend */
    ERROR_WIFI_NOT_SUPPORTED = -5, /**< WiFi not supported by the current version or device.
                                        CNcomment:当前版本或设备不支持WiFi.CNend */
    ERROR_WIFI_NOT_AVAILABLE = -6, /**< WiFi unavailable.CNcomment:WiFi不可用.CNend */
    ERROR_WIFI_NOT_STARTED = -7, /**< Wi-Fi not initialized or starte.CNcomment:WiFi未初始化或者开启.CNend */
    ERROR_WIFI_BUSY = -8, /**< System busy.CNcomment:系统忙.CNend */
    ERROR_WIFI_INVALID_PASSWORD = -9, /**< WiFi invalid password.CNcomment:WiFi密码无效.CNend */
    ERROR_WIFI_UNKNOWN = -128 /**< Unknown error.CNcomment:未知的错误.CNend */
} wifi_return_code;

/**
* @ingroup  soc_wifi_basic
* @brief  Set protocol mode of sta.CNcomment:设置station接口的protocol模式.CNend
*
* @par Description:
*           Set protocol mode of sta, set before calling uapi_wifi_sta_start().\n
*           CNcomment:配置station接口的protocol模式, 在sta start之前调用.CNend
*
* @attention  Default mode 802.11BGN CNcomment:默认模式 802.11BGN.CNend
* @param  mode            [IN]     Type #protocol_mode_enum, protocol mode.
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_set_protocol_mode(protocol_mode_enum mode);

/**
* @ingroup  soc_wifi_basic
* @brief  Get protocol mode of.CNcomment:获取station接口的protocol模式.CNend
*
* @par Description:
*           Get protocol mode of station.CNcomment:获取station接口的protocol模式.CNend
*
* @attention  NULL
* @param      NULL
*
* @retval #protocol_mode_enum protocol mode.If fail, return EXT_WIFI_PHY_MODE_BUTT.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
protocol_mode_enum uapi_wifi_sta_get_protocol_mode(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Set protocol mode of softap.CNcomment:设置softap接口的protocol模式.CNend
*
* @par Description:
*           Set protocol mode of softap.CNcomment:设置softap接口的protocol模式.CNend\n
*           Initiallize config, set before softap start.CNcomment:初始配置,在softap start之前调用.CNend
*
* @attention  Default mode(802.11BGN) CNcomment:默认模式（802.11BGN）.CNend
* @param  mode            [IN]     Type  #protocol_mode_enum protocol mode.
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_softap_set_protocol_mode(protocol_mode_enum mode);

/**
* @ingroup  soc_wifi_basic
* @brief  Get protocol mode of softap.CNcomment:获取softap接口的protocol模式.CNend
*
* @par Description:
*           Get protocol mode of softap.CNcomment:获取softap接口的protocol模式.CNend
*
* @attention  NULL
* @param      NULL
*
* @retval #protocol_mode_enum protocol mode. If fail, return WIFI_MODE_UNDEFINE.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
protocol_mode_enum uapi_wifi_softap_get_protocol_mode(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Config pmf settings of sta.CNcomment:配置station的pmf.CNend
*
* @par Description:
*           Config pmf settings of sta, set before sta start.CNcomment:配置station的pmf, 在sta start之前调用.CNend
*
* @attention  Default pmf enum value 1. CNcomment:默认pmf枚举值1.CNend
* @param  pmf           [IN]     Type #wifi_pmf_option_enum, pmf enum value.CNcoment:pmf枚举值.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_set_pmf(wifi_pmf_option_enum pmf);

wifi_pmf_option_enum uapi_wifi_get_pmf(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Get wifi device config.CNcomment:获取wifi device的配置.CNend
*
* @par Description:
*           Get wifi device config.CNcomment:获取wifi device的配置.CNend
*
* @attention  NULL
* @param  iftype           [IN]     Type #ext_wifi_iftype, iftype enum value.CNcoment:iftype枚举值.CNend
*
* @retval #struct ext_wifi_dev *     Execute successfully.
* @retval #NULL                     Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
struct ext_wifi_dev *wifi_dev_get(ext_wifi_iftype iftype);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of service_wifi_api.h */
