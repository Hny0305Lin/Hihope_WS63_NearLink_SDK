/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  LOG MODULE
 * Author:
 * Create:
 */
#include "log_common.h"
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
#include "tcxo.h"
#endif
#include "soc_osal.h"
#include "non_os.h"
#include "log_oam_logger.h"
#include "log_system_messages.h"
#include "log_oml_exception.h"
#include "log_memory_region.h"
#ifdef USE_CMSIS_OS
#ifdef USE_OSAL_INSTEAD_CMSIS
#include "los_sched_pri.h"
#else
#include "cmsis_os2.h"
#endif
#endif
#ifdef SUPPORT_DFX_PRESERVE
#include "preserve.h"
#endif
#include "log_buffer.h"
#include "log_trigger.h"
#ifdef SUPPORT_CONNECTIVITY
#include "connectivity_log.h"
#endif
#if (CORE == BT) || (CORE == CONTROL_CORE) || (CORE == SENSOR)
#include "log_def.h"
#include "log_oam_mem_query.h"
#include "log_oam_reg_query.h"
#include "log_printf.h"
#define LOG_MEMORY_REGION_SECTION_CORE LOG_MEMORY_REGION_SECTION_0
#define CORES_THIS_CORE                CORES_BT_CORE
#elif (CORE == PROTOCOL) && defined(CHIP_CAT1) && (CHIP_CAT1 == 1)
#define LOG_MEMORY_REGION_SECTION_CORE LOG_MEMORY_REGION_SECTION_0
#define CORES_THIS_CORE                CORES_PROTOCOL_CORE
#elif CORE == APPS
#include "log_oam_mem_query.h"
#include "log_oam_reg_query.h"
#if CORE_NUMS > 1
#define LOG_MEMORY_REGION_SECTION_CORE LOG_MEMORY_REGION_SECTION_1
#else
#define LOG_MEMORY_REGION_SECTION_CORE LOG_MEMORY_REGION_SECTION_0
#endif
#define CORES_THIS_CORE                CORES_APPS_CORE
#elif CORE == GNSS
#include "log_def.h"
#define LOG_MEMORY_REGION_SECTION_CORE LOG_MEMORY_REGION_SECTION_3
#define CORES_THIS_CORE                CORES_GNSS_CORE
#elif CORE == SECURITY
#include "log_def.h"
#define LOG_MEMORY_REGION_SECTION_CORE LOG_MEMORY_REGION_SECTION_4
#define CORES_THIS_CORE                CORES_SEC_CORE
#else
#error "No core defined"
#endif

#if CORE == BT
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_0
#elif CORE == APPS
#if CORE_NUMS == 1
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_0
#else
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_1
#endif
#elif CORE == GNSS
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_0
#elif CORE == SECURITY
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_0
#elif CORE == CONTROL_CORE
#define MASS_MEMORY_REGION_SECTION_CORE MASS_MEMORY_REGION_SECTION_0
#endif

#if CORE != APPS
#include "ipc.h"
#endif

#if CORE == BT
#include "log_oam_msg.h"
#include "log_oam_ota.h"
#include "log_oam_status.h"
#endif

#if (CORE == MASTER_BY_ALL)
#if SYS_DEBUG_MODE_ENABLE == YES
#include "dfx_feature_config.h"
#endif
#endif

#define BT_CORE_INDEX 1
#define GNSS_CORE_INDEX 3
#define SEC_CORE_INDEX 4

#ifndef DEFAULT_LOG_LEVEL
#define DEFAULT_LOG_LEVEL 3
#endif
/** Variable storing the number of missed messages since last time it was logged */
static uint32_t g_missed_messages = 0;
static uint32_t g_cnt_messages = 0;

/** Variable storing the current logging level */
static log_level_e g_log_current_level = (log_level_e)DEFAULT_LOG_LEVEL;

/* Whether need trigger log uart. */
static bool g_trigger_tx = false;
static uint16_t g_log_sn_number = 0;

#if (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
#elif CORE != APPS && !defined(IPC_NEW)
static bool set_log_level_action_handler(ipc_action_t message,
                                         const volatile ipc_payload *payload_p, cores_t src, uint32_t id);
#endif

uint16_t get_log_sn_number(void)
{
    return g_log_sn_number++;
}

void log_init(void)
{
    UNUSED(g_missed_messages);
    log_buffer_init(LOG_MEMORY_REGION_SECTION_CORE);
#ifdef SUPPORT_CONNECTIVITY
    init_log_level();
#endif
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    default_register_hal_exception_dump_callback();
#else /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES */
    compress_log_init();
#endif /* end of USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG */

#if (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
#elif (CORE != APPS) && !defined(IPC_NEW)
    UNUSED(set_log_level_action_handler);

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
#if SYS_DEBUG_MODE_ENABLE == YES
    log_register_oam_msg_callback();
#endif /* end of SYS_DEBUG_MODE_ENABLE == YES */
#else /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES */
    (void)ipc_register_handler(IPC_ACTION_SET_LOG_TIME, set_log_time_action_handler);
    (void)ipc_register_handler(IPC_ACTION_SET_LOG_SWITCH, set_log_switch_action_handler);
#endif /* end of USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG */
    (void)ipc_register_handler(IPC_ACTION_SET_LOG_LEVEL, (ipc_action_handler)set_log_level_action_handler);
#if SYS_DEBUG_MODE_ENABLE == YES

#endif
#endif

#if SYS_DEBUG_MODE_ENABLE == YES
#if CORE == APPS
    log_register_oam_msg_callback();
#elif CORE == BT
    (void)ipc_register_handler(IPC_ACTION_HCI_INFORM, (ipc_action_handler)get_hci_data_action_handler);
#endif
#endif
}

void massdata_init(void)
{
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES)
    mass_buffer_init(MASS_MEMORY_REGION_SECTION_CORE);
#endif
}

#ifdef USE_CMSIS_OS
void log_init_after_rtos(void)
{
#if CORE == BT
    log_oml_ota_init();
    log_oam_status_store_init();
#endif
}
#endif

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
static void log_event_trigger(void)
{
#ifdef USE_CMSIS_OS
#ifdef USE_OSAL_INSTEAD_CMSIS
    if (OS_SCHEDULER_ALL_ACTIVE) {
#else
    if (osKernelGetState() == osKernelRunning) {
#endif
        if (g_trigger_tx) {
            log_trigger();
            g_trigger_tx = false;
        }
    }
#else
    if (g_trigger_tx) {
        log_trigger();
        g_trigger_tx = false;
    }
#endif  // defined USE_CMSIS_OS
}
#endif
void log_event(const uint8_t *buffer, uint16_t length)
{
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    log_ret_t lret;
    uint32_t lb_available = 0;
    bool was_empty = false;
    log_buffer_header_t lb_header;

#if CORE != APPS
    if (log_memory_is_init() != true) { return; }
#endif
    if (buffer == NULL || length == 0) { return; }

    uint32_t irq = osal_irq_lock();
    lb_header.time_us = (uint32_t)uapi_tcxo_get_us();

    // If missed messages pending send notification
    /* Check if there is space for the missed message indication. */
    lret = log_buffer_get_available_for_next_message(&lb_available);
    if (lret != LOG_RET_OK) {
        osal_irq_restore(irq);
        return;
    }

    if (lb_available > length) {
        lb_header.length = length + (uint16_t)sizeof(lb_header);
        lb_magic_set(&lb_header);
        // add message header to log buffer
        log_buffer_write(&lb_header, (const uint8_t *)buffer, &was_empty);
        if (!g_trigger_tx) {
            g_trigger_tx = was_empty;
        }
    } else {
#if (CORE != MASTER_BY_ALL) || (CORE_NUMS == 1)
        g_trigger_tx = true;
#endif
        g_missed_messages++;
    }
    g_cnt_messages++;
    osal_irq_restore(irq);

    log_event_trigger();

#else  /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES */
    UNUSED(buffer);
    UNUSED(length);
    UNUSED(g_trigger_tx);
#endif /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG */
}

#if (CORE != APPS) && defined(SUPPORT_CONNECTIVITY)
void init_log_level(void)
{
    /* Point to the share memory and clear the area */
    log_level_config_t *log_level_config_p = (log_level_config_t *)(uintptr_t)(SYSTEM_CFG_REGION_START);
    uint8_t index_data;
#if CORE == BT
    index_data = BT_CORE_INDEX;
#elif CORE == GNSS
    index_data = GNSS_CORE_INDEX;
#else
    index_data = SEC_CORE_INDEX;
#endif
    uint32_t mgc_log_lvl =  *((uint32_t *)log_level_config_p + index_data);
    if ((get_log_level_magic(mgc_log_lvl) != LOG_LEVEL_MAGIC)
        || (get_real_log_level(mgc_log_lvl) >= LOG_LEVEL_MAX)
        || (get_real_log_level(mgc_log_lvl) == LOG_LEVEL_NONE)) {
        /* Set the log level in share memory to default values */
        mgc_log_lvl = set_log_level_magic((uint32_t)DEFAULT_LOG_LEVEL);
         *((uint32_t *)log_level_config_p + index_data) = mgc_log_lvl;
    }

    log_set_local_log_level((log_level_e)get_real_log_level(mgc_log_lvl));
}
#endif

/**
 * Set local log level from system log level
 * @return none
 */
void log_set_local_log_level(log_level_e log_level)
{
    g_log_current_level = log_level;
}

/**
 * Get current log level
 * @return current log level
 */
log_level_e log_get_local_log_level(void)
{
    return g_log_current_level;
}

/**
 * Get missed messages count
 * @return missed messages count
 */
uint32_t log_get_missed_messages_count(void)
{
    return g_missed_messages;
}

/**
 * Get all messages count
 * @return all messages count
 */
uint32_t log_get_all_messages_count(void)
{
    return g_cnt_messages;
}
#if (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
#elif (CORE != APPS) && !defined(IPC_NEW)
/**
 * This handler gets called when the app core wants to set the log level of other cores
 * @param message   The IPC message
 * @param payload_p The payload, contains the reset reason
 * @param src       The core who has just started
 * @param id        The message ID (should always be 0 - the first message after starting)
 * @return
 */
static bool set_log_level_action_handler(ipc_action_t message,
                                         const volatile ipc_payload *payload_p, cores_t src, uint32_t id)
{
    if (message != IPC_ACTION_SET_LOG_LEVEL) {
        oml_pf_log_print1(LOG_BCORE_PLT_LIB_LOG, LOG_NUM_LIB_LOG, LOG_LEVEL_INFO,
                          "[INFO]:Change log level failed, message is %d", message);
        return true;
    }
    UNUSED(message);
    UNUSED(id);
    UNUSED(src);

    oml_pf_log_print1(LOG_BCORE_PLT_LIB_LOG, LOG_NUM_LIB_LOG, LOG_LEVEL_INFO, "[INFO]:Change log level to %d", \
                      payload_p->set_log_level.log_level);

    (void)log_set_local_log_level((log_level_e)payload_p->set_log_level.log_level);

    return true;
}
#endif

#if SYS_DEBUG_MODE_ENABLE == YES
void log_register_oam_msg_callback(void)
{
#if (CORE == MASTER_BY_ALL)
#if defined(CONFIG_DFX_SDT_MEMORY_OPERATE) && (CONFIG_DFX_SDT_MEMORY_OPERATE == DFX_YES)
    oml_reg_register_callback();
    oml_mem_register_callback();
#endif
#endif

#if MCU_ONLY && (CORE_NUMS  > 1)
#if CHIP_WS53
#else
    oml_btc_cmd_callback();
#endif
    oml_ssi_reg_register_callback();
#ifdef USE_GPIO_SIMULATE_SSI
    oml_ssi_reg32_register_callback();
    oml_ssi_block_callback();
#endif
#endif
}
#endif  /* end of SYS_DEBUG_MODE_ENABLE == YES */
