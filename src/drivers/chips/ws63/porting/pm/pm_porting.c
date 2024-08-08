/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provide pm port source. \n
 *
 * History: \n
 * 2023-11-13, Create file. \n
 */

#include <los_task.h>
#include "errcode.h"
#include "chip_io.h"
#include "osal_types.h"
#include "soc_osal.h"
#include "arch_barrier.h"
#include "debug_print.h"
#include "tcxo.h"
#include "timer.h"
#include "timer_porting.h"
#include "soc_porting.h"
#include "uart.h"
#include "uart_porting.h"
#include "dma.h"
#include "gpio.h"
#include "efuse.h"
#include "tsensor.h"
#include "watchdog.h"
#include "clock_init.h"
#include "security_init.h"
#include "pinctrl.h"
#include "efuse.h"
#include "main.h"
#include "wifi_device.h"
#include "hal_watchdog_v151_regs_op.h"
#include "hal_watchdog.h"
#include "radar_service.h"
#include "reboot_porting.h"
#include "pm_porting.h"

// 寄存器地址定义
#define REG_AON_CRG_CKEN_CTL        0x40002100
#define REG_AON_SOFT_RST_CTL        0x40002110

#define REG_PMU_CMULDO1_0P9         0x4000312C
#define REG_PMU_RXLDO_1P0           0x40003134
#define REG_PMU_LNALDO_1P0          0x40003138
#define REG_PMU_VCOLDO_1P0          0x40003140
#define REG_PMU_ABBLDO_1P0          0x40003144
#define REG_PMU_PLLLDO_1P0          0x40003148
#define REG_PMU_TXLDO_1P8           0x40003168
#define REG_PMU_PHY_LDO             0x4000316C
#define REG_EXCEP_RO_RG             0x4000319C
#define REG_ISO_SIG                 0x40003230

#define REG_FNPLL_CFG0              0x4000340C
#define REG_FNPLL_CFG1              0x40003410
#define REG_FNPLL_CFG2              0x40003414
#define REG_FNPLL_CFG3              0x40003418
#define REG_FNPLL_CFG4              0x4000341C
#define REG_FNPLL_CFG5              0x40003420

#define REG_CMU_FNPLL_SIG           0x4000342C
#define REG_CMU_FBDIV               0x40003430
#define REG_CMU_FRAC                0x40003434
#define REG_CMU_CLK_480M_WDBB       0x4000344C

#define REG_WDT_CR                  0x40006010

#define REG_CLDO_SUB_CRG_CKEN_CTL0  0x44001100
#define REG_CLDO_SUB_CRG_CKEN_CTL1  0x44001104
#define REG_CLDO_CRG_DIV_CTL0       0x44001108
#define REG_CLDO_CRG_DIV_CTL7       0x44001124
#define REG_CLDO_CRG_CLK_SEL        0x44001134
#define REG_CLDO_CRG_RST_SOFT_CFG0  0x44001138
#define REG_CLDO_CRG_RST_SOFT_CFG1  0x4400113C

#define TCXO_CLOCK_24M  24000000
#define TCXO_CLOCK_40M  40000000
#define PLL_CLOCK_160M  160000000
#define PMU_RF_OPEN_DELAY_US 120
#define RF_PLL_LOCK_DELAY_US 100
#define PMU_PROCESS_DELAY_US 10
#define UART_PRINT_DELAY_MS 10
#define WIFI_BLE_HW_PROCESS_DELAY_US 10

// 恢复时FNPLL锁定及查询处理
#define PLL_LOCKED   1
#define PLL_UNLOCKED 0
#define FNPLL_LOCK_RETRY_DELAY_US 2
#define FNPLL_LOCK_QUERY_DELAY_MS 1
#define FNPLL_LOCK_RETRY_TIMES 3
#define FNPLL_LOCK_QUERY_TIMES 30

// 当前记录 GPIO 0-14 的IOCFG信息, 保留UART0/1
#define IOCFG_RECORD_NUM 15
#define REG_GPIO_SEL_START_ADDR 0x4400D000
#define REG_GPIO_PAD_START_ADDR 0x4400D800
#define REG_ADDR_INTERVAL 0x4
#define VAL_GPIO_SEL_DEFAULT 0x0
#define VAL_GPIO_PAD_DEFAULT 0x800

// 仅保留WKUP_UART允许唤醒, 其余UART低功耗过程中关中断
#define WKUP_UART                   CONFIG_PM_SUPPORT_WKUP_UART

// 看门狗默认超时时间
#define WDT_TIMEOUT_S 15

#define PM_ENTER_LOWPOWER_WFI_DELAY_MS  200
#define PM_WAIT_EXIT_LOWPOWER_MS        100
#define PM_WAIT_FLASH_WORK_DOWN_MS       10

typedef enum {
    PM_CRG_CKEN_CTL0 = 0,
    PM_CRG_CKEN_CTL1,
    PM_CRG_RST_CTL0,
    PM_CRG_RST_CTL1,
    PM_CRG_RECORD_MAX
} pm_crg_record_index_t;

// 相关寄存器状态记录变量
uint32_t g_pm_iocfg_sel_record[IOCFG_RECORD_NUM] = {};
uint32_t g_pm_iocfg_pad_record[IOCFG_RECORD_NUM] = {};
uint32_t g_pm_crg_record[PM_CRG_RECORD_MAX] = {};
uint32_t g_pm_uart_clock_record[UART_BUS_MAX_NUMBER] = {};
uint32_t g_pm_wdt_timeout = 0;
uint32_t g_pm_wdt_mode = 0;

#ifdef CONFIG_PM_SUPPORT_SRV_DECOUPLING
typedef enum {
    PM_CTRL_INIT,
    PM_CTRL_RESUME,
    PM_CTRL_SUSPEND,
    PM_CTRL_MAX,
} pm_ctrl_mode;

typedef struct _pm_srv_ctrl {
    pm_srv_mode srv_mode[PM_SRV_MAX];
    pm_ctrl_mode ctrl_mode;
    pm_ctrl_mode ctrl_protect;
} pm_srv_ctrl;

pm_srv_ctrl g_pm_srv_ctrl = {0};

static void pm_print_srv_stats(void)
{
    uint32_t srv_index = 0;
    while (srv_index < PM_SRV_MAX) {
        PRINT("pm_srv[%u] mode[%u].\r\n", srv_index, g_pm_srv_ctrl.srv_mode[srv_index]);
        srv_index++;
    }
    PRINT("pm_ctrl:[%u] :protect[%u].\r\n", g_pm_srv_ctrl.ctrl_mode, g_pm_srv_ctrl.ctrl_protect);
}

static void pm_config_srv_power(pm_srv_type svr, pm_srv_mode mode)
{
    PRINT("[pm_port] svc[%u] mode change: [%u] -> [%u]\r\n", svr, g_pm_srv_ctrl.srv_mode[svr], mode);
    if ((mode == PM_MODE_SUSPEND) && (g_pm_srv_ctrl.srv_mode[svr] != PM_MODE_SUSPEND)) {
        /* 关闭模块时钟和模块复位 */
        if (svr == PM_SRV_WIFI) {
            pm_port_close_wifi_hw();
        }
        if (svr == PM_SRV_BLE) {
            pm_port_close_bt_hw();
        }
    } else if ((mode == PM_MODE_RESUME) && (g_pm_srv_ctrl.srv_mode[svr] == PM_MODE_SUSPEND)) {
       /* 恢复模块时钟 */
        if (svr == PM_SRV_WIFI) {
            pm_port_open_wifi_hw();
        }
        if (svr == PM_SRV_BLE) {
            pm_port_open_bt_hw();
        }
    }
    PRINT("[pm_port] mode change finish.\r\n");
}

errcode_t pm_set_srv_mode(pm_srv_type svr, pm_srv_mode mode)
{
    uint32_t srv_index = 0;
    uint32_t irq_flag;
    bool pm_suspend = true;

    if (svr >= PM_SRV_PLAT || mode >= PM_MODE_MAX) {
        return ERRCODE_FAIL;
    }

    PRINT("pm_set_srv_mode srv:%u, mode:%u.\r\n", svr, mode);
    irq_flag = osal_irq_lock();
    pm_config_srv_power(svr, mode);
    g_pm_srv_ctrl.srv_mode[svr] = mode;
    if (mode > PM_MODE_SUSPEND) {
        g_pm_srv_ctrl.srv_mode[PM_SRV_PLAT] = PM_MODE_RESUME;
        osal_irq_restore(irq_flag);
        return ERRCODE_SUCC;
    }
    while (srv_index < PM_SRV_PLAT) {
        if (g_pm_srv_ctrl.srv_mode[srv_index] > PM_MODE_SUSPEND) {
            pm_suspend = false;
            break;
        }
        srv_index++;
    }
    if (pm_suspend) {
        PRINT("pm_set_srv_mode plat suspend.\r\n");
        g_pm_srv_ctrl.srv_mode[PM_SRV_PLAT] = PM_MODE_SUSPEND;
    }
    osal_irq_restore(irq_flag);
    return ERRCODE_SUCC;
}

errcode_t pm_porting_init(void)
{
    uint32_t srv_index = 0;
    while (srv_index < PM_SRV_MAX) {
        g_pm_srv_ctrl.srv_mode[srv_index] = PM_MODE_INIT;
        srv_index++;
    }
    g_pm_srv_ctrl.ctrl_mode = PM_CTRL_INIT;
    g_pm_srv_ctrl.ctrl_protect = PM_CTRL_INIT;
    return ERRCODE_SUCC;
}
#endif

// 低功耗处理回调
pm_hook_cb_t g_pm_hook_cb_list[PM_HOOK_MAX_NUM] = {
    pm_port_enter_lowpower_prepare_cb_default,
    pm_port_enter_lowpower_finish_cb_default,
    pm_port_exit_lowpower_prepare_cb_default,
    pm_port_exit_lowpower_finish_cb_default
};

// wifi bt 硬件状态记录
bool g_pm_is_wifi_hw_open = true;
bool g_pm_is_bt_hw_open = true;

// 低功耗状态记录
#ifndef CONFIG_PM_SUPPORT_SRV_DECOUPLING
bool g_pm_is_in_lowpower = false;
bool g_pm_is_in_process = false;
#endif

#define HW_CTL              0x40000014
#define EXCEP_RO_RG         0x4000319c
#define CMU_CLK_320M_WDBB   0x40003448
#define CMU_CLK_480M_WDBB   0x4000344c
#define CMU_CLK_24M_USB     0x40003454
#define CLDO_CRG_CLK_SEL    0x44001134
#define CLDO_CRG_DIV_CTL0   0x44001108

#define CLOCK_MASK    0x1
#define CLOCK_24MHZ   0x1

static void check_efuse_boot_done(void)
{
#define EFUSE_STS 0x4400802c
#define EFUSE_BOOT_DONE_MASK 0x4
#define MAX_RETRY_TIMES 50
    uint32_t data;

    for (uint32_t i = 0; i < MAX_RETRY_TIMES; i++) {
        data = readl(EFUSE_STS);
        if ((data & EFUSE_BOOT_DONE_MASK) == EFUSE_BOOT_DONE_MASK) {
            break;
        }
        uapi_tcxo_delay_ms(1);
    }
}

static void set_efuse_clock_period(void)
{
#define EFUSE_CLOCK_PERIOD  0x44008034
#define EFUSE_24M_CLOCK_PERIOD 0x29
#define EFUSE_40M_CLOCK_PERIOD 0x19
    uint32_t reg_value = 0;
    reg_value = readl(HW_CTL);
    if ((reg_value & CLOCK_MASK) == CLOCK_24MHZ) {
        writel(EFUSE_CLOCK_PERIOD, EFUSE_24M_CLOCK_PERIOD);
    } else {
        writel(EFUSE_CLOCK_PERIOD, EFUSE_40M_CLOCK_PERIOD);
    }
}

void pm_port_open_rf(void)
{
    uapi_reg_setbits(REG_PMU_RXLDO_1P0, 14, 1, 0x1);    // 0x1 -> 0x40003134 bit 14
    uapi_tcxo_delay_us(PMU_RF_OPEN_DELAY_US);
    uapi_reg_setbits(REG_PMU_RXLDO_1P0, 13, 1, 0x1);    // 0x1 -> 0x40003134 bit 13
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);

    uapi_reg_setbits(REG_PMU_PLLLDO_1P0, 14, 1, 0x1);   // 0x1 -> 0x40003148 bit 14
    uapi_tcxo_delay_us(PMU_RF_OPEN_DELAY_US);
    uapi_reg_setbits(REG_PMU_PLLLDO_1P0, 13, 1, 0x1);   // 0x1 -> 0x40003148 bit 13
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);

    uapi_reg_setbits(REG_PMU_LNALDO_1P0, 14, 1, 0x1);   // 0x1 -> 0x40003138 bit 14
    uapi_tcxo_delay_us(PMU_RF_OPEN_DELAY_US);
    uapi_reg_setbits(REG_PMU_LNALDO_1P0, 13, 1, 0x1);   // 0x1 -> 0x40003138 bit 13
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);

    uapi_reg_setbits(REG_PMU_VCOLDO_1P0, 14, 1, 0x1);   // 0x1 -> 0x40003140 bit 14
    uapi_tcxo_delay_us(PMU_RF_OPEN_DELAY_US);
    uapi_reg_setbits(REG_PMU_VCOLDO_1P0, 13, 1, 0x1);   // 0x1 -> 0x40003140 bit 13
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);

    uapi_reg_setbits(REG_PMU_TXLDO_1P8, 14, 1, 0x1);    // 0x1 -> 0x40003168 bit 14
    uapi_tcxo_delay_us(PMU_RF_OPEN_DELAY_US);
    uapi_reg_setbits(REG_PMU_TXLDO_1P8, 13, 1, 0x1);    // 0x1 -> 0x40003168 bit 13
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);

    uapi_reg_setbits(REG_PMU_ABBLDO_1P0, 14, 1, 0x1);   // 0x1 -> 0x40003144 bit 14
    uapi_tcxo_delay_us(PMU_RF_OPEN_DELAY_US);
    uapi_reg_setbits(REG_PMU_ABBLDO_1P0, 13, 1, 0x1);   // 0x1 -> 0x40003144 bit 13
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);

    uapi_reg_write(REG_ISO_SIG, 0x3);                   // 0x3 -> 0x40003230
    uapi_tcxo_delay_us(RF_PLL_LOCK_DELAY_US);
}

void pm_port_close_rf(void)
{
    uapi_reg_write(REG_ISO_SIG, 0xC);                   // 0xC -> 0x40003230
    uapi_reg_setbits(REG_PMU_ABBLDO_1P0, 13, 2, 0x0);   // 0x0 -> 0x40003144 bit 14:13 (2 bits)
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);
    uapi_reg_setbits(REG_PMU_TXLDO_1P8, 13, 2, 0x0);    // 0x0 -> 0x40003168 bit 14:13 (2 bits)
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);
    uapi_reg_setbits(REG_PMU_VCOLDO_1P0, 13, 2, 0x0);   // 0x0 -> 0x40003140 bit 14:13 (2 bits)
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);
    uapi_reg_setbits(REG_PMU_LNALDO_1P0, 13, 2, 0x0);   // 0x0 -> 0x40003138 bit 14:13 (2 bits)
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);
    uapi_reg_setbits(REG_PMU_PLLLDO_1P0, 13, 2, 0x0);   // 0x0 -> 0x40003148 bit 14:13 (2 bits)
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);
    uapi_reg_setbits(REG_PMU_RXLDO_1P0, 13, 2, 0x0);    // 0x0 -> 0x40003134 bit 14:13 (2 bits)
}

void pm_port_open_wifi_sub(void)
{
    // wifi 开启
    uapi_reg_setbits(REG_CLDO_SUB_CRG_CKEN_CTL1, 13, 2, 0x3);   // 0x3 -> 0x44001104 bit 14:13 (2 bits)
    uapi_reg_setbits(REG_CLDO_SUB_CRG_CKEN_CTL1, 29, 1, 0x1);   // 0x1 -> 0x44001104 bit 29
    uapi_reg_setbits(REG_CLDO_CRG_RST_SOFT_CFG1, 4, 1, 0x1);    // 0x1 -> 0x4400113C bit 4
    uapi_tcxo_delay_us(WIFI_BLE_HW_PROCESS_DELAY_US);
}

void pm_port_close_wifi_sub(void)
{
    // wifi 关闭
    uapi_reg_setbits(REG_CLDO_CRG_RST_SOFT_CFG1, 4, 1, 0x0);    // 0x0 -> 0x4400113C bit 4
    uapi_reg_setbits(REG_CLDO_SUB_CRG_CKEN_CTL1, 13, 2, 0x0);   // 0x0 -> 0x44001104 bit 14:13 (2 bits)
    uapi_reg_setbits(REG_CLDO_SUB_CRG_CKEN_CTL1, 29, 1, 0x0);   // 0x0 -> 0x44001104 bit 29
    uapi_tcxo_delay_us(WIFI_BLE_HW_PROCESS_DELAY_US);
}

void pm_port_open_bt_sub(void)
{
    // BT 开启
    uapi_reg_setbits(REG_CLDO_SUB_CRG_CKEN_CTL1, 8, 5, 0x1F);   // 0x1F -> 0x44001104 bit 12:8 (5 bits)
    uapi_reg_setbits(REG_CLDO_CRG_RST_SOFT_CFG1, 1, 3, 0x7);    // 0x7 -> 0x4400113C bit 3:1 (3 bits)
    uapi_tcxo_delay_us(WIFI_BLE_HW_PROCESS_DELAY_US);
}

void pm_port_close_bt_sub(void)
{
    // BT 关闭
    uapi_reg_setbits(REG_CLDO_CRG_RST_SOFT_CFG1, 1, 3, 0x0);    // 0x0 -> 0x4400113C bit 3:1 (3 bits)
    uapi_reg_setbits(REG_CLDO_SUB_CRG_CKEN_CTL1, 8, 5, 0x0);    // 0x0 -> 0x44001104 bit 12:8 (5 bits)
    uapi_tcxo_delay_us(WIFI_BLE_HW_PROCESS_DELAY_US);
}

void pm_port_open_wifi_hw(void)
{
    uint32_t irq_status;
    irq_status = osal_irq_lock();
    if (g_pm_is_wifi_hw_open) {
        osal_irq_restore(irq_status);
        return;
    }

    // wifi ble 首次打开时开启RF
    if (!(g_pm_is_wifi_hw_open || g_pm_is_bt_hw_open)) {
        pm_port_open_rf();
    }

    pm_port_open_wifi_sub();

    g_pm_is_wifi_hw_open = true;
    osal_irq_restore(irq_status);
    return;
}

void pm_port_close_wifi_hw(void)
{
    uint32_t irq_status;
    irq_status = osal_irq_lock();
    if (!g_pm_is_wifi_hw_open) {
        osal_irq_restore(irq_status);
        return;
    }

    pm_port_close_wifi_sub();
    g_pm_is_wifi_hw_open = false;

    // wifi ble 全部关闭时关闭RF
    if (!(g_pm_is_wifi_hw_open || g_pm_is_bt_hw_open)) {
        pm_port_close_rf();
    }

    osal_irq_restore(irq_status);
    return;
}

void pm_port_open_bt_hw(void)
{
    uint32_t irq_status;
    irq_status = osal_irq_lock();
    if (g_pm_is_bt_hw_open) {
        osal_irq_restore(irq_status);
        return;
    }

    // wifi ble 首次打开时开启RF
    if (!(g_pm_is_wifi_hw_open || g_pm_is_bt_hw_open)) {
        pm_port_open_rf();
    }

    pm_port_open_bt_sub();

    g_pm_is_bt_hw_open = true;
    osal_irq_restore(irq_status);
    return;
}

void pm_port_close_bt_hw(void)
{
    uint32_t irq_status;
    irq_status = osal_irq_lock();
    if (!g_pm_is_bt_hw_open) {
        osal_irq_restore(irq_status);
        return;
    }

    pm_port_close_bt_sub();
    g_pm_is_bt_hw_open = false;

    // wifi ble 全部关闭时关闭RF
    if (!(g_pm_is_wifi_hw_open || g_pm_is_bt_hw_open)) {
        pm_port_close_rf();
    }

    osal_irq_restore(irq_status);
    return;
}

#define PM_FLASH_SEM_INIT_CNT 1
osal_semaphore *g_flash_sem = NULL;

int pm_port_flash_sem_down(void)
{
    if (g_flash_sem == NULL) {
        g_flash_sem = (osal_semaphore *)osal_kmalloc(sizeof(osal_semaphore), 0);
        if (g_flash_sem == OSAL_NULL) {
            return OSAL_FAILURE;
        }
        if (osal_sem_binary_sem_init(g_flash_sem, PM_FLASH_SEM_INIT_CNT) != OSAL_SUCCESS) {
            osal_kfree(g_flash_sem);
            g_flash_sem = NULL;
            return OSAL_FAILURE;
        }
    }
    return osal_sem_down(g_flash_sem);
}

void pm_port_flash_sem_up(void)
{
    if (g_flash_sem == NULL) {
        return;
    }
    osal_sem_up(g_flash_sem);
}

#define PM_DMA_SEM_INIT_CNT 1
osal_semaphore *g_dma_sem = NULL;
int pm_port_dma_sem_down(void)
{
    if (g_dma_sem == NULL) {
        g_dma_sem = (osal_semaphore *)osal_kmalloc(sizeof(osal_semaphore), 0);
        if (g_dma_sem == OSAL_NULL) {
            return OSAL_FAILURE;
        }
        if (osal_sem_binary_sem_init(g_dma_sem, PM_DMA_SEM_INIT_CNT) != OSAL_SUCCESS) {
            osal_kfree(g_dma_sem);
            g_dma_sem = NULL;
            return OSAL_FAILURE;
        }
    }
    return osal_sem_down(g_dma_sem);
}

void pm_port_dma_sem_up(void)
{
    if (g_dma_sem == NULL) {
        return;
    }
    osal_sem_up(g_dma_sem);
}

void pm_port_pm_hook_cb_register(uint32_t hook, pm_hook_cb_t cb)
{
    if (hook >= PM_HOOK_MAX_NUM) {
        return;
    }
    g_pm_hook_cb_list[hook] = cb;
}


void pm_port_enter_lowpower_prepare_cb_default(void)
{
#if !defined(TEST_SUITE)
    // 业务退出
#ifndef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    uapi_radar_set_status(0);
    wifi_deinit();
    bt_thread_close_btc();
#endif
    // 看门狗恢复
    g_pm_wdt_timeout = hal_watchdog_get_funcs()->get_attr();
    g_pm_wdt_mode = (hal_watchdog_wdt_cr_get_wdt_mode() == 0) ? WDT_MODE_RESET : WDT_MODE_INTERRUPT;
    // testsuit版本未启动看门狗
    uapi_watchdog_disable();

    uapi_watchdog_deinit();
#endif
    // 外设驱动去初始化
    uapi_drv_cipher_env_suspend();
    uapi_tsensor_deinit();
    uapi_gpio_deinit();
    uapi_pin_deinit();
    uapi_efuse_deinit();

#ifndef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    pm_port_flash_sem_down();
#endif
    // UART休眠, 并记录时钟
    uapi_uart_suspend(0);
    g_pm_uart_clock_record[UART_BUS_0] = uart_port_get_clock_value(UART_BUS_0);
    g_pm_uart_clock_record[UART_BUS_1] = uart_port_get_clock_value(UART_BUS_1);
    g_pm_uart_clock_record[UART_BUS_2] = uart_port_get_clock_value(UART_BUS_2);

    // dma休眠
    uapi_dma_suspend(0);
}

#define UART2_RXD_SEL 0x4400d01c
#define UART_2_MODE   2
#define UART1_RXD_SEL 0x4400d040
#define UART_1_MODE   1
#define UART0_RXD_SEL 0x4400d048
#define UART_0_MODE   1
void pm_port_enter_lowpower_finish_cb_default(void)
{
    uint32_t index;

    // dma恢复
    uapi_dma_resume(0);

    // UART重设时钟并恢复
    if (get_tcxo_freq() == CLK24M_TCXO) {
        uart_port_set_clock_value(UART_BUS_0, TCXO_CLOCK_24M);
        uart_port_set_clock_value(UART_BUS_1, TCXO_CLOCK_24M);
        uart_port_set_clock_value(UART_BUS_2, TCXO_CLOCK_24M);
    } else {
        uart_port_set_clock_value(UART_BUS_0, TCXO_CLOCK_40M);
        uart_port_set_clock_value(UART_BUS_1, TCXO_CLOCK_40M);
        uart_port_set_clock_value(UART_BUS_2, TCXO_CLOCK_40M);
    }
    uapi_uart_resume(0);

    // UART0/2 关闭中断
    for (index = UART_BUS_0; index < UART_BUS_MAX_NUMBER; index++) {
        if (index == WKUP_UART) {
            continue;
        }
        osal_irq_disable(UART_0_IRQN + index - UART_BUS_0);
        if (index == UART_BUS_2) {
            writel(UART2_RXD_SEL, 0x0);
        } else if (index == UART_BUS_1) {
            writel(UART1_RXD_SEL, 0x0);
        } else if (index == UART_BUS_0) {
            writel(UART0_RXD_SEL, 0x0);
        }
    }
}

void pm_port_exit_lowpower_prepare_cb_default(void)
{
    uint32_t index;

    for (index = UART_BUS_0; index < UART_BUS_MAX_NUMBER; index++) {
        if (index == WKUP_UART) {
            continue;
        }
        if (index == UART_BUS_2) {
            writel(UART2_RXD_SEL, UART_2_MODE);
        } else if (index == UART_BUS_1) {
            writel(UART1_RXD_SEL, UART_1_MODE);
        } else if (index == UART_BUS_0) {
            writel(UART0_RXD_SEL, UART_0_MODE);
        }
        osal_irq_enable(UART_0_IRQN + index - UART_BUS_0);
    }

    // UART休眠
    uapi_uart_suspend(0);

    // dma休眠
    uapi_dma_suspend(0);
}

void pm_port_exit_lowpower_finish_cb_default(void)
{
    // dma恢复
    uapi_dma_resume(0);

    // UART重设时钟并恢复
    uart_port_set_clock_value(UART_BUS_0, g_pm_uart_clock_record[UART_BUS_0]);
    uart_port_set_clock_value(UART_BUS_1, g_pm_uart_clock_record[UART_BUS_1]);
    uart_port_set_clock_value(UART_BUS_2, g_pm_uart_clock_record[UART_BUS_2]);
    uapi_uart_resume(0);

    // 外设驱动恢复
    // flash
#ifndef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    pm_port_flash_sem_up();
#endif
    // pinctrl & gpio
    uapi_pin_init();
    uapi_gpio_init();

    // efuse
    check_efuse_boot_done();
    set_efuse_period();
    set_efuse_clock_period();
    uapi_efuse_init();

    // tsensor
    uapi_tsensor_init();
    // 默认采样周期为 0xFFFF 个32K时钟
    uapi_tsensor_start_inquire_mode(TSENSOR_SAMP_MODE_AVERAGE_CYCLE, 0xFFFF);

    // 安全驱动恢复
    uapi_drv_cipher_env_resume();

#if !defined(TEST_SUITE)
    // watchdog恢复
    uapi_watchdog_init(WDT_TIMEOUT_S);
    hal_watchdog_get_funcs()->set_attr(g_pm_wdt_timeout);
    uapi_watchdog_enable(g_pm_wdt_mode);
    // 业务恢复
    // 设置wifi校准标志为false
    cali_set_cali_done_flag(OSAL_FALSE);
#ifndef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    wifi_init();
    bt_thread_open_btc();
#endif
#endif
}

static bool cmu_is_fnpll_locked(void)
{
    uint32_t i;

    for (i = 0; i < FNPLL_LOCK_QUERY_TIMES; i++) {
        // 查询 CMU_FNPLL_LOCK_GRM (bit 12) 为 0x1
        if (uapi_reg_getbits(REG_EXCEP_RO_RG, 12, 1) == 0x1) {
            return true;
        }
    }

    return false;
}

static uint32_t check_cmu_lock_status(void)
{
    uint32_t retry_times;

    for (retry_times = 0; retry_times < FNPLL_LOCK_RETRY_TIMES; retry_times++) {
        // enable pd
        uapi_reg_setbits(REG_CMU_FNPLL_SIG, 15, 1, 0x0);     // 0x0 -> 0x4000342C bit 15
        uapi_tcxo_delay_ms(FNPLL_LOCK_QUERY_DELAY_MS);
        if (cmu_is_fnpll_locked()) {
            return PLL_LOCKED;
        }
        // disable pd
        uapi_reg_setbits(REG_CMU_FNPLL_SIG, 15, 1, 0x1);     // 0x1 -> 0x4000342C bit 15
        uapi_tcxo_delay_us(FNPLL_LOCK_RETRY_DELAY_US);
    }

    return PLL_UNLOCKED;
}

static void pm_port_wfi_process(void)
{
    LOS_TaskLock();

    // timer0-liteOS时钟 屏蔽中断
    uapi_reg_setbit(0x44002110, 3); // 0x1 -> 0x44002110 bit 3
    osal_irq_clear(TIMER_0_IRQN);
    dsb();
    wfi();
    isb();
    nop();
    nop();
    nop();
    nop();

    // timer0-liteOS时钟 恢复中断
    uapi_reg_clrbit(0x44002110, 3); // 0x0 -> 0x44002110 bit 3

    LOS_TaskUnlock();
}

void pm_port_enter_lowpower(void)
{
    uint32_t irq_status, index, reg_addr, reg_val;

    PRINT("Enter low-power start.\r\n");
    uapi_tcxo_delay_ms(UART_PRINT_DELAY_MS);
#ifdef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    irq_status = osal_irq_lock();
    g_pm_srv_ctrl.ctrl_protect = PM_CTRL_SUSPEND;
    if (g_pm_srv_ctrl.ctrl_mode == PM_CTRL_SUSPEND) {
        PRINT("Already in low-power state, enter process return.\r\n");
        pm_port_wfi_process();
        osal_irq_restore(irq_status);
        return;
    }
    if (g_pm_srv_ctrl.srv_mode[PM_SRV_PLAT] == PM_MODE_RESUME) {
        osal_irq_restore(irq_status);
        PRINT("pm service not suspend.\r\n");
        pm_print_srv_stats();
        return;
    }
    osal_irq_restore(irq_status);
    pm_port_flash_sem_down();
    pm_port_dma_sem_down();
    irq_status = osal_irq_lock();
    /* 如果等待期间客户要求退出低功耗，则平台不进低功耗 */
    if (g_pm_srv_ctrl.ctrl_protect != PM_CTRL_SUSPEND) {
        osal_irq_restore(irq_status);
        pm_port_flash_sem_up();
        pm_port_dma_sem_up();
        PRINT("Return before entering low-power.\r\n");
        return;
    }
    g_pm_srv_ctrl.ctrl_mode = PM_CTRL_SUSPEND;
#else
    /* 1.低功耗状态判断 */
    irq_status = osal_irq_lock();
    if (g_pm_is_in_process) {
        osal_irq_restore(irq_status);
        PRINT("Low-power processing, enter process return.\r\n");
        return;
    }
    if (g_pm_is_in_lowpower) {
        osal_irq_restore(irq_status);
        PRINT("Already in low-power state, enter process return.\r\n");
        pm_port_wfi_process();
        return;
    }
    g_pm_is_in_process = true;
    osal_irq_restore(irq_status);
#endif

    /* 2.调用 enter_lowpower_prepare 回调 */
    if (g_pm_hook_cb_list[PM_HOOK_ENTER_LOWPOWER_PREPARE] != NULL) {
        g_pm_hook_cb_list[PM_HOOK_ENTER_LOWPOWER_PREPARE]();
    }

#ifndef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    irq_status = osal_irq_lock();
#endif
    /* 3.WDT关闭 */
    uapi_reg_clrbit(REG_AON_CRG_CKEN_CTL, 4); // 0x0 -> 0x40002100 bit 4
    uapi_reg_clrbit(REG_AON_SOFT_RST_CTL, 1); // 0x0 -> 0x40002110 bit 1

    /* 4.CLDO 各模块 复位并关闭时钟 */
    // BIT[30/29/28/27 /24/23/22/21/20/19/18/17 /15/14/13/12/11/10/9/8/7/6/5/4/3]配置成0x0
    uapi_reg_read32(REG_CLDO_CRG_RST_SOFT_CFG0, reg_val);
    g_pm_crg_record[PM_CRG_RST_CTL0] = reg_val;
    uapi_reg_setbits(REG_CLDO_CRG_RST_SOFT_CFG0, 3, 13, 0x0);   // 0x0 -> 0x44001138 bit 15:3 (13 bits)
    uapi_reg_setbits(REG_CLDO_CRG_RST_SOFT_CFG0, 17, 8, 0x0);   // 0x0 -> 0x44001138 bit 24:17 (8 bits)
    uapi_reg_setbits(REG_CLDO_CRG_RST_SOFT_CFG0, 27, 2, 0x0);   // 0x0 -> 0x44001138 bit 28:27 (2 bits)
    uapi_reg_setbits(REG_CLDO_CRG_RST_SOFT_CFG0, 30, 1, 0x0);   // 0x0 -> 0x44001138 bit 30

    // BIT[7, 5, 0]配置成0x0
    uapi_reg_read32(REG_CLDO_CRG_RST_SOFT_CFG1, reg_val);
    g_pm_crg_record[PM_CRG_RST_CTL1] = reg_val;
    uapi_reg_setbits(REG_CLDO_CRG_RST_SOFT_CFG1, 0, 1, 0x0);    // 0x0 -> 0x4400113C bit 0
    uapi_reg_setbits(REG_CLDO_CRG_RST_SOFT_CFG1, 5, 1, 0x0);    // 0x0 -> 0x4400113C bit 5
    uapi_reg_setbits(REG_CLDO_CRG_RST_SOFT_CFG1, 7, 1, 0x0);    // 0x0 -> 0x4400113C bit 7

    // [27/26/25/24/23/22/21/20/19/18 /16/15/14/13/12/11/10/9/8/7/6/5/4/3/2] 配置成0x0
    uapi_reg_read32(REG_CLDO_SUB_CRG_CKEN_CTL0, reg_val);
    g_pm_crg_record[PM_CRG_CKEN_CTL0] = reg_val;
    uapi_reg_setbits(REG_CLDO_SUB_CRG_CKEN_CTL0, 18, 10, 0x0);  // 0x0 -> 0x44001100 bit 27:18 (10 bits)
    uapi_reg_setbits(REG_CLDO_SUB_CRG_CKEN_CTL0, 2, 15, 0x0);   // 0x0 -> 0x44001100 bit 16:2 (15 bits)

    // BIT[28/27/26/25/24/23/22 /17/16 /7/6 /4/3/2/1/0] 配置成0x0
    uapi_reg_read32(REG_CLDO_SUB_CRG_CKEN_CTL1, reg_val);
    g_pm_crg_record[PM_CRG_CKEN_CTL1] = reg_val;
    uapi_reg_setbits(REG_CLDO_SUB_CRG_CKEN_CTL1, 0, 5, 0x0);    // 0x0 -> 0x44001104 bit 4:0 (5 bits)
    uapi_reg_setbits(REG_CLDO_SUB_CRG_CKEN_CTL1, 6, 2, 0x0);    // 0x0 -> 0x44001104 bit 7:6 (2 bits)
    uapi_reg_setbits(REG_CLDO_SUB_CRG_CKEN_CTL1, 16, 2, 0x0);   // 0x0 -> 0x44001104 bit 17:16 (2 bits)
    uapi_reg_setbits(REG_CLDO_SUB_CRG_CKEN_CTL1, 22, 7, 0x0);   // 0x0 -> 0x44001104 bit 28:22 (7 bits)

    /* 6.Flash/UART/总线时钟切换到TCXO */
    uapi_reg_clrbit(REG_CLDO_CRG_CLK_SEL, 18);  // 0x0 -> 0x44001134 bit 18
    uapi_reg_clrbit(REG_CLDO_CRG_CLK_SEL, 1);   // 0x0 -> 0x44001134 bit 1
    uapi_reg_clrbit(REG_CLDO_CRG_CLK_SEL, 2);   // 0x0 -> 0x44001134 bit 2
    uapi_reg_clrbit(REG_CLDO_CRG_CLK_SEL, 3);   // 0x0 -> 0x44001134 bit 2

    // wifi phy 时钟切换
    uapi_reg_clrbit(REG_CLDO_CRG_CLK_SEL, 19);   // 0x0 -> 0x44001134 bit 19
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);

    if (get_tcxo_freq() == CLK24M_TCXO) {
        // 总线时钟降频, 24M -> 4M
        uapi_reg_clrbit(REG_CLDO_CRG_DIV_CTL0, 8);          // 0x0 -> 0x44001108 bit 8
        uapi_reg_setbits(REG_CLDO_CRG_DIV_CTL0, 4, 4, 0x6); // 0x6 -> 0x44001108 bit 7:4 (4 bits)
        uapi_reg_setbit(REG_CLDO_CRG_DIV_CTL0, 8);          // 0x0 -> 0x44001108 bit 8
    } else {
        // 总线时钟降频, 40M -> 4M
        uapi_reg_clrbit(REG_CLDO_CRG_DIV_CTL0, 8);          // 0x0 -> 0x44001108 bit 8
        uapi_reg_setbits(REG_CLDO_CRG_DIV_CTL0, 4, 4, 0xA); // 0xA -> 0x44001108 bit 7:4 (4 bits)
        uapi_reg_setbit(REG_CLDO_CRG_DIV_CTL0, 8);          // 0x0 -> 0x44001108 bit 8
    }
    uapi_reg_clrbit(REG_CLDO_CRG_CLK_SEL, 17);  // 0x0 -> 0x44001134 bit 17

    /* 7.FNPLL时钟关断 */
    uapi_reg_setbit(REG_CMU_FNPLL_SIG, 14);     // 0x1 -> 0x4000342C bit 14
    uapi_reg_setbit(REG_CMU_FNPLL_SIG, 15);     // 0x1 -> 0x4000342C bit 15

    /* 8.FNPLL电源关闭 */
    uapi_reg_setbits(REG_PMU_PHY_LDO, 13, 3, 0x4);      // 0x4 -> 0x4000316C bit 15:13 (3 bits)
    uapi_reg_setbits(REG_PMU_CMULDO1_0P9, 13, 3, 0x4);  // 0x4 -> 0x4000312C bit 15:13 (3 bits)
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);

    /* 9.备份并复位IOCFG配置(除UART和SFC), 降低VDDIO功耗 */
    for (index = 0; index < IOCFG_RECORD_NUM; index++) {
        // SEL配置
        reg_addr = REG_GPIO_SEL_START_ADDR + (REG_ADDR_INTERVAL * index);
        uapi_reg_read32(reg_addr, reg_val);
        g_pm_iocfg_sel_record[index] = reg_val;
        uapi_reg_write32(reg_addr, VAL_GPIO_SEL_DEFAULT);

        // PAD配置
        reg_addr = REG_GPIO_PAD_START_ADDR + (REG_ADDR_INTERVAL * index);
        uapi_reg_read32(reg_addr, reg_val);
        g_pm_iocfg_pad_record[index] = reg_val;
        uapi_reg_write32(reg_addr, VAL_GPIO_PAD_DEFAULT);
    }

    /* 10.调用 enter_lowpower_finish 回调 */
    if (g_pm_hook_cb_list[PM_HOOK_ENTER_LOWPOWER_FINISH] != NULL) {
        g_pm_hook_cb_list[PM_HOOK_ENTER_LOWPOWER_FINISH]();
    }

#ifndef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    /* 11.低功耗状态刷新 */
    g_pm_is_in_process = false;
    g_pm_is_in_lowpower = true;
    osal_irq_restore(irq_status);
