/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT AUDIO module.
 */

/**
 * @defgroup bluetooth_bt_audio AUDIO API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BT_AUDIO_H
#define BT_AUDIO_H
#include <td_type.h>
#ifdef __cplusplus
extern "C" {
#endif

#define BT_AVRCP_EVENT_STATUS_MAX_LEN 0x08

/**
 * @if Eng
 * @brief  Struct of avrcp event param of TG.
 * @else
 * @brief  TG返回的GetElementAttributes响应参数,在接口 {@link AvrcpCtGetElementAttributeCallback} 中使用。
 * @endif
 */
typedef struct {
    td_u8 event_id;       /*!< @if Eng  event id @else 事件id @endif */
    td_u8 pad[3];         /*!< @if Eng  reserved  @else 预留 @endif */
    td_u8 event_status[BT_AVRCP_EVENT_STATUS_MAX_LEN];  /*!< @if Eng  event status @else 事件的状态信息 @endif */
} bt_avrcp_tg_evt_status_param;

/**
 * @if Eng
 * @brief  Callback invoked in response to avrcp pass through.
 * @par Description:
 *          Callback invoked in response to avrcp pass through.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @param  [in]  key_operation type  #td_u32, key operation.
 * @param  [in]  key_value type  #td_u32, key value.
 * @retval #void  no return value.
 * @par Dependency:
 * @li bt_audio.h
 * @see  bt_avrcp_tg_bts_cbk
 * @else
 * @brief  avrcp pass through响应回调函数。
 * @par 说明:avrcp pass through响应回调函数。
 * @attention 1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param  [in]  key_operation type  #td_u32, 按键操作。
 * @param  [in]  key_value type  #td_u32, 按键值。
 * @retval #void  无返回值。
 * @par 依赖:
 * @li bt_audio.h
 * @see  bt_avrcp_tg_bts_cbk
 * @endif
 */
typedef td_void (*bt_avrcp_tg_pass_through_response_cbks)(td_u32 key_operation, td_u32 key_value);

/**
 * @if Eng
 * @brief  Callback invoked in getting media status by bts.
 * @par Description:
 *         Callback invoked in getting media status by bts.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  event_status_param type  #bt_avrcp_tg_evt_status_param, media status of the avrcp tg.
 * @retval #void  no return value.
 * @par Dependency:
 * @li bt_audio.h
 * @see  bt_avrcp_tg_bts_cbk
 * @else
 * @brief  蓝牙服务查询媒体状态信息的回调函数。
 * @par 说明:蓝牙服务查询媒体状态信息的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  event_status_param type  #bt_avrcp_tg_evt_status_param, avrcp tg端的媒体状态信息。
 * @retval #void  无返回值
 * @par 依赖:
 * @li bt_audio.h
 * @see  bt_avrcp_tg_bts_cbk
 * @endif
 */
typedef td_void (*bt_avrcp_tg_get_media_status_cbks)(bt_avrcp_tg_evt_status_param *event_status_param);

/**
 * @if Eng
 * @brief  Struct of Avrcp TG media callback function.
 * @else
 * @brief  Avrcp TG媒体回调函数接口定义。
 * @endif
 */
typedef struct {
    bt_avrcp_tg_pass_through_response_cbks notify_pass_through_status_cbk;
    bt_avrcp_tg_get_media_status_cbks   get_media_status_cbk;
} bt_avrcp_tg_bts_cbk;

/**
 * @if Eng
 * @brief  Use this funtion to register callback function of avrcp tg media.
 * @par Description:
 *           Use this funtion to register callback function of avrcp tg media.
 * @attention  NULL
 * @param  [in]  func type  #bt_avrcp_tg_bts_cbk *, a poniter of the callback funcs.
 * @retval #td_void  no return value.
 * @par Dependency:
 * @li bt_audio.h
 * @else
 * @brief  媒体注册Avrcp TG回调函数。
 * @par 说明:媒体注册Avrcp TG回调函数。
 * @attention  NULL
 * @param  [in]  func type  #bt_avrcp_tg_bts_cbk *, 指向回调函数接口定义的指针, 参考 {@link bt_avrcp_tg_bts_cbk}。
 * @retval #td_void  无返回值。
 * @par 依赖:
 * @li bt_audio.h
 * @endif
 */
td_void bt_avrcp_tg_register_audio_cbk(bt_avrcp_tg_bts_cbk *func);

/**
 * @if Eng
 * @brief  Use this funtion to deregister callback function of avrcp tg media.
 * @par Description:
 *           Use this funtion to deregister callback function of avrcp tg media.
 * @attention  NULL
 * @param  [in]  #td_void
 * @retval #td_void  no return value.
 * @par Dependency:
 * @li bt_audio.h
 * @else
 * @brief  媒体去注册Avrcp TG回调函数。
 * @par 说明:媒体去注册Avrcp TG回调函数。
 * @attention  NULL
 * @param  [in]  #td_void
 * @retval #td_void  无返回值。
 * @par 依赖:
 * @li bt_audio.h
 * @endif
 */
td_void bt_avrcp_tg_degister_audio_cbk(td_void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* __BT_A2DP_SERVICE_H__ */
