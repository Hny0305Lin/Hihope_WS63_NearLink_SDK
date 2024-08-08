/**
* @file radar_service.h
*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023.All rights reserved.
* Description: header file for radar service api. \n
* \n
* History: \n
* 2023-11-27，初始化该文件 \n
*/

#ifndef SERVICE_RADAR_SERVICE_H
#define SERVICE_RADAR_SERVICE_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RADAR_RANGE_BIN_NUM 20
#define RADAR_RAW_DATA_NUM (2 * RADAR_RANGE_BIN_NUM)

/**
 * @defgroup middleware_service_radar_service RADAR Service
 * @ingroup  middleware_service_radar
 * @{
 */

/**
 * @if Eng
 * @brief  radar status set.
 * @else
 * @brief  雷达状态配置。
 * @endif
 */
typedef enum {
    RADAR_STATUS_STOP = 0,  /*!< @if Eng radar status set: stop
                                @else   雷达状态配置停止 @endif */
    RADAR_STATUS_START,     /*!< @if Eng radar status set: start
                                @else   雷达状态配置启动 @endif */
    RADAR_STATUS_RESET,     /*!< @if Eng radar status set: rst
                                @else   雷达状态配置复位 @endif */
    RADAR_STATUS_RESUME,    /*!< @if Eng radar status set: resume
                                @else   雷达状态配置状态恢复 @endif */
} radar_set_sts_t;

/**
 * @if Eng
 * @brief  radar status.
 * @else
 * @brief  雷达状态。
 * @endif
 */
typedef enum {
    RADAR_STATUS_IDLE = 0,  /*!< @if Eng radar status: idle
                                @else   雷达状态未工作 @endif */
    RADAR_STATUS_RUNNING,   /*!< @if Eng radar status: running
                                @else   雷达状态工作 @endif */
} radar_get_sts_t;

/**
 * @if Eng
 * @brief  radar result.
 * @else
 * @brief  雷达结果。
 * @endif
 */
typedef struct {
    uint32_t lower_boundary;    /*!< @if Eng radar result: lower boundary
                                    @else   雷达结果靠近检测下边界 @endif */
    uint32_t upper_boundary;    /*!< @if Eng radar result: upper boundary
                                    @else   雷达结果靠近检测上边界 @endif */
    uint8_t is_human_presence;  /*!< @if Eng radar result: human presence
                                    @else   雷达结果有无人体存在 @endif */
    uint8_t reserved_0;
    uint8_t reserved_1;
    uint8_t reserved_2;
} radar_result_t;

/**
 * @if Eng
 * @brief Callback invoked when the radar detection is complete and the result changes.
 * @par Callback invoked when the radar detection is complete and the result changes.
 * @attention This callback function runs on the radar feature thread.
 * @attention It cannot be blocked or wait for a long time or use a large stack space.
 * @param [in] res radar detect result.
 * @else
 * @brief  雷达检测完成并且结果出现变化时的回调函数。
 * @par    雷达检测完成并且结果出现变化时的回调函数。
 * @attention  该回调函数运行于radar feature线程, 不能阻塞或长时间等待, 不能使用较大栈空间。
 * @param [in] res 雷达检测结果。
 * @endif
 */
typedef void (*radar_result_cb_t)(radar_result_t *result);

/**
 * @if Eng
 * @brief  radar raw data.
 * @attention IQ alternate storage.
 * @else
 * @brief  雷达原始数据。
 * @attention IQ交替存储。
 * @endif
 */
typedef struct {
    int32_t data[RADAR_RAW_DATA_NUM];
} radar_raw_data_t;

/**
 * @if Eng
 * @brief  radar debug type.
 * @else
 * @brief  雷达维测数据类型。
 * @endif
 */
typedef enum {
    RADAR_DBG_NO_DATA_RPT = 0,    /*!< @if Eng no data report
                            @else   无数据上报 @endif */
    RADAR_DBG_PC_DATA_RPT,        /*!< @if Eng report pc data by uart1
                            @else   通过uart1上报脉压后的数据 @endif */
    RADAR_DBG_ADC_DATA_RPT,       /*!< @if Eng report adc data by uart1
                            @else   通过uart1上报adc数据 @endif */
    RADAR_DBG_RES_RPT,            /*!< @if Eng report result by uart1
                            @else   通过uart1上报result @endif */
    RADAR_DBG_UART0_RES_RPT,      /*!< @if Eng report result by uart0
                            @else   通过uart0上报result @endif */
    RADAR_DBG_RPT_BUTT
} radar_dbg_type_t;

/**
 * @if Eng
 * @brief  radar debug parameters.
 * @attention
 * @else
 * @brief  雷达维测参数。
 * @attention
 * @endif
 */
typedef struct {
    uint8_t times;      /*!< @if Eng Number of subframe transmissions, default value 0, always sent, range 0~20
                            @else   子帧发送次数, 默认值0, 一直发送, 范围0~20 @endif */
    uint8_t loop;       /*!< @if Eng Loop times of radar waveform in one subframe, default value 8
                            @else   单个子帧雷达波形循环发送次数, 默认值为8 @endif */
    uint8_t ant;        /*!< @if Eng Selection of radar antenna, default value 0
                            @else   接收通路选择, 默认值为0 @endif */
    uint8_t wave;       /*!< @if Eng Selection of radar wave, default value 2
                            @else   雷达发射波形类型选择, 默认值为2 @endif */
    uint8_t dbg_type;   /*!< @if Eng Selection of debug information, default value 0, for details,
                            see @ref radar_dbg_type_t
                            @else   维测信息输出选择, 默认值为0, 只打印基础流程日志, 范围0~4,
                            参考 @ref radar_dbg_type_t @endif */
    uint16_t period;    /*!< @if Eng Radar subframe interval, unit us, default value is 5000, range 3000~100000
                            @else   雷达子帧间隔，单位us，默认值为5000, 范围3000~100000 @endif */
} radar_dbg_para_t;

/**
 * @if Eng
 * @brief  Elevation information (the height of the module position from the ground).
 * @else
 * @brief  架高信息(模组位置距离地面的高度)。
 * @endif
 */
typedef enum {
    RADAR_HEIGHT_1M, /*!< @if Eng 0-1.5 meters
                            @else   0到1.5米 @endif */
    RADAR_HEIGHT_2M, /*!< @if Eng 1.5-2.5 meters
                            @else   1.5米到2.5米 @endif */
    RADAR_HEIGHT_3M, /*!< @if Eng 2.5 meters or more
                            @else   2.5米以上 @endif */
    RADAR_HEIGHT_BUTT,
} radar_height_type_t;

/**
 * @if Eng
 * @brief  scenario type.
 * @else
 * @brief  场景类型。
 * @endif
 */
typedef enum {
    RADAR_SCENARIO_TYPE_HOME, /*!< @if Eng Home scene
                                @else   家居场景 @endif */
    RADAR_SCENARIO_TYPE_HALL, /*!< @if Eng Open scene (area greater than 50 square meters or
                                floor height greater than 3.5 meters)
                                @else   空旷场景(面积大于50平方米或层高高于3.5米) @endif */
    RADAR_SCENARIO_TYPE_BUTT,
} radar_scenario_type_t;

/**
 * @if Eng
 * @brief  Module line-of-sight direction blocking material type.
 * @else
 * @brief  模组视距方向遮挡材料类型。
 * @endif
 */
typedef enum {
    RADAR_MATERIAL_PLATSIC,   /*!< @if Eng Plastic or no covering
                            @else   塑料或无遮挡物 @endif */
    RADAR_MATERIAL_PCB,       /*!< @if Eng PCB or metal
                            @else   PCB或金属 @endif */
    RADAR_MATERIAL_BUTT,
} radar_material_type_t;

/**
 * @if Eng
 * @brief  algorithm parameter suite selection of radar.
 * @attention
 * @else
 * @brief  雷达算法参数套选择参数。
 * @attention
 * @endif
 */
typedef struct {
    uint8_t height;     /*!< @if Eng Module installation height information: 1/2/3 meters,
                            see @ref radar_height_type_t
                            @else   模组安装架高信息: 1/2/3米, 参考 @ref radar_height_type_t @endif */
    uint8_t scenario;   /*!< @if Eng Scene: Home/empty, see @ref radar_scenario_type_t
                            @else   场景: 家居/空旷, 参考 @ref radar_scenario_type_t @endif */
    uint8_t material;   /*!< @if Eng Module line-of-sight direction blocking material: plastic/metal,
                            see @ref radar_material_type_t
                            @else   模组视距方向遮挡材料: 塑料/金属, 参考 @ref radar_material_type_t @endif */
    uint8_t fusion_track;   /*!< @if Eng Whether to fuse distance tracking results
                            @else   是否融合距离跟踪结果 @endif */
    uint8_t fusion_ai;  /*!< @if Eng Whether to integrate AI results
                            @else   是否融合AI结果 @endif */
} radar_sel_para_t;

/**
 * @if Eng
 * @brief  radar algorithm parameters.
 * @attention
 * @else
 * @brief  雷达算法参数。
 * @attention
 * @endif
 */
typedef struct {
    uint8_t d_th_1m;    /*!< @if Eng Close to the 1 meter threshold, range 0-99
                            @else   靠近1米档门限, 范围0-99 @endif */
    uint8_t d_th_2m;    /*!< @if Eng Close to the 2 meters threshold, range 0-99
                            @else   靠近2米档门限, 范围0-99 @endif */
    uint8_t p_th;       /*!< @if Eng presence in 6 meters threshold, range 0-99
                            @else   存在6米档门限, 范围0-99 @endif */
    uint8_t t_th_1m;    /*!< @if Eng Range tracking 1 meter threshold, unit:dm, range 0-30
                            @else   距离跟踪1米档门限, 单位:分米, 范围0-30 @endif */
    uint8_t t_th_2m;    /*!< @if Eng Range tracking 2 meters threshold unit:dm, range 0-30
                            @else   距离跟踪2米档门限, 单位:分米, 范围0-30 @endif */
    uint8_t b_th_ratio; /*!< @if Eng Anti-spectrum symmetrical interference ratio threshold, range 0-99
                            @else   抗频谱对称干扰比例门限, 范围0-99 @endif */
    uint8_t b_th_cnt;   /*!< @if Eng Anti-spectrum symmetrical interference quantity threshold, range 0-99
                            @else   抗频谱对称干扰数量门限, 范围0-99 @endif */
    uint8_t a_th;       /*!< @if Eng AI human body recognition similarity threshold, range 0-99
                            @else   AI人体识别相似度门限, 范围0-99 @endif */
} radar_alg_para_t;

/**
 * @if Eng
 * @brief Callback function when radar echo reception is complete.
 * @par Callback function when radar echo reception is complete.
 * @attention This callback function runs on the radar driver thread.
 * @attention It cannot be blocked or wait for a long time or use a large stack space.
 * @param [in] res radar detect raw data.
 * @else
 * @brief  雷达回波接收完成时的回调函数。
 * @par    雷达回波接收完成时的回调函数。
 * @attention  该回调函数运行于radar driver线程, 不能阻塞或长时间等待, 不能使用较大栈空间。
 * @param [in] res 雷达检测原始数据。
 * @endif
 */
typedef void (*radar_raw_data_cb_t)(radar_raw_data_t *raw_data);

/**
 * @if Eng
 * @brief  Set status of radar.
 * @par Description: Set status of radar.
 * @param [in] sts status of radar, see @ref radar_set_sts_t.
 * @retval error code.
 * @else
 * @brief  设置雷达状态。
 * @par Description: 设置雷达状态。
 * @param [in] sts 雷达状态。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t uapi_radar_set_status(uint8_t sts);

/**
 * @if Eng
 * @brief  Get status of radar.
 * @par Description: Get status of radar.
 * @param [in] *sts status of radar, see @ref radar_get_sts_t.
 * @retval error code.
 * @else
 * @brief  获取雷达状态。
 * @par Description: 获取雷达状态。
 * @param [in] *sts 雷达状态。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t uapi_radar_get_status(uint8_t *sts);

/**
 * @if Eng
 * @brief  Radar result callback registration function.
 * @par Description: Radar result callback registration function.
 * @param [in] cb callback function.
 * @retval error code.
 * @else
 * @brief  雷达结果回调注册函数。
 * @par Description: 雷达结果回调注册函数。
 * @param [in] cb 回调函数。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t uapi_radar_register_result_cb(radar_result_cb_t cb);

/**
 * @if Eng
 * @brief  Get repot result of radar.
 * @par Description: Get repot result of radar.
 * @param [in] *res repot result of radar, see @ref radar_result_t.
 * @retval error code.
 * @else
 * @brief  获取雷达上报结果。
 * @par Description: 获取雷达上报结果。
 * @param [in] *res 雷达上报结果。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t uapi_radar_get_result(radar_result_t *res);

/**
 * @if Eng
 * @brief  Radar raw data callback registration function.
 * @par Description: Radar raw data callback registration function.
 * @param [in] cb callback function.
 * @retval error code.
 * @else
 * @brief  雷达原始数据回调注册函数。
 * @par Description: 雷达原始数据回调注册函数。
 * @param [in] cb 回调函数。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t uapi_radar_register_raw_data_cb(radar_raw_data_cb_t cb);

/**
 * @if Eng
 * @brief  Set quit delay time of radar.
 * @par Description: Set quit delay time of radar.
 * @param [in] time quit delay time of radar.
 * @retval error code.
 * @else
 * @brief  设置退出延迟时间。
 * @par Description: 设置退出延迟时间。
 * @param [in] time 退出延迟时间。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t uapi_radar_set_delay_time(uint16_t time);

/**
 * @if Eng
 * @brief  Get quit delay time of radar.
 * @par Description: Get quitdelay time of radar.
 * @param [in] *time quit delay time of radar.
 * @retval error code.
 * @else
 * @brief  获取退出延迟时间。
 * @par Description: 获取退出延迟时间。
 * @param [in] *time 退出延迟时间。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t uapi_radar_get_delay_time(uint16_t *time);

/**
 * @if Eng
 * @brief  Get isolation infomation of radar.
 * @par Description: Get isolation infomation of radar.
 * @param [in] *iso isolation infomation of radar.
 * @retval error code.
 * @else
 * @brief  获取天线隔离度信息。
 * @par Description: 获取天线隔离度信息。
 * @param [in] *iso 天线隔离度信息。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t uapi_radar_get_isolation(uint16_t *iso);

/**
 * @if Eng
 * @brief  Set debug parameters of radar.
 * @par Description: Set debug parameters of radar.
 * @param [in] *para debug parameters of radar, see @ref radar_dbg_para_t.
 * @retval error code.
 * @else
 * @brief  设置雷达维测参数。
 * @par Description: 设置雷达维测参数。
 * @param [in] *para 雷达维测参数, 参考 @ref radar_dbg_para_t 。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t uapi_radar_set_debug_para(radar_dbg_para_t *para);

/**
 * @if Eng
 * @brief  Set algorithm parameter suite selection of radar.
 * @par Description: Select algorithm parameter suite of radar.
 * @param [in] *para Parameters of algorithm parameter suite , see @ref radar_sel_para_t.
 * @retval error code.
 * @else
 * @brief  设置雷达算法参数套选择参数。
 * @par Description: 设置雷达算法参数套选择参数。
 * @param [in] *para 雷达算法参数套选择参数, 参考 @ref radar_sel_para_t 。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t uapi_radar_select_alg_para(radar_sel_para_t *para);

/**
 * @if Eng
 * @brief  Set algorithm parameter of radar.
 * @par Description: Set algorithm parameterof radar.
 * @param [in] *para algorithm parameters of radar, see @ref radar_alg_para_t.
 * @param [in] write_to_flash whether write to flash or no.
 * @retval error code.
 * @else
 * @brief  设置雷达算法参数。
 * @par Description: 设置雷达算法参数。
 * @param [in] *para 雷达算法参数, 参考 @ref radar_alg_para_t 。
 * @param [in] write_to_flash 是否写入flash。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t uapi_radar_set_alg_para(radar_alg_para_t *para, uint8_t write_to_flash);

#ifdef __cplusplus
}
#endif
#endif
/**
 * @}
 */