#endif
    PRINT("Enter low-power succ!\r\n");

    uapi_tcxo_delay_ms(PM_ENTER_LOWPOWER_WFI_DELAY_MS);

    pm_port_wfi_process();
#ifdef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    osal_irq_restore(irq_status);
#endif
    pm_port_dma_sem_up();
}

void pm_port_exit_lowpower(void)
{
    uint32_t irq_status, reg_addr, index;

    PRINT("Exit low-power start.\r\n");
    pm_port_dma_sem_down();
    /* 1.低功耗状态判断 */
    irq_status = osal_irq_lock();
#ifdef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    if (g_pm_srv_ctrl.ctrl_mode != PM_CTRL_SUSPEND) {
        pm_port_dma_sem_up();
        osal_irq_restore(irq_status);
        PRINT("Not in low-power state, exit process return.\r\n");
        return;
    }
    g_pm_srv_ctrl.ctrl_mode = PM_CTRL_RESUME;
    g_pm_srv_ctrl.ctrl_protect = PM_CTRL_RESUME;
#else
    if (g_pm_is_in_process) {
        osal_irq_restore(irq_status);
        PRINT("Low-power processing, exit process return.\r\n");
        return;
    }
    if (!g_pm_is_in_lowpower) {
        osal_irq_restore(irq_status);
        PRINT("Not in low-power state, exit process return.\r\n");
        return;
    }
    g_pm_is_in_process = true;
