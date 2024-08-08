/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides iot_gpio driver source \n
 *
 * History: \n
 * 2023-10-18， Create file. \n
 */
#include "iot_errno.h"
#include "gpio.h"
#include "iot_gpio.h"

typedef struct {
    GpioIsrCallbackFunc func;
    char *arg;
} iot_gpio_callback_t;

static bool g_iot_gpio_inited = false;

static iot_gpio_callback_t g_iot_gpio_callback[PIN_NONE] = { NULL };

unsigned int IoTGpioInit(unsigned int id)
{
    unused(id);
    if (!g_iot_gpio_inited) {
        uapi_gpio_init();
    }
    g_iot_gpio_inited = true;
    return IOT_SUCCESS;
}

unsigned int IoTGpioDeinit(unsigned int id)
{
    unused(id);
    if (g_iot_gpio_inited) {
        uapi_gpio_deinit();
    }
    g_iot_gpio_inited = false;
    return IOT_SUCCESS;
}

unsigned int IoTGpioSetDir(unsigned int id, IotGpioDir dir)
{
    if (uapi_gpio_set_dir((pin_t)id, (gpio_direction_t)dir) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}

unsigned int IoTGpioGetDir(unsigned int id, IotGpioDir *dir)
{
    if (dir == NULL) {
        return IOT_FAILURE;
    }
    *dir = (IotGpioDir)uapi_gpio_get_dir((pin_t)id);
    return IOT_SUCCESS;
}

unsigned int IoTGpioSetOutputVal(unsigned int id, IotGpioValue val)
{
    if (uapi_gpio_set_val((pin_t)id, (gpio_level_t)val) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}

unsigned int IoTGpioGetOutputVal(unsigned int id, IotGpioValue *val)
{
    if (val == NULL) {
        return IOT_FAILURE;
    }
    *val = (IotGpioValue)uapi_gpio_get_output_val((pin_t)id);
    return IOT_SUCCESS;
}

unsigned int IoTGpioGetInputVal(unsigned int id, IotGpioValue *val)
{
    if (val == NULL) {
        return IOT_FAILURE;
    }
    *val = (IotGpioValue)uapi_gpio_get_val((pin_t)id);
    return IOT_SUCCESS;
}

static void GpioCallbackFunc(pin_t id, uintptr_t arg)
{
    unused(arg);
    if (g_iot_gpio_callback[id].func) {
        g_iot_gpio_callback[id].func(g_iot_gpio_callback[id].arg);
    }
}

unsigned int IoTGpioRegisterIsrFunc(unsigned int id, IotGpioIntType intType, IotGpioIntPolarity intPolarity,
                                    GpioIsrCallbackFunc func, char *arg)
{
    uint32_t trigger = 0;
    if (intType == 0 && intPolarity == 0) {
        trigger = GPIO_INTERRUPT_LOW;
    } else if (intType == 0 && intPolarity == 1) {
        trigger = GPIO_INTERRUPT_HIGH;
    } else if (intType == 1 && intPolarity == 0) {
        trigger = GPIO_INTERRUPT_FALLING_EDGE;
    } else if (intType == 1 && intPolarity == 1) {
        trigger = GPIO_INTERRUPT_RISING_EDGE;
    } else {
        return IOT_FAILURE;
    }
    g_iot_gpio_callback[id].func = func;
    g_iot_gpio_callback[id].arg = arg;
    if (uapi_gpio_register_isr_func((pin_t)id, trigger, (gpio_callback_t)GpioCallbackFunc) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}

unsigned int IoTGpioUnregisterIsrFunc(unsigned int id)
{
    if (uapi_gpio_unregister_isr_func((pin_t)id) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
    g_iot_gpio_callback[id].func = NULL;
    g_iot_gpio_callback[id].arg = NULL;
    return IOT_SUCCESS;
}

unsigned int IoTGpioSetIsrMode(unsigned int id, IotGpioIntType intType, IotGpioIntPolarity intPolarity)
{
    uint32_t trigger = 0;
    if (intType == 0 && intPolarity == 0) {
        trigger = GPIO_INTERRUPT_LOW;
    } else if (intType == 0 && intPolarity == 1) {
        trigger = GPIO_INTERRUPT_HIGH;
    } else if (intType == 1 && intPolarity == 0) {
        trigger = GPIO_INTERRUPT_FALLING_EDGE;
    } else if (intType == 1 && intPolarity == 1) {
        trigger = GPIO_INTERRUPT_RISING_EDGE;
    } else {
        return IOT_FAILURE;
    }
    if (uapi_gpio_set_isr_mode((pin_t)id, trigger) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}

unsigned int IoTGpioSetIsrMask(unsigned int id, unsigned char mask)
{
    if (mask == 0) {
        if (uapi_gpio_enable_interrupt((pin_t)id) != ERRCODE_SUCC) {
            return IOT_FAILURE;
        }
    } else if (mask == 1) {
        if (uapi_gpio_disable_interrupt((pin_t)id) != ERRCODE_SUCC) {
            return IOT_FAILURE;
        }
    } else {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}