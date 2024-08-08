/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT SPP module.
 */

/**
 * @defgroup bluetooth_bts_spp SPP API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BTS_SPP_H
#define BTS_SPP_H

#include <stdbool.h>
#include "bts_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  SPP is disconnected.
 * @else
 * @brief  SPP连接已经断开。
 * @endif
 */
#define SPP_READ_SOCKET_CLOSED (0)

/**
 * @if Eng
 * @brief  Read data failed.
 * @else
 * @brief  读取数据失败。
 * @endif
 */
#define SPP_READ_FAILED (-1)

/**
 * @if Eng
 * @brief  Write data failed.
 * @else
 * @brief  写数据失败。
 * @endif
 */
#define SPP_WRITE_FAILED (-1)

/**
 * @if Eng
 * @brief  Invalid SPP server/client ID.
 * @else
 * @brief  非法SPP Server 或 Client ID。
 * @endif
 */
#define SPP_INVALID_ID (-1)

/**
 * @if Eng
 * @brief  Enum of SPP socket type.
 * @else
 * @brief  SPP socket类型定义枚举。
 * @endif
 */
typedef enum {
    SPP_SOCKET_RFCOMM = 0x0,    /*!< @if Eng RFCOMM
                                     @else   RFCOMM @endif */
} spp_socket_type_t;

/**
 * @if Eng
 * @brief  Parameters for create SPP socket.
 * @else
 * @brief  创建SPP socket的参数。
 * @endif
 */
typedef struct {
    bt_uuid_t uuid;                /*!< @if Eng UUID of service.
                                        @else   SPP socket对应的服务UUID。 @endif */
    spp_socket_type_t socket_type; /*!< @if Eng SPP socket type
                                        @else SPP socket类型。@endif */
    bool is_encrypt;               /*!< @if Eng is encrypted (not use now, encrypted default)
                                        @else 是否加密（目前此参数未使用，默认加密）。@endif */
} spp_create_socket_para_t;

/**
 * @if Eng
 * @brief  Creates an server listening socket based on the service record.
 * @attention NULL.
 * @param  [in]  socket_para The parameters to create a server socket.
 * @param  [in]  name       The service's name.
 * @param  [in]  len        The length of the service's name, len = strlen(name) + 1.
 * @retval SPP_INVALID_ID create server fail.
 * @retval others create server success, return a server ID.
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_server_accept | spp_server_close
 * @else
 * @brief  创建SPP Server。
 * @attention NULL.
 * @param  [in]  socket_para 入参，SPP Server创建参数。
 * @param  [in]  name 入参，SPP Server 名称。
 * @param  [in]  len 入参，SPP Server 名称长度，取值为strlen(name) + 1。
 * @retval SPP_INVALID_ID 创建Server失败，返回非法ID。
 * @retval others 创建Server成功，返回Server ID。
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_server_accept | spp_server_close
 * @endif
 */
int spp_server_create(spp_create_socket_para_t *socket_para, const char *name, unsigned int len);

/**
 * @if Eng
 * @brief  Waits for a remote device to connect to this server socket.
 * @par Description:
 *      This method return a client ID indicates a client socket
 *      can be used to read data from and write data to remote device.
 * @attention This is a synchronous API.
 * @param  [in]  server_id The relative ID used to identify the current server
 *                         socket, obtain the value by calling {@link spp_server_create}.
 * @retval SPP_INVALID_ID Accept fail.
 * @retval others Return a client ID if accept success.
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_server_create | spp_disconnect | is_spp_connected | spp_get_remote_addr |
 *      spp_read | spp_write
 * @else
 * @brief  等待远端设备来连接此Server。
 * @par Description:此方法将返回一个Client ID，用户可以使用该Client与远端设备进行数据收发。
 * @attention 这是一个同步接口。
 * @param  [in]  server_id 入参，调用接口 {@link spp_server_create} 创建Server成功后得到的Server ID。
 * @retval SPP_INVALID_ID 等待失败。
 * @retval others 等待成功，返回一个有效的Client ID。
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_server_create | spp_disconnect | is_spp_connected | spp_get_remote_addr |
 *      spp_read | spp_write
 * @endif
 */
int spp_server_accept(int server_id);

/**
 * @if Eng
 * @brief  Disables an spp server socket and releases related resources.
 * @attention This is a synchronous API.
 * @param  [in]  server_id The relative ID used to identify the current server
 *                         socket, obtain the value by calling {@link spp_server_create}.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_server_create
 * @else
 * @brief  关闭Server（同时释放相关资源）。
 * @attention 这是一个同步接口。
 * @param  [in]  server_id 入参，调用接口 {@link spp_server_create} 创建Server成功后得到的Server ID。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_server_create
 * @endif
 */
int spp_server_close(int server_id);

/**
 * @if Eng
 * @brief  Connects to a remote device over the socket.
 * @attention This is a synchronous API.
 * @param  [in]  socket_para The params to create a client socket and connect to a remote device.
 * @retval SPP_INVALID_ID Connect fail.
 * @retval others Return a client ID if accept success.
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_disconnect | is_spp_connected | spp_get_remote_addr | spp_read | spp_write
 * @else
 * @brief  连接远端设备。
 * @attention 这是一个同步接口。
 * @param  [in]  socket_para 入参，用于创建Client socket并连接远端设备的参数。
 * @retval SPP_INVALID_ID 连接失败。
 * @retval others 连接成功，返回有效Client ID。
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_disconnect | is_spp_connected | spp_get_remote_addr | spp_read | spp_write
 * @endif
 */
