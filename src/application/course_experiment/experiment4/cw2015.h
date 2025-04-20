#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "app_init.h"
#define CW2015_ADDR 0xC4
#define CW2015_REG_MODE 0x0A
#define CW2015_REG_VCELL_HIGH 0x02
#define CW2015_REG_VCELL_LOW 0x03
#define CW2015_MODE_AWAKE 0x00
void cw2015_init(void);
uint32_t cw2015_GetBatteryVoltage(void);
