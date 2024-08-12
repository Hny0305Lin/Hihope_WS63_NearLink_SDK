/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: ADC Sample Source. \n
 *
 * History: \n
 * 2023-07-06, Create file. \n
 */
#include "pinctrl.h"
#include "soc_osal.h"
#include "adc.h"
#include "adc_porting.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"

#define ADC_TASK_STACK_SIZE               0x1000
#define ADC_TASK_PRIO                     (osPriority_t)(17)
#define ADC_AUTO_SAMPLE_TEST_TIMES        2000


void test_adc_callback(uint8_t ch, uint32_t *buffer, uint32_t length, bool *next) 
{ 
     UNUSED(next);
    for (uint32_t i = 0; i < length; i++) {
        printf("channel: %d, voltage: %dmv\r\n", ch, buffer[i]);
    } 
}

static void *adc_task(const char *arg)
{
    UNUSED(arg);
    osal_printk("start adc sample test");
    uapi_adc_init(ADC_CLOCK_500KHZ);
    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, true);
    adc_scan_config_t config = {
        .type = 0,
        .freq = 1,
    };
    
    while (1)
    {
        uapi_adc_auto_scan_ch_enable(ADC_CHANNEL_0, config, test_adc_callback);
        uapi_adc_auto_scan_ch_disable(ADC_CHANNEL_0);
        osal_msleep(ADC_AUTO_SAMPLE_TEST_TIMES);
    }
    
    return NULL;
}

static void adc_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "ADCTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = ADC_TASK_STACK_SIZE;
    attr.priority = ADC_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)adc_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the adc_entry. */
app_run(adc_entry);