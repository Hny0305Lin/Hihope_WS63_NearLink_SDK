/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V150 HAL GPIO common code. \n
 *
 * History: \n
 * 2023-06-01, Create file. \n
*/
#include <stdint.h>
#include "common_def.h"
#include "soc_osal.h"
#include "hal_gpio.h"
#include "hal_gpio_v150_regs_op.h"
#include "hal_gpio_v150_comm.h"

/* GPIO 回调表, 注册每根GPIO管脚的中断回调, 通过 gpio_group_info_t 结构中的 start_callback_id 字段进行查询访问; */
static gpio_callback_t g_hal_gpio_callback_list[GPIO_PIN_NUM] = {NULL};

gpio_callback_t *hal_gpio_v150_callback_list_get(void)
{
    return g_hal_gpio_callback_list;
}

STATIC int hal_gpio_v150_irq_handler(int irq_num, const void *tmp)
{
    unused(tmp);

    uint32_t channel, group, group_pin, intr_state;
    hal_gpio_channel_info_t *channel_info;
    hal_gpio_group_context_t *group_context;

    // 清除中断
    osal_irq_clear((uint32_t)irq_num);

    // 根据中断号匹配channel
    for (channel = GPIO_CHANNEL_0; channel < GPIO_CHANNEL_MAX_NUM; channel++) {
        channel_info = gpio_porting_channel_info_get(channel);
        if (channel_info->irq_num == (uint32_t)irq_num) {
            break;
        }
    }

    if (channel >= GPIO_CHANNEL_MAX_NUM) {
        // 异常场景, 未找到对应的GPIO_CHANNEL
        return 0;
    }

    for (group = 0; group < channel_info->group_num; group++) {
        group_context = gpio_porting_group_context_get(channel, group);
        if (group_context->cb_registered == 0) {
            // 本组无已注册回调的GPIO, 跳过
            hal_gpio_gpio_int_eoi_clr_all(channel, group);
            continue;
        }

        // 记录该寄存器值并清除该组中断值
        intr_state = hal_gpio_gpio_intr_get_data(channel, group);
        hal_gpio_gpio_int_eoi_clr_all(channel, group);

        // 遍历本组寄存器的中断状态
        for (group_pin = 0; group_pin < channel_info->group_list[group].pin_num; group_pin++) {
            if (intr_state == 0) {
                break;
            }
            if ((intr_state & 0x1) != 0) {
                // 调用回调
                hal_gpio_v150_callback_get(channel, group, group_pin)(
                    (pin_t)hal_gpio_v150_pin_id_get(channel, group, group_pin), 0);
            }
            intr_state >>= 1;
        }
    }

    return 0;
}

void hal_gpio_v150_register_irq(uint32_t int_id)
{
    osal_irq_request(int_id, (osal_irq_handler)hal_gpio_v150_irq_handler, NULL, NULL, NULL);
}

void hal_gpio_v150_unregister_irq(uint32_t int_id)
{
    osal_irq_free(int_id, NULL);
}

errcode_t hal_gpio_v150_pin_info_get(pin_t pin, uint32_t *channel, uint32_t *group, uint32_t *group_pin)
{
    uint32_t channel_id, group_id;
    uint32_t pin_id = (uint32_t)pin;
    hal_gpio_channel_info_t *channel_info = NULL;
    hal_gpio_group_info_t *group_info = NULL;

    for (channel_id = GPIO_CHANNEL_0; channel_id < GPIO_CHANNEL_MAX_NUM; channel_id++) {
        channel_info = gpio_porting_channel_info_get(channel_id);
        if (pin_id >= channel_info->start_pin_id && pin_id < channel_info->start_pin_id + channel_info->pin_num) {
            break;
        }
    }
    if (channel_id >= GPIO_CHANNEL_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }

    for (group_id = 0; group_id < channel_info->group_num; group_id++) {
        group_info = hal_gpio_v150_group_info_get(channel_id, group_id);
        if (pin_id >= group_info->start_pin_id && pin_id <= group_info->start_pin_id + group_info->pin_num) {
            break;
        }
    }
    if (group_id >= channel_info->group_num) {
        return ERRCODE_INVALID_PARAM;
    }

    *channel = channel_id;
    *group = group_id;
    *group_pin = pin_id - group_info->start_pin_id;

    return ERRCODE_SUCC;
}

uint32_t hal_gpio_v150_pin_id_get(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    return hal_gpio_v150_group_info_get(channel, group)->start_pin_id + group_pin;
}

errcode_t hal_gpio_v150_register_cb(uint32_t channel, uint32_t group, uint32_t group_pin, gpio_callback_t cb)
{
    if (hal_gpio_v150_callback_get(channel, group, group_pin) != NULL ||
        (hal_gpio_v150_callback_registered_get(channel, group) & bit(group_pin)) != 0) {
        return ERRCODE_GPIO_ALREADY_SET_CALLBACK;
    }

    hal_gpio_v150_callback_set(channel, group, group_pin, cb);
    hal_gpio_v150_callback_registered_set_true(channel, group, group_pin);
    return ERRCODE_SUCC;
}

void hal_gpio_v150_unregister_cb(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    hal_gpio_v150_callback_registered_set_false(channel, group, group_pin);
    hal_gpio_v150_callback_set(channel, group, group_pin, NULL);
}