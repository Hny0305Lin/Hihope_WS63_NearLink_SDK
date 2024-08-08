/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2015-2023. All rights reserved.
 * Description: plat pm head file.
 * Create: 2015-04-10
 */

#ifndef __PLAT_PM_WLAN_H__
#define __PLAT_PM_WLAN_H__

/*****************************************************************************
  1 Include other Head file
*****************************************************************************/
#if defined(_PRE_OS_VERSION) && defined(_PRE_OS_VERSION_LINUX) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/mutex.h>
#include <linux/kernel.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
#if defined(_PRE_OS_VERSION) && defined(_PRE_OS_VERSION_LINUX) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/pm_wakeup.h>
#endif
#endif
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio.h>

#include <linux/fb.h>
#endif
#include "hcc_bus.h"

#include "oal_ext_if.h"

#ifdef WIN32
#include "oal_plat_type.h"
#endif

#define WLAN_WAKUP_MSG_WAIT_TIMEOUT     (1000)
#define WLAN_SLEEP_MSG_WAIT_TIMEOUT     (10000)
#define WLAN_POWEROFF_ACK_WAIT_TIMEOUT  (1000)
#define WLAN_OPEN_BCPU_WAIT_TIMEOUT     (1000)
#define WLAN_HALT_BCPU_TIMEOUT          (1000)
#define WLAN_SLEEP_TIMER_PERIOD         (50)    /* 睡眠定时器50ms定时 */
#define WLAN_SLEEP_DEFAULT_CHECK_CNT    (6)     /* 默认100ms */
#define WLAN_SLEEP_LONG_CHECK_CNT       (8)    /* 20 入网阶段,延长至400ms */
#define WLAN_SLEEP_FAST_CHECK_CNT       (1)     /* fast sleep,20ms */
#define WLAN_WAKELOCK_HOLD_TIME         (500)   /* hold wakelock 500ms */

#define WLAN_SDIO_MSG_RETRY_NUM         (3)
#define WLAN_WAKEUP_FAIL_MAX_TIMES      (1)  /* 连续多少次wakeup失败，可进入DFR流程 */

#define HOST_WAIT_BOTTOM_WIFI_TIMEOUT   20000
#define HOST_WAIT_BOTTOM_WIFI_TIMEOUT_QUARTERS   5000
#define H2W_SLEEP_REQUEST_FORBID_LIMIT  60
#define H2W_TRY_WKUP_MAX_TIMES  3
#define WLAN_PM_MODULE               "[wlan]"


enum WLAN_PM_CPU_FREQ_ENUM {
    WLCPU_40MHZ     =   1,
    WLCPU_80MHZ     =   2,
    WLCPU_160MHZ    =   3,
    WLCPU_240MHZ    =   4,
    WLCPU_320MHZ    =   5,
    WLCPU_480MHZ    =   6,
};

enum WLAN_PM_SLEEP_STAGE {
    SLEEP_STAGE_INIT    = 0,  // 初始
    SLEEP_REQ_SND       = 1,  // sleep request发送完成
    SLEEP_ALLOW_RCV     = 2,  // 收到allow sleep response
    SLEEP_DISALLOW_RCV  = 3,  // 收到allow sleep response
    SLEEP_CMD_SND       = 4,  // 允许睡眠reg设置完成
};

enum WLAN_HOST_STATUS {
    HOST_NOT_SLEEP = 0,
    HOST_SLEEPED = 1,
};

#define ALLOW_IDLESLEEP     (1)
#define DISALLOW_IDLESLEEP  (0)

#define WIFI_PM_POWERUP_EVENT          (3)
#define WIFI_PM_POWERDOWN_EVENT        (2)
#define WIFI_PM_SLEEP_EVENT            (1)
#define WIFI_PM_WAKEUP_EVENT           (0)

/*****************************************************************************
  3 STRUCT DEFINE
*****************************************************************************/
typedef oal_bool_enum_uint8 (*wifi_srv_get_pm_pause_func)(osal_void);
typedef osal_void (*wifi_srv_open_notify)(oal_bool_enum_uint8);
typedef osal_void (*wifi_srv_pm_state_notify)(oal_bool_enum_uint8);

#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
typedef osal_void (*wifi_srv_data_wkup_print_en_func)(oal_bool_enum_uint8);
#endif
struct wifi_srv_callback_handler {
    wifi_srv_get_pm_pause_func     p_wifi_srv_get_pm_pause_func;
#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    wifi_srv_data_wkup_print_en_func     p_data_wkup_print_en_func;
#endif
};

typedef enum {
    WLAN_PM_ID_BCPU = 0,
    WLAN_PM_ID_MAX  = 31,
} wlan_pm_vote_id;

struct wlan_pm_s {
    hcc_bus                *bus;              // 保存oal_bus 的指针

    osal_ulong                wlan_pm_enable;    // host 侧软件控制pm使能开关 -- 是否向wcpu投票
    osal_ulong                wcpu_platform_pm_enable; /* wcpu平台侧低功耗使能 */
    osal_ulong                wlan_power_state;  // wlan power on state
    osal_ulong                wlan_state;  // wlan power on state
    volatile osal_ulong  host_sleep_state;  /* host sleep state notify device */

    volatile osal_ulong       wlan_dev_state;    // wlan sleep state
    volatile osal_ulong       sub_vote_status;
    oal_wakelock_stru       deepsleep_wakelock;
    osal_u32              packet_cnt;       // 睡眠周期内统计的packet个数
    osal_u32              wdg_timeout_cnt;  // timeout check cnt
    osal_u32              wdg_timeout_curr_cnt;  // timeout check current cnt
    volatile osal_ulong       sleep_stage;      // 睡眠过程阶段标识

    /* mark user doing something on node */
    atomic_t h2w_visit_users;
    osal_u32 users_refuse_slp_cnt;

    oal_completion          open_bcpu_done;
    oal_completion          close_bcpu_done;
    oal_completion          close_done;
    oal_completion          wakeup_done;
    oal_completion          sleep_request_ack;
    oal_completion          halt_bcpu_done;
    oal_completion          wifi_powerup_done;
    oal_completion          wcpu_pmctrl_ack;
    oal_completion          host_sleep_req_ack;

    struct wifi_srv_callback_handler wifi_srv_handler;

    /* 维测统计 */
    osal_u32              open_cnt;
    osal_u32              open_bcpu_done_callback;
    osal_u32              close_bcpu_done_callback;
    osal_u32              close_cnt;
    osal_u32              close_done_callback;
    osal_u32              wakeup_succ;
    osal_u32              wakeup_succ_work_submit;
    osal_u32              wakeup_dev_ack;
    osal_u32              wakeup_done_callback;
    osal_u32              wakeup_fail;
    osal_u32              wakeup_fail_wait_sdio;
    osal_u32              hcc_wkup_req_fail_cnt;
    osal_u32              wakeup_fail_timeout;
    osal_u32              wkup_err_cnt;
    osal_u32              wakeup_fail_set_reg;
    osal_u32              wakeup_fail_submit_work;
    osal_u32              d2h_work_cnt;

    osal_u32              sleep_succ;
    osal_u32              try_sleep_feed_wdg_cnt;
    osal_u32              sleep_feed_wdg_cnt;
    osal_u32              sleep_fail_request;
    osal_u32              sleep_fail_wait_timeout;
    osal_u32              sleep_fail_set_reg;
    osal_u32              sleep_fail_forbid;
    osal_u32              sleep_fail_forbid_cnt; /* forbid 计数，当睡眠成功后清除，维测信息 */
    osal_u32              sleep_work_submit;
    osal_s32              wlan_irq;
    osal_s32              irq_stat;
    osal_spinlock         irq_lock;
};
typedef struct wlan_memdump_s {
    osal_s32 addr;
    osal_s32 len;
    osal_s32 en;
} wlan_memdump_t;

/*****************************************************************************
  4 EXTERN VARIABLE
*****************************************************************************/
#if defined(_PRE_OS_VERSION) && defined(_PRE_OS_VERSION_LINUX) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
extern osal_u8 g_uc_custom_cali_done_etc;
#endif

typedef osal_s32 (*wlan_customize_init_cb)(osal_void);

typedef void (*pm_wow_wkup_cb)(void);
typedef void (*pm_wlan_suspend_cb)(void);
typedef osal_u32 (*pm_wlan_resume_cb)(void);
typedef void (*pm_ble_suspend_cb)(void);
typedef osal_s32 (*pm_ble_resume_cb)(void);

