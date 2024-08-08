/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: slp error code
 * ROM      : NO
 * STRATEGY : NO
 */

/**
 * @defgroup middleware_service_slp_errcode SLP Error Code
 * @ingroup  middleware_service_slp
 * @{
 */

#ifndef SLP_ERRCODE_H
#define SLP_ERRCODE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  SLP error code base.
 * @else
 * @brief  SLP 错误码起始。
 * @endif
 */
#define ERRCODE_SLP_BASE 0x80009400

/**
 * @if Eng
 * @brief  SLP error code base.
 * @else
 * @brief  SLP 错误码结束。
 * @endif
 */
#define ERRCODE_SLP_END  0x800095FF

/**
 * @if Eng
 * @brief  SLP Client error code base.
 * @else
 * @brief  SLP Client 错误码起始。
 * @endif
 */
#define ERRCODE_SLPC_BASE 0x80009600

/**
 * @if Eng
 * @brief  SLP Client error code base.
 * @else
 * @brief  SLP Client错误码结束。
 * @endif
 */
#define ERRCODE_SLPC_END  0x800097FF

/**
 * @if Eng
 * @brief  SLP error code.
 * @else
 * @brief  SLP 错误码。
 * @endif
 */
typedef enum {
    ERRCODE_SLP_SUCCESS = 0,                               /*!< @if Eng error code of success
                                                                @else   执行成功错误码 @endif */
    ERRCODE_SLP_EXCEED_MAX_COUNTER = ERRCODE_SLP_BASE,     /*!< @if Eng error code of exceeding max conter
                                                                @else   超过芯片最大计数值错误码 @endif */
    RRCODE_SLP_MAX = ERRCODE_SLP_END                       /*!< @if Eng maximum of SLP error code
                                                                @else   SLP 错误码最大值 @endif */
} ErrcodeSlp;

/**
 * @if Eng
 * @brief  SLP Client error code.
 * @else
 * @brief  SLP Client 错误码。
 * @endif
 */
typedef enum {
    ERRCODE_SLPC_SUCCESS = 0,                               /*!< @if Eng error code of success
                                                                 @else   执行成功错误码 @endif */
    ERRCODE_SLPC_MALLOC_FAILED = ERRCODE_SLPC_BASE,         /*!< @if Eng error code of malloc failed
                                                                 @else   申请动态内存失败错误码 @endif */
    ERRCODE_SLPC_MEMCPY_FAILED,                             /*!< @if Eng error code of memcpy failed
                                                                 @else   内存拷贝失败错误码 @endif */
    ERRCODE_SLPC_SEND_PAYLOAD_FAILED,                       /*!< @if Eng error code of sending payload failed
                                                                 @else   发送负载失败错误码 @endif */
    ERRCODE_SLPC_RECV_PAYLOAD_FAILED,                       /*!< @if Eng error code of receiving payload failed
                                                                 @else   接收负载失败错误码 @endif */
    RRCODE_SLPC_MAX = ERRCODE_SLPC_END                       /*!< @if Eng maximum of SLP error code
                                                                 @else   SLPC 错误码最大值 @endif */
} ErrcodeSlpClient;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif