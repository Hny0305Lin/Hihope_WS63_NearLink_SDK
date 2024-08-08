/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: In mbedtls lib, add the ability to dynamically configure.
 */
#include "common.h"

#if defined(VENDOR_CUSTOMIZE_CONFIGS_C)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/hw_mbedtls_config.h"

/*****************************************************************************
 * \brief mbedtls定制特性，支持一些特性的动态修改与调整。
 * 默认所有的特性按mbedtls官方的能力处理。
******************************************************************************/
HW_MBEDTLS_CONFIG_TYPES g_mbedtls_configs[HW_MBEDTLS_CONFIG_BUTT] = {HW_MBEDTLS_CONFIG_DISABLE};

/*****************************************************************************
 * \brief 配置mbedtls动态开关。
 * 
 * \param type 需要修改的特性名。
 * \param value 0表示关闭此特性，非0表示打开此特性。
 * \return int 0表示设置成功，非0表示设置失败。
 * 产品在使用mbedtls库时，需要根据业务诉求，修改一些默认配置。
******************************************************************************/
int hw_mbedtls_set_config(HW_MBEDTLS_CONFIG_TYPES type, int value)
{
    if ((type <= HW_MBEDTLS_CONFIG_NONE) || (type >= HW_MBEDTLS_CONFIG_BUTT)){
        /* 错误的特性类型 */
        return -1;
    }
    g_mbedtls_configs[type] = (value == 0 ? HW_MBEDTLS_CONFIG_DISABLE : HW_MBEDTLS_CONFIG_ENABLE);
    return 0;
}

/*****************************************************************************
 * \brief 获取mbedtls定制特性的动态值。
 * 
 * \param type 需要查询的特性名。
 * \return int 返回此特性的值：0表示没有开启；1表示开启；-1表示处理错误，未知状态。
******************************************************************************/
int hw_mbedtls_get_config(HW_MBEDTLS_CONFIG_TYPES type)
{
    if ((type <= HW_MBEDTLS_CONFIG_NONE) || (type >= HW_MBEDTLS_CONFIG_BUTT)){
        /* 错误的特性类型 */
        return -1;
    }    
    return g_mbedtls_configs[type];
}

#endif
