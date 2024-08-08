/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides AT message source \n
 */

#include "at_base.h"
#include "at_channel.h"
#include "at_process.h"
#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
#include "at_notify.h"
#endif
#include "at_msg.h"

static unsigned long g_at_msg_queue;
#define AT_NO_WAIT 0x0
#define AT_WAIT_FOREVER 0xFFFFFFFF

void at_msg_process(at_msg_block_t *msg)
{
    switch (msg->type) {
        case AT_CMD_MSG:
            at_proc_cmd_handle(msg->sub_msg.cmd.channel_id);
            break;
#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
        case AT_CMD_RESULT_MSG:
            at_proc_cmd_result_handle(msg->sub_msg.result.err_code);
            break;
        case AT_CMD_TIMEOUT_MSG:
            at_proc_timeout_handle();
            break;
        case AT_CMD_INTERACTIVITY_MSG:
            at_proc_interactivity_handle(msg->sub_msg.cmd.channel_id);
            break;
#endif
#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
        case AT_CMD_URC_REPORT_MSG:
            at_proc_cmd_urc_handle();
            break;
#endif
        default:
            break;
    }
}

errcode_t at_msg_send(at_msg_block_t *msg)
{
    if (at_msg_queue_write(g_at_msg_queue, msg, sizeof(at_msg_block_t), AT_NO_WAIT) != 0) {
        return ERRCODE_AT_MSG_SEND_ERROR;
    }
    return ERRCODE_SUCC;
}

#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
errcode_t uapi_at_send_async_result(uint16_t err)
{
    at_msg_block_t msg;
    msg.type = AT_CMD_RESULT_MSG;
    msg.sub_msg.result.err_code = err;
 
    return at_msg_send(&msg);
}
#endif

void uapi_at_msg_main(void *unused)
{
    unused(unused);
    at_msg_block_t msg;
    uint32_t buffer_size = sizeof(at_msg_block_t);

    if (at_base_is_at_init() != true) {
        return;
    }

    at_msg_queue_create(AT_MSG_MAX_NUM, sizeof(at_msg_block_t), &g_at_msg_queue);
#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
    at_notify_init();
#endif
    at_channel_check_and_enable();

    for (;;) {
        if (at_msg_queue_read(g_at_msg_queue, &msg, &buffer_size, AT_WAIT_FOREVER) == 0) {
            at_msg_process(&msg);
        }
        (void)at_yield();
#ifdef UT_TEST
        break;
#endif
    }
}
