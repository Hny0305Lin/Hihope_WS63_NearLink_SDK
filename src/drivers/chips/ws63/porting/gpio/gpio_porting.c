/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides gpio port template \n
 *
 * History: \n
 * 2022-07-26， Create file. \n
 */
#include "hal_gpio_v150.h"
#include "chip_io.h"
#include "chip_core_irq.h"
#include "platform_types.h"
#include "platform_core.h"
#include "gpio_porting.h"

/**
 * @brief  GPIO group info table.
 *         Developer should adapt GPIO info here.
 * @note   This is const information and does not change during code running.
 */
static hal_gpio_group_info_t const g_gpio_channel_0_group_info[GPIO_CHANNEL_0_GROUP_NUM] = {
    // GPIO_00 ~ GPIO_07
    {
        .start_pin_id = GPIO_00,
        .pin_num = GPIO_CHANNEL_0_GROUP_0_PIN_NUM,
        .start_callback_id = GPIO_CHANNEL_0_GROUP_0_CB_START_ID,
    },
};
static hal_gpio_group_info_t const g_gpio_channel_1_group_info[GPIO_CHANNEL_1_GROUP_NUM] = {
    // GPIO_08 ~ GPIO_15
    {
        .start_pin_id = GPIO_08,
        .pin_num = GPIO_CHANNEL_1_GROUP_0_PIN_NUM,
        .start_callback_id = GPIO_CHANNEL_1_GROUP_0_CB_START_ID,
    },
};
static hal_gpio_group_info_t const g_gpio_channel_2_group_info[GPIO_CHANNEL_2_GROUP_NUM] = {
    // GPIO_16 ~ GPIO_18
    {
        .start_pin_id = GPIO_16,
        .pin_num = GPIO_CHANNEL_2_GROUP_0_PIN_NUM,
        .start_callback_id = GPIO_CHANNEL_2_GROUP_0_CB_START_ID,
    },
};

/**
 * @brief  GPIO channel info table.
 *         Developer should adapt GPIO info here.
 * @note   This is const information and does not change during code running.
 */
static hal_gpio_channel_info_t const g_gpio_channel_info_list[GPIO_CHANNEL_MAX_NUM] = {
    // channel0, GPIO_00 ~ GPIO_07
    {
        .start_pin_id = GPIO_00,
        .pin_num = GPIO_CHANNEL_0_PIN_NUM,
        .group_num = GPIO_CHANNEL_0_GROUP_NUM,
        .irq_num = GPIO_0_IRQN,
        .base_addr = (uintptr_t)GPIO_CHANNEL_0_BASE_ADDR,
        .group_list = (hal_gpio_group_info_t *)g_gpio_channel_0_group_info,
    },

    // channel1, GPIO_08 ~ GPIO_15
    {
        .start_pin_id = GPIO_08,
        .pin_num = GPIO_CHANNEL_1_PIN_NUM,
        .group_num = GPIO_CHANNEL_1_GROUP_NUM,
        .irq_num = GPIO_1_IRQN,
        .base_addr = (uintptr_t)GPIO_CHANNEL_1_BASE_ADDR,
        .group_list = (hal_gpio_group_info_t *)g_gpio_channel_1_group_info,
    },

    // channel2, GPIO_16 ~ GPIO_18
    {
        .start_pin_id = GPIO_16,
        .pin_num = GPIO_CHANNEL_2_PIN_NUM,
        .group_num = GPIO_CHANNEL_2_GROUP_NUM,
        .irq_num = GPIO_2_IRQN,
        .base_addr = (uintptr_t)GPIO_CHANNEL_2_BASE_ADDR,
        .group_list = (hal_gpio_group_info_t *)g_gpio_channel_2_group_info,
    }
};

/**
 * @brief  GPIO context info table.
 *         Developer should adapt channel number, and group number of each channel here.
 */
static hal_gpio_group_context_t g_gpio_channel_0_group_context[GPIO_CHANNEL_0_GROUP_NUM] = {0};
static hal_gpio_group_context_t g_gpio_channel_1_group_context[GPIO_CHANNEL_1_GROUP_NUM] = {0};
static hal_gpio_group_context_t g_gpio_channel_2_group_context[GPIO_CHANNEL_2_GROUP_NUM] = {0};
static hal_gpio_group_context_t const *g_gpio_channel_context_list[GPIO_CHANNEL_MAX_NUM] = {
    g_gpio_channel_0_group_context,
    g_gpio_channel_1_group_context,
    g_gpio_channel_2_group_context,
};

hal_gpio_channel_info_t *gpio_porting_channel_info_get(uint32_t channel)
{
    return (hal_gpio_channel_info_t *)&g_gpio_channel_info_list[channel];
}

hal_gpio_group_context_t *gpio_porting_group_context_get(uint32_t channel, uint32_t group)
{
    hal_gpio_group_context_t *channel_info = (hal_gpio_group_context_t *)g_gpio_channel_context_list[channel];
    return &channel_info[group];
}

void gpio_porting_channel_context_clean(uint32_t channel, uint32_t group_num)
{
    memset_s((void *)(g_gpio_channel_context_list[channel]), sizeof(hal_gpio_group_context_t) * group_num, 0,
        sizeof(gpio_callback_t) * group_num);
}

uintptr_t gpio_porting_base_addr_get(uint32_t channel)
{
    return gpio_porting_channel_info_get(channel)->base_addr;
}

void gpio_ulp_int_en(bool on)
{
    unused(on);
}

void gpio_select_core(pin_t pin, cores_t core)
{
    unused(pin);
    unused(core);
}