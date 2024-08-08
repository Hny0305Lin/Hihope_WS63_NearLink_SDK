/**
* @file soc_wifi_mfg_test_if.h
*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: header file for wifi manufacturing test interference.CNcomment:描述：WiFi 产测接口头文件CNend\n
* Create: 2019-01-03
*/

/**
 * @defgroup soc_wifi_mfg_test_if
 */
/**
 * @defgroup soc_wifi_mfg_test_if Basic Settings
 * @ingroup soc_wifi
 */

#ifndef HI_WIFI_MFG_TEST_IF_H
#define HI_WIFI_MFG_TEST_IF_H

#include "td_type.h"
#include "osal_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    EXT_AT_AL_TX,
    EXT_AT_AL_RX,
    EXT_AT_RX_INFO,
    EXT_AT_SET_COUNTRY,
    EXT_AT_GET_COUNTRY,
    EXT_AT_SET_WLAN0_BW,
    EXT_AT_SET_AP0_BW,
    EXT_AT_SET_MESH0_BW,
    EXT_AT_GET_WLAN0_MESHINFO,
    EXT_AT_GET_MESH0_MESHINFO,
    EXT_AT_SET_TPC,
    EXT_AT_SET_TRC,
    EXT_AT_SET_RATE,
    EXT_AT_SET_ARLOG,
    EXT_AT_GET_VAP_INFO,
    EXT_AT_GET_USR_INFO,
    EXT_AT_SET_RATE_POWER,
    EXT_AT_CAL_TONE,

    EXT_AT_TYPE_BUTT
}ext_at_type_enum;

/**
* @ingroup  soc_wifi_mfg_test_if
* @brief  Set cal rate power.CNcomment:对不同协议场景、不用速率分别做功率补偿CNend
*
* @par Description:
*           Set cal band power.CNcomment:对不同协议场景、不用速率分别做功率补偿CNend
*
* @attention  NULL
* @param  protol         [IN]     Type  #unsigned char protol.CNcomment:协议序号CNend
* @param  rate           [IN]     Type  #unsigned char rate.CNcomment:速率CNend
* @param  val            [IN]     Type  #int power val.CNcomment:补偿值CNend
*
* @retval #EXT_ERR_SUCCESS  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_mfg_test_if.h: WiFi mfg_test
* @see  NULL
* @since
*/
td_u32 wal_set_cal_rate_power(td_u8 protol, td_u8 rate, td_s32 val);

/**
* @ingroup  soc_wifi_mfg_test_if
* @brief  Set cal freq.CNcomment:进行常温频偏功率补偿CNend
*
* @par Description:
*           Set cal freq.CNcomment:进行常温频偏功率补偿CNend
*
* @attention  NULL
* @param  freq_offset    [IN]     Type  #int freq offset.CNcomment:补偿值CNend
*
* @retval #EXT_ERR_SUCCESS  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_mfg_test_if.h: WiFi mfg_test
* @see  NULL
* @since
*/
td_u32 wal_set_cal_freq(td_s32 freq_offset);

/**
* @ingroup  soc_wifi_mfg_test_if
* @brief  get cur cal data.CNcomment:获取当前产测校准值CNend
*
* @par Description:
*           get cur cal data.CNcomment:获取当前产测校准值CNend
*
* @attention  NULL
* @param  NULL
*
* @retval #EXT_ERR_SUCCESS  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_mfg_test_if.h: WiFi mfg_test
* @see  NULL
* @since
*/
unsigned int wal_get_cal_data(void);

#ifdef CONFIG_FACTORY_TEST_MODE
/**
* @ingroup  soc_wifi_mfg_test_if
* @brief  add cfg vap.CNcomment:添加配置vapCNend
*
* @par Description:
*           add cfg vap.CNcomment:添加配置vapCNend
*
* @attention  NULL
* @param  NULL
*
* @retval #EXT_ERR_SUCCESS  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_mfg_test_if.h: WiFi mfg_test
* @see  NULL
* @since
*/
unsigned int wal_add_cfg_vap(void);
#endif

/**
* @ingroup  soc_wifi_mfg_test_if
* @brief  always rx/tx interference function.CNcomment:常发常收等接口函数CNend
*
* @par Description:
*           always rx/tx interference function.CNcomment:常发常收等接口函数CNend
*
* @attention  NULL
* @param  argc         [IN]     Type  #int argc.CNcomment:命令参数个数CNend
* @param  argv         [IN]     Type  #const char *argv.CNcomment:命令各参数对应的字符串数组CNend
* @param  cmd_type     [IN]     Type  #unsigned int cmd_type.CNcomment:命令类型CNend
*
* @retval #EXT_ERR_SUCCESS  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_mfg_test_if.h: WiFi mfg_test
* @see  NULL
* @since
*/
osal_u32 uapi_wifi_at_start(osal_s32 argc, const osal_char *argv[], osal_u32 cmd_type);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
