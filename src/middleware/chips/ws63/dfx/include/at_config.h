/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides at product config header \n
 * Author:
 * History: \n
 * 2022-09-02, Create file. \n
 */

#ifndef    AT_CONFIG_H
#define    AT_CONFIG_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "securec.h"
#include "soc_osal.h"
#include "common_def.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_chips_at_config Config
 * @ingroup  middleware_chips_at
 * @{
 */

/**
 * @if Eng
 * @brief  Definition maximum length of AT command name.Determined by the product.
 * @else
 * @brief  定义AT命令名称的最大长度，由产品定义。
 * @endif
 */
#define AT_CMD_NAME_MAX_LENGTH    32

/**
 * @if Eng
 * @brief  Definition maximum length of AT command.Determined by the product.
 * @else
 * @brief  定义AT命令的最大长度，由产品定义。
 * @endif
 */
#define AT_CMD_MAX_LENGTH    4160

/**
 * @if Eng
 * @brief  Definition timeout of AT command.Determined by the product.
 * @else
 * @brief  定义AT命令的超时时间，由产品定义。
 * @endif
 */
#define AT_MAX_TIME_OUT    150

/**
 * @if Eng
 * @brief  Definition maximum length of AT command message queue.Determined by the product.
 * @else
 * @brief  定义AT命令消息队列最大长度，由产品定义。
 * @endif
 */
#define AT_MSG_MAX_NUM    10

#ifdef CONFIG_AT_SUPPORT_CMD_TABLE_CHECK
#define AT_PARA_MAX_NUM    32
#endif

/**
 * @if Eng
 * @brief  Definition AT command channel number.Determined by the product.
 * @note   Each channel number will occupy 16 bytes of resources.
 * @else
 * @brief  定义AT命令通道号，由产品定义。
 * @note   每个通道号都会占用16字节资源。
 * @endif
 */
typedef enum {
    AT_UART_PORT                = 0x00,
#ifdef CONFIG_AT_SUPPORT_ZDIAG
    AT_ZDIAG_PORT               = 0x01,
#endif
    AT_MAX_PORT_NUMBER
} at_channel_id_t;

/**
 * @}
 */
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif