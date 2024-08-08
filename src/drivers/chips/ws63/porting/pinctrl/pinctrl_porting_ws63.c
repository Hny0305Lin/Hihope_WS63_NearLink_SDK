/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides pinctrl port \n
 *
 * History: \n
 * 2022-08-25， Create file. \n
 */
#include "pinctrl_porting.h"
#include "hal_pinctrl.h"
#include "hal_pinctrl_ws63.h"

#ifndef BIT
#define BIT(x) (1UL << (uint32_t)(x))
#endif

typedef enum {
    PIN_FUNC_0 = BIT(PIN_MODE_0),
    PIN_FUNC_1 = BIT(PIN_MODE_1),
    PIN_FUNC_2 = BIT(PIN_MODE_2),
    PIN_FUNC_3 = BIT(PIN_MODE_3),
    PIN_FUNC_4 = BIT(PIN_MODE_4),
    PIN_FUNC_5 = BIT(PIN_MODE_5),
    PIN_FUNC_6 = BIT(PIN_MODE_6),
    PIN_FUNC_7 = BIT(PIN_MODE_7),
    PIN_FUNC_NONE = 0,
} pin_mode_func_t;

typedef enum {
    PIO_MODE_0 = BIT(0),
    PIO_MODE_1 = BIT(1),
    PIO_MODE_2 = BIT(2),
    PIO_MODE_3 = BIT(3),
    PIO_MODE_4 = BIT(4),
    PIO_MODE_5 = BIT(5),
    PIO_MODE_6 = BIT(6),
    PIO_MODE_7 = BIT(7),
    PIO_MODE_NONE = 0,
} pio_mode_t;

static uint8_t const g_pio_pins_avaliable_mode[PIN_MAX_NUMBER] = {
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4,                                        // GPIO_00
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4 | PIO_MODE_5,                           // GPIO_01
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4 | PIO_MODE_5 | PIO_MODE_6,              // GPIO_02
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4 | PIO_MODE_5,                           // GPIO_03
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4 | PIO_MODE_5,                           // GPIO_04
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4 | PIO_MODE_5 | PIO_MODE_6,              // GPIO_05
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4 | PIO_MODE_5 | PIO_MODE_6 | PIO_MODE_7, // GPIO_06
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4 | PIO_MODE_5,                           // GPIO_07
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4,                                        // GPIO_08
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4 | PIO_MODE_5 | PIO_MODE_6 | PIO_MODE_7, // GPIO_09
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4 | PIO_MODE_5,                           // GPIO_10
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4 | PIO_MODE_5 | PIO_MODE_6,              // GPIO_11
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_4 | PIO_MODE_6,                                        // GPIO_12
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4,                                        // GPIO_13
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3 | PIO_MODE_4,                                        // GPIO_14
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,                                                                  // GPIO_15
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,                                                                  // GPIO_16
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,                                                                  // GPIO_17
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,                                                                  // GPIO_18
};

bool pin_check_mode_is_valid(pin_t pin, pin_mode_t mode)
{
    return (bool)((g_pio_pins_avaliable_mode[pin] >> (uint8_t)mode) & 1U);
}

void pin_port_register_hal_funcs(void)
{
    hal_pin_register_funcs(hal_pin_ws63_funcs_get());
}

void pin_port_unregister_hal_funcs(void)
{
    hal_pin_unregister_funcs();
}
