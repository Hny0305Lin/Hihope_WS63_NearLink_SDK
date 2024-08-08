/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides ADC driver API \n
 *
 * History: \n
 * 2022-09-16, Create file. \n
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_adc ADC
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  ADC once sample need 16 clocks period, so the sample rate = adc_clk / 16.
 * @else
 * @brief  ADC一次采样需要16个时钟周期，因此采样速率 = adc_clk / 16。
 * @endif
 */
typedef enum adc_clock {
    ADC_CLOCK_500KHZ = 0,               /*!< @if Eng ADC clock: 500KHZ.
                                             @else   ADC时钟频率： 500KHZ。  @endif */
    ADC_CLOCK_250KHZ = 1,               /*!< @if Eng ADC clock: 250KHZ.
                                             @else   ADC时钟频率： 250KHZ。  @endif */
    ADC_CLOCK_125KHZ = 2,               /*!< @if Eng ADC clock: 125KHZ.
                                             @else   ADC时钟频率： 125KHZ。  @endif */
    ADC_CLOCK_015KHZ = 3,               /*!< @if Eng ADC clock: 015KHZ.
                                             @else   ADC时钟频率： 015KHZ。  @endif */
    ADC_CLOCK_MAX,
    ADC_CLOCK_NONE = ADC_CLOCK_MAX
} adc_clock_t;

/**
 * @if Eng
 * @brief  The scan mode of ADC.
 * @else
 * @brief  ADC的扫描模式。
 * @endif
 */
typedef enum afe_scan_mode {
    AFE_GADC_MODE = 0,                  /*!< @if Eng  Analog Front-End ADC General precision mode.
                                             @else    模拟前端ADC常规精度模式。 @endif */
#if defined (CONFIG_ADC_SUPPORT_HAFE)
    AFE_HADC_MODE,                      /*!< @if Eng  Analog Front-End ADC High precision mode.
                                             @else    模拟前端ADC高精度模式。 @endif */
#elif (defined CONFIG_ADC_SUPPORT_AMIC)
    AFE_AMIC_MODE,                      /*!< @if Eng  Analog Front-End ADC microphone mode.
                                             @else    模拟前端ADC麦克风模式。 @endif */
    AFE_BIO_MODE,                      /*!< @if Eng  Analog Front-End ADC biologic mode.
                                             @else    模拟前端ADC生物测量模式。 @endif */
#endif
    AFE_SCAN_MODE_MAX_NUM
} afe_scan_mode_t;

/**
 * @if Eng
 * @brief  ADC scan configuration.
 * @else
 * @brief  ADC扫描配置。
 * @endif
 */
typedef struct adc_scan_config {
    uint8_t type;                       /*!< @if Eng  Scan when the FIFO is full or exceeds the threshold.
                                             @else    FIFO全扫描或阈值扫描。 @endif */

    float threshold_l;                  /*!< @if Eng  Lower threshold of scanning voltage (v).
                                             @else    阈值扫描电压（v）下限。 @endif */

    float threshold_h;                  /*!< @if Eng  Upper threshold of scanning voltage (v).
                                             @else    阈值扫描电压（v）上限。 @endif */

    uint8_t freq;                       /*!< @if Eng  ADC scan frequency for all channels.
                                             @else    ADC扫描频率，用于所有频道。 @endif */
#if defined(CONFIG_ADC_SUPPORT_LONG_SAMPLE)
    uint32_t long_sample_time;           /*!< @if Eng  ADC long sample report cycle [ms].
                                            @else    ADC长采样上报周期（单位：毫秒）。 @endif */
#endif
} adc_scan_config_t;

/**
 * @if Eng
 * @brief  Pointer to the ADC automatic scanning callback function.
 * @param  [in]  channel Automatic scanning channel.
 * @param  [out] buffer Automatic scanning sample result buffer.
 * @param  [in]  length When scanning failed, length is 0; \n
 *                      when FIFO full scan, length is 128; \n
 *                      when threshold scan, length is 1.
 * @param  [out] next Continue automatic scanning or stop automatic scanning.
 * @else
 * @brief  ADC自动扫描回调函数指针。
 * @param  [in]  channel 自动扫描通道。
 * @param  [out] buffer 自动扫描采样结果存放。
 * @param  [in]  length 扫描失败时，长度为0； \n
 *                      fifo全扫描时，长度为128；\n
 *                      阈值扫描时，长度为1。
 * @param  [out] next 继续自动扫描或停止自动扫描。
 * @endif
 */
typedef void (*adc_callback_t)(uint8_t channel, uint32_t *buffer, uint32_t length, bool *next);

