/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT A2DP-SRC module.
 */

/**
 * @addtogroup bluetooth_bts_a2dp_source A2DP SRC API
 * @ingroup    bluetooth
 * @{
 */

#ifndef BTS_A2DP_SOURCE_H
#define BTS_A2DP_SOURCE_H

#include "bts_def.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Setting of A2dp codec priority.
 * @else
 * @brief A2dp 编码格式优先级设置。
 * @endif
 */
#define A2DP_CODEC_PRIORITY_DISABLED (-1)
#define A2DP_CODEC_PRIORITY_DEFAULT 0
#define A2DP_CODEC_PRIORITY_HIGHEST (1000 * 1000)

/**
 * @if Eng
 * @brief Sample rate of a2dp.
 * @else
 * @brief A2dp 采样率。
 * @endif
 */
#define A2DP_CODEC_SAMPLE_RATE_NONE 0x00
#define A2DP_CODEC_SAMPLE_RATE_44100 (0x01 << 0)
#define A2DP_CODEC_SAMPLE_RATE_48000 (0x01 << 1)
#define A2DP_CODEC_SAMPLE_RATE_88200 (0x01 << 2)
#define A2DP_CODEC_SAMPLE_RATE_96000 (0x01 << 3)
#define A2DP_CODEC_SAMPLE_RATE_176400 (0x01 << 4)
#define A2DP_CODEC_SAMPLE_RATE_192000 (0x01 << 5)

/**
 * @if Eng
 * @brief Sample bit of a2dp sample rate.
 * @else
 * @brief A2dp 的采样比特位。
 * @endif
 */
#define A2DP_CODEC_BITS_PER_SAMPLE_NONE 0x00
#define A2DP_CODEC_BITS_PER_SAMPLE_16 (0x01 << 0)
#define A2DP_CODEC_BITS_PER_SAMPLE_24 (0x01 << 1)
#define A2DP_CODEC_BITS_PER_SAMPLE_32 (0x01 << 2)

/**
 * @if Eng
 * @brief Channel mode of a2dp NONE.
 * @else
 * @brief A2dp声道模式NONE。
 * @endif
 */
#define A2DP_CODEC_CHANNEL_MODE_NONE 0x00

/**
 * @if Eng
 * @brief Channel mode of a2dp MONO.
 * @else
 * @brief A2dp声道模式MONO。
 * @endif
 */
#define A2DP_CODEC_CHANNEL_MODE_MONO (0x01 << 0)
/**
 * @if Eng
 * @brief Channel mode of a2dp STEREO.
 * @else
 * @brief A2dp声道模式STEREO。
 * @endif
 */
#define A2DP_CODEC_CHANNEL_MODE_STEREO (0x01 << 1)

/**
 * @if Eng
 * @brief Enum of a2dp codec type.
 * @else
 * @brief A2dp编码格式类型。
 * @endif
 */
typedef enum {
    A2DP_CODEC_TYPE_SBC = 0x00,        /*!< @if Eng SBC codec type.
                                            @else   编码格式类型为SBC。 @endif */
    A2DP_CODEC_TYPE_AAC,               /*!< @if Eng AAC codec type.
                                            @else   编码格式类型为AAC。 @endif */
    A2DP_CODEC_TYPE_APTX,              /*!< @if Eng APTX codec type.
                                            @else   编码格式类型为APTX。 @endif */
    A2DP_CODEC_TYPE_APTX_HD,           /*!< @if Eng APTX_HD codec type.
                                            @else   编码格式类型为APTX_HD。 @endif */
    A2DP_CODEC_TYPE_LDAC,              /*!< @if Eng LDAC codec type.
                                            @else   编码格式类型为LDAC。 @endif */
    A2DP_CODEC_TYPE_LHDC,              /*!< @if Eng LHDC codec type.
                                            @else   编码格式类型为LHDC。 @endif */
    A2DP_CODEC_TYPE_LC3,               /*!< @if Eng LC3 codec type.
                                            @else   编码格式类型为LC3。 @endif */
    A2DP_CODEC_TYPE_L2HC,              /*!< @if Eng L2HC codec type.
                                            @else   编码格式类型为L2HC。 @endif */
    A2DP_CODEC_TYPE_INVALID = 0xF4240, /*!< @if Eng Invalid codec type.
                                            @else   无效编码格式类型。 @endif */
} a2dp_codec_type_t;

/**
 * @if Eng
 * @brief Enum of a2dp optional codec support state define.
 * @else
 * @brief A2dp配置编码格式支持状态。
 * @endif
 */
