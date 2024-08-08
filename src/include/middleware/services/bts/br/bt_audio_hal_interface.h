/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description:  BT AUDIO HAL module.
 */

/**
 * @addtogroup bluetooth_bts_audio_hal AUDIO HAL API
 * @ingroup    bluetooth
 * @{
 */

#ifndef BT_AUDIO_HAL_INTERFACE_H
#define BT_AUDIO_HAL_INTERFACE_H
#include <stdint.h>
#include <td_type.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Max length of bluetooth A2DP Codec Capabilities.
 * @else
 * @brief A2DP编码格式能力最大长度。
 * @endif
 */
#define BT_A2DP_CODECCAPS_MAX_LEN 0x10

/**
 * @if Eng
 * @brief Max length of AVRCP event status.
 * @else
 * @brief :AVRCP事件状态最大长度。
 * @endif
 */
#define BT_AVRCP_EVENT_STATUS_MAX_LEN 0x08

/**
 * @if Eng
 * @brief invalid task ID.
 * @else
 * @brief 蓝牙错误的无效任务ID。
 * @endif
 */
#define BTS_ERR_INVALID_TASKID 0xffffffff

/**
 * @if Eng
 * @brief Define of Bluetooth Audio Channel parameters type.
 * @else
 * @brief 蓝牙通道参数类型定义。
 * @endif
 */
typedef enum {
    BT_AUDIO_PARAM_A2DP_CODEC,      /*!< @if Eng A2DP codec.
                                         @else  A2DP编码格式。 @endif */
    BT_AUDIO_PARAM_AVRCP_VOLUME,    /*!< @if Eng AVRCP volume.
                                         @else  AVRCP音量。 @endif */
} bt_audio_param_type;

/**
 * @if Eng
 * @brief SBC codec type.
 * @else
 * @brief SBC编码格式类型。
 * @endif
 */
#define BT_AUDIO_CODEC_SBC      0x00

/**
 * @if Eng
 * @brief MPEG12 codec type.
 * @else
 * @brief MPEG12编码格式类型。
 * @endif
 */
#define BT_AUDIO_CODEC_MPEG12   0x01
/**
 * @if Eng
 * @brief MPEG24/AAC codec type.
 * @else
 * @brief MPEG24/AAC编码格式类型。
 * @endif
 */
#define BT_AUDIO_CODEC_MPEG24   0x02

/**
 * @if Eng
 * @brief unknown codec type.
 * @else
 * @brief 位置编码格式类型。
 * @endif
 */
#define BT_AUDIO_CODEC_UNKNOWN  0xFF

/**
 * @if Eng
 * @brief SBC capabilitis Octet 1.
 * @else
 * @brief SBC能力字节1。
 * @endif
 */
enum {
    /* sampling_freq, do not need to shift again, already in macro */
    BT_AUDIO_A2DP_SBC_SF_16000 =                0x80,
    BT_AUDIO_A2DP_SBC_SF_32000 =                0x40,
    BT_AUDIO_A2DP_SBC_SF_44100 =                0x20, /* SRC must support 44.1 or 48, SNK must support both */
    BT_AUDIO_A2DP_SBC_SF_48000 =                0x10,
    BT_AUDIO_A2DP_SBC_SF_ALL =                  0xF0,
    BT_AUDIO_A2DP_SBC_CHMODE_MONO =             0x08,
    BT_AUDIO_A2DP_SBC_CHMODE_DUAL_CHANNEL =     0x04,
    BT_AUDIO_A2DP_SBC_CHMODE_STEREO =           0x02,
    BT_AUDIO_A2DP_SBC_CHMODE_JOINT_STEREO =     0x01,
    BT_AUDIO_A2DP_SBC_CHMODE_ALL =              0x0F
};

/**
 * @if Eng
 * @brief SBC capabilitis Octet 2.
 * @else
 * @brief SBC能力字节2。
 * @endif
 */
