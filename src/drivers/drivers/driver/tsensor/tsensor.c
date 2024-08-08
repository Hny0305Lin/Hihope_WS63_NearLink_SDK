/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides tsensor driver source \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */
#include "common_def.h"
#include "hal_tsensor.h"
#include "tsensor.h"
/**
 * @if Eng
 * @brief  Definition of the Tsensor work mode.
 * @else
 * @brief  Tsensor工作模式定义。
 * @endif
 */
typedef enum tsensor_work_mode {
    TSENSOR_WORK_MODE_INQUIRE,                /*!< @if Eng Tsensor mode: inquire mode.
                                                   @else   Tsensor模式：查询模式。  @endif */
    TSENSOR_WORK_MODE_INTERRUPT,              /*!< @if Eng Tsensor mode: interrupt mode.
                                                   @else   Tsensor模式：中断模式。  @endif */
    TSENSOR_WORK_MODE_MAX_NUM,
    TSENSOR_WORK_MODE_NONE = TSENSOR_WORK_MODE_MAX_NUM
} tsensor_work_mode_t;

static bool g_tsensor_inited = false;
static hal_tsensor_funcs_t *g_hal_funcs = NULL;
static tsensor_work_mode_t g_tsensor_work_mode = TSENSOR_WORK_MODE_NONE;

#define TSENSOR_TEMP_THRESHOLD_L_MAX (HAL_TSENSOR_TEMP_THRESHOLD_L_MAX)
#define TSENSOR_TEMP_THRESHOLD_H_MAX (HAL_TSENSOR_TEMP_THRESHOLD_H_MAX)

static errcode_t uapi_tsensor_temp_check(int16_t temp)
{
    if ((temp < TSENSOR_TEMP_THRESHOLD_L_MAX) ||
        (temp > TSENSOR_TEMP_THRESHOLD_H_MAX)) {
        return ERRCODE_TSENSOR_INVALID_PARAMETER;
    }

    return ERRCODE_SUCC;
}


