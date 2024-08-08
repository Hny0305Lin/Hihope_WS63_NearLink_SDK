/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides adc driver source \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include "common_def.h"
#include "hal_adc.h"
#include "adc_porting.h"
#include "adc.h"

#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
bool adc_is_power_on[AFE_SCAN_MODE_MAX_NUM] = {false};        /* ADC is power on or not. */
#else
bool adc_is_power_on = false;        /* ADC is power on or not. */
#endif /* CONFIG_ADC_SUPPORT_AFE */

bool adc_is_initialised = false;     /* ADC is initialised or not. */

#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
uint8_t adc_working_channel[2] = {0}; /* ADC differential channel be choosed. */
#else
uint8_t adc_working_channel = 0; /* ADC channel be choosed. */
#endif /* CONFIG_ADC_SUPPORT_DIFFERENTIAL */

#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
afe_scan_mode_t adc_working_afe = AFE_SCAN_MODE_MAX_NUM;
#endif /* CONFIG_ADC_SUPPORT_AFE */


static hal_adc_funcs_t *g_hal_funcs = NULL;

errcode_t uapi_adc_init(adc_clock_t clock)
{
    if (adc_is_initialised) {
        return ERRCODE_SUCC;
    }

    adc_port_init_clock(clock);
    adc_port_clock_enable(true);
    adc_port_register_hal_funcs();
    g_hal_funcs = hal_adc_get_funcs();
    errcode_t ret = g_hal_funcs->init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    adc_port_register_irq();
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */
    adc_is_initialised = true;

    return ret;
}

errcode_t uapi_adc_deinit(void)
{
    if (!adc_is_initialised) {
        return ERRCODE_SUCC;
    }

    errcode_t ret = g_hal_funcs->deinit();

    adc_port_unregister_hal_funcs();

    adc_port_clock_enable(false);
#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    adc_port_unregister_irq();
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */
    adc_is_initialised = false;

    return ret;
}

void uapi_adc_power_en(afe_scan_mode_t afe_scan_mode, bool en)
{
#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
    if (afe_scan_mode >= AFE_SCAN_MODE_MAX_NUM || adc_is_power_on[afe_scan_mode] == en) {
        return;
    }
#else
    if (adc_is_power_on == en) {
        return;
    }
#endif /* CONFIG_ADC_SUPPORT_AFE */

    adc_port_power_on(en);
#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
#if (AFE_ENABLE_INTERRUPT)
    if (en) {
        adc_working_afe = afe_scan_mode;
        adc_port_register_irq(afe_scan_mode);
    } else {
        adc_port_unregister_irq(afe_scan_mode);
    }
#endif /* AFE_ENABLE_INTERRUPT */
#endif /* CONFIG_ADC_SUPPORT_AFE */

    g_hal_funcs->power_en(afe_scan_mode, en);

#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
    adc_is_power_on[afe_scan_mode] = en;
#else
    adc_is_power_on = en;
#endif /* CONFIG_ADC_SUPPORT_AFE */
}

bool uapi_adc_is_using(void)
{
#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
    return adc_is_power_on[AFE_GADC_MODE] || adc_is_power_on[AFE_HADC_MODE];
#else
    return adc_is_power_on;
#endif /* CONFIG_ADC_SUPPORT_AFE */
}

errcode_t uapi_adc_open_channel(uint8_t channel)
{
    if (unlikely(channel >= ADC_CHANNEL_MAX_NUM)) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }

#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    if (uapi_adc_auto_scan_is_enabled()) {
        return ERRCODE_ADC_SCAN_NOT_DISABLE;
    }
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */
    uint32_t irq_sts = adc_irq_lock();
#if defined(CONFIG_ADC_SUPPORT_AFE)
    errcode_t ret = g_hal_funcs->ch_set(channel, true);
#else
    errcode_t ret = g_hal_funcs->ch_set((adc_channel_t)channel, true);
#endif /* CONFIG_ADC_SUPPORT_AFE */
    adc_irq_unlock(irq_sts);
    if (ret == ERRCODE_SUCC) {
#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
        adc_working_channel[0] = channel;
#else
        adc_working_channel = channel;
#endif
    }

    return ret;
}

errcode_t uapi_adc_close_channel(uint8_t channel)
{
#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
    if (unlikely(channel != adc_working_channel[0])) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
#else
    if (unlikely(channel != adc_working_channel)) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
#endif

#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    if (uapi_adc_auto_scan_is_enabled()) {
        return ERRCODE_ADC_SCAN_NOT_DISABLE;
    }
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */

    uint32_t irq_sts = adc_irq_lock();
    errcode_t ret = g_hal_funcs->ch_set((adc_channel_t)channel, false);
    adc_irq_unlock(irq_sts);
    if (ret == ERRCODE_SUCC) {
#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
        adc_working_channel[0] = ADC_CHANNEL_NONE;
#else
        adc_working_channel = ADC_CHANNEL_NONE;
#endif
    }
    return ret;
}

#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
errcode_t uapi_adc_open_differential_channel(uint8_t postive_ch, uint8_t negative_ch)
{
    if (unlikely(postive_ch >= ADC_CHANNEL_MAX_NUM) || unlikely(negative_ch >= ADC_CHANNEL_MAX_NUM)) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }

    uint32_t irq_sts = adc_irq_lock();
    errcode_t ret = g_hal_funcs->diff_ch_set((adc_channel_t)postive_ch, (adc_channel_t)negative_ch, true);
    adc_irq_unlock(irq_sts);
    if (ret == ERRCODE_SUCC) {
        adc_working_channel[0] = postive_ch;
        adc_working_channel[1] = negative_ch;
    }

    return ret;
}

errcode_t uapi_adc_close_differential_channel(uint8_t postive_ch, uint8_t negative_ch)
{
    if (unlikely(postive_ch != adc_working_channel[0]) ||
        unlikely(negative_ch != adc_working_channel[1])) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }

#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    if (uapi_adc_auto_scan_is_enabled()) {
        return ERRCODE_ADC_SCAN_NOT_DISABLE;
    }
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */

    uint32_t irq_sts = adc_irq_lock();
    errcode_t ret = g_hal_funcs->diff_ch_set((adc_channel_t)postive_ch, (adc_channel_t)negative_ch, false);
    adc_irq_unlock(irq_sts);
    if (ret == ERRCODE_SUCC) {
        adc_working_channel[0] = ADC_CHANNEL_NONE;
        adc_working_channel[1] = ADC_CHANNEL_NONE;
    }
    return ret;
}
#endif

#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
static bool adc_auto_scan_ch_param_check(uint8_t channel, adc_scan_config_t *config, adc_callback_t callback)
{
    if (unlikely(channel >= ADC_CHANNEL_MAX_NUM)) {
        return false;
    }

    if (unlikely(callback == NULL)) {
        return false;
    }

    if (unlikely(config->type > HAL_ADC_SCAN_TYPE_THRESHOLD)) {
        return false;
    }

    if (unlikely(config->freq >= HAL_ADC_SCAN_FREQ_MAX)) {
        return false;
    }

    return true;
}

errcode_t uapi_adc_auto_scan_ch_enable(uint8_t channel, adc_scan_config_t config, adc_callback_t callback)
{
    hal_adc_scan_config_t adc_config;

    if (!adc_auto_scan_ch_param_check(channel, &config, callback)) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
    if (unlikely(!adc_is_power_on)) {
        return ERRCODE_PWM_NOT_POWER_ON;
    }

    adc_config.type = (hal_adc_scan_type_t)config.type;
    adc_config.freq = (hal_adc_scan_freq_t)config.freq;
#if defined(CONFIG_ADC_SUPPORT_LONG_SAMPLE)
    adc_config.long_sample_time = config.long_sample_time;
#endif
    adc_config.threshold_l = config.threshold_l;
    adc_config.threshold_h = config.threshold_h;

    adc_port_init_clock(ADC_CLOCK_500KHZ);

    errcode_t ret = g_hal_funcs->ch_config((adc_channel_t)channel, &adc_config, (hal_adc_callback_t)callback);
    if (ret != ERRCODE_SUCC) {
        adc_port_init_clock(ADC_CLOCK_015KHZ);
    }

    return ret;
}

errcode_t uapi_adc_auto_scan_ch_disable(uint8_t channel)
{
    if (unlikely(channel >= ADC_CHANNEL_MAX_NUM)) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }

    return g_hal_funcs->ch_enable((adc_channel_t)channel, false);
}

void uapi_adc_auto_scan_disable(void)
{
    g_hal_funcs->enable(false);
    uapi_adc_open_channel(ADC_CHANNEL_NONE);
#if defined(CONFIG_ADC_SUPPORT_AFE) && defined(CONFIG_ADC_SUPPORT_HAFE)
    uapi_adc_power_en(adc_working_afe, false);
#else
    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, false);
#endif /* CONFIG_ADC_SUPPORT_AFE */
}

bool uapi_adc_auto_scan_is_enabled(void)
{
    return g_hal_funcs->isenable();
}
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */

int32_t uapi_adc_manual_sample(uint8_t channel)
{
    if (channel >= ADC_CHANNEL_MAX_NUM) { return 0; }

    return g_hal_funcs->manual((adc_channel_t)channel);
}

#if defined(CONFIG_ADC_SUPPORT_AFE)
int32_t uapi_adc_auto_sample(uint8_t channel)
{
    if (channel >= ADC_CHANNEL_MAX_NUM) { return 0; }

    return g_hal_funcs->auto_sample((adc_channel_t)channel);
}
#endif /* CONFIG_ADC_SUPPORT_AFE */
