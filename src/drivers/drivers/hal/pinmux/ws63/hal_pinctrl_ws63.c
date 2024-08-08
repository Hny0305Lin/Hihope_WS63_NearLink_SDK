/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides hal pinctrl \n
 *
 * History: \n
 * 2022-08-29, Create file. \n
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "errcode.h"
#include "pinctrl_porting.h"
#include "hal_pinctrl_ws63.h"

#define HAL_IO_CFG_BASE_ADDR            0x4400D000
#define HAL_PIN_GPIO_SEL_START_OFFSET   0x000
#define HAL_PIN_UART_SEL_START_OFFSET   0x03C
#define HAL_PIN_GPIO_CTRL_START_OFFSET  0x800
#define HAL_PIN_SFC_CTRL_START_OFFSET   0x868

#define HAL_PIN_GPIO_SEL_START_ADDR     (HAL_IO_CFG_BASE_ADDR + HAL_PIN_GPIO_SEL_START_OFFSET)
#define HAL_PIN_UART_SEL_START_ADDR     (HAL_IO_CFG_BASE_ADDR + HAL_PIN_UART_SEL_START_OFFSET)
#define HAL_PIN_GPIO_CTRL_START_ADDR    (HAL_IO_CFG_BASE_ADDR + HAL_PIN_GPIO_CTRL_START_OFFSET)
#define HAL_PIN_SFC_CTRL_START_ADDR     (HAL_IO_CFG_BASE_ADDR + HAL_PIN_SFC_CTRL_START_OFFSET)

// group num of each pin func
#define HAL_PIN_MODE_GROUP_NUM 2
#define HAL_PIN_DS_GROUP_NUM 2
#define HAL_PIN_PULL_GROUP_NUM 2
#define HAL_PIN_IE_GROUP_NUM 2
#define HAL_PIN_ST_GROUP_NUM 2

#define HAL_PIN_CONFIG_PER_NUM 1

#define HAL_PIN_GPIO_SEL_START_BIT  0
#define HAL_PIN_GPIO_SEL_BITS_NUM   3

#define HAL_PIN_UART_SEL_START_BIT  0
#define HAL_PIN_UART_SEL_BITS_NUM   2

#define HAL_PIN_ST_START_BIT    3
#define HAL_PIN_ST_BITS_NUM     1

#define HAL_PIN_DS_START_BIT    4
#define HAL_PIN_DS_BITS_NUM     3

#define HAL_PIN_PULL_START_BIT  9
#define HAL_PIN_PULL_BITS_NUM   2

#define HAL_PIN_IE_START_BIT    11
#define HAL_PIN_IE_BITS_NUM     1

/**
 * @brief  PIN config type enum.
 */
typedef enum {
    PIN_CONFIG_TYPE_MODE = 0,
    PIN_CONFIG_TYPE_DS,
    PIN_CONFIG_TYPE_PULL,
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    PIN_CONFIG_TYPE_IE,
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
#if defined(CONFIG_PINCTRL_SUPPORT_ST)
    PIN_CONFIG_TYPE_ST,
#endif /* CONFIG_PINCTRL_SUPPORT_ST */
    PIN_CONFIG_TYPE_NUM
} hal_pin_config_type_t;

/**
 * @brief  PIN config addr group struct.
 */
typedef struct {
    pin_t begin;             //!< The first pin of each group.
    pin_t end;               //!< The last pin of each group.
    uint32_t reg_addr;       //!< Address of the group.
    uint32_t per_num;        //!< Number of pins that can be configured by one register.
    uint32_t first_bit;      //!< The first bit to config.
    uint32_t bits_num;       //!< Num of bits to config.
} hal_pin_config_group_t;

/**
 * @brief  PIN config addr map struct.
 */
typedef struct {
    uint32_t config_type;
    uint32_t group_num;
    hal_pin_config_group_t *group;
} hal_pin_config_map_t;

static hal_pin_config_group_t const g_pin_mode_map[] = {
    {
        GPIO_00,
        GPIO_14,
        HAL_PIN_GPIO_SEL_START_ADDR,
        HAL_PIN_CONFIG_PER_NUM,
        HAL_PIN_GPIO_SEL_START_BIT,
        HAL_PIN_GPIO_SEL_BITS_NUM
    },
    {
        GPIO_15,
        GPIO_18,
        HAL_PIN_UART_SEL_START_ADDR,
        HAL_PIN_CONFIG_PER_NUM,
        HAL_PIN_UART_SEL_START_BIT,
        HAL_PIN_UART_SEL_BITS_NUM
    }
};

static hal_pin_config_group_t const g_pin_pull_map[] = {
    {
        GPIO_00,
        GPIO_14,
        HAL_PIN_GPIO_CTRL_START_ADDR,
        HAL_PIN_CONFIG_PER_NUM,
        HAL_PIN_PULL_START_BIT,
        HAL_PIN_PULL_BITS_NUM
    },
    {
        SFC_CLK,
        SFC_IO3,
        HAL_PIN_SFC_CTRL_START_ADDR,
        HAL_PIN_CONFIG_PER_NUM,
        HAL_PIN_PULL_START_BIT,
        HAL_PIN_PULL_BITS_NUM
    }
};

