#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "i2c.h"
#include "app_init.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "watchdog.h"

#define LEDG GPIO_11
#define LEDB GPIO_07
#define LEDR GPIO_09
#define I2C_PCA_ADDR 0x28

void io_expander_init(void);