/**
 * @if Eng
 * @brief  Initialize the ADC.
 * @param  [in]  clock The clock which is used for ADC sample, ADC source clock is 2MHz.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化ADC。
 * @param  [in]  clock ADC采样的时钟，ADC源时钟为2MHz。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_adc_init(adc_clock_t clock);

/**
 * @if Eng
 * @brief  Deinitialize the ADC.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化ADC。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_adc_deinit(void);

/**
 * @if Eng
 * @brief  Power on/off and enable/disable ADC.
 * @param  [in]  afe_scan_mode  AFE mode to use.
 * @param  [in]  en Power on or off.
 * @else
 * @brief  上下电并启用或关闭ADC。
 * @param  [in]  afe_scan_mode 选择使用的AFE精度模式。
 * @param  [in]  en 上电或下电。
 * @endif
 */
void uapi_adc_power_en(afe_scan_mode_t afe_scan_mode, bool en);

/**
 * @if Eng
 * @brief  Check if the ADC is in using or not.
 * @retval true   Is in using.
 * @retval false  Is not in using.
 * @else
 * @brief  检查ADC是否正在使用。
 * @retval true   使用中。
 * @retval false  未使用。
 * @endif
 */
bool uapi_adc_is_using(void);

/**
 * @if Eng
 * @brief  Open an ADC channel.
 * @param  [in]  channel The ADC channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  开启一个ADC通道。
 * @param  [in]  channel ADC通道。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_adc_open_channel(uint8_t channel);

/**
 * @if Eng
 * @brief  Close an ADC channel.
 * @param  [in]  channel The ADC channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  关闭一个ADC通道。
 * @param  [in]  channel ADC通道。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_adc_close_channel(uint8_t channel);

#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
/**
 * @if Eng
 * @brief  Open differential ADC channels.
 * @param  [in]  postive_ch The postive ADC channel.
 * @param  [in]  negative_ch The negative ADC channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  开启ADC差分通道。
 * @param  [in]  postive_ch ADC正极通道。
 * @param  [in]  negative_ch ADC负极通道。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_adc_open_differential_channel(uint8_t postive_ch, uint8_t negative_ch);

/**
 * @if Eng
 * @brief  Close differential ADC channels.
 * @param  [in]  postive_ch The postive ADC channel.
 * @param  [in]  negative_ch The negative ADC channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  关闭ADC差分通道。
 * @param  [in]  postive_ch ADC正极通道。
 * @param  [in]  negative_ch ADC负极通道。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_adc_close_differential_channel(uint8_t postive_ch, uint8_t negative_ch);
#endif

#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
/**
 * @if Eng
 * @brief  Enable ADC automatic scanning channel, need power on ADC before enable automatic scanning.
 * @param  [in]  channel The ADC channel.
 * @param  [in]  config Automatic scanning configuration, FIFO full scan or threshold scan.
 *               For details, see @ref adc_scan_config_t.
 * @param  [in]  callback Automatic scanning interrupt callback.
 *               For details, see @ref adc_callback_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  启用ADC自动扫描通道，在启用自动扫描之前，需要打开ADC电源。
 * @param  [in]  channel ADC通道。
 * @param  [in]  config 自动扫描配置、fifo全扫描或阈值扫描。参考 @ref adc_scan_config_t 。
 * @param  [in]  callback 自动扫描中断回调。参考 @ref adc_callback_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_adc_auto_scan_ch_enable(uint8_t channel, adc_scan_config_t config, adc_callback_t callback);

/**
 * @if Eng
 * @brief  Disable automatic scanning channel.
 * @param  [in]  channel The ADC channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  手动禁用单通道自动扫描。
 * @param  [in]  channel ADC通道。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_adc_auto_scan_ch_disable(uint8_t channel);

/**
 * @if Eng
 * @brief  Disable automatic scanning, which will disable all scanning channel, and power off the ADC.
 * @else
 * @brief  手动禁用自动扫描，禁用所有扫描通道，并关闭ADC电源。
 * @endif
 */
void uapi_adc_auto_scan_disable(void);

/**
 * @if Eng
 * @brief  Get automatic scanning status.
 * @retval true   enabled.
 * @retval false  disable.
 * @else
 * @brief  获取自动扫描状态。
 * @retval true   使能。
 * @retval false  未使能。
 * @endif
 */
bool uapi_adc_auto_scan_is_enabled(void);
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */

/**
 * @if Eng
 * @brief  Trigger ADC manual sample.
 * @param  [in]  channel The ADC channel.
 * @return ADC sample stick.
 * @else
 * @brief  ADC手动采样。
 * @param  [in]  channel ADC通道。
 * @return ADC采样值。
 * @endif
 */
int32_t uapi_adc_manual_sample(uint8_t channel);

#if defined(CONFIG_ADC_SUPPORT_AFE)
/**
 * @if Eng
 * @brief  Trigger ADC auto sample.
 * @param  [in]  channel The ADC channel.
 * @retval ADC sample stick.
 * @else
 * @brief  ADC自动采样。
 * @param  [in]  channel ADC通道。
 * @retval ADC采样值。
 * @endif
 */
int32_t uapi_adc_auto_sample(uint8_t channel);
#endif /* CONFIG_ADC_SUPPORT_AFE */

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
