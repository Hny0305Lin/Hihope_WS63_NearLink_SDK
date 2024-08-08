/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: slp api
 * ROM      : NO
 * STRATEGY : NO
 */

/**
 * @defgroup middleware_service_slp_api SLP API
 * @ingroup  middleware_service_slp
 * @{
 */

#ifndef SLP_H
#define SLP_H

#include <stdint.h>
#include <stdbool.h>
#include "slp_errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  slp max device number
 * @else
 * @brief  slp最大设备数
 * @endif
 */
#define SLP_MAX_DEVICE_NUM 16

/**
 * @if Eng
 * @brief  slp addr length, unit: byte
 * @else
 * @brief  slp地址长度，单位：字节
 * @endif
 */
#define SLP_ADDR_LEN 6

/**
 * @if Eng
 * @brief Enum of slp device address
 * @else
 * @brief slp设备地址
 * @endif
 */
typedef struct {
    uint8_t addr[SLP_ADDR_LEN];     /*!< @if Eng address
                                         @else 地址 @endif */
} SlpDeviceAddr;

/**
 * @if Eng
 * @brief Enum of slp security mode
 * @else
 * @brief slp安全模式
 * @endif
 */
typedef enum {
    SLP_SEC_MODE_LPLS = 0,          /*!< @if Eng slp low power consumption low security mode
                                     @else slp低功耗低安全模式 @endif */
    SLP_SEC_MODE_HPHS               /*!< @if Eng slp high power consumption high security mode
                                     @else slp高功耗高安全模式 @endif */
} SlpSecMode;

/**
 * @if Eng
 * @brief Enum of slp encryption mode
 * @else
 * @brief slp加密模式
 * @endif
 */
typedef enum {
    SLP_ENCRYPT_MODE_AES128 = 0,       /*!< @if Eng slp aes-128 encryption mode
                                     @else slp aes-128 加密模式 @endif */
    SLP_ENCRYPT_MODE_SM4               /*!< @if Eng slp sm4 encryption mode
                                     @else slp sm4加密模式 @endif */
} SlpEncryptMode;

/**
 * @if Eng
 * @brief Enum of slp session key mode
 * @else
 * @brief slp会话密钥模式
 * @endif
 */
typedef enum {
    SLP_WB_SHARED_KEY = 0,       /*!< @if Eng slp wideband shared key
                                  @else slp 宽带共享密钥 @endif */
    SLP_NB_SESSION_KEY           /*!< @if Eng slp wideband shared key
                                  @else slp 窄带会话密钥 @endif */
} SlpSessionKeyMode;

/**
 * @if Eng
 * @brief Struct of slp security parameters
 * @else
 * @brief slp安全参数
 * @endif
 */
typedef struct {
    uint8_t secMode;            /*!< @if Eng security mode, @ref SlpSecMode
                                     @else 安全模式，参考 { @ref SlpSecMode }. @endif */
    uint8_t encryptMode;        /*!< @if Eng encrypt mode, @ref SlpEncryptMode
                                     @else 加密模式，参考 { @ref SlpEncryptMode }. @endif */
    uint8_t sessionKeyMode;     /*!< @if Eng session key mode, @ref SlpSessionKeyMode
                                     @else 会话密钥模式，参考 { @ref SlpSessionKeyMode }. @endif */
    uint8_t sessionKeyIdx;      /*!< @if Eng session key index when sessionKeyMode is WB_SHARED_KEY, 0~15
                                     @else 会话密钥索引值，会话密钥模式是宽带共享密钥时有效，取值范围0~15 @endif */
} SlpSecurityParam;

/**
 * @if Eng
 * @brief Struct of slp frame parameters
 * @else
 * @brief slp帧参数
 * @endif
 */
typedef struct {
    uint8_t codeLen;            /*!< @if Eng preamble code length, 0-31(16), 1-63(8), 2-91(4), 3-127(4)
                                     @else 前导码长度 @endif */
    uint8_t syncCodeIdx;        /*!< @if Eng sync code index, codeLen31/91: 0~7, codeLen63: 0~5, codeLen127: 0~15
                                     @else sync段的码字索引值 @endif */
    uint8_t syncSymNum;         /*!< @if Eng sync symbol number, 0-16, 1-32, 2-64, 3-96, 4-128, 5-256
                                     @else sync段的符号数量 @endif */
    uint8_t ctsSegNum;          /*!< @if Eng segment number of cts, 0-1, 1-2, 2-4, 3-8, 4-16, 5-32
                                     @else cts域中segment段的数量 @endif */
    uint8_t ctsSegSymNum;       /*!< @if Eng segment symbol number, 0-16, 1-32
                                     @else 一个segment段中的符号数量 @endif */
    uint8_t gapBaseSymNum;      /*!< @if Eng gap base symbol number, 0-12, 1-25, 2-50, 3-125, 4-250, 5-500, 6-1000
                                     @else gap段基准的符号数量 @endif */
    uint8_t ctsCpLen;           /*!< @if Eng cts cp length, 0-0, 1-64, 2-96, 3-128
                                     @else cts段中cp长度 @endif */
} SlpFrameParam;

/**
 * @if Eng
 * @brief Enum of slp channel
 * @else
 * @brief slp信道
 * @endif
 */
typedef enum {
    SLP_CH_5 = 0,        /*!< @if Eng slp channel 5, center frequency 6489.6MHz
                              @else slp信道5，中心频率6489.6MHz @endif */
    SLP_CH_6,            /*!< @if Eng slp channel 6, center frequency 6988.8MHz
                              @else slp信道6，中心频率6988.8MHz @endif */
    SLP_CH_8,            /*!< @if Eng slp channel 8, center frequency 7488MHz
                              @else slp信道8，中心频率7488MHz @endif */
    SLP_CH_9             /*!< @if Eng slp channel 9, center frequency 7987.2MHz
                              @else slp信道9，中心频率7987.2MHz @endif */
} SlpChannel;

/**
 * @if Eng
 * @brief Enum of slp usage mode
 * @else
 * @brief slp使用模式
 * @endif
 */
typedef enum {
    SLP_USAGE_RANGING_AOX = 0,        /*!< @if Eng slp ranging and aox usage mode
                                           @else slp测距测角使用模式 @endif */
    SLP_USAGE_AIR_MOUSE,              /*!< @if Eng slp air mouse usage mode
                                           @else slp空中鼠标使用模式 @endif */
} SlpUsageMode;

/**
 * @if Eng
 * @brief Enum of slp mode
 * @else
 * @brief slp模式
 * @endif
 */
typedef enum {
    SLP_RANGING_ONLY = 0,           /*!< @if Eng slp ranging
                                         @else slp测距 @endif */
    SLP_RANGING_AOA,                /*!< @if Eng slp ranging and angle of arrival
                                         @else slp测距和aoa测角 @endif */
} SlpRangingMode;

/**
 * @if Eng
 * @brief Enum of slp multiple node mode
 * @else
 * @brief slp多节点模式
 * @endif
 */
typedef enum {
    SLP_ONE_TO_ONE = 0,         /*!< @if Eng slp one-to-one
                                     @else slp 1对1 @endif */
    SLP_ONE_TO_MANY,            /*!< @if Eng slp one-to-many
                                     @else slp 1对多 @endif */
    SLP_MANY_TO_MANY            /*!< @if Eng slp many-to-many
                                     @else slp 多对多 @endif */
} SlpMultiNodeMode;

/**
 * @if Eng
 * @brief Enum of slp ranging round usage
 * @else
 * @brief slp测距轮用法
 * @endif
 */
typedef enum {
    SLP_OWR = 0,            /*!< @if Eng slp one way ranging
                                 @else slp 单向测距 @endif */
    SLP_SS_TWR,             /*!< @if Eng slp single-sided two way ranging
                                 @else slp 单边双向测距 @endif */
    SLP_DS_TWR              /*!< @if Eng slp double-sided two way ranging
                                 @else slp 双边双向测距 @endif */
} SlpRangingRoundUsage;

/**
 * @if Eng
 * @brief Enum of slp measurement request source
 * @else
 * @brief slp测量值请求源，测量值接收方表示计算距离的一方
 * @endif
 */
typedef enum {
    SLP_MR_RECV = 0,        /*!< @if Eng slp measurement receiver
                                 @else slp测量值接收方，表示计算距离的一方 @endif */
    SLP_MR_SEND             /*!< @if Eng slp measurement sender,
                                 @else slp测量值发送方 @endif */
} SlpMrSource;

/**
 * @if Eng
 * @brief Enum of slp aox direction
 * @else
 * @brief slp测角方向
 * @endif
 */
typedef enum {
    SLP_POLL_AOX = 0,              /*!< @if Eng slp initiator send aox frame
                                        @else slp测距发起方发测角帧 @endif */
    SLP_RESPONSE_AOX,              /*!< @if Eng slp responder send aox frame
                                        @else slp测距响应方发测角帧 @endif */
    SLP_DOUBLE_SIDE_AOX            /*!< @if Eng slp double-sided angle measurement
                                        @else slp双边测角 @endif */
} SlpAoxDirection;

/**
 * @if Eng
 * @brief Enum of narrowband time synchronization mode
 * @else
 * @brief 窄带时间同步模式
 * @endif
 */
typedef enum {
    SLP_NB_SINGLE_SIDE_SYNC = 1,        /*!< @if Eng narrowband single side sync
                                             @else 窄带单向同步 @endif */
    SLP_NB_DOUBLE_SIDE_SYNC,            /*!< @if Eng narrowband double side sync
                                             @else 窄带双向同步 @endif */
} SlpNbSyncMode;

/**
 * @if Eng
 * @brief Struct of slp ranging parameters
 * @else
 * @brief slp测距参数
 * @endif
 */
typedef struct {
    uint8_t chIdx;                          /*!< @if Eng channel index, @ref SlpChannel
                                                 @else 通道号，参考 { @ref SlpChannel }. @endif */
    uint8_t usageMode;                      /*!< @if Eng slp usage mode, @ref SlpUsageMode
                                                 @else slp使用模式，参考 { @ref SlpUsageMode }. @endif */
    uint8_t slpRangingMode;                 /*!< @if Eng slp ranging mode, @ref SlpRangingMode
                                                 @else slp测距模式，参考 { @ref SlpRangingMode }. @endif */
    uint8_t multiNodeMode;                  /*!< @if Eng slp multiple node mode, @ref SlpMultiNodeMode
                                                 @else slp多节点模式，参考 { @ref SlpMultiNodeMode }. @endif */
    uint8_t rangingRoundUsage;              /*!< @if Eng slp ranging round usage, @ref SlpRangingRoundUsage
                                                 @else slp测距轮用法，参考 { @ref SlpRangingRoundUsage }. @endif */
    uint8_t mrSource;                       /*!< @if Eng slp measurement request source, @ref SlpMrSource
                                                 @else slp测量值请求源，参考 { @ref SlpMrSource }. @endif */
    uint8_t rangingFreq;                    /*!< @if Eng ranging frequency, value:1~20, unit: Hz
                                                 @else 测距频率，取值：1~20，单位：Hz @endif */
    uint8_t validRoundNum;                  /*!< @if Eng valid ranging round number, value: 0~255, 0: infinity round
                                                 @else 有效测距轮数，取值范围：0~255，其中0表示无限轮 @endif */
    uint8_t aoxDirection;                   /*!< @if Eng aox direaction, @ref SlpAoxDirection
                                                 @else 测角方向，参考 { @ref SlpAoxDirection }. @endif */
    uint8_t nbSyncMode;                     /*!< @if Eng narrowband sync mode, @ref SlpNbSyncMode
                                                 @else 窄带同步模式，参考 { @ref SlpNbSyncMode }. @endif */
    uint8_t deviceNum;                      /*!< @if Eng device number, value:2
                                                 @else 参与测距的设备个数，取值：2 @endif */
    uint32_t sessionId;                     /*!< @if Eng session identifier
                                                 @else 会话标识符，随机值，每次启动测距，值不同 @endif */
    SlpDeviceAddr deviceAddr[SLP_MAX_DEVICE_NUM]; /*!< @if Eng device address list, the fist address is initiator
                                                 @else 设备地址列表，第1个是测距发起者的地址 @endif */
} SlpRangingParam;

/**
 * @if Eng
 * @brief Struct of slp ranging parameters
 * @else
 * @brief slp测距控制参数
 * @endif
 */
typedef struct {
    SlpFrameParam frameParam;           /*!< @if Eng slp frame parameters
                                             @else slp帧参数 @endif */
    SlpSecurityParam secParam;          /*!< @if Eng slp security parameters
                                             @else slp安全参数 @endif */
    SlpRangingParam rangingParam;       /*!< @if Eng slp ranging parameters
                                             @else slp测距参数 @endif */
} SlpStartRangingParam;

/**
 * @if Eng
 * @brief Enum of slp screen resolution
 * @else
 * @brief slp屏幕分辨率
 * @endif
 */
typedef enum {
    SLP_SCREEN_720P = 0,              /*!< @if Eng slp screen resolution 720p
                                           @else slp屏幕分辨率720p @endif */
    SLP_SCREEN_1080P,                 /*!< @if Eng slp screen resolution 1080p
                                           @else slp屏幕分辨率1080p @endif */
    SLP_SCREEN_4K,                    /*!< @if Eng slp screen resolution 4K
                                           @else slp屏幕分辨率4K @endif */
    SLP_SCREEN_8K                     /*!< @if Eng slp screen resolution 8K
                                           @else slp屏幕分辨率8k @endif */
} SlpScreenResolution;

/**
 * @if Eng
 * @brief Enum of slp cursor speed
 * @else
 * @brief slp光标移动速度
 * @endif
 */
typedef enum {
    SLP_CURSOR_SPEED_LOW = 0,          /*!< @if Eng slp cursor low speed
                                            @else slp光标低速移动 @endif */
    SLP_CURSOR_SPEED_MEDIUM,           /*!< @if Eng slp cursor medium speed
                                            @else slp光标中速移动 @endif */
    SLP_CURSOR_SPEED_HIGH,             /*!< @if Eng slp cursor high speed
                                            @else slp光标高速移动 @endif */
} SlpCursorSpeed;

/**
 * @if Eng
 * @brief Enum of slp screen parameters
 * @else
 * @brief slp屏幕参数
 * @endif
 */
typedef struct {
    uint8_t cursorSpeed;               /*!< @if Eng slp cursor speed, @ref SlpCursorSpeed
                                            @else slp光标移动速度，参考 { @ref SlpCursorSpeed }. @endif */
    uint8_t resolution;                /*!< @if Eng slp screen resolution 720p, @ref SlpScreenResolution
                                            @else slp屏幕分辨率720p，参考 { @ref SlpScreenResolution }. @endif */
} SlpScreenParam;

/**
 * @if Eng
 * @brief Struct of slp local attribute
 * @else
 * @brief slp本机属性
 * @endif
 */
typedef struct {
    SlpDeviceAddr localAddr;        /*!< @if Eng local address
                                         @else 本机地址 @endif */
    SlpScreenParam screenParam;     /*!< @if Eng screen parameters
                                         @else 屏幕参数 @endif */
} SlpLocalAtt;

/**
 * @if Eng
 * @brief Struct of slp ranging report interface
 * @else
 * @brief slp测距结果上报
 * @endif
 */
