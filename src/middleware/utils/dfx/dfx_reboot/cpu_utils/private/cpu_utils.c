/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  BT CPU UTILS Module
 * Author:
 * Create:
 */

#include "cpu_utils.h"
#if defined(SUPPORT_IPC)
#include "ipc_actions.h"
#include "ipc.h"
#elif defined(IPC_NEW)
#include "ipc.h"
#include "ipc_porting.h"
#endif
#ifdef SUPPORT_CPU_TRACE
#include "cpu_trace.h"
#endif
#include "chip_io.h"
#include "debug_print.h"
#include "non_os_reboot.h"
#include "hal_reboot.h"
#include "preserve.h"
#ifdef SUPPORT_PARTITION_INFO
#include "partition.h"
#endif
#include "preserve.h"
#if EXCEPTION_TEST_ENABLE == YES
#include "non_os.h"
#endif
#if defined(BUILD_APPLICATION_STANDARD) && (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
#include "log_def_pf.h"
#include "diag_log.h"
#endif
#ifdef SUPPORT_CRASH_DATA_RAM
#include "crash_data.h"
#endif
#if MCU_ONLY && defined(USE_CMSIS_OS)
#include "log_oml_exception.h"
#endif

#if MCU_ONLY
static cpu_utils_reboot_cb g_reboot_mcu_cb = NULL;
#endif

void cpu_utils_set_system_status_by_cause(reboot_cause_t cause)
{
#if (CORE == MASTER_BY_ALL)
    set_cpu_utils_reset_cause(cause);
    set_cpu_utils_system_boot_magic();
#else
    UNUSED(cause);
#endif
}

void cpu_utils_reset_chip_with_log(cores_t core, reboot_cause_t cause)
{
#if defined(BUILD_APPLICATION_STANDARD) && (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    PRINT("Reboot core:%d cause 0x%x", core, cause);
#endif
    UNUSED(core);
    UNUSED(cause);

#if (CORE == MASTER_BY_ALL)
    set_update_reset_cause_on_boot(false);
#endif
    cpu_utils_set_system_status_by_cause(cause);
#if (WAIT_APPS_DUMP_FOREVER == NO)
    hal_reboot_chip();
#endif
}

void cpu_utils_reset_chip_with_cause(reboot_cause_t cause)
{
#if (CORE == MASTER_BY_ALL)
    set_update_reset_cause_on_boot(false);
#endif
    cpu_utils_set_system_status_by_cause(cause);
    hal_reboot_chip();
}

static inline void cpu_utils_sec_rebooted(void)
{
#if (CORE == MASTER_BY_ALL)
    if (get_update_reset_cause_on_boot()) {
        set_cpu_utils_reset_cause(reboot_get_security_reboot_reason());
    }
#endif
}

#if EXCEPTION_TEST_ENABLE == YES && defined(SUPPORT_IPC)
static bool exception_test_handler(ipc_action_t message, const volatile ipc_payload *payload_p,
                                   cores_t src, uint32_t id)
{
    UNUSED(message);
    UNUSED(src);
    UNUSED(id);

    uint16_t exception_cause = payload_p->exception_test_cmd.command;
    volatile uint16_t i = true;
    switch (exception_cause) {
        case EXCEPTION_TEST_COMMAND_BT_WDT_REBOOT:
        case EXCEPTION_TEST_COMMAND_GNSS_WDT_REBOOT:
            while (i == true) {}
            break;
        case EXCEPTION_TEST_COMMAND_BT_HARDFAULT:
        case EXCEPTION_TEST_COMMAND_GNSS_HARDFAULT:
            // tscancode-suppress *
            writel(0xFFFFFFFF, 0xFF);
            break;
        case EXCEPTION_TEST_COMMAND_BT_PANIC:
        case EXCEPTION_TEST_COMMAND_GNSS_PANIC:
            panic(PANIC_EXCEPTION_TEST, __LINE__);
            break;
        case EXCEPTION_TEST_COMMAND_BT_STD_CHIP_WDT_FRST:
            non_os_enter_critical();
            while (i == true) {}
            non_os_exit_critical();
            break;
        default:
            break;
    }

    return true;
}
#endif

static void exception_test_init(void)
{
#if EXCEPTION_TEST_ENABLE == YES && defined(SUPPORT_IPC)
    (void)ipc_register_handler(IPC_ACTION_EXCEPTION_TEST, exception_test_handler);
#endif
}

#ifdef SUPPORT_IPC
static bool cpu_utils_sys_reboot_req(ipc_action_t message, const volatile ipc_payload *payload_p,
                                     cores_t src, uint32_t id)
{
    UNUSED(message);
    UNUSED(src);
    UNUSED(id);
    uint16_t reboot_reason = payload_p->request_reboot.requested_reboot_reason;
    set_cpu_utils_reset_cause((reboot_cause_t)reboot_reason);

#ifdef SUPPORT_CPU_TRACE
    cpu_trace_disable();
#endif
#if (BTH_WITH_SMART_WEAR == YES)
    set_cpu_utils_system_boot_magic();
#endif
    set_update_reset_cause_on_boot(false);
#if MCU_ONLY && defined(USE_CMSIS_OS)
    log_oml_memory_dump();
#endif
#if MCU_ONLY
    if (g_reboot_mcu_cb != NULL) {  // when registering g_reboot_mcu_cb, need to restart chip by yourself.
        g_reboot_mcu_cb(src);
        return true;
    }
#if (defined(SUPPORT_CRASH_DATA_RAM) && defined(SAVE_EXC_INFO))
    crash_data_save();
#endif

#endif
    cpu_utils_reset_chip_with_log(src, (reboot_cause_t)reboot_reason);

    return true;
}
#elif defined(IPC_NEW)
static void cpu_utils_sys_reboot_req(uint8_t *payload_addr, uint32_t payload_len)
{
    UNUSED(payload_len);
    req_reboot_msg *msg = (req_reboot_msg *)payload_addr;
    uint16_t reboot_reason = msg->req_reboot_reason;
    cores_t src = (cores_t)msg->core;
    set_cpu_utils_reset_cause((reboot_cause_t)reboot_reason);
#ifdef SUPPORT_CPU_TRACE
    cpu_trace_disable();
#endif
#if (BTH_WITH_SMART_WEAR == YES)
    set_cpu_utils_system_boot_magic();
#endif
    set_update_reset_cause_on_boot(false);
#if MCU_ONLY && defined(USE_CMSIS_OS)
    log_oml_memory_dump();
#endif
#if MCU_ONLY
    if (g_reboot_mcu_cb != NULL) {  // when registering g_reboot_mcu_cb, need to restart chip by yourself.
        g_reboot_mcu_cb(src);
        return;
    }
#endif
    cpu_utils_reset_chip_with_log(src, (reboot_cause_t)reboot_reason);
}
#endif

void cpu_utils_init(void)
{
    cpu_utils_sec_rebooted();
    set_update_reset_cause_on_boot(true);
#ifdef SUPPORT_IPC
    (void)ipc_register_handler(IPC_ACTION_SYS_REBOOT_REQ, cpu_utils_sys_reboot_req);
#elif defined(IPC_NEW)
    ipc_rx_handler_info_t handler_info = {0};
    handler_info.msg_id = IPC_MSG_SYS_REBOOT_REQ;
    handler_info.cb = cpu_utils_sys_reboot_req;
    uapi_ipc_register_rx_handler(&handler_info);
#endif
    exception_test_init();
}

cores_t cpu_utils_core_images_to_cores(core_images_e cimage)
{
    switch (cimage) {
        case (CORE_IMAGES_BT):
        case (CORE_IMAGES_RECOVERY):
            return CORES_BT_CORE;
        case (CORE_IMAGES_PROTOCOL):
            return CORES_PROTOCOL_CORE;
        case (CORE_IMAGES_APPS):
            return CORES_APPS_CORE;
#if CORE_NUMS >= 2 && CORE_NUMS < 5
        case (CORE_IMAGES_EXTERN0):
            return CORES_EXTERN0_CORE;
#elif CORE_NUMS >= 5
        case (CORE_IMAGES_EXTERN0):
            return CORES_EXTERN0_CORE;
        case (CORE_IMAGES_EXTERN1):
        case (CORE_IMAGES_EXTERN1_SSB):
            return CORES_EXTERN1_CORE;
#endif
        default:
            return CORES_UNKNOWN;
    }
}

void cpu_utils_set_mcu_callback(cpu_utils_reboot_cb cb)
{
#if MCU_ONLY
    if (cb != NULL) {
        g_reboot_mcu_cb = cb;
    }
#else
    UNUSED(cb);
#endif
}

#ifdef SUPPORT_PARTITION_INFO
partition_ids_t cpu_utils_core_iamge_to_partition_id(core_images_e cimage)
{
    switch (cimage) {
        case (CORE_IMAGES_BT):
            return PARTITION_BT_IMAGE;
        case (CORE_IMAGES_APPS):
            return PARTITION_ACPU_IMAGE;
        case (CORE_IMAGES_PROTOCOL):
            return PARTITION_DSP0_IMAGE;
        case (CORE_IMAGES_EXTERN0):
            return PARTITION_DSP1_IMAGE;
        case (CORE_IMAGES_RECOVERY):
            return PARTITION_FOTA_DATA;
        default:
            return PARTITION_MAX_CNT;
    }
}
#endif