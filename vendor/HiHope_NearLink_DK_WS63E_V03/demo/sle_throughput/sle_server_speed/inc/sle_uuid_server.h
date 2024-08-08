/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 *
 * Description: SLE UUID Server module.
 */

/**
 * @defgroup SLE UUID SERVER API
 * @ingroup
 * @{
 */
#ifndef SLE_UUID_SERVER_H
#define SLE_UUID_SERVER_H

#include "sle_ssap_server.h"

/* Service UUID */
#define SLE_UUID_SERVER_SERVICE        0xABCD

/* Property UUID */
#define SLE_UUID_SERVER_NTF_REPORT     0x1122

/* Property Property */
#define SLE_UUID_TEST_PROPERTIES  (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

/* Descriptor Property */
#define SLE_UUID_TEST_DESCRIPTOR   (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

/**
 * @if Eng
 * @brief  SLE uuid server inir.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li sle_ssap_server.h
 * @else
 * @brief  SLE UUID服务器初始化。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li sle_ssap_server.h
 * @endif
 */
errcode_t sle_uuid_server_init(void);

/**
 * @if Eng
 * @brief  send data to peer device by uuid on uuid server.
 * @attention  NULL
 * @param  [in]  value  send value.
 * @param  [in]  len    Length of send value。
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li sle_ssap_server.h
 * @else
 * @brief  通过uuid server 发送数据给对端。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li sle_ssap_server.h
 * @endif
 */
errcode_t sle_uuid_server_send_report_by_uuid(uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  send data to peer device by handle on uuid server.
 * @attention  NULL
 * @param  [in]  value  send value.
 * @param  [in]  len    Length of send value。
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li sle_ssap_server.h
 * @else
 * @brief  通过uuid server 发送数据给对端。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li sle_ssap_server.h
 * @endif
 */
errcode_t sle_uuid_server_send_report_by_handle(const uint8_t *data, uint8_t len);
#endif