typedef struct {
    uint32_t distance;                  /*!< @if Eng distance, unit: mm
                                             @else 距离，单位：毫米 @endif */
    uint8_t aoxAziFom;                  /*!< @if Eng azimuth figure-of-merit
                                             @else 方位角置信度 @endif */
    uint8_t aoxElevFom;                 /*!< @if Eng elevation figure-of-merit
                                             @else 俯仰角置信度 @endif */
    int16_t aoxAzi;                     /*!< @if Eng azimuth, uint: degree
                                             @else 方位角，单位：度 @endif */
    int16_t aoxElev;                    /*!< @if Eng elevation, uint: degree
                                             @else 俯仰角，单位：度 @endif */
    SlpDeviceAddr providerAddr;         /*!< @if Eng provider address
                                             @else 测量值提供方的地址 @endif */
    SlpDeviceAddr requestorAddr;        /*!< @if Eng requestor address
                                             @else 测量值请求方的地址 @endif */
} SlpRangingRpt;

/**
 * @if Eng
 * @brief Struct of slp payload Information
 * @else
 * @brief slp负载信息
 * @endif
 */
typedef struct {
    uint8_t *payload;                  /*!< @if Eng slp payload
                                            @else slp负载 @endif */
    uint16_t payloadLen;               /*!< @if Eng slp payload length
                                            @else slp负载长度，取值范围[0, 255] @endif */
} SlpPayloadInfo;

/**
 * @if Eng
 * @brief Callback invoked when report ranging result
 * @par Callback invoked when report ranging result
 * @attention 1.This function is called in SLP context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the slp automatically.
 * @param  [in]  rangingRpt ranging result.
 * @else
 * @brief  测距结果上报的回调函数。
 * @par    测距结果上报的回调函数。
 * @attention  1. 该回调函数运行于SLP线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLP申请内存，也由SLP释放，回调中不应释放。
 * @param  [in]  rangingRpt 测距结果。
 * @endif
 */
typedef void (*SlpReportRangingCallback)(SlpRangingRpt *rangingRpt);

/**
 * @if Eng
 * @brief Callback invoked when report cursor data
 * @par Callback invoked when report cursor data
 * @attention 1.This function is called in SLP context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the slp automatically.
 * @param  [in]  x x-axis coordinate value. The upper left corner of the screen is used as the coordinate origin,
                   and the horizontal rightward direction is the positive direction of the X axis. unit: pixel
 * @param  [in]  y y-axis coordinate value. The upper left corner of the screen is used as the coordinate origin,
                   and the vertical downward direction is the positive direction of the y axis. unit: pixel
 * @else
 * @brief  光标数据上报的回调函数。
 * @par    光标数据上报的回调函数。
 * @attention  1. 该回调函数运行于SLP线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLP申请内存，也由SLP释放，回调中不应释放。
 * @param  [in]  x x轴坐标值，以屏幕左上角为圆点，水平向右为x轴正方向，单位：像素
 * @param  [in]  y y轴坐标值，以屏幕左上角为圆点，垂直向下为y轴正方向，单位：像素
 * @endif
 */
typedef void (*SlpReportCursorCallback)(uint16_t x, uint16_t y);

/**
 * @if Eng
 * @brief Callback invoked when send payload
 * @par Callback invoked when send payload
 * @attention 1.This function is called in SLP context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the slp automatically.
 * @param  [in]  info payload information, see @ref SlpPayloadInfo.
 * @else
 * @brief  发送负载的回调函数。
 * @par    发送负载的回调函数。
 * @attention  1. 该回调函数运行于SLP线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLP申请内存，也由SLP释放，回调中不应释放。
 * @param  [in]  info 负载信息，参考 SlpPayloadInfo。
 * @endif
 */
typedef void (*SlpSendPayloadCallback)(const SlpPayloadInfo *info);

/**
 * @if Eng
 * @brief  SLP main.
 * @par Description: SLP main.
 * @param  [in]  param parameters that may be passed in
 * @else
 * @brief  SLP线程主函数。
 * @par Description: SLP线程主函数。
 * @param  [in]  param 可能传入的参数
 * @endif
 */
void SlpTask(void* param);

/**
 * @if Eng
 * @brief  SLP power on, complete SLP loading.
 * @par Description: SLP power on, complete SLP loading.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref ErrcodeSlpClient
 * @else
 * @brief  SLP上电，完成SLP加载。
 * @par Description: SLP上电，完成SLP加载。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref ErrcodeSlpClient
 * @endif
 */
ErrcodeSlpClient SlpPowerOnCommand(void);

/**
 * @if Eng
 * @brief  SLP Start Ranging.
 * @par Description: SLP Start Ranging.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref ErrcodeSlpClient
 * @param  [in]  param start ranging parameters, see @ref SlpStartRangingParam
 * @else
 * @brief  SLP启动测距。
 * @par Description: SLP启动测距。
 * @param  [in]  param 启动测距参数，参考 @ref SlpStartRangingParam
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref ErrcodeSlpClient
 * @endif
 */
ErrcodeSlpClient SlpStartRangingCommand(const SlpStartRangingParam *param);

/**
 * @if Eng
 * @brief  SLP power off.
 * @par Description: SLP power off.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref ErrcodeSlpClient
 * @else
 * @brief  SLP下电。
 * @par Description: SLP下电。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref ErrcodeSlpClient
 * @endif
 */
ErrcodeSlpClient SlpPowerOffCommand(void);

/**
 * @if Eng
 * @brief  SLP sleep.
 * @par Description: SLP sleep.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref ErrcodeSlpClient
 * @else
 * @brief  SLP睡眠。
 * @par Description: SLP睡眠。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref ErrcodeSlpClient
 * @endif
 */
ErrcodeSlpClient SlpSleepCommand(void);

/**
 * @if Eng
 * @brief  Set local device attributes.
 * @par Description: Set local device attributes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref ErrcodeSlpClient
 * @param  [in]  param local device attribute parameters, see @ref SlpLocalAtt
 * @else
 * @brief  设置本机特性。
 * @par Description: 设置本机特性。
 * @param  [in]  param 本机特性参数，参考 @ref SlpLocalAtt
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref ErrcodeSlpClient
 * @endif
 */
ErrcodeSlpClient SlpSetLocalAttCommand(const SlpLocalAtt *param);

/**
 * @if Eng
 * @brief  SLP receive payload.
 * @par Description: SLP receive payload.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref ErrcodeSlpClient
 * @param  [in]  info payload information, see @ref SlpPayloadInfo
 * @else
 * @brief  SLP接收负载。
 * @par Description: SLP接收负载。
 * @param  [in]  info 负载信息，参考 @ref SlpPayloadInfo
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref ErrcodeSlpClient
 * @endif
 */
ErrcodeSlpClient SlpRecvPayload(const SlpPayloadInfo *info);

/**
 * @if Eng
 * @brief Register SLP send payload callback
 * @par Description: Register SLP send payload callback
 * @param  [in]  cbk callback function.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref ErrcodeSlpClient
 * @else
 * @brief  注册SLP发送负载回调。
 * @par Description: 注册SLP发送负载回调。
 * @param  [in]  cbk 回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref ErrcodeSlpClient
 * @endif
 */
ErrcodeSlpClient SlpRegisterSendPayloadCallback(SlpSendPayloadCallback cbk);

/**
 * @if Eng
 * @brief Enum of SLP result report callbacks
 * @else
 * @brief SLP结果上报回调
 * @endif
 */
typedef struct {
    SlpReportCursorCallback rptCursorCbk;               /*!< @if Eng slp report cursor callback.
                                                             @else sle上报光标回调函数。 @endif */
    SlpReportRangingCallback rptRangingCbk;             /*!< @if Eng slp report ranging result callback.
                                                             @else sle上报测距结果回调函数。 @endif */
} SlpReportCallbacks;

/**
 * @if Eng
 * @brief  Register SLP report callbacks.
 * @par Description: Register SLP report callbacks.
 * @param  [in]  func Callback function.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref ErrcodeSlpClient
 * @else
 * @brief  注册SLP上报结果回调函数。
 * @par Description: 注册SLP上报结果回调函数。
 * @param  [in]  func 回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref ErrcodeSlpClient
 * @endif
 */
ErrcodeSlpClient SlpRegisterReportCallbacks(SlpReportCallbacks *func);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif