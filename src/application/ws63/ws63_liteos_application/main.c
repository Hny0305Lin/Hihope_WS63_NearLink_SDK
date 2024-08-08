/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Application core main function for standard \n
 *
 * History: \n
 * 2022-07-27, Create file. \n
 */

#include "cmsis_os2.h"
#include "stdlib.h"
#include "uart.h"
#include "exception.h"
#include "timer.h"
#include "arch/cache.h"
#include "osal_task.h"
#include "systick.h"
#include "systick_porting.h"
#include "tcxo.h"
#include "watchdog.h"
#include "sfc.h"
#include "security_init.h"
#include "tsensor.h"
#ifdef MBEDTLS_HARDEN_OPEN
#include "mbedtls_harden_adapt.h"
#endif
#include "soc_osal.h"
#ifdef AT_COMMAND
#include "at_config.h"
#include "at_product.h"
#ifdef CONFIG_AT_SUPPORT_ZDIAG
#include "at_zdiag.h"
#endif
#ifdef CONFIG_AT_SUPPORT_PLT
#include "at_plt_cmd_register.h"
#endif
#endif
#include "chip_io.h"
#if defined(DEVICE_ONLY) // 这个宏指的是BGLE DEVICE ONLY
#include "wvt_uart.h"
#endif
#include "flash_patch.h"

#ifdef TEST_SUITE
#include "test_suite_uart.h"
#include "test_suite.h"
#include "test_auxiliary.h"
#endif

#ifdef LOG_SUPPORT
#include "log_memory_region.h"
#include "log_common.h"
#include "log_uart.h"
#include "log_oam_logger.h"
#include "log_oml_exception.h"
#endif

#include <los_memory.h>
#include "share_mem_config.h"
#include "pmp_cfg.h"
#include "dyn_mem.h"
#ifdef HSO_SUPPORT
#include "dfx_system_init.h"
#include "diag_rom_api.h"
#include "soc_diag_util.h"
#undef THIS_FILE_ID
#define THIS_FILE_ID _MAIN_C_
#endif
#include "arch/exception.h"
#include "los_task_pri.h"

#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
#include "nv.h"
#if defined(CONFIG_OTA_UPDATE_SUPPORT)
#include "nv_config.h"
#if (defined(CONFIG_NV_SUPPORT_OTA_UPDATE) && (defined(NV_YES)) && (CONFIG_NV_SUPPORT_OTA_UPDATE == NV_YES))
#include "upg_common_porting.h"
#include "cipher.h"
#include "nv_upg.h"
#endif
#endif
#endif
#ifdef CONFIG_MIDDLEWARE_SUPPORT_LFS
#include "littlefs_adapt.h"
#endif

#include "pinctrl.h"
#include "gpio.h"
#include "partition.h"
#include "main.h"

#ifdef BOARD_ASIC
#include "clock_init.h"
#endif
#include "efuse.h"
#include "efuse_porting.h"
#ifdef WIFI_TASK_EXIST
#include "mac_addr.h"
#endif

#include "app_init.h"
#include "reboot_porting.h"
#include "xo_trim_porting.h"
#if defined(CONFIG_PM_SUPPORT_POWER_EXCEPTION_DEBUG) && (CONFIG_PM_SUPPORT_POWER_EXCEPTION_DEBUG == 1)
#include "pm_pwr_dbg.h"
#endif
#ifdef CONFIG_PM_SUPPORT_SRV_DECOUPLING
#include "pm_porting.h"
#endif

#define WDT_TIMEOUT_S           15   /* wdg超时时间15s */

#define TASK_PRIORITY_LOG       25
#define TASK_PRIORITY_APP       27
#define TASK_PRIORITY_CMD       1
#define TASK_PRIORITY_BT        1
#define TASK_PRIORITY_SDK       12
#define TASK_PRIORITY_WF        25
#define TASK_PRIORITY_BTH_SDK   13
#define TASK_PRIORITY_BTH_RECV  10
#define TASK_PRIORITY_SRV       12

#define STACK_SIZE_BASELINE     0x200
#define FS_EXTEND_STACK         (STACK_SIZE_BASELINE * 3)
#define APP_STACK_SIZE          0x800
#define TESTSUITE_STACK_SIZE    0x1000
#define HSO_STACK_SIZE          0x800
#define BT_STACK_SIZE           (STACK_SIZE_BASELINE * 4 + FS_EXTEND_STACK)
#define BT_SDK_STACK_SIZE       0x800
#define BTH_SDK_STACK_SIZE      0x200
#define BTH_RECV_STACK_SIZE     0x800
#define BTH_SERVICE_STACK_SIZE (STACK_SIZE_BASELINE * 8)
#define AT_STACK_SIZE           0x2000
#define WIFI_STACK_SIZE         0x2000

#ifdef CONFIG_RADAR_SERVICE
#define TASK_PRIORITY_RD_D        23
#define TASK_PRIORITY_RD_F        24
#define RADAR_STACK_SIZE_D        0x800
#define RADAR_STACK_SIZE_F        0x2600
#define RADAR_STACK_SIZE          0x400
#endif

typedef struct {
    const char *name;
    uint32_t stack_size;
    uint32_t priority;
    osal_kthread_handler func;
} app_task_definition_t;

static osStatus_t g_kernel_state = osError;

#ifdef TEST_SUITE
static void cmd_processor_entry(void *args);
static void test_suite_main_add_functions(void);
static int test_fun(int argc, const char *argv[]);
#endif

#ifdef CONFIG_RADAR_SERVICE
static int radar_driver_task_weakref(void *param) __attribute__ ((weakref("radar_driver_task")));
static int radar_feature_task_weakref(void *param) __attribute__ ((weakref("radar_feature_task")));
static int radar_demo_init_weakref(void *param) __attribute__ ((weakref("radar_demo_init")));
static void at_radar_cmd_register_weakref(void) __attribute__ ((weakref("at_radar_cmd_register")));

#endif
#ifdef WIFI_TASK_EXIST
static void at_sys_cmd_register_weakref(void) __attribute__ ((weakref("at_sys_cmd_register")));
#endif

#ifdef BTH_TASK_EXIST
static void at_bt_cmd_register_weakref(void) __attribute__ ((weakref("at_bt_cmd_register")));
void bt_acore_task_main(void);
void bt_tran_task_queue_init(void);
void recv_data_task(void);
void btsrv_task_body(const void *data);
void sdk_msg_thread(void);
#endif
#ifndef CHIP_EDA
static void app_main(const void *unused);
#endif
static const app_task_definition_t g_app_tasks[] = {
#ifndef CHIP_EDA
    {"app", APP_STACK_SIZE, TASK_PRIORITY_APP, (osal_kthread_handler)app_main},
#endif
#ifdef TEST_SUITE
    {"cmd_loop", TESTSUITE_STACK_SIZE, TASK_PRIORITY_CMD, (osal_kthread_handler)cmd_processor_entry},
#endif
#ifdef HSO_SUPPORT
    {"log", HSO_STACK_SIZE, TASK_PRIORITY_LOG, (osal_kthread_handler)log_main},
#endif
#ifdef BGLE_TASK_EXIST
#ifdef SPARKLYZER_ACTIVATED
    {"bt", BT_STACK_SIZE, TASK_PRIORITY_BT, (osal_kthread_handler)splz_thread_handle},
#else
    {"bt", BT_STACK_SIZE, TASK_PRIORITY_BT, (osal_kthread_handler)bt_thread_handle},
#endif
#endif
#ifdef BTH_TASK_EXIST
    {"bt_sdk",          BT_SDK_STACK_SIZE,   TASK_PRIORITY_SDK,      (osal_kthread_handler)bt_acore_task_main},
    {"bth_sdk",         BTH_SDK_STACK_SIZE,  TASK_PRIORITY_BTH_SDK,  (osal_kthread_handler)sdk_msg_thread},
    {"recvBthDataTask", BTH_RECV_STACK_SIZE, TASK_PRIORITY_BTH_RECV, (osal_kthread_handler)recv_data_task},
    { "bt_service",   BTH_SERVICE_STACK_SIZE, TASK_PRIORITY_SRV,     (osal_kthread_handler)btsrv_task_body},
#endif
#ifdef AT_COMMAND
    {"at", AT_STACK_SIZE, TASK_PRIORITY_CMD, (osal_kthread_handler)uapi_at_msg_main},
#endif
#ifndef DEVICE_BGLE_ALL
#ifdef WIFI_TASK_EXIST
    /* 栈大小待确认 任务优先级待确认 */
    {"wifi", WIFI_STACK_SIZE, TASK_PRIORITY_WF, (osal_kthread_handler)wifi_host_task},
#endif
#ifdef CONFIG_RADAR_SERVICE
    {"radar_driver", RADAR_STACK_SIZE_D, TASK_PRIORITY_RD_D, (osal_kthread_handler)radar_driver_task_weakref},
    {"radar_feature", RADAR_STACK_SIZE_F, TASK_PRIORITY_RD_F, (osal_kthread_handler)radar_feature_task_weakref},
    {"radar_demo", RADAR_STACK_SIZE, TASK_PRIORITY_RD_F, (osal_kthread_handler)radar_demo_init_weakref},
#endif
#endif
#ifdef CONFIG_SUPPORT_HILINK
    {"hilink", WIFI_STACK_SIZE, TASK_PRIORITY_WF, (osal_kthread_handler)hilink_entry},
#endif
};

static void systick_cali_xclk_bottom_half(void);

#define APP_MAIN_DELAY_TIME 1000 /* 1000 tick，1 tick=10ms，共10s */
#define M_NUM_TASKS (sizeof(g_app_tasks) / sizeof(app_task_definition_t))
#define PATCH_NUM 194
#define PATCH_REMAP_TAB_WORD_NUM 2
#define PATCH_CMP_HEADINFO_NUM 3
static uint32_t patch_remap[PATCH_NUM * PATCH_REMAP_TAB_WORD_NUM] __attribute__((section(".patch_remap"))) = { 0 };
static uint32_t patch_cmp[PATCH_NUM + PATCH_CMP_HEADINFO_NUM] __attribute__((section(".patch_cmp"))) = { 0 };

static void patch_init(void)
{
    riscv_cfg_t patch_cfg;
    patch_cfg.cmp_start_addr = (uint32_t)(uintptr_t)((void*)patch_cmp);
    patch_cfg.remap_addr = (uint32_t)(uintptr_t)((void*)patch_remap);
    patch_cfg.off_region = true;
    patch_cfg.flplacmp0_en = 0;
    patch_cfg.flplacmp1_en = 0;
    riscv_patch_init(patch_cfg);
}

static uint32_t g_at_uart_recv_cnt = 0;
#ifndef CHIP_EDA
static void app_main(const void *unused)
{
    LOS_MEM_POOL_STATUS status;
    UNUSED(unused);
    (void)osDelay(100); // 100: 100tiks = 1s
    systick_cali_xclk_bottom_half();
    while (1) {
        (void)osDelay(APP_MAIN_DELAY_TIME);
        LOS_MemInfoGet(m_aucSysMem0, &status);
        PRINT("[SYS INFO] mem: used:%u, free:%u; log: drop/all[%u/%u], at_recv %u.\r\n", status.uwTotalUsedSize,
            status.uwTotalFreeSize, log_get_missed_messages_count(), log_get_all_messages_count(), g_at_uart_recv_cnt);
    }
}
#endif

#ifdef TEST_SUITE
static void cmd_processor_entry(void *args)
{
    UNUSED(args);
    uapi_test_suite_init();
    test_suite_main_add_functions();
}
#endif

static void main_initialise(const void *unused1, uint32_t unused2)
{
    osal_task *task_handle = NULL;
    UNUSED(unused1);
    UNUSED(unused2);
    osal_kthread_lock();
    for (uint32_t i = 0; i < (sizeof(g_app_tasks) / sizeof(g_app_tasks[0])); i++) {
        if (g_app_tasks[i].func == NULL) {
            PRINT("WARNING: main_initialise::thread[%d] func is null\r\n", i);
            continue;
        }
        task_handle = osal_kthread_create(g_app_tasks[i].func, NULL, g_app_tasks[i].name, g_app_tasks[i].stack_size);
        if (task_handle == NULL) {
            PRINT("ERROR: main_initialise::osal_kthread_create[%d] [%s] failed\r\n", i, g_app_tasks[i].name);
            continue;
        }
        if (osal_kthread_set_priority(task_handle, g_app_tasks[i].priority) != OSAL_SUCCESS) {
            PRINT("ERROR: main_initialise::osal_kthread_set_priority[%d] failed\r\n", i);
        }
    }
    osal_kthread_unlock();
}

#ifdef TEST_SUITE
static int test_fun(int argc, const char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    PRINT("Test Hso Last Dump\r\n");
    WRITE_UINT32(0x0, 0x0);
    return 0;
}

static void test_suite_main_add_functions(void)
{
    add_auxiliary_functions();
    uapi_test_suite_add_function("test_fun", "Test functions", (test_suite_function_callback_t)test_fun);
}
#endif

static const sfc_flash_config_t sfc_cfg = {
    .read_type = FAST_READ_QUAD_OUTPUT,
    .write_type = PAGE_PROGRAM,
    .mapping_addr = 0x200000,
    .mapping_size = 0x800000,
};

#ifdef BOARD_ASIC
static void bypass_uart_auto_gate(void)
{
#define IP_AUTO_CG_BYPASS 0x44000244
#define BIT_UART0_CK_EN_HW 10
#define BIT_UART1_CK_EN_HW 11
#define BIT_UART2_CK_EN_HW 12
#define BIT_UART0_AUTO_CG_BYPASS 0
#define BIT_UART1_AUTO_CG_BYPASS 1
#define BIT_UART2_AUTO_CG_BYPASS 2
    // bypass uart auto gate
    reg_setbit(IP_AUTO_CG_BYPASS, 0, BIT_UART0_CK_EN_HW);
    reg_setbit(IP_AUTO_CG_BYPASS, 0, BIT_UART1_CK_EN_HW);
    reg_setbit(IP_AUTO_CG_BYPASS, 0, BIT_UART2_CK_EN_HW);
    reg_setbit(IP_AUTO_CG_BYPASS, 0, BIT_UART0_AUTO_CG_BYPASS);
    reg_setbit(IP_AUTO_CG_BYPASS, 0, BIT_UART1_AUTO_CG_BYPASS);
    reg_setbit(IP_AUTO_CG_BYPASS, 0, BIT_UART2_AUTO_CG_BYPASS);
}
#endif

/* 利用tcxo校准systick */
static uint64_t systick_count1;
static uint64_t tcxo_count1;

static void systick_cali_xclk_upper_half(void)
{
    uint32_t irq_sts = osal_irq_lock();
    systick_count1 = uapi_systick_get_count();
    tcxo_count1 = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
}

static void systick_cali_xclk_bottom_half(void)
{
    uint32_t cali_clock;
    uint64_t systick_count2;
    uint64_t tcxo_count2;

    uint32_t irq_sts = osal_irq_lock();
    systick_count2 = uapi_systick_get_count();
    tcxo_count2 = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);

    /*
     *   Increase of actual systic_count        Increase of us count
     *  (systick_count2 - systick_count1)    (tcxo_count2 - tcxo_count1)
     *  --------------------------------  =  ---------------------------
     *        g_systick_clock                      10000000us(1s)
     *
     *  1000000.0: 使用浮点计算中间结果
     */
    cali_clock = (uint32_t)(((systick_count2 - systick_count1) * 1000000.0) /
                            (tcxo_count2 - tcxo_count1));
    systick_clock_set(cali_clock);
}

static void hw_init(void)
{
#ifdef BOARD_ASIC
    open_rf_power();
    switch_clock();
    bypass_uart_auto_gate();
#endif
    uapi_pin_init();
    uapi_gpio_init();
#ifdef SW_UART_DEBUG
    sw_debug_uart_init(CONFIG_DEBUG_UART_BAUDRATE);
    PRINT("Debug uart init succ.\r\n");
#endif
    uapi_timer_init();
    uapi_timer_adapter(1, TIMER_1_IRQN, irq_prio(TIMER_1_IRQN));
    uapi_systick_init();
    uapi_tcxo_init();
    systick_cali_xclk_upper_half();
    // 软件初始化watchdog
    uapi_watchdog_init(WDT_TIMEOUT_S);
    uapi_watchdog_set_time(WDT_TIMEOUT_S);
    uapi_watchdog_enable(WDT_MODE_INTERRUPT);
    set_efuse_period();
    uapi_efuse_init(); // efuse函数初始化

    uapi_sfc_init((sfc_flash_config_t *)&sfc_cfg);

#if defined(CONFIG_PM_SUPPORT_POWER_EXCEPTION_DEBUG) && (CONFIG_PM_SUPPORT_POWER_EXCEPTION_DEBUG == 1)
    pm_pwr_dbg_init();
#endif

    uapi_tsensor_init();
    // 默认采样周期为 0xFFFF 个32K时钟
    uapi_tsensor_start_inquire_mode(TSENSOR_SAMP_MODE_AVERAGE_CYCLE, 0xFFFF);
    uapi_drv_cipher_env_init();
#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
    uapi_nv_init();
#if defined(CONFIG_OTA_UPDATE_SUPPORT)
#if (defined(CONFIG_NV_SUPPORT_OTA_UPDATE) && (defined(NV_YES)) && (CONFIG_NV_SUPPORT_OTA_UPDATE == NV_YES))
    (void)ws63_upg_init();
    (void)uapi_drv_cipher_hash_init();
    (void)nv_upg_upgrade_task_process();
#endif
#endif
#endif
#ifdef WIFI_TASK_EXIST
    init_dev_addr();
#endif
    cmu_xo_trim_init();
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    // 频偏温补默认不开启 因此没有支持nv则不会开启该功能 注:需要在读取完efuse值之后执行
    cmu_xo_trim_temp_comp_init();
#endif
}

#ifdef AT_COMMAND
#define CRLF_STR                      "\r\n"
#define CR_ASIC_II					0xD
#define AT_RX_BUFFER_SIZE 16

static uint8_t g_at_test_uart_rx_buffer_test[AT_RX_BUFFER_SIZE];

static void at_write_func(const char *data)
{
    uapi_uart_write(AT_UART_BUS, (const uint8_t *)data, strlen(data), 0);
}
static uint8_t g_at_pre_char = 0;
static void at_uart_rx_callback(const void *buffer, uint16_t length, bool error)
{
    errcode_t ret;
    const uint8_t *data = (const uint8_t *)buffer;
    if (error) {
        osal_printk("*******uart error*******\r\n");
    }
#ifndef CHIP_EDA
    if (length == 0) {
        panic(PANIC_TESTSUIT, __LINE__);
    }
#endif
    g_at_uart_recv_cnt += length;
    if (((char *)buffer)[0] == CR_ASIC_II) {
        uapi_uart_write(AT_UART_BUS, (uint8_t *)CRLF_STR, (uint16_t)strlen(CRLF_STR), 0);
    } else {
        uapi_uart_write(AT_UART_BUS, (const uint8_t *)buffer, (uint32_t)length, 0);
    }
    ret = uapi_at_channel_data_recv(AT_UART_PORT, (uint8_t *)buffer, (uint32_t)length);
    if (ret != ERRCODE_SUCC) {
        /* 前一个字符为'\r'时单独一个'\n'导致的CHANNEL_BUSY不打印 */
        if (g_at_pre_char != '\r' || length != 1 || data[0] != '\n' || ret != ERRCODE_AT_CHANNEL_BUSY) {
            osal_printk("\r\nat_uart_rx_callback fail:0x%x\r\n", ret);
        }
    }
    g_at_pre_char = data[length - 1];
}

void at_uart_init(void)
{
    uart_buffer_config_t uart_buffer_config;
    uart_pin_config_t uart_pin_config = {
        .tx_pin = 0,
        .rx_pin = 0,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };
    uart_attr_t uart_line_config = {
        .baud_rate = CONFIG_AT_UART_BAUDRATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };
    uart_buffer_config.rx_buffer_size = AT_RX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_at_test_uart_rx_buffer_test;
    uapi_uart_deinit(AT_UART_BUS);
    (void)uapi_uart_init(AT_UART_BUS, &uart_pin_config, &uart_line_config, NULL, &uart_buffer_config);
    (void)uapi_uart_register_rx_callback(AT_UART_BUS, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                         AT_RX_BUFFER_SIZE, at_uart_rx_callback);
}

static void at_base_api_queue_create(uint32_t msg_count, uint32_t msg_size, unsigned long *queue_id)
{
    osal_msg_queue_create(NULL, (uint16_t)msg_count, queue_id, 0, (uint16_t)msg_size);
}

static uint32_t at_base_api_msg_queue_write(unsigned long queue_id, void *msg_ptr,
                                            uint32_t msg_size, uint32_t timeout)
{
    return (uint32_t)osal_msg_queue_write_copy(queue_id, msg_ptr, msg_size, timeout);
}

static uint32_t at_base_api_msg_queue_read(unsigned long queue_id, void *buf_ptr,
                                           uint32_t *buf_size, uint32_t timeout)
{
    return (uint32_t)osal_msg_queue_read_copy(queue_id, buf_ptr, buf_size, timeout);
}

static void at_base_api_task_pause(void)
{
    osal_yield();
}

static void* at_base_api_malloc(uint32_t size)
{
    return osal_kmalloc(size, OSAL_GFP_ATOMIC);
}

static void at_base_api_free(void *addr)
{
    osal_kfree(addr);
}

static void at_base_api_register(void)
{
    at_base_api_t base_api = {
        .msg_queue_create_func = at_base_api_queue_create,
        .msg_queue_write_func = at_base_api_msg_queue_write,
        .msg_queue_read_func = at_base_api_msg_queue_read,
        .task_pause_func = at_base_api_task_pause,
        .malloc_func = at_base_api_malloc,
        .free_func = at_base_api_free,
    };
    uapi_at_base_api_register(base_api);
}
#endif

static void cpu_cache_init(void)
{
    ArchICacheFlush();
    ArchDCacheInvalidate();
    ArchICacheEnable(CACHE_32KB);
    ArchICachePrefetchEnable(CACHE_PREF_1_LINES);
    ArchDCacheEnable(CACHE_4KB);
    return;
}

#ifdef LOG_SUPPORT
uint32_t *g_hso_buff = NULL;
uint32_t get_hso_buff(void)
{
    return (uint32_t)g_hso_buff;
}
#endif

static void sys_fault_handler(uint32_t exc_type, exc_context_t *exc_buff_addr)
{
    (void)OsDbgTskInfoGet(OS_ALL_TASK_MASK);
    OsExcStackInfo();
    do_fault_handler(exc_type, exc_buff_addr);
}

#ifdef AT_COMMAND
__attribute__((weak)) void at_custom_cmd_register(void)
{
    return;
}

static void do_at_cmd_register(void)
{
    at_custom_cmd_register();
#ifdef CONFIG_AT_SUPPORT_PLT
    at_plt_cmd_register();
#endif
#ifdef WIFI_TASK_EXIST
    if (at_sys_cmd_register_weakref != NULL) {
        at_sys_cmd_register_weakref();
    }
#endif
#ifdef BTH_TASK_EXIST
    if (at_bt_cmd_register_weakref != NULL) {
        at_bt_cmd_register_weakref();
    }
#endif
#ifdef CONFIG_RADAR_SERVICE
    if (at_radar_cmd_register_weakref != NULL) {
        at_radar_cmd_register_weakref();
    }
#endif
}
#endif

__attribute__((weak)) void OHOS_SystemInit(void)
{
    return;
}
LITE_OS_SEC_TEXT_INIT int main(void)
{
    patch_init();
    uapi_partition_init();
    pmp_enable();
    cpu_cache_init();
#ifdef BOARD_ASIC
    // 在OS启动之前设置工作时钟
    set_uart_tcxo_clock_period();
#endif
    ArchSetExcHook((EXC_PROC_FUNC)sys_fault_handler);
    LOS_PrepareMainTask();
    g_kernel_state = osKernelInitialize();
    hw_init();
#ifdef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    pm_porting_init();
#endif
#ifdef CONFIG_MEMORY_CUSTOMIZE_RSV
    // 硬件初始化完成后，提前擦除，方便后续重启前写入
    reboot_mem_flash_erase();
#endif

#ifdef MBEDTLS_HARDEN_OPEN
    (void)mbedtls_adapt_register_func();
#endif
#if defined(TEST_SUITE) // testsuite是平台用于跑测试，正常业务版本(包括BGLE DEVICE ONLY)不需要打开。
    uapi_uart_deinit(TEST_SUITE_UART_BUS);
    test_suite_uart_init();
#elif defined(DEVICE_ONLY) // 这个宏指的是BGLE DEVICE ONLY
    wvt_uart_init();
    const uint8_t str1[] = "DEVICE_ONLY UART INIT SUCC!\r\n";
    uapi_uart_write(UART_BUS_2, str1, sizeof(str1), 0);
#endif
#ifdef AT_COMMAND
    at_uart_init();
    PRINT("AT uart init succ.\r\n");
    at_base_api_register();
    uapi_at_channel_write_register(AT_UART_PORT, at_write_func);
#ifdef CONFIG_AT_SUPPORT_ZDIAG
    zdiag_at_init();
#endif
    do_at_cmd_register();
#endif
#ifdef LOG_SUPPORT
    g_hso_buff = (uint32_t *)osal_kmalloc(LOGGING_REGION_LENGTH, 0);
    log_memory_region_init();
    log_init();
    log_oml_ota_init();
    log_oam_status_store_init();
    log_uart_init_after_rtos();
    log_uart_port_init();
    const uint8_t str2[] = "HSO UART INIT SUCC!\r\n";
    uapi_uart_write(LOG_UART_BUS, str2, sizeof(str2), 0);
#endif
#ifdef HSO_SUPPORT
    dfx_system_init();
    hal_register_exception_dump_callback(log_exception_dump);
#endif
#ifdef CONFIG_MIDDLEWARE_SUPPORT_LFS
    PRINT("=========FS MOUNT=========\r\n");
    fs_adapt_mount();
    PRINT("=========FS READY=========\r\n");
#endif

    main_initialise(NULL, 0);

    OHOS_SystemInit();
    app_tasks_init();

    /* Start LiteOS */
    (void)osKernelStart();
    /* jump to main() in asm, so ra is main(), return will loop main() */
    for (;;) {}
    return 0;
}

/* copy ram */
__attribute__((section(".text.runtime.init"))) void copy_bin_to_ram(unsigned int *start_addr,
    const unsigned int *const load_addr, unsigned int size)
{
    unsigned int i;

    for (i = 0; i < size / sizeof(unsigned int); i++) {
        *(start_addr + i) = *(load_addr + i);
    }
}

/* init ram value */
__attribute__((section(".text.runtime.init"))) void init_mem_value(unsigned int *start_addr,
    const unsigned int *const end_addr, unsigned int init_val)
{
    unsigned int *dest = start_addr;

    while (dest < end_addr) {
        *dest = init_val;
        dest++;
    }
}

#ifdef CONFIG_MEMORY_CUSTOMIZE_RSV
#define REG_SYS_RST_RECORD  0x40000098
#define SYS_SOFT_RST        (0x1 << 1)
#define SYS_WDT_RST         (0x1 << 0)
#define MEM_NEED_RESUME     1

/* Determine whether resume is required */
__attribute__((section(".text.runtime.init"))) int is_resume_required(void)
{
    unsigned int sys_rst_record_val = *((unsigned int *)REG_SYS_RST_RECORD);

    if (sys_rst_record_val & (SYS_WDT_RST | SYS_SOFT_RST)) {
        return MEM_NEED_RESUME;
    }
    return 0;
}

#endif

__attribute__((section(".text.runtime.init"))) void do_relocation(void)
{
    /* copy rom_data from flash to DTCM */
    copy_bin_to_ram(&__rom_data_begin__, &__rom_data_load__, (unsigned int)&__rom_data_size__);
    /* clear rom_bss on DTCM */
    init_mem_value(&__rom_bss_begin__, &__rom_bss_end__, 0);

    /* copy rom_patch from flash to ITCM */
    copy_bin_to_ram(&__rom_patch_begin__, &__rom_patch_load__, (unsigned int)&__rom_patch_size__);

    /* copy tcm_text from flash to ITCM */
    copy_bin_to_ram(&__tcm_text_begin__, &__tcm_text_load__, (unsigned int)&__tcm_text_size__);
    /* copy tcm_data from flash to DTCM */
    copy_bin_to_ram(&__tcm_data_begin__, &__tcm_data_load__, (unsigned int)&__tcm_data_size__);
    /* clear tcm_bss on DTCM */
    init_mem_value(&__tcm_bss_begin__, &__tcm_bss_end__, 0);

    /* copy sram_text from flash to SRAM */
    copy_bin_to_ram(&__sram_text_begin__, &__sram_text_load__, (unsigned int)&__sram_text_size__);

    /* copy data from flash to SRAM */
    copy_bin_to_ram(&__data_begin__, &__data_load__, (unsigned int)&__data_size__);

#ifdef CONFIG_MEMORY_CUSTOMIZE_RSV
    if (is_resume_required() == MEM_NEED_RESUME) {
        /* copy saved data from flash to SRAM */
        copy_bin_to_ram(&__mem_rsv_begin__, &__mem_rsv_load__, (unsigned int)&__mem_rsv_size__);
    } else {
        /* clear rsv mem */
        init_mem_value(&__mem_rsv_begin__, &__mem_rsv_end__, 0);
    }
#endif

    /* clear bss on SRAM */
    init_mem_value(&__bss_begin__, &__bss_end__, 0);

    return;
}

__attribute__((section(".text.runtime.init"))) void runtime_init(void)
{
    dyn_mem_cfg();
#ifndef CHIP_EDA
    do_relocation();
#endif
    /* Jump to main */
    main();
}
