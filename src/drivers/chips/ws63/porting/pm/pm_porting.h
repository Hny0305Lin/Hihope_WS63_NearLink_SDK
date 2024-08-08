/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provide pm port header \n
 *
 * History: \n
 * 2023-11-13, Create file. \n
 */

#ifndef PM_PORTING_H
#define PM_PORTING_H

#include "common_def.h"
#include "osal_types.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* 本模块类型定义 */
typedef enum {
    PM_HOOK_ENTER_LOWPOWER_PREPARE = 0,
    PM_HOOK_ENTER_LOWPOWER_FINISH,
    PM_HOOK_EXIT_LOWPOWER_PREPARE,
    PM_HOOK_EXIT_LOWPOWER_FINISH,
    PM_HOOK_MAX_NUM
} pm_hook_t;

#ifdef CONFIG_PM_SUPPORT_SRV_DECOUPLING
typedef enum {
    PM_MODE_INIT,
    PM_MODE_SUSPEND,
    PM_MODE_RESUME,
    PM_MODE_WORK,
    PM_MODE_MAX,
} pm_srv_mode;

typedef enum {
    PM_SRV_WIFI,
    PM_SRV_BLE,
    PM_SRV_SLE,
    PM_SRV_PLAT, // 必须是最后一个
    PM_SRV_MAX,
} pm_srv_type;

errcode_t pm_porting_init(void);
errcode_t pm_set_srv_mode(pm_srv_type svr, pm_srv_mode mode);
#endif

typedef void (*pm_hook_cb_t)(void);

/* extern 引用其他模块函数 */
extern void test_suite_uart_init(void);
extern void at_uart_init(void);
extern void set_efuse_period(void);
extern osal_void cali_set_cali_done_flag(osal_u8 is_done);

extern uint32_t bt_thread_open_btc(void);
extern uint32_t bt_thread_close_btc(void);
extern void bt_vendor_init(void);
extern errcode_t enable_ble(void);
extern errcode_t disable_ble(void);

/* 本模块函数接口声明 */
void pm_port_open_rf(void);
void pm_port_close_rf(void);
void pm_port_open_wifi_sub(void);
void pm_port_close_wifi_sub(void);
void pm_port_open_bt_sub(void);
void pm_port_close_bt_sub(void);

void pm_port_open_wifi_hw(void);
void pm_port_close_wifi_hw(void);
void pm_port_open_bt_hw(void);
void pm_port_close_bt_hw(void);

int pm_port_flash_sem_down(void);
void pm_port_flash_sem_up(void);
int pm_port_dma_sem_down(void);
void pm_port_dma_sem_up(void);

void pm_port_exit_lowpower_finish_cb_default(void);
void pm_port_exit_lowpower_prepare_cb_default(void);
void pm_port_enter_lowpower_finish_cb_default(void);
void pm_port_enter_lowpower_prepare_cb_default(void);

void pm_port_pm_hook_cb_register(uint32_t hook, pm_hook_cb_t cb);

void pm_port_enter_lowpower(void);
void pm_port_exit_lowpower(void);

void pm_porting_wait_exit_lowpower(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif