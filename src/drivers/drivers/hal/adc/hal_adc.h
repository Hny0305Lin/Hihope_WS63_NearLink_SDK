/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL adc \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */
#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "adc_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_adc_api ADC
 * @ingroup  drivers_hal_adc
 * @{
 */

typedef port_adc_scan_freq_t hal_adc_scan_freq_t;

/**
 * @if Eng
 * @brief  ADC scan type.
 * @else
 * @brief  adc扫描类型。
 * @endif
 */
typedef enum {
    /** @if Eng  Interrupt will be triggered when fifo is full(128 word).
     *  @else    当fifo满（128字）时，将触发中断。
     *  @endif */
    HAL_ADC_SCAN_TYPE_FIFO_FULL,
    /** @if Eng  Interrupt will be triggered when sample result out of volatage range(v) \n
     *           threshold_l ~ threshold_h.
     *  @else    当样本结果超出电压范围（v）时，将触发中断， \n
     *           阈值_l~阈值_h。
     *  @endif */
    HAL_ADC_SCAN_TYPE_THRESHOLD
} hal_adc_scan_type_t;

/**
 * @if Eng
 * @brief  ADC scan config.
 * @else
 * @brief  adc扫描配置。
 * @endif
 */
typedef struct {
    /** @if Eng  Fifo full scan or threshold scan.
     *  @else    Fifo全扫描或阈值扫描。
     *  @endif */
    hal_adc_scan_type_t type;
    /** @if Eng  ADC scan frequency, is public used for all channel.
     *  @else    adc扫描频率，用于所有频道。
     *  @endif */
    hal_adc_scan_freq_t freq;
    /** @if Eng  Threshold scan volatage(v) lower limit.
     *  @else    阈值扫描电压（v）下限。
     *  @endif */
    float threshold_l;
    /** @if Eng  Threshold scan volatage(v) upper limit.
     *  @else    阈值扫描电压（v）上限。
     *  @endif */
    float threshold_h;
#if defined(CONFIG_ADC_SUPPORT_LONG_SAMPLE)
    /** @if Eng  ADC long sample report cycle [ms].
     *  @else    ADC长采样上报周期（单位：毫秒）。
     *  @endif */
    uint32_t long_sample_time;
#endif
} hal_adc_scan_config_t;

/**
 * @if Eng
 * @brief  ADC channel type.
 * @else
 * @brief  adc通道类型。
 * @endif
 */
typedef enum {
    /** @if Eng  differential with buffer channel.
     *  @else    带缓冲通道的差分。
     *  @endif */
    HAL_ADC_CHANNEL_TYPE_DIF_BUF,
    /** @if Eng  single with buffer channel.
     *  @else    带缓冲通道的单通道。
     *  @endif */
    HAL_ADC_CHANNEL_TYPE_SIN_BUF,
    /** @if Eng  single without buffer channel.
     *  @else    单通道无缓冲通道。
     *  @endif */
    HAL_ADC_CHANNEL_TYPE_SIN_NOBUF,
    HAL_ADC_CHANNEL_TYPE_NONE
} hal_adc_channel_type_t;

/**
 * @if Eng
 * @brief  ADC buffer type.
 * @else
 * @brief  adc缓存类型。
 * @endif
 */
typedef enum {
    HAL_ADC_BUFFER_OFF = 0,
    HAL_ADC_BUFFER_ON = 0x1,
    HAL_ADC_BUFFER_DIFF = 0x3,
    HAL_ADC_BUFFER_MAX,
    HAL_ADC_BUFFER_NONE = HAL_ADC_BUFFER_MAX
} hal_adc_buffer_t;

/**
 * @if Eng
 * @brief  ADC auto scan discard number.
 * @else
 * @brief  adc自动扫描丢弃次数。
 * @endif
 */
typedef enum {
    HAL_ADC_SCAN_DIS_8,
    HAL_ADC_SCAN_DIS_16,
    HAL_ADC_SCAN_DIS_32,
    HAL_ADC_SCAN_DIS_64,
    HAL_ADC_SCAN_DIS_128,
    HAL_ADC_SCAN_DIS_256,
    HAL_ADC_SCAN_DIS_512,
    HAL_ADC_SCAN_DIS_1024,
    HAL_ADC_SCAN_DIS_MAX,
    HAL_ADC_SCAN_DIS_NONE = HAL_ADC_SCAN_DIS_MAX
} hal_adc_scan_dis_t;

/**
 * @if Eng
 * @brief  ADC auto scan average number.
 * @else
 * @brief  adc自动扫描平均次数。
 * @endif
 */
typedef enum {
    HAL_ADC_SCAN_AVG_2,
    HAL_ADC_SCAN_AVG_4,
    HAL_ADC_SCAN_AVG_8,
    HAL_ADC_SCAN_AVG_16,
    HAL_ADC_SCAN_AVG_MAX,
    HAL_ADC_SCAN_AVG_NONE = HAL_ADC_SCAN_AVG_MAX
} hal_adc_scan_avg_t;

/**
 * @if Eng
 * @brief  Auto scan callback for hal adc.
 * @param  [in]  channel Auto scan channel.
 * @param  [out] buffer Auto scan sample result buffer.
 * @param  [in]  length When scan failed, length is 0; \n
                        when fifo full scan, length is 128; \n
                        when threshold scan, length is 1.
 * @param  [out] next Continue auto scan or stop auto scan.
 * @else
 * @param  [in]  channel 自动扫描通道。
 * @param  [out] buffer 自动扫描采样结果存放。
 * @param  [in]  length 扫描失败时，长度为0;fifo全扫描时，长度为128；阈值扫描时，长度为1。
 * @param  [out] next 继续自动扫描或停止自动扫描。
 * @endif
 */
typedef void (*hal_adc_callback_t)(uint8_t channel, uint32_t *buffer, uint32_t length, bool *next);

/**
 * @if Eng
 * @brief  Initialize device for hal adc.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层adc的初始化接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_adc_init_t)(void);

/**
 * @if Eng
 * @brief  Deinitialize device for hal adc.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层adc的去初始化接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_adc_deinit_t)(void);

 /**
 * @if Eng
 * @brief  Power on/off interface for hal adc.
 * @param  [in]  on flag of power on or off.
 * @else
 * @brief  HAL层adc上下电接口。
 * @param  [in]  on 上下电标志。
 * @endif
 */
typedef void (*hal_adc_power_en_t)(afe_scan_mode_t afe_scan_mode, bool on);

/**
 * @if Eng
 * @brief Channel select interface for hal adc.
 * @param  [in]  ch The adc channel. For details, see @ref adc_channel_t.
 * @param  [in]  on flag of enable or disable.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层通道选择接口。
 * @param  [in]  ch adc通道 参考 @ref adc_channel_t 。
 * @param  [in]  on 使能/去使能标志。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_adc_set_t)(adc_channel_t ch, bool on);

#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
/**
 * @if Eng
 * @brief Channel select differential interface for hal adc.
 * @param  [in]  postive_ch The postive adc channel. For details, see @ref adc_channel_t.
 * @param  [in]  negative_ch The negative adc channel. For details, see @ref adc_channel_t.
 * @param  [in]  on flag of enable or disable.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层通道选择差分接口。
 * @param  [in]  postive_ch adc正通道 参考 @ref adc_channel_t 。
 * @param  [in]  negative_ch adc负通道 参考 @ref adc_channel_t 。
 * @param  [in]  on 使能/去使能标志。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_adc_diff_set_t)(adc_channel_t postive_ch, adc_channel_t negative_ch, bool on);
#endif

#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
/**
 * @if Eng
 * @brief Configure channel auto scan interface for hal adc.
 * @param  [in]  ch The adc channel. For details, see @ref adc_channel_t.
 * @param  [in]  adc_config Auto scan config. For details, see @ref adc_scan_config_t.
 * @param  [in]  callback Auto scan interrupt callback. For details, see @ref hal_adc_callback_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层单个通道的自动扫描配置接口。
 * @param  [in]  ch adc通道 参考 @ref adc_channel_t 。
 * @param  [in]  adc_config 自动扫描配置信息 参考 @ref adc_scan_config_t 。
 * @param  [in]  callback 自动扫描中断回调函数 参考 @ref hal_adc_callback_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_adc_ch_config_t)(adc_channel_t ch, hal_adc_scan_config_t *adc_config,
                                         hal_adc_callback_t callback);

/**
 * @if Eng
 * @brief Channel auto scan enable/disable interface for hal adc.
 * @param  [in]  ch The adc channel. For details, see @ref adc_channel_t.
 * @param  [in]  en The adc channel auto scan enable flag.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层单个通道的自动扫描使能/去使能接口。
 * @param  [in]  ch adc通道 参考 @ref adc_channel_t 。
 * @param  [in]  en adc通道自动扫描使能标志。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_adc_ch_enable_t)(adc_channel_t ch, bool en);

/**
 * @if Eng
 * @brief Auto scan control enable/disable interface for hal adc.
 * @param  [in]  en adc auto scan control enable flag.
 * @else
 * @brief  HAL层自动扫描总控制使能/去使能接口。
 * @param  [in]  en adc自动扫描总控制使能标志。
 * @endif
 */
typedef void (*hal_adc_enable_t)(bool en);

/**
 * @if Eng
 * @brief Auto scan enable judgment interface for hal adc.
 * @retval true   enabled.
 * @retval false  disable.
 * @else
 * @brief  HAL层自动扫描总控制判断接口。
 * @retval true   使能。
 * @retval false  未使能。
 * @endif
 */
typedef bool (*hal_adc_isenable_t)(void);
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */

#if defined(CONFIG_ADC_SUPPORT_AFE)
/**
 * @if Eng
 * @brief ADC auto sample interface for hal adc.
 * @param  [in]  channel The adc channel. For details, see @ref adc_channel_t.
 * @retval ADC sample stick.
 * @else
 * @brief  HAL层自动采样接口接口。
 * @param  [in]  channel adc通道 参考 @ref adc_channel_t 。
 * @retval ADC自动采样值。
 * @endif
 */
typedef int32_t (*hal_adc_auto_sample_t)(adc_channel_t channel);
#endif /* CONFIG_ADC_SUPPORT_AFE */

/**
 * @if Eng
 * @brief ADC manual sample interface for hal adc.
 * @retval ADC sample stick.
 * @else
 * @brief  HAL层手动采样接口接口。
 * @retval 采样值。
 * @endif
 */
typedef int32_t (*hal_adc_manual_t)(adc_channel_t channel);

/**
 * @if Eng
 * @brief  Interface between ADC driver and ADC hal.
 * @else
 * @brief  Driver层adc和HAL层adc的接口。
 * @endif
 */
typedef struct {
    hal_adc_init_t   init;                  /*!< @if Eng Init device interface.
                                               @else   HAL层adc的初始化接口。 @endif */
    hal_adc_deinit_t deinit;                /*!< @if Eng Deinit device interface.
                                               @else   HAL层adc去初始化接口。 @endif */
    hal_adc_power_en_t   power_en;          /*!< @if Eng Power on/off device interface.
                                               @else   HAL层adc上下电接口 。@endif */
    hal_adc_set_t   ch_set;                 /*!< @if Eng Channel select device interface.
                                               @else   HAL层adc通道选择接口。 @endif */
#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
    hal_adc_diff_set_t diff_ch_set;         /*!< @if Eng Channel select diff device interface.
                                               @else   HAL层adc差分通道选择接口。 @endif */
#endif
#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    hal_adc_ch_config_t   ch_config;        /*!< @if Eng Channel auto scan enable device interface.
                                               @else   HAL层adc单个通道的自动扫描使能接口。 @endif */
    hal_adc_ch_enable_t   ch_enable;        /*!< @if Eng Channel auto scan disable device interface.
                                               @else   HAL层adc单个通道的自动扫描去使能接口。 @endif */
    hal_adc_enable_t   enable;              /*!< @if Eng Auto scan control enable/disable device interface.
                                               @else   HAL层adc自动扫描总控制开关使能/去使能接口。 @endif */
    hal_adc_isenable_t   isenable;          /*!< @if Eng Auto scan control enabled device interface.
                                               @else   HAL层adc判断是否已经使能自动扫描总控制接口。 @endif */
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */
#if defined(CONFIG_ADC_SUPPORT_AFE)
    hal_adc_auto_sample_t   auto_sample;    /*!< @if Eng Auto sample based on pre-configured parameters.
                                               @else   HAL层adc依据前序配置的参数自动采样。 @endif */
#endif /* CONFIG_ADC_SUPPORT_AFE */
    hal_adc_manual_t   manual;              /*!< @if Eng Manual sample device interface.
                                               @else   HAL层adc手动采样获取采样值接口。 @endif */
} hal_adc_funcs_t;

/**
 * @if Eng
 * @brief  Register @ref hal_adc_funcs_t into the g_hal_adcs_funcs.
 * @param  [out] funcs Interface between adc driver and adc hal.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  注册 @ref hal_adc_funcs_t 到 g_hal_adcs_funcs 。
 * @param  [out] funcs Driver层adc和HAL层adc的接口实例。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_adc_register_funcs(hal_adc_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_adc_funcs_t from the g_hal_adcs_funcs.
 * @return ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从g_hal_adcs_funcs注销 @ref hal_adc_funcs_t 。
 * @return ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_adc_unregister_funcs(void);

/**
 * @if Eng
 * @brief  Get interface between adc driver and adc hal, see @ref hal_adc_funcs_t.
 * @return Interface between adc driver and adc hal, see @ref hal_adc_funcs_t.
 * @else
 * @brief  获取Driver层adc和HAL层adc的接口实例，参考 @ref hal_adc_funcs_t 。
 * @return Driver层adc和HAL层adc的接口实例，参考 @ref hal_adc_funcs_t 。
 * @endif
 */
hal_adc_funcs_t *hal_adc_get_funcs(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif