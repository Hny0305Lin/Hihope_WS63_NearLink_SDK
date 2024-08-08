/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Radar samples function \n
 *
 */

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "radar_service.h"
#include "gpio.h"
#include "pinctrl.h"

#define WIFI_IFNAME_MAX_SIZE             16
#define WIFI_MAX_SSID_LEN                33
#define WIFI_SCAN_AP_LIMIT               64
#define WIFI_MAC_LEN                     6
#define WIFI_INIT_WAIT_TIME              500 // 5s
#define WIFI_START_STA_DELAY             100 // 1s

#define RADAR_STATUS_CALI_ISO            4
#define RADAR_STATUS_QUERY_DELAY         1000 // 10s

#define RADAR_DEFAULT_LOOP 8
#define RADAR_DEFAULT_PERIOD 5000
#define RADAR_DEFAULT_DBG_TYPE 1
#define RADAR_DEFAULT_WAVE 2

#define RADAR_API_NO_HUMAN 0
#define RADAR_API_RANGE_CLOSE 50
#define RADAR_API_RANGE_NEAR 100
#define RADAR_API_RANGE_MEDIUM 200
#define RADAR_API_RANGE_FAR 600

// led档位控制参数
typedef enum {
    RADAR_INSIDE_1M,
    RADAR_INSIDE_2M,
    RADAR_INSIDE_6M,
} radar_led_gear_t;

radar_led_gear_t g_radar_led_gear = RADAR_INSIDE_1M;

/*****************************************************************************
  STA 扫描-关联 sample用例
*****************************************************************************/
void radar_set_led_gear(radar_led_gear_t gear)
{
    PRINT("[RADAR_SAMPLE] SET LED GEAR:%u!\r\n", gear);
    g_radar_led_gear = gear;
}

static void radar_led_init(void)
{
    // 1. 初始化所有GPIO并设置GPIO的类型
    uapi_gpio_init();
    // 2. 设置GPIO为输出
    errcode_t ret = uapi_gpio_set_dir(GPIO_13, GPIO_DIRECTION_OUTPUT);
    if (ret != ERRCODE_SUCC) {
        PRINT("[RADAR_SAMPLE] led uapi_gpio_set_dir failed %u!\r\n", ret);
    }
    // 3. 设置GPIO PIN模式为0，普通模式
    ret = uapi_pin_set_mode(GPIO_13, PIN_MODE_0);
    if (ret != ERRCODE_SUCC) {
        PRINT("[RADAR_SAMPLE] led uapi_pin_set_mode failed %u!\r\n", ret);
    }
}

static void radar_set_led_on(void)
{
    errcode_t ret = uapi_gpio_set_val(GPIO_13, GPIO_LEVEL_HIGH);
    if (ret!= ERRCODE_SUCC) {
        PRINT("[RADAR_SAMPLE] led ctrl failed %u!\r\n", ret);
    }
}

static void radar_set_led_off(void)
{
    errcode_t ret = uapi_gpio_set_val(GPIO_13, GPIO_LEVEL_LOW);
    if (ret!= ERRCODE_SUCC) {
        PRINT("[RADAR_SAMPLE] led ctrl failed %u!\r\n", ret);
    }
}

static void radar_ctrl_led(radar_result_t *res)
{
    switch (g_radar_led_gear) {
        case RADAR_INSIDE_1M:
            if (res->lower_boundary == 0 && res->upper_boundary == RADAR_API_RANGE_NEAR) {
                radar_set_led_on();
            } else {
                radar_set_led_off();
            }
            break;
        case RADAR_INSIDE_2M:
            if ((res->lower_boundary == RADAR_API_RANGE_NEAR &&
                 res->upper_boundary == RADAR_API_RANGE_MEDIUM) ||
                (res->lower_boundary == 0 && res->upper_boundary == RADAR_API_RANGE_NEAR)) {
                radar_set_led_on();
            } else {
                radar_set_led_off();
            }
            break;
        default:    // 默认6M档位
            if (res->is_human_presence == 1) {
                radar_set_led_on();
            } else {
                radar_set_led_off();
            }
    }
}

td_s32 radar_start_sta(td_void)
{
    (void)osDelay(WIFI_INIT_WAIT_TIME); /* 500: 延时0.5s, 等待wifi初始化完毕 */
    PRINT("STA try enable.\r\n");
    /* 创建STA接口 */
    if (wifi_sta_enable() != 0) {
        PRINT("sta enbale fail !\r\n");
        return -1;
    }

    /* 连接成功 */
    PRINT("STA connect success.\r\n");
    return 0;
}

static void radar_print_res(radar_result_t *res)
{
    PRINT("[RADAR_SAMPLE] lb:%u, hb:%u, hm:%u\r\n", res->lower_boundary, res->upper_boundary, res->is_human_presence);

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

int radar_demo_init(void *param)
{
    PRINT("[RADAR_SAMPLE] radar_demo_init sta!\r\n");
    param = param;
    radar_led_init();
    radar_start_sta();
    uapi_radar_register_result_cb(radar_print_res);
    radar_init_para();
    // 遍历1~13信道隔离度, 选取最佳隔离度进行雷达探测
    (void)osDelay(WIFI_START_STA_DELAY);
    uapi_radar_set_status(RADAR_STATUS_CALI_ISO);

    for (;;) {
        (void)osDelay(RADAR_STATUS_QUERY_DELAY);
        uint8_t sts;
        uapi_radar_get_status(&sts);
        uint16_t time;
        uapi_radar_get_delay_time(&time);
        uint16_t iso;
        uapi_radar_get_isolation(&iso);
        radar_result_t res = {0};
        uapi_radar_get_result(&res);
    }

    return 0;
}
