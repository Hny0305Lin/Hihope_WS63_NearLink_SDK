/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: dfx system init
 * This file should be changed only infrequently and with great care.
 */

#include "dfx_adapt_layer.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "diag_cmd_connect.h"
#include "diag_cmd_filter.h"
#include "diag_cmd_password.h"
#include "diag_cmd_beat_heart.h"
#include "diag_cmd_get_mem_info.h"
#include "diag_cmd_get_task_info.h"
#include "diag_cmd_mem_read_write.h"
#include "diag_mocked_shell.h"
#include "diag_bt_sample_data.h"
#include "osal_task.h"
#include "osal_msgqueue.h"
#include "diag_ind_src.h"
#include "diag_filter.h"
#include "diag_msg.h"
#ifdef SUPPORT_DIAG_V2_PROTOCOL
#include "diag_service.h"
#include "diag_cmd_dispatch.h"
#endif /* SUPPORT_DIAG_V2_PROTOCOL */
#include "diag_rom_api.h"
#include "diag_channel.h"
#include "diag_adapt_layer.h"
#include "diag_adapt_sdt.h"
#include "soc_log_uart_instance.h"
#include "sample_data_adapt.h"
#include "dfx_channel.h"
#if (CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES)
#include "transmit.h"
#endif
#include "diag_adapt_psd.h"
#include "diag_sample_data.h"
#include "log_common.h"
#include "diag_dfx_cmd_init.h"
#ifdef CONFIG_MIDDLEWARE_NV_SUPPORT_HSO_ACCESS
#include "nv_adapt_zdiag.h"
#endif
#ifdef CONFIG_AT_SUPPORT_ZDIAG
#include "at_zdiag.h"
#endif

static diag_cmd_reg_obj_t g_diag_default_cmd_tbl[] = {
    { DIAG_CMD_CONNECT_RANDOM, DIAG_CMD_PWD_CHANGE, diag_cmd_password },
    { DIAG_CMD_HOST_CONNECT, DIAG_CMD_HOST_DISCONNECT, diag_cmd_hso_connect_disconnect },
#ifndef SUPPORT_DIAG_V2_PROTOCOL
    { DIAG_CMD_MSG_RPT_AIR, DIAG_CMD_MSG_RPT_USR, diag_cmd_filter_set },
    { DIAG_CMD_MSG_CFG_SET_AIR, DIAG_CMD_MSG_CFG_SET_LEVEL, diag_cmd_filter_set },
#else
    { DIAG_CMD_MSG_CFG_SET_SYS, DIAG_CMD_MSG_CFG_SET_LEVEL, diag_cmd_filter_set },
#endif
#if (CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART == DFX_YES)
    { DIAG_CMD_HEART_BEAT, DIAG_CMD_HEART_BEAT, diag_cmd_beat_heart },
#endif
    { DIAG_CMD_GET_TASK_INFO, DIAG_CMD_GET_TASK_INFO, diag_cmd_get_task_info},
    { DIAG_CMD_GET_MEM_INFO, DIAG_CMD_GET_MEM_INFO, diag_cmd_get_mem_info},
#if (CONFIG_DFX_MEMORY_OPERATE == DFX_YES)
    { DIAG_CMD_MEM_MEM32, DIAG_CMD_MEM_W4, diag_cmd_mem_operate },
#endif
#if (CONFIG_DFX_BT_SAMPLE_DATA == DFX_YES)
    { DIAG_CMD_ID_SAMPLE, DIAG_CMD_ID_SAMPLE, diag_cmd_sample_data},
#endif
    { DIAG_CMD_ID_PSD_ENABLE, DIAG_CMD_ID_PSD_ENABLE, diag_cmd_psd_enable},
    { DIAG_CMD_ID_BGLE_SAMPLE, DIAG_CMD_ID_WLAN_PHY_SAMPLE, diag_cmd_wlan_module_sample_data},
#ifdef CONFIG_MIDDLEWARE_NV_SUPPORT_HSO_ACCESS
    {DIAG_CMD_NV_QRY, DIAG_CMD_NV_QRY, zdiag_adapt_nv_read},
    {DIAG_CMD_NV_WR, DIAG_CMD_NV_WR, zdiag_adapt_nv_write},
#endif
#ifdef CONFIG_AT_SUPPORT_ZDIAG
    {DIAG_CMD_SIMULATE_AT, DIAG_CMD_SIMULATE_AT, zdiag_at_proc},
#endif
};

#define DFX_MSG_STACK_SIZE          0x800
#define TASK_PRIORITY_DFX_MSG       (osPriority_t)(5)
unsigned long g_dfx_osal_queue_id;
unsigned long dfx_get_osal_queue_id(void)
{
    return g_dfx_osal_queue_id;
}
static errcode_t register_default_diag_cmd(void)
{
    return uapi_diag_register_cmd(g_diag_default_cmd_tbl,
        sizeof(g_diag_default_cmd_tbl) / sizeof(g_diag_default_cmd_tbl[0]));
}

int32_t msg_process_proc(uint32_t msg_id, uint8_t *data, uint32_t size)
{
    switch (msg_id) {
        case DFX_MSG_ID_DIAG_PKT:
            diag_msg_proc((uint16_t)msg_id, data, size);
            break;
        case DFX_MSG_ID_SDT_MSG:
            zdiag_adapt_sdt_msg_dispatch(msg_id, data, size);
            break;
        case DFX_MSG_ID_BEAT_HEART:
#if (CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART == DFX_YES)
            diag_beat_heart_process();
#endif
            break;
#if defined(CONFIG_DFX_SUPPORT_TRANSMIT_FILE) && (CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES)
        case DFX_MSG_ID_TRANSMIT_FILE:
            transmit_msg_proc(msg_id, data, size);
            break;
#endif
        default:
            break;
    }
    return ERRCODE_SUCC;
}

static void cmd_shell_proc(uint8_t *data, uint32_t data_len)
{
    diag_debug_cmd_proc(data, data_len);
    dfx_log_debug("cmd shell: %s", data);
}

static void dfx_set_log_leve(bool enable)
{
    /* 断开hso, 不上报日志 */
    if (enable == false) {
        log_set_local_log_level(LOG_LEVEL_NONE);
    }
}

static int msg_process_thread(void *data)
{
    uint8_t msg_data[DFX_MSG_MAX_SIZE + DFX_MSG_ID_LEN];
    uint32_t msg_data_size = (uint32_t)sizeof(msg_data);
    uint32_t msg_id;
    int32_t ret;

    unused(data);

    while (true) {
        (void)memset_s(msg_data, sizeof(msg_data), 0, sizeof(msg_data));

        ret = osal_msg_queue_read_copy(g_dfx_osal_queue_id, msg_data, &msg_data_size, OSAL_MSGQ_WAIT_FOREVER);
        if (ret != ERRCODE_SUCC) {
            PRINT("osal_msg_queue_read_copy, err ret[%d]", ret);
            break;
        }

        msg_id = *((uint32_t*)&msg_data[0]);

        msg_process_proc(msg_id, &msg_data[DFX_MSG_ID_LEN], DFX_MSG_MAX_SIZE);
    }

    return ERRCODE_SUCC;
}

static errcode_t thread_msg_event_init(void)
{
    errcode_t ret;
    ret = (uint32_t)osal_msg_queue_create("dfx_msg", DFX_QUEUE_MAX_SIZE, &g_dfx_osal_queue_id,
        0, DFX_MSG_MAX_SIZE + DFX_MSG_ID_LEN);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    osal_task *task = osal_kthread_create(msg_process_thread, NULL, "dfx_msg", DFX_MSG_STACK_SIZE);
    if (task == NULL) {
        return ERRCODE_FAIL;
    }
    osal_kthread_lock();
    if (osal_kthread_set_priority(task, TASK_PRIORITY_DFX_MSG) != OSAL_SUCCESS) {
        print_str("osal_kthread_set_priority excute failed!!! \r\n");
    }
    osal_kthread_unlock();
    return ERRCODE_SUCC;
}

errcode_t dfx_system_init(void)
{
    errcode_t ret;
    diag_rom_api_t rom_api;

#ifdef SUPPORT_DIAG_V2_PROTOCOL
    uapi_diag_service_init();
#endif
    ret = register_default_diag_cmd();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    zdiag_filter_init();
    zdiag_filter_register_notify_hook(dfx_set_log_leve);
    dfx_set_log_leve(false);

#ifndef FORBID_AUTO_LOG_REPORT
    diag_auto_log_report_enable();
#else
#if CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART == DFX_YES
    ret = diag_beat_heart_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#endif
#endif

    ret = thread_msg_event_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = diag_register_channel();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if (CONFIG_DFX_SUPPORT_DIAG_VRTTUAL_SHELL == DFX_YES)
    zdiag_mocked_shell_init();
    zdiag_mocked_shell_register_cmd_data_proc(cmd_shell_proc);
#endif /* CONFIG_DFX_SUPPORT_DIAG_VRTTUAL_SHELL */

    rom_api.report_sys_msg = uapi_zdiag_report_sys_msg_instance;
    diag_rom_api_register(&rom_api);

#if CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES
    uapi_transmit_init();
#endif

    ret = diag_dfx_cmd_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}
