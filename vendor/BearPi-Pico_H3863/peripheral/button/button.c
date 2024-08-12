/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Button Sample Source. \n
 *
 * History: \n
 * 2023-04-03, Create file. \n
 */

#include "pinctrl.h"
#include "gpio.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"


static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    UNUSED(pin);
    UNUSED(param);
    osal_printk("Button pressed.\r\n");
}

static void button_entry(void)
{
    uapi_pin_set_mode(CONFIG_BUTTON_PIN, HAL_PIO_FUNC_GPIO);
    uapi_pin_set_pull(CONFIG_BUTTON_PIN, PIN_PULL_TYPE_UP);
    uapi_gpio_set_dir(CONFIG_BUTTON_PIN, GPIO_DIRECTION_INPUT);    
    uapi_gpio_register_isr_func(CONFIG_BUTTON_PIN, GPIO_INTERRUPT_DEDGE, gpio_callback_func);
}

/* Run the button_entry. */
app_run(button_entry);