/*****************************************************************************
  5 EXTERN FUNCTION
*****************************************************************************/
extern struct wlan_pm_s*  wlan_pm_get_drv_etc(osal_void);
extern osal_void wlan_pm_debug_sleep_etc(void);
extern osal_void wlan_pm_debug_wakeup_etc(void);
extern void wlan_pm_dump_host_info_etc(void);
extern osal_s32 wlan_pm_host_info_print_etc(struct wlan_pm_s *wlan_pm, char *buf, osal_s32 buf_len);
extern void wlan_pm_dump_device_info_etc(void);
extern osal_void wlan_pm_debug_wake_lock_etc(void);
extern osal_void wlan_pm_debug_wake_unlock_etc(void);
extern struct wlan_pm_s*  wlan_pm_init_etc(osal_void);
extern osal_ulong  wlan_pm_exit_etc(osal_void);
extern osal_u32 wlan_pm_is_poweron_etc(osal_void);
extern osal_void wlan_cfg_init_cb_reg(wlan_customize_init_cb cb_cfg);
extern osal_s32 wlan_pm_open_etc(osal_void);
extern osal_s32 wlan_open(osal_void);
extern osal_s32 wlan_open_ex(osal_void);
extern osal_u32 wlan_pm_close_etc(osal_void);
extern osal_u32 wlan_close(osal_void);
extern osal_u32 wlan_close_ex(osal_void);
extern osal_ulong wlan_pm_init_dev_etc(void);
extern osal_ulong wlan_pm_wakeup_dev_etc(osal_void);
extern osal_ulong wlan_pm_wakeup_host_etc(void);
extern osal_ulong  wlan_pm_open_bcpu_etc(osal_void);
extern osal_ulong wlan_pm_state_get_etc(void);
extern osal_u32 wlan_pm_enable_etc(osal_void);
extern osal_u32 wlan_pm_disable_etc(osal_void);
extern osal_u32 wlan_pm_disable_check_wakeup(osal_s32 flag);
struct wifi_srv_callback_handler* wlan_pm_get_wifi_srv_handler_etc(osal_void);
extern osal_void wlan_pm_wakeup_dev_ack_etc(osal_void);
extern osal_s32 wlan_pm_poweroff_cmd_etc(osal_void);
osal_s32 wlan_power_open_cmd(osal_void);
osal_s32 wlan_power_close_cmd(osal_void);
extern osal_s32 wlan_pm_shutdown_bcpu_cmd_etc(osal_void);
extern osal_void  wlan_pm_feed_wdg_etc(osal_void);
extern osal_s32 wlan_pm_stop_wdg_etc(struct wlan_pm_s *wlan_pm_info);
extern void wlan_pm_info_clean_etc(void);
osal_u32 wlan_wcpu_platform_pm_enable(osal_s32 enable);
osal_void pm_wlan_gpio_intr_enable(osal_u32 en);
void wlan_pm_sub_vote(wlan_pm_vote_id idx, osal_u8 is_work);
extern osal_s32 wlan_pm_wakeup_for_pcie_open_wcpu(osal_void);
osal_s32 wlan_pm_h2d_notify_sleep_state(osal_u8 slp);
/* Increase or decrease the number of users accessing the device.
  @is_add: if true, after adding H2W users, the device system will be prevented from sleeping again.
           if false, will decrease user number, that mean no more access to device
 */
void wlan_pm_add_h2w_visit_user(int is_add);

extern osal_void pm_wifi_suspend_cb_host_register(pm_wlan_suspend_cb cb);
extern osal_void pm_wifi_resume_cb_host_register(pm_wlan_resume_cb cb);
extern osal_void pm_ble_suspend_cb_host_register(pm_ble_suspend_cb cb);
extern osal_void pm_ble_resume_cb_host_register(pm_ble_resume_cb cb);
extern osal_void pm_wifi_wkup_cb_host_register(pm_wow_wkup_cb cb);

void pm_shutdown_process_host(void);
osal_s32 pm_wkup_process_host(osal_void);

#if defined(_PRE_OS_VERSION) && defined(_PRE_OS_VERSION_LINUX) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
osal_s32 wlan_pm_register_notifier(struct notifier_block *nb);
osal_void wlan_pm_unregister_notifier(struct notifier_block *nb);
#endif

#endif