enum {
    BT_AUDIO_A2DP_SBC_BLOCK_4 =                 0x80, /* block_len, do not need to shift */
    BT_AUDIO_A2DP_SBC_BLOCK_8 =                 0x40,
    BT_AUDIO_A2DP_SBC_BLOCK_12 =                0x20,
    BT_AUDIO_A2DP_SBC_BLOCK_16 =                0x10,
    BT_AUDIO_A2DP_SBC_BLOCK_ALL =               0xF0,
    BT_AUDIO_A2DP_SBC_SUBBAND_4 =               0x08, /* subbands, do not need to shift */
    BT_AUDIO_A2DP_SBC_SUBBAND_8 =               0x04,
    BT_AUDIO_A2DP_SBC_SUBBAND_ALL =             0x0C,
    BT_AUDIO_A2DP_SBC_SNR =                     0x02, /* allocation_method, do not need to shift */
    BT_AUDIO_A2DP_SBC_LOUDNESS =                0x01, /* Default, most headphone only support Loudness */
    BT_AUDIO_A2DP_SBC_ALLOCATION_ALL =          0x03
};

#define BT_AUDIO_A2DP_SBC_MINBITPOOL            0x02
#define BT_AUDIO_A2DP_SBC_MAXBITPOOL            0x44

/* MPEG12 capabilitis */
/* chnl_mode */
#define BT_AUDIO_A2DP_MPEG12_JOINTSTEREO       0x01
#define BT_AUDIO_A2DP_MPEG12_STEREO            0x02
#define BT_AUDIO_A2DP_MPEG12_DUAL              0x04
#define BT_AUDIO_A2DP_MPEG12_MONO              0x08
/* crc */
#define BT_AUDIO_A2DP_MPEG12_CRCSUPPORT        0x01
/* layer */
#define BT_AUDIO_A2DP_MPEG12_LAYER1            0x04
#define BT_AUDIO_A2DP_MPEG12_LAYER2            0x02
#define BT_AUDIO_A2DP_MPEG12_LAYER3            0x01
/* sample_frequency */
#define BT_AUDIO_A2DP_MPEG12_FS48000           0x01
#define BT_AUDIO_A2DP_MPEG12_FS44100           0x02
#define BT_AUDIO_A2DP_MPEG12_FS32000           0x04
#define BT_AUDIO_A2DP_MPEG12_FS24000           0x08
#define BT_AUDIO_A2DP_MPEG12_FS22050           0x10
#define BT_AUDIO_A2DP_MPEG12_FS16000           0x20

/**
 * @if Eng
 * @brief MPEG12 MPF2 support.
 * @else
 * @brief 支持MPEG12 MPF2。
 * @endif
 */
#define BT_AUDIO_A2DP_MPEG12_MPF2SUPPORT       0x01
/* bitrate */
#define BT_AUDIO_A2DP_MPEG12_BITRATE0000        0x0001  /* free */
#define BT_AUDIO_A2DP_MPEG12_BITRATE0001        0x0002  /* 32Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE0010        0x0004  /* 40Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE0011        0x0008  /* 48Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE0100        0x0010  /* 56Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE0101        0x0020  /* 64Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE0110        0x0040  /* 80Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE0111        0x0080  /* 96Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE1000        0x0100  /* 112Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE1001        0x0200  /* 128Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE1010        0x0400  /* 160Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE1011        0x0800  /* 192Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE1100        0x1000  /* 224Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE1101        0x2000  /* 256Kbps for MPEG-1 layer 3 */
#define BT_AUDIO_A2DP_MPEG12_BITRATE1110        0x4000  /* 320Kbps for MPEG-1 layer 3 */
/* vbr */
#define BT_AUDIO_A2DP_MPEG12_VBRSUPPORT        0x01

/* MPEG24(AAC) capabilitis */
#define BT_AUDIO_A2DP_AAC_SF8000                 0x0800
#define BT_AUDIO_A2DP_AAC_SF11025                0x0400
#define BT_AUDIO_A2DP_AAC_SF12000                0x0200
#define BT_AUDIO_A2DP_AAC_SF16000                0x0100
#define BT_AUDIO_A2DP_AAC_SF22050                0x0080
#define BT_AUDIO_A2DP_AAC_SF24000                0x0040
#define BT_AUDIO_A2DP_AAC_SF32000                0x0020
#define BT_AUDIO_A2DP_AAC_SF44100                0x0010
#define BT_AUDIO_A2DP_AAC_SF48000                0x0008
#define BT_AUDIO_A2DP_AAC_SF64000                0x0004
#define BT_AUDIO_A2DP_AAC_SF88200                0x0002
#define BT_AUDIO_A2DP_AAC_SF96000                0x0001
#define BT_AUDIO_A2DP_AAC_SFSEPALL               0x0FFF