static errcode_t uapi_tsensor_check(int8_t temp_threshold_low,
                                    int8_t temp_threshold_high)
{
    errcode_t ret = ERRCODE_SUCC;
    ret = uapi_tsensor_temp_check(temp_threshold_low);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = uapi_tsensor_temp_check(temp_threshold_high);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (temp_threshold_low > temp_threshold_high) {
        return ERRCODE_TSENSOR_INVALID_PARAMETER;
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_tsensor_init(void)
{
    if (unlikely(g_tsensor_inited == true)) {
        return ERRCODE_FAIL;
    }

    tsensor_port_register_hal_funcs();
    g_hal_funcs = hal_tsensor_get_funcs();
    errcode_t ret = g_hal_funcs->init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#if defined(CONFIG_TSENSOR_USING_V150)
    tsensor_port_power_on_and_fre_div(true);
#endif /* CONFIG_TSENSOR_USING_V150 */
    tsensor_port_register_irq();
    g_tsensor_inited = true;

    return ret;
}

errcode_t uapi_tsensor_deinit(void)
{
    if (unlikely(g_tsensor_inited == false)) {
        return ERRCODE_TSENSOR_NOT_INIT;
    }

#if defined(CONFIG_TSENSOR_USING_V150)
    tsensor_port_power_on_and_fre_div(false);
#endif /* CONFIG_TSENSOR_USING_V150 */
    tsensor_port_unregister_irq();
    g_hal_funcs->deinit();
    tsensor_port_unregister_hal_funcs();
    g_tsensor_inited = false;

    return ERRCODE_SUCC;
}

errcode_t uapi_tsensor_start_inquire_mode(tsensor_samp_mode_t mode, uint32_t period)
{
    if (unlikely(g_tsensor_inited == false)) {
        return ERRCODE_TSENSOR_NOT_INIT;
    }

    if (mode >= TSENSOR_SAMP_MODE_NONE) {
        return ERRCODE_TSENSOR_INVALID_PARAMETER;
    }

    uint32_t flag = tsensor_port_irq_lock();
    g_hal_funcs->set_samp_mode((hal_tsensor_samp_mode_t)mode, period);
    g_tsensor_work_mode = TSENSOR_WORK_MODE_INQUIRE;
    tsensor_port_irq_unlock(flag);

    return ERRCODE_SUCC;
}

errcode_t uapi_tsensor_get_current_temp(int8_t *temp)
{
    if (unlikely(g_tsensor_inited == false)) {
        return ERRCODE_TSENSOR_NOT_INIT;
    }

    if (temp == NULL) {
        return ERRCODE_TSENSOR_INVALID_PARAMETER;
    }

    g_hal_funcs->refresh_temp();

    if (!g_hal_funcs->get_temp((volatile int8_t *)temp)) {
        return ERRCODE_TSENSOR_GET_TEMP_INVALID;
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_tsensor_enable_outtemp_interrupt(hal_tsensor_callback_t callback,
                                                int8_t temp_threshold_low,
                                                int8_t temp_threshold_high)
{
    if (unlikely(g_tsensor_inited == false)) {
        return ERRCODE_TSENSOR_NOT_INIT;
    }

    errcode_t ret = uapi_tsensor_check(temp_threshold_low, temp_threshold_high);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uint32_t flag = tsensor_port_irq_lock();
    g_hal_funcs->set_temp_threshold(TSENSOR_SET_LOW_TEMP, temp_threshold_low);
    g_hal_funcs->set_temp_threshold(TSENSOR_SET_HIGH_TEMP, temp_threshold_high);
    g_hal_funcs->set_callback(HAL_TSENSOR_INTERRIPT_TYPE_OUT_THRESH, callback);
    g_hal_funcs->set_interrupt(HAL_TSENSOR_INTERRIPT_TYPE_OUT_THRESH, true);
    g_tsensor_work_mode = TSENSOR_WORK_MODE_INTERRUPT;
    tsensor_port_irq_unlock(flag);

    return ERRCODE_SUCC;
}

errcode_t uapi_tsensor_enable_overtemp_interrupt(hal_tsensor_callback_t callback, int8_t overtemp)
{
    if (unlikely(g_tsensor_inited == false)) {
        return ERRCODE_TSENSOR_NOT_INIT;
    }

    errcode_t ret = uapi_tsensor_temp_check(overtemp);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    uint32_t flag = tsensor_port_irq_lock();
    g_hal_funcs->set_temp_threshold(TSENSOR_SET_OVER_TEMP, overtemp);
    g_hal_funcs->set_callback(HAL_TSENSOR_INTERRIPT_TYPE_OVERTEMP, callback);
    g_hal_funcs->set_interrupt(HAL_TSENSOR_INTERRIPT_TYPE_OVERTEMP, true);
    g_tsensor_work_mode = TSENSOR_WORK_MODE_INTERRUPT;
    tsensor_port_irq_unlock(flag);

    return ERRCODE_SUCC;
}

errcode_t uapi_tsensor_enable_done_interrupt(hal_tsensor_callback_t callback)
{
    if (unlikely(g_tsensor_inited == false)) {
        return ERRCODE_TSENSOR_NOT_INIT;
    }

    uint32_t flag = tsensor_port_irq_lock();
    g_hal_funcs->set_callback(HAL_TSENSOR_INTERRIPT_TYPE_DONE, callback);
    g_hal_funcs->set_interrupt(HAL_TSENSOR_INTERRIPT_TYPE_DONE, true);
    g_tsensor_work_mode = TSENSOR_WORK_MODE_INTERRUPT;
    tsensor_port_irq_unlock(flag);

    return ERRCODE_SUCC;
}

#if defined(CONFIG_TSENSOR_TEMP_COMPENSATION)
void uapi_tsensor_set_calibration_single_point(tsensor_calibration_point_t *point)
{
    if (unlikely(g_tsensor_inited == false)) {
        return;
    }

    g_hal_funcs->enable_calibration((hal_tsensor_calibration_point_t *)point, HAL_TSENSOR_CALIBRATION_NUM_SINGLE);
}

void uapi_tsensor_set_calibration_two_points(const tsensor_calibration_point_t *point_first,
                                             const tsensor_calibration_point_t *point_second)
{
    if (unlikely(g_tsensor_inited == false)) {
        return;
    }

    tsensor_calibration_point_t point[HAL_TSENSOR_CALIBRATION_NUM_TWO];
    point[0] = *point_first;
    point[1] = *point_second;

    g_hal_funcs->enable_calibration((hal_tsensor_calibration_point_t *)point, HAL_TSENSOR_CALIBRATION_NUM_TWO);
}
#endif /* CONFIG_TSENSOR_TEMP_COMPENSATION */

#if defined(CONFIG_TSENSOR_MULTILEVEL)
errcode_t uapi_tsensor_set_multilevel_threshold_value(tsensor_multilevel_value_t level, int16_t temp)
{
    if (unlikely(g_tsensor_inited == false)) {
        return ERRCODE_TSENSOR_NOT_INIT;
    }

    if (level >= TSENSOR_MULTILEVEL_VAL_MAX) {
        return ERRCODE_TSENSOR_INVALID_PARAMETER;
    }

    errcode_t ret = uapi_tsensor_temp_check(temp);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return g_hal_funcs->set_multilevel_value(level, temp);
}

errcode_t uapi_tsensor_set_multilevel_threshold_en(tsensor_multilevel_en_t level,
                                                   hal_tsensor_callback_t callback)
{
    if (unlikely(g_tsensor_inited == false)) {
        return ERRCODE_TSENSOR_NOT_INIT;
    }

    if (level >= TSENSOR_MULTILEVEL_EN_MAX) {
        return ERRCODE_TSENSOR_INVALID_PARAMETER;
    }
    return g_hal_funcs->set_multilevel_en(level, callback);
}
#endif /* CONFIG_TSENSOR_MULTILEVEL */