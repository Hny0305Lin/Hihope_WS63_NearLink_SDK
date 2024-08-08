/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2015-2023. All rights reserved.
 * Description: plat pm head file.
 * Create: 2015-04-10
 */

#ifndef __PLAT_PM_H__
#define __PLAT_PM_H__

/*****************************************************************************
  1 Include other Head file
*****************************************************************************/
#include <linux/mutex.h>
#include <linux/kernel.h>
#include <linux/version.h>
#if defined(_PRE_OS_VERSION) && defined(_PRE_OS_VERSION_LINUX) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
#include <linux/pm_wakeup.h>
#endif
#endif
#include "plat_pm_wlan.h"
/*****************************************************************************
  2 Define macro
*****************************************************************************/
#define BT_LOCK_NAME                   "bt_wake_lock"

#define FIRMWARE_CFG_INIT_OK            0x01

#define SUCCESS                         (0)
#define FAILURE                         (1)

#define GNSS_AGREE_SLEEP                (1)
#define GNSS_NOT_AGREE_SLEEP            (0)

#define BT_TRY_SLEEP         0
#define BT_SLEEP             1
#define BT_WKUPING           2
#define BT_ACTIVE            3

#define HOST_DISALLOW_TO_SLEEP          (0)
#define HOST_ALLOW_TO_SLEEP             (1)
#define HOST_WKING_FROM_SLEEP           (2)

#define BT_PM_ENABLE                  1
#define BT_PM_DISABLE                 0

#define WAIT_DEVACK_MSEC               (10)
#define WAIT_DEVACK_CNT               (10)
#define WAIT_DEVACK_TIMEOUT_MSEC       (2000)

/* 超时时间要大于wkup dev work中的最长执行时间，否则超时以后进入DFR和work中会同时操作tty，导致冲突 */
#define WAIT_WKUPDEV_MSEC              (10000)

#define WAIT_BT_SLP_ACK_MSEC     1000
#define WKUP_BT_RETRY_TIMES      3
#define WAIT_BT_PMENABLE_ACK_MSEC     1000

#define NEED_WAIT_NEXT_GPIO   0x55
#define WAIT_GPIO_INTR_DONE   0x5a

enum uart_state_enum {
    UART_NOT_READY              = 0,
    UART_READY                  = 1,
    UART_BPS_CHG_SEND_ACK       = 2,
    UART_BPS_CHG_IN_PROGRESS    = 3,
    UART_BPS_CHG_SEND_COMPLETE  = 4,
};


/* BFGX系统上电加载异常类型 */
enum bt_power_on_exception_enum {
    BT_POWER_FAILED                               = -1,
    BT_POWER_SUCCESS                              = 0,

    BT_POWER_PULL_POWER_GPIO_FAIL                 = 1,
    BT_POWER_TTY_OPEN_FAIL                        = 2,
    BT_POWER_TTY_FLOW_ENABLE_FAIL                 = 3,

    BT_POWER_WIFI_DERESET_BCPU_FAIL               = 4,
    BT_POWER_WIFI_ON_BOOT_UP_FAIL                 = 5,

    BT_POWER_WIFI_OFF_BOOT_UP_FAIL                = 6,
    BT_POWER_DOWNLOAD_FIRMWARE_FAIL               = 7,

    BT_POWER_WAKEUP_FAIL                          = 8,
    BT_POWER_OPEN_CMD_FAIL                        = 9,

    BT_POWER_ENUM_BUTT,
};

/* wifi系统上电加载异常类型 */
enum wifi_power_on_exception_enum {
    WIFI_POWER_FAIL                                 = -1,
    WIFI_POWER_SUCCESS                              = 0,
    WIFI_POWER_PULL_POWER_GPIO_FAIL                 = 1,

    WIFI_POWER_BFGX_OFF_BOOT_UP_FAIL                = 2,
    WIFI_POWER_BFGX_OFF_FIRMWARE_DOWNLOAD_FAIL      = 3,

    WIFI_POWER_BFGX_ON_BOOT_UP_FAIL                 = 4,
    WIFI_POWER_BFGX_DERESET_WCPU_FAIL               = 5,
    WIFI_POWER_BFGX_ON_FIRMWARE_DOWNLOAD_FAIL       = 6,
    WIFI_POWER_REPROBLE_DEVICE_FAIL                 = 7,
    WIFI_POWER_ENUM_BUTT,
};

enum bt_pm_sleep_stage {
    BT_SLP_STAGE_INIT    = 0,  // 初始
    BT_SLP_REQ_SND       = 1,  // sleep request发送完成
    BT_SLP_ALLOW_RCV     = 2,  // 收到allow sleep response
    BT_SLP_DISALLOW_RCV  = 3,  // 收到allow sleep response
    BT_SLP_CMD_SND       = 4,  // 允许睡眠reg设置完成
};

/*****************************************************************************
  3 STRUCT DEFINE
*****************************************************************************/
typedef struct {
    osal_s32 (*bt_open_wcpu_cb)(osal_void);
    osal_s32 (*bcpu_is_depends_on_wcpu_cb)(osal_void);
    osal_s32 (*bt_is_shutdown_cb)(osal_void);
} pm_bcpu_ops;

/* private data for pm driver */
struct pm_drv_data {
    /* wlan interface pointer */
    struct wlan_pm_s                *wlan_pm_info;
    /* board customize info */
    board_info                      *board;
    pm_bcpu_ops *bcpu_ops;
    /* mutex for sync */
    struct mutex                    host_mutex;
    /* flag for firmware cfg file init */
    osal_ulong firmware_cfg_init_flag;
};

/*****************************************************************************
  4 EXTERN VARIABLE
*****************************************************************************/

/*****************************************************************************
  5 EXTERN FUNCTION
*****************************************************************************/
extern struct pm_drv_data *pm_get_drvdata(void);
extern osal_s32 host_wkup_bt_device(void);
extern void bfgx_print_subsys_state(void);
extern osal_s32 wlan_is_shutdown(void);
extern osal_s32 bt_is_shutdown(void);
extern osal_s32 wlan_power_on(void);
extern osal_s32 wlan_power_off(void);
extern void bt_wake_lock(void);
extern void bt_wake_unlock(void);

void bt_start_sleep_timer(void);
void bt_pm_set_packet_cnt(osal_u32 delt);
int bt_pf_pm_enable(void);
int bt_pf_pm_disable(void);
osal_s32 bcpu_is_depends_on_wcpu(void);
int bt_open_wcpu(void);

#define WKUP_BCPU_EVENT  0x1
#define OPEN_BCPU_EVENT  0x2
#define BCPU_WORK_EVENT  0x3
#define W2B_SHUTDOWN_EVENT 0x4
osal_slong pm_bcpu_notify_wcpu(osal_u32 events);
void host_pm_b2w_vote_work(osal_u8 is_work);
osal_s32 get_b2h_gpio_irq(osal_u32 *irq);
osal_s32 pm_bcpu_ops_register(pm_bcpu_ops *bcpu_ops);
osal_s32 bt_try_open_wcpu(osal_void);
osal_void pm_host_lock_mux(osal_void);
osal_void pm_host_unlock_mux(osal_void);

#endif