/* MPEG AAC Sampling Frequency */
#define BT_AUDIO_A2DP_SMAPLE_RATE8000   8000
#define BT_AUDIO_A2DP_SMAPLE_RATE11025  11025
#define BT_AUDIO_A2DP_SMAPLE_RATE12000  12000
#define BT_AUDIO_A2DP_SMAPLE_RATE16000  16000
#define BT_AUDIO_A2DP_SMAPLE_RATE22050  22050
#define BT_AUDIO_A2DP_SMAPLE_RATE24000  24000
#define BT_AUDIO_A2DP_SMAPLE_RATE32000  32000
#define BT_AUDIO_A2DP_SMAPLE_RATE44100  44100
#define BT_AUDIO_A2DP_SMAPLE_RATE48000  48000
#define BT_AUDIO_A2DP_SMAPLE_RATE64000  64000
#define BT_AUDIO_A2DP_SMAPLE_RATE88200  88200
#define BT_AUDIO_A2DP_SMAPLE_RATE96000  96000

/**
 * @if Eng
 * @brief MPEG AAC Object type MPEG2_LC.
 * @else
 * @brief MPEG AAC Object type MPEG2_LC。
 * @endif
 */
#define BT_AUDIO_A2DP_AAC_MPEG2_LC                0x80

/**
 * @if Eng
 * @brief MPEG AAC Object type MPEG4_LC。.
 * @else
 * @brief MPEG AAC Object type MPEG4_LC。
 * @endif
 */
#define BT_AUDIO_A2DP_AAC_MPEG4_LC                0x40

/**
 * @if Eng
 * @brief MPEG AAC Object type MPEG4_LTP.
 * @else
 * @brief MPEG AAC Object type MPEG4_LTP
 * @endif
 */
#define BT_AUDIO_A2DP_AAC_MPEG4_LTP               0x20

/**
 * @if Eng
 * @brief MPEG AAC Object type MPEG4_SCALABLE.
 * @else
 * @brief MPEG AAC Object type MPEG4_SCALABLE
 * @endif
 */
#define BT_AUDIO_A2DP_AAC_MPEG4_SCALABLE          0x10

/**
 * @if Eng
 * @brief MPEG AAC Object type OBJECT_SEPALL.
 * @else
 * @brief MPEG AAC Object type OBJECT_SEPALL。
 * @endif
 */
#define BT_AUDIO_A2DP_AAC_OBJECT_SEPALL           0xF0

/**
 * @if Eng
 * @brief MPEG AAC Channels 1.
 * @else
 * @brief MPEG AAC Channels 1。
 * @endif
 */
#define BT_AUDIO_A2DP_AAC_CH_1                    0x02

/**
 * @if Eng
 * @brief MPEG AAC Channels 2.
 * @else
 * @brief MPEG AAC Channels 2。
 * @endif
 */
#define BT_AUDIO_A2DP_AAC_CH_2                    0x01

/**
 * @if Eng
 * @brief MPEG AAC VBR Support.
 * @else
 * @brief 支持MPEG AAC VBR。
 * @endif
 */
#define BT_AUDIO_A2DP_AAC_VBR                     0x01

/**
 * @if Eng
 * @brief MPEG AAC NOVBR Support.
 * @else
 * @brief 支持MPEG AAC NOVBR。
 * @endif
 */
#define BT_AUDIO_A2DP_AAC_NOVBR                   0x00

/**
 * @if Eng
 * @brief Define of A2DP codec parameters.
 * @else
 * @brief A2DP 编码格式参数定义。
 * @endif
 */
typedef struct {
    td_u8 codec_type;       /*!< @if Eng codec type.
                                 @else   编码格式类型。 @endif */
    td_u8 cap_len;          /*!< @if Eng codec cap len.
                                 @else   编码格式能力集长度。 @endif */
    td_u16 payload;         /*!< @if Eng Max payload supportet by btc.
                                 @else   btc最大支持载荷。 @endif */
    td_u8 codec_caps[BT_A2DP_CODECCAPS_MAX_LEN]; /*!< @if Eng codec bt_a2dp_sbc_codec_caps, bt_a2dp_mpeg24_codec_caps
                                                      @else   codec bt_a2dp_sbc_codec_caps, bt_a2dp_mpeg24_codec_caps。
                                                      @endif */
} bt_a2dp_codec_param;

/* SBC codec 信息 *//**
 * @if Eng
 * @brief SBC codec info.
 * @else
 * @brief SBC编码格式信息。
 * @endif
 */
typedef struct {
    td_u32 sample_frequency;    /*!< @if Eng Sample frequency, unit Hz.
                                     @else   采样频率，单位Hz。 @endif */
    td_u8 chnl_mode;            /*!< @if Eng See A2DP_CODEC_CHANNEL_MODE_MONO.
                                     @else   参考A2DP_CODEC_CHANNEL_MODE_MONO。 @endif */
    td_u8 block_length;         /*!< @if Eng Block length, value  4 8 12 16.
                                     @else   块长度, 取值 4 8 12 16。 @endif */
    td_u8 subband;              /*!< @if Eng Num of subband, value 4 or 8.
                                     @else   子带个数, 取值 4 or 8。 @endif */
    td_u8 alloc_method;         /*!< @if Eng See A2DP_SBC_ALLOC_METHOD_LOUNDNESS.
                                     @else   参考A2DP_SBC_ALLOC_METHOD_LOUNDNESS。 @endif */
    td_u8 min_bitpool;          /*!< @if Eng Min bitpool value 2~250.
                                     @else   最位池，取值 2~250。 @endif */
    td_u8 max_bitpool;          /*!< @if Eng Max bitpool value 2~250.
                                     @else   最位池，值 2~250。 @endif */
} bt_a2dp_sbc_codec_caps;

/**
 * @if Eng
 * @brief MPEG4 AAC ISO 14496-3 codec info.
 * @else
 * @brief MPEG4 AAC ISO 14496-3编码格式信息。
 * @endif
 */
typedef struct {
    td_u32 sample_frequency;
    td_u32 bit_rate;
    td_u8 channels;
    td_u8 object_type;
    td_u8 rfa;
    td_u8 vbr;
} bt_a2dp_mpeg24_codec_caps;

/**
 * @if Eng
 * @brief Define of Bluetooth Audio Channel event type.
 * @else
 * @brief 蓝牙音频通道事件类型定义。
 * @endif
 */
typedef enum {
    BT_AUDIO_A2DP_STREAM_CREATE,        /* A2DP流创建，数据为流handle，长度为sizeof(td_pvoid) */
    BT_AUDIO_A2DP_STREAM_OPENED,        /* A2DP流打开，数据为 bt_audio_a2dp_stream_open_data，长度为sizeof(td_pvoid) */
    BT_AUDIO_A2DP_STREAM_STRAT,         /* A2DP流开始，数据为流handle，长度为sizeof(td_pvoid) */
    BT_AUDIO_A2DP_STREAM_SUSPENDED,     /* A2DP流暂停，数据为流handle，长度为sizeof(td_pvoid) */
    BT_AUDIO_A2DP_STREAM_CLOSED,        /* A2DP流关闭，数据为流handle，长度为sizeof(td_pvoid) */
    BT_AUDIO_A2DP_STREAM_CONFIG_CHANGE, /* A2DP流参数变更，数据结构为 bt_audio_a2dp_config_chg_data */
    BT_AUDIO_SCO_CONN_ESTABLISHED,      /* SCO连接建立，数据为流handle，长度为sizeof(td_pvoid) */
    BT_AUDIO_SCO_CONN_RELEASED,         /* SCO连接释放，数据为流handle，长度为sizeof(td_pvoid) */
} bt_audio_event_type;

/**
 * @if Eng
 * @brief Define of a2dp stream open event data.
 * @else
 * @brief A2DP流打开事件数据。
 * @endif
 */
typedef struct {
    td_pvoid stream_hdl;
    td_u16 stream_mtu;
    td_u16 frame_size;
    td_u8 num_frame;
    td_u8 pad;
    td_u16 pad1;
} bt_audio_a2dp_stream_open_data;

/**
 * @if Eng
 * @brief Define of a2dp config change data.
 * @else
 * @brief A2DP流参数变更事件数据。
 * @endif
 */
typedef struct {
    td_pvoid stream_hdl;
    bt_a2dp_codec_param codec;
} bt_audio_a2dp_config_chg_data;

