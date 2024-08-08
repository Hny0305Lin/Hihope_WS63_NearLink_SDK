/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides v151 hal tsensor \n
 *
 * History: \n
 * 2023-02-28， Create file. \n
 */
#include "common_def.h"
#include "hal_tsensor.h"
#include "hal_tsensor_v151.h"

static hal_tsensor_callback_t g_hal_tsensor_callbacks[HAL_TSENSOR_INTERRIPT_TYPE_MAX_NUM] = { NULL };
static hal_tsensor_calibration_point_t g_hal_tsensor_points[HAL_TSENSOR_CALIBRATION_NUM_MAX] = {0};
static int g_hal_tsensor_calibration_type = 0;

static inline int8_t hal_tsensor_reg_tempcode_transto_temp(uint16_t temp_code)
{
    /* temperature(℃) = (temp_code - 114) / (896 - 114) * [125 - (-40)] + (-40) */
    return (int8_t)((temp_code - TEMP_CODE_MIN) * (TEMP_VAL_MAX - TEMP_VAL_MIN) / (TEMP_CODE_MAX - TEMP_CODE_MIN) +
           TEMP_VAL_MIN);
}

static inline uint16_t hal_tsensor_reg_temp_transto_tempcode(int8_t temp)
{
    /* temperature(℃) = (temp_code - 114) / (896 - 114) * [125 - (-40)] + (-40) */
    return (uint16_t)((((temp) - TEMP_VAL_MIN) * (TEMP_CODE_MAX - TEMP_CODE_MIN) / (TEMP_VAL_MAX - TEMP_VAL_MIN)) +
           TEMP_CODE_MIN);
}

static void tsensor_v151_calibration_juction_to_environment(int16_t *temperature)
{
    int16_t ref_d_value = 0;
    int16_t temp_d_value = 0;

    switch (g_hal_tsensor_calibration_type) {
        case HAL_TSENSOR_CALIBRATION_NUM_SINGLE:
            *temperature += g_hal_tsensor_points[0].environment_temp - g_hal_tsensor_points[0].tsensor_temp;
            break;
        case HAL_TSENSOR_CALIBRATION_NUM_TWO:
            ref_d_value = g_hal_tsensor_points[1].environment_temp - g_hal_tsensor_points[0].environment_temp;
            temp_d_value = g_hal_tsensor_points[1].tsensor_temp - g_hal_tsensor_points[0].tsensor_temp;

            if (temp_d_value == 0 || ref_d_value == 0) {
                *temperature += g_hal_tsensor_points[0].environment_temp - g_hal_tsensor_points[0].tsensor_temp;
            } else {
                *temperature = (*temperature - g_hal_tsensor_points[0].tsensor_temp) * ref_d_value /
                               temp_d_value + g_hal_tsensor_points[0].environment_temp;
            }
            break;
        default:
            break;
    }
}

static void tsensor_v151_calibration_environment_to_juction(int16_t *temperature)
{
    int16_t ref_d_value = 0;
    int16_t temp_d_value = 0;

    switch (g_hal_tsensor_calibration_type) {
        case HAL_TSENSOR_CALIBRATION_NUM_SINGLE:
            *temperature += g_hal_tsensor_points[0].tsensor_temp - g_hal_tsensor_points[0].environment_temp;
            break;
        case HAL_TSENSOR_CALIBRATION_NUM_TWO:
            ref_d_value = g_hal_tsensor_points[1].tsensor_temp - g_hal_tsensor_points[0].tsensor_temp;
            temp_d_value = g_hal_tsensor_points[1].environment_temp - g_hal_tsensor_points[0].environment_temp;

            if (temp_d_value == 0 || ref_d_value == 0) {
                *temperature += g_hal_tsensor_points[0].tsensor_temp - g_hal_tsensor_points[0].environment_temp;
            } else {
                *temperature = (*temperature - g_hal_tsensor_points[0].environment_temp) * ref_d_value /
                               temp_d_value + g_hal_tsensor_points[0].tsensor_temp;
            }
            break;
        default:
            break;
    }
}

static void hal_tsensor_v151_enable_calibration(hal_tsensor_calibration_point_t *point_data, int8_t point_num)
{
    int8_t i = 0;
    hal_tsensor_v151_reg_set_calib_enable();
    for (; i < point_num; i++) {
        g_hal_tsensor_points[i] = point_data[i];
    }
 
    g_hal_tsensor_calibration_type = point_num;
}

static errcode_t hal_tsensor_v151_init(void)
{
    if (hal_tsensor_regs_init() != ERRCODE_SUCC) {
        return ERRCODE_TSENSOR_REG_ADDR_INVALID;
    }
    hal_tsensor_v151_reg_set_auto_refresh_enable(false);
    hal_tsensor_v151_reg_set_calib_enable();

    return ERRCODE_SUCC;
}

static void hal_tsensor_v151_deinit(void)
{
    hal_tsensor_v151_reg_clear_sts();
    hal_tsensor_v151_reg_clr_temp_intr();
    hal_tsensor_v151_reg_set_auto_refresh_enable(false);

    hal_tsensor_v151_reg_set_tsensor_done_intr_enable(0);
    hal_tsensor_v151_reg_set_out_temp_threshold_intr_enable(0);
    hal_tsensor_v151_reg_set_overtemp_intr_enable(0);
    hal_tsensor_v151_reg_set_tsensor_disable();
    hal_tsensor_regs_deinit();
}

static void hal_tsensor_v151_set_samp_mode(hal_tsensor_samp_mode_t mode, uint32_t period)
{
    hal_tsensor_v151_reg_set_auto_refresh_enable(false);
    hal_tsensor_v151_reg_set_tsensor_mode(mode);
    hal_tsensor_v151_reg_set_auto_refresh_period(period);
    hal_tsensor_v151_reg_set_tsensor_enable();
}

static void hal_tsensor_v151_temp_threshold_set_low(hal_tsensor_set_temp_id_t id, int8_t temp)
{
    unused(id);
    uint16_t temp_code;

    temp_code = hal_tsensor_reg_temp_transto_tempcode(temp);
    hal_tsensor_v151_reg_set_temp_low_limit(temp_code);
}

static void hal_tsensor_v151_temp_threshold_set_high(hal_tsensor_set_temp_id_t id, int8_t temp)
{
    unused(id);
    uint16_t temp_code;

    temp_code = hal_tsensor_reg_temp_transto_tempcode(temp);
    hal_tsensor_v151_reg_set_temp_high_limit(temp_code);
}

static void hal_tsensor_v151_temp_threshold_set_over(hal_tsensor_set_temp_id_t id, int8_t temp)
{
    unused(id);
    uint16_t temp_code;

    temp_code = hal_tsensor_reg_temp_transto_tempcode(temp);
    hal_tsensor_v151_reg_set_overtemp_threshold(temp_code);
}

static hal_tsensor_set_temp_threshold_t g_hal_tsensor_set_func_array[TSENSOR_SET_TEMP_MAX] = {
    hal_tsensor_v151_temp_threshold_set_low,
    hal_tsensor_v151_temp_threshold_set_high,
    hal_tsensor_v151_temp_threshold_set_over,
};

static void hal_tsensor_v151_set_temp_threshold(hal_tsensor_set_temp_id_t id, int8_t temp)
{
    int16_t temp_d = temp;
    tsensor_v151_calibration_environment_to_juction((int16_t *)&temp_d);
    if (temp_d < HAL_TSENSOR_TEMP_THRESHOLD_L_MAX) {
        temp_d = HAL_TSENSOR_TEMP_THRESHOLD_L_MAX;
    }
    if (temp_d > HAL_TSENSOR_TEMP_THRESHOLD_H_MAX) {
        temp_d = HAL_TSENSOR_TEMP_THRESHOLD_H_MAX;
    }

    g_hal_tsensor_set_func_array[id](id, temp_d);
}

static void hal_tsensor_v151_set_interrupt(hal_tsensor_interript_type_t interrupt_type, bool value)
{
    hal_tsensor_v151_reg_set_auto_refresh_enable(true);
    if (interrupt_type == HAL_TSENSOR_INTERRIPT_TYPE_DONE) {
        hal_tsensor_v151_reg_set_tsensor_done_intr_enable(value);
    } else if (interrupt_type == HAL_TSENSOR_INTERRIPT_TYPE_OVERTEMP) {
        hal_tsensor_v151_reg_set_out_temp_threshold_intr_enable(value);
    } else if (interrupt_type == HAL_TSENSOR_INTERRIPT_TYPE_OUT_THRESH) {
        hal_tsensor_v151_reg_set_overtemp_intr_enable(value);
    }
}