#endif
    /* 2.调用 exit_lowpower_prepare 回调 */
    if (g_pm_hook_cb_list[PM_HOOK_EXIT_LOWPOWER_PREPARE] != NULL) {
        g_pm_hook_cb_list[PM_HOOK_EXIT_LOWPOWER_PREPARE]();
    }

    /* 3.恢复IOCFG配置 */
    for (index = 0; index < IOCFG_RECORD_NUM; index++) {
        // SEL配置
        reg_addr = REG_GPIO_SEL_START_ADDR + (REG_ADDR_INTERVAL * index);
        uapi_reg_write32(reg_addr, g_pm_iocfg_sel_record[index]);

        // PAD配置
        reg_addr = REG_GPIO_PAD_START_ADDR + (REG_ADDR_INTERVAL * index);
        uapi_reg_write32(reg_addr, g_pm_iocfg_pad_record[index]);
    }

    /* 4.FNPLL 电源恢复 */
    // 打开PMU_CMU_LDO电源
    uapi_reg_setbits(REG_PMU_CMULDO1_0P9, 15, 1, 0x1);  // 0x4 -> 0x4000312C bit 15
    uapi_reg_setbits(REG_PMU_CMULDO1_0P9, 14, 1, 0x1);  // 0x4 -> 0x4000312C bit 14
    uapi_tcxo_delay_us(PMU_RF_OPEN_DELAY_US);
    uapi_reg_setbits(REG_PMU_CMULDO1_0P9, 13, 1, 0x1);  // 0x4 -> 0x4000312C bit 13
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);

    // 打开PMU_PHY_LDO电源
    uapi_reg_setbits(REG_PMU_PHY_LDO, 15, 1, 0x1);  // 0x4 -> 0x4000316C bit 15
    uapi_reg_setbits(REG_PMU_PHY_LDO, 14, 1, 0x1);  // 0x4 -> 0x4000316C bit 14
    uapi_tcxo_delay_us(PMU_RF_OPEN_DELAY_US);
    uapi_reg_setbits(REG_PMU_PHY_LDO, 13, 1, 0x1);  // 0x4 -> 0x4000316C bit 13
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);

    /* 5.时钟初始化与恢复 */
    // 晶振时钟初始化
    if (get_tcxo_freq() == CLK24M_TCXO) {
        // 24M晶振
        uapi_reg_setbits(REG_CMU_FNPLL_SIG, 6, 6, 0x1);     // 0x1 -> 0x4000342C bit 6
        uapi_reg_setbits(REG_CMU_FBDIV, 0, 12, 0x78);       // 0x78 -> 0x40003430 bit 12:0 (13 bits)
        uapi_reg_setbits(REG_CMU_FRAC, 0, 24, 0x0);         // 0x0 -> 0x40003434 bit 24:0 (25 bits)
        uapi_reg_setbits(REG_CMU_FNPLL_SIG, 3, 3, 0x2);     // 0x2 -> 0x4000342C bit 5:3 (3 bits)
        uapi_reg_setbits(REG_CMU_FNPLL_SIG, 0, 3, 0x0);     // 0x0 -> 0x4000342C bit 2:0 (3 bits)
        uapi_reg_setbits(REG_CMU_FNPLL_SIG, 17, 1, 0x1);    // 0x1 -> 0x4000342C bit 17
        uapi_reg_write32(REG_FNPLL_CFG0, 0xE0000004);       // 0xE0000004 -> 0x4000340C
        uapi_reg_write32(REG_FNPLL_CFG1, 0x00B40000);       // 0x00B40000 -> 0x40003410
        uapi_reg_write32(REG_FNPLL_CFG2, 0x20111FA8);       // 0x20111FA8 -> 0x40003414
        uapi_reg_write32(REG_FNPLL_CFG3, 0x2404FF20);       // 0x2404FF20 -> 0x40003418
        uapi_reg_write32(REG_FNPLL_CFG4, 0x0802013F);       // 0x0802013F -> 0x4000341C
        uapi_reg_write32(REG_FNPLL_CFG5, 0x00004046);       // 0x00004046 -> 0x40003420
    } else {
        // 40M晶振
        uapi_reg_setbits(REG_CMU_FNPLL_SIG, 6, 6, 0x1);     // 0x1 -> 0x4000342C bit 6
        uapi_reg_setbits(REG_CMU_FBDIV, 0, 12, 0x48);       // 0x48 -> 0x40003430 bit 12:0 (13 bits)
        uapi_reg_setbits(REG_CMU_FRAC, 0, 24, 0x0);         // 0x0 -> 0x40003434 bit 24:0 (25 bits)
        uapi_reg_setbits(REG_CMU_FNPLL_SIG, 3, 3, 0x2);     // 0x2 -> 0x4000342C bit 5:3 (3 bits)
        uapi_reg_setbits(REG_CMU_FNPLL_SIG, 0, 3, 0x0);     // 0x0 -> 0x4000342C bit 2:0 (3 bits)
        uapi_reg_setbits(REG_CMU_FNPLL_SIG, 17, 1, 0x1);    // 0x1 -> 0x4000342C bit 17
        uapi_reg_write32(REG_FNPLL_CFG0, 0x84000004);       // 0x84000004 -> 0x4000340C
        uapi_reg_write32(REG_FNPLL_CFG1, 0x00B40000);       // 0x00B40000 -> 0x40003410
        uapi_reg_write32(REG_FNPLL_CFG2, 0x20111FA8);       // 0x20111FA8 -> 0x40003414
        uapi_reg_write32(REG_FNPLL_CFG3, 0x2404FF20);       // 0x2404FF20 -> 0x40003418
        uapi_reg_write32(REG_FNPLL_CFG4, 0x0802013F);       // 0x0802013F -> 0x4000341C
        uapi_reg_write32(REG_FNPLL_CFG5, 0x00004046);       // 0x00004046 -> 0x40003420
    }
    uapi_tcxo_delay_us(1);

    // 尝试PLL锁定, 若失败则恢复uart进行打印, 然后复位
    if (check_cmu_lock_status() != PLL_LOCKED) {
        // PLL时钟初始化失败, 恢复串口
        if (get_tcxo_freq() == CLK24M_TCXO) {
            uart_port_set_clock_value(UART_BUS_0, TCXO_CLOCK_24M);
            uart_port_set_clock_value(UART_BUS_1, TCXO_CLOCK_24M);
            uart_port_set_clock_value(UART_BUS_2, TCXO_CLOCK_24M);
        } else {
            uart_port_set_clock_value(UART_BUS_0, TCXO_CLOCK_40M);
            uart_port_set_clock_value(UART_BUS_1, TCXO_CLOCK_40M);
            uart_port_set_clock_value(UART_BUS_2, TCXO_CLOCK_40M);
        }
        uapi_uart_resume(0);

        // PLL初始化失败, 打印 10 次后复位
        for (index = 10; index > 0; index--) {
            PRINT("PLL init fail, reboot in %us!\r\n", index);
            uapi_tcxo_delay_ms(1000); // delay 1000ms
        }
        reboot_port_reboot_chip();
    }

    uapi_reg_setbits(REG_CMU_FNPLL_SIG, 14, 1, 0x0);        // 0x0 -> 0x4000342C bit 14
    uapi_reg_setbits(REG_CMU_CLK_480M_WDBB, 0, 1, 0x1);     // 0x1 -> 0x4000344C bit 0
    uapi_tcxo_delay_us(1);
    uapi_reg_setbits(REG_CMU_CLK_480M_WDBB, 1, 1, 0x1);     // 0x1 -> 0x4000344C bit 1
    uapi_tcxo_delay_us(1);
    uapi_reg_setbits(REG_CLDO_CRG_CLK_SEL, 17, 1, 0x1);     // 0x1 -> 0x44001134 bit 17
    uapi_reg_setbits(REG_CLDO_CRG_DIV_CTL7, 24, 1, 0x0);    // 0x0 -> 0x44001124 bit 24
    uapi_reg_setbits(REG_CLDO_CRG_DIV_CTL7, 22, 2, 0x2);    // 0x2 -> 0x44001124 bit 23:22 (2 bits)
    uapi_reg_setbits(REG_CLDO_CRG_DIV_CTL7, 24, 1, 0x1);    // 0x1 -> 0x44001124 bit 24

    uapi_reg_clrbit(REG_CLDO_CRG_DIV_CTL0, 8);              // 0x0 -> 0x44001108 bit 8
    uapi_reg_setbits(REG_CLDO_CRG_DIV_CTL0, 4, 4, 0x1);     // 0x1 -> 0x44001108 bit 7:4 (4 bits)
    uapi_reg_setbit(REG_CLDO_CRG_DIV_CTL0, 8);              // 0x0 -> 0x44001108 bit 8

    // wifi phy 时钟切换
    uapi_reg_setbit(REG_CLDO_CRG_CLK_SEL, 19);   // 0x0 -> 0x44001134 bit 19
    uapi_tcxo_delay_us(PMU_PROCESS_DELAY_US);

    /* 6.Flash/UART时钟切换 */
    uapi_reg_setbit(REG_CLDO_CRG_CLK_SEL, 1);   // 0x1 -> 0x44001134 bit 1
    uapi_reg_setbit(REG_CLDO_CRG_CLK_SEL, 2);   // 0x1 -> 0x44001134 bit 2
    uapi_reg_setbit(REG_CLDO_CRG_CLK_SEL, 3);   // 0x1 -> 0x44001134 bit 2
    uapi_reg_setbit(REG_CLDO_CRG_CLK_SEL, 18);  // 0x1 -> 0x44001134 bit 18

    /* 8.CLDO 各设备恢复时钟并解复位 */
    uapi_reg_write32(REG_CLDO_SUB_CRG_CKEN_CTL1, g_pm_crg_record[PM_CRG_CKEN_CTL1]);
    uapi_reg_write32(REG_CLDO_SUB_CRG_CKEN_CTL0, g_pm_crg_record[PM_CRG_CKEN_CTL0]);
    uapi_reg_write32(REG_CLDO_CRG_RST_SOFT_CFG1, g_pm_crg_record[PM_CRG_RST_CTL1]);
    uapi_reg_write32(REG_CLDO_CRG_RST_SOFT_CFG0, g_pm_crg_record[PM_CRG_RST_CTL0]);

    /* 9.WDT门控开启, 并停止使能 */
    uapi_reg_setbit(REG_AON_SOFT_RST_CTL, 1);   // 0x1 -> 0x40002110 bit 1
    uapi_reg_setbit(REG_AON_CRG_CKEN_CTL, 4);   // 0x1 -> 0x40002100 bit 4
    uapi_reg_clrbit(REG_WDT_CR, 0);             // 0x0 -> 0x40006010 bit 0

#ifndef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    osal_irq_restore(irq_status);
#endif
    /* 10.调用 exit_lowpower_finish 回调 */
    if (g_pm_hook_cb_list[PM_HOOK_EXIT_LOWPOWER_FINISH] != NULL) {
        g_pm_hook_cb_list[PM_HOOK_EXIT_LOWPOWER_FINISH]();
    }

    /* 11.低功耗状态刷新 */
#ifndef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    irq_status = osal_irq_lock();
    g_pm_is_in_process = false;
    g_pm_is_in_lowpower = false;
#else
    osal_irq_restore(irq_status);
    pm_port_flash_sem_up();
#endif
    pm_port_dma_sem_up();
    PRINT("Exit low-power succ!\r\n");
}

void pm_porting_wait_exit_lowpower(void)
{
    PRINT("[pm_port][0x%x][%ld]Wait start.\r\n", __builtin_return_address(0), osal_get_current_tid());
#ifdef CONFIG_PM_SUPPORT_SRV_DECOUPLING
    while (g_pm_srv_ctrl.ctrl_mode == PM_CTRL_SUSPEND) {
#else
    while (g_pm_is_in_lowpower) {
#endif
        osal_msleep(PM_WAIT_EXIT_LOWPOWER_MS);
    }
    PRINT("[pm_port][0x%x][%ld]Wait succ!\r\n", __builtin_return_address(0), osal_get_current_tid());
}