/**
 * @if Eng
 * @brief Define of audio port parameter type.
 * @else
 * @brief 音频端口类型定义。
 * @endif
 */
typedef enum {
    A2DP,
    SCO_IN,
    SCO_OUT,
    BT_AUDIO_PORT_MAX
} bt_audio_port_type;

/**
 * @if Eng
 * @brief Audio port parameters.
 * @else
 * @brief 音频端口参数。
 * @endif
 */
typedef struct {
    bt_audio_port_type port_type;   /*!< @if Eng Audio port parameter type.
                                         @else   音频端口参数类型。 @endif */
    union {
        td_u32 share_mem_id;        /*!< @if Eng Share memery ID.
                                         @else   共享内存ID。 @endif */
    };
} bt_audio_port_params;

/**
 * @if Eng
 * @brief  Callback invoked in response to a2dp state event changed.
 * @par Description:
 *           When registered, the bts uses this callback to deliver a2dp state event
 *           after state event changed.
 * @attention  1. This function is called in bts context, should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  type       A2dp state event. See @ref bt_audio_event_type
 * @param  [in]  data       Data carry in the a2dpstate event.
 * @param  [in]  size       Data size carry in the a2dpstate event.
 * @param  [in]  context    Context carry in the a2dpstate event.
 * @retval No return value. See @ref void
 * @else
 * @brief  A2dp 状态事件改变回调函数。
 * @par 说明:
 *           注册该回调函数之后，BTS每次完成a2dp状态更新后调用该接口反馈结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  type       A2dp状态事件。参考 @ref bt_audio_event_type
 * @param  [in]  data       A2dp状态事件携带的数据。
 * @param  [in]  size       A2dp状态事件携带的数据大小。
 * @param  [in]  context    A2dp状态事件携带的上下文。
 * @retval 无返回值。参考 @ref void
 * @endif
 * @par Dependency:
 *            @li bt_audio_hal_interface.h
 */
typedef td_void (*bt_audio_listener_cb)(bt_audio_event_type type, const td_void *data, int32_t size, td_void *context);

/**
 * @if Eng
 * @brief  Use this funtion to register callback function of audio listener.
 * @par Description:
 *           Use this funtion to register callback function of audio listener.
 * @param  [in]  cb         A poniter of the callback function of audio listener. See @ref bt_audio_listener_cb
 * @retval #EXT_ERR_SUCCESS Success.
 * @retval Other            Failure. For details, see @ref ext_errno
 * @else
 * @brief   注册音频监听回调。
 * @par 说明:
 *            注册音频监听回调。
 * @param  [in]  cb         音频监听回调函数指针。
 * @retval #EXT_ERR_SUCCESS 成功。
 * @retval Other            失败，参考 @ref ext_errno
 * @endif
 * @par Dependency:
 *            @li bt_audio_hal_interface.h
 */
td_u32 bt_register_audio_listener(bt_audio_listener_cb cb, td_void *context);

/**
 * @if Eng
 * @brief  Use this funtion to deregister callback function of audio listener.
 * @par Description:
 *           Use this funtion to deregister callback function of audio listener.
 * @param  [in]  cb         A poniter of the callback function of audio listener. See @ref bt_audio_listener_cb
 * @retval #EXT_ERR_SUCCESS Success.
 * @retval Other            Failure. For details, see @ref ext_errno
 * @else
 * @brief   取消注册音频监听回调。
 * @par 说明:
 *            取消注册音频监听回调。
 * @param  [in]  cb         音频监听回调函数指针。
 * @retval #EXT_ERR_SUCCESS 成功。
 * @retval Other            失败，参考 @ref ext_errno
 * @endif
 * @par Dependency:
 *            @li bt_audio_hal_interface.h
 */
td_u32 bt_deregister_audio_listener(bt_audio_listener_cb cb);

/**
 * @if Eng
 * @brief  Use this funtion to start Bluetooth Audio Channel Data Transfer.
 * @par Description:
 *           Use this funtion to start Bluetooth Audio Channel Data Transfer.
 * @param  [in]  stream_hdl A handle of the stream.
 * @retval #EXT_ERR_SUCCESS Success.
 * @retval Other            Failure. For details, see @ref ext_errno
 * @else
 * @brief   启动蓝牙音频通道数据传输。
 * @par 说明:
 *            启动蓝牙音频通道数据传输。
 * @param  [in]  stream_hdl 流句柄。
 * @retval #EXT_ERR_SUCCESS 成功。
 * @retval Other            失败，参考 @ref ext_errno
 * @endif
 * @par Dependency:
 *            @li bt_audio_hal_interface.h
 */
