/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: Radar samples function \n
 * Author: @CompanyNameTag \n
 */

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "soc_osal.h"
#include "radar_service.h"
#include "gpio.h"
#include "pinctrl.h"
#include "app_init.h"

#define WIFI_IFNAME_MAX_SIZE             16
#define WIFI_MAX_SSID_LEN                33
#define WIFI_SCAN_AP_LIMIT               64
#define WIFI_MAC_LEN                     6
#define WIFI_INIT_WAIT_TIME              5000
#define WIFI_START_STA_DELAY             1000

#define RADAR_STATUS_CALI_ISO            4
#define RADAR_STATUS_QUERY_DELAY         10000

#define RADAR_DEFAULT_LOOP 8
#define RADAR_DEFAULT_PERIOD 5000
#define RADAR_DEFAULT_DBG_TYPE 1
#define RADAR_DEFAULT_WAVE 2

#define RADAR_API_NO_HUMAN 0
#define RADAR_API_RANGE_CLOSE 50
#define RADAR_API_RANGE_NEAR 100
#define RADAR_API_RANGE_MEDIUM 200
#define RADAR_API_RANGE_FAR 600

#define CONFIG_RED_LED_PIN 7
#define CONFIG_GREEN_LED_PIN 11
#define CONFIG_YELLOW_LED_PIN 10

#define RADAR_TASK_PRIO              24
#define RADAR_TASK_STACK_SIZE        0x1000

// led档位控制参数
typedef enum {
    RADAR_INSIDE_1M,
    RADAR_INSIDE_2M,
    RADAR_INSIDE_6M,
} radar_led_gear_t;

typedef struct {
    uint8_t times; // 发射次数, 0-无限次
    uint8_t loop; // 单次雷达工作, TRx的波形数量
    uint8_t ant; // Rx天线数量
    uint8_t wave; // 波形选择, 0-320M/40M CTA, 1-160M/20M CW
    uint8_t dbg_type; // 维测方式. 0-不外发维测数据, 1-外发脉压后的数据, 2-外发相干累加后的数据
    uint16_t period; // 雷达工作间隔
} radar_driver_para_t;

static void radar_set_driver_para_weakref(radar_driver_para_t *para) __attribute__ ((weakref("radar_set_driver_para")));
static void gpio_set_value(pin_t pin)
{
    uapi_pin_set_mode(pin, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_OUTPUT);
}

static void radar_led_init(void)
{
    // 1. 初始化所有GPIO并设置GPIO的类型
    uapi_gpio_init();
    gpio_set_value(CONFIG_RED_LED_PIN);
    gpio_set_value(CONFIG_GREEN_LED_PIN);
    gpio_set_value(CONFIG_YELLOW_LED_PIN);
}

static void radar_switch_green_on_off(bool onoff)
{
    if (onoff) {
        uapi_gpio_set_val(CONFIG_GREEN_LED_PIN, GPIO_LEVEL_LOW);
    } else {
        uapi_gpio_set_val(CONFIG_GREEN_LED_PIN, GPIO_LEVEL_HIGH);
    }
}

static void radar_switch_yellow_on_off(bool onoff)
{
    if (onoff) {
        uapi_gpio_set_val(CONFIG_YELLOW_LED_PIN, GPIO_LEVEL_LOW);
    } else {
        uapi_gpio_set_val(CONFIG_YELLOW_LED_PIN, GPIO_LEVEL_HIGH);
    }
}

static void radar_switch_red_on_off(bool onoff)
{
    if (onoff) {
        uapi_gpio_set_val(CONFIG_RED_LED_PIN, GPIO_LEVEL_LOW);
    } else {
        uapi_gpio_set_val(CONFIG_RED_LED_PIN, GPIO_LEVEL_HIGH);
    }
}

static void radar_ctrl_led(radar_result_t *res)
{
    if (res->is_human_presence) {
        radar_switch_green_on_off(true);
    } else {
        radar_switch_green_on_off(false);
    }
    if (res->lower_boundary == 0 && res->upper_boundary == RADAR_API_RANGE_NEAR) {
        printf("R 1 meters\r\n");
        radar_switch_red_on_off(true);
        radar_switch_yellow_on_off(true);
    } else if ((res->lower_boundary == RADAR_API_RANGE_NEAR && res->upper_boundary == RADAR_API_RANGE_MEDIUM)) {
        printf("R 2 meters\r\n");
        radar_switch_red_on_off(false);
        radar_switch_yellow_on_off(true);
    } else {
        printf("R xxxx meters\r\n");
        radar_switch_red_on_off(false);
        radar_switch_yellow_on_off(false);
    }
}

static errcode_t radar_start_sta(void)
{
    osal_msleep(WIFI_INIT_WAIT_TIME);
    printf("STA try enable.\r\n");
    /* 创建STA接口 */
    if (wifi_sta_enable() != 0) {
        printf("sta enbale fail !\r\n");
        return -1;
    }

    /* 连接成功 */
    printf("STA connect success.\r\n");
    return 0;
}

static void radar_printf_res(radar_result_t *res)
{
    printf("[RADAR_SAMPLE] lb:%u, hb:%u, hm:%u\r\n", res->lower_boundary, res->upper_boundary, res->is_human_presence);

    radar_ctrl_led(res);
}

static void radar_init_para(void)
{
    radar_dbg_para_t dbg_para;
    dbg_para.times = 0;
    dbg_para.loop = RADAR_DEFAULT_LOOP;
    dbg_para.ant = 0;
    dbg_para.wave = RADAR_DEFAULT_WAVE;
    dbg_para.dbg_type = RADAR_DEFAULT_DBG_TYPE;
    dbg_para.period = RADAR_DEFAULT_PERIOD;
    uapi_radar_set_debug_para(&dbg_para);
 
    radar_sel_para_t sel_para;
    sel_para.height = 0;
    sel_para.scenario = 0;
    sel_para.material = 2;
    sel_para.fusion_track = 1;
    sel_para.fusion_ai = 1;
    uapi_radar_select_alg_para(&sel_para);
 
    radar_alg_para_t alg_para;
    alg_para.d_th_1m = 32;
    alg_para.d_th_2m = 27;
    alg_para.p_th = 30;
    alg_para.t_th_1m = 13;
    alg_para.t_th_2m = 26;
    alg_para.b_th_ratio = 50;
    alg_para.b_th_cnt = 15;
    alg_para.a_th = 70;
    uapi_radar_set_alg_para(&alg_para, 0);
}

static void radar_demo_init(const char *arg)
{
    UNUSED(arg);
    printf("[RADAR_SAMPLE] radar_demo_init sta!\r\n");
    radar_led_init();
    radar_start_sta();
    uapi_radar_register_result_cb(radar_printf_res);
    radar_init_para();
    // 遍历1~13信道隔离度, 选取最佳隔离度进行雷达探测
    osal_msleep(WIFI_START_STA_DELAY);
    uapi_radar_set_status(RADAR_STATUS_CALI_ISO);
    for (;;) {
        uapi_radar_set_delay_time(8); // 设置退出延迟时间为8S,目前最小时间为8
        osal_msleep(RADAR_STATUS_QUERY_DELAY);
        uint8_t sts;
        uapi_radar_get_status(&sts);
        uint16_t time;
        uapi_radar_get_delay_time(&time);
        uint16_t iso;
        uapi_radar_get_isolation(&iso);
    }
}

static void radar_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)radar_demo_init, 0, "RadarTask", RADAR_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, RADAR_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the pwm_entry. */
app_run(radar_entry);