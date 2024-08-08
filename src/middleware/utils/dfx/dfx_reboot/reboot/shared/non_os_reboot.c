/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  Reboot interface.
 * Author:
 * Create:  2018-10-15
 */
#include "non_os.h"
#include "systick.h"
#include "hal_reboot.h"
#include "preserve.h"
#if defined(SUPPORT_IPC)
#include "ipc.h"
#elif defined(IPC_NEW)
#include "ipc.h"
#include "ipc_porting.h"
#endif
#if  ((IS_MAIN_CORE == NO))
#include "watchdog.h"
#endif
#include "non_os_reboot.h"
#if defined(CONFIG_DFX_SUPPORT_DELAY_REBOOT) && (CONFIG_DFX_SUPPORT_DELAY_REBOOT == DFX_YES)
#include "dfx_feature_config.h"
#include "dfx_adapt_layer.h"
#endif

#define DELAY_MS_BEFORE_REBOOT 1000

#if defined(CONFIG_DFX_SUPPORT_DELAY_REBOOT) && (CONFIG_DFX_SUPPORT_DELAY_REBOOT == DFX_YES)
typedef struct {
    uint32_t permit_sec;  /* 允许重启时间 */
} delay_request_item_t;   /* 延时请求的一个item */

typedef struct {
    delay_request_item_t item[VETO_REBOOT_REASON_MAX];
    uint32_t permit_reboot_sec;
} delay_request_t;    /* 延时请求结构 */

static delay_request_t g_delay_request_info = {0};
#endif /* CONFIG_DFX_SUPPORT_DELAY_REBOOT */

static reboot_cause_t g_reboot_reset_reason = REBOOT_CAUSE_UNKNOWN;

/* Initialise the reboot subsystem */
void reboot_init(void)
{
    if (non_os_is_driver_initialised(DRIVER_INIT_REBOOT) == true) {
        return;
    }

    non_os_set_driver_initalised(DRIVER_INIT_REBOOT, true);
}

/* De-initialise the reboot subsystem */
void reboot_deinit(void)
{
    non_os_set_driver_initalised(DRIVER_INIT_REBOOT, false);
}

#if CORE == MASTER_BY_ALL
void reboot_chip(void)
{
    hal_reboot_chip();
}
#endif

#if CORE == APPS
#if EXCEPTION_TEST_ENABLE == YES && defined(SUPPORT_IPC)
static void cores_exception_test(cores_t core, ipc_exception_test_command_e exception_test_command)
{
    ipc_status_t ipc_returned_value;
    ipc_payload_exception_test test_command;
    test_command.command = exception_test_command;

    ipc_returned_value = ipc_spin_send_message_timeout(core,
                                                       IPC_ACTION_EXCEPTION_TEST,
                                                       (const ipc_payload *)&test_command,
                                                       sizeof(ipc_payload_exception_test),
                                                       IPC_PRIORITY_LOWEST,
                                                       false, IPC_SPIN_SEND_DEFAULT_TIMEOUT);
    if (ipc_returned_value != IPC_STATUS_OK) {
        UNUSED(ipc_returned_value);
    }
}

static void app_exception_test(ipc_exception_test_command_e exception_test_command)
{
    uint16_t* test_pointer = NULL;
    volatile uint16_t i = true;
    switch (exception_test_command) {
        case EXCEPTION_TEST_COMMAND_APP_WTD_REBOOT:
            while (i != 0) {}
            break;
        case EXCEPTION_TEST_COMMAND_APP_PANIC:
            panic(PANIC_EXCEPTION_TEST, __LINE__);
            break;
        case EXCEPTION_TEST_COMMAND_APP_HARDFAULT:
            // tscancode-suppress *
            *test_pointer = 0;
            break;
        case EXCEPTION_TEST_COMMAND_APP_WDT_FRST:
            non_os_enter_critical();
            while (i != 0) {}
            non_os_exit_critical();
            break;
        default:
            break;
    }
}
#endif

#if EXCEPTION_TEST_ENABLE == YES && defined(SUPPORT_IPC)
void exception_test(ipc_exception_test_command_e exception_test_command)
{
    if (exception_test_command <= EXCEPTION_TEST_COMMAND_BT_STD_CHIP_WDT_FRST) {
        cores_exception_test(CORES_BT_CORE, exception_test_command);
    } else if (exception_test_command <= EXCEPTION_TEST_COMMAND_APP_WDT_FRST) {
        app_exception_test(exception_test_command);
#if (CHIP_LIBRA == 1)
    } else {
        cores_exception_test(CORES_GNSS_CORE, exception_test_command);
#endif
    }
}
#endif
#endif

/* Perform a software reboot of the entire system. */
void reboot_system(reboot_cause_t cause)
{
    UNUSED(cause);
#ifdef SUPPORT_IPC
    ipc_payload_request_reboot request_reboot;
    request_reboot.requested_reboot_reason = ((uint16_t)cause);
    UNUSED(request_reboot);
#if (IS_MAIN_CORE == NO)
    non_os_enter_critical();
    (void)ipc_spin_send_message_timeout(CORES_APPS_CORE,
                                        IPC_ACTION_SYS_REBOOT_REQ,
                                        (const ipc_payload *)&request_reboot,
                                        sizeof(ipc_payload_request_reboot),
                                        IPC_PRIORITY_HIGHEST,
                                        false, IPC_SPIN_SEND_MAX_TIMEOUT);
    non_os_exit_critical();
    while (true) {
        uapi_watchdog_kick();
    }
#endif
#elif (defined(IPC_NEW) && (IS_MAIN_CORE == NO))
    req_reboot_msg msg = {0};
    msg.req_reboot_reason = (uint16_t)cause;
    msg.core = CORE;
    ipc_msg_info_t head = {0};
    head.dst_core = CORES_APPS_CORE;
    head.priority = 1;
    head.msg_id = IPC_MSG_SYS_REBOOT_REQ;
    head.buf_addr = (uint8_t *)&msg;
    head.buf_len = (size_t)sizeof(req_reboot_msg);
    non_os_enter_critical();
    (void)uapi_ipc_send_msg_sync(&head);
    non_os_exit_critical();
#endif
#if MCU_ONLY
    reboot_chip();
#endif
}

reboot_cause_t reboot_get_reset_reason(void)
{
    return g_reboot_reset_reason;
}

#if CORE == MASTER_BY_ALL
reboot_cause_t reboot_get_security_reboot_reason(void)
{
    return (reboot_cause_t)hal_reboot_get_reset_reason();
}

#endif  // CORE == BT

#if defined(CONFIG_DFX_SUPPORT_DELAY_REBOOT) && (CONFIG_DFX_SUPPORT_DELAY_REBOOT == DFX_YES)
/*
* @brief 延时重启,在一段时间内投票反对重启，仅影响uapi_reboot_system_check_veto接口。
* @param reason 延时原因
* @param sec 延时时间
* @retval 返回实际延时时间。如果系统未对最大延时时间进行配置返回传入时间。如果系统进行了限制则返回时间小于最大延时时间。
*/
uint32_t uapi_reboot_vote_against(veto_reboot_reason_t reason, uint32_t sec)
{
    uint32_t permit_sec = 0;
    if (reason >= VETO_REBOOT_REASON_MAX) {
        return ERRCODE_FAIL;
    }

    uint32_t cur_sec = dfx_get_cur_second();
    if (cur_sec == 0) {
        return ERRCODE_FAIL;
    }
    permit_sec = cur_sec + sec;

    uint32_t int_value = osal_irq_lock();

    if (permit_sec > g_delay_request_info.item[reason].permit_sec) {
        g_delay_request_info.item[reason].permit_sec = permit_sec;
    }

    osal_irq_restore(int_value);
    return ERRCODE_SUCC;
}

/*
* @brief 去延时重启
* @param reason 延时原因
* @retval 返回实际延时时间。如果系统未对最大延时时间进行配置返回传入时间。如果系统进行了限制则返回时间小于最大延时时间。
*/
void uapi_reboot_unvote_against(veto_reboot_reason_t reason)
{
    if (reason >= VETO_REBOOT_REASON_MAX) {
        return ;
    }

    uint32_t int_value = osal_irq_lock();
    g_delay_request_info.item[reason].permit_sec = 0;

    osal_irq_restore(int_value);
}

/*
* @brief 重启。受延时重启影响
* @param reason 重启原因
*/
void uapi_reboot_system_check_veto(reboot_cause_t reason)
{
    uint32_t cur_sec = dfx_get_cur_second();
    if (cur_sec == ERRCODE_FAIL) {
        return;
    }
    uint32_t permit_sec = 0;
    uint32_t int_value = osal_irq_lock();

    for (int i = 0; i < VETO_REBOOT_REASON_MAX; i++) {
        if (g_delay_request_info.item[i].permit_sec > permit_sec) {
            permit_sec = g_delay_request_info.item[i].permit_sec;
        }
    }

    osal_irq_restore(int_value);

    if (permit_sec > cur_sec) {
        return;
    } else {
        reboot_system(reason);
    }
}

#else
uint32_t uapi_reboot_vote_against(veto_reboot_reason_t reason, uint32_t sec)
{
    unused(reason);
    unused(sec);
    return 0;
}
void uapi_reboot_unvote_against(veto_reboot_reason_t reason)
{
    unused(reason);
    return;
}

void uapi_reboot_system_check_veto(reboot_cause_t reason)
{
    reboot_system(reason);
}
#endif /* CONFIG_DFX_SUPPORT_DELAY_REBOOT */
