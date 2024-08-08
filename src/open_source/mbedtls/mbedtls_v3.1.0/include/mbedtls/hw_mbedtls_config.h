/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: In mbedtls lib, adaption for polarssl.
 */
#ifndef HW_MBEDTLS_CONFIG_H
#define HW_MBEDTLS_CONFIG_H

/*****************************************************************************
 * \brief 配置使能
 * 
******************************************************************************/
#define HW_MBEDTLS_CONFIG_DISABLE   (0)

/*****************************************************************************
 * \brief 配置不使能
 * 
******************************************************************************/
#define HW_MBEDTLS_CONFIG_ENABLE    (1)

/*****************************************************************************
 * \brief 定制修改，用于动态调整mbedtls库的功能。
 * 
******************************************************************************/
typedef enum HW_MBEDTLS_CONFIG_TYPES_en {
    HW_MBEDTLS_CONFIG_NONE = 0,             /* 无效值 */
    HW_MBEDTLS_CHECK_CERT_DATE = 1,         /* SSL协议栈校验证书有效期，mbedtls库默认校验 */
    HW_MBEDTLS_CHECK_CERT_ISSUE_NAME = 2,   /* X.509证书校验发生者名字，mbedtls库默认校验 */
    HW_MBEDTLS_DISABLE_MD2 = 3,             /* 禁用MD2，默认为1，不使用MD2 */
    HW_MBEDTLS_DISABLE_MD4 = 4,             /* 禁用MD4，默认为1，不使用MD4 */
    HW_MBEDTLS_CONFIG_BUTT
} HW_MBEDTLS_CONFIG_TYPES;

/*****************************************************************************
 * \brief 配置mbedtls动态开关。
 * 
 * \param type 需要修改的特性名。
 * \param value 0表示关闭此特性，非0表示打开此特性。
 * \return int 0表示设置成功，非0表示设置失败。
******************************************************************************/
int hw_mbedtls_set_config(HW_MBEDTLS_CONFIG_TYPES type, int value);

/*****************************************************************************
 * \brief 获取mbedtls定制特性的动态值。
 * 
 * \param type 需要查询的特性名。
 * \return int 返回此特性的值：0表示没有开启；1表示开启；-1表示处理错误，未知状态。
******************************************************************************/
int hw_mbedtls_get_config(HW_MBEDTLS_CONFIG_TYPES type);

#endif