td_u32 bt_start_audio_stream(td_pvoid stream_hdl);

/**
 * @if Eng
 * @brief  Use this funtion to pause Bluetooth Audio Channel Data Transfer.
 * @par Description:
 *           Use this funtion to pause Bluetooth Audio Channel Data Transfer.
 * @param  [in]  stream_hdl A handle of the stream.
 * @retval #EXT_ERR_SUCCESS Success.
 * @retval Other            Failure. For details, see @ref ext_errno
 * @else
 * @brief   暂停蓝牙音频通道数据传输。
 * @par 说明:
 *            暂停蓝牙音频通道数据传输。
 * @param  [in]  stream_hdl 流句柄。
 * @retval #EXT_ERR_SUCCESS 成功。
 * @retval Other            失败，参考 @ref ext_errno
 * @endif
 * @par Dependency:
 *            @li bt_audio_hal_interface.h
 */
td_u32 bt_pause_audio_stream(td_pvoid stream_hdl);

/**
 * @if Eng
 * @brief  Use this funtion to stop Bluetooth Audio Channel Data Transfer.
 * @par Description:
 *           Use this funtion to stop Bluetooth Audio Channel Data Transfer.
 * @param  [in]  stream_hdl A handle of the stream.
 * @retval #EXT_ERR_SUCCESS Success.
 * @retval Other            Failure. For details, see @ref ext_errno
 * @else
 * @brief   停止蓝牙音频通道数据传输。
 * @par 说明:
 *            停止蓝牙音频通道数据传输。
 * @param  [in]  stream_hdl 流句柄。
 * @retval #EXT_ERR_SUCCESS 成功。
 * @retval Other            失败，参考 @ref ext_errno
 * @endif
 * @par Dependency:
 *            @li bt_audio_hal_interface.h
 */
td_u32 bt_stop_audio_stream(td_pvoid stream_hdl);

/**
 * @if Eng
 * @brief  Use this funtion to attach an Audio Port to a Bluetooth Audio Channel.
 * @par Description:
 *           Use this funtion to attach an Audio Port to a Bluetooth Audio Channel.
 * @param  [in]  stream_hdl A handle of the stream.
 * @param  [in]  param      BT audio port parameter type. See @ref bt_audio_port_params
 * @retval #EXT_ERR_SUCCESS Success.
 * @retval Other            Failure. For details, see @ref ext_errno
 * @else
 * @brief   为蓝牙音频通道绑定音频端口。
 * @par 说明:
 *            为蓝牙音频通道绑定音频端口。
 * @param  [in]  stream_hdl 流句柄。
 * @param  [in]  param      蓝牙音频端口参数。参考 @ref bt_audio_port_params
 * @retval #EXT_ERR_SUCCESS 成功。
 * @retval Other            失败，参考 @ref ext_errno
 * @endif
 * @par Dependency:
 *            @li bt_audio_hal_interface.h
 */
td_u32 bt_attach_audio_port(td_pvoid stream_hdl, bt_audio_port_params *param);

/**
 * @if Eng
 * @brief  Use this funtion to detach an Audio Port to a Bluetooth Audio Channel.
 * @par Description:
 *           Use this funtion to detach an Audio Port to a Bluetooth Audio Channel.
 * @param  [in]  stream_hdl A handle of the stream.
 * @param  [in]  param      BT audio port parameter type. See @ref bt_audio_port_params
 * @retval #EXT_ERR_SUCCESS Success.
 * @retval Other            Failure. For details, see @ref ext_errno
 * @else
 * @brief   为蓝牙音频通道取消绑定音频端口。
 * @par 说明:
 *            为蓝牙音频通道取消绑定音频端口。
 * @param  [in]  stream_hdl 流句柄。
 * @param  [in]  param      蓝牙音频端口参数。参考 @ref bt_audio_port_params
 * @retval #EXT_ERR_SUCCESS 成功。
 * @retval Other            失败，参考 @ref ext_errno
 * @endif
 * @par Dependency:
 *            @li bt_audio_hal_interface.h
 */
td_u32 bt_detach_audio_port(td_pvoid stream_hdl, bt_audio_port_params *param);

/**
 * @if Eng
 * @brief  Use this funtion to set the parameters of a specified Bluetooth audio channel.
 * @par Description:
 *           Use this funtion to set the parameters of a specified Bluetooth audio channel,
             such as the encoding format and data format.
 * @attention 1. When the A2DP is not connected, you can directly set this parameter.
 * @attention 2. When A2DP is connected, there are two scenarios:
 * @attention 2.1. Codec switchover of different categories, this parameter needs to be set when the stream is closed
 * @attention      and then renegotiation is performed when the stream is opened. The renegotiation result may not be
 * @attention      the same as the setting.
 * @attention 2.2. Codec switchover of the same category, this parameter needs to be set when the stream is in the
 * @attention      OPEN state. That is, if the stream is in the START state, you need to suspend the codec and then
 * @attention      invoke this interface to set the codec.
 * @param  [in]  stream_hdl A handle of the stream.
 * @param  [in]  type       BT audio parameter type. See @ref bt_audio_param_type
 * @param  [in]  params     The parameters of Bluetooth audio channel.
 * @param  [in]  len        The parameters lenght of Bluetooth audio channel.
 * @retval #EXT_ERR_SUCCESS Success.
 * @retval Other            Failure. For details, see @ref ext_errno
 * @else
 * @brief 设置指定蓝牙音频通道的参数。
 * @par 说明:
 *           目前只支持设置A2DP CODEC。
 * @attention  1. 在A2DP未连接情况下，可直接设置。
 * @attention  2. 在A2DP已连接情况下，分两种场景：
 * @attention  2.1. 不同大类codec切换：需要在stream CLOSE的状态下设置，然后在OPEN时进行重新协商，且不保证重协商结果
 * @attention        与设置完全一致。
 * @attention  2.2. 相同大类codec切换：需要在stream OPEN的状态下设置，即若处于START状态需要先SUSPEND再调本接口设置。
 * @param  [in]  stream_hdl 流句柄。
 * @param  [in]  type       蓝牙音频参数类型。参考 @ref bt_audio_param_type
 * @param  [in]  params     设置的蓝牙音频通道的参数。
 * @param  [in]  len        设置的蓝牙音频通道的参数长度。
 * @retval #EXT_ERR_SUCCESS 成功。
 * @retval Other            失败，参考 @ref ext_errno
 * @endif
 * @par Dependency:
 *            @li bt_audio_hal_interface.h
 */
td_u32 bt_set_audio_parameter(td_pvoid stream_hdl, bt_audio_param_type type, td_void *params, int32_t len);

/**
 * @if Eng
 * @brief  Use this funtion to get the parameters of a specified Bluetooth audio channel.
 * @par Description:
 *           Use this funtion to get the parameters of a specified Bluetooth audio channel,
             such as the encoding format and data format.
 * @param  [in]  stream_hdl A handle of the stream.
 * @param  [in]  type       BT audio parameter type. See @ref bt_audio_param_type
 * @param  [out] params     The parameters of Bluetooth audio channel.
 * @param  [out] len        The parameters lenght of Bluetooth audio channel.
 * @retval #EXT_ERR_SUCCESS Success.
 * @retval Other            Failure. For details, see @ref ext_errno
 * @else
 * @brief  获取指定蓝牙音频通道的参数。
 * @par 说明:
 *           获取指定蓝牙音频通道的参数，如编码格式、数据格式等。
 * @param  [in]  stream_hdl 流句柄。
 * @param  [in]  type       蓝牙音频参数类型。参考 @ref bt_audio_param_type
 * @param  [out] params     获取的蓝牙音频通道的参数。
 * @param  [out] len        获取的蓝牙音频通道的参数长度。
 * @retval #EXT_ERR_SUCCESS 成功。
 * @retval Other            失败，参考 @ref ext_errno
 * @endif
 * @par Dependency:
 *            @li bt_audio_hal_interface.h
 */
td_u32 bt_get_audio_parameter(td_pvoid stream_hdl, bt_audio_param_type type, td_void *params, int32_t len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* BT_AUDIO_HAL_INTERFACE_H */