static void hal_tsensor_v151_set_callback(hal_tsensor_interript_type_t interrupt_type,
                                          hal_tsensor_callback_t tsensor_callback)
{
    g_hal_tsensor_callbacks[interrupt_type] = tsensor_callback;
}

static void hal_tsensor_v151_refresh_temp(void)
{
    hal_tsensor_v151_reg_set_start();
}

static bool hal_tsensor_v151_get_temp(volatile int8_t *data)
{
    int16_t temp;
    uint16_t temp_code;

    /* wait for temperature valid flag */
    uint64_t start_time = tsensor_port_get_ms();
    while (hal_tsensor_v151_reg_is_tsensor_ready() == 0) {
        if ((tsensor_port_get_ms() - start_time) > CONFIG_TSENSOR_WAIT_TIME_MS) {
            return false;
        }
    }

    /* get temp_code and tranfer temp_code to tepmerature */
    temp_code = hal_tsensor_v151_reg_get_data();
    temp = hal_tsensor_reg_tempcode_transto_temp(temp_code);

    tsensor_v151_calibration_juction_to_environment((int16_t *)&temp);

    if (temp < HAL_TSENSOR_TEMP_THRESHOLD_L_MAX) {
        *data = HAL_TSENSOR_TEMP_THRESHOLD_L_MAX;
        return false;
    }
    if (temp > HAL_TSENSOR_TEMP_THRESHOLD_H_MAX) {
        *data = HAL_TSENSOR_TEMP_THRESHOLD_H_MAX;
        return false;
    }

    *data = temp;
    return true;
}

static hal_tsensor_funcs_t g_hal_tsensor_v151_funcs = {
    .init = hal_tsensor_v151_init,
    .deinit = hal_tsensor_v151_deinit,
    .set_samp_mode = hal_tsensor_v151_set_samp_mode,
    .set_temp_threshold = hal_tsensor_v151_set_temp_threshold,
    .set_interrupt = hal_tsensor_v151_set_interrupt,
    .set_callback = hal_tsensor_v151_set_callback,
#if defined(CONFIG_TSENSOR_MULTILEVEL)
    .set_multilevel_value = NULL,
    .set_multilevel_en = NULL,
#endif /* CONFIG_TSENSOR_MULTILEVEL */
    .enable_calibration = hal_tsensor_v151_enable_calibration,
    .refresh_temp = hal_tsensor_v151_refresh_temp,
    .get_temp = hal_tsensor_v151_get_temp,
};

hal_tsensor_funcs_t *hal_tsensor_v151_funcs_get(void)
{
    return &g_hal_tsensor_v151_funcs;
}

static void hal_tsensor_notify_int_callback(hal_tsensor_interript_type_t interrupt_type, int8_t temp)
{
    if (g_hal_tsensor_callbacks[interrupt_type]) {
        g_hal_tsensor_callbacks[interrupt_type](temp);
    }
}

void hal_tsensor_irq_handler(void)
{
    int8_t temp;
    uint32_t interrupt_sts = hal_tsensor_v151_reg_get_temp_intr_status();
    if ((uint16_t)interrupt_sts & hal_tsensor_v151_reg_get_temp_intr_enable_status()) {
        hal_tsensor_v151_get_temp(&temp);
    }

    if (interrupt_sts & (0x1 << HAL_TSENSOR_INTERRIPT_TYPE_DONE)) {
        hal_tsensor_notify_int_callback(HAL_TSENSOR_INTERRIPT_TYPE_DONE, temp);
    }
    if (interrupt_sts & (0x1 << HAL_TSENSOR_INTERRIPT_TYPE_OUT_THRESH)) {
        hal_tsensor_notify_int_callback(HAL_TSENSOR_INTERRIPT_TYPE_OUT_THRESH, temp);
    }
    if (interrupt_sts & (0x1 << HAL_TSENSOR_INTERRIPT_TYPE_OVERTEMP)) {
        hal_tsensor_notify_int_callback(HAL_TSENSOR_INTERRIPT_TYPE_OVERTEMP, temp);
    }

    hal_tsensor_v151_reg_set_auto_refresh_enable(true);
    hal_tsensor_v151_reg_clear_sts();   /* 清除16点平均单次上报模式或16点平均循环上报模式的状态 */
    hal_tsensor_v151_reg_clr_temp_intr();
}