static hal_pin_config_group_t const g_pin_ds_map[] = {
    {
        GPIO_00,
        GPIO_14,
        HAL_PIN_GPIO_CTRL_START_ADDR,
        HAL_PIN_CONFIG_PER_NUM,
        HAL_PIN_DS_START_BIT,
        HAL_PIN_DS_BITS_NUM
    },
    {
        SFC_CLK,
        SFC_IO3,
        HAL_PIN_SFC_CTRL_START_ADDR,
        HAL_PIN_CONFIG_PER_NUM,
        HAL_PIN_DS_START_BIT,
        HAL_PIN_DS_BITS_NUM
    }
};

#if defined(CONFIG_PINCTRL_SUPPORT_IE)
static hal_pin_config_group_t const g_pin_ie_map[] = {
    {
        GPIO_00,
        GPIO_18,
        HAL_PIN_GPIO_CTRL_START_ADDR,
        HAL_PIN_CONFIG_PER_NUM,
        HAL_PIN_IE_START_BIT,
        HAL_PIN_IE_BITS_NUM
    },
    {
        SFC_CLK,
        SFC_IO3,
        HAL_PIN_SFC_CTRL_START_ADDR,
        HAL_PIN_CONFIG_PER_NUM,
        HAL_PIN_IE_START_BIT,
        HAL_PIN_IE_BITS_NUM
    }
};
#endif /* CONFIG_PINCTRL_SUPPORT_IE */

#if defined(CONFIG_PINCTRL_SUPPORT_ST)
static hal_pin_config_group_t const g_pin_st_map[] = {
    {
        GPIO_00,
        GPIO_18,
        HAL_PIN_GPIO_CTRL_START_ADDR,
        HAL_PIN_CONFIG_PER_NUM,
        HAL_PIN_ST_START_BIT,
        HAL_PIN_ST_BITS_NUM
    },
    {
        SFC_CLK,
        SFC_IO3,
        HAL_PIN_SFC_CTRL_START_ADDR,
        HAL_PIN_CONFIG_PER_NUM,
        HAL_PIN_ST_START_BIT,
        HAL_PIN_ST_BITS_NUM
    }
};
#endif /* CONFIG_PINCTRL_SUPPORT_ST */

static hal_pin_config_map_t const g_pin_config_map_pinctrl[] = {
    {PIN_CONFIG_TYPE_MODE, HAL_PIN_MODE_GROUP_NUM, (hal_pin_config_group_t *)g_pin_mode_map},
    {PIN_CONFIG_TYPE_DS, HAL_PIN_DS_GROUP_NUM, (hal_pin_config_group_t *)g_pin_ds_map},
    {PIN_CONFIG_TYPE_PULL, HAL_PIN_PULL_GROUP_NUM, (hal_pin_config_group_t *)g_pin_pull_map},
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    {PIN_CONFIG_TYPE_IE, HAL_PIN_IE_GROUP_NUM, (hal_pin_config_group_t *)g_pin_ie_map},
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
#if defined(CONFIG_PINCTRL_SUPPORT_ST)
    {PIN_CONFIG_TYPE_ST, HAL_PIN_ST_GROUP_NUM, (hal_pin_config_group_t *)g_pin_st_map},
#endif /* CONFIG_PINCTRL_SUPPORT_ST */
};

STATIC errcode_t hal_pin_get_config_map(uint32_t config_type, hal_pin_config_group_t **pin_map, uint32_t *map_size)
{
    if (config_type >= PIN_CONFIG_TYPE_NUM) {
        return ERRCODE_FAIL;
    }
    *map_size = g_pin_config_map_pinctrl[config_type].group_num;
    *pin_map = g_pin_config_map_pinctrl[config_type].group;
    return ERRCODE_SUCC;
}

STATIC hal_pin_config_group_t *hal_pin_get_config_group(uint32_t config_type, pin_t pin)
{
    hal_pin_config_group_t *pin_group = NULL;
    uint32_t map_size = 0;
    errcode_t ret;

    ret = hal_pin_get_config_map(config_type, &pin_group, &map_size);
    if (ret != ERRCODE_SUCC) {
        return NULL;
    }
    for (uint32_t i = 0; i < map_size; i++) {
        if ((pin >= pin_group[i].begin) && (pin <= pin_group[i].end)) {
            return (pin_group + i);
        }
    }
    return NULL;
}

STATIC errcode_t hal_pin_set_config_val(uint32_t config_type, pin_t pin, uint32_t val)
{
    uint32_t reg_addr;
    hal_pin_config_group_t *pin_group = hal_pin_get_config_group(config_type, pin);
    if ((pin_group == NULL) || (pin_group->per_num == 0)) {
        return ERRCODE_FAIL;
    }
    reg_addr = (uintptr_t)pin_group->reg_addr + ((uint32_t)(pin - pin_group->begin) / pin_group->per_num) *
               (sizeof(uint32_t));
    if (val > (uint32_t)((1 << pin_group->bits_num) - 1)) {
        return ERRCODE_INVALID_PARAM;
    }
    reg32_setbits(reg_addr, pin_group->first_bit, pin_group->bits_num, val);
    return ERRCODE_SUCC;
}

STATIC errcode_t hal_pin_get_config_val(uint32_t config_type, pin_t pin, uint32_t *val)
{
    uint32_t reg_addr;
    hal_pin_config_group_t *pin_group = hal_pin_get_config_group(config_type, pin);
    if ((pin_group == NULL) || (pin_group->per_num == 0)) {
        return ERRCODE_FAIL;
    }
    reg_addr = (uintptr_t)pin_group->reg_addr + ((uint32_t)(pin - pin_group->begin) / pin_group->per_num) *
               (sizeof(uint32_t));
    *val = reg32_getbits(reg_addr, pin_group->first_bit, pin_group->bits_num);
    return ERRCODE_SUCC;
}

STATIC errcode_t hal_pin_ws63_set_mode(pin_t pin, pin_mode_t mode)
{
    return hal_pin_set_config_val(PIN_CONFIG_TYPE_MODE, pin, mode);
}

STATIC pin_mode_t hal_pin_ws63_get_mode(pin_t pin)
{
    uint32_t val;
    if (hal_pin_get_config_val(PIN_CONFIG_TYPE_MODE, pin, &val) != ERRCODE_SUCC) {
        return PIN_MODE_MAX;
    }
    return val;
}

STATIC errcode_t hal_pin_ws63_set_ds(pin_t pin, pin_drive_strength_t ds)
{
    return hal_pin_set_config_val(PIN_CONFIG_TYPE_DS, pin, ds);
}

STATIC pin_drive_strength_t hal_pin_ws63_get_ds(pin_t pin)
{
    uint32_t val;
    if (hal_pin_get_config_val(PIN_CONFIG_TYPE_DS, pin, &val) != ERRCODE_SUCC) {
        return PIN_DS_MAX;
    }
    return val;
}

STATIC errcode_t hal_pin_ws63_set_pull(pin_t pin, pin_pull_t pull_type)
{
    return hal_pin_set_config_val(PIN_CONFIG_TYPE_PULL, pin, pull_type);
}

STATIC pin_pull_t hal_pin_ws63_get_pull(pin_t pin)
{
    uint32_t val;
    if (hal_pin_get_config_val(PIN_CONFIG_TYPE_PULL, pin, &val) != ERRCODE_SUCC) {
        return PIN_PULL_MAX;
    }
    return val;
}

#if defined(CONFIG_PINCTRL_SUPPORT_IE)
STATIC errcode_t hal_pin_ws63_set_ie(pin_t pin, pin_input_enable_t ie)
{
    return hal_pin_set_config_val(PIN_CONFIG_TYPE_IE, pin, ie);
}

STATIC pin_input_enable_t hal_pin_ws63_get_ie(pin_t pin)
{
    uint32_t val;
    if (hal_pin_get_config_val(PIN_CONFIG_TYPE_IE, pin, &val) != ERRCODE_SUCC) {
        return PIN_IE_MAX;
    }
    return val;
}
#endif /* CONFIG_PINCTRL_SUPPORT_IE */

#if defined(CONFIG_PINCTRL_SUPPORT_ST)
STATIC errcode_t hal_pin_ws63_set_st(pin_t pin, pin_schmitt_trigger_t st)
{
    return hal_pin_set_config_val(PIN_CONFIG_TYPE_ST, pin, st);
}

STATIC pin_schmitt_trigger_t hal_pin_ws63_get_st(pin_t pin)
{
    uint32_t val;
    if (hal_pin_get_config_val(PIN_CONFIG_TYPE_ST, pin, &val) != ERRCODE_SUCC) {
        return PIN_ST_MAX;
    }
    return val;
}
#endif /* CONFIG_PINCTRL_SUPPORT_ST */

static hal_pin_funcs_t g_hal_pin_ws63_funcs = {
    .set_mode = hal_pin_ws63_set_mode,
    .get_mode = hal_pin_ws63_get_mode,
    .set_ds = hal_pin_ws63_set_ds,
    .get_ds = hal_pin_ws63_get_ds,
    .set_pull = hal_pin_ws63_set_pull,
    .get_pull = hal_pin_ws63_get_pull,
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    .set_ie = hal_pin_ws63_set_ie,
    .get_ie = hal_pin_ws63_get_ie,
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
#if defined(CONFIG_PINCTRL_SUPPORT_ST)
    .set_st = hal_pin_ws63_set_st,
    .get_st = hal_pin_ws63_get_st,
#endif /* CONFIG_PINCTRL_SUPPORT_ST */
};

hal_pin_funcs_t *hal_pin_ws63_funcs_get(void)
{
    return &g_hal_pin_ws63_funcs;
}