int spp_connect(spp_create_socket_para_t *socket_para, const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Disables a connection and releases related resources.
 * @attention This is a synchronous API.
 * @param  [in]  client_id The relative ID used to identify the current client socket.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_server_accept | spp_connect | is_spp_connected
 * @else
 * @brief  断开连接（同时释放相关资源）。
 * @attention 这是一个同步接口。
 * @param  [in]  client_id 入参，用来指定当前操作Client的ID。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_server_accept | spp_connect | is_spp_connected
 * @endif
 */
int spp_disconnect(int client_id);

/**
 * @if Eng
 * @brief  Get the connection status of this socket.
 * @attention NULL.
 * @param  [in]  client_id The relative ID used to identify the current client socket.
 * @retval true connected.
 * @retval false disconnected.
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_server_accept | spp_connect | spp_disconnect
 * @else
 * @brief  获取指定SPP socket的连接状态。
 * @attention NULL.
 * @param  [in]  client_id 入参，用来指定当前操作Client的ID。
 * @retval true 连接。
 * @retval false 未连接。
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_server_accept | spp_connect | spp_disconnect
 * @endif
 */
bool is_spp_connected(int client_id);

/**
 * @if Eng
 * @brief  Spp get remote device's address.
 * @attention NULL.
 * @param  [in]  client_id The relative ID used to identify the current client socket.
 * @param  [out] remote_addr Remote device's address, memory allocated by caller.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_server_accept | spp_connect | is_spp_connected
 * @else
 * @brief  获取SPP socket对应远端设备的地址。
 * @attention NULL.
 * @param  [in]  client_id 入参，用来指定当前操作Client的ID。
 * @param  [out] remote_addr 出参，远端设备的地址。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_server_accept | spp_connect | is_spp_connected
 * @endif
 */
int spp_get_remote_addr(int client_id, bd_addr_t *remote_addr);

/**
 * @if Eng
 * @brief  Client write data to socket.
 * @attention NULL.
 * @param  [in]  client_id The relative ID used to identify the current client socket.
 * @param  [in]  data     Indicate the data to be written.
 * @param  [in]  len      Indicates the length of the data to be written.
 * @retval SPP_WRITE_FAILED The operation failed.
 * @retval others The actual write length.
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_server_accept | spp_connect | spp_disconnect | is_spp_connected
 * @else
 * @brief  向SPP socket写入数据
 * @attention NULL.
 * @param  [in]  client_id 入参，用来指定当前操作Client的ID。
 * @param  [in]  data 入参，存储待写入数据的buf。
 * @param  [in]  len 入参，待写入数据的长度。
 * @retval SPP_WRITE_FAILED 操作失败。
 * @retval others 操作成功，返回实际写入长度，单位字节。
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_server_accept | spp_connect | spp_disconnect | is_spp_connected
 * @endif
 */
int spp_write(int client_id, const char *data, const unsigned int len);

/**
 * @if Eng
 * @brief Callback function for connection event.
 * @par Description: When registered, this callback reports connection event to upper layer application.
 * @param  [in]  client_id The relative ID used to identify the current client socket.
 * @param  [in]  state    Connect state of spp. See {@link profile_connect_state_t}
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_spp.h
 * @see spp_client_callbacks_t
 * @else
 * @brief 连接变化事件回调函数。
 * @par 说明: 注册该回调函数之后，bluetooth调用该回调函数将连接事件上报给上层。
 * @param  [in]  client_id 入参，用来指定当前操作Client的ID。
 * @param  [in]  state    spp的连接状态。见 {@link profile_connect_state_t}
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_spp.h
 * @see spp_callbacks_t
 * @endif
 */
typedef void (*spp_conn_state_changed_callback)(uint8_t client_id, profile_connect_state_t state);

/**
 * @if Eng
 * @brief Callback function for data reception.
 * @par Description: When registered, this callback reports received data to upper layer application.
 * @param  [in]  client_id The relative ID used to identify the current client socket.
 * @param  [in]  data     Received data.
 * @param  [in]  data_len      Length of data.
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_spp.h
 * @see spp_client_callbacks_t
 * @else
 * @brief 接收到数据回调函数。
 * @par 说明: 注册该回调函数之后，bluetooth调用该回调函数将接收到的数据上报给上层。
 * @param  [in]  client_id 入参，用来指定当前操作Client的ID。
 * @param  [in]  data     接收到的数据。
 * @param  [in]  data_len      数据长度。
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_spp.h
 * @see spp_callbacks_t
 * @endif
 */
typedef void (*spp_receive_data_callback)(uint8_t client_id, uint8_t *data, uint32_t data_len);

/**
 * @if Eng
 * @brief Struct of SPP callback functions.
 * @else
 * @brief SPP回调函数接口定义。
 * @endif
 */
typedef struct {
    spp_receive_data_callback receive_data_cb;
    spp_conn_state_changed_callback conn_state_changed_cb;
} spp_callbacks_t;

/**
 * @if Eng
 * @brief Register callback functions for PBAP.
 * @par Description: Register callback functions for PBAP.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  func  A poniter of the Callback funcs.See @ref spp_client_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_pbap.h
 * @see spp_callbacks_t
 * @else
 * @brief 注册PBAP回调函数。
 * @par 说明: 注册上层应用的回调。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  func  指向回调函数接口定义的指针。见 @ref spp_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_pbap.h
 * @see spp_callbacks_t
 * @endif
 */
int spp_register_callbacks(spp_callbacks_t *func);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif
