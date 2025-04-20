#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "app_init.h"
#define BH1750_ADDR 0x46

// cmd
#define BH1750_POWER_OFF 0x00
#define BH1750_POWER_ON 0x01
#define BH1750_MODULE_RESET 0x07
#define BH1750_CONTINUE_H_MODE 0x10
#define BH1750_CONTINUE_H_MODE2 0x11
#define BH1750_CONTINUE_L_MODE 0x13
#define BH1750_ONE_TIME_H_MODE 0x20
#define BH1750_ONE_TIME_H_MODE2 0x21
#define BH1750_ONE_TIME_L_MODE 0x23
#define MEASURE_MODE BH1750_CONTINUE_H_MODE
#define BH1750_RES 1
void bh1750_init(void);
uint16_t bh1750_GetLightIntensity(void);