typedef enum {
    A2DP_OPTIONAL_CODEC_NOT_SUPPORT = 0x00, /*!< @if Eng Not support optional codec.
                                                 @else   不支持设置编码格式类型。 @endif */
    A2DP_OPTIONAL_CODEC_SUPPORT,            /*!< @if Eng Support optional codec.
                                                 @else   支持设置编码格式类型。 @endif */
    A2DP_OPTIONAL_CODEC_SUPPORT_UNKNOWN,    /*!< @if Eng Support user define codec.
                                                 @else   支持用户定义编码格式类型。 @endif */
} a2dp_optional_codec_support_state_t;

/**
 * @if Eng
 * @brief Enum of a2dp src codec configuration information.
 * @else
 * @brief A2dp src配置编码格式参数。
 * @endif
 */
typedef struct {
    int codec_priority;            /*!< @if Eng Codec priority.
                                        @else   编码格式优先级。 @endif */
    unsigned int codec_type;       /*!< @if Eng Codec type.
                                        @else   编码格式类型。 @endif */
    unsigned char sample_rate;     /*!< @if Eng Codec sample rate.
                                        @else   编码器采样率。 @endif */
    unsigned char bits_per_sample; /*!< @if Eng Codec bits per sampl.
                                        @else   采样比特位。 @endif */
    unsigned char channel_mode;    /*!< @if Eng Codec channel mode.
                                        @else   声道模式。 @endif */
    unsigned long codec_specific1; /*!< @if Eng Codec specific value 1.
                                        @else   自定义参数1。 @endif */
    unsigned long codec_specific2; /*!< @if Eng Codec specific value 2.
                                        @else   自定义参数2。 @endif */
    unsigned long codec_specific3; /*!< @if Eng Codec specific value 3.
                                        @else   自定义参数3。 @endif */
    unsigned long codec_specific4; /*!< @if Eng Codec specific value 4.
                                        @else   自定义参数4。 @endif */
} a2dp_codec_info_t;


/**
 * @if Eng
 * @brief  Callback invoked in response to a2dp src connection state changed.
 * @par Description:
 *           When registered, the bts uses this callback to deliver a2dp connect stat info updated
 *           after connection state updated.
 * @attention  1. This function is called in bts context, should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  bd_addr        A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @param  [in]  conn_state     A2dp src connection state. See @ref int
 * @retval No return value. See  @ref void
 * @else
 * @brief  a2dp src连接状态改变回调函数。
 * @par 说明:
 *           注册该回调函数之后，BTS每次完成a2dp状态更新后调用该接口反馈结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bd_addr        对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @param  [in]  conn_state     A2dp src的连接状态。参考 @ref int
 * @retval 无返回值。 参考 @ref void
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 * @see  a2dp_src_callbacks_t
 */
typedef void (*a2dp_src_connect_state_changed_callback)(bd_addr_t *bd_addr, int conn_state);

/**
 * @if Eng
 * @brief  Callback invoked in response to a2dp playing state changed.
 * @par Description:
 *           When registered, the bts uses this callback to deliver a2dp playing stat after playing state updated.
 * @attention  1. This function is called in bts context, should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  bd_addr            A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @param  [in]  playing_state      A2dp src playingState state. See @ref int
 * @retval No return value. See  @ref void
 * @else
 * @brief A2dp src播放状态改变回调函数。
 * @par 说明:
 *           注册该回调函数之后，BTS每次完成a2dp状态更新后调用该接口反馈结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bd_addr            对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @param  [in]  playing_state      A2dp src的播放状态。参考 @ref int
 * @retval 无返回值。参考 @ref void
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 * @see  a2dp_src_callbacks_t
 */
typedef void (*a2dp_src_playing_state_changed_callback)(bd_addr_t *bd_addr, int playing_state);

/**
 * @if Eng
 * @brief  Callback invoked in response to a2dp codec information changed.
 * @par Description:
 *           When registered, the bts uses this callback to deliver a2dp codec information
 *           after codec information changed.
 * @attention  1. This function is called in bts context, should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @param  [in]  info       A poniter of device's codec information. See @ref a2dp_codec_info_t
 * @retval No return value. See @ref void
 * @else
 * @brief  A2dp 编码格式改变回调函数。
 * @par 说明:
 *           注册该回调函数之后，BTS每次完成a2dp状态更新后调用该接口反馈结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @param  [in]  info       A2dp 的编码格式信息。参考 @ref a2dp_codec_info_t
 * @retval 无返回值。参考 @ref void
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 * @see  a2dp_src_callbacks_t
 */
typedef void (*a2dp_src_configuration_changed_callback)(bd_addr_t *bd_addr, a2dp_codec_info_t *info);

/**
 * @if Eng
 * @brief Struct of a2dp src callback function.
 * @else
 * @brief A2dp src回调接口定义。
 * @endif
 */
typedef struct {
    a2dp_src_connect_state_changed_callback connectstate_changed_cb;
    a2dp_src_playing_state_changed_callback playing_state_changed_cb;
    a2dp_src_configuration_changed_callback configuration_changed_cb;
} a2dp_src_callbacks_t;

/**
 * @if Eng
 * @brief  Use this funtion to get device connection state by address.
 * @par Description:
 *           Use this funtion to get device connection state by address.

 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval Return connecting/connected/disconnecting/disconnected. See @ref profile_connect_state_t
 * @else
 * @brief  通过设备地址查询设备连接状态。
 * @par 说明:
 *           通过设备地址查询设备连接状态。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval 返回值为正在连接，已连接，正在断开连接，已断开连接。参考 @ref profile_connect_state_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
int a2dp_src_get_device_connect_state(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to get device playing state by address when target device is on connected state.
 * @par Description:
 *           Use this funtion to get device playing state by address when target device is on connected state.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval Return not playing/is playing. See @ref a2dp_playing_state_t
 * @else
 * @brief  当对端设备已经连接时，通过设备地址查询对端蓝牙设备播放状态。
 * @par 说明:
 *           当对端设备已经连接时，通过设备地址查询对端蓝牙设备播放状态。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval 返回值为未播放/正在播放。参考 @ref a2dp_playing_state_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
int a2dp_src_get_playing_state(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to connect to the peer bluetooth device by address.
 * @par Description:
 *           Use this funtion to connect to the peer bluetooth device by address.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  与对端设备建立a2dp连接。
 * @par 说明:
 *           与对端设备建立a2dp连接。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
int a2dp_src_connect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to disconnect to the peer bluetooth device by address.
 * @par Description:
 *           Use this funtion to disconnect to the peer bluetooth device by address.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  与对端设备断开a2dp连接。
 * @par 说明:
 *           与对端设备断开a2dp连接。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
int a2dp_src_disconnect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to get active device address.
 * @par Description:
 *           Use this funtion to get active device address.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval NOT NULL     Success.
 * @retval NULL         Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  查询对端活跃蓝牙设备的地址。
 * @par 说明:
 *           查询对端活跃蓝牙设备的地址。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval NOT NULL     成功。
 * @retval NULL         失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
bd_addr_t a2dp_src_get_active_device(void);

/**
 * @if Eng
 * @brief  Use this funtion to set the status of Audio start streaming.
 * @par Description:
 *           Use this funtion to set the status of Audio start streaming.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  设置蓝牙音乐的播放为start状态。
 * @par 说明:
 *           设置蓝牙音乐的播放为start状态。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
int a2dp_src_start_playing(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to set the status of Audio suspend streaming.
 * @par Description:
 *           Use this funtion to set the status of Audio suspend streaming.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  设置蓝牙音乐的播放为suspend状态。
 * @par 说明:
 *           设置蓝牙音乐的播放为suspend状态。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
int a2dp_src_suspend_playing(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to set the status of Audio stop streaming.
 * @par Description:
 *           Use this funtion to set the status of Audio stop streaming.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  设置蓝牙音乐的播放为stop状态。
 * @par 说明:
 *           设置蓝牙音乐的播放为stop状态。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
int a2dp_src_stop_playing(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to register callback function of framework.
 * @par Description:
 *           Use this funtion to register callback function of framework.
 * @param  [in]  func   A poniter of the callback function. See @ref a2dp_src_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  注册上层应用的回调。
 * @par 说明:
 *           注册上层应用的回调。
 * @param  [in]  func   回调函数指针。参考 @ref a2dp_src_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
int a2dp_src_register_callbacks(a2dp_src_callbacks_t *func);

/**
 * @if Eng
 * @brief  Use this funtion to deregister callback function of framework.
 * @par Description:
 *           Use this funtion to deregister callback function of framework.
 * @param  [in]  func   A poniter of the callback function. See @ref a2dp_src_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  去注册上层应用的回调。
 * @par 说明:
 *           去注册上层应用的回调。
 * @param  [in]  func   回调函数指针。参考 @ref a2dp_src_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
int a2dp_src_deregister_callbacks(a2dp_src_callbacks_t *func);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* BTS_A2DP_SOURCE_H */
