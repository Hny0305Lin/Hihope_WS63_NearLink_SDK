/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SPI Sample Source. \n
 *
 * History: \n
 * 2023-06-25, Create file. \n
 */
#include "pinctrl.h"
#include "soc_osal.h"
#include "spi.h"
#include "gpio.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"


#include "ssd1306_fonts.h"
#include "ssd1306.h"

#define SPI_SLAVE_NUM                   1
#define SPI_FREQUENCY                   2
#define SPI_CLK_POLARITY                1
#define SPI_CLK_PHASE                   1
#define SPI_FRAME_FORMAT                0
#define SPI_FRAME_FORMAT_STANDARD       0
#define SPI_FRAME_SIZE_8                0x1f
#define SPI_TMOD                        0
#define SPI_WAIT_CYCLES                 0x10

#define SPI_TASK_STACK_SIZE             0x1000
#define SPI_TASK_DURATION_MS            1000
#define SPI_TASK_PRIO                   (osPriority_t)(17)

static void app_spi_init_pin(void)
{

    if (CONFIG_SPI_MASTER_BUS_ID == 0) {
        uapi_pin_set_mode(CONFIG_SPI_DI_MASTER_PIN, PIN_MODE_3);
        uapi_pin_set_mode(CONFIG_SPI_DO_MASTER_PIN, PIN_MODE_3);
        uapi_pin_set_mode(CONFIG_SPI_CLK_MASTER_PIN, PIN_MODE_3);
        uapi_pin_set_mode(CONFIG_SPI_CS_MASTER_PIN, PIN_MODE_0);
        uapi_gpio_set_dir(CONFIG_SPI_CS_MASTER_PIN, GPIO_DIRECTION_OUTPUT);
        uapi_gpio_set_val(CONFIG_SPI_CS_MASTER_PIN, GPIO_LEVEL_HIGH);
    }
}
static void app_spi_master_init_config(void)
{
    spi_attr_t config = { 0 };
    spi_extra_attr_t ext_config = { 0 };

    config.is_slave = false;
    config.slave_num = SPI_SLAVE_NUM;
    config.bus_clk = 32000000;              //32000000 HZ
    config.freq_mhz = SPI_FREQUENCY;
    config.clk_polarity = SPI_CLK_POLARITY;
    config.clk_phase = SPI_CLK_PHASE;
    config.frame_format = SPI_FRAME_FORMAT;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    config.frame_size = HAL_SPI_FRAME_SIZE_8;
    config.tmod = SPI_TMOD;
    config.sste = 1;

    ext_config.qspi_param.wait_cycles = SPI_WAIT_CYCLES;
    uapi_spi_init(CONFIG_SPI_MASTER_BUS_ID, &config, &ext_config);
}

static void *spi_master_task(const char *arg)
{
    unused(arg);
    /* SPI pinmux. */
    app_spi_init_pin();

    /* SPI master init config. */
    app_spi_master_init_config();

    ssd1306_Init();
    ssd1306_Fill(Black);

    while (1) {
        osal_msleep(SPI_TASK_DURATION_MS);
        ssd1306_SetCursor(0, 0);
        ssd1306_DrawString("Hello BearPi", Font_7x10, White);
        ssd1306_UpdateScreen();
    }

    return NULL;
}

static void spi_master_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "SpiMasterTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = SPI_TASK_STACK_SIZE;
    attr.priority = SPI_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)spi_master_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the spi_master_entry. */
app_run(spi_master_entry);