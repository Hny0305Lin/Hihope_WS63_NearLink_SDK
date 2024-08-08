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
#include "uart.h"
#include "pinctrl.h"

#define WIFI_IFNAME_MAX_SIZE             16
#define WIFI_MAX_SSID_LEN                33
#define WIFI_SCAN_AP_LIMIT               64
#define WIFI_MAC_LEN                     6
#define WIFI_INIT_WAIT_TIME              500 // 5s
#define WIFI_START_STA_DELAY             100 // 1s
#define RPT_CTRL_DELAY                   30  // 300ms

#define RADAR_STATUS_CALI_ISO            1
#define RADAR_STATUS_QUERY_DELAY         1000 // 10s

#define RADAR_DEFAULT_LOOP 8
#define RADAR_DEFAULT_PERIOD 5000
#define RADAR_DEFAULT_DBG_TYPE 0
#define RADAR_DEFAULT_WAVE 2

#define RADAR_API_NO_HUMAN 0
#define RADAR_API_RANGE_CLOSE 50
#define RADAR_API_RANGE_NEAR 100
#define RADAR_API_RANGE_MEDIUM 200
#define RADAR_API_RANGE_FAR 600

#define RADAR_QUIT_TIME 20

#define LOG_UART_BAUD_RATE 9600

#define CTRL_AIR_CONDITIONER_LI

#ifdef CTRL_WATER_HEATER
#define RPT_1M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x0C', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x5D', '\x02', '\x00', '\x0B', '\x77'                           \
    }
#define RPT_2M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x0C', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x5D', '\x02', '\x00', '\x2A', '\xA6'                           \
    }
#define RPT_6M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x0C', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x5D', '\x02', '\x00', '\x2B', '\xA7'                           \
    }
#define RPT_OPEN_CTRL                                                            \
    {                                                                            \
        '\xFF', '\xFF', '\x0C', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x5D', '\x01', '\x00', '\x01', '\x6C'                           \
    }
#define RPT_CLOSE_CTRL                                                           \
    {                                                                            \
        '\xFF', '\xFF', '\x0C', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x5D', '\x01', '\x00', '\x00', '\x6B'                           \
    }
#define NEED_OPEN_CTRL
#endif

#ifdef CTRL_AIR_CONDITIONER_LI
#define RPT_1M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x36', '\x22', '\x26', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x01', '\x06', '\x00', '\x00',  \
        '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x1E', '\x00',  \
        '\x00', '\x00', '\x9E', '\xC1', '\x0D'                                   \
    }
#define RPT_2M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x34', '\x22', '\x26', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x01', '\x06', '\x00', '\x00',  \
        '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x1E', '\x00',  \
        '\x00', '\x00', '\x9C', '\x7E', '\xFE'                                   \
    }
#define RPT_6M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x30', '\x22', '\x26', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x06', '\x02', '\x00', '\x00',  \
        '\x01', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x1E', '\x00',  \
        '\x00', '\x00', '\x98', '\xC3', '\x56'                                   \
    }
#define RPT_CLOSE_CTRL                                                           \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x37', '\x22', '\x26', '\x00', '\x02', '\x00',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x06', '\x06', '\x00', '\x00',  \
        '\x01', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x1E', '\x00',  \
        '\x00', '\x00', '\xA2', '\xA9', '\xAD'                                   \
    }
#define NEED_HANDSHAKE
#endif

#ifdef CTRL_AIR_CONDITIONER_GUA
#define RPT_1M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x36', '\x01', '\x20', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x01', '\x06', '\x00', '\x00',  \
        '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x00', '\x00', '\x59', '\x77', '\x94'                                   \
    }
#define RPT_2M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x34', '\x06', '\x20', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x01', '\x06', '\x00', '\x00',  \
        '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x00', '\x00', '\x5C', '\xA6', '\xCD'                                   \
    }
#define RPT_6M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x30', '\x06', '\x20', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x06', '\x02', '\x00', '\x00',  \
        '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x00', '\x00', '\x59', '\x5F', '\x6A'                                   \
    }
#define RPT_CLOSE_CTRL                                                           \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x30', '\x06', '\x20', '\x00', '\x02', '\x00',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x06', '\x02', '\x00', '\x00',  \
        '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x00', '\x00', '\x58', '\x8E', '\x7A'                                   \
    }
#define NEED_HANDSHAKE
#endif

#ifdef CTRL_AIR_CONDITIONER_GUA_SH
#define RPT_1M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x14', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x0B', '\x01', '\x23', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\xE8', '\x7F', '\xA2'                   \
    }
#define RPT_2M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x14', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x0A', '\x01', '\x23', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\xE7', '\xBA', '\xF3'                   \
    }
#define RPT_6M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x14', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x08', '\x06', '\x21', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\xE8', '\x99', '\xF5'                   \
    }
#define RPT_CLOSE_CTRL                                                           \
    {                                                                            \
        '\xFF', '\xFF', '\x14', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x08', '\x06', '\x21', '\x00', '\x02', '\x00',  \
        '\x00', '\x00', '\x00', '\x00', '\xE7', '\x59', '\xC8'                   \
    }
#define NEED_HANDSHAKE
#endif

#ifdef NEED_HANDSHAKE
#define HANDSHAKE_STEP_ONE                                                       \
    {                                                                            \
        '\xFF', '\xFF', '\x0A', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x61', '\x00', '\x0F', '\x7A'                                           \
    }
#define HANDSHAKE_STEP_TWO                                                       \
    {                                                                            \
        '\xFF', '\xFF', '\x08', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x70', '\xB8', '\x86', '\x41'                                           \
    }
#define HANDSHAKE_STEP_THIRD                                                     \
    {                                                                            \
        '\xFF', '\xFF', '\x0A', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x4D', '\x01', '\x99', '\xB3', '\xB4'                           \
    }
#define HANDSHAKE_STEP_FOUR                                                      \
    {                                                                            \
        '\xFF', '\xFF', '\x08', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x73', '\xBB', '\x87', '\x01'                                           \
    }
#endif

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
static void uapi_radar_set_delay_time_weakref(uint16_t time) __attribute__ ((weakref("uapi_radar_set_delay_time")));
static void radar_print_dbg_data_weakref(uint8_t *buf, uint16_t len) __attribute__ ((weakref("radar_print_dbg_data")));

radar_led_gear_t g_radar_led_gear = RADAR_INSIDE_2M;

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

#ifdef RADAR_ONE_GEAR_CTRL
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
#endif

void radar_uart_port_init(void)
{
    uart_attr_t uart_line_config = {
        .baud_rate = LOG_UART_BAUD_RATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };

    (void)uapi_uart_set_attr(LOG_UART_BUS, &uart_line_config);
}

#ifdef RADAR_ONE_GEAR_CTRL
static void radar_send_open_msg(void)
{
    const uint8_t arr[] = { '\xAA', '\x36', '\xFB', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x02', '\x01',
        '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\xFF', '\x00', '\x00', '\x00', '\x05', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x00', '\x00', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\x00', '\x00', '\xFF',
        '\xFF', '\xD0' };
    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }
}

static void radar_send_close_msg(void)
{
    const uint8_t arr[] = { '\xAA', '\x36', '\xFB', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x02', '\x02',
        '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\xFF', '\x00', '\x00', '\x00', '\x05', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x00', '\x00', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\x00', '\x00', '\xFF',
        '\xFF', '\xCF' };
    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }
}
#else
static void radar_1m_rpt(void)
{
    const uint8_t arr[] = RPT_1M_CTRL;

    PRINT("=================================== cb min: 0~100 ===================================\r\n");

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }

    (void)osDelay(RPT_CTRL_DELAY);
}

static void radar_2m_rpt(void)
{
    const uint8_t arr[] = RPT_2M_CTRL;

    PRINT("=================================== cb min: 100~200 ===================================\r\n");

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }

    (void)osDelay(RPT_CTRL_DELAY);
}

static void radar_6m_rpt(void)
{
    const uint8_t arr[] = RPT_6M_CTRL;

    PRINT("=================================== cb min: 200~600 ===================================\r\n");

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }

    (void)osDelay(RPT_CTRL_DELAY);
}

static void radar_close_machine(void)
{
    const uint8_t arr[] = RPT_CLOSE_CTRL;

    PRINT("=================================== cb min: 0~0 ===================================\r\n");

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }

    (void)osDelay(RPT_CTRL_DELAY);
}

#ifdef NEED_OPEN_CTRL
static void radar_open_machine(void)
{
    const uint8_t arr[] = RPT_OPEN_CTRL;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }
}
#endif
#endif

#ifdef RADAR_ONE_GEAR_CTRL
static void radar_ctrl_proc(radar_result_t *res)
{
    switch (g_radar_led_gear) {
        case RADAR_INSIDE_1M:
            if (res->lower_boundary == 0 && res->upper_boundary == RADAR_API_RANGE_NEAR) {
                radar_set_led_on();
                radar_send_open_msg();
            } else {
                radar_set_led_off();
                radar_send_close_msg();
            }
            break;
        case RADAR_INSIDE_2M:
            if ((res->lower_boundary == RADAR_API_RANGE_NEAR &&
                 res->upper_boundary == RADAR_API_RANGE_MEDIUM) ||
                (res->lower_boundary == 0 && res->upper_boundary == RADAR_API_RANGE_NEAR)) {
                radar_set_led_on();
                radar_send_open_msg();
            } else {
                radar_set_led_off();
                radar_send_close_msg();
            }
            break;
        default:    // 默认6M档位
            if (res->is_human_presence == 1) {
                radar_set_led_on();
                radar_send_open_msg();
            } else {
                radar_set_led_off();
                radar_send_close_msg();
            }
    }
}
#else
static void radar_ctrl_proc(radar_result_t *res)
{
    if (res->is_human_presence == 0) { // 无人
        radar_close_machine();
    } else {
        if (res->lower_boundary == 0 && res->upper_boundary == RADAR_API_RANGE_NEAR) { //1m
#ifdef NEED_OPEN_CTRL
            radar_open_machine();
#endif
            radar_1m_rpt();
        } else if (res->lower_boundary == RADAR_API_RANGE_NEAR && res->upper_boundary == RADAR_API_RANGE_MEDIUM) { // 2m
#ifdef NEED_OPEN_CTRL
            radar_open_machine();
#endif
            radar_2m_rpt();
        } else { // 6m
#ifdef NEED_OPEN_CTRL
            radar_open_machine();
#endif
            radar_6m_rpt();
        }
    }
}
#endif

#ifdef NEED_HANDSHAKE
static void handshake_proc(void)
{
    const uint8_t arr1[] = HANDSHAKE_STEP_ONE;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr1, sizeof(arr1));
    }

    (void)osDelay(RPT_CTRL_DELAY);

    const uint8_t arr2[] = HANDSHAKE_STEP_TWO;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr2, sizeof(arr2));
    }

    (void)osDelay(RPT_CTRL_DELAY);

    const uint8_t arr3[] = HANDSHAKE_STEP_THIRD;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr3, sizeof(arr3));
    }

    (void)osDelay(RPT_CTRL_DELAY);

     const uint8_t arr4[] = HANDSHAKE_STEP_FOUR;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr4, sizeof(arr4));
    }

    (void)osDelay(RPT_CTRL_DELAY);
}
#endif

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

    radar_ctrl_proc(res);

}

static void radar_init_para(void)
{
    if (radar_set_driver_para_weakref != NULL) {
        radar_driver_para_t para;
        para.ant = 0;
        para.dbg_type = RADAR_DEFAULT_DBG_TYPE;
        para.loop = RADAR_DEFAULT_LOOP;
        para.period = RADAR_DEFAULT_PERIOD;
        para.times = 0; // 1-使用软件控雷达, 0-使用硬件控雷达
        para.wave = RADAR_DEFAULT_WAVE;
        radar_set_driver_para_weakref(&para);
    }

    radar_uart_port_init();
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
#ifdef NEED_HANDSHAKE
    handshake_proc();
#endif
    (void)osDelay(WIFI_START_STA_DELAY);
    uapi_radar_set_status(RADAR_STATUS_CALI_ISO);
    (void)osDelay(RADAR_STATUS_QUERY_DELAY);

    for (;;) {
        (void)osDelay(RADAR_STATUS_QUERY_DELAY);
        uint8_t sts;
        uapi_radar_get_status(&sts);
        uint16_t time;
        uapi_radar_get_delay_time(&time);
        uint16_t iso;
        uapi_radar_get_isolation(&iso);
    }

    return 0;
}
