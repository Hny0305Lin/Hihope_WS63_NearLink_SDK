/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved.
 *
 * Description: SLE private service register sample of client.
 */

/**
 * @defgroup SLE UUID CLIENT API
 * @ingroup
 * @{
 */

#ifndef SLE_CLIENT_ADV_H
#define SLE_CLIENT_ADV_H

/**
 * @if Eng
 * @brief  sle uuid client init.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li NULL
 * @else
 * @brief  sle uuid客户端初始化。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li NULL
 * @endif
 */
void sle_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb);

/**
 * @if Eng
 * @brief  sle start scan.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li NULL
 * @else
 * @brief  sle启动扫描。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li NULL
 * @endif
 */
void sle_start_scan(void);

#